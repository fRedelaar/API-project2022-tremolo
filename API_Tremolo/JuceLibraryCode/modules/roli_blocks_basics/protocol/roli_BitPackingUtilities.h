/*
  ==============================================================================

   Copyright (c) 2020 - ROLI Ltd

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED “AS IS” AND ROLI LTD DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL ROLI LTD BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
   OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

namespace roli
{
namespace BlocksProtocol
{

/**
    All sysex messages to or from a BLOCKS device begin with these header bytes.
    The next byte that follows indicates the device index within the topology, where
    the 0x40 bit is set for device->host messages, and clear for host->device messages.
    The lower 6 bits contain the topology index of the destination or source device.
*/
static const juce::uint8 roliSysexHeader[] = { 0xf0, 0x00, 0x21, 0x10, 0x77 };


static juce::uint8 calculatePacketChecksum (const juce::uint8* data, juce::uint32 size) noexcept
{
    juce::uint8 checksum = (juce::uint8) size;

    for (juce::uint32 i = 0; i < size; ++i)
        checksum = static_cast<juce::uint8> (checksum + (checksum * 2 + data[i]));

    return checksum & 0x7f;
}


//==============================================================================
/**
    Helper class to define an integer with a specific bit size.

    @tags{Blocks}
*/
template <int numBits>
struct IntegerWithBitSize
{
    IntegerWithBitSize() = default;
    IntegerWithBitSize (const IntegerWithBitSize&) = default;
    IntegerWithBitSize& operator= (const IntegerWithBitSize&) = default;

    IntegerWithBitSize (juce::uint32 v) noexcept : value (v)
    {
        static_assert (numBits <= 32, "numBits must be <= 32");
        jassert (v >= 0 && v <= maxValue);
    }

    enum
    {
        bits = numBits,
        maxValue = static_cast<juce::uint32> ((1ULL << numBits) - 1ULL)
    };

    operator juce::uint32() const noexcept            { return value; }
    juce::uint32 get() const noexcept                 { return value; }

    juce::uint8 getScaledToByte() const noexcept
    {
        return (juce::uint8) (numBits < 8 ? (juce::uint32) (value << (8 - numBits))
                                          : (juce::uint32) (value >> (numBits - 8)));
    }

    float toUnipolarFloat() const noexcept      { return (float) value / (float) maxValue; }
    float toBipolarFloat() const noexcept       { return (float) static_cast<juce::int32> (value << (32 - numBits)) / (float) 0x80000000u; }

    static IntegerWithBitSize fromUnipolarFloat (float value) noexcept
    {
        static_assert (numBits <= 31, "numBits must be <= 31");
        return IntegerWithBitSize ((juce::uint32) juce::jlimit (0, (int) maxValue, (int) (value * maxValue)));
    }

    static IntegerWithBitSize fromBipolarFloat (float value) noexcept
    {
        static_assert (numBits <= 31, "numBits must be <= 31");
        return IntegerWithBitSize (maxValue & (juce::uint32) juce::jlimit ((int) -(maxValue / 2), (int) (maxValue / 2), (int) (value * (maxValue / 2))));
    }

    juce::uint32 value = 0;
};

//==============================================================================
/**
    This helper class allocates a block of 7-bit bytes and can push sequences of bits into it.
    @see Packed7BitArrayReader

    @tags{Blocks}
*/
template <int allocatedBytes>
struct Packed7BitArrayBuilder
{
    const void* getData() const noexcept        { return data; }
    int size() const noexcept                   { return bytesWritten + (bitsInCurrentByte > 0 ? 1 : 0); }

    bool hasCapacity (int bitsNeeded) const noexcept
    {
        return ((bytesWritten + 2) * 7 + bitsInCurrentByte + bitsNeeded) <= allocatedBytes * 7;
    }

    void writeHeaderSysexBytes (juce::uint8 deviceIndex) noexcept
    {
        jassert (bytesWritten + bitsInCurrentByte == 0);

        for (int i = 0; i < (int) sizeof (roliSysexHeader); ++i)
            data[bytesWritten++] = roliSysexHeader[i];

        jassert (deviceIndex < 128);
        data[bytesWritten++] = deviceIndex & 0x7f;
    }

