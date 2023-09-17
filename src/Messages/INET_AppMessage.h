#ifndef __INET_APPMESSAGE_H
#define __INET_APPMESSAGE_H

#include "INET_AppMessage_m.h"

class INET_AppMessage: public INET_AppMessage_Base {
protected:
    using INET_AppMessage_Base::setAppMessage;
public:
    INET_AppMessage(SIMCAN_Message *sm);
    INET_AppMessage(const INET_AppMessage& other) : INET_AppMessage_Base(other) {}
    virtual INET_AppMessage *dup() const override { return new INET_AppMessage(*this);}

    INET_AppMessage& operator=(const INET_AppMessage &other);
    bool operator==(const INET_AppMessage&);
};

#endif
