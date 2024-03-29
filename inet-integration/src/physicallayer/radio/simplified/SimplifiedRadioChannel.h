/* -*- mode:c++ -*- ********************************************************
 * file:        SimplifiedRadioChannel.h
 *
 * copyright:   (C) 2006 Levente Meszaros, 2005 Andras Varga
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 **************************************************************************/

#ifndef __INET_SIMPLIFIEDRADIOCHANNEL_H
#define __INET_SIMPLIFIEDRADIOCHANNEL_H

#include <vector>
#include <list>
#include <set>

#include "INETDefs.h"
#include "Coord.h"
#include "RadioChannelBase.h"
#include "ISimplifiedRadioChannel.h"

// Forward declarations
class SimplifiedRadioFrame;

#define TRANSMISSION_PURGE_INTERVAL 1.0

/**
 * Keeps track of radios/NICs, their positions and channels;
 * also caches neighbor info (which other Radios are within
 * interference distance).
 */
struct ISimplifiedRadioChannel::RadioEntry {
    cModule *radioModule;  // the module that registered this radio interface
    cGate *radioInGate;  // gate on host module used to receive radio frames
    int channel;
    Coord pos; // cached radio position

    struct Compare {
        bool operator() (const RadioRef &lhs, const RadioRef &rhs) const {
            ASSERT(lhs && rhs);
            return lhs->radioModule->getId() < rhs->radioModule->getId();
        }
    };
    // we cache neighbors set in an std::vector, because std::set iteration is slow;
    // std::vector is created and updated on demand
    std::set<RadioRef, Compare> neighbors; // cached neighbor list
    std::vector<RadioRef> neighborList;
    bool isNeighborListValid;
};

/**
 * Monitors which radios are "in range". Supports multiple channels.
 *
 * @ingroup radioChannel
 * @see ChannelAccess
 */
class INET_API SimplifiedRadioChannel : public RadioChannelBase, public ISimplifiedRadioChannel
{
  protected:
    typedef std::list<RadioEntry> RadioList;
    typedef std::vector<RadioRef> RadioRefVector;

    RadioList radios;

    /** keeps track of ongoing transmissions; this is needed when a radio
     * switches to another channel (then it needs to know whether the target channel
     * is empty or busy)
     */
    typedef std::vector<TransmissionList> ChannelTransmissionLists;
    ChannelTransmissionLists transmissions; // indexed by channel number (size=numChannels)

    /** used to clear the transmission list from time to time */
    simtime_t lastOngoingTransmissionsUpdate;

    friend std::ostream& operator<<(std::ostream&, const RadioEntry&);
    friend std::ostream& operator<<(std::ostream&, const TransmissionList&);

    /** the biggest interference distance in the network.*/
    double maxInterferenceDistance;

  protected:
    virtual void updateConnections(RadioRef h);

    /** Calculate interference distance*/
    virtual double calcInterfDist();

    /** Reads init parameters and calculates a maximal interference distance*/
    virtual void initialize(int stage);

    /** Throws away expired transmissions. */
    virtual void purgeOngoingTransmissions();

    /** Validate the channel identifier */
    virtual void checkChannel(int channel);

    /** Get the list of modules in range of the given host */
    virtual const RadioRefVector& getNeighbors(RadioRef h);

    /** Notifies the radio channel with an ongoing transmission */
    virtual void addOngoingTransmission(RadioRef h, SimplifiedRadioFrame *frame);

    /** Returns the "handle" of a previously registered radio. The pointer to the registering (radio) module must be provided */
    virtual RadioRef lookupRadio(cModule *radioModule);

  public:
    SimplifiedRadioChannel();
    virtual ~SimplifiedRadioChannel();

    /** Registers the given radio. If radioInGate==NULL, the "radioIn" gate is assumed */
    virtual RadioRef registerRadio(cModule *radioModule, cGate *radioInGate = NULL);

    /** Unregisters the given radio */
    virtual void unregisterRadio(RadioRef r);

    /** Returns the host module that contains the given radio */
    virtual cModule *getRadioModule(RadioRef r) const { return r->radioModule; }

    /** Returns the input gate of the host for receiving radio frames */
    virtual cGate *getRadioGate(RadioRef r) const { return r->radioInGate; }

    /** Returns the channel the given radio listens on */
    virtual int getRadioChannel(RadioRef r) const { return r->channel; }

    /** To be called when the host moved; updates proximity info */
    virtual void setRadioPosition(RadioRef r, const Coord& pos);

    /** Called when host switches channel */
    virtual void setRadioChannel(RadioRef r, int channel);

    /** Provides a list of transmissions currently on the air */
    virtual const TransmissionList& getOngoingTransmissions(int channel);

    /** Called from ChannelAccess, to transmit a frame to the radios in range, on the frame's channel */
    virtual void sendToChannel(RadioRef srcRadio, SimplifiedRadioFrame *radioFrame);

    /** Returns the maximal interference distance*/
    virtual double getInterferenceRange(RadioRef r) { return maxInterferenceDistance; }

    /** Returns propagation speed of the signal in meter/sec */
    virtual double getPropagationSpeed() { return SPEED_OF_LIGHT; }
};

#endif
