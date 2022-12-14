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

namespace
{
    struct PortIOStats
    {
        PortIOStats (const char* nm) : name (nm) {}

        const char* const name;
        int byteCount           = 0;
        int messageCount        = 0;
        int bytesPerSec         = 0;
        int largestMessageBytes = 0;
        int lastMessageBytes    = 0;

        void update (double elapsedSec)
        {
            if (byteCount > 0)
            {
                bytesPerSec = (int) (byteCount / elapsedSec);
                byteCount = 0;
                juce::Logger::writeToLog (getString());
            }
        }

        juce::String getString() const
        {
            return juce::String (name) + ": "
            + "count="    + juce::String (messageCount).paddedRight (' ', 7)
            + "rate="     + (juce::String (bytesPerSec / 1024.0f, 1) + " Kb/sec").paddedRight (' ', 11)
            + "largest="  + (juce::String (largestMessageBytes) + " bytes").paddedRight (' ', 11)
            + "last="     + (juce::String (lastMessageBytes) + " bytes").paddedRight (' ', 11);
        }

        void registerMessage (int numBytes) noexcept
        {
            byteCount += numBytes;
            ++messageCount;
            lastMessageBytes = numBytes;
            largestMessageBytes = juce::jmax (largestMessageBytes, numBytes);
        }
    };

    static PortIOStats inputStats { "Input" }, outputStats { "Output" };
    static juce::uint32 startTime = 0;

    static void resetOnSecondBoundary()
    {
        auto now = juce::Time::getMillisecondCounter();
        double elapsedSec = (now - startTime) / 1000.0;

        if (elapsedSec >= 1.0)
        {
            inputStats.update (elapsedSec);
            outputStats.update (elapsedSec);
            startTime = now;
        }
    }

    static void registerBytesOut (int numBytes)
    {
        outputStats.registerMessage (numBytes);
        resetOnSecondBoundary();
    }

    static void registerBytesIn (int numBytes)
    {
        inputStats.registerMessage (numBytes);
        resetOnSecondBoundary();
    }
}

juce::String getMidiIOStats()
{
    return inputStats.getString() + "   " + outputStats.getString();
}

} // namespace roli
