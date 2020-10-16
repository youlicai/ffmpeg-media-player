//
// Created by apple on 2020/9/29.
//

#include "UChange.h"

UChange::UChange(AVCodecContext *pCodecCtx){
    this->pCodecCtx=pCodecCtx;
    swsContext=sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
    out_buffer=new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height)];

}

void UChange::usws_scale(AVFrame *inAVFrame) {
    desAVFrame=av_frame_alloc();
    av_image_fill_arrays(desAVFrame->data, desAVFrame->linesize, out_buffer, AV_PIX_FMT_RGB24,
                         pCodecCtx->width, pCodecCtx->height, 1);
    sws_scale(swsContext, (uint8_t const * const *)inAVFrame->data, inAVFrame->linesize, 0, pCodecCtx->height, desAVFrame->data, desAVFrame->linesize);
    av_frame_ref(inAVFrame,desAVFrame);
}