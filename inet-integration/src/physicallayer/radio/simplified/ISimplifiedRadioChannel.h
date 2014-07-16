/* -*- mode:c++ -*- ********************************************************
 * file:        ISimplifiedRadioChannel.h
 *
 * copyright:   (C) Rudolf Hornig
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 *
 **************************************************************************/

#ifndef __INET_ISIMPLIFIEDRADIOCHANNEL_H
#define __INET_ISIMPLIFIEDRADIOCHANNEL_H

#include <vector>
#include <list>
#include <set>

#include "INETDefs.h"
#include "Coord.h"

// Forward declarations
class SimplifiedRadioFrame;

/**
 * Interface to implement for a module that controls radio frequency channel access.
 */
class INET_API ISimplifiedRadioChannel
{
  protected:
    struct RadioEntry;

  public:
    typedef RadioEntry *RadioRef; // handle for SimplifiedRadioChannel's clients
    typedef std::list<SimplifiedRadioFrame*> TransmissionList;

  public:
    virtual ~ISimplifiedRadioChannel() {}

    /** Registers the given radio. If radioInGate==NULL, the "radioIn" gate is assumed */
    virtual RadioRef registerRadio(cModule *radioModule, cGate *radioInGate = NULL) = 0;

    /** Unregisters the given radio */
    virtual void unregisterRadio(RadioRef r) = 0;

    /** Returns the host module that contains the given radio */
    virtual cModule *getRadioModule(RadioRef r) const = 0;

    /** Returns the input gate of the host for receiving SimplifiedRadioFrames */
    virtual cGate *getRadioGate(RadioRef r) const = 0;

    /** Returns the channel the given radio listens on */
    virtual int getRadioChannel(RadioRef r) const = 0;

    /** To be called when the host moved; updates proximity info */
    virtual void setRadioPosition(RadioRef r, const Coord& pos) = 0;

    /** Called when host switches channel */
    virtual void setRadioChannel(RadioRef r, int channel) = 0;

    /** Provides a list of transmissions currently on the air */
    virtual const TransmissionList& getOngoingTransmissions(int channel) = 0;

    /** Called from ChannelAccess, to transmit a frame to the radios in range, on the frame's channel */
    virtual void sendToChannel(RadioRef srcRadio, SimplifiedRadioFrame *radioFrame) = 0;

    /** Returns the maximal interference distance*/
    virtual double getInterferenceRange(RadioRef r) = 0;

    /** Returns propagation speed of the signal in meter/sec */
    virtual double getPropagationSpeed() = 0;
};

#endif
