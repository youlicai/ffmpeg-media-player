//
// Created by apple on 2020/9/22.
//

#ifndef FACEU_UQUEUE_H
#define FACEU_UQUEUE_H
#include "queue"
#include "pthread.h"
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
}
#include<Android/log.h>
#define TAG "android_jni_uqueue"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
class UQueue {
public:
    std::queue<AVFrame *> queueBuff;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
public:
    UQueue();
    ~UQueue();
    int putAvpacket(AVFrame *queueBuff);
    AVFrame * getAvpacket(AVFrame *queueBuff);
    int getQueueSize();
    void clearQueue();
};
#endif //FACEU_UQUEUE_H
