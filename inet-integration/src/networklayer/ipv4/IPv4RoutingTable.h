//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004-2006 Andras Varga
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

//
//  Author:     Jochen Reber
//    Date:       18.5.00
//    On Linux:   19.5.00 - 29.5.00
//  Modified by Vincent Oberle
//    Date:       1.2.2001
//  Cleanup and rewrite: Andras Varga, 2004
//

#ifndef __INET_IPV4ROUTINGTABLE_H
#define __INET_IPV4ROUTINGTABLE_H

#include <vector>

#include "INETDefs.h"

#include "IPv4Address.h"
#include "IIPv4RoutingTable.h"
#include "ILifecycle.h"

class IInterfaceTable;
class RoutingTableParser;
class IRoutingTable;


/**
 * Represents the routing table. This object has one instance per host
 * or router. It has methods to manage the route table and the interface table,
 * so one can achieve functionality similar to the "route" and "ifconfig" commands.
 *
 * See the NED documentation for general overview.
 *
 * This is a simple module without gates, it requires function calls to it
 * (message handling does nothing). Methods are provided for reading and
 * updating the interface table and the route table, as well as for unicast
 * and multicast routing.
 *
 * Interfaces are dynamically registered: at the start of the simulation,
 * every L2 module adds its own interface entry to the table.
 *
 * The route table is read from a file (RoutingTableParser); the file can
 * also fill in or overwrite interface settings. The route table can also
 * be read and modified during simulation, typically by routing protocol
 * implementations (e.g. OSPF).
 *
 * Entries in the route table are represented by IPv4Route objects.
 * IPv4Route objects can be polymorphic: if a routing protocol needs
 * to store additional data, it can simply subclass from IPv4Route,
 * and add the derived object to the table.
 *
 * Uses RoutingTableParser to read routing files (.irt, .mrt).
 *
 *
 * @see InterfaceEntry, IPv4InterfaceData, IPv4Route
 */
class INET_API IPv4RoutingTable: public cSimpleModule, public IIPv4RoutingTable, protected cListener, public ILifecycle
{
  protected:
    IInterfaceTable *ift; // cached pointer

    IPv4Address routerId;
    bool IPForward;
    bool multicastForward;
    bool isNodeUp;

    // for convenience
    typedef IPv4MulticastRoute::OutInterface OutInterface;
    typedef IPv4MulticastRoute::OutInterfaceVector OutInterfaceVector;

    // routing cache: maps destination address to the route
    typedef std::map<IPv4Address, IPv4Route *> RoutingCache;
    mutable RoutingCache routingCache;

    // local addresses cache (to speed up isLocalAddress())
    typedef std::set<IPv4Address> AddressSet;
    mutable AddressSet localAddresses;
    // JcM add: to handle the local broadcast address
    mutable AddressSet localBroadcastAddresses;

  private:
    // The vectors storing routes are ordered by prefix length, administrative distance, and metric.
    // Subclasses should use internalAdd[Multicast]Route() and internalRemove[Multicast]Route() methods
    // to modify them, but they can not access them directly.

    typedef std::vector<IPv4Route *> RouteVector;
    RouteVector routes;          // Unicast route array, sorted by netmask desc, dest asc, metric asc

    typedef std::vector<IPv4MulticastRoute*> MulticastRouteVector;
    MulticastRouteVector multicastRoutes; // Multicast route array, sorted by netmask desc, origin asc, metric asc


  protected:
    // set IPv4 address etc on local loopback
    virtual void configureLoopbackForIPv4();

    // set router Id
    virtual void configureRouterId();

    // adjust routes with src=IFACENETMASK to actual interface netmasks
    virtual void updateNetmaskRoutes();

    // creates a new empty route
    virtual IPv4Route *createNewRoute();

    // displays summary above the icon
    virtual void updateDisplayString();

    // delete routes for the given interface
    virtual void deleteInterfaceRoutes(const InterfaceEntry *entry);

    // invalidates routing cache and local addresses cache
    virtual void invalidateCache();

    // helper for sorting routing table, used by addRoute()
    static bool routeLessThan(const IPv4Route *a, const IPv4Route *b);

    // helper for sorting multicast routing table, used by addMulticastRoute()
    static bool multicastRouteLessThan(const IPv4MulticastRoute *a, const IPv4MulticastRoute *b);

    // helper functions:
    void internalAddRoute(IPv4Route *entry);
    IPv4Route *internalRemoveRoute(IPv4Route *entry);
    void internalAddMulticastRoute(IPv4MulticastRoute *entry);
    IPv4MulticastRoute *internalRemoveMulticastRoute(IPv4MulticastRoute *entry);

  public:
    IPv4RoutingTable();
    virtual ~IPv4RoutingTable();

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

  public:
    /**
     * For debugging
     */
    virtual void printRoutingTable() const;

    /**
     * For debugging
     */
    virtual void printMulticastRoutingTable() const;

    /**
     * Returns the host or router this routing table lives in.
     */
    virtual cModule *getHostModule();

    /** @name Interfaces */
    //@{
    virtual void configureInterfaceForIPv4(InterfaceEntry *ie);

    /**
     * Returns an interface given by its address. Returns NULL if not found.
     */
    virtual InterfaceEntry *getInterfaceByAddress(const IPv4Address& address) const;
    //@}

    /**
     * IPv4 forwarding on/off
     */
    virtual bool isForwardingEnabled() const  {return IPForward;}

    /**
     * IPv4 multicast forwarding on/off
     */
    virtual bool isMulticastForwardingEnabled() const  { return multicastForward; }

    /**
     * Returns routerId.
     */
    virtual IPv4Address getRouterId() const  {return routerId;}

    /**
     * Sets routerId.
     */
    virtual void setRouterId(IPv4Address a);

    /** @name Routing functions (query the route table) */
    //@{
    /**
     * Checks if the address is a local one, i.e. one of the host's.
     */
    virtual bool isLocalAddress(const IPv4Address& dest) const;

    /** @name Routing functions (query the route table) */
    //@{
    /**
     * Checks if the address is a local network broadcast address, i.e. one of the
     * broadcast addresses derived from the interface addresses and netmasks.
     */
    virtual bool isLocalBroadcastAddress(const IPv4Address& dest) const;

    /**
     * Returns the interface entry having the specified address
     * as its local broadcast address.
     */
    virtual InterfaceEntry *findInterfaceByLocalBroadcastAddress(const IPv4Address& dest) const;

    /**
     * The routing function. Performs longest prefix match for the given
     * destination address, and returns the resulting route. Returns NULL
     * if there is no matching route.
     */
    virtual IPv4Route *findBestMatchingRoute(const IPv4Address& dest) const;

    /**
     * Convenience function based on findBestMatchingRoute().
     *
     * Returns the output interface for the packets with dest as destination
     * address, or NULL if the destination is not in routing table.
     */
    virtual InterfaceEntry *getInterfaceForDestAddr(const IPv4Address& dest) const;

    /**
     * Convenience function based on findBestMatchingRoute().
     *
     * Returns the gateway for the destination address. Returns the unspecified
     * address if the destination is not in routing table or the gateway field
     * is not filled in in the route.
     */
    virtual IPv4Address getGatewayForDestAddr(const IPv4Address& dest) const;
    //@}

    /** @name Multicast routing functions */
    //@{

    /**
     * Checks if the address is in one of the local multicast group
     * address list.
     */
    virtual bool isLocalMulticastAddress(const IPv4Address& dest) const;

    /**
     * Returns route for a multicast source and multicast group.
     */
    virtual const IPv4MulticastRoute *findBestMatchingMulticastRoute(const IPv4Address &origin, const IPv4Address& group) const;
    //@}

    /** @name Route table manipulation */
    //@{

    /**
     * Returns the total number of routes (unicast, multicast, plus the
     * default route).
     */
    virtual int getNumRoutes() const { return routes.size(); }

    /**
     * Returns the kth route.
     */
    virtual IPv4Route *getRoute(int k) const;

    /**
     * Finds and returns the default route, or NULL if it doesn't exist
     */
    virtual IPv4Route *getDefaultRoute() const;

    /**
     * Adds a route to the routing table. Routes are allowed to be modified
     * while in the routing table. (There is a notification mechanism that
     * allows routing table internals to be updated on a routing entry change.)
     */
    virtual void addRoute(IPv4Route *entry);

    virtual void addRoute(IPv4Route *entry, bool flag);

    /**
     * Removes the given route from the routing table, and returns it.
     * NULL is returned of the route was not in the routing table.
     */
    virtual IPv4Route *removeRoute(IPv4Route *entry);

    /**
     * Deletes the given route from the routing table.
     * Returns true if the route was deleted, and false if it was
     * not in the routing table.
     */
    virtual bool deleteRoute(IPv4Route *entry);

    /**
     * Returns the total number of multicast routes.
     */
    virtual int getNumMulticastRoutes() const { return multicastRoutes.size(); }

    /**
     * Returns the kth multicast route.
     */
    virtual IPv4MulticastRoute *getMulticastRoute(int k) const { return k < (int)multicastRoutes.size() ? multicastRoutes[k] : NULL; }

    /**
     * Adds a multicast route to the routing table. Routes are allowed to be modified
     * while in the routing table. (There is a notification mechanism that
     * allows routing table internals to be updated on a routing entry change.)
     */
    virtual void addMulticastRoute(IPv4MulticastRoute *entry);

    /**
     * Removes the given route from the routing table, and returns it.
     * NULL is returned of the route was not in the routing table.
     */
    virtual IPv4MulticastRoute *removeMulticastRoute(IPv4MulticastRoute *entry);

    /**
     * Deletes the given multicast route from the routing table.
     * Returns true if the route was deleted, and false if it was
     * not in the routing table.
     */
    virtual bool deleteMulticastRoute(IPv4MulticastRoute *entry);

    /**
     * Deletes invalid routes from the routing table. Invalid routes are those
     * where the isValid() method returns false.
     */
    virtual void purge();

    /**
     * Utility function: Returns a vector of all addresses of the node.
     */
    virtual std::vector<IPv4Address> gatherAddresses() const;

    /**
     * To be called from route objects whenever a field changes. Used for
     * maintaining internal data structures and firing "routing table changed"
     * notifications.
     */
    virtual void routeChanged(IPv4Route *entry, int fieldCode);

    /**
     * To be called from multicast route objects whenever a field changes. Used for
     * maintaining internal data structures and firing "routing table changed"
     * notifications.
     */
    virtual void multicastRouteChanged(IPv4MulticastRoute *entry, int fieldCode);
    //@}

    /**
     * ILifecycle method
     */
    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback);

    virtual Address getRouterIdAsGeneric() const {return getRouterId();}
    virtual bool isLocalAddress(const Address& dest) const {return isLocalAddress(dest.toIPv4());}
    virtual InterfaceEntry *getInterfaceByAddress(const Address& address) const {return getInterfaceByAddress(address.toIPv4());}
    virtual IRoute *findBestMatchingRoute(const Address& dest) const {return findBestMatchingRoute(dest.toIPv4());}
    virtual InterfaceEntry *getOutputInterfaceForDestination(const Address& dest) const {return getInterfaceForDestAddr(dest.toIPv4());} //XXX inconsistent names
    virtual Address getNextHopForDestination(const Address& dest) const {return getGatewayForDestAddr(dest.toIPv4());}  //XXX inconsistent names
    virtual bool isLocalMulticastAddress(const Address& dest) const {return isLocalMulticastAddress(dest.toIPv4());}
    virtual IMulticastRoute *findBestMatchingMulticastRoute(const Address &origin, const Address& group) const {return const_cast<IPv4MulticastRoute*>(findBestMatchingMulticastRoute(origin.toIPv4(), group.toIPv4()));} //XXX remove 'const' from IPv4 method?
    virtual IRoute *createRoute() { return new IPv4Route(); }


  private:
    virtual void addRoute(IRoute *entry) { addRoute(check_and_cast<IPv4Route *>(entry)); }
    virtual IRoute *removeRoute(IRoute *entry) { return removeRoute(check_and_cast<IPv4Route *>(entry)); }
    virtual bool deleteRoute(IRoute *entry) { return deleteRoute(check_and_cast<IPv4Route *>(entry)); }

    virtual void addMulticastRoute(IMulticastRoute *entry) { addMulticastRoute(check_and_cast<IPv4MulticastRoute *>(entry)); }
    virtual IMulticastRoute *removeMulticastRoute(IMulticastRoute *entry) { return removeMulticastRoute(check_and_cast<IPv4MulticastRoute *>(entry)); }
    virtual bool deleteMulticastRoute(IMulticastRoute *entry) { return deleteMulticastRoute(check_and_cast<IPv4MulticastRoute *>(entry)); }
};

#endif
