#ifndef SIMCAN_EX_DNS_TREE
#define SIMCAN_EX_DNS_TREE

#include "s2f/architecture/dns/DnsCommon.h"

namespace s2f
{
    namespace dns
    {
        constexpr int NUM_DNS_LEVELS = 3; //!< Number of DNS levels

        /**
         * @brief Function that matches the DnsLevel enum to a string
         *
         * @param level The level
         * @return const char* The string representation
         */
        static const char *dnsLevelToString(DnsLevel level)
        {
            static const char *levels[] = {"ROOT", "TLD", "AUTHORITATIVE"};
            return levels[level];
        }

        /**
         * @brief Class that represents DNS tree node
         *
         * @author Ulysses de Aguilar Gudmundsson
         * @version 1.0
         */
        class DnsTreeNode
        {
        protected:
            omnetpp::opp_string domain;                          //!< Domain/Subdomain that the node represents
            DnsTreeNode *parent;                                 //!< Parent node, nullptr if it is the root
            DnsLevel level;                                      //!< DNS level: ROOT, TLD, AUTHORITATIVE
            std::map<omnetpp::opp_string, DnsTreeNode> children; //!< Child nodes
            std::set<ResourceRecord> records;                    //!< DNS records in the zone that this node represents

        public:
            /**
             * @brief Construct a new Dns Tree Node object
             * @param domain Domain/Subdomain that the node represents
             * @param level DNS level: ROOT, TLD, AUTHORITATIVE
             * @param parent Parent node, nullptr if it is the root
             */
            DnsTreeNode(const char *domain, DnsLevel level, DnsTreeNode *parent) : domain(domain), parent(parent), level(level) {}

            DnsTreeNode() : DnsTreeNode("", ROOT, nullptr) {}

            virtual ~DnsTreeNode() = default;

            /**
             * @brief Insert a child node
             * @param node The child node
             */
            void insertElement(DnsTreeNode *node) { children[node->getDomain()] = *node; }

            /**
             * @brief Get the corresponding child node
             * @param domain Subdomain of the child
             * @return DnsTreeNode* The child node or nullptr if not found
             */
            DnsTreeNode *getChild(const char *domain);

            /**
             * @brief Get the corresponding child node, or creates a new child
             * @param domain Subdomain of the child
             * @return DnsTreeNode* The found or created node
             */
            DnsTreeNode *getChildOrCreate(const char *domain);

            /**
             * @brief Get the number of children
             * @return int The number of children
             */
            int getChildCount() const { return children.size(); }

            /**
             * @brief Get the domain
             * @return const char* The domain
             */
            const char *getDomain() const { return domain.c_str(); }

            /**
             * @brief Get the DNS level
             * @return DnsLevel The DNS level
             */
            DnsLevel getDnsLevel() const { return level; }

            /**
             * @brief Get the parent node
             * @return DnsTreeNode* The parent node
             */
            const DnsTreeNode *getParent() const { return parent; }

            /**
             * @brief Get the child nodes
             * @return const std::map<omnetpp::opp_string, DnsTreeNode>& The child nodes
             */
            const std::map<omnetpp::opp_string, DnsTreeNode> &getChildren() const { return children; }

            /**
             * @brief Get the records
             * @return std::set<ResourceRecord>* The records
             */
            std::set<ResourceRecord> *getRecords() { return records.size() > 0 ? &records : nullptr; }

            /**
             * @brief Get the records
             * @return const std::set<ResourceRecord>* The records
             */
            const std::set<ResourceRecord> *getRecords() const { return records.size() > 0 ? &records : nullptr; }

            /**
             * @brief Add a record
             * @param record The record
             */
            void addRecord(const ResourceRecord *record) { records.insert(*record); }

            /**
             * @brief Print the node
             * @param out The output stream
             * @param level The level of the node, used for indentation
             * @return std::ostream& The output stream
             */
            std::ostream &print(std::ostream &out, int level = 0) const;

            /**
             * @brief Clear the node
             */
            void clear();

            friend std::ostream &operator<<(std::ostream &out, const DnsTreeNode &node);
        };

        /**
         * @brief DNS Tree
         *
         * @author Ulysses de Aguilar Gudmundsson
         * @version 1.0
         */
        class DnsTree
        {
        protected:
            DnsTreeNode root;
            DnsTreeNode *searchRecordsForUpdate(const char *domain);

        public:
            /**
             * @brief Constructor
             */
            DnsTree();

            /**
             * @brief Destructor
             */
            virtual ~DnsTree() = default;

            /**
             * @brief Insert a DNS record
             *
             * @param zone Zone to insert the record
             * @param record The record to be inserted
             */
            void insertRecord(const char *zone, const ResourceRecord *record);

            /**
             * @brief Remove a DNS record
             *
             * @param zone Zone to remove the record
             * @param record The record to be removed
             */
            bool removeRecord(const char *zone, const ResourceRecord *record);

            /**
             * @brief Search for DNS records
             * @details This search is a best effort. It returns a node that matches
             * partially or completely the domain
             *
             * @param domain Domain to search for
             * @return const DnsTreeNode* The found node
             * @return nullptr if not found
             */
            const DnsTreeNode *searchRecords(const char *domain) { return static_cast<const DnsTreeNode *>(searchRecordsForUpdate(domain)); }

            /**
             * @brief Clear the tree
             */
            void clear();

            friend std::ostream &operator<<(std::ostream &os, const DnsTree &tree) { return tree.root.print(os); }
        };
    }
}

#endif // SIMCAN_EX_DNS_TREE