//***************************************************************************
// * file:        SimplifiedRadioChannel.ned
// *
// * author:      Andras Varga, Steffen Sroka, Daniel Willkomm
// *
// * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
// *              Technische Universitaet Berlin, Germany.
// *
// *              This program is free software; you can redistribute it
// *              and/or modify it under the terms of the GNU General Public
// *              License as published by the Free Software Foundation; either
// *              version 2 of the License, or (at your option) any later
// *              version.
// *              For further information see file COPYING
// *              in the top level directory
// ***************************************************************************
// * part of:     framework implementation developed by tkn
// **************************************************************************/

package inet.physicallayer.radio.simplified;

import inet.physicallayer.radio.common.RadioChannelBase;

//
// ~SimplifiedRadioChannel has exactly one instance in every network model that
// contains mobile or wireless nodes. This module gets informed about the
// location and movement of nodes, and determines which nodes are within
// communication or interference distance. This info is then used by the
// radio interfaces of nodes at transmissions.
// Must be named as "radioChannel" inside the network.
//
// This ~SimplifiedRadioChannel is a different implementation from the one in
// Mobility Framework 1.0a5: here we use sendDirect(), while the MF version
// used normal send() and dynamic connections.
//
// @author Andras Varga (based on MF's SimplifiedRadioChannel by Steffen Sroka and Daniel Willkomm)
// @see ~IMobility
//
simple SimplifiedRadioChannel extends RadioChannelBase
{
    parameters:
        double pMax @unit("mW") = default(20mW); // maximum sending power used for this network (in mW)
        double sat @unit("dBm") = default(-110dBm); // signal attenuation threshold (in dBm)
        double alpha = default(2); // path loss coefficient
        double carrierFrequency @unit("Hz") = default(2.4GHz); // base carrier frequency of all the channels (in Hz)
        string propagationModel @enum("FreeSpaceModel","TwoRayGroundModel","RiceModel","RayleighModel","NakagamiModel","LogNormalShadowingModel") = default("FreeSpaceModel");

        @class(SimplifiedRadioChannel);
        @display("i=misc/sun");
        @labels(node);
}
