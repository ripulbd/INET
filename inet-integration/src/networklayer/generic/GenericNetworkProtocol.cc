//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include "IPSocket.h"
#include "GenericNetworkProtocol.h"
#include "GenericDatagram.h"
#include "GenericNetworkProtocolControlInfo.h"
#include "GenericRoute.h"
#include "GenericRoutingTable.h"
#include "GenericNetworkProtocolInterfaceData.h"

Define_Module(GenericNetworkProtocol);

void GenericNetworkProtocol::initialize()
{
    QueueBase::initialize();

    interfaceTable = InterfaceTableAccess().get();
    routingTable = check_and_cast<GenericRoutingTable *>(getModuleByPath(par("routingTableModule")));

    queueOutGate = gate("queueOut");
    defaultHopLimit = par("hopLimit");
    numLocalDeliver = numDropped = numUnroutable = numForwarded = 0;

    WATCH(numLocalDeliver);
    WATCH(numDropped);
    WATCH(numUnroutable);
    WATCH(numForwarded);
}

void GenericNetworkProtocol::updateDisplayString()
{
    char buf[80] = "";
    if (numForwarded>0) sprintf(buf+strlen(buf), "fwd:%d ", numForwarded);
    if (numLocalDeliver>0) sprintf(buf+strlen(buf), "up:%d ", numLocalDeliver);
    if (numDropped>0) sprintf(buf+strlen(buf), "DROP:%d ", numDropped);
    if (numUnroutable>0) sprintf(buf+strlen(buf), "UNROUTABLE:%d ", numUnroutable);
    getDisplayString().setTagArg("t",0,buf);
}

void GenericNetworkProtocol::handleMessage(cMessage *msg)
{
    if (dynamic_cast<RegisterTransportProtocolCommand*>(msg)) {
        RegisterTransportProtocolCommand * command = check_and_cast<RegisterTransportProtocolCommand *>(msg);
        mapping.addProtocolMapping(command->getProtocol(), msg->getArrivalGate()->getIndex());
        delete msg;
    }
    else
        QueueBase::handleMessage(msg);
}

void GenericNetworkProtocol::endService(cPacket *pk)
{
    if (pk->getArrivalGate()->isName("transportIn"))
        handleMessageFromHL(pk);
    else {
        GenericDatagram *dgram = check_and_cast<GenericDatagram *>(pk);
        handlePacketFromNetwork(dgram);
    }

    if (ev.isGUI())
        updateDisplayString();
}

const InterfaceEntry *GenericNetworkProtocol::getSourceInterfaceFrom(cPacket *msg)
{
    cGate *g = msg->getArrivalGate();
    return g ? interfaceTable->getInterfaceByNetworkLayerGateIndex(g->getIndex()) : NULL;
}

void GenericNetworkProtocol::handlePacketFromNetwork(GenericDatagram *datagram)
{
    if (datagram->hasBitError()) {
        //TODO discard
    }

    delete datagram->removeControlInfo();

    // hop counter decrement; FIXME but not if it will be locally delivered
    datagram->setHopLimit(datagram->getHopLimit()-1);

    Address nextHop;
    const InterfaceEntry *inIE = getSourceInterfaceFrom(datagram);
    const InterfaceEntry *destIE = NULL;
    if (datagramPreRoutingHook(datagram, inIE, destIE, nextHop) != IHook::ACCEPT)
        return;

    datagramPreRouting(datagram, inIE, destIE, nextHop);
}

void GenericNetworkProtocol::handleMessageFromHL(cPacket *msg)
{
    // if no interface exists, do not send datagram
    if (interfaceTable->getNumInterfaces() == 0)
    {
        EV << "No interfaces exist, dropping packet\n";
        delete msg;
        return;
    }

    // encapsulate and send
    const InterfaceEntry *destIE; // will be filled in by encapsulate()
    GenericDatagram *datagram = encapsulate(msg, destIE);

    Address nextHop;
    if (datagramLocalOutHook(datagram, destIE, nextHop) != IHook::ACCEPT)
        return;

    datagramLocalOut(datagram, destIE, nextHop);
}

