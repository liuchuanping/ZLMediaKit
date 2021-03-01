/*
 * Copyright (c) 2016 The ZLMediaKit project authors. All Rights Reserved.
 *
 * This file is part of ZLMediaKit(https://github.com/xia-chu/ZLMediaKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#include "HeartbeatManager.h"

namespace mediakit {

	HeartbeatManager::HeartbeatManager() {
		_isRunning = true;
	}
	HeartbeatManager::~HeartbeatManager() {}

	INSTANCE_IMP(HeartbeatManager);

	void HeartbeatManager::regist(const string kBroadcast,float second,const function<bool()>& cb,const EventPoller::Ptr& poller,bool continueWhenException) {
		//创建超时管理定时器
		if (second > 0 && !kBroadcast.empty()) {
			Timer::Ptr& ref = _timer_map[kBroadcast];
			auto isRunning = _isRunning;
			ref = std::make_shared<Timer>(second, [isRunning,cb]() {
				if (isRunning) {
					return cb();
				}else {
					return false;
				}
			}, poller, continueWhenException);
		}
	}

	void HeartbeatManager::stop() {
		_isRunning = false;
		_timer_map.clear();
	}

}