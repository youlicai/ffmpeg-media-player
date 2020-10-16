//
// Created by apple on 2020/9/21.
//

#ifndef FACEU_UEGL_H
#define FACEU_UEGL_H
#include "UQueue.h"
#include "UOpenSLES.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <jni.h>
#include <android/native_window_jni.h>
#include<Android/log.h>
#define TAG "android_jni_uegl"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}
class UEGL {
public:
    EGLDisplay eglDisp;
    EGLSurface eglWindow;
    EGLContext eglCtx;

    float *vertexData;
    float *textureVertexData;
    GLuint yTextureId;
    GLuint uTextureId;
    GLuint vTextureId;
    pthread_t thread_play;
    UQueue *uqueue;
    struct SwsContext *sws_ctx;

    JNIEnv *env;
    jobject sur;
    AVCodecContext *pCodecCtx;

    AVRational time_base;
    UOpenSLES *audio = NULL;
    double clock = 0;
    double delayTime = 0;
    double defaultDelayTime = 0.03;

    int STATUS=0;
    bool seek=false;
    bool cut=false;
public:
    UEGL(JNIEnv *env,jobject sur,AVCodecContext *pCodecCtx);
    void init();
    void draw();
    void play();
    void pause();
    void continue_();
    void destory();
    double getFrameDiffTime(AVFrame *avFrame);
    double getDelayTime(double diff);
};


#endif //FACEU_UEGL_H
