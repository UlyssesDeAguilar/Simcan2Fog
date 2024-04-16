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
#include "Management/dataClasses/Users/CloudUserPriority.h"
#include "Management/dataClasses/Users/CloudUserInstancePriority.h"

class UserGeneratorCost : public UserGenerator_simple
{
protected:
    friend class CloudUserModel;

    /**Hasmap to accelerate the management of the users*/
    std::map<std::string, bool> priorizedHashMap;
    omnetpp::cPar *offerAcceptanceDistribution;
    double offerCostIncrease;

    /** Vector that contains the types of slas generated in the current simulation */
    std::string strUserTraceSla;

    virtual void initialize() override;
    virtual void finish() override;
    virtual void initializeHashMaps();

    virtual SM_UserVM *createVmMessage() override;
    virtual CloudUser *createUserTraceType() override;

    virtual CloudUserInstance *createCloudUserInstance(const CloudUser *ptrUser,
                                                       unsigned int totalUserInstance,
                                                       unsigned int userNumber,
                                                       int currentInstanceIndex,
                                                       int totalUserInstances) override;

    BaseUserModel *newUserModelInstance() { return new CloudUserModel(*this); }
};

#endif
