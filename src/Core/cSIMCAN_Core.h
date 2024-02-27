#ifndef __SIMCAN_2_0_cSIMCANCORE_H_
#define __SIMCAN_2_0_cSIMCANCORE_H_

#include <omnetpp.h>
#include <string>
#include <cassert>
#include "../Messages/SM_CPU_Message.h"
#include "include/SIMCAN_types.h"
#include "Management/utils/LogUtils.h"

#define assert_msg(exp, msg) assert(((void)msg, exp))
using namespace omnetpp;
using std::string;
using std::vector;

/**
 * @brief Gets a value associated with a key
 * @details It's a template that uses type inference
 * @tparam K Key type
 * @tparam V Value type
 * @param map The map
 * @param key Key to search
 * @return V The value associated with the key or nullptr
 */
template <typename K, typename V>
V *getOrNull(const std::map<K, V> &map, const K &key)
{
	auto iter = map.find(key);
	return iter != map.end() ? &iter->second : nullptr;
}

template <typename K, typename V>
const V *getOrNullConst(const std::map<K, V> &map, const K &key)
{
	auto iter = map.find(key);
	return iter != map.end() ? &iter->second : nullptr;
}

template <typename K, typename V>
V *getOrNull(const std::map<K, V *> &map, const K &key)
{
	auto iter = map.find(key);
	return iter != map.end() ? iter->second : nullptr;
}

template <typename K, typename V>
V getOrDefault(const std::map<K, V> &map, const K &key, V default_val)
{
	auto iter = map.find(key);
	return iter != map.end() ? iter->second : default_val;
}

enum SimCanInitStages
{
	LOCAL,			  //!< Strictly read from parameters given to module
	NEAR,			  //!< For modules that cooperate closely, pass configuration around
	BLADE,			  //!< Blade registering in Fog/Node
	MANAGER,		  //!< Manager final processing of nodes registered
	DC,				  //!< DataCentre registering in CP DB
	CP,				  //!< CloudProvider processing Data Centres registrations
	TOTAL_INIT_STAGES //!< Constant that indicates the total number of init stages
};

/**
 * @class cSIMCAN_Core cSIMCAN_Core.h "cSIMCAN_Core.h"
 *
 * Base class that contains common functionality for SIMCAN classes.
 * This functionality includes:
 *
 * - Manage the path between modules for the received/sent messages.
 * - Send response/request messages to the corresponding destination module.
 * - Recognize response/request messages.
 *
 */
class cSIMCAN_Core : public cSimpleModule
{

protected:
	bool showInitValues;	  //!< Flag used to show module parameters at initialize phase 
	bool showMessageContents; //!< Flag used to show the content of each processed message in the log 
	bool showMessageTrace;	  //!< Show the trace of each message (for deep-debugging purpose only) 
	bool debugSimcanCore;	  //!< Show log messages of cSIMCAN_Core (for deep-debugging purpose only) 
	cQueue queue;			  //!< Incoming messages Queue 
	cMessage *latencyMessage; //!< Latency message 
	reqNum_t currentRequest;  //!< Current request 

	virtual void initialize() override;
	virtual void initialize(int stage) override { if (stage == 0) initialize(); }
	virtual int numInitStages() const override { return 1; }
	virtual void finish() override;
	virtual ~cSIMCAN_Core();

	/**
	 * @brief This method classifies an incoming message and invokes the corresponding method
	 * to process it.
	 *
	 * For self-messages, it invokes processSelfMessage(sm);
	 * For request-messages, it invokes processRequestMessage(sm);
	 * For response-messages, it invokes processResponseMessage(sm);
	 */
	virtual void handleMessage(cMessage *msg) override;

	/**
	 * Get the out Gate to the module that sent <b>msg</b>.
	 * @param msg Arrived message.
	 * @return Gate (out) to module that sent <b>msg</b> or nullptr if gate not found.
	 */
	virtual cGate *getOutGate(cMessage *msg) = 0;

	/**
	 * Process a self message.
	 * @param msg Self message.
	 */
	virtual void processSelfMessage(cMessage *msg) = 0;

	/**
	 * Process a request message.
	 * @param sm Request message.
	 */
	virtual void processRequestMessage(SIMCAN_Message *sm) = 0;

	/**
	 * Process a response message.
	 * @param sm Request message.
	 */
	virtual void processResponseMessage(SIMCAN_Message *sm) = 0;

	/**
	 * Send a request message to its destination!
	 * @param sm Request message.
	 * @param gate Gate used to send the message.
	 */
	virtual void sendRequestMessage(SIMCAN_Message *sm, cGate *gate) { sendRequestMessage(sm, gate->getId()); }

	/**
	 * Send a request message to its destination
	 * @param sm Request message
	 * @param gateId The gate id that will be used to send the message
	 */
	virtual void sendRequestMessage(SIMCAN_Message *sm, int gateId);

	/**
	 * Send a response message to its destination!
	 * @param sm Response message.
	 */
	virtual void sendResponseMessage(SIMCAN_Message *sm);

	/**
	 * Update the current message trace with current module ID and current request number.
	 * @param sm Received message to be updated.
	 */
	void updateMessageTrace(SIMCAN_Message *sm);

	/**
	 * Process current request message.
	 */
	virtual void processCurrentRequestMessage();

	/**
	 * This method calculates if there is any pending request.
	 * @return True if there is no pending request or false if there is a pending request.
	 */
	virtual bool isPendingRequest();

	/**
	 * Returns a string containing the values of each parameter.
	 * If the module that extends this class needs to show more data structures, it should override this method.
	 */
	virtual string initialParametersToString();
};

#endif /*cSIMCAN_Base_H_*/
