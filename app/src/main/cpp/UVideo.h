//
// Created by apple on 2020/9/18.
//

#ifndef FACEU_UVIDEO_H
#define FACEU_UVIDEO_H


#include "UEGL.h"
#include "UCallJava.h"
#include<Android/log.h>
extern "C"{
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
};
#define TAG "android_jni_video"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
class UVideo {
public:
    UEGL *egl;
    UCallJava *callJava;
    bool seek=false;
public:
    void addcalljava(UCallJava *callJava);
    void initPar(JNIEnv *env,jobject sur,AVCodecContext *pCodecCtx);
    void initDevice(JNIEnv *env,jobject sur,AVCodecContext *pCodecCtx);
    void putAVFrameQueue(AVFrame *avFrame);
    int getAVFrameQueueSize();
    void clearAVFrameQueue();
    void start();
    void pause();
    void continue_();
    void forword();
    void back();
    void seeking(bool seek);
    void cut();
    void destroy();
};


#endif //FACEU_UVIDEO_H
