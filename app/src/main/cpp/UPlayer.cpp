//
// Created by apple on 2020/9/18.
//

#include "UPlayer.h"
#include "UAudio.h"
#include "UEGL.h"
#include "UVideo.h"
#include "UDecode.h"
#include "UChange.h"

UAudio *audio=new UAudio();
UVideo *video=new UVideo();
UDecode *aDecodec=new UDecode();
int v_stream_idx = -1;
int a_stream_idx=-1;

bool search_key_flag= false;
UPlayer::UPlayer(){
    pthread_mutex_init(&seek_mutex, NULL);
}
void UPlayer::addcalljava(UCallJava *calljava1) {
    audio->addcalljava(calljava1);
    video->addcalljava(calljava1);
}

void UPlayer::play(JNIEnv *env, const char *path_in,const char *path_out,jobject sur) {

    if(PLAYERSTATUS==INIT) {
        PLAYERSTATUS=PLAYING;
        //获取输入输出文件名
        const char *input = path_in;
        const char *output_yuv = path_out;
        LOGD("path: %s", input);
        //1.注册所有组件
        av_register_all();
        //封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
        pFormatCtx = avformat_alloc_context();

        //2.打开输入视频文件
        int ret;
        char buf[1024];
        ret = avformat_open_input(&pFormatCtx, input, NULL, NULL);
        if (ret != 0) {
            av_strerror(ret, buf, 1024);
//        LOGD("无法打开输入视频文件 %d",ret);
            printf("%s", "无法打开输入视频文件");
            LOGD("Couldn’t open file %s: %d(%s)", input, ret, buf);
            return;
        }
        //3.获取视频文件信息
        if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
            LOGD("无法获取视频文件信息");
            printf("%s", "无法获取视频文件信息");
            return;
        }
        //获取视频流的索引位置
        //遍历所有类型的流（音频流、视频流、字幕流），找到视频流

        int i = 0;
        for (; i < pFormatCtx->nb_streams; i++) {
            //流的类型
            if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                v_stream_idx = i;
            } else if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
                a_stream_idx = i;
            }
        }

        if (v_stream_idx == -1) {
            LOGD("找不到视频流");
            return;
        }
        if (a_stream_idx == -1) {
            LOGD("找不到音频流");
            return;
        }

        //输出视频信息
        LOGD("视频的文件格式：%s", pFormatCtx->iformat->name);
        LOGD("视频时长：%d", (pFormatCtx->duration) / 1000000);
        audio->opensles->duration = (pFormatCtx->duration) / 1000000;
        AVPacket *packet = av_packet_alloc();

        aDecodec->createADecodec(pFormatCtx->streams[a_stream_idx]->codecpar);
        aDecodec->createVDecodec(pFormatCtx->streams[v_stream_idx]->codecpar);

//    audio=new UAudio();
        audio->opensles->duration = pFormatCtx->duration / AV_TIME_BASE;
        audio->opensles->time_base = pFormatCtx->streams[a_stream_idx]->time_base;

//    UVideo *video=new UVideo();
//    video=new UVideo();
        video->initPar(env, sur, aDecodec->vCodecCtx);
        video->egl->audio = audio->opensles;
        video->egl->time_base = pFormatCtx->streams[v_stream_idx]->time_base;
        int num = pFormatCtx->streams[v_stream_idx]->avg_frame_rate.num;
        int den = pFormatCtx->streams[v_stream_idx]->avg_frame_rate.den;
        if (num != 0 && den != 0) {
            int fps = num / den;//[25 / 1]
            video->egl->defaultDelayTime = 1.0 / fps;
        }

        audio->start();
        video->start();
        UChange *change=new UChange(aDecodec->vCodecCtx);


        while (true) {
            int size = audio->getAVFrameQueueSize();
            LOGD("audio queue size: %d", size);
            if (size > 100) {
                continue;
            }
            if (av_read_frame(pFormatCtx, packet) >= 0) {
                LOGD("NOT hello");
                AVFrame *pFrame420 = av_frame_alloc();
                if (packet->stream_index == v_stream_idx) {
                    if (PLAYERSTATUS==SEEKING) {
                        //seek后找到关键帧，解决花屏问题。
                        if (!(packet->flags & AV_PKT_FLAG_KEY)) {
                            LOGD("NOT AV_PKT_FLAG_KEY");
                            continue;
                        }
                        PLAYERSTATUS = PLAYING;
                        LOGD("IS AV_PKT_FLAG_KEY");
                    }
                    if (aDecodec->decodeV(pFrame420, packet) != 0) {
                        LOGD("decodeV fail");
                        av_packet_unref(packet);
                        continue;
                    }
//                    pFrame420= change->usws_scale(pFrame420);
//                    LOGD("pFrame420 info: %d",pFrame420->height);
                    video->putAVFrameQueue(pFrame420);
                }
                if (packet->stream_index == a_stream_idx) {
                    if (PLAYERSTATUS==SEEKING)//seek后，寻找I帧过程中，扔掉音频。
                        continue;
                    if (aDecodec->decodeA(pFrame420, packet) != 0) {
                        LOGD("decodeA fail");
                        continue;
                    }
                    audio->putAVFrameQueue(pFrame420);
                }
                av_packet_unref(packet);
            }
        }
    }
}

void UPlayer::pause() {
    if(PLAYERSTATUS==PLAYING){
        audio->pause();
        video->pause();
        PLAYERSTATUS=PAUSE;
    }

}

void UPlayer::continue_() {
    if(PLAYERSTATUS==PAUSE) {
        audio->continue_();
        video->continue_();
        PLAYERSTATUS=PLAYING;
    }
}

void UPlayer::seek(int sec) {
    continue_();
    if(PLAYERSTATUS!=SEEKING&&PLAYERSTATUS!=INIT) {
        search_key_flag = true;
        audio->seeking(true);
        video->seeking(true);
        pthread_mutex_lock(&seek_mutex);

        av_seek_frame(pFormatCtx, v_stream_idx, (int64_t) (sec / av_q2d(video->egl->time_base)),
                      AVSEEK_FLAG_BACKWARD);
        av_seek_frame(pFormatCtx, a_stream_idx,
                      (int64_t) (sec / av_q2d(audio->opensles->time_base)), AVSEEK_FLAG_BACKWARD);
        audio->opensles->clock = 0;
        audio->clearAVFrameQueue();
        avcodec_flush_buffers(aDecodec->aCodecCtx);

        video->egl->clock = 0;
        video->clearAVFrameQueue();
//    avcodec_flush_buffers(aDecodec->vCodecCtx); //清理后回崩溃
        video->seeking(false);
        audio->seeking(false);
        pthread_mutex_unlock(&seek_mutex);
        PLAYERSTATUS=SEEKING;
    }
}


void UPlayer::cut() {
    video->cut();
}
