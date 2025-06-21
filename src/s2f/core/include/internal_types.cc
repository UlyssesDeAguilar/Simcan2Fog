#include "s2f/core/include/SIMCAN_types.h"

std::ostream &operator<<(std::ostream &os, tApplicationState s)
{
	switch (s)
	{
	case appWaiting:
		os << "appWaiting";
		break;
	case appRunning:
		os << "appRunning";
		break;
	case appFinishedOK:
		os << "appFinishedOK";
		break;
	case appFinishedTimeout:
		os << "appFinishedTimeout";
		break;
	case appFinishedError:
		os << "appFinishedError";
		break;
	default:
		os.setstate(std::ios_base::failbit);
	}
	return os;
}