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

#include "UserListParser.h"

UserListParser::UserListParser(const char *s, std::vector<VirtualMachine*> *vmTypesVector, std::vector<Application*> *appTypesVector) : Parser<CloudUser>(s) {
    vmTypes = vmTypesVector;
    appTypes = appTypesVector;
    // Init...
    result = SC_OK;
    numUsers = currentUser = numUserInstances = 0;
    numApps = currentApp = numAppInstances = 0;
    numVMs = currentVM = numVmInstances = nRentTime = 0;
}

UserListParser::~UserListParser() {

}

int UserListParser::parse() {
        cStringTokenizer tokenizer(parseString);

        // First token, number of users
        if (tokenizer.hasMoreTokens())
          {
            const char *numUsersChr = tokenizer.nextToken();
            numUsers = atoi(numUsersChr);
          }
        else
          {
            EV_ERROR << "Cannot read the first token (number of users)" << endl;
            result = SC_ERROR;
          }

        // For each user in the list...
        while ((tokenizer.hasMoreTokens()) && (currentUser < numUsers) && (result == SC_OK))
          {
            // Get the user type
            if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
              {
                const char *userTypeChr = tokenizer.nextToken();
                userTypeStr = userTypeChr;
              }
            else
              {
                EV_ERROR << "Cannot read the user name for user:" << currentUser << endl;
                result = SC_ERROR;
              }

            // Get the number of user instances
            if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
              {
               const char *numUserInstancesChr = tokenizer.nextToken();
               numUserInstances = atoi(numUserInstancesChr);
              }
            else
              {
               EV_ERROR << "Cannot read the number of user instances for user:" << userTypeStr << endl;
               result = SC_ERROR;
              }

            // Get the number of apps for the current user
            if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
              {
                const char *numAppsChr = tokenizer.nextToken();
                numApps = atoi(numAppsChr);
              }
            else
              {
                EV_ERROR << "Cannot read the number of applications for user:" << userTypeStr << endl;
                result = SC_ERROR;
              }


            // Parsing applications for current user...
            if (result == SC_OK)
              {
                // Init...
                currentApp = 0;

                // Create current user instance
                currentUserObject = new CloudUser(userTypeStr, numUserInstances);

                // Include each application
                while ((currentApp < numApps) && (result == SC_OK))
                  {
                    // Get the app name
                    if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
                      {
                        const char *appNameChr = tokenizer.nextToken();
                        appNameStr = appNameChr;
                      }
                    else
                      {
                        EV_ERROR << "Cannot read the app name[" << currentApp << "] for user:" << userTypeStr << endl;
                        result = SC_ERROR;
                      }

                    // Get the number of app instances
                    if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
                      {
                        const char *numAppInstancesChr = tokenizer.nextToken();
                        numAppInstances = atoi(numAppInstancesChr);
                      }
                    else
                      {
                        EV_ERROR << "Cannot read the number of instances for app [" << currentApp << "] in user:" << userTypeStr << endl;
                        result = SC_ERROR;
                      }

                    // Locate the AppInstance in the vector
                    appPtr = findApplication (appNameStr);

                    // App found!
                    if (appPtr != nullptr)
                      {
                        currentUserObject->insertApplication(appPtr, numAppInstances);
                      }
                    else
                      {
                        EV_ERROR << "Application not found [" << appNameStr << "] in user:" << userTypeStr << endl;
                        result = SC_ERROR;
                      }

                    // Next app
                    currentApp++;
                  }
              }

            // Get the number of VMs for the current user
            if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
              {
                const char *numVMsChr = tokenizer.nextToken();
                numVMs = atoi(numVMsChr);
              }
            else
              {
                EV_ERROR << "Cannot read the number of VMs for user:" << userTypeStr << endl;
                result = SC_ERROR;
              }


            // Parsing VMs for current user...
            if (result == SC_OK)
              {
                // Init...
                currentVM = 0;

                // Include each VMs
                while ((currentVM < numVMs) && (result == SC_OK))
                  {

                    // Get the VM name
                    if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
                      {
                        const char *vmNameChr = tokenizer.nextToken();
                        vmNameStr = vmNameChr;
                      }
                    else
                      {
                        EV_ERROR << "Cannot read the VM name[" << currentVM << "] for user:" << userTypeStr << endl;
                        result = SC_ERROR;
                      }

                    // Get the number of VMs instances
                    if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
                      {
                        const char *numVmInstancesChr = tokenizer.nextToken();
                        numVmInstances = atoi(numVmInstancesChr);
                      }
                    else
                      {
                        EV_ERROR << "Cannot read the number of instances for VM [" << currentVM << "] in user:" << userTypeStr << endl;
                        result = SC_ERROR;
                      }

                    // Get the rental of VMs instances
                    if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
                      {
                        const char *nRentTimeChr = tokenizer.nextToken();
                        nRentTime = atoi(nRentTimeChr);
                      }
                    else
                      {
                        EV_ERROR << "Cannot read the rental time of instances for VM [" << currentVM << "] in user:" << userTypeStr << endl;
                        result = SC_ERROR;
                      }

                    // Locate the VM in the vector
                    vmPtr = findVirtualMachine (vmNameStr);

                    // VMType found!
                    if (vmPtr != nullptr)
                      {
                        currentUserObject->insertVirtualMachine(vmPtr, numVmInstances, nRentTime);
                      }
                    else
                      {
                        EV_ERROR << "Virtual Machine not found [" << vmNameStr << "] in user:" << userTypeStr << endl;
                        result = SC_ERROR;
                      }

                    // Next vm
                    currentVM++;
                  }
              }

            // Include user in the vector
            parseResult.push_back(currentUserObject);

            // Process next user
            currentUser++;
          }

    return result;
}

Application* UserListParser::findApplication (std::string appName) {
    // Init
    Application *result = nullptr;
    std::vector<Application*>::iterator it = appTypes->begin();

    // Search...
    while (it != appTypes->end())
      {
        if ((*it)->getName().compare(appName) == 0)
          {
            result = *it;
            break;
          }

        it++;
      }

    return result;
}


VirtualMachine* UserListParser::findVirtualMachine (std::string vmType) {
    // Init
    VirtualMachine *result = nullptr;
    std::vector<VirtualMachine*>::iterator it = vmTypes->begin();

    // Search...
    while (it != vmTypes->end())
      {

        if ((*it)->getType().compare(vmType) == 0)
          {
            result = (*it);
            break;
          }

        it++;
      }

    return result;
}

