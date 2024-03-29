import inet.nodes.inet.StandardHost;
import inet.transport.sctp.SCTPNatRouter;
import inet.nodes.inet.Router;
import ned.DatarateChannel;

network multiRendezvous {
	parameters:
        volatile double testTimeout;
        bool testing;
    submodules:
        Peer_1: StandardHost {
            parameters:
                IPForward = false;
                routingTable.routingFile = "../../lib/nat_peer1.mrt";
                networkLayer.configurator.networkConfiguratorModule = "";
		@display("p=56,156;i=device/laptop");
            gates:
                pppg[2];
	}
        Peer_2: StandardHost {
            parameters:
                IPForward = false;
                routingTable.routingFile = "../../lib/nat_peer2.mrt";
                networkLayer.configurator.networkConfiguratorModule = "";
		@display("p=436,160;i=device/laptop");
            gates:
                pppg[2];
	}
        Server: StandardHost {
            parameters:
                IPForward = false;
                routingTable.routingFile = "../../lib/nat_server.mrt";
                networkLayer.configurator.networkConfiguratorModule = "";
		@display("p=252,140;i=device/server");
            gates:
                pppg[2];
	}
        Nat_1_1: SCTPNatRouter {
            parameters:
                routingTable.routingFile = "../../lib/nat1_1.mrt";
                networkLayer.configurator.networkConfiguratorModule = "";
		@display("p=152,104;i=block/join");
            gates:
                pppg[2];
	}
        Nat_2_1: SCTPNatRouter {
            parameters:
                routingTable.routingFile = "../../lib/nat2_1.mrt";
                networkLayer.configurator.networkConfiguratorModule = "";
		@display("p=340,104;i=block/join");
            gates:
                pppg[2];
	}
        Router_1: Router {
            parameters:
                routingTable.routingFile = "../../lib/nat_router_1.mrt";
                networkLayer.configurator.networkConfiguratorModule = "";
		@display("p=252,48;i=abstract/router");
            gates:
                pppg[3];
	}
        Nat_1_2: SCTPNatRouter {
		parameters:
                routingTable.routingFile = "../../lib/nat1_2.mrt";
                networkLayer.configurator.networkConfiguratorModule = "";
		@display("p=152,200;i=block/join");
            gates:
                pppg[2];
	}
        Nat_2_2: SCTPNatRouter {
		parameters:
                routingTable.routingFile = "../../lib/nat2_2.mrt";
                networkLayer.configurator.networkConfiguratorModule = "";
		@display("p=340,204;i=block/join");
            gates:
                pppg[2];
	}
        Router_2: Router {
	    parameters:
                routingTable.routingFile = "../../lib/nat_router_2.mrt";
                networkLayer.configurator.networkConfiguratorModule = "";
		@display("p=252,232;i=abstract/router");
            gates:
                pppg[3];
	}
    connections:
        Peer_1.pppg[0] <--> natline <--> Nat_1_1.pppg[0];
        Nat_1_1.pppg[1] <--> natline <--> Router_1.pppg[0];
        Router_1.pppg[2] <--> natline <--> Server.pppg[0];
        Router_1.pppg[1] <--> natline <--> Nat_2_1.pppg[0];
        Nat_2_1.pppg[1] <--> natline <--> Peer_2.pppg[0];
        Peer_1.pppg[1] <--> natline <--> Nat_1_2.pppg[0];
        Nat_2_2.pppg[1] <--> natline <--> Peer_2.pppg[1];
        Nat_1_2.pppg[1] <--> natline <--> Router_2.pppg[0];
        Router_2.pppg[1] <--> natline <--> Nat_2_2.pppg[0];
        Router_2.pppg[2] <--> natline <--> Server.pppg[1];

}

channel natline extends DatarateChannel
{
    datarate = 100 Mbps;
}