    void writePacketSysexFooter() noexcept
    {
        if (bitsInCurrentByte != 0)
        {
            bitsInCurrentByte = 0;
            ++bytesWritten;
        }

        jassert (hasCapacity (0));

        juce::uint32 headerBytes = (juce::uint32) sizeof (roliSysexHeader) + 1;
        data[bytesWritten] = calculatePacketChecksum (data + headerBytes, (juce::uint32) bytesWritten - headerBytes);
        ++bytesWritten;

        data[bytesWritten++] = 0xf7;
    }

    template <int numBits>
    Packed7BitArrayBuilder& operator<< (IntegerWithBitSize<numBits> value) noexcept
    {
        writeBits (value.value, numBits);
        return *this;
    }

    void writeBits (juce::uint32 value, int numBits) noexcept
    {
        jassert (numBits <= 32);
        jassert (hasCapacity (numBits));
        jassert (numBits == 32 || (value >> numBits) == 0);

        while (numBits > 0)
        {
            if (bitsInCurrentByte == 0)
            {
                if (numBits < 7)
                {
                    data[bytesWritten] = (juce::uint8) value;
                    bitsInCurrentByte = numBits;
                    return;
                }

                if (numBits == 7)
                {
                    data[bytesWritten++] = (juce::uint8) value;
                    return;
                }

                data[bytesWritten++] = (juce::uint8) (value & 0x7f);
                value >>= 7;
                numBits -= 7;
            }
            else
            {
                const int bitsToDo = juce::jmin (7 - bitsInCurrentByte, numBits);

                data[bytesWritten] = static_cast<juce::uint8> (data[bytesWritten] | ((value & (juce::uint32) ((1 << bitsToDo) - 1)) << bitsInCurrentByte));
                value >>= bitsToDo;
                numBits -= bitsToDo;
                bitsInCurrentByte += bitsToDo;

                if (bitsInCurrentByte == 7)
                {
                    bitsInCurrentByte = 0;
                    ++bytesWritten;
                }
            }
        }
    }

    /** Describes the current building state */
    struct State
    {
        int bytesWritten, bitsInCurrentByte;
    };

    State getState() const noexcept
    {
        return { bytesWritten, bitsInCurrentByte };
    }

    void restore (State state) noexcept
    {
        bytesWritten = state.bytesWritten;
        bitsInCurrentByte = state.bitsInCurrentByte;
    }

private:
    juce::uint8 data[(size_t) allocatedBytes];
    int bytesWritten = 0, bitsInCurrentByte = 0;
};


//==============================================================================
/**
    This helper class reads from a block of 7-bit bytes as sequences of bits.
    @see Packed7BitArrayBuilder

    @tags{Blocks}
*/
struct Packed7BitArrayReader
{
    Packed7BitArrayReader (const void* sourceData, int numBytes) noexcept
        : data (static_cast<const juce::uint8*> (sourceData)), totalBits (numBytes * 7)
    {
    }

    int getRemainingBits() const noexcept
    {
        return totalBits - bitsReadInCurrentByte;
    }

    template <typename Target>
    Target read() noexcept
    {
        return Target (readBits (Target::bits));
    }

    juce::uint32 readBits (int numBits) noexcept
    {
        jassert (numBits <= 32);
        jassert (getRemainingBits() >= numBits);

        juce::uint32 value = 0;
        int bitsSoFar = 0;

        while (numBits > 0)
        {
            const auto valueInCurrentByte = (juce::uint32) (*data >> bitsReadInCurrentByte);

            const int bitsAvailable = 7 - bitsReadInCurrentByte;

            if (bitsAvailable > numBits)
            {
                value |= ((valueInCurrentByte & (juce::uint32) ((1 << numBits) - 1)) << bitsSoFar);
                bitsReadInCurrentByte += numBits;
                break;
            }

            value |= (valueInCurrentByte << bitsSoFar);
            numBits -= bitsAvailable;
            bitsSoFar += bitsAvailable;
            bitsReadInCurrentByte = 0;
            ++data;
            totalBits -= 7;
        }

        return value;
    }

    static bool checksumIsOK (const juce::uint8* data, juce::uint32 size) noexcept
    {
        return size > 1 && calculatePacketChecksum (data, size - 1) == data[size - 1];
    }

private:
    const juce::uint8* data;
    int totalBits, bitsReadInCurrentByte = 0;
};

} // namespace BlocksProtocol
} // namespace roli
