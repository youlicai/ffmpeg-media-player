//
// Created by apple on 2020/9/28.
//

#ifndef FACEU_UCUT_H
#define FACEU_UCUT_H

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
}

#include<Android/log.h>
#define TAG "android_jni_audio"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
class UCut {
public:
    void cut(AVFrame *avFrame);
};


#endif //FACEU_UCUT_H