void GenericNetworkProtocol::routePacket(GenericDatagram *datagram, const InterfaceEntry *destIE, const Address & requestedNextHop, bool fromHL)
{
    // TBD add option handling code here

    Address destAddr = datagram->getDestinationAddress();

    EV << "Routing datagram `" << datagram->getName() << "' with dest=" << destAddr << ": ";

    // check for local delivery
    if (routingTable->isLocalAddress(destAddr))
    {
        EV << "local delivery\n";
        if (datagram->getSourceAddress().isUnspecified())
            datagram->setSourceAddress(destAddr); // allows two apps on the same host to communicate
        numLocalDeliver++;

        if (datagramLocalInHook(datagram, getSourceInterfaceFrom(datagram)) != INetfilter::IHook::ACCEPT)
            return;

        sendDatagramToHL(datagram);
        return;
    }

    // if datagram arrived from input gate and Generic_FORWARD is off, delete datagram
    if (!fromHL && !routingTable->isForwardingEnabled())
    {
        EV << "forwarding off, dropping packet\n";
        numDropped++;
        delete datagram;
        return;
    }

    // if output port was explicitly requested, use that, otherwise use GenericNetworkProtocol routing
    // TODO: see IPv4, using destIE here leaves nextHope unspecified
    Address nextHop;
    if (destIE && !requestedNextHop.isUnspecified())
    {
        EV << "using manually specified output interface " << destIE->getName() << "\n";
        nextHop = requestedNextHop;
    }
    else
    {
        // use GenericNetworkProtocol routing (lookup in routing table)
        const GenericRoute *re = routingTable->findBestMatchingRoute(destAddr);

        // error handling: destination address does not exist in routing table:
        // throw packet away and continue
        if (re==NULL)
        {
            EV << "unroutable, discarding packet\n";
            numUnroutable++;
            delete datagram;
            return;
        }

        // extract interface and next-hop address from routing table entry
        destIE = re->getInterface();
        nextHop = re->getNextHopAsGeneric();
    }

    // set datagram source address if not yet set
    if (datagram->getSourceAddress().isUnspecified())
        datagram->setSourceAddress(destIE->getGenericNetworkProtocolData()->getAddress());

    // default: send datagram to fragmentation
    EV << "output interface is " << destIE->getName() << ", next-hop address: " << nextHop << "\n";
    numForwarded++;

    sendDatagramToOutput(datagram, destIE, nextHop);
}

void GenericNetworkProtocol::routeMulticastPacket(GenericDatagram *datagram, const InterfaceEntry *destIE, const InterfaceEntry *fromIE)
{
    Address destAddr = datagram->getDestinationAddress();
    // if received from the network...
    if (fromIE!=NULL)
    {
        // check for local delivery
        if (routingTable->isLocalMulticastAddress(destAddr))
            sendDatagramToHL(datagram);
//
//        // don't forward if GenericNetworkProtocol forwarding is off
//        if (!rt->isGenericForwardingEnabled())
//        {
//            delete datagram;
//            return;
//        }
//
//        // don't forward if dest address is link-scope
//        if (destAddr.isLinkLocalMulticast())
//        {
//            delete datagram;
//            return;
//        }
    }
    else {
        //TODO
        for (int i=0; i<interfaceTable->getNumInterfaces(); ++i) {
            const InterfaceEntry * destIE = interfaceTable->getInterface(i);
            if (!destIE->isLoopback())
                sendDatagramToOutput(datagram->dup(), destIE, datagram->getDestinationAddress());
        }
        delete datagram;
    }

//    Address destAddr = datagram->getDestinationAddress();
//    EV << "Routing multicast datagram `" << datagram->getName() << "' with dest=" << destAddr << "\n";
//
//    numMulticast++;
//
//    // DVMRP: process datagram only if sent locally or arrived on the shortest
//    // route (provided routing table already contains srcAddr); otherwise
//    // discard and continue.
//    const InterfaceEntry *shortestPathIE = rt->getInterfaceForDestinationAddr(datagram->getSourceAddress());
//    if (fromIE!=NULL && shortestPathIE!=NULL && fromIE!=shortestPathIE)
//    {
//        // FIXME count dropped
//        EV << "Packet dropped.\n";
//        delete datagram;
//        return;
//    }
//
//    // if received from the network...
//    if (fromIE!=NULL)
//    {
//        // check for local delivery
//        if (rt->isLocalMulticastAddress(destAddr))
//        {
//            GenericDatagram *datagramCopy = (GenericDatagram *) datagram->dup();
//
//            // FIXME code from the MPLS model: set packet dest address to routerId (???)
//            datagramCopy->setDestinationAddress(rt->getRouterId());
//
//            reassembleAndDeliver(datagramCopy);
//        }
//
//        // don't forward if GenericNetworkProtocol forwarding is off
//        if (!rt->isGenericForwardingEnabled())
//        {
//            delete datagram;
//            return;
//        }
//
//        // don't forward if dest address is link-scope
//        if (destAddr.isLinkLocalMulticast())
//        {
//            delete datagram;
//            return;
//        }
//
//    }
//
//    // routed explicitly via Generic_MULTICAST_IF
//    if (destIE!=NULL)
//    {
//        ASSERT(datagram->getDestinationAddress().isMulticast());
//
//        EV << "multicast packet explicitly routed via output interface " << destIE->getName() << endl;
//
//        // set datagram source address if not yet set
//        if (datagram->getSourceAddress().isUnspecified())
//            datagram->setSourceAddress(destIE->ipv4Data()->getGenericAddress());
//
//        // send
//        sendDatagramToOutput(datagram, destIE, datagram->getDestinationAddress());
//
//        return;
//    }
//
//    // now: routing
//    MulticastRoutes routes = rt->getMulticastRoutesFor(destAddr);
//    if (routes.size()==0)
//    {
//        // no destination: delete datagram
//        delete datagram;
//    }
//    else
//    {
//        // copy original datagram for multiple destinations
//        for (unsigned int i=0; i<routes.size(); i++)
//        {
//            const InterfaceEntry *destIE = routes[i].interf;
//
//            // don't forward to input port
//            if (destIE && destIE!=fromIE)
//            {
//                GenericDatagram *datagramCopy = (GenericDatagram *) datagram->dup();
//
//                // set datagram source address if not yet set
//                if (datagramCopy->getSourceAddress().isUnspecified())
//                    datagramCopy->setSourceAddress(destIE->ipv4Data()->getGenericAddress());
//
//                // send
//                Address nextHop = routes[i].gateway;
//                sendDatagramToOutput(datagramCopy, destIE, nextHop);
//            }
//        }
//
//        // only copies sent, delete original datagram
//        delete datagram;
//    }
}

