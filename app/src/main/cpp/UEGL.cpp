//
// Created by apple on 2020/9/21.
//



#include "UEGL.h"
#include "ShaderUtils.h"
#include "UCut.h"


#define GET_STR(x) #x
const char *vertexShaderString = GET_STR(
        attribute vec4 aPosition;
        attribute vec2 aTexCoord;
        varying vec2 vTexCoord;
        void main() {
            vTexCoord=vec2(aTexCoord.x,1.0-aTexCoord.y);
            gl_Position = aPosition;
        }
);
const char *fragmentShaderString = GET_STR(
        precision mediump float;
        varying vec2 vTexCoord;
        uniform sampler2D yTexture;
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;
            rgb = mat3(1.0,       1.0,         1.0,
                       0.0,       -0.39465,  2.03211,
                       1.13983, -0.58060,  0.0) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);


UEGL::UEGL(JNIEnv *env1,jobject sur1,AVCodecContext *pCodecCtx1){
    uqueue=new UQueue();
    env=env1;
    sur=sur1;
    pCodecCtx=pCodecCtx1;
}
void UEGL::init() {
    STATUS=1;
//    sws_ctx = sws_getContext(pCodecCtx->width,pCodecCtx->height,pCodecCtx->pix_fmt,
//                             pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,SWS_BICUBIC, NULL, NULL, NULL);

    /**
   *初始化egl
   **/
    int windowWidth;
    int windowHeight;
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, sur);
    //-----------------初始化EGL-------------------
    //1 EGL display创建和初始化
    eglDisp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(eglDisp == EGL_NO_DISPLAY)
    {
        LOGD("eglGetDisplay failed!");
        return;
    }
    if(EGL_TRUE != eglInitialize(eglDisp,0,0))
    {
        LOGD("eglInitialize failed!");
        return;
    }
    //2 surface
    //2-1 surface窗口配置
    EGLConfig eglConf;
    EGLint numConfigs;
    EGLint configSpec[] = { EGL_RED_SIZE, 8,
                            EGL_GREEN_SIZE, 8,
                            EGL_BLUE_SIZE, 8,
                            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE };
//    EGLint eglMajVers, eglMinVers;
//    eglInitialize(eglDisp, &eglMajVers, &eglMinVers);
    if(EGL_TRUE !=eglChooseConfig(eglDisp, configSpec, &eglConf, 1, &numConfigs)){
        LOGD("eglChooseConfig failed!");
        return;
    }
    //创建surface
    eglWindow = eglCreateWindowSurface(eglDisp, eglConf,nativeWindow, NULL);
    if(eglWindow == EGL_NO_SURFACE)
    {
        LOGD("eglCreateWindowSurface failed!");
        return;
    }
    eglQuerySurface(eglDisp,eglWindow,EGL_WIDTH,&windowWidth);
    eglQuerySurface(eglDisp,eglWindow,EGL_HEIGHT,&windowHeight);

    //3 context 创建关联的上下文
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    eglCtx= eglCreateContext(eglDisp, eglConf,EGL_NO_CONTEXT, ctxAttr);
    if(eglCtx == EGL_NO_CONTEXT) {
        LOGD("eglCreateContext failed!");
        return;
    }
    if(EGL_TRUE != eglMakeCurrent(eglDisp, eglWindow, eglWindow, eglCtx)) {
        LOGD("eglMakeCurrent failed!");
        return;
    }

    LOGD("EGL Init Success!");

    ShaderUtils *shaderUtils = new ShaderUtils();
    GLuint programId = shaderUtils->createProgram(vertexShaderString,fragmentShaderString);
    if(programId == 0)
    {
        LOGD("glCreateProgram failed!");
        return;
    }
    delete shaderUtils;

    //-------------------------顶点着色器----------------------------
    //传递顶点数据
    vertexData = new float[12]{
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
    };
    GLuint aPositionHandle = (GLuint) glGetAttribLocation(programId, "aPosition");
    glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE,
                          12, vertexData);
    glEnableVertexAttribArray(aPositionHandle);

    //传递材质数据
    textureVertexData = new float[8]{
            1.0f, 0.0f,//右下
            0.0f, 0.0f,//左下
            1.0f, 1.0f,//右上
            0.0f, 1.0f//左上
    };
    GLuint aTextureCoordHandle = (GLuint) glGetAttribLocation(programId, "aTexCoord");
    glEnableVertexAttribArray(aTextureCoordHandle);
    glVertexAttribPointer(aTextureCoordHandle,2,GL_FLOAT,GL_FALSE,8,textureVertexData);



    //因为没有用矩阵所以就手动自适应
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;
    int left,top,viewWidth,viewHeight;
    if(windowHeight > windowWidth){
        left = 0;
        viewWidth = windowWidth;
        viewHeight = (int)(videoHeight*1.0f/videoWidth*viewWidth);
        top = (windowHeight - viewHeight)/2;
    }else{
        top = 0;
        viewHeight = windowHeight;
        viewWidth = (int)(videoWidth*1.0f/videoHeight*viewHeight);
        left = (windowWidth - viewWidth)/2;
    }
    glViewport(left, top, viewWidth, viewHeight);

    glUseProgram(programId);

    //-------------------------初始化空的yuv纹理----------------------------
    GLuint textureSamplerHandleY = (GLuint) glGetUniformLocation(programId, "yTexture");
    GLuint textureSamplerHandleU = (GLuint) glGetUniformLocation(programId, "uTexture");
    GLuint textureSamplerHandleV = (GLuint) glGetUniformLocation(programId, "vTexture");

    glGenTextures(1,&yTextureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,yTextureId);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(textureSamplerHandleY,0);

    glGenTextures(1,&uTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,uTextureId);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(textureSamplerHandleU,1);

    glGenTextures(1,&vTextureId);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,vTextureId);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(textureSamplerHandleV,2);
    draw();
}

