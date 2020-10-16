//
// Created by apple on 2020/9/22.
//

#ifndef FACEU_UOPENSLES_H
#define FACEU_UOPENSLES_H

#include <cstdlib>
#include <queue>

extern "C"{
#include <libavutil/time.h>
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
};
#include<Android/log.h>
#include "UQueue.h"
#include "UCallJava.h"

#define TAG "android_jni_sles"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
class UOpenSLES {

public:
    // 引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputEnvironmentalReverbItf = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    //pcm
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    SLVolumeItf pcmPlayerVolume = NULL;

    int out_buffer_size=0;
    uint8_t *out_buffer1;
    UQueue *uqueue;
    pthread_t thread_play;
    int duration = 0;
    AVRational time_base;
    double clock;//总的播放时长
    double now_time;//当前frame时间
    double last_tiem; //上一次调用时间

    int STATUS=0; //1:playing  2:pause
    UCallJava *callJava;
    bool seek=false;

public:
    UOpenSLES();
    void init();
    void pushQueue(AVFrame *queuebuff);
    int getQueue();
    void play();
    void continue_();
    void pause();

};


#endif //FACEU_UOPENSLES_H
