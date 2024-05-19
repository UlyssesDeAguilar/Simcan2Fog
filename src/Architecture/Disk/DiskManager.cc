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

#include "DiskManager.h"
#include "Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.h"
#include "OperatingSystem/Hypervisors/common.h"

Define_Module(DiskManager);

void DiskManager::initialize(int stage)
{
    switch (stage)
    {
    case LOCAL:
    {
        auto hwm = check_and_cast<HardwareManager *>(getParentModule()->getSubmodule("hardwareManager"));

        queueTable.resize(hwm->getTotalResources().vms);
        specs = hwm->getDiskSpecs();

        for (auto &entry : queueTable)
            entry.ioFinished.setContextPointer(&entry);
        break;
    }
    default:
        break;
    }
}

void DiskManager::finish()
{
    for (auto &entry : queueTable)
        flushQueue(entry);
}

void DiskManager::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        DiskQueue *entry = reinterpret_cast<DiskQueue *>(msg->getContextPointer());
        SM_Syscall *request = reinterpret_cast<SM_Syscall *>(entry->queue.pop());

        request->setIsResponse(true);
        request->setResult(SC_OK);
        send(request, gate("diskOut"));

        scheduleIo(*entry);
    }
    else
    {
        auto request = check_and_cast<SM_Syscall *>(msg);
        auto appId = check_and_cast<AppIdLabel *>(request->getControlInfo());
        DiskQueue &entry = queueTable.at(appId->getVmId());
        entry.queue.insert(request);

        if (entry.queue.getLength() == 1)
            scheduleIo(entry);
    }
}

void DiskManager::scheduleIo(DiskQueue &entry)
{
    if (entry.queue.getLength() > 0)
    {
        auto head = reinterpret_cast<SM_Syscall *>(entry.queue.front());
        simtime_t eta;

        if (head->getContext().opCode == Syscall::READ)
            eta = head->getContext().data.bufferSize / specs.readBandwidth;
        else
            eta = head->getContext().data.bufferSize / specs.writeBandwidth;

        scheduleAt(eta, &entry.ioFinished);
    }
}

void DiskManager::flushQueue(DiskQueue &entry)
{
    // Cancel the IO event
    if (entry.ioFinished.isScheduled())
        cancelEvent(&entry.ioFinished);

    // Clear the queue
    entry.queue.clear();
}

void DiskManager::stopVmQueue(uint32_t vmId)
{
    Enter_Method_Silent();
    flushQueue(queueTable.at(vmId));
}