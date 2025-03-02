#ifndef __DUMMY_APPLICATION_H_
#define __DUMMY_APPLICATION_H_

#include <omnetpp.h>

#include "s2f/apps/AppBase.h"

/**
 * @class DummyApplication DummyApplication.h "DummyApplication.h"
 *
 * Dummy application.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-13
 */
class DummyApplication : public AppBase
{
protected:
	const char* testParameter;
	virtual void initialize() override;
	virtual void finish() override;
	virtual void scheduleExecStart() override {}
	void processSelfMessage(cMessage *msg) override {}
	void processRequestMessage(SIMCAN_Message *sm) override;
	void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif
