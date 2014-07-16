//
// Copyright (C) 2013 Andras Varga
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

#ifndef __INET_INETWORKLAYER_H_
#define __INET_INETWORKLAYER_H_

#include "INetworkProtocolControlInfo.h"
#include "NetworkProtocolCommand_m.h"

/**
 * This purely virtual interface provides an abstraction for different network protocols.
 */
class INET_API INetworkProtocol {
  public:
    virtual ~INetworkProtocol() { }

//    virtual INetworkLayerControlInfo *createControlInfo() = 0;

//    virtual Address resolveAddress(const char * address) = 0;
};

#endif
