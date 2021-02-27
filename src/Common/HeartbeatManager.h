/*
 * Copyright (c) 2016 The ZLMediaKit project authors. All Rights Reserved.
 *
 * This file is part of ZLMediaKit(https://github.com/xia-chu/ZLMediaKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */


#ifndef HEARTBEAT_MANAGER_H
#define HEARTBEAT_MANAGER_H

#include <string>
#include <functional>
#include <unordered_map>
#include <cstdlib>
#include "Util/util.h"
#include "Poller/Timer.h"
#include "Poller/EventPoller.h"

using namespace std;
using namespace toolkit;

namespace mediakit {

class HeartbeatManager : public std::enable_shared_from_this<HeartbeatManager> {
public:
	HeartbeatManager();
	~HeartbeatManager();

	static HeartbeatManager& Instance();

	void regist(const string kBroadcast,
		float second,
		const function<bool()>& cb,
		const EventPoller::Ptr& poller,
		bool continueWhenException = true);

	void stop();
private:
	bool _isRunning;
	std::unordered_map<string, Timer::Ptr> _timer_map;
};

}  // namespace mediakit
#endif /* HEARTBEAT_MANAGER_H */