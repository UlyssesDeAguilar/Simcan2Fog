#ifndef __CPU_CORE_H_
#define __CPU_CORE_H_

#include "s2f/core/cSIMCAN_Core.h"

/**
 * @class CpuCore CpuCore.h "CpuCore.h"
 *
 * Network Service Module
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-11
 */
class CpuCore : public cSIMCAN_Core
{
protected:
	double speed;	// Speed of the CPU core 
	simtime_t tick; // Tick time (in seconds) 
	double ipt;		// Instructions per tick 

	cGate *inGate;	// Gate ID. Input gate. 
	cGate *outGate; // Gate ID. Output gate. 

	cMessage *pendingMessage; // Pending message 

	virtual void initialize() override;
	virtual void finish() override;

private:
	/**
	 * Calculates the amount of time to execute completely the current computing block.
	 * @param reainingMIs Million instructions to be executed.
	 * @return Time to execute reainingMIs instructions.
	 */
	simtime_t getTimeToExecuteCompletely(unsigned long reainingMIs);

	/**
	 * Calculates the amount of time to execute completely the current computing block.
	 * @param reainingTime Amount of time for executing current CB.
	 * @return Amount of CPU time to execute reainingTime.
	 */
	simtime_t getMaximumTimeToExecute(simtime_t reainingTime);

	virtual cGate *getOutGate(cMessage *msg) override;
	virtual void processSelfMessage(cMessage *msg) override;
	virtual void processRequestMessage(SIMCAN_Message *sm) override;
	virtual void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif
