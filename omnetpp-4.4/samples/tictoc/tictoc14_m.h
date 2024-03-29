//
// Generated file, do not edit! Created by opp_msgc 4.4 from tictoc14.msg.
//

#ifndef _TICTOC14_M_H_
#define _TICTOC14_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0404
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>tictoc14.msg</tt> by opp_msgc.
 * <pre>
 * message TicTocMsg14
 * {
 *     int source;
 *     int destination;
 *     int hopCount = 0;
 * }
 * </pre>
 */
class TicTocMsg14 : public ::cMessage
{
  protected:
    int source_var;
    int destination_var;
    int hopCount_var;

  private:
    void copy(const TicTocMsg14& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const TicTocMsg14&);

  public:
    TicTocMsg14(const char *name=NULL, int kind=0);
    TicTocMsg14(const TicTocMsg14& other);
    virtual ~TicTocMsg14();
    TicTocMsg14& operator=(const TicTocMsg14& other);
    virtual TicTocMsg14 *dup() const {return new TicTocMsg14(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getSource() const;
    virtual void setSource(int source);
    virtual int getDestination() const;
    virtual void setDestination(int destination);
    virtual int getHopCount() const;
    virtual void setHopCount(int hopCount);
};

inline void doPacking(cCommBuffer *b, TicTocMsg14& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, TicTocMsg14& obj) {obj.parsimUnpack(b);}


#endif // _TICTOC14_M_H_
