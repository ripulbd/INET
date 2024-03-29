// Copyright (C) 2007 Faqir Zarrar Yousaf
// Communication Networks Institute
// University of Dortmund, Germany
// faqir.yousaf@uni-dortmund.de
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


package inet.examples.mobileipv6;

import inet.physicallayer.radio.simplified.SimplifiedRadioChannel;
import inet.nodes.xmipv6.WirelessHost6;
import inet.nodes.xmipv6.HomeAgent6;
import inet.nodes.xmipv6.CorrespondentNode6;
import inet.nodes.wireless.AccessPoint;
import inet.nodes.ipv6.Router6;
import inet.networklayer.autorouting.ipv6.FlatNetworkConfigurator6;
import inet.linklayer.ethernet.EtherHub;


channel fiberline extends ned.DatarateChannel
{
    parameters:
        delay = 1us;
        datarate = 512Mbps;
}

channel ethernetline extends ned.DatarateChannel
{
    parameters:
        delay = 0.1us;
        datarate = 100Mbps;
}

network mIPv6Network
{
    parameters:
        double total_mn;
        double total_cn;

        @display("bgb=799,698");
    submodules:
        configurator: FlatNetworkConfigurator6 {
            parameters:
                @display("p=763,53");
        }
        radioChannel: SimplifiedRadioChannel {
            parameters:
                numChannels = 5;
                @display("p=753,123");
        }
        Home_Agent: HomeAgent6 {
            parameters:
                @display("p=249,229;i=abstract/router");
        }
        R_1: Router6 {
            parameters:
                @display("p=566,227");
        }
        R_2: Router6 {
            parameters:
                @display("p=406,355");
        }
        MN[total_mn]: WirelessHost6 {
            parameters:
                @display("p=220,404");
        }
        CN[total_cn]: CorrespondentNode6;
        AP_Home: AccessPoint {
            parameters:
                @display("p=249,172;i=device/accesspoint_s");
        }
        AP_1: AccessPoint {
            parameters:
                @display("p=566,172;i=device/accesspoint_s");
        }
        hub: EtherHub {
            parameters:
                @display("p=406,412;i=device/hub_s");
        }
    connections allowunconnected:

                         //Home_Agent.ethOut++ --> ethernetline --> R_1.ethIn++;
                         //Home_Agent.ethIn++ <-- ethernetline <-- R_1.ethOut++;

        R_1.ethg++ <--> ethernetline <--> R_2.ethg++;

                                                       //R_2.ethOut++ --> ethernetline --> R_3.ethIn++;
                                                       //R_2.ethIn++ <-- ethernetline <-- R_3.ethOut++;

        Home_Agent.ethg$o++ --> ethernetline --> R_2.ethg$i++;
        Home_Agent.ethg$i++ <-- ethernetline <-- R_2.ethg$o++;

        for i=0..total_cn-1 {
            CN[i].ethg$o++ --> ethernetline --> hub.ethg$i++;
            CN[i].ethg$i++ <-- ethernetline <-- hub.ethg$o++;
        }

        hub.ethg++ <--> ethernetline <--> R_2.ethg++;

                                                    //R_1.ethOut++ --> ethernetline --> CN.ethIn++;
                                                    //R_1.ethIn++ <-- ethernetline <-- CN.ethOut++;

        AP_Home.ethg$o++ --> ethernetline --> Home_Agent.ethg$i++;
        AP_Home.ethg$i++ <-- ethernetline <-- Home_Agent.ethg$o++;

        AP_1.ethg++ <--> ethernetline <--> R_1.ethg++;

                                                        //AP_2.ethOut++ --> ethernetline --> R_2.ethIn++;
                                                        //AP_2.ethIn++ <-- ethernetline <-- R_2.ethOut++;

                                                        //AP_3.ethOut++ --> ethernetline --> R_3.ethIn++;
                                                        //AP_3.ethIn++ <-- ethernetline <-- R_3.ethOut++;

}

