#include "INET_AppMessage.h"
#include <cstring>

#define NAME_LEN 20

INET_AppMessage::INET_AppMessage(SIMCAN_Message *sm) {
    chunkLength = inet::b(sm->getBitLength());
    appMessage = sm;
}

INET_AppMessage& INET_AppMessage::operator=(const INET_AppMessage &other) {
    INET_AppMessage_Base::operator=(other);
    return *this;
}

bool INET_AppMessage::operator==(const INET_AppMessage &other) {
    return this->appMessage == other.getAppMessage();
}
