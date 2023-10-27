#include "Messages/SM_FogIO.h"

Register_Class(SM_FogIO);

/* This section implements all the compulsory methods from
 * SM_FogIO_Base
 */
SM_FogIO::SM_FogIO(const char *name, short kind) : SM_FogIO_Base(name, kind)
{
    setByteLength(SM_BaseLength);
    setName("SM_FogIO");
}

SM_FogIO::SM_FogIO(const SM_FogIO &other) : SM_FogIO_Base(other.getName())
{
    operator=(other);
    setByteLength(SM_BaseLength);
    setName("SM_FogIO");
}

SM_FogIO::~SM_FogIO()
{
    // Empty destructor
}

SM_FogIO &SM_FogIO::operator=(const SM_FogIO &other)
{
    SM_FogIO_Base::operator=(other);
    return *this;
}

SM_FogIO *SM_FogIO::dup() const
{
    SM_FogIO *newMessage = new SM_FogIO();

    // Base parameters...
    newMessage->setOperation(getOperation());
    newMessage->setIsResponse(isResponse());
    newMessage->setRemoteOperation(getRemoteOperation());
    newMessage->setConnectionId(getConnectionId());
    newMessage->setCommId(getCommId());
    newMessage->setSourceId(getSourceId());
    newMessage->setNextModuleIndex(getNextModuleIndex());
    newMessage->setResult(getResult());

    newMessage->setByteLength(getByteLength());
    newMessage->setParentRequest(getParentRequest());

    // The fields related to the FogIO
    newMessage->setRequestSize(getRequestSize());

    return newMessage;
}
/* -------------- RELEVANT SECTION  -------------- */

void SM_FogIO::setRequestSize(double requestSize)
{
    SM_FogIO_Base::setRequestSize(requestSize);
    setByteLength(requestSize * MB);
}
