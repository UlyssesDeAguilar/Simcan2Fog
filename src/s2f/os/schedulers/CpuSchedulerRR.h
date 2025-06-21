#ifndef __CPU_SCHEDULER_RR_H_
#define __CPU_SCHEDULER_RR_H_

#include "s2f/core/cSIMCAN_Core.h"

namespace s2f
{
	namespace os
	{
		/**
		 * @class CpuSchedulerRR CpuSchedulerRR.h "CpuSchedulerRR.h"
		 *
		 * CPU scheduler that implements a Round-Robin policy
		 *
		 * @author Alberto N&uacute;&ntilde;ez Covarrubias
		 * @author Ulysses de Aguilar Gudmundsson
		 * @version 1.1
		 * @date 2024-12-25
		 */
		class CpuSchedulerRR : public cSIMCAN_Core
		{
		public:
			/**
			 * @brief Gets the assigned CPU core index
			 * @return unsigned int* The array that indicates the CPU cores managed by this scheduler
			 */
			unsigned int *getCpuCoreIndex() const;

			/**
			 * @brief Sets the assigned CPU core index
			 * @param cpuCoreIndex The array that indicates the CPU cores managed by this scheduler
			 */
			void setCpuCoreIndex(unsigned int *cpuCoreIndex);

			/**
			 * @brief Gets the flag that indicates if this scheduler is currently running
			 * @return bool True if the scheduler is running, false otherwise
			 */
			bool isRunning() const;

			/**
			 * @brief Sets the flag that indicates if this scheduler is currently running
			 * @param bRunning True if the scheduler is running, false otherwise
			 */
			void setIsRunning(bool bRunning);

			/**
			 * @brief Gets the number of CPU cores managed by this scheduler
			 * @return unsigned int The number of CPU cores managed by this scheduler
			 */
			unsigned int getManagedCpuCores() const;

			/**
			 * @brief Sets the number of CPU cores managed by this scheduler
			 * @param managedCpuCores The number of CPU cores managed by this scheduler
			 */
			void setManagedCpuCores(unsigned int managedCpuCores);

		protected:
			unsigned int numCpuCores;	  //!< Number of CPU cores in the CPU processor
			unsigned int managedCpuCores; //!< Number of CPU cores managed by this scheduler
			int quantum;				  //!< Quantum
			bool isVirtualHardware;		  //!< Indicates if this blade allows virtualization
			bool bRunning;				  //!< Flag that indicates if this scheduler is currently used
			std::vector<bool> cores;	  //!< Array to show the CPU with an idle state
			unsigned int *cpuCoreIndex;	  //!< Array that indicates the CPU cores managed by this schedulers
			cQueue requestsQueue;		  //!< Queue of requests
			cQueue abortsQueue;			  //!< Queue of aborts
			cGate *toHypervisorGate;	  //!< Output to Hypervisor
			cGate *fromHypervisorGate;	  //!< Input from Hypervisor
			cGate *toHubGate;			  //!< Output to Hub
			cGate *fromHubGate;			  //!< Input from Hub

			virtual void initialize() override;
			virtual void finish() override;

			/**
			 * @brief Deletes a certain request from the requests queue
			 *
			 * @param sm The request message
			 * @return bool True if the request was deleted, false otherwise
			 */
			bool deleteFromRequestsQueue(SIMCAN_Message *sm);

			/**
			 * @brief Deletes a certain abort from the aborts queue
			 *
			 * @param sm The abort message
			 * @return bool True if the abort was deleted, false otherwise
			 */
			bool deleteFromAbortsQueue(SIMCAN_Message *sm);

			/**
			 * @brief Translates from a real CPU index to a virtual CPU index
			 *
			 * @param realCpuIndex The real cpu index
			 * @return int The virtual cpu index
			 */
			int getVirtualCpuIndex(unsigned int realCpuIndex);

			/**
			 * @brief Stops all the CPUs
			 */
			void stopAllProcess();

			/**
			 * @brief Stops a specific CPU
			 * @param virtualCpuIndex The virtual CPU index to be stopped
			 */
			void stopCpu(unsigned int virtualCpuIndex);

		private:
			/**
			 * Get the outGate ID to the module that sent <b>msg</b>
			 * @param msg Arrived message.
			 * @return. Gate Id (out) to module that sent <b>msg</b> or NOT_FOUND if gate not found.
			 */
			virtual cGate *getOutGate(cMessage *msg) override;

			/**
			 * Process a self message.
			 * @param msg Self message.
			 */
			virtual void processSelfMessage(cMessage *msg) override;

			/**
			 * Process a request message.
			 * @param sm Request message.
			 */
			virtual void processRequestMessage(SIMCAN_Message *sm) override;

			/**
			 * Process a response message.
			 * @param sm Request message.
			 */
			virtual void processResponseMessage(SIMCAN_Message *sm) override;

			/**
			 * Search for an idle CPU
			 * @return The index of an idle CPU or SC_NotFound if all CPU are busy.
			 */
			int searchIdleCPU();
		};
	}
}

#endif
