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

/**
    Represents an individual BLOCKS device.

    @tags{Blocks}
*/
class Block   : public juce::ReferenceCountedObject
{
public:
    //==============================================================================
    /** Destructor. */
    ~Block() override;

    /** The different block types.
        @see Block::getType()
    */
    enum Type
    {
        unknown = 0,           /**< Unknown block type.           */
        lightPadBlock,         /**< Lightpad block type.          */
        liveBlock,             /**< Live control block type.      */
        loopBlock,             /**< Loop control block type.      */
        developerControlBlock, /**< Developer control block type. */
        touchBlock,            /**< Touch control block type.     */
        seaboardBlock,         /**< Seaboard block type.          */
        lumiKeysBlock          /**< LUMI Keys block type          */
    };

    /** Determines whether set program will be a temporary or default script saved to flash
    */
    enum class ProgramPersistency { setAsTemp, setAsDefault };

    /** The Block class is reference-counted, so always use a Block::Ptr when
        you are keeping references to them.
    */
    using Ptr = juce::ReferenceCountedObjectPtr<Block>;

    /** The Block class is reference-counted, so Block::Array is useful when
        you are storing lists of them.
    */
    using Array = juce::ReferenceCountedArray<Block>;

    /** The Block's serial number. */
    const juce::String serialNumber;

    /** The Block's version number */
    juce::String versionNumber;

    /** The Block's name */
    juce::String name;

    /** This type is used for the unique block identifier. */
    using UID = juce::uint64;

    /** This Block's UID.
        This will be globally unique, and remains constant for a particular device.
    */
    const UID uid;

    ProgramPersistency programPersistency { ProgramPersistency::setAsTemp };

    //==============================================================================
    /** Two blocks are considered equal if they have the same UID. */
    bool operator== (const Block& other) const noexcept     { return uid == other.uid; }
    /** Two blocks are considered equal if they have the same UID. */
    bool operator!= (const Block& other) const noexcept     { return uid != other.uid; }

    //==============================================================================
    /** Returns the type of this device.
        @see Block::Type
    */
    virtual Type getType() const = 0;

    /** Returns true if this a control block. **/
    bool isControlBlock() const;

    /** Returns true if Block::Type is a control block. */
    static bool isControlBlock (Block::Type);

    /** Returns a human-readable description of this device type. */
    virtual juce::String getDeviceDescription() const = 0;

    /** Returns the battery level in the range 0.0 to 1.0. */
    virtual float getBatteryLevel() const = 0;

    /** Returns true if the battery is charging. */
    virtual bool isBatteryCharging() const = 0;

    //==============================================================================
    /** Returns true if this block is connected and active. */
    virtual bool isConnected() const = 0;

    /** Returns the time this block object was connected to the topology.
        Only valid when isConnected == true.

        @see Block::isConnected
     */
    virtual juce::Time getConnectionTime() const = 0;

    /** Returns true if this block or the master block this block is connected to,
        is connected via bluetooth.

        Only valid when isConnected == true.

        @see Block::isConnected, Block::isMasterBlock
     */
    virtual bool isConnectedViaBluetooth() const = 0;

    /** Returns true if this block is directly connected to the application,
        as opposed to only being connected to a different block via a connection port.

        @see Block::ConnectionPort
    */
    virtual bool isMasterBlock() const = 0;

    /** Returns the UID of the master block this block is connected to. */
    virtual UID getConnectedMasterUID() const = 0;

    //==============================================================================
    /** Returns the width of the device in logical device units. */
    virtual int getWidth() const = 0;

    /** Returns the height of the device in logical device units. */
    virtual int getHeight() const = 0;

    /** Returns true if the device is a physical hardware block (i.e. not a virtual block). */
    virtual bool isHardwareBlock() const = 0;

    /** Returns the length of one logical device unit as physical millimeters. */
    virtual float getMillimetersPerUnit() const = 0;

    /** A simple struct representing the area of a block. */
    struct BlockArea
    {
        int x, y, width, height;
    };

    /** Returns the area that this block covers within the layout of the group as a whole.
        The coordinates are in logical block units, and are relative to the origin, which is the master block's top-left corner.
     */
    virtual BlockArea getBlockAreaWithinLayout() const = 0;

