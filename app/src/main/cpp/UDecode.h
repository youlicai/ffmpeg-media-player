//
// Created by apple on 2020/9/27.
//

#ifndef FACEU_UDECODE_H
#define FACEU_UDECODE_H

extern "C"{
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
}
#include<Android/log.h>
#define TAG "android_jni_decodec"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
class UDecode {

public:
    AVCodecContext *aCodecCtx;
    AVCodec *aCodec;

    AVCodecContext *vCodecCtx;
    AVCodec *vCodec;
public:
    UDecode();
    int createADecodec(const AVCodecParameters *codecpar);
    int createVDecodec(const AVCodecParameters *codecpar);
    int decodeA(AVFrame *inFrame,AVPacket *packet);
    int decodeV(AVFrame *inFrame,AVPacket *packet);
};


#endif //FACEU_UDECODE_H
