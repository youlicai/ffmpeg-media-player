//
// Created by apple on 2020/9/18.
//

#ifndef FACEU_UPLAYER_H
#define FACEU_UPLAYER_H


#include <jni.h>
extern "C"{
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
}
#include<Android/log.h>
#include "UCallJava.h"
#define TAG "android_jni_uplayer"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

#define INIT -1
#define PLAYING 0
#define PAUSE 1
#define SEEKING 2


class UPlayer {
public:
    AVFormatContext *pFormatCtx;
    UCallJava *calljava;
    pthread_mutex_t seek_mutex;
    int PLAYERSTATUS=INIT;

public:
    UPlayer();
    void addcalljava(UCallJava *calljava);
    void play(JNIEnv *env, const char *path_in,const char *path_out,jobject sur);
    void pause();
    void continue_();
    void seek(int sec);
    void cut();
};


#endif //FACEU_UPLAYER_H
