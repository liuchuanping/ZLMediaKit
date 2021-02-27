/*
 * Copyright (c) 2016 The ZLMediaKit project authors. All Rights Reserved.
 *
 * This file is part of ZLMediaKit(https://github.com/xia-chu/ZLMediaKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#if defined(ENABLE_RTPPROXY)

#include "RtpChannel.h"

namespace mediakit {

	RtpChannel::RtpChannel(const uint32_t& ssrc, const string& stream_id) {
		_ssrc = ssrc;
		_stream_id = stream_id;
	}
	RtpChannel::~RtpChannel() {}

	string RtpChannel::getStreamId() {
		return _stream_id;
	}
	uint32_t RtpChannel::getSsrc(){
		return _ssrc;
	}

	void RtpChannel::setOnDetach(const function<void()>& cb) {
		RtpProcess::Ptr process;
		if (!_stream_id.empty()) {
			//指定了流id，那么一个端口一个流(不管是否包含多个ssrc的多个流，绑定rtp源后，会筛选掉ip端口不匹配的流)
			process = RtpSelector::Instance().getProcess(_stream_id, true);
			process->setOnDetach(cb);
		}
		_rtp_process = process;
	}

	RtpChannelSelecter::RtpChannelSelecter() {}
	RtpChannelSelecter::~RtpChannelSelecter() {}

	INSTANCE_IMP(RtpChannelSelecter);

	RtpChannel::Ptr RtpChannelSelecter::regist(bool isReal, const string& domain,const string& stream_id, const function<void()>& cb) {
        lock_guard<decltype(_mtx_map)> lck(_mtx_map);
		uint32_t ssrc = SsrcManager::Instance().getSSRC(isReal, domain);
		if (ssrc > 0) {
			RtpChannel::Ptr& stream_id_ref = _stream_id_map_rtp_channel[stream_id];
			RtpChannel::Ptr& ssrc_ref = _ssrc_map_rtp_channel[ssrc];
			if (!stream_id_ref) {
				stream_id_ref = ssrc_ref = std::make_shared<RtpChannel>(ssrc, stream_id);
				stream_id_ref->setOnDetach(cb);
			}
			return stream_id_ref;
		}
		return nullptr;
	}

	string RtpChannelSelecter::getStreamId(const uint32_t& ssrc) {
		lock_guard<decltype(_mtx_map)> lck(_mtx_map);
		auto it = _ssrc_map_rtp_channel.find(ssrc);
		if (it == _ssrc_map_rtp_channel.end()) {
			return "";
		}
		return it->second->_stream_id;
	}

	uint32_t RtpChannelSelecter::getSSRC(const string& stream_id) {
		lock_guard<decltype(_mtx_map)> lck(_mtx_map);
		auto it = _stream_id_map_rtp_channel.find(stream_id);
		if (it == _stream_id_map_rtp_channel.end()) {
			return 0;
		}
		return it->second->_ssrc;
	}

	void RtpChannelSelecter::delByStreamId(const string& stream_id) {
		lock_guard<decltype(_mtx_map)> lck(_mtx_map);
		auto stream_id_it = _stream_id_map_rtp_channel.find(stream_id);
		if (stream_id_it != _stream_id_map_rtp_channel.end()) {
			auto ssrc = stream_id_it->second->_ssrc;
			auto ssrc_it = _ssrc_map_rtp_channel.find(ssrc);
			if (ssrc_it != _ssrc_map_rtp_channel.end()) {
				_ssrc_map_rtp_channel.erase(ssrc_it);
			}
			SsrcManager::Instance().releaseSSRC(ssrc);
			_stream_id_map_rtp_channel.erase(stream_id_it);
		}
	}

	void RtpChannelSelecter::clear() {
		lock_guard<decltype(_mtx_map)> lck(_mtx_map);
		_ssrc_map_rtp_channel.clear();
		_stream_id_map_rtp_channel.clear();
		SsrcManager::Instance().clear();
	}


	SsrcManager::SsrcManager() {}
	SsrcManager::~SsrcManager() {}

	INSTANCE_IMP(SsrcManager);

	uint32_t SsrcManager::getSSRC(bool isReal, const string& domain) {
		if(domain.length() >= 10){
			string new_ssrc_s = "";
			uint32_t new_ssrc = 0;
			string prefix = (isReal ? "0" : "1") + domain.substr(3,5);

			lock_guard<decltype(_mtx_set)> lck(_mtx_set);
			srand((int)time(0));
			for (int i = 10; i > 0; i--) {
				char random_num_str[5];
				sprintf(random_num_str,"%04d",random(0, 10000));
				new_ssrc_s = prefix + random_num_str;
				new_ssrc = stoi(new_ssrc_s);
				auto it = _ssrc_set_used.find(new_ssrc);
				if (it == _ssrc_set_used.end()) {
					_ssrc_set_used.insert(new_ssrc);
					return new_ssrc;
				}
			}
		}
		return 0;
	}

	void SsrcManager::releaseSSRC(const uint32_t& ssrc) {
		lock_guard<decltype(_mtx_set)> lck(_mtx_set);
		auto it = _ssrc_set_used.find(ssrc);
		if (it != _ssrc_set_used.end()) {
			_ssrc_set_used.erase(it);
		}
	}

	void SsrcManager::clear() {
		lock_guard<decltype(_mtx_set)> lck(_mtx_set);
		_ssrc_set_used.clear();
	}
}

#endif
