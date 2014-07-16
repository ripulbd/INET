//
// Generated file, do not edit! Created by opp_msgc 4.4 from TelnetPkt.msg.
//

#ifndef _TELNETPKT_M_H_
#define _TELNETPKT_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0404
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include "NetPkt_m.h"
// }}



/**
 * Class generated from <tt>TelnetPkt.msg</tt> by opp_msgc.
 * <pre>
 * packet TelnetPkt extends NetPkt
 * {
 *     string payload;
 * };
 * </pre>
 */
class TelnetPkt : public ::NetPkt
{
  protected:
    opp_string payload_var;

  private:
    void copy(const TelnetPkt& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const TelnetPkt&);

  public:
    TelnetPkt(const char *name=NULL, int kind=0);
    TelnetPkt(const TelnetPkt& other);
    virtual ~TelnetPkt();
    TelnetPkt& operator=(const TelnetPkt& other);
    virtual TelnetPkt *dup() const {return new TelnetPkt(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getPayload() const;
    virtual void setPayload(const char * payload);
};

inline void doPacking(cCommBuffer *b, TelnetPkt& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, TelnetPkt& obj) {obj.parsimUnpack(b);}


#endif // _TELNETPKT_M_H_