cPacket *GenericNetworkProtocol::decapsulate(GenericDatagram *datagram)
{
    // decapsulate transport packet
    const InterfaceEntry *fromIE = getSourceInterfaceFrom(datagram);
    cPacket *packet = datagram->decapsulate();

    // create and fill in control info
    GenericNetworkProtocolControlInfo *controlInfo = new GenericNetworkProtocolControlInfo();
    controlInfo->setProtocol(datagram->getTransportProtocol());
    controlInfo->setSourceAddress(datagram->getSourceAddress());
    controlInfo->setDestinationAddress(datagram->getDestinationAddress());
    controlInfo->setInterfaceId(fromIE ? fromIE->getInterfaceId() : -1);
    controlInfo->setHopLimit(datagram->getHopLimit());

    // attach control info
    packet->setControlInfo(controlInfo);

    return packet;
}

GenericDatagram *GenericNetworkProtocol::encapsulate(cPacket *transportPacket, const InterfaceEntry *&destIE)
{
    GenericNetworkProtocolControlInfo *controlInfo = check_and_cast<GenericNetworkProtocolControlInfo*>(transportPacket->removeControlInfo());
    GenericDatagram *datagram = new GenericDatagram(transportPacket->getName());
//    datagram->setByteLength(HEADER_BYTES); //TODO parameter
    datagram->encapsulate(transportPacket);

    // set source and destination address
    Address dest = controlInfo->getDestinationAddress();
    datagram->setDestinationAddress(dest);

    // Generic_MULTICAST_IF option, but allow interface selection for unicast packets as well
    destIE = interfaceTable->getInterfaceById(controlInfo->getInterfaceId());

    Address src = controlInfo->getSourceAddress();

    // when source address was given, use it; otherwise it'll get the address
    // of the outgoing interface after routing
    if (!src.isUnspecified())
    {
        // if interface parameter does not match existing interface, do not send datagram
        if (routingTable->getInterfaceByAddress(src)==NULL)
            opp_error("Wrong source address %s in (%s)%s: no interface with such address",
                      src.str().c_str(), transportPacket->getClassName(), transportPacket->getFullName());
        datagram->setSourceAddress(src);
    }

    // set other fields
    short ttl;
    if (controlInfo->getHopLimit() > 0)
        ttl = controlInfo->getHopLimit();
    else if (false) //TODO: datagram->getDestinationAddress().isLinkLocalMulticast())
        ttl = 1;
    else
        ttl = defaultHopLimit;

    datagram->setHopLimit(ttl);
    datagram->setTransportProtocol(controlInfo->getProtocol());

    // setting GenericNetworkProtocol options is currently not supported

    delete controlInfo;
    return datagram;
}

void GenericNetworkProtocol::sendDatagramToHL(GenericDatagram *datagram)
{
    int protocol = datagram->getTransportProtocol();
    int gateIndex = mapping.findOutputGateForProtocol(protocol);
    // check if the transportOut port are connected, otherwise discard the packet
    if (gateIndex >= 0)
    {
        cGate* outGate = gate("transportOut", gateIndex);
        if (outGate->isPathOK())
        {
            // decapsulate and send on appropriate output gate
            cPacket *packet = decapsulate(datagram);
            delete datagram;
            send(packet, "transportOut", gateIndex);
            return;
        }
    }

    //TODO send an ICMP error: protocol unreachable
    delete datagram;
}

void GenericNetworkProtocol::sendDatagramToOutput(GenericDatagram *datagram, const InterfaceEntry *ie, const Address & nextHop)
{
    if (datagram->getByteLength() > ie->getMTU())
        error("datagram too large"); //TODO refine

    // hop counter check
    if (datagram->getHopLimit() <= 0) {
        EV << "datagram hopLimit reached zero, discarding\n";
        delete datagram;  //TODO stats counter???
        return;
    }

    // send out datagram to ARP, with control info attached
    GenericRoutingDecision *routingDecision = new GenericRoutingDecision();
    routingDecision->setInterfaceId(ie->getInterfaceId());
    routingDecision->setNextHop(nextHop);
    datagram->setControlInfo(routingDecision);

    send(datagram, queueOutGate);
}

void GenericNetworkProtocol::datagramPreRouting(GenericDatagram* datagram, const InterfaceEntry * inIE, const InterfaceEntry * destIE, const Address & nextHop)
{
    // route packet
    if (!datagram->getDestinationAddress().isMulticast())
        routePacket(datagram, destIE, nextHop, false);
    else
        routeMulticastPacket(datagram, destIE, inIE);
}

void GenericNetworkProtocol::datagramLocalIn(GenericDatagram* datagram, const InterfaceEntry * inIE)
{
    sendDatagramToHL(datagram);
}

void GenericNetworkProtocol::datagramLocalOut(GenericDatagram* datagram, const InterfaceEntry * destIE, const Address & nextHop)
{
    // route packet
    if (!datagram->getDestinationAddress().isMulticast())
        routePacket(datagram, destIE, nextHop, true);
    else
        routeMulticastPacket(datagram, destIE, NULL);
}

void GenericNetworkProtocol::registerHook(int priority, IHook* hook)
{
    Enter_Method("registerHook()");
    hooks.insert(std::pair<int, IHook*>(priority, hook));
}

void GenericNetworkProtocol::unregisterHook(int priority, IHook* hook)
{
    Enter_Method("unregisterHook()");
    for (std::multimap<int, IHook*>::iterator iter = hooks.begin(); iter != hooks.end(); iter++) {
        if ((iter->first == priority) && (iter->second == hook)) {
            hooks.erase(iter);
            return;
        }
    }
}

void GenericNetworkProtocol::dropQueuedDatagram(const INetworkDatagram * datagram)
{
    Enter_Method("dropDatagram()");
    for (std::list<QueuedDatagramForHook>::iterator iter = queuedDatagramsForHooks.begin(); iter != queuedDatagramsForHooks.end(); iter++) {
        if (iter->datagram == datagram) {
            delete datagram;
            queuedDatagramsForHooks.erase(iter);
            return;
        }
    }
}

void GenericNetworkProtocol::reinjectQueuedDatagram(const INetworkDatagram* datagram)
{

    Enter_Method("reinjectDatagram()");
    for (std::list<QueuedDatagramForHook>::iterator iter = queuedDatagramsForHooks.begin(); iter != queuedDatagramsForHooks.end(); iter++) {
        if (iter->datagram == datagram) {
            GenericDatagram* datagram = iter->datagram;
            const InterfaceEntry * inIE = iter->inIE;
            const InterfaceEntry * outIE = iter->outIE;
            const Address & nextHop = iter->nextHop;
            INetfilter::IHook::Type hookType = iter->hookType;
            switch (hookType) {
                case INetfilter::IHook::PREROUTING:
                    datagramPreRouting(datagram, inIE, outIE, nextHop);
                    break;
                case INetfilter::IHook::LOCALIN:
                    datagramLocalIn(datagram, inIE);
                    break;
                case INetfilter::IHook::LOCALOUT:
                    datagramLocalOut(datagram, outIE, nextHop);
                    break;
                default:
                    error("Re-injection of datagram queued for this hook not implemented");
                    break;
            }
            queuedDatagramsForHooks.erase(iter);
            return;
        }
    }
}