    /** Returns the rotation of this block relative to the master block in 90 degree steps clockwise. */
    virtual int getRotation() const = 0;

    //==============================================================================
    /** If this block has a grid of LEDs, this will return an object to control it.
        Note that the pointer that is returned belongs to this object, and the caller must
        neither delete it or use it after the lifetime of this Block object has finished.
        If there are no LEDs, then this method will return nullptr.
    */
    virtual LEDGrid* getLEDGrid() const = 0;

    /** If this block has a row of LEDs, this will return an object to control it.
        Note that the pointer that is returned belongs to this object, and the caller must
        neither delete it or use it after the lifetime of this Block object has finished.
        If there are no LEDs, then this method will return nullptr.
    */
    virtual LEDRow* getLEDRow() = 0;

    /** If this block has any status LEDs, this will return an array of objects to control them.
        Note that the objects in the array belong to this Block object, and the caller must
        neither delete them or use them after the lifetime of this Block object has finished.
    */
    virtual juce::Array<StatusLight*> getStatusLights() const = 0;

    /** If this block has a pressure-sensitive surface, this will return an object to
        access its data.
        Note that the pointer returned does is owned by this object, and the caller must
        neither delete it or use it after the lifetime of this Block object has finished.
        If the device is not touch-sensitive, then this method will return nullptr.
    */
    virtual TouchSurface* getTouchSurface() const = 0;

    /** If this block has any control buttons, this will return an array of objects to control them.
        Note that the objects in the array belong to this Block object, and the caller must
        neither delete them or use them after the lifetime of this Block object has finished.
    */
    virtual juce::Array<ControlButton*> getButtons() const = 0;

    //==============================================================================
    /** This returns true if the block supports generating graphics by drawing into a JUCE
        Graphics context. This should only be true for virtual on-screen blocks; hardware
        blocks will instead use the LED Grid for visuals.
    */
    virtual bool supportsGraphics() const = 0;

    //==============================================================================
    /** These are the edge-connectors that a device may have. */
    struct ConnectionPort
    {
        enum class DeviceEdge
        {
            north,
            south,
            east,
            west
        };

        /** The side of the device on which this port is located. */
        DeviceEdge edge;

        /** The index of this port along the device edge.
            For north and south edges, index 0 is the left-most port.
            For east and west edges, index 0 is the top-most port.
        */
        int index;

        bool operator== (const ConnectionPort&) const noexcept;
        bool operator!= (const ConnectionPort&) const noexcept;
    };

    /** Returns a list of the connectors that this device has. */
    virtual juce::Array<ConnectionPort> getPorts() const = 0;

    //==============================================================================
    /** A program that can be loaded onto a block. */
    struct Program
    {
        Program (Block&);
        virtual ~Program() = default;

        /** Returns the LittleFoot program to execute on the BLOCKS device. */
        virtual juce::String getLittleFootProgram() = 0;

        /** Returns an array of search paths to use when resolving includes. **/
        virtual juce::Array<juce::File> getSearchPaths() { return {}; }

        Block& block;
    };

    /** Sets the Program to run on this block.

        The supplied Program's lifetime will be managed by this class, so do not
        use the Program in other places in your code.

        Optional parameter to determine if program is set temporarily or saved
        to flash as the default prgram./
    */
    virtual juce::Result setProgram (std::unique_ptr<Program>,
                                     ProgramPersistency persistency = ProgramPersistency::setAsTemp) = 0;

    /** Returns a pointer to the currently loaded program. */
    virtual Program* getProgram() const = 0;

    /** Listener interface to be informed of program loaded events*/
    struct ProgramLoadedListener
    {
        virtual ~ProgramLoadedListener() = default;

        /** Called whenever a program has been loaded. */
        virtual void handleProgramLoaded (Block&) = 0;
    };

    /** Adds a new listener for program load completions. */
    void addProgramLoadedListener (ProgramLoadedListener*);

    /** Removes a listener for program load completions. */
    void removeProgramLoadedListener (ProgramLoadedListener*);

    //==============================================================================
    /** A message that can be sent to the currently loaded program. */
    struct ProgramEventMessage
    {
        juce::int32 values[3];
    };

