//
// Created by apple on 2020/9/22.
//


#include "UQueue.h"
UQueue::UQueue() {
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);
}

UQueue::~UQueue() {

}

int UQueue::putAvpacket(AVFrame *queuebuff) {
    pthread_mutex_lock(&mutexPacket);
    queueBuff.push(queuebuff);
//    LOGD("放入一个AVpacket到队里里面， 个数为：%d", queueBuff.size());
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

AVFrame* UQueue::getAvpacket(AVFrame *queuebuff1) {
    pthread_mutex_lock(&mutexPacket);
//    if(queueBuff.size()>0)

//    av_frame_ref(queuebuff1,queueBuff.front());
    AVFrame* queuebuff11 =  queueBuff.front();
    queueBuff.pop();
//    av_frame_free(&queuebuff11);
//    LOGD("从队列里面取出一个AVpacket，还剩下 %d 个", queueBuff.size());
    pthread_mutex_unlock(&mutexPacket);
    return queuebuff11;
}

int UQueue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mutexPacket);
    size = queueBuff.size();
    pthread_mutex_unlock(&mutexPacket);
    return size;
}

void UQueue::clearQueue() {
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);

    while (!queueBuff.empty()){
        AVFrame *packet1 = queueBuff.front();
        queueBuff.pop();
        av_frame_free(&packet1);
        av_free(packet1);
        packet1 = NULL;
    }
    pthread_mutex_unlock(&mutexPacket);
}