//
// Copyright (C) 2004 Andras Varga
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

#ifndef __INET_NETWORKDATAGRAMMULTIPLEXER_H_
#define __INET_NETWORKDATAGRAMMULTIPLEXER_H_

#include "INETDefs.h"

/**
 * This class provides network datagram multiplexing based on the the datagram
 * runtime type or on the type of attached control info.
 */
class NetworkDatagramMultiplexer : public cSimpleModule {
  public:
    NetworkDatagramMultiplexer() { }
    virtual ~NetworkDatagramMultiplexer() { }

  protected:
    virtual void handleMessage(cMessage * message);
    int getProtocolIndex(cMessage * message);
};

#endif