    /** Sends a message to the currently loaded program.

        To receive the message the program must provide a littlefoot function called
        handleMessage with the following form:
        @code
        void handleMessage (int param1, int param2, int param3)
        {
            // Do something with the two integer parameters that the app has sent...
        }
        @endcode
    */
    virtual void sendProgramEvent (const ProgramEventMessage&) = 0;

    /** Interface for objects listening to custom program events. */
    struct ProgramEventListener
    {
        virtual ~ProgramEventListener() = default;

        /** Called whenever a message from a block is received. */
        virtual void handleProgramEvent (Block& source, const ProgramEventMessage&) = 0;
    };

    /** Adds a new listener for custom program events from the block. */
    void addProgramEventListener (ProgramEventListener*);

    /** Removes a listener for custom program events from the block. */
    void removeProgramEventListener (ProgramEventListener*);

    //==============================================================================
    /** Returns the overall memory of the block. */
    virtual juce::uint32 getMemorySize() = 0;

    /** Returns the size of the data block that setDataByte and other functions can write to. */
    virtual juce::uint32 getHeapMemorySize() = 0;

    /** Sets a single byte on the littlefoot heap. */
    virtual void setDataByte (size_t offset, juce::uint8 value) = 0;

    /** Sets multiple bytes on the littlefoot heap. */
    virtual void setDataBytes (size_t offset, const void* data, size_t num) = 0;

    /** Sets multiple bits on the littlefoot heap. */
    virtual void setDataBits (juce::uint32 startBit, juce::uint32 numBits, juce::uint32 value) = 0;

    /** Sets a single, 32 bit or less, value on the littlefoot heap. */
    template<typename Type>
    void setData (juce::uint32 offset, Type value)
    {
        const auto numBytes = sizeof (Type);

        for (auto byte = numBytes; --byte > 0u;)
        {
            auto v = *reinterpret_cast<unsigned*> (&value);
            v = (v >> (numBytes - byte) * 8) & 0xFF;
            setDataByte (offset + byte, juce::uint8 (v));
        }
    }

    /** Gets a byte from the littlefoot heap. */
    virtual juce::uint8 getDataByte (size_t offset) = 0;

    /** Sets the current program as the block's default state. */
    virtual void saveProgramAsDefault() = 0;

    /** Resets the loaded program to the block's default state. */
    virtual void resetProgramToDefault() = 0;

    //==============================================================================
    /** Metadata for a given config item */
    struct ConfigMetaData
    {
        static constexpr juce::int32 numOptionNames = 16;

        enum class ConfigType
        {
            integer,
            floating,
            boolean,
            colour,
            options
        };

        ConfigMetaData (juce::uint32 itemIndex)
          :  item (itemIndex)
        {}

        // Constructor to work around VS2015 bugs...
        ConfigMetaData (juce::uint32 itemIndex,
                        juce::int32 itemValue,
                        juce::Range<juce::int32> rangeToUse,
                        bool active,
                        const char* itemName,
                        ConfigType itemType,
                        const char* options[ConfigMetaData::numOptionNames],
                        const char* groupName)
          : item (itemIndex),
            value (itemValue),
            range (rangeToUse),
            isActive (active),
            name (itemName),
            type (itemType),
            group (groupName)
        {
            for (int i = 0; i < numOptionNames; ++i)
                optionNames[i] = options[i];
        }

        ConfigMetaData (const ConfigMetaData& other)
        {
            *this = other;
        }

        const ConfigMetaData& operator= (const ConfigMetaData& other)
        {
            if (this != &other)
            {
                item     = other.item;
                value    = other.value;
                range    = other.range;
                isActive = other.isActive;
                name     = other.name;
                type     = other.type;
                group    = other.group;

                for (int i = 0; i < numOptionNames; ++i)
                    optionNames[i] = other.optionNames[i];
            }

            return *this;
        }

        bool operator== (const ConfigMetaData& other) const
        {
            for (juce::int32 optionIndex = 0; optionIndex < numOptionNames; ++optionIndex)
                if (optionNames[optionIndex] != other.optionNames[optionIndex])
                    return false;

            return item     == other.item
                && value    == other.value
                && range    == other.range
                && isActive == other.isActive
                && name     == other.name
                && group    == other.group;
        }

