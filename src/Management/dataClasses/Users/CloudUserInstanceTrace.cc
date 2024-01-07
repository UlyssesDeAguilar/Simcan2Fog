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

#include "CloudUserInstanceTrace.h"

CloudUserInstanceTrace::CloudUserInstanceTrace(Job_t jobIn, int totalUserInstance, int nCurrentNumber, int nUserInstance, int nTotalInstances, Application *ptrApp)
    : CloudUserInstancePriority(nullptr, totalUserInstance, nCurrentNumber, nUserInstance, nTotalInstances)
{
    UserVmReference *vmReference;
    int nRentTime, nInstances;
    VirtualMachine *vmBase;
    std::ostringstream osStream;

    // TODO: Aqui para simplificar hemos creado, 1 job - 1 usuario
    // Lo suyo seria crear un usuario con todos los jobs dentro

    // Generate userID
    osStream << "(-1)"
             << "UserTrace"
             << "[" << (nUserInstance + 1) << "/" << nTotalInstances << "]";
    id = osStream.str();
    type = "UserTrace";

    // userID = std::to_string(jobIn.id);

    if (jobIn.runtime > 3600)
        nRentTime = jobIn.runtime / 3600;
    else
        nRentTime = 1;

    // Include VM instances
    // for (currentVm = 0; currentVm < jobIn.reqsize; currentVm++){
    // for (currentVm = 0; currentVm < 2; currentVm++){

    // TODO: Aqui se deberia hacer un matching entre las vMs que oferta el servidor
    // y lo que requiere el job, but unfortunately... a fuego
    nInstances = jobIn.size;
    // if (nInstances>500) nInstances = 500;
    vmBase = new VirtualMachine("VM_small", 10, 1, 1.0, 250.0, 2.0);
    vmReference = new UserVmReference(vmBase, nInstances, nRentTime);

    // Insert a new collection of application instances
    // hardcoded for 1024cores
    // TODO: configure with num of architecture cores and log it.

    insertNewVirtualMachineInstances(vmReference->getVmBase(), nInstances, vmReference->getRentTime(), nInstances, 0);
    numTotalVMs += nInstances;
    //}

    // TODO:
    AppInstanceCollection *newAppCollection;

    newAppCollection = new AppInstanceCollection(ptrApp, this->id, 1);
    applications.push_back(newAppCollection);
    appInstances.push_back(new AppInstance("AppCPUIntensive", 0, 1, id));

    /* FIXME: All of this is actually duplicated -- Done in CloudUserInstance!

    requestVmMsg = nullptr;
    requestAppMsg = nullptr;
    subscribeVmMsg = nullptr;

    bTimeout_t2 = bTimeout_t4 = false;
    setInitTime(0.0);
    setEndTime(0.0);
    */
}
