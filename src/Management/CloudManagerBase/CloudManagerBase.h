#ifndef __SIMCAN_2_0_CLOUDMANAGER_BASE_H_
#define __SIMCAN_2_0_CLOUDMANAGER_BASE_H_

#include "Core/cSIMCAN_Core.h"
#include "Core/DataManager/DataManager.h"
#include "Management/utils/LogUtils.h"
#include "Messages/SM_CloudProvider_Control_m.h"
#include "Messages/SM_UserVM.h"
#include "Management/algorithms/ExponentialSmoothing.h"
#include <algorithm>
#include <functional>

// #define INITIAL_STAGE "INITIAL_STAGE"
// #define EXEC_APP_END "EXEC_APP_END"
// #define EXEC_VM_RENT_TIMEOUT "EXEC_VM_RENT_TIMEOUT"
// #define EXEC_APP_END_SINGLE "EXEC_APP_END_SINGLE"
// #define EXEC_APP_END_SINGLE_TIMEOUT "EXEC_APP_END_SINGLE_TIMEOUT"
// #define MANAGE_SUBSCRIBTIONS "MANAGE_SUBSCRIBTIONS"
// #define USER_SUBSCRIPTION_TIMEOUT "SUBSCRIPTION_TIMEOUT"
// #define SIMCAN_MESSAGE "SIMCAN_Message"
// #define CPU_STATUS "CPU_STATUS"
// #define MANAGE_MACHINES "MANAGE_MACHINES"
#define FORECASTING_DIMENSION 1

/**
 * @brief Base class for Cloud Managers.
 * @details This class is the base for managing users. Before it also took the responsibility to parse the data types
 * but that functionality has been given to the DataManager.
 * @author Pablo Cerro Cañizares
 * @author Ulysses de Aguilar Gudmundsson
 * @version 2.0
 */
class CloudManagerBase : public cSIMCAN_Core
{

protected:
  typedef enum
  {
    INITIAL_STAGE,
    EXEC_APP_END,
    EXEC_VM_RENT_TIMEOUT,
    EXEC_APP_END_SINGLE,
    EXEC_APP_END_SINGLE_TIMEOUT,
    MANAGE_SUBSCRIBTIONS,
    USER_SUBSCRIPTION_TIMEOUT,
    SIMCAN_MESSAGE,
    CPU_STATUS,
    MANAGE_MACHINES
  }Event;
  template <class T>
  using Callback = std::map<int, std::function<void(T *)>>;

  DataManager *dataManager; // Data Manager
  bool bFinished;           // Flag that indicates if the process has been finished
  bool acceptResponses;     // Flag that indicates if the module allows accepting responses

  /** Handler maps */
  Callback<cMessage> selfHandlers;
  Callback<SIMCAN_Message> requestHandlers;
  Callback<SIMCAN_Message> responseHandlers;

  /** Time Series Forecasting object **/
  single_exponential_smoothing<double, FORECASTING_DIMENSION> timeSeriesForecasting;
  es_vec<double, FORECASTING_DIMENSION> currForecastingQuery;
  es_vec<double, FORECASTING_DIMENSION> smthForecastingResult;

  /**
   * Destructor
   */
  virtual ~CloudManagerBase();

  // Base Core functions, documentation will be inherited from super class
  
  virtual void initialize() override;
  virtual void finish() override {}
  virtual cGate *getOutGate(cMessage *msg) override = 0;
  virtual void processSelfMessage(cMessage *msg) override;
  virtual void processRequestMessage(SIMCAN_Message *sm) override;
  virtual void processResponseMessage(SIMCAN_Message *sm) override; 

  /**
   * Get the total cores of a virtual machine type.
   * @param vmType Type of the virtual machine.
   * @return Number of cores.
   */
  int getTotalCoresByVmType(const std::string &vmType);

  /**
   * @brief Calculate the total cores requested by an specific user.
   * @details This method is specially useful in order to check if there exist enough space in the datacentre to handle
   * all the requests of the user.
   * @param vmRequest User VM request.
   * @return Total number of cores requested by the user.
   */
  int calculateTotalCoresRequested(const SM_UserVM *vmRequest);

  /**
   * @brief Creates and schedules a timeout message for a vm request message
   * 
   * @param eventType Type of event (tipically EXEC_VM_RENT_TIMEOUT)
   * @param userId   Owner of the request
   * @param request  vm request
   * @param timeOut  If different than 0, override renting time of vm request (T2)
   * @return SM_UserVM_Finish* Scheduled message
   */
  SM_UserVM_Finish *scheduleVmMsgTimeout(Event eventType, std::string &userId, VM_Request &request, double timeOut = 0.0);

  /**
   * @brief Creates and schedules a timeout message for a given request
   * @details It's used for user subscription timeouts
   * @param eventType Type of event (tipically USER_SUBSCRIPTION_TIMEOUT)
   * @param request   VM request from user
   * @param timeOut   Time out for the event
   * @return SM_UserVM_Finish* Scheduled message
   */
  SM_UserVM_Finish *scheduleVmMsgTimeout(Event eventType, const SM_UserVM * request, double timeOut);

  /**
   * @brief Finds the user type by it's id
   * 
   * @param userId The user id
   * @return CloudUser or nullptr if not found 
   */
  const CloudUser *findUserTypeById(const std::string &userId);

  /**
   * @brief Configures the mapping for auto events (self messages)
   * @details Uses the cMessage kind field for indexing
   */
  virtual void initializeSelfHandlers() = 0;

  /**
   * @brief Configures the mapping for request events
   * @details Uses the SIMCAN_Message operation field for indexing
   */
  virtual void initializeRequestHandlers() = 0;

  /**
   * @brief Configures the mapping for response events
   * @details Uses the SIMCAN_Message result field for indexing
   */
  virtual void initializeResponseHandlers() = 0;
};

#endif