        bool operator != (const ConfigMetaData& other) const
        {
            return ! (*this == other);
        }

        juce::uint32 item = 0;
        juce::int32 value = 0;
        juce::Range<juce::int32> range;
        bool isActive = false;
        juce::String name;
        ConfigType type = ConfigType::integer;
        juce::String optionNames[numOptionNames] = {};
        juce::String group;
    };

    /** Listener interface to be informed of block config changes */
    struct ConfigItemListener
    {
        virtual ~ConfigItemListener() = default;

        /** Called whenever a config changes. */
        virtual void handleConfigItemChanged (Block&, const ConfigMetaData&, juce::uint32 index) = 0;

        /*-* Callled following a config sync request*/
        virtual void handleConfigSyncEnded (Block&) = 0;
    };

    /** Adds a new listener for config item changes. */
    void addConfigItemListener (ConfigItemListener*);

    /** Removes a listener for config item changes. */
    void removeConfigItemListener (ConfigItemListener*);

    /** Returns the maximum number of config items available */
    virtual juce::uint32 getMaxConfigIndex() = 0;

    /** Determine if this is a valid config item index */
    virtual bool isValidUserConfigIndex (juce::uint32 item) = 0;

    /** Get local config item value */
    virtual juce::int32 getLocalConfigValue (juce::uint32 item) = 0;

    /** Set local config item value */
    virtual void setLocalConfigValue (juce::uint32 item, juce::int32 value) = 0;

    /** Set local config item range */
    virtual void setLocalConfigRange (juce::uint32 item, juce::int32 min, juce::int32 max) = 0;

    /** Set if config item is active or not */
    virtual void setLocalConfigItemActive (juce::uint32 item, bool isActive) = 0;

    /** Determine if config item is active or not */
    virtual bool isLocalConfigItemActive (juce::uint32 item) = 0;

    /** Get config item metadata */
    virtual ConfigMetaData getLocalConfigMetaData (juce::uint32 item) = 0;

    /** Request sync of factory config with block */
    virtual void requestFactoryConfigSync() = 0;

    /** Reset all items active status */
    virtual void resetConfigListActiveStatus() = 0;

    /** Perform factory reset on Block */
    virtual void factoryReset() = 0;

    /** Reset this Block */
    virtual void blockReset() = 0;

    /** Set Block name */
    virtual bool setName (const juce::String& name) = 0;

    //==============================================================================
    /** Allows the user to provide a function that will receive log messages from the block. */
    virtual void setLogger (std::function<void (const Block& block, const juce::String&)> loggingCallback) = 0;

    /** Sends a firmware update packet to a block, and waits for a reply. Returns an error code. */
    virtual bool sendFirmwareUpdatePacket (const juce::uint8* data, juce::uint8 size,
                                           std::function<void (juce::uint8, juce::uint32)> packetAckCallback) = 0;

    //==============================================================================
    /** Interface for objects listening to input data port. */
    struct DataInputPortListener
    {
        virtual ~DataInputPortListener() = default;

        /** Called whenever a message from a block is received. */
        virtual void handleIncomingDataPortMessage (Block& source, const void* messageData, size_t messageSize) = 0;
    };

    /** Adds a new listener for the data input port. */
    virtual void addDataInputPortListener (DataInputPortListener*);

    /** Removes a listener for the data input port. */
    virtual void removeDataInputPortListener (DataInputPortListener*);

    /** Sends a message to the block. */
    virtual void sendMessage (const void* messageData, size_t messageSize) = 0;

    //==============================================================================
    /** This type is used for timestamping events. It represents a number of milliseconds since the block
        device was booted.
    */
    using Timestamp = juce::uint32;

protected:
    //==============================================================================
    Block (const juce::String& serialNumberToUse);
    Block (const juce::String& serial, const juce::String& version, const juce::String& name);

    juce::ListenerList<ProgramLoadedListener> programLoadedListeners;
    juce::ListenerList<ProgramEventListener>  programEventListeners;
    juce::ListenerList<ConfigItemListener>    configItemListeners;
    juce::ListenerList<DataInputPortListener> dataInputPortListeners;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Block)
};

} // namespace juce
