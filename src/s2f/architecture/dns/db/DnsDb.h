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

#ifndef SIMCAN_EX_DNS_DB_H_
#define SIMCAN_EX_DNS_DB_H_

#include <omnetpp.h>
#include "s2f/architecture/dns/DnsCommon.h"
#include "s2f/architecture/dns/db/DnsTree.h"

namespace s2f
{
    namespace dns
    {

        /**
         * @brief Simulation model that offers DNS database services
         *
         * @author Ulysses de Aguilar Gudmundsson
         * @version 1.0
         */
        class DnsDb : public omnetpp::cSimpleModule
        {
        protected:
            DnsTree tree; //!< Tree-like data structure that holds the DNS zones and records

        public:
            // Kernel lifecycle
            virtual void initialize() override;
            virtual void finish() override { tree.clear(); };
            virtual void handleMessage(omnetpp::cMessage *msg) override { error("This module doesn't take any messages"); }

            /**
             * @brief Inserts a DNS record into a given DNS zone
             *
             * @param zone Zone to insert the record
             * @param record The record to be inserted
             */
            void insertRecord(const char *zone, const ResourceRecord &record);

            /**
             * @brief Removes a DNS record from a given DNS zone
             *
             * @param zone Zone to remove the record
             * @param record The record to be removed
             */
            void removeRecord(const char *zone, const ResourceRecord &record);

            /**
             * @brief Searches for DNS records that match a given DNS question
             * @details This method provides a best effort implementation, it returns
             * the first node in the tree that matches the question either partially or fully
             *
             * @param question DNS question
             * @return const DnsTreeNode* The node that matches the question
             */
            const DnsTreeNode *searchRecords(const DnsQuestion &question);
        };
    }
}

#endif
