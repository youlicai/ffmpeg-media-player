//
// Created by apple on 2020/9/29.
//

#ifndef FACEU_UCHANGE_H
#define FACEU_UCHANGE_H

extern "C"{
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
};

#include "ULog.h"
class UChange {

public:
    AVCodecContext *pCodecCtx;
    AVFrame *desAVFrame=NULL;
    struct SwsContext *swsContext;//视频
    SwrContext *swrContext;//音频
    uint8_t *out_buffer;

public:

    UChange(AVCodecContext *pCodecCtx);
    void usws_scale(AVFrame *esAVFrame);
    void swr_convert();

};


#endif //FACEU_UCHANGE_H
