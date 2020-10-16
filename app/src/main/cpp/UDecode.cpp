//
// Created by apple on 2020/9/27.
//

#include "UDecode.h"

UDecode::UDecode(){
//    packet=av_packet_alloc();
}

int UDecode::createADecodec(const AVCodecParameters *codecpar) {
    aCodecCtx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(aCodecCtx, codecpar);
    //3、根据解码器上下文，获得解码器ID，然后查找解码器
    aCodec=avcodec_find_decoder(aCodecCtx->codec_id);
    if(aCodec==NULL){
        LOGD("avcodec_find_decoder");
        return -1;
    }
    //第五步：打开解码器
    int avcodec_open2_result = avcodec_open2(aCodecCtx, aCodec,NULL);
    if (avcodec_open2_result !=0){
        LOGD("avcodec_open2");
        return -1;
    }
    return 0;
}

int UDecode::createVDecodec(const AVCodecParameters *codecpar) {
    vCodecCtx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(vCodecCtx, codecpar);

    vCodecCtx->bit_rate = 400000;
    vCodecCtx->gop_size=250;

    vCodecCtx->time_base.num = 1;
    vCodecCtx->time_base.den = 65;
    vCodecCtx->qmin = 10;
    vCodecCtx->qmax = 51;


    //3、根据解码器上下文，获得解码器ID，然后查找解码器
    vCodec=avcodec_find_decoder(vCodecCtx->codec_id);
    if(aCodec==NULL){
        LOGD("avcodec_find_decoder");
        return -1;
    }
    //第五步：打开解码器
    int avcodec_open2_result = avcodec_open2(vCodecCtx, vCodec,NULL);
    if (avcodec_open2_result !=0){
        LOGD("avcodec_open2");
        return -1;
    }
    return 0;
}

int UDecode::decodeA(AVFrame *inFrame,AVPacket *packet) {
    int ret;
    ret=avcodec_send_packet(aCodecCtx, packet);
    if (ret!=0){
        LOGD("avcodec_send_packet fail %d",ret);
        return -1;
    }
    av_packet_unref(packet);
    ret = avcodec_receive_frame(aCodecCtx, inFrame);
    if(ret!=0){
        LOGD("avcodec_receive_frame fail %d",ret);
        return -1;
    }
    return 0;
}

int UDecode::decodeV(AVFrame *inFrame,AVPacket *packet) {
    int ret;
    ret=avcodec_send_packet(vCodecCtx, packet);
    if (ret!=0){
        LOGD("avcodec_send_packet fail %d",ret);
        return -1;
    }
    av_packet_unref(packet);
    ret = avcodec_receive_frame(vCodecCtx, inFrame);
    if(ret!=0){
        LOGD("avcodec_receive_frame fail %d",ret);
        return -1;
    }
    return 0;
}