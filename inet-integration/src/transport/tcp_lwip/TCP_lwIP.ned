//
// Copyright (C) 2010 Zoltan Bojthe
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


package inet.transport.tcp_lwip;

import inet.transport.ITCP;


//
// TCP model based on the LWIP 1.3.2 project.
// The original LWIP is available on the http://savannah.nongnu.org/projects/lwip/ page.
//
// See ~ITCP for general TCP layer informations.
//
simple TCP_lwIP like ITCP
{
    parameters:
        bool recordStats = default(true); // recording of seqNum etc. into output vectors enabled/disabled
        string sendQueueClass = default("");    // Obsolete!!!
        string receiveQueueClass = default(""); // Obsolete!!!
        @display("i=block/wheelbarrow");

    gates:
        input appIn[];
        input ipIn;
        output appOut[];
        output ipOut;
}
