/**
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */
#include "PhysicalLayerTCPServer.h"

#include <asio.hpp>

#include <functional>
#include <string>

#include <openpal/logging/LogMacros.h>
#include <openpal/channel/IPhysicalLayerCallbacks.h>
#include <openpal/logging/LogLevels.h>

using namespace asio;
using namespace openpal;
using namespace std;

namespace asiopal
{

PhysicalLayerTCPServer::PhysicalLayerTCPServer(
	openpal::LogRoot& root,
	asio::io_service& service,
    const std::string& endpoint,
    uint16_t port,
    std::function<void (asio::ip::tcp::socket&)> configure_) :

	PhysicalLayerBaseTCP(root, service),
	localEndpointString(endpoint),
	localEndpoint(ip::tcp::v4(), port),
	acceptor(service),
	configure(configure_)
{

}

/* Implement the actions */
void PhysicalLayerTCPServer::DoOpen()
{
	if (!acceptor.is_open())
	{
		std::error_code ec;
		auto address = asio::ip::address::from_string(localEndpointString, ec);
		if (ec)
		{
			auto lambda = [this, ec]() { this->OnOpenCallback(ec); };
			pExecutor->PostLambda(lambda);
		}
		else
		{
			localEndpoint.address(address);
			acceptor.open(localEndpoint.protocol(), ec);
			if (ec)
			{
				auto lambda = [this, ec]() { this->OnOpenCallback(ec); };
				pExecutor->PostLambda(lambda);
			}
			else
			{
				acceptor.set_option(ip::tcp::acceptor::reuse_address(true));
				acceptor.bind(localEndpoint, ec);
				if (ec)
				{
					auto lambda = [this, ec]() { this->OnOpenCallback(ec); };
					pExecutor->PostLambda(lambda);
				}
				else
				{
					acceptor.listen(socket_base::max_connections, ec);
					if (ec)
					{
						auto lambda = [this, ec]() { this->OnOpenCallback(ec); };
						pExecutor->PostLambda(lambda);
					}
					else
					{
						auto callback = [this](const std::error_code & code) { this->OnOpenCallback(code); };
						acceptor.async_accept(socket, remoteEndpoint, executor.strand.wrap(callback));
					}
				}
			}
		}
	}
	else
	{
		auto callback = [this](const std::error_code & code) { this->OnOpenCallback(code); };
		acceptor.async_accept(socket, remoteEndpoint, executor.strand.wrap(callback));
	}
}

void PhysicalLayerTCPServer::CloseAcceptor()
{
	std::error_code ec;
	acceptor.close(ec);
	if(ec)
	{
		FORMAT_LOG_BLOCK(logger, logflags::WARN, "Error while closing tcp acceptor: %s", ec.message().c_str());
	}
}

void PhysicalLayerTCPServer::DoOpenCallback()
{
	this->CloseAcceptor();
}

void PhysicalLayerTCPServer::DoOpeningClose()
{
	this->CloseAcceptor();
}

void PhysicalLayerTCPServer::DoOpenSuccess()
{
	FORMAT_LOG_BLOCK(logger, logflags::INFO, "Accepted connection from: %s", remoteEndpoint.address().to_string().c_str());
	configure(socket);
}

}


