//
// Generated file, do not edit! Created by opp_msgc 4.4 from tictoc16.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "tictoc16_m.h"

USING_NAMESPACE

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(TicTocMsg16);

TicTocMsg16::TicTocMsg16(const char *name, int kind) : cMessage(name,kind)
{
    this->source_var = 0;
    this->destination_var = 0;
    this->hopCount_var = 0;
}

TicTocMsg16::TicTocMsg16(const TicTocMsg16& other) : cMessage(other)
{
    copy(other);
}

TicTocMsg16::~TicTocMsg16()
{
}

TicTocMsg16& TicTocMsg16::operator=(const TicTocMsg16& other)
{
    if (this==&other) return *this;
    cMessage::operator=(other);
    copy(other);
    return *this;
}

void TicTocMsg16::copy(const TicTocMsg16& other)
{
    this->source_var = other.source_var;
    this->destination_var = other.destination_var;
    this->hopCount_var = other.hopCount_var;
}

void TicTocMsg16::parsimPack(cCommBuffer *b)
{
    cMessage::parsimPack(b);
    doPacking(b,this->source_var);
    doPacking(b,this->destination_var);
    doPacking(b,this->hopCount_var);
}

void TicTocMsg16::parsimUnpack(cCommBuffer *b)
{
    cMessage::parsimUnpack(b);
    doUnpacking(b,this->source_var);
    doUnpacking(b,this->destination_var);
    doUnpacking(b,this->hopCount_var);
}

int TicTocMsg16::getSource() const
{
    return source_var;
}

void TicTocMsg16::setSource(int source)
{
    this->source_var = source;
}

int TicTocMsg16::getDestination() const
{
    return destination_var;
}

void TicTocMsg16::setDestination(int destination)
{
    this->destination_var = destination;
}

int TicTocMsg16::getHopCount() const
{
    return hopCount_var;
}

void TicTocMsg16::setHopCount(int hopCount)
{
    this->hopCount_var = hopCount;
}

class TicTocMsg16Descriptor : public cClassDescriptor
{
  public:
    TicTocMsg16Descriptor();
    virtual ~TicTocMsg16Descriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(TicTocMsg16Descriptor);

TicTocMsg16Descriptor::TicTocMsg16Descriptor() : cClassDescriptor("TicTocMsg16", "cMessage")
{
}

TicTocMsg16Descriptor::~TicTocMsg16Descriptor()
{
}

bool TicTocMsg16Descriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<TicTocMsg16 *>(obj)!=NULL;
}

const char *TicTocMsg16Descriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int TicTocMsg16Descriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int TicTocMsg16Descriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *TicTocMsg16Descriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "source",
        "destination",
        "hopCount",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int TicTocMsg16Descriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "source")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destination")==0) return base+1;
    if (fieldName[0]=='h' && strcmp(fieldName, "hopCount")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *TicTocMsg16Descriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *TicTocMsg16Descriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int TicTocMsg16Descriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    TicTocMsg16 *pp = (TicTocMsg16 *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string TicTocMsg16Descriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    TicTocMsg16 *pp = (TicTocMsg16 *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSource());
        case 1: return long2string(pp->getDestination());
        case 2: return long2string(pp->getHopCount());
        default: return "";
    }
}

bool TicTocMsg16Descriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    TicTocMsg16 *pp = (TicTocMsg16 *)object; (void)pp;
    switch (field) {
        case 0: pp->setSource(string2long(value)); return true;
        case 1: pp->setDestination(string2long(value)); return true;
        case 2: pp->setHopCount(string2long(value)); return true;
        default: return false;
    }
}

const char *TicTocMsg16Descriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *TicTocMsg16Descriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    TicTocMsg16 *pp = (TicTocMsg16 *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


