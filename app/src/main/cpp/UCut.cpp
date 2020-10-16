//
// Created by apple on 2020/9/28.
//


#include "UCut.h"


void UCut::cut(AVFrame *avFrame) {

    // 输出文件路径
    const char* out_file = "/storage/emulated/0/ffmpeg_test.jpg";
    // 分配AVFormatContext对象
    AVFormatContext* pFormatCtx = avformat_alloc_context();
    // 设置输出文件格式
    pFormatCtx->oformat = av_guess_format("mjpeg", NULL, NULL);

    // 创建并初始化一个和该url相关的AVIOContext
    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0){
        printf("Couldn't open output file");
        return;
    }

    // 构建一个新stream
    AVStream* pAVStream = avformat_new_stream(pFormatCtx, 0);
    if (pAVStream == NULL){
        printf("Frame2JPG::avformat_new_stream error.");
        return;
    }
    // 设置该stream的信息
    AVCodecContext* pCodecCtx = pAVStream->codec;
    pCodecCtx->codec_id = pFormatCtx->oformat->video_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    pCodecCtx->width =  avFrame->width;
    pCodecCtx->height =  avFrame->height;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;

    // 查找解码器
    AVCodec* pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec){
        printf("avcodec_find_encoder() error.");
        return;
    }
    // 设置pCodecCtx的解码器为pCodec
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
        printf("Could not open codec.");
        return;
    }

    //Write Header
    int ret = avformat_write_header(pFormatCtx, NULL);
    if (ret < 0){
        return;
    }

    int y_size = pCodecCtx->width * pCodecCtx->height;
    //Encode
    // 给AVPacket分配足够大的空间
    AVPacket pkt;
    av_new_packet(&pkt, y_size * 3);
    int got_picture = 0;
    ret = avcodec_send_frame(pCodecCtx, avFrame);
    if (ret < 0) {
        return;
    }
    got_picture = avcodec_receive_packet(pCodecCtx, &pkt);
    if (!got_picture) {
        ret = av_write_frame(pFormatCtx, &pkt);
    }
    av_free_packet(&pkt);
    av_write_trailer(pFormatCtx);
    if (pAVStream){
        avcodec_close(pAVStream->codec);
    }
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);
}