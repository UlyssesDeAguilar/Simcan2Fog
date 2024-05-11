#ifndef SIMCAN_EX_HTTP_PROXY_SERVICE
#define SIMCAN_EX_HTTP_PROXY_SERVICE

#include "Messages/SM_REST_API_m.h"
#include "TcpBaseProxy.h"

class HttpProxyThread; // Forward Declaration

class HttpProxyService : public TcpBaseProxy
{
protected:
    // Factory method so that subclasses can have their own thread class
    virtual TcpBaseProxyThread *newTcpThread() override;
};

class HttpProxyThread : public TcpBaseProxyThread
{
protected:
    virtual void selectFromPool(SIMCAN_Message *sm) override;
};

#endif