void UEGL::draw(){
    while (true) {

        if(STATUS!=1){
            LOGD("video pause");
            av_usleep(1000*100);
            continue;
        }
        if(seek){
            LOGD("seek");
            continue;
        }

        if(uqueue->getQueueSize()==0){
            continue;
        }

        AVFrame *yuvFrame=uqueue->getAvpacket(NULL);
//        av_frame_free(&yuvFrame);
//        continue;
        if(yuvFrame==NULL){
            continue;
        }
        double diff = getFrameDiffTime(yuvFrame);
        av_usleep(getDelayTime(diff) * 1000000);

        /***
        *解码后的数据更新到yuv纹理中
        ***/

        //==================cut==================
//        if(cut){
//            UCut *cut=new UCut();
//            cut->cut(yuvFrame);
//        }
//        cut=false;
        //==================cut==================

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, yTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yuvFrame->linesize[0], pCodecCtx->height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvFrame->data[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, uTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yuvFrame->linesize[1], pCodecCtx->height / 2, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvFrame->data[1]);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, vTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yuvFrame->linesize[2], pCodecCtx->height / 2, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvFrame->data[2]);
//
//        /***
//        * 纹理更新完成后开始绘制
//        ***/
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        eglSwapBuffers(eglDisp, eglWindow);
        av_frame_free(&yuvFrame);

        LOGD("draw!");
    }
}

void *playdraw(void *data)
{
    UEGL *wlAudio = (UEGL *) data;
    wlAudio->init();
    pthread_exit(&wlAudio->thread_play);
}

void UEGL::play() {
//    draw();
    pthread_create(&thread_play, NULL, playdraw, this);
}

void UEGL::pause() {
    STATUS=2;
}
void UEGL::continue_() {
    STATUS=1;
}
void UEGL::destory(){
    /***
    * 释放资源
    * **/
    delete vertexData;
    delete textureVertexData;

    eglMakeCurrent(eglDisp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisp, eglCtx);
    eglDestroySurface(eglDisp, eglWindow);
    eglTerminate(eglDisp);
    eglDisp = EGL_NO_DISPLAY;
    eglWindow = EGL_NO_SURFACE;
    eglCtx = EGL_NO_CONTEXT;
}

double UEGL::getFrameDiffTime(AVFrame *avFrame) {
    double pts = av_frame_get_best_effort_timestamp(avFrame);
    if(pts == AV_NOPTS_VALUE){
        pts = 0;
    }
    pts *= av_q2d(time_base);
    if(pts > 0){
        clock = pts;
    }
    double diff = audio->clock - clock;
    return diff;
}

double UEGL::getDelayTime(double diff) {
    if(diff > 0.003){
        delayTime = delayTime * 2 / 3;
        if(delayTime < defaultDelayTime / 2){
            delayTime = defaultDelayTime * 2 / 3;
        }
        else if(delayTime > defaultDelayTime * 2){
            delayTime = defaultDelayTime * 2;
        }
    }
    else if(diff < - 0.003){
        delayTime = delayTime * 3 / 2;
        if(delayTime < defaultDelayTime / 2){
            delayTime = defaultDelayTime * 2 / 3;
        }
        else if(delayTime > defaultDelayTime * 2){
            delayTime = defaultDelayTime * 2;
        }
    }
    else if(diff == 0.003){

    }
    if(diff >= 0.5){
        delayTime = 0;
    }
    else if(diff <= -0.5){
        delayTime = defaultDelayTime * 2;
    }

    if(fabs(diff) >= 10){
        delayTime = defaultDelayTime;
    }
    return delayTime;
}
