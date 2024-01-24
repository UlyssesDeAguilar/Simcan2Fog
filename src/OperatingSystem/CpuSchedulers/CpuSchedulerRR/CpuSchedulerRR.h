#ifndef __CPU_SCHEDULER_RR_H_
#define __CPU_SCHEDULER_RR_H_

#include "Core/cSIMCAN_Core.h"

/**
 * @class CpuSchedulerRR CpuSchedulerRR.h "CpuSchedulerRR.h"
 *
 * CPU scheduler that implements a Round-Robin policy
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2009-03-11
 */
class CpuSchedulerRR : public cSIMCAN_Core
{
public:
	unsigned int *getCpuCoreIndex() const;
	void setCpuCoreIndex(unsigned int *cpuCoreIndex);
	bool isRunning() const;
	void setIsRunning(bool bRunning);
	unsigned int getManagedCpuCores() const;
	void setManagedCpuCores(unsigned int managedCpuCores);
	bool *getIsCpuIdle() const;
	void setIsCpuIdle(bool *isCpuIdle);

protected:
	/** Indicates if this blade allows virtualization */
	bool isVirtualHardware;

	/** Number of CPU cores in the CPU processor */
	unsigned int numCpuCores;

	/** Number of CPU cores managed by this scheduler */
	unsigned int managedCpuCores;

	/** Quantum */
	int quantum;

	/** Flag that indicates if this scheduler is currently used */
	bool bRunning;

	/** Array to show the CPU with an idle state */
	bool *isCPU_Idle;

	/** Array that indicates the CPU cores managed by this schedulers */
	unsigned int *cpuCoreIndex;

	/** Request queue array */
	cQueue requestsQueue;
	cQueue abortsQueue;

	/** Output gate to the Hypervisor. */
	cGate *toHypervisorGate;

	/** Input gate from the Hypervisor. */
	cGate *fromHypervisorGate;

	/** Output gates to checking Hub. */
	cGate *toHubGate;

	/** Input gates from checking Hub. */
	cGate *fromHubGate;

	/**
	 * Destructor.
	 */
	~CpuSchedulerRR();

	/**
	 *  Module initialization.
	 */
	void initialize() override;

	/**
	 * Module ending.
	 */
	void finish() override;
	bool deleteFromRequestsQueue(SIMCAN_Message *sm);
	bool deleteFromAbortsQueue(SIMCAN_Message *sm);
	int getVirtualCpuIndex(unsigned int realCpuIndex);
	void stopAllProcess();
	void stopCpu(unsigned int virtualCpuIndex);

private:
	/**
	 * Get the outGate ID to the module that sent <b>msg</b>
	 * @param msg Arrived message.
	 * @return. Gate Id (out) to module that sent <b>msg</b> or NOT_FOUND if gate not found.
	 */
	cGate *getOutGate(cMessage *msg);

	/**
	 * Process a self message.
	 * @param msg Self message.
	 */
	void processSelfMessage(cMessage *msg);

	/**
	 * Process a request message.
	 * @param sm Request message.
	 */
	void processRequestMessage(SIMCAN_Message *sm);

	/**
	 * Process a response message.
	 * @param sm Request message.
	 */
	void processResponseMessage(SIMCAN_Message *sm);

	/**
	 * Search for an idle CPU
	 *
	 * @return The index of an idle CPU or SC_NotFound if all CPU are busy.
	 */
	int searchIdleCPU();
};

#endif
