//
// Created by apple on 2020/9/18.
//

#include "UVideo.h"

//UVideo::UVideo(JNIEnv *env, jobject sur, AVCodecContext *pCodecCtx){
//    initDevice(env,sur,pCodecCtx);
//}

void UVideo::addcalljava(UCallJava *callJava1) {
    callJava=callJava1;
}
void UVideo::initPar(JNIEnv *env, jobject sur, AVCodecContext *pCodecCtx) {
    initDevice(env,sur,pCodecCtx);
}
void UVideo::initDevice(JNIEnv *env,jobject sur,AVCodecContext *pCodecCtx) {
    if(egl==NULL)
        egl=new UEGL(env,sur,pCodecCtx);
}
void UVideo::putAVFrameQueue(AVFrame *avFrame) {
    egl->uqueue->putAvpacket(avFrame);
}

int UVideo::getAVFrameQueueSize() {
    return egl->uqueue->getQueueSize();
}

void UVideo::clearAVFrameQueue() {
    egl->uqueue->clearQueue();
}
void UVideo::pause() {
    egl->pause();
}

void UVideo::start() {
    egl->play();
}

void UVideo::continue_() {
    egl->continue_();
}
void UVideo::destroy() {
    egl->destory();
}

void UVideo::seeking(bool seek_) {
    egl->seek=seek_;
}

void UVideo::cut() {
    egl->cut=true;
}