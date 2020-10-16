//
// Created by apple on 2020/9/18.
//
#include "UAudio.h"
UAudio::UAudio(){
    initDevice();
}

void UAudio::forword() {

}

void UAudio::back() {

}

void UAudio::addcalljava(UCallJava *callJava1){
    callJava=callJava1;
    opensles->callJava=callJava;
}

void UAudio::initDevice() {
    opensles=new UOpenSLES();

}

void UAudio::putAVFrameQueue(AVFrame *avFrame) {
    opensles->uqueue->putAvpacket(avFrame);
}
int UAudio::getAVFrameQueueSize() {
    return opensles->uqueue->getQueueSize();
}
void UAudio::clearAVFrameQueue() {
    opensles->uqueue->clearQueue();
}
void UAudio::start() {
    opensles->play();
}

void UAudio::pause() {
    opensles->pause();
}

void UAudio::continue_() {
    opensles->continue_();
}


void UAudio::seeking(bool seek_) {
    opensles->seek=seek_;
}