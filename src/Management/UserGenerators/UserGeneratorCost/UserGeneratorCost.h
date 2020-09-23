//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __SIMCAN_2_0_USERGENERATORCOST_H_
#define __SIMCAN_2_0_USERGENERATORCOST_H_

#include <algorithm>
#include "Management/UserGenerators/UserGenerator_simple/UserGenerator_simple.h"
#include "Messages/SM_UserVM_Cost_m.h"
#include "Management/dataClasses/CloudUserPriority.h"
#include "Management/dataClasses/CloudUserInstancePriority.h"

using namespace omnetpp;

class UserGeneratorCost : public UserGenerator_simple
{
  protected:
    /**Hasmap to accelerate the management of the users*/
        std::map<std::string, int> extensionTimeHashMap;
        std::map<std::string, bool> priorizedHashMap;
        cPar* offerAcceptanceDistribution;
        double offerCostIncrease;

        /** Vector that contains the types of slas generated in the current simulation */
        std::vector<Sla*> slaTypes;


    virtual void initialize() override;

    virtual void initializeHashMaps();

    virtual CloudUserInstance* handleResponseAccept(SIMCAN_Message *userVm_RAW) override;

    virtual CloudUserInstance* handleResponseReject(SIMCAN_Message *msg) override;


//    virtual SM_UserVM* createVmRequest(CloudUserInstance *pUserInstance) override;

//
//    /**
//     * Updates the status of a user
//     */
//    virtual void updateVmUserStatus(std::string strUserId, std::string strVmId, tVmState state) override;

    virtual CloudUserInstance* handleResponseAppTimeout(SIMCAN_Message *msg) override;

    virtual void parseConfig() override;

    /**
     * Parses each sla type used in the simulation. These slas are allocated in the <b>slaTypes</b> vector.
     *
     * @return If the parsing process is successfully executed, this method returns SC_OK. In other case, it returns SC_ERROR.
     */
    virtual int parseSlasList();

    /**
     * Parses each user type used in the simulation. These users are allocated in the <b>userTypes</b> vector.
     *
     * @return If the parsing process is successfully executed, this method returns SC_OK. In other case, it returns SC_ERROR.
     */
    virtual int parseUsersList() override;

    /**
     * Converts the parsed sla into string format.
     *
     * @return A string containing the parsed slas.
     */
    std::string slasToString();

    /**
     * Search for a specific type of CloudUser.
     *
     * @param userType Type of a user.
     * @return If the requested type of user is located in the userTypes vector, then a pointer to its object is returned. In other case, \a nullptr is returned.
     */
     Sla* findSla (std::string slaType);

    virtual bool hasToExtendVm(SM_UserAPP* userApp);

    virtual void resumeExecution(SM_UserAPP* userApp);

    virtual void endExecution(SM_UserAPP* userApp);

    virtual SM_UserVM* createVmMessage() override;

    virtual CloudUserInstance* createCloudUserInstance(CloudUser *ptrUser, unsigned int  totalUserInstance, unsigned int  userNumber, int currentInstanceIndex, int totalUserInstances) override;

//
//  public:
//    /**
//     * Builds an App request given a user
//     */
//    virtual SM_UserAPP* createAppRequest(SM_UserVM* userVm) override;
//
//    /**
//     * Handles the App response sent from the CloudProvider
//     * @param userApp incoming message
//     */
//    //virtual void handleUserAppResponse(SIMCAN_Message* userApp) override;
//
//    /**
//     * Handles the VM response received from the CloudProvider
//     * @param userVm incoming message
//     */
//    //virtual void handleUserVmResponse(SM_UserVM* userVm) override;
//
//    /**
//     * Calculates the statistics of the experiment.
//     */
    virtual void calculateStatistics() override;
//

//
//    /**
//     * Builds the VM request which corresponds with the provided user instance.
//     */
//    virtual SM_UserVM* createVmRequest(CloudUserInstance* pUserInstance) override;
};

#endif
