/**
 * @file SimSchema.cpp
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Defines the module that implements the SimSchema interface with a MySQL database
 * @version 0.1
 * @date 2023-12-28
 *
 */

#ifndef SIMCAN_EX_SIM_SCHEMA_MYSQL
#define SIMCAN_EX_SIM_SCHEMA_MYSQL

// Interface contract
#include <cstdlib>

// Allow lambdas
#include <functional>

// OMNeT++ framework
#include <omnetpp.h>

// MySQL connector for talking with the DB
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include "s2f/core/include/json.hpp"
#include "s2f/core/simdata/SimSchema.h"

namespace simschema
{
    /**
     * @author Ulysses de Aguilar Gudmundsson
     * @brief SimSchema implementation with a MySQL database
     * It's important to note that it expects the password in the variable S2F_DB_PASSWORD
     */
    class SimSchemaMySQL : public SimSchema, public omnetpp::cSimpleModule
    {
    private:
        enum Queries
        {
            APP_DATA,
            VM_DATA,
            SLA_DATA,
            VM_COST,
            USER_BASE_INFO,
            SIM_USERS,
            APP_INFO,
            VM_INFO,
            NUM_QUERIES
        };

        typedef std::array<std::string, Queries::NUM_QUERIES> QueryDefinitionArray;
        typedef std::array<sql::PreparedStatement *, Queries::NUM_QUERIES> QueryArray;

        std::string hostName;                                // Host of the DB, i.e "tcp://127.0.0.1:3306"
        std::string userName;                                // Name of the DB user
        sql::Driver *driver;                                 // Shall not be deleted (MySQLConnector responsibility)
        std::unique_ptr<sql::Connection> connection;         // Should be kept alive for performance reasons
        QueryArray queries;                                  // Prepared statements for reuse
        const static QueryDefinitionArray queriesDefinition; // The definition of the queries

        /**
         * @brief Erases the prepared statements
         */
        void deleteStatements();

        /**
         * @brief Create or reload prepared statements
         */
        void createStatements();

        /**
         * @brief Attempt reconnection with database
         * @details Should exclusively be done when connection->isValid() is False
         */
        void attemptReconnect();

        /**
         * @brief Executes the selected statement
         *
         * @param selectedQuery The statement to be executed
         * @param binder Function that will bind the arguments for the statement (tipically a lambda)
         * @return ResultSet or nullptr if there was an error
         */
        std::unique_ptr<sql::ResultSet>
        executeStatement(Queries selectedQuery, std::function<void(sql::PreparedStatement *)> binder);

    protected:
        virtual void initialize() override;
        virtual void finish() override;
        virtual void handleMessage(omnetpp::cMessage *msg) override
        {
            delete msg;
            error("This module doesn't receive messages");
        }

    public:
        virtual std::unique_ptr<Application>
        searchApp(const std::string &name) override;

        virtual std::unique_ptr<VirtualMachine>
        searchVirtualMachine(const std::string &name) override;

        virtual std::unique_ptr<Sla>
        searchSLA(const std::string &name) override;

        virtual std::unique_ptr<Sla::VMCost>
        searchVMCost(const std::string &sla, const std::string &vmType) override;

        virtual DataList<UserBaseInfo>
        searchUsersInExperiment(const std::string &experiment) override;

        virtual std::unique_ptr<UserBaseInfo>
        searchUserBaseInfo(const std::string &user, const std::string &experiment) override;

        virtual DataList<UserAppInfo>
        searchUserApps(const std::string &user) override;

        virtual DataList<UserVmInfo>
        searchUserVms(const std::string &user) override;
    };
};

#endif /*SIMCAN_EX_SIM_SCHEMA_MYSQL*/
