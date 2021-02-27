/*
 * Copyright (c) 2016 The ZLMediaKit project authors. All Rights Reserved.
 *
 * This file is part of ZLMediaKit(https://github.com/xia-chu/ZLMediaKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#ifndef ZLMEDIAKIT_RTPCHANNEL_H
#define ZLMEDIAKIT_RTPCHANNEL_H

#if defined(ENABLE_RTPPROXY)

#include <memory>
#include <string>
#include <functional>
#include <cstdlib>
#include <ctime>
#include "Network/Socket.h"
#include "RtpProcess.h"
#include "RtpSelector.h"

using namespace std;
using namespace toolkit;

namespace mediakit {

#define random(a,b) (rand()%(b-a)+a)

class RtpChannel : public std::enable_shared_from_this<RtpChannel> {
	public:
		typedef std::shared_ptr<RtpChannel> Ptr;
		friend class RtpChannelSelecter;
		RtpChannel(const uint32_t& ssrc,const string& stream_id);
		~RtpChannel();
		string getStreamId();
		uint32_t getSsrc();
		void setOnDetach(const function<void()>& cb);
	private:
		uint32_t _ssrc;
		string _stream_id;
		RtpProcess::Ptr _rtp_process;
};

class RtpChannelSelecter : public std::enable_shared_from_this<RtpChannelSelecter> {
	public:
		RtpChannelSelecter();
		~RtpChannelSelecter();

		static RtpChannelSelecter& Instance();

		RtpChannel::Ptr regist(bool isReal, const string& domain, const string& stream_id, const function<void()>& cb);
		string getStreamId(const uint32_t& ssrc);
		uint32_t getSSRC(const string& stream_id);
		void delByStreamId(const string& stream_id);
		/**
		 * 清空所有对象
		 */
		void clear();
	private:
		recursive_mutex _mtx_map;
		unordered_map<string, RtpChannel::Ptr> _stream_id_map_rtp_channel;
		unordered_map<uint32_t, RtpChannel::Ptr> _ssrc_map_rtp_channel;
};

class SsrcManager : public std::enable_shared_from_this<SsrcManager> {
public:
	SsrcManager();
	~SsrcManager();

	static SsrcManager& Instance();

	uint32_t getSSRC(bool isReal,const string& domain);

	void releaseSSRC(const uint32_t& ssrc);
	/**
	* 清空所有对象
	*/
	void clear();
private:
	recursive_mutex _mtx_set;
	unordered_set<uint32_t> _ssrc_set_used;
};

class GenerateSsrcException : public std::exception
{
public:
	GenerateSsrcException() {}
	~GenerateSsrcException() {}

};
}//namespace mediakit
#endif//ENABLE_RTPPROXY
#endif //ZLMEDIAKIT_RTPCHANNEL_H