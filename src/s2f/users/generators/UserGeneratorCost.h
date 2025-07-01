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

#include "s2f/users/generators/UserGenerator_simple.h"
#include "s2f/management/dataClasses/Users/CloudUserPriority.h"
#include "s2f/management/dataClasses/Users/CloudUserInstancePriority.h"
#include "s2f/messages/SM_UserVM_Cost_m.h"

namespace s2f
{
    namespace users
    {
        class UserGeneratorCost : public UserGenerator_simple
        {
        protected:
            friend class CloudUserModel;

            std::map<std::string, bool> priorizedHashMap; //!< Map to accelerate the management of the users
            omnetpp::cPar *offerAcceptanceDistribution;   //!< Distribution of acceptance of a vm extension
            double offerCostIncrease;                     //!< Increase in the cost of a vm extension
            std::string strUserTraceSla;                  //!< Slas of the simulation. FIXME: Remove this

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
    }
}


#endif /* __SIMCAN_2_0_USERGENERATORCOST_H_ */
