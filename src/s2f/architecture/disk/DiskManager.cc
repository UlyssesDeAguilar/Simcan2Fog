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

#include "s2f/os/hardwaremanagers/HardwareManager.h"
#include "s2f/os/hypervisors/common.h"

using namespace hypervisor;
Define_Module(DiskManager);

void DiskManager::initialize()
{
    diskSpecs.readBandwidth = par("diskReadBandwidth");
    diskSpecs.writeBandwidth = par("diskWriteBandwidth");
    queueTable.resize(par("numVms"));
    for (auto &entry : queueTable)
        entry.ioFinished.setContextPointer(&entry);
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
        Syscall *request = reinterpret_cast<DiskSyscall *>(entry->queue.pop());

        request->setIsResponse(true);
        request->setResult(OK);
        send(request, gate("diskOut"));

        scheduleIo(*entry);
    }
    else
    {
        auto request = check_and_cast<DiskSyscall *>(msg);
        DiskQueue &entry = queueTable.at(request->getVmId());
        entry.queue.insert(request);

        if (entry.queue.getLength() == 1)
            scheduleIo(entry);
    }
}

void DiskManager::scheduleIo(DiskQueue &entry)
{
    if (entry.queue.getLength() > 0)
    {
        auto head = reinterpret_cast<DiskSyscall *>(entry.queue.front());
        double eta{};

        if (head->getOpCode() == READ)
            eta = head->getBufferSize() / diskSpecs.readBandwidth;
        else
            eta = head->getBufferSize() / diskSpecs.writeBandwidth;

        scheduleAt(simTime() + eta, &entry.ioFinished);
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
