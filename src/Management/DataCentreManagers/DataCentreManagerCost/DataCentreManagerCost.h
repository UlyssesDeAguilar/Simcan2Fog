#ifndef __SIMCAN_2_0_DATACENTREMANAGERCOST_H_
#define __SIMCAN_2_0_DATACENTREMANAGERCOST_H_

#include "../DataCentreManagerFirstFit/DataCentreManagerFirstFit.h"
#include "Management/dataClasses/Users/CloudUserPriority.h"

/**
 * @brief Module that implements resource allocation based on user priority
 * @author (v1) Adrian Bernal
 * @author (v2) Ulysses de Aguilar
 */
class DataCentreManagerCost : public DataCentreManagerFirstFit
{
protected:
    bool checkReservedFirst;
    
    virtual void initialize() override;
    virtual void initializeRequestHandlers() override;
    virtual std::pair<uint32_t, size_t> selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs) override;
    std::pair<uint32_t, size_t> selectNodeReserved(SM_UserVM_Cost *userVM_Rq, const VirtualMachine &vmSpecs);
};

#endif
