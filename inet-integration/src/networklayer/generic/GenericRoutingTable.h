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

#ifndef __GENERICROUTINGTABLE_H
#define __GENERICROUTINGTABLE_H

#include <vector>

#include "INETDefs.h"

#include "GenericRoute.h"
#include "IRoutingTable.h"

class IInterfaceTable;


/**
 * A C++ interface to abstract the functionality of a routing table, regardless of address type.
 */
class INET_API GenericRoutingTable : public cSimpleModule, public IRoutingTable, public cListener
{
    private:
        IInterfaceTable *ift; // cached pointer

        Address routerId;
        Address::AddressType addressType;
        bool forwardingEnabled;
        bool multicastForwardingEnabled;

        typedef std::vector<GenericRoute *> RouteVector;
        RouteVector routes;          // unicast route table, sorted by prefix match order

        typedef std::vector<GenericMulticastRoute*> MulticastRouteVector;
        MulticastRouteVector multicastRoutes; // multicast route table, sorted by prefix match order

    protected:
        virtual int numInitStages() const { return NUM_INIT_STAGES; }
        virtual void initialize(int stage);

        /**
         * Raises an error.
         */
        virtual void handleMessage(cMessage *);

        /**
         * Called by the signal handler whenever a change of a category
         * occurs to which this client has subscribed.
         */
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);

        virtual void configureRouterId();

        virtual void updateDisplayString();

        virtual void configureInterface(InterfaceEntry *ie);

        virtual void configureLoopback();

        static bool routeLessThan(const GenericRoute *a, const GenericRoute *b);

        void internalAddRoute(GenericRoute *route);
        GenericRoute *internalRemoveRoute(GenericRoute *route);

    public:
        GenericRoutingTable();
        virtual ~GenericRoutingTable();

        /** @name Miscellaneous functions */
        //@{
        /**
         * Forwarding on/off
         */
        virtual bool isForwardingEnabled() const;  //XXX IP modulba?

        /**
         * Multicast forwarding on/off
         */
        virtual bool isMulticastForwardingEnabled() const; //XXX IP modulba?

        /**
         * Returns routerId.
         */
        virtual Address getRouterIdAsGeneric() const;

        /**
         * Checks if the address is a local one, i.e. one of the host's.
         */
        virtual bool isLocalAddress(const Address& dest) const;  //XXX maybe into InterfaceTable?

        /**
         * Returns an interface given by its address. Returns NULL if not found.
         */
        virtual InterfaceEntry *getInterfaceByAddress(const Address& address) const;   //XXX should be find..., see next one

        /**
         * To be called from route objects whenever a field changes. Used for
         * maintaining internal data structures and firing "routing table changed"
         * notifications.
         */
        virtual void routeChanged(GenericRoute *entry, int fieldCode);
        //@}

        /** @name Routing functions (query the route table) */
        //@{
        /**
         * The routing function. Performs longest prefix match for the given
         * destination address, and returns the resulting route. Returns NULL
         * if there is no matching route.
         */
        virtual GenericRoute *findBestMatchingRoute(const Address& dest) const;  //TODO make coveriant return types everywhere

        /**
         * Convenience function based on findBestMatchingRoute().
         *
         * Returns the output interface for the packets with dest as destination
         * address, or NULL if the destination is not in routing table.
         */
        virtual InterfaceEntry *getOutputInterfaceForDestination(const Address& dest) const;  //XXX redundant

        /**
         * Convenience function based on findBestMatchingRoute().
         *
         * Returns the gateway for the destination address. Returns the unspecified
         * address if the destination is not in routing table or the gateway field
         * is not filled in in the route.
         */
        virtual Address getNextHopForDestination(const Address& dest) const; //XXX redundant AND unused
        //@}

        /** @name Multicast routing functions */
        //@{

        /**
         * Checks if the address is in one of the local multicast group
         * address list.
         */
        virtual bool isLocalMulticastAddress(const Address& dest) const;

        /**
         * Returns route for a multicast origin and group.
         */
        virtual IMulticastRoute *findBestMatchingMulticastRoute(const Address &origin, const Address& group) const;
        //@}

        /** @name Route table manipulation */
        //@{

        /**
         * Returns the total number of unicast routes.
         */
        virtual int getNumRoutes() const;

        /**
         * Returns the kth route.
         */
        virtual IRoute *getRoute(int k) const;

        /**
         * Finds and returns the default route, or NULL if it doesn't exist
         */
        virtual IRoute *getDefaultRoute() const;  //XXX is this a universal concept?

        /**
         * Adds a route to the routing table. Routes are allowed to be modified
         * while in the routing table. (There is a notification mechanism that
         * allows routing table internals to be updated on a routing entry change.)
         */
        virtual void addRoute(IRoute *entry);

        /**
         * Removes the given route from the routing table, and returns it.
         * NULL is returned if the route was not in the routing table.
         */
        virtual IRoute *removeRoute(IRoute *entry);

        /**
         * Deletes the given route from the routing table.
         * Returns true if the route was deleted, and false if it was
         * not in the routing table.
         */
        virtual bool deleteRoute(IRoute *entry);

        /**
         * Returns the total number of multicast routes.
         */
        virtual int getNumMulticastRoutes() const;

        /**
         * Returns the kth multicast route.
         */
        virtual IMulticastRoute *getMulticastRoute(int k) const;

        /**
         * Adds a multicast route to the routing table. Routes are allowed to be modified
         * while in the routing table. (There is a notification mechanism that
         * allows routing table internals to be updated on a routing entry change.)
         */
        virtual void addMulticastRoute(IMulticastRoute *entry);

        /**
         * Removes the given route from the routing table, and returns it.
         * NULL is returned of the route was not in the routing table.
         */
        virtual IMulticastRoute *removeMulticastRoute(IMulticastRoute *entry);

        /**
         * Deletes the given multicast route from the routing table.
         * Returns true if the route was deleted, and false if it was
         * not in the routing table.
         */
        virtual bool deleteMulticastRoute(IMulticastRoute *entry);
        //@}

        virtual IRoute *createRoute();
};


#endif

