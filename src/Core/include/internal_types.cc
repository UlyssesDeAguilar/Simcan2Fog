#include "SIMCAN_types.h"

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

ServiceURL::ServiceURL(const std::string &url)
{
	// Search for the @
	size_t npos = url.find('@');

	// If not found
	if (npos == std::string::npos)
	{
		// Attempt parsing
		local.set(url.c_str());
		if (local.getAddressCategory() == LocalAddress::AddressCategory::PRIVATE_NETWORK)
		{
			state = ONLY_LOCAL;
		}
		else
		{
			global.set(local);
			state = ONLY_GLOBAL;
		}
	}
	else
	{
		std::string local_str = url.substr(0, npos);
		// Local address parsing
		local.set(local_str.c_str());
		
		// Global address parsing
		global.tryParse(npos + url.c_str() + 1);
	}
}