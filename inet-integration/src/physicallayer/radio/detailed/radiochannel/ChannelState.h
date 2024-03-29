#ifndef _CHANNELSTATE_H_
#define _CHANNELSTATE_H_

#include <sstream>
#include "INETDefs.h"

/**
 * @brief Provides information about the current state of the channel:
 *
 * idle/receiving - is the physical currently receiving something?
 * RSSI - the currently received signal strength indicator.
 *
 * @ingroup decider
 */
class INET_API ChannelState
{
protected:

	/** @brief defines if the channel is currently idle */
	bool idle;

    /** @brief defines if the channel is currently busy */
	bool busy;

	/** @brief the current RSSI value of the channel */
	double rssi;
public:

	/**
	 * @brief Creates and initializes a new ChannelState with the
	 * passed state.
	 *
	 * idle - defines if the channel is currently idle
	 * rssi - the current RSSI value of the channel
	 */
	ChannelState(bool idle = false, bool busy = false, double rssi = 0.0) :
		idle(idle), busy(busy), rssi(rssi) {}

	/**
	 * @brief Returns true if the channel is considered idle, meaning
	 * depends on the used decider.
	 */
	bool isIdle() const { return idle; }

	bool isBusy() const { return busy; }

	bool isReceiving() const { return !idle; }

	/**
	 * @brief Returns the current RSSI value of the channel.
	 */
	double getRSSI() const { return rssi; }

	/**
	 * @brief Output for this ChannelState.
	 *
	 * Of the form "[<idle/receiving> with rssi of x]".
	 */
	std::string info() const {
		std::stringstream os;
		if (idle) {
			os << "[idle with rssi of ";
		} else {
			os << "[receiving with rssi of ";
		}
		os << rssi << "]";
		return os.str();
	}
};

#endif /*_CHANNELSTATE_H_*/
