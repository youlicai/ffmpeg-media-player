//
// Created by apple on 2020/9/28.
//

#ifndef FACEU_UCALLJAVA_H
#define FACEU_UCALLJAVA_H
#include "jni.h"
#include <linux/stddef.h>
#include <stdint.h>
#include<Android/log.h>

#define MAIN_THREAD 0
#define CHILD_THREAD 1

class UCallJava {
public:
    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;

    jmethodID jmid_parpared;
    jmethodID jmid_load;
    jmethodID jmid_timeinfo;
    jmethodID jmid_error;
    jmethodID jmid_complete;
    jmethodID jmid_renderyuv;

public:
    UCallJava(_JavaVM *javaVM, JNIEnv *env, jobject *obj);
    ~UCallJava();

    void onCallParpared(int type);

    void onCallLoad(int type, bool load);

    void onCallTimeInfo(int type, int curr, int total);

    void onCallError(int type, int code, char *msg);

    void onCallComplete(int type);

    void onCallRenderYUV(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv);

};


#endif //FACEU_UCALLJAVA_H
