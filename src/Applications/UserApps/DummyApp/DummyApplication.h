#ifndef __DUMMY_APPLICATION_H_
#define __DUMMY_APPLICATION_H_

#include <omnetpp.h>
#include "Applications/Base/UserAppBase/UserAppBase.h"

/**
 * @class DummyApplication DummyApplication.h "DummyApplication.h"
 *
 * Dummy application.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-13
 */
class DummyApplication : public UserAppBase
{
protected:
	string testParameter;

	virtual void initialize() override;
	virtual void finish() override;
	virtual void run() override{};

	void processRequestMessage(SIMCAN_Message *sm) override;
	void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif
