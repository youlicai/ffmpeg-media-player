//
// Created by apple on 2020/9/22.
//

#include <malloc.h>
#include "UOpenSLES.h"

//-------------------步骤---------------------
//1、获取引擎接口
//2、创建输出混音器
//3、创建播放器，并获取播放接口
//4、获取缓冲队列接口并给缓冲队列注册回调函数
//5、设置播放状态、手动调用回调函数

FILE *pcmFile;
//void *buffer;
uint8_t *out_buffer;
AVFrame  *frame;
//缓冲器队列接口
SLAndroidSimpleBufferQueueItf pcmBufferQueue;
UOpenSLES::UOpenSLES(){
    uqueue=new UQueue();

    out_buffer = (uint8_t *) malloc(44100 * 2 );
    out_buffer1 = (uint8_t *) av_malloc(44100 * 2 * 2);
}

void UOpenSLES::pushQueue(AVFrame * queueBuff) {
    uqueue->putAvpacket(queueBuff);
}

int UOpenSLES::getQueue() {

    while (true) {
        if(seek){
            continue;
        }
        if(uqueue->getQueueSize()==0){
            continue;
        }
        AVFrame *avFrame=uqueue->getAvpacket(NULL);

        if (avFrame->channels && avFrame->channel_layout == 0) {
            avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
        } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
            avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
        }

        SwrContext *swr_ctx;
        swr_ctx = swr_alloc_set_opts(
                NULL,
                AV_CH_LAYOUT_STEREO,
                AV_SAMPLE_FMT_S16,
                avFrame->sample_rate,
                avFrame->channel_layout,
                (AVSampleFormat) avFrame->format,
                avFrame->sample_rate,
                NULL, NULL
        );
        swr_init(swr_ctx);

        int nb = swr_convert(
                swr_ctx,
                &out_buffer1,
                avFrame->nb_samples,
                (const uint8_t **) avFrame->data,
                avFrame->nb_samples);
        int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
        out_buffer_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);


        now_time = avFrame->pts * av_q2d(time_base);
        if(now_time < clock){
            now_time = clock;
        }
        clock = now_time;
        if(callJava!=NULL)
            callJava->onCallTimeInfo(CHILD_THREAD,clock,duration);
        LOGD("当前播放时间 %f",clock);
        av_frame_free(&avFrame);
//        av_free(avFrame);
//        avFrame = NULL;
        swr_free(&swr_ctx);
        break;
    }
    return out_buffer_size;
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf1, void *context) {
    LOGD("pcmBufferCallBack");
    UOpenSLES *openSles=(UOpenSLES *)context;

    if(openSles==NULL){
        LOGD("openSles NULL");
    }
    int size=openSles->getQueue();
    if (size>0) {
        SLresult result;
        result = (*pcmBufferQueue)->Enqueue(pcmBufferQueue,(char *) openSles->out_buffer1, size);

    }

//    av_free(result);
}

void UOpenSLES::init() {
    //分配一个内存空间，44100 采样率，16位存储=2字节，2个声道数
    //-----------------获取引擎接口-----------------
    //1.创建
    slCreateEngine(&engineObject,0, NULL, 0, NULL, NULL);
    //2.实现
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    //3.得到具体的接口
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    //------------------创建输出混音器------------------
    const SLInterfaceID slitfids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean slitfreqs[1] = {SL_BOOLEAN_FALSE};
    //1.创建混音器
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, slitfids, slitfreqs);
    //2.实现混音器
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    //得到具体的接口
    (*outputMixObject)->GetInterface(outputMixObject,
                                              SL_IID_ENVIRONMENTALREVERB,&outputEnvironmentalReverbItf);
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};

    //----------------------创建播放器----------------------
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};
    //pcm格式信息
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            2,//2个声道（立体声）
            SL_SAMPLINGRATE_44_1,//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT ,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };
    //数据源，队列，pcm
    SLDataSource slDataSource = {&android_queue, &pcm};
    SLDataSink audioSnk = {&outputMix, NULL};
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSnk, 3,
                                       ids, req);
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);

    //------------------创建缓冲区和回调函数---------------------
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    //缓冲接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);
    //获取音量接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmPlayerVolume);
//    //第五步，设置播放状态----------------------------------------
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    //第六步，主动调用回调函数开始工作----------------------------------------
    pcmBufferCallBack(NULL, this);
    //释放
LOGD("---------------------");
}

void *decodPlay(void *data)
{
    UOpenSLES *wlAudio = (UOpenSLES *) data;
    wlAudio->init();
    pthread_exit(&wlAudio->thread_play);
}

void UOpenSLES::play() {
    pthread_create(&thread_play, NULL, decodPlay, this);
}
void UOpenSLES::continue_() {
    STATUS=1;
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
}

void UOpenSLES::pause() {
    STATUS=2;
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
}