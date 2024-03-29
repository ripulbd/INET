//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


#ifndef __IPv6_H__
#define __IPv6_H__

#include "INETDefs.h"
#include "INetfilter.h"
#include "INetworkProtocol.h"
#include "QueueBase.h"
#include "IPv6RoutingTable.h"
#include "ICMPv6.h"
#include "IPv6NeighbourDiscovery.h"

#include "IPv6Tunneling.h"

#include "IPv6Datagram.h"
#include "IPv6FragBuf.h"
#include "ProtocolMap.h"

class ICMPv6Message;

/**
 * IPv6 implementation.
 */
class INET_API IPv6 : public QueueBase, public ILifecycle, public INetfilter, public INetworkProtocol
{
  public:
    /**
    * Represents an IPv4Datagram, queued by a Hook
    */
    class QueuedDatagramForHook {
      public:
        QueuedDatagramForHook(IPv6Datagram* datagram, const InterfaceEntry* inIE, const InterfaceEntry* outIE, const IPv6Address& nextHopAddr, IHook::Type hookType) :
              datagram(datagram), inIE(inIE), outIE(outIE), nextHopAddr(nextHopAddr), hookType(hookType) {}
        virtual ~QueuedDatagramForHook() {}

        IPv6Datagram* datagram;
        const InterfaceEntry* inIE;
        const InterfaceEntry* outIE;
        IPv6Address nextHopAddr;
        const IHook::Type hookType;
    };

  protected:
    IInterfaceTable *ift;
    IPv6RoutingTable *rt;
    IPv6NeighbourDiscovery *nd;
    ICMPv6 *icmp;

    IPv6Tunneling* tunneling;

    // working vars
    unsigned int curFragmentId; // counter, used to assign unique fragmentIds to datagrams
    IPv6FragBuf fragbuf;  // fragmentation reassembly buffer
    simtime_t lastCheckTime; // when fragbuf was last checked for state fragments
    ProtocolMapping mapping; // where to send packets after decapsulation

    // statistics
    int numMulticast;
    int numLocalDeliver;
    int numDropped;
    int numUnroutable;
    int numForwarded;

#ifdef WITH_xMIPv6
    // 28.9.07 - CB
    // datagrams that are supposed to be sent with a tentative IPv6 address
    // are rescheduled for later resubmission.
    class ScheduledDatagram : public cPacket
    {
      public:
        IPv6Datagram* datagram;
        const InterfaceEntry *ie;
        MACAddress macAddr;
        bool fromHL;
    };
#endif /* WITH_xMIPv6 */

    // netfilter hook variables
    typedef std::multimap<int, IHook*> HookList;
    HookList hooks;
    typedef std::list<QueuedDatagramForHook> DatagramQueueForHooks;
    DatagramQueueForHooks queuedDatagramsForHooks;

  protected:
    // utility: look up interface from getArrivalGate()
    virtual InterfaceEntry *getSourceInterfaceFrom(cPacket *msg);

    // utility: show current statistics above the icon
    virtual void updateDisplayString();

    /**
     * Encapsulate packet coming from higher layers into IPv6Datagram
     */
    virtual IPv6Datagram *encapsulate(cPacket *transportPacket, IPv6ControlInfo *ctrlInfo);

    virtual void preroutingFinish(IPv6Datagram *datagram, const InterfaceEntry *fromIE, const InterfaceEntry *destIE, IPv6Address nextHopAddr);

    /**
     * Handle messages (typically packets to be send in IPv6) from transport or ICMP.
     * Invokes encapsulate(), then routePacket().
     */
    virtual void handleMessageFromHL(cPacket *msg);
    virtual void datagramLocalOut(IPv6Datagram* datagram, const InterfaceEntry* destIE, IPv6Address requestedNextHopAddress);

    /**
     * Handle incoming ICMP messages.
     */
    virtual void handleReceivedICMP(ICMPv6Message *msg);

    /**
     * Performs routing. Based on the routing decision, it dispatches to
     * localDeliver() for local packets, to fragmentAndSend() for forwarded packets,
     * to routeMulticastPacket() for multicast packets, or drops the packet if
     * it's unroutable or forwarding is off.
     */
    virtual void routePacket(IPv6Datagram *datagram, const InterfaceEntry *destIE, IPv6Address requestedNextHopAddress, bool fromHL);

    /**
     * Forwards packets to all multicast destinations, using fragmentAndSend().
     */
    virtual void routeMulticastPacket(IPv6Datagram *datagram, const InterfaceEntry *destIE, const InterfaceEntry *fromIE, bool fromHL);

    /**
     * Performs fragmentation if needed, and sends the original datagram or the fragments
     * through the specified interface.
     */
    virtual void fragmentAndSend(IPv6Datagram *datagram, const InterfaceEntry *destIE, const MACAddress &nextHopAddr, bool fromHL);
    /**
     * Perform reassembly of fragmented datagrams, then send them up to the
     * higher layers using sendToHL().
     */
    virtual void localDeliver(IPv6Datagram *datagram);

    /**
     * Decapsulate and return encapsulated packet after attaching IPv6ControlInfo.
     */
    virtual cPacket *decapsulate(IPv6Datagram *datagram);

    /**
     * Last hoplimit check, then send datagram on the given interface.
     */
    virtual void sendDatagramToOutput(IPv6Datagram *datagram, const InterfaceEntry *destIE, const MACAddress& macAddr);

    // NetFilter functions:
  protected:
    /**
     * called before a packet arriving from the network is routed
     */
    IHook::Result datagramPreRoutingHook(INetworkDatagram* datagram, const InterfaceEntry* inIE, const InterfaceEntry*& outIE, Address& nextHopAddr);

    /**
     * called before a packet arriving from the network is delivered via the network
     */
    IHook::Result datagramForwardHook(INetworkDatagram* datagram, const InterfaceEntry* inIE, const InterfaceEntry*& outIE, Address& nextHopAddr);

    /**
     * called before a packet is delivered via the network
     */
    IHook::Result datagramPostRoutingHook(INetworkDatagram* datagram, const InterfaceEntry* inIE, const InterfaceEntry*& outIE, Address& nextHopAddr);

    /**
     * called before a packet arriving from the network is delivered locally
     */
    IHook::Result datagramLocalInHook(INetworkDatagram* datagram, const InterfaceEntry* inIE);

    /**
     * called before a packet arriving locally is delivered
     */
    IHook::Result datagramLocalOutHook(INetworkDatagram* datagram, const InterfaceEntry*& outIE, Address& nextHopAddr);

  public:
    IPv6() {}

    // Netfilter:
    virtual void registerHook(int priority, IHook * hook);
    virtual void unregisterHook(int priority, IHook * hook);
    virtual void dropQueuedDatagram(const INetworkDatagram * daragram);
    virtual void reinjectQueuedDatagram(const INetworkDatagram * datagram);

  protected:
    /**
     * Initialization
     */
    virtual void initialize(int stage);
    virtual int numInitStages() const { return NUM_INIT_STAGES; }

    /**
     * Handle message
     */
    virtual void handleMessage(cMessage *msg);

    /**
     * Processing of IPv6 datagrams. Called when a datagram reaches the front
     * of the queue.
     */
    virtual void endService(cPacket *msg);

    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback);

    /**
     * Determines the correct interface for the specified destination address.
     * The nextHop and interfaceId are output parameter.
     */
    bool determineOutputInterface(const IPv6Address& destAddress, IPv6Address& nextHop, int& interfaceId,
            IPv6Datagram* datagram, bool fromHL);

#ifdef WITH_xMIPv6
    /**
     * Process the extension headers of the datagram.
     * Returns true if all have been processed successfully and false if errors occured
     * and the packet has to be dropped or if the datagram has been forwarded to another
     * module for further processing.
     */
    bool processExtensionHeaders(IPv6Datagram* datagram);
#endif /* WITH_xMIPv6 */
};

#endif

