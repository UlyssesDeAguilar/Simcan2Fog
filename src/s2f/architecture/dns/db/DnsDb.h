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

namespace dns
{

    class DnsDb : public omnetpp::cSimpleModule
    {
    protected:
        DnsTree tree;

    public:
        // Kernel lifecycle
        virtual void initialize() override;
        virtual void finish() override { tree.clear(); };
        virtual void handleMessage(omnetpp::cMessage *msg) override { error("This module doesn't take any messages"); }

        void insertRecord(const char *zone, const ResourceRecord &record);
        void removeRecord(const char *zone, const ResourceRecord &record);
        const DnsTreeNode *searchRecords(const DnsQuestion &question);
    };

};

#endif
