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
#ifndef OPENDNP3_CLEARRESTARTTASK_H
#define OPENDNP3_CLEARRESTARTTASK_H

#include "opendnp3/master/IMasterTask.h"
#include "opendnp3/master/TaskPriority.h"

namespace opendnp3
{

/**
* Clear the IIN restart bit
*/
class ClearRestartTask : public IMasterTask
{	

public:	

	ClearRestartTask(IMasterApplication& application, openpal::TimeDuration retryPeriod, openpal::Logger logger);

	virtual char const* Name() const override final { return "Clear Restart IIN"; }

	virtual bool IsRecurring() const override final { return true; }

	virtual int Priority() const override final { return priority::CLEAR_RESTART; }

	virtual bool BlocksLowerPriority() const override final { return true; }	

	virtual void BuildRequest(APDURequest& request, uint8_t seq) override final;	

	virtual void Schedule() { expiration = 0; }

private:

	virtual MasterTaskType GetTaskType() const override final { return MasterTaskType::USER_TASK; }

	virtual bool IsEnabled() const override final { return true; }

	virtual void _OnResponseTimeout(openpal::MonotonicTimestamp now) override final;

	virtual ResponseResult _OnResponse(const APDUResponseHeader& response, const openpal::ReadBufferView& objects) override final;

	virtual void OnResponseError(openpal::MonotonicTimestamp now) override final;

	virtual void OnResponseOK(openpal::MonotonicTimestamp now) override final;
	
	virtual void _OnLowerLayerClose(openpal::MonotonicTimestamp now) override final;

	openpal::TimeDuration retryPeriod;	
};

} //end ns


#endif
