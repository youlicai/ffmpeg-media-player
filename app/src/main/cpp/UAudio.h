//
// Created by apple on 2020/9/18.
//

#ifndef FACEU_UAUDIO_H
#define FACEU_UAUDIO_H
#include "UEGL.h"
#include "UQueue.h"
#include "UOpenSLES.h"
#include "UCallJava.h"


extern "C"{
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
};
#include<Android/log.h>
#define TAG "android_jni_audio"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
class UAudio {
public:
    UOpenSLES *opensles;
    UCallJava *callJava;
    bool seek=false;
public:
    UAudio();
    ~UAudio();
    void addcalljava(UCallJava *callJava);
    void initDevice();
    void putAVFrameQueue(AVFrame *avFrame);
    int getAVFrameQueueSize();
    void clearAVFrameQueue();
    void start();
    void pause();
    void continue_();
    void forword();
    void back();
    void seeking(bool seek_);
    void destroy();
};


#endif //FACEU_UAUDIO_H
