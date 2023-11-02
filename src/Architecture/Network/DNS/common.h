/**
 * @file common.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Defines and accumulates common concepts of the DNS protocol
 * @version 0.1
 * @date 2023-11-01
 * 
 */

#ifndef SIMCAN_EX_DNS_COMMON
#define SIMCAN_EX_DNS_COMMON

#include "Messages/DNS_Request_m.h"
#include <memory>

namespace dns
{
    /**
     * @brief Commodity struct for extracting information from a DNS domain name
     * @author Ulysses de Aguilar Gudmundsson
     */
    struct DomainName
    {
        uint8_t topLevelIdx;
        std::unique_ptr<const std::string> domainName;

        DomainName(std::string const &domainName)
        {
            // Auto managed unique name reference
            this->domainName = std::unique_ptr<const std::string>(new std::string(domainName));
            std::stringstream stream(domainName);
            std::string token[3], buffer;
            int i;

            // Iterate and fill the plausible fields
            for (i = 0; std::getline(stream,buffer,'.'); i++)
            {
                    if (i < 3)
                        token[i] = buffer;
                    else
                        throw std::invalid_argument("domainName: There are too many subdomains (maximum 1)");
            }

            
            /*for (int j = 0; j < i; j++)
                std::cout << "Token:" << j << " " << token[j] << token[j].length() <<  std::endl;*/

            // If there are 0 tokens or only 1 the domain name is invalid
            if (i <= 1)
                throw std::invalid_argument("domainName: There isn't a structure (missing dots ?)");

            // Search for the top level accumulating the length of the subdomains
            int j = 0;
            i--;
            topLevelIdx = 0;

            for (; j < i; j++)
                topLevelIdx += token[j].length();

            // Add the amount of dots
            topLevelIdx += i-1;
        };
        
        /**
         * @brief Returns the complete domain name
         * 
         * @return std::string const& Constant reference to the string containing the full domain name
         */
        std::string const& getFullName()
        {
            return *(domainName.get());
        }

        /**
         * @brief Get the Top Level Domain Name object
         * 
         * @param str Reference to the string where the TLD name will be written
         */
        void getTopLevelDomainName(std::string &str)
        {
            if (topLevelIdx > 0)
                str = domainName->substr(topLevelIdx);
            else
                str = *domainName.get();
        }
    };
}
#endif