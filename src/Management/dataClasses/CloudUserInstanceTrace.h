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

#ifndef CLOUDUSERINSTANCETRACE_H_
#define CLOUDUSERINSTANCETRACE_H_

#include "CloudUserInstancePriority.h"
#include "Management/traces/est_swf_job.hh"
#include "Application.h"
class CloudUserInstanceTrace: public CloudUserInstancePriority {

public:
    CloudUserInstanceTrace(Job_t jobIn, int totalUserInstance, int nCurrentNumber, int nUserInstance, int nTotalInstances, Application* ptrApp);
    void setJob(Job_t jobIn){job=jobIn;};
    Job_t getJob(){return job;};
protected:
    Job_t job;
};

#endif /* CLOUDUSERINSTANCETRACE_H_ */
