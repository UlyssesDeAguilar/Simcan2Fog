#ifndef _SIMCAN_MESSAGE_H_
#define _SIMCAN_MESSAGE_H_

#include "SIMCAN_Message_m.h"
#include "Core/include/SIMCAN_opcodes.h"
#include "inet/networklayer/common/L3Address.h"
#include <stack>
#include <sstream>
#include <vector>
#include <string>
#include <map>
using namespace omnetpp;


/**
 * @class SIMCAN_Message SIMCAN_Message.h "SIMCAN_Message.h"
 *
 * 
 * @details Class that represents a SIMCAN_Message.
 * 
 * Updated by Ulysses de Aguilar Gudmundsson based upon the work made by 
 * Pablo Cerro Cañizares and Alberto Nuñez Covarrubias
 * 
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @version 3.0
 * @date 24-09-2023
 */
class SIMCAN_Message : public SIMCAN_Message_Base
{
private:
	/**
	 * @brief Base for copying operations (constructor, dup, assignment operator, etc)
	 * @param other 
	 */
	void copy(const SIMCAN_Message &other);

protected:
	typedef std::stack<inet::L3Address> IpStack;
	typedef std::vector<std::pair<std::string, std::vector<TraceComponent>>> TraceVector;

	cMessage *parentRequest; // Pointer to parent request message or NULL if has no parent
	TraceVector trace;		 // Message trace
	IpStack *ipStack;		 // Stack that keeps trace of the IPs (SimCan2CloudEX)
public:
	/**
	 * Destructor.
	 */
	virtual ~SIMCAN_Message();

	/**
	 * Constructor of SIMCAN_Message
	 * @param name Message name
	 * @param kind Message kind
	 */
	SIMCAN_Message(const char *name = NULL, int kind = 0);

	/**
	 * Constructor of SIMCAN_Message
	 * @param other Message
	 */
	SIMCAN_Message(const SIMCAN_Message &other);

	/**
	 * = operator
	 * @param other Message
	 */
	SIMCAN_Message &operator=(const SIMCAN_Message &other);

	/**
	 * Method that makes a copy of a SIMCAN_Message
	 */
	virtual SIMCAN_Message *dup() const { return new SIMCAN_Message(*this); }

	void addNewIp(inet::L3Address addr);

	void popIp();

	inet::L3Address getNextIp();

	/**
	 * Reserve space for the trace
	 */
	virtual void setTraceArraySize(size_t size);

	/**
	 * Gets the trace size
	 */
	virtual size_t getTraceArraySize() const;

	/**
	 * Empty method
	 */
	virtual const TraceComponent &getTrace(size_t k) const;

	/**
	 * Empty method
	 */
	virtual void setTrace(size_t k, const TraceComponent &trace_var){};

	/**
	 * Add a module to message trace
	 * @param module Added mdule.
	 * @param gate Gate ID to <b>module</b>.
	 * @param currentRequest Current request number.
	 */
	void addModuleToTrace(int module, int gate, reqNum_t currentRequest);

	/**
	 * Removes the last module from the message trace, including all its request numbers.
	 * If last trace component is not a Module, then return an empty string.
	 */
	void removeLastModuleFromTrace();

	/**
	 * Add a node to message trace.
	 * @param node Added node.
	 */
	void addNodeToTrace(std::string node);

	/**
	 * Removes the last node from message trace, including all its modules and request numbers.
	 */
	void removeLastNodeFromTrace();

	/**
	 * Add a request to message trace.
	 * @param request Added request.
	 */
	void addRequestToTrace(int request);

	/**
	 * Removes last request from message trace.
	 */
	void removeLastRequestFromTrace();

	/**
	 * Get the las Gate ID from message trace.
	 * @return If all OK, the return last gate ID, else return SIMCAN_ERROR
	 */
	int getLastGateId();

	/**
	 * Get the las module ID from message trace.
	 * @return If all OK, the return last module ID, else return SIMCAN_ERROR
	 */
	int getLastModuleId();

	/**
	 * Get the current request.
	 * @return Current request or SIMCAN_ERROR if an error occurs.
	 */
	int getCurrentRequest();

	/**
	 * Get the parent request message
	 * @return Pointer to parent request message or NULL if not exists a parent message.
	 */
	cMessage *getParentRequest() const;

	/**
	 * Set the parent request message
	 * @param parent Pointer to parent request message.
	 */
	void setParentRequest(cMessage *parent);

	/**
	 * Parse current trace to string
	 * @return String with the corresponding trace.
	 */
	std::string traceToString();

	/**
	 * Parse all parameters of current message to string.
	 * @param showContents Shows the contents of the message.
	 * @param includeTrace Message trace is also included in the parsing process.
	 * @return String with the corresponding contents.
	 */
	virtual std::string contentsToString(bool showContents, bool includeTrace);

	/**
	 * Parse a SIMCAN_Message operation to string.
	 * @param operation SIMCAN_Message operation
	 * @return Operation SIMCAN_Message in string format.
	 */
	std::string operationToString(unsigned int operation);

	/**
	 * Parse a SIMCAN_Message operation to string.
	 * @return Operation SIMCAN_Message in string format.
	 */
	std::string operationToString();

	/**
	 * Calculates if message trace is empty.
	 * @return True if message trace is empty or false in another case.
	 */
	bool isTraceEmpty();

	/**
	 * Method that calculates if current subRequest fits with its parent trace.
	 *
	 */
	bool fitWithParent();

	/**
	 * Method that calculates if this message is son of <b>parentMsg</b>.
	 * @return True if current message if son of parentMsg, or false in another case.
	 */
	bool fitWithParent(cMessage *parentMsg);

	/**
	 * Add a node to trace
	 * @param host Hostname
	 * @param nodeTrace Node trace
	 */
	void addNodeTrace(std::string host, std::vector<TraceComponent> nodeTrace);

	/**
	 * Get the name of initial module in node k
	 * @param k kth Node
	 * @return Name of the initial module in the kth node
	 */
	std::string getInitialModuleName(int k);

	/**
	 * Get the kthe traceNode
	 * @param k kth node
	 * @return kth node trace
	 */
	std::vector<TraceComponent> getNodeTrace(int k);
	
	virtual void insertTrace(const TraceComponent& trace) {};
	virtual void insertTrace(size_t k, const TraceComponent& trace) {};
    virtual void eraseTrace(size_t k) {};
};

#endif
