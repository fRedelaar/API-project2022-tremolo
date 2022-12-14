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

//==============================================================================
/** These can be useful when debugging the topology. */

#ifndef LOG_BLOCKS_CONNECTIVITY
  #define LOG_BLOCKS_CONNECTIVITY 0
#endif

#ifndef LOG_BLOCKS_PINGS
  #define LOG_BLOCKS_PINGS 0
#endif

#ifndef DUMP_BANDWIDTH_STATS
  #define DUMP_BANDWIDTH_STATS 0
#endif

#ifndef DUMP_TOPOLOGY
  #define DUMP_TOPOLOGY 0
#endif

#define TOPOLOGY_LOG(text) \
 JUCE_BLOCK_WITH_FORCED_SEMICOLON (juce::String buf ("Topology Src:   "); \
 juce::Logger::outputDebugString (buf << text);)

#if LOG_BLOCKS_CONNECTIVITY
 #define LOG_CONNECTIVITY(text) TOPOLOGY_LOG(text)
#else
 #define LOG_CONNECTIVITY(text)
#endif

#if LOG_BLOCKS_PINGS
 #define LOG_PING(text) TOPOLOGY_LOG(text)
#else
 #define LOG_PING(text)
#endif

#if DUMP_BANDWIDTH_STATS
 #include "internal/roli_BandwidthStatsLogger.cpp"
#endif

#include "internal/roli_MidiDeviceConnection.cpp"
#include "internal/roli_MIDIDeviceDetector.cpp"
#include "internal/roli_DeviceInfo.cpp"
#include "internal/roli_DepreciatedVersionReader.cpp"
#include "internal/roli_BlockSerialReader.cpp"
#include "internal/roli_ConnectedDeviceGroup.cpp"
#include "internal/roli_BlockImplementation.cpp"
#include "internal/roli_Detector.cpp"
#include "internal/roli_DetectorHolder.cpp"

namespace roli
{

//==============================================================================
PhysicalTopologySource::PhysicalTopologySource (bool startDetached)
{
    if (! startDetached)
        setActive (true);
}

PhysicalTopologySource::PhysicalTopologySource (DeviceDetector& detectorToUse, bool startDetached)
    : customDetector (&detectorToUse)
{
    if (! startDetached)
        setActive (true);
}

PhysicalTopologySource::~PhysicalTopologySource()
{
    setActive (false);
}

void PhysicalTopologySource::setActive (bool shouldBeActive)
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED

    if (isActive() == shouldBeActive)
        return;

    if (shouldBeActive)
    {
        if (customDetector == nullptr)
            detector = std::make_unique<DetectorHolder>(*this);
        else
            detector = std::make_unique<DetectorHolder>(*this, *customDetector);

        detector->detector->activeTopologySources.add (this);
    }
    else
    {
        detector->detector->detach (this);
        detector.reset();
    }

    listeners.call ([] (TopologySource::Listener& l){ l.topologyChanged(); });
}

bool PhysicalTopologySource::isActive() const
{
    return detector != nullptr;
}

bool PhysicalTopologySource::isLockedFromOutside() const
{
    if (detector != nullptr && detector->detector != nullptr)
        return detector->detector->deviceDetector.isLockedFromOutside();

    return false;
}

BlockTopology PhysicalTopologySource::getCurrentTopology() const
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED // This method must only be called from the message thread!

    if (detector != nullptr)
        return detector->detector->currentTopology;

    return {};
}

void PhysicalTopologySource::cancelAllActiveTouches() noexcept
{
    if (detector != nullptr)
        detector->detector->cancelAllActiveTouches();
}

bool PhysicalTopologySource::hasOwnServiceTimer() const     { return false; }
void PhysicalTopologySource::handleTimerTick()
{
    if (detector != nullptr)
        detector->handleTimerTick();
}

PhysicalTopologySource::DeviceConnection::DeviceConnection() {}
PhysicalTopologySource::DeviceConnection::~DeviceConnection() {}

PhysicalTopologySource::DeviceDetector::DeviceDetector() {}
PhysicalTopologySource::DeviceDetector::~DeviceDetector() {}

const char* const* PhysicalTopologySource::getStandardLittleFootFunctions() noexcept
{
    return BlocksProtocol::ledProgramLittleFootFunctions;
}

template <typename ListType>
static bool collectionsMatch (const ListType& list1, const ListType& list2) noexcept
{
    if (list1.size() != list2.size())
        return false;

    for (auto&& b : list1)
        if (! list2.contains (b))
            return false;

    return true;
}

bool BlockTopology::operator== (const BlockTopology& other) const noexcept
{
    return collectionsMatch (connections, other.connections) && collectionsMatch (blocks, other.blocks);
}

bool BlockTopology::operator!= (const BlockTopology& other) const noexcept
{
    return ! operator== (other);
}

bool BlockDeviceConnection::operator== (const BlockDeviceConnection& other) const noexcept
{
    return (device1 == other.device1 && device2 == other.device2
             && connectionPortOnDevice1 == other.connectionPortOnDevice1
             && connectionPortOnDevice2 == other.connectionPortOnDevice2)
        || (device1 == other.device2 && device2 == other.device1
             && connectionPortOnDevice1 == other.connectionPortOnDevice2
             && connectionPortOnDevice2 == other.connectionPortOnDevice1);
}

bool BlockDeviceConnection::operator!= (const BlockDeviceConnection& other) const noexcept
{
    return ! operator== (other);
}

} // namespace roli
