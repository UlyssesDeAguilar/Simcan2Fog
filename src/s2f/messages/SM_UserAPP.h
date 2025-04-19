#ifndef SM_USERAPP_H_
#define SM_USERAPP_H_

#include <algorithm>

#include "s2f/architecture/net/routing/RoutingInfo_m.h"
#include "s2f/core/include/GroupVector.hpp"
#include "s2f/messages/SM_UserAPP_m.h"

/**
 * @brief Class that describes a user app deployment request
 * @author Pablo Cerro Cañizares
 * @author Ulysses de Aguilar
 * @version 1.2
 * @date 28-01-2024
 */
class SM_UserAPP : public SM_UserAPP_Base
{
private:
	void copy(const SM_UserAPP &other)
	{
	}
	
	public:
	SM_UserAPP(const char *name = nullptr, short kind = 0) : SM_UserAPP_Base(name, kind) {}
	SM_UserAPP(const SM_UserAPP &other) : SM_UserAPP_Base(other) { copy(other); }
	virtual SM_UserAPP *dup() const override { return new SM_UserAPP(*this); }
	
	SM_UserAPP &operator=(const SM_UserAPP &other)
	{
		if (this == &other)
			return *this;
		SM_UserAPP_Base::operator=(other);
		copy(other);
		return *this;
	}
	
	void createNewAppRequest(const char *name, const char *type, double startRentTime);
	int findRequestIndex(const char *service);

	void changeState(const char *service, tApplicationState eNewState);
	void changeStateByIndex(int nIndex, tApplicationState eNewState);

	bool isFinishedOK(const char *service);
	bool isFinishedKO(const char *service);

	void abortAllApps();
	bool allAppsFinishedOK();
	bool allAppsFinishedKO();
	bool allAppsFinished();

	/**
	 * @brief Updates the state of each individual app instance
	 * @param newData A SM_UserAPP containing the newerstates
	 */
	// virtual void update(const SM_UserAPP *newData);
	friend std::ostream &operator<<(std::ostream &os, const SM_UserAPP &obj);
	friend class UserAppBuilder;
};

class UserAppBuilder
{
private:
	std::map<std::string, std::vector<AppRequest>> requests;

public:
	void newRequest()
	{
		requests.clear();
	}

	void createNewAppRequest(const char *name, const char *type,
							 const char *vmId, double startRentTime)
	{
		AppRequest appRQ;

		appRQ.setName(name);
		appRQ.setType(type);
		appRQ.setState(appWaiting);
		appRQ.setStartTime(startRentTime);

		EV_DEBUG << "+RQ(new): App: " << appRQ.getName() << " | status: "
				 << appRQ.getState() << " | startTime: "
				 << appRQ.getStartTime() << " | endTime: "
				 << appRQ.getFinishTime() << '\n';

		requests[vmId].push_back(appRQ);
	}

	std::vector<SM_UserAPP *> *finish(const char *userId)
	{
		auto petitions = new std::vector<SM_UserAPP *>();

		for (const auto &entry : requests)
		{
			SM_UserAPP *request = new SM_UserAPP();
			// Setup of the request message
			request->setUserId(userId);
			request->setVmId(entry.first.c_str());
			request->setIsResponse(false);
			request->setOperation(SM_APP_Req);
			request->setAppArraySize(entry.second.size());

			// Push data in compact and organized way
			int i = 0;
			for (const auto &elem : entry.second)
				request->setApp(i++, elem);

			petitions->push_back(request);
		}

		return petitions;
	}
};

#endif /* SM_USERAPP_H_ */
