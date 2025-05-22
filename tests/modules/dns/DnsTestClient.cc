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

#include "DnsTestClient.h"
#include "s2f/architecture/dns/client/DnsClientCommand_m.h"
#include "s2f/architecture/dns/DnsCommon.h"

using namespace dns;

using namespace s2f::tests;

Define_Module(DnsTestClient);

void DnsTestClient::initialize()
{
	cValueArray *domainArray = check_and_cast<cValueArray *>(par("domainList").objectValue());
	for (const auto &domain : domainArray->asStringVector())
	{
		EV_INFO << "Domain to resolve: " << domain << "\n";
		domainsToResolve.push_back(domain);
	}

	cValueArray *targetDnsArray = check_and_cast<cValueArray *>(par("targetDnsList").objectValue());
	for (const auto &dnsIp : targetDnsArray->asStringVector())
	{
		EV_INFO << "Target DNS: " << dnsIp << "\n";
		dnsServers.push_back(L3Address(dnsIp.c_str()));
	}

	if (domainsToResolve.empty())
	{
		EV_WARN << "No domains to resolve\n";
		return;
	}

	if (dnsServers.empty())
	{
		EV_WARN << "No target DNS servers\n";
		return;
	}

	prepareAndSendNextResolution();
}

void DnsTestClient::finish()
{
	EV_INFO << "Finished\n";
	dnsServers.clear();
	domainsToResolve.clear();
}

void DnsTestClient::prepareAndSendNextResolution()
{
	// Create the question
	DnsQuestion question;
	question.setDomain(domainsToResolve[requestId].c_str());
	question.setQuestionType(RecordType::ANY);

	// DNS request
	auto packet = new Packet("DNS request");
	auto request = makeShared<DnsRequest>();
	request->setOperationCode(QUERY);
	request->setRequestId(requestId);
	request->appendQuestion(question);
	packet->insertData(request);

	// Command to send to the client
	auto command = new DnsClientCommand();
	command->setRequestId(requestId);
	command->setPayload(packet);
	for (auto &ip : dnsServers)
		command->appendIpPool(ip);

	send(command, "clientOut");
	requestId++;
}
void DnsTestClient::handleMessage(cMessage *msg)
{
	auto indication = check_and_cast<DnsClientIndication *>(msg);
	EV_INFO << "Response for " << indication->getRequestId() << " received, status is "
			<< ((indication->getResult() != 0) ? "ERROR" : "OK") << "\n";
	if (indication->getResult() == 0)
	{
		const auto &payload = indication->getPayload();
		const auto reply = payload->peekData<DnsRequest>();

		size_t authCount = reply->getAuthoritativeAnswerArraySize();
		size_t nonAuthCount = reply->getNonAuthoritativeAnswerArraySize();
		EV_INFO << "Return code: " << std::to_string(reply->getReturnCode()) << "\n";
		EV_INFO << "Authoritative answer count: " << authCount << "\n";
		EV_INFO << "Non authoritative answer count: " << nonAuthCount << "\n";

		for (size_t i = 0; i < authCount; i++)
		{
			const auto &answer = reply->getAuthoritativeAnswer(i);
			EV_INFO << "Answer (auth) => domain: " << answer.domain << ", type: " << answer.type
					<< ", ip: " << answer.ip << ", text: " << answer.contents << "\n";
		}

		for (size_t i = 0; i < nonAuthCount; i++)
		{
			const auto &answer = reply->getNonAuthoritativeAnswer(i);
			EV_INFO << "Answer (non auth) => domain: " << answer.domain << ", type: " << answer.type
					<< ", ip: " << answer.ip << ", text: " << answer.contents << "\n";
		}
	}

	delete msg;

	if (requestId > domainsToResolve.size() - 1)
		return;
	prepareAndSendNextResolution();
}
