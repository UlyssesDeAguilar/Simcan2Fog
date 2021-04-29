//##############################################################################
//        file: est_swf_job.hh
//      writer: Dan Tsafrir
// description: the structure of an SWF-record and how to read/write it.
//##############################################################################
#ifndef EST_SWF_JOB_HH__
#define EST_SWF_JOB_HH__



//##############################################################################
// include files
//##############################################################################
#include <iomanip>
#include <stdio.h>



//##############################################################################
// representing one SWFfile job record
// [ see: http://www.cs.huji.ac.il/labs/parallel/workload/swf.html ]
//##############################################################################
struct Job_t {

    //--------------------------------------------------------------------------
    // record's structure
    //--------------------------------------------------------------------------
    int		id;		//  1 job id
    double	submit;		//  2 submit time
    double	wait;		//  3 wait time
    double	runtime;	//  4 runtime
    int		size;		//  5 number of allocated processors
    double	cpu;		//  6 cpu time used
    double	mem;		//  7 memory used
    int		reqsize;	//  8 requested number of processors
    double	estimate;	//  9 requested time
    double	reqmem;		// 10 requested memory
    short	status;		// 11 status
    int		uid;		// 12 submitter's user id
    int		gid;		// 13 submitter's group id
    int		executable;	// 14 application number
    int		queueid;	// 15 queue number
    int		partition;	// 16 partition number
    int		depjob;		// 17 current job can only start after this job
    double	think;		// 18 think time

    //--------------------------------------------------------------------------
    // parse the above from an SWF line. return true only upon success.
    //--------------------------------------------------------------------------
    bool read(const char* line) {
	return 18 ==
	    sscanf( line,
		    "%d"	//  1 job id
		    " %lf"	//  2 submit time
		    " %lf"	//  3 wait time
		    " %lf"	//  4 runtime
		    " %d"	//  5 number of allocated processors
		    " %lf"	//  6 cpu time used
		    " %lf"	//  7 memory used
		    " %d"	//  8 requested number of processors
		    " %lf"	//  9 requested time
		    " %lf"	// 10 requested memory
		    " %hd"	// 11 status
		    " %d"	// 12 submitter's user id
		    " %d"	// 13 submitter's group id
		    " %d"	// 14 application number
		    " %d"	// 15 queue number
		    " %d"	// 16 partition number
		    " %d"	// 17 current job can only start after this job
		    " %lf",	// 18 think time
		    &id, &submit, &wait, &runtime, &size, &cpu, &mem, &reqsize,
		    &estimate, &reqmem, &status, &uid, &gid, &executable,
		    &queueid, &partition, &depjob, &think);
    }

    //--------------------------------------------------------------------------
    // write the above as an SWF line.
    // [precision = number of digits aftet the floating point for double fields
    //--------------------------------------------------------------------------
    void write(std::ostream& out, int precision=0) {

	using namespace std;
	out << setprecision(precision);
	out << fixed;
	int w = ( precision > 0 ) ?  8 + precision : 7;
	
	out << setw(6) << id 			//  1
	    << " " << setw(w+2) << submit 	//  2
	    << " " << setw(w) << wait		//  3
	    << " " << setw(w) << runtime	//  4
	    << " " << setw(4) << size		//  5
	    << " " << setw(w) << cpu		//  6
	    << " " << setw(w) << mem		//  7
	    << " " << setw(4) << reqsize	//  8
	    << " " << setw(w) << estimate	//  9
	    << " " << setw(w) << reqmem		// 10
	    << " " << setw(1) << status		// 11
	    << " " << setw(4) << uid		// 12
	    << " " << setw(4) << gid		// 13
	    << " " << setw(4) << executable	// 14
	    << " " << setw(2) << queueid	// 15
	    << " " << setw(2) << partition	// 16
	    << " " << setw(6) << depjob		// 17
	    << " " << setw(w) << think		// 18
	    << "\n";
    }
};



#endif /*EST_SWF_JOB_HH__*/
//##############################################################################
//                                   EOF
//##############################################################################
