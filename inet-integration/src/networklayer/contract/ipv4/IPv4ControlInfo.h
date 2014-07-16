//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_IPv4CONTROLINFO_H
#define __INET_IPv4CONTROLINFO_H

#include "INetworkProtocolControlInfo.h"
#include "IPv4ControlInfo_m.h"

class IPv4Datagram;

/**
 * Control information for sending/receiving packets over IPv4.
 *
 * See the IPv4ControlInfo.msg file for more info.
 */
class INET_API IPv4ControlInfo : public IPv4ControlInfo_Base, public INetworkProtocolControlInfo
{
  protected:
    IPv4Datagram *dgram;

  private:
    void copy(const IPv4ControlInfo& other);
    void clean();

  public:
    IPv4ControlInfo() : IPv4ControlInfo_Base() {dgram = NULL;}
    virtual ~IPv4ControlInfo();
    IPv4ControlInfo(const IPv4ControlInfo& other) : IPv4ControlInfo_Base(other) { dgram = NULL; copy(other); }
    IPv4ControlInfo& operator=(const IPv4ControlInfo& other);
    virtual IPv4ControlInfo *dup() const {return new IPv4ControlInfo(*this);}

    /**
     * Returns bits 0-5 of the Type of Service field, a value in the 0..63 range
     */
    virtual int getDiffServCodePoint() const { return getTypeOfService() & 0x3f; }

    /**
     * Sets bits 0-5 of the Type of Service field; expects a value in the 0..63 range
     */
    virtual void setDiffServCodePoint(int dscp)  { setTypeOfService( (getTypeOfService() & 0xc0) | (dscp & 0x3f)); }

    /**
     * Returns bits 6-7 of the Type of Service field, a value in the range 0..3
     */
    virtual int getExplicitCongestionNotification() const  { return (getTypeOfService() >> 6) & 0x03; }

    /**
     * Sets bits 6-7 of the Type of Service; expects a value in the 0..3 range
     */
    virtual void setExplicitCongestionNotification(int ecn)  { setTypeOfService( (getTypeOfService() & 0x3f) | ((ecn & 0x3) << 6)); }

    virtual void setOrigDatagram(IPv4Datagram *d);
    virtual IPv4Datagram *getOrigDatagram() const {return dgram;}
    virtual IPv4Datagram *removeOrigDatagram();

    virtual short getTransportProtocol() const { return IPv4ControlInfo_Base::getProtocol(); }
    virtual void setTransportProtocol(short protocol) { IPv4ControlInfo_Base::setProtocol(protocol); }
    virtual Address getSourceAddress() const { return Address(srcAddr_var); }
    virtual void setSourceAddress(const Address & address)  { srcAddr_var = address.toIPv4(); }
    virtual Address getDestinationAddress() const { return Address(destAddr_var); }
    virtual void setDestinationAddress(const Address & address) { destAddr_var = address.toIPv4(); }
    virtual int getInterfaceId() const { return IPv4ControlInfo_Base::getInterfaceId(); }
    virtual void setInterfaceId(int interfaceId) { IPv4ControlInfo_Base::setInterfaceId(interfaceId); }
    virtual short getHopLimit() const { return getTimeToLive(); }
    virtual void setHopLimit(short hopLimit) { setTimeToLive(hopLimit); }
};

#endif


