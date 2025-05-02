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

#ifdef WITH_SQL_DATA_ENGINE
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
#include "s2f/core/simdata/engine/DataEngine.h"

namespace s2f
{
    namespace data
    {
        /**
         * @author Ulysses de Aguilar Gudmundsson
         * @brief SimSchema implementation with a MySQL database
         * It's important to note that it expects the password in the variable S2F_DB_PASSWORD
         */
        class MySqlDataEngine : public DataEngine
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
            template <typename T>
            struct DataReturn
            {
                bool ok;
                T value;
            };

            struct UserBaseInfo
            {
                std::string userName;
                std::string slaId;
                uint32_t numInstances;
                tPriorityType priority;
            };
            
            virtual void initialize() override;
            virtual void finish() override;

            DataReturn<UserBaseInfo> searchUserBaseInfo(const std::string &user, const std::string &experiment);
            std::unique_ptr<sql::ResultSet> searchUserApps(const std::string &user);
            std::unique_ptr<sql::ResultSet> searchUserVms(const std::string &user);
            void loadUserAppsAndVms(CloudUserPriority *user);

        public:
            virtual Application *searchApp(const std::string &name) override;
            virtual VirtualMachine *searchVm(const std::string &name) override;
            virtual Sla *searchSla(const std::string &name) override;
            virtual CloudUserPriority *searchUser(const std::string &userType, const std::string &experiment) override;
            virtual void loadUsersInExperiment(const std::string &experiment) override;
        };
    };
};

#endif

#endif /*SIMCAN_EX_SIM_SCHEMA_MYSQL*/
