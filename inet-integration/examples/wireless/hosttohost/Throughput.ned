//
// Copyright (C) 2005 Andras Varga
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

package inet.examples.wireless.hosttohost;

import inet.applications.ethernet.EtherAppCli;
import inet.base.Sink;
import inet.linklayer.ieee80211.Ieee80211Nic;
import inet.mobility.single.CircleMobility;
import inet.mobility.static.StationaryMobility;
import inet.nodes.wireless.AccessPoint;
import inet.physicallayer.radio.simplified.SimplifiedRadioChannel;

module ThroughputClient
{
    parameters:
        @display("i=device/wifilaptop");
        @node();
    gates:
        input radioIn @directIn;

    submodules:
        cli: EtherAppCli {
            parameters:
                registerSAP = false;
                @display("b=40,24;p=180,60,col");
        }
        wlan: Ieee80211Nic {
            parameters:
                @display("p=112,134;q=queue");
                mgmtType = "Ieee80211MgmtSTASimplified";
        }
        mobility: CircleMobility {
            parameters:
                @display("p=50,141");
        }
    connections allowunconnected:
        wlan.radioIn <-- radioIn;
        cli.out --> wlan.upperLayerIn;
}

module ThroughputServer
{
    parameters:
        @display("i=device/wifilaptop");
        @node();
    gates:
        input radioIn @directIn;

    submodules:
        sink: Sink {
            parameters:
                @display("p=210,68,col");
        }
        wlan: Ieee80211Nic {
            parameters:
                @display("p=120,158;q=queue");
                mgmtType = "Ieee80211MgmtSTASimplified";
        }
        mobility: StationaryMobility {
            parameters:
                @display("p=50,141");
        }
    connections allowunconnected:
        wlan.radioIn <-- radioIn;
        sink.in++ <-- wlan.upperLayerOut;
}

network Throughput
{
    parameters:
        int numCli;
        @display("b=297,203");
    submodules:
        cliHost[numCli]: ThroughputClient {
            parameters:
                @display("r=,,#707070");
        }
        srvHost: ThroughputServer {
            parameters:
                @display("p=350,350;r=,,#707070");
        }
        ap: AccessPoint {
            parameters:
                @display("p=200,200;r=,,#707070");
                wlan[*].mgmtType = "Ieee80211MgmtAPSimplified";
        }
        radioChannel: SimplifiedRadioChannel {
            parameters:
                @display("p=61,46");
        }
}