INetfilter::IHook::Result GenericNetworkProtocol::datagramPreRoutingHook(GenericDatagram* datagram, const InterfaceEntry * inIE, const InterfaceEntry *& outIE, Address & nextHop)
{
    for (std::multimap<int, IHook*>::iterator iter = hooks.begin(); iter != hooks.end(); iter++) {
        IHook::Result r = iter->second->datagramPreRoutingHook(datagram, inIE, outIE, nextHop);
        switch(r)
        {
            case IHook::ACCEPT: break;   // continue iteration
            case IHook::DROP:   delete datagram; return r;
            case IHook::QUEUE:  queuedDatagramsForHooks.push_back(QueuedDatagramForHook(datagram, inIE, NULL, nextHop, INetfilter::IHook::PREROUTING)); return r;
            case IHook::STOLEN: return r;
            default: throw cRuntimeError("Unknown Hook::Result value: %d", (int)r);
        }
    }
    return IHook::ACCEPT;
}

INetfilter::IHook::Result GenericNetworkProtocol::datagramForwardHook(GenericDatagram* datagram, const InterfaceEntry * inIE, const InterfaceEntry *& outIE, Address& nextHop)
{
    for (std::multimap<int, IHook*>::iterator iter = hooks.begin(); iter != hooks.end(); iter++) {
        IHook::Result r = iter->second->datagramForwardHook(datagram, inIE, outIE, nextHop);
        switch(r)
        {
            case IHook::ACCEPT: break;   // continue iteration
            case IHook::DROP:   delete datagram; return r;
            case IHook::QUEUE:  queuedDatagramsForHooks.push_back(QueuedDatagramForHook(datagram, inIE, outIE, nextHop, INetfilter::IHook::FORWARD)); return r;
            case IHook::STOLEN: return r;
            default: throw cRuntimeError("Unknown Hook::Result value: %d", (int)r);
        }
    }
    return IHook::ACCEPT;
}

INetfilter::IHook::Result GenericNetworkProtocol::datagramPostRoutingHook(GenericDatagram* datagram, const InterfaceEntry * inIE, const InterfaceEntry *& outIE, Address& nextHop)
{
    for (std::multimap<int, IHook*>::iterator iter = hooks.begin(); iter != hooks.end(); iter++) {
        IHook::Result r = iter->second->datagramPostRoutingHook(datagram, inIE, outIE, nextHop);
        switch(r)
        {
            case IHook::ACCEPT: break;   // continue iteration
            case IHook::DROP:   delete datagram; return r;
            case IHook::QUEUE:  queuedDatagramsForHooks.push_back(QueuedDatagramForHook(datagram, inIE, outIE, nextHop, INetfilter::IHook::POSTROUTING)); return r;
            case IHook::STOLEN: return r;
            default: throw cRuntimeError("Unknown Hook::Result value: %d", (int)r);
        }
    }
    return IHook::ACCEPT;
}

INetfilter::IHook::Result GenericNetworkProtocol::datagramLocalInHook(GenericDatagram* datagram, const InterfaceEntry * inIE)
{
    Address address;
    for (std::multimap<int, IHook*>::iterator iter = hooks.begin(); iter != hooks.end(); iter++) {
        IHook::Result r = iter->second->datagramLocalInHook(datagram, inIE);
        switch(r)
        {
            case IHook::ACCEPT: break;   // continue iteration
            case IHook::DROP:   delete datagram; return r;
            case IHook::QUEUE:  queuedDatagramsForHooks.push_back(QueuedDatagramForHook(datagram, inIE, NULL, address, INetfilter::IHook::LOCALIN)); return r;
            case IHook::STOLEN: return r;
            default: throw cRuntimeError("Unknown Hook::Result value: %d", (int)r);
        }
    }
    return IHook::ACCEPT;
}

INetfilter::IHook::Result GenericNetworkProtocol::datagramLocalOutHook(GenericDatagram* datagram, const InterfaceEntry *& outIE, Address & nextHop)
{
    for (std::multimap<int, IHook*>::iterator iter = hooks.begin(); iter != hooks.end(); iter++) {
        IHook::Result r = iter->second->datagramLocalOutHook(datagram, outIE, nextHop);
        switch(r)
        {
            case IHook::ACCEPT: break;   // continue iteration
            case IHook::DROP:   delete datagram; return r;
            case IHook::QUEUE:  queuedDatagramsForHooks.push_back(QueuedDatagramForHook(datagram, NULL, outIE, nextHop, INetfilter::IHook::LOCALOUT)); return r;
            case IHook::STOLEN: return r;
            default: throw cRuntimeError("Unknown Hook::Result value: %d", (int)r);
        }
    }
    return IHook::ACCEPT;
}
