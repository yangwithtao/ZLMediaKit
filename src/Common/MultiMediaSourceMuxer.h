﻿/*
 * MIT License
 *
 * Copyright (c) 2016 xiongziliang <771730766@qq.com>
 *
 * This file is part of ZLMediaKit(https://github.com/xiongziliang/ZLMediaKit).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ZLMEDIAKIT_MULTIMEDIASOURCEMUXER_H
#define ZLMEDIAKIT_MULTIMEDIASOURCEMUXER_H

#include "RtspMuxer/RtspMediaSourceMuxer.h"
#include "RtmpMuxer/RtmpMediaSourceMuxer.h"

class MultiMediaSourceMuxer : public FrameRingWriterInterface{
public:
    typedef std::shared_ptr<MultiMediaSourceMuxer> Ptr;

    MultiMediaSourceMuxer(const string &vhost,
                          const string &strApp,
                          const string &strId,
                          float dur_sec = 0.0){
        _rtmp = std::make_shared<RtmpMediaSourceMuxer>(vhost,strApp,strId,std::make_shared<TitleMete>(dur_sec));
        _rtsp = std::make_shared<RtspMediaSourceMuxer>(vhost,strApp,strId,std::make_shared<TitleSdp>(dur_sec));
    }
    virtual ~MultiMediaSourceMuxer(){}


    /**
     * 添加音视频媒体
     * @param track 媒体描述
     */
    void addTrack(const Track::Ptr & track) {
        //克隆track的目的是防止inputFrame时由于用的同一个track而导致写入两次数据
        _rtmp->addTrack(track->clone());
        _rtsp->addTrack(track->clone());
    }

    /**
     * 写入帧数据然后打包rtmp
     * @param frame 帧数据
     */
    void inputFrame(const Frame::Ptr &frame) override {
        //_rtmp和_rtsp对象不能使用相同的Track，否则会触发两次inputFrame操作
        _rtmp->inputFrame(frame);
        _rtsp->inputFrame(frame);
    }

    /**
     * 设置事件监听器
     * @param listener
     */
    void setListener(const std::weak_ptr<MediaSourceEvent> &listener){
        _rtmp->setListener(listener);
        _rtsp->setListener(listener);
    }

    /**
     * 返回总的消费者个数
     * @return
     */
    int readerCount() const{
        return _rtsp->readerCount() + _rtmp->readerCount();
    }

    void setTimeStamp(uint32_t stamp){
        _rtsp->setTimeStamp(stamp);
    }
private:
    RtmpMediaSourceMuxer::Ptr _rtmp;
    RtspMediaSourceMuxer::Ptr _rtsp;
};


#endif //ZLMEDIAKIT_MULTIMEDIASOURCEMUXER_H