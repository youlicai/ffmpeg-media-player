#include <jni.h>
#include <string>
#include "android/log.h"
#include "UPlayer.h"
#include "ShaderUtils.h"
#include "UCallJava.h"
#include "UCut.h"


extern  "C"{
#include "libavformat/avformat.h"
}

#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR, "ffmpeg-invoke", __VA_ARGS__)
extern "C"
JNIEXPORT jstring JNICALL
Java_onlyou_pw_faceu_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    LOGD("00000");
    std::string hello = "Hello from C++0000";
    av_register_all();
    char info[40000] = {0};

    AVInputFormat *if_temp = av_iformat_next(NULL);
    AVOutputFormat *of_temp = av_oformat_next(NULL);
//    while (if_temp != NULL) {
//        LOGD("00000 %s",if_temp->name);
//        sprintf(info, "%sInput: %s\n", info, if_temp->name);
//        if_temp = if_temp->next;
//    }
//    while (of_temp != NULL) {
//        sprintf(info, "%sOutput: %s\n", info, of_temp->name);
//        of_temp = of_temp->next;
//    }

    return env->NewStringUTF("dddddddddd");
}
//extern "C"
//JNIEXPORT void JNICALL
//Java_onlyou_pw_faceu_MainActivity_test(JNIEnv *env, jobject thiz, jstring path_in,
//                                       jstring path_out) {
//    const char *src = env->GetStringUTFChars(path_in, 0);
//    const char *out = env->GetStringUTFChars(path_out, 0);
//    UPlayer *player=new UPlayer();
//    player->init(src,out);
//}


#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <jni.h>
#include <android/native_window_jni.h>
#include<Android/log.h>
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
}

#define GET_STR(x) #x
const char *vertexShaderString1 = GET_STR(
        attribute vec4 aPosition;
        attribute vec2 aTexCoord;
        varying vec2 vTexCoord;
        void main() {
            vTexCoord=vec2(aTexCoord.x,1.0-aTexCoord.y);
            gl_Position = aPosition;
        }
);
const char *fragmentShaderString1 = GET_STR(
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

_JavaVM *javaVM = NULL;


bool nexit = true;
pthread_t thread_start;


extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if(vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK)
    {

        return result;
    }
    return JNI_VERSION_1_4;

}


UPlayer *player=new UPlayer();
UCallJava *calljava;
extern "C"
JNIEXPORT void JNICALL
Java_onlyou_pw_faceu_MainActivity_play(JNIEnv *env, jobject thiz, jstring path_in, jstring path_out,
                                       jobject sur) {
    const char *src = env->GetStringUTFChars(path_in, 0);
    const char *out = env->GetStringUTFChars(path_out, 0);

    calljava=new UCallJava(javaVM, env, &thiz);
    player->addcalljava(calljava);
    player->play(env,src,out,sur);
}extern "C"
JNIEXPORT void JNICALL
Java_onlyou_pw_faceu_MainActivity_pause(JNIEnv *env, jobject thiz) {
    player->pause();
}extern "C"
JNIEXPORT void JNICALL
Java_onlyou_pw_faceu_MainActivity_continue_1(JNIEnv *env, jobject thiz) {
    // TODO: implement continue_()
    player->continue_();
}extern "C"
JNIEXPORT void JNICALL
Java_onlyou_pw_faceu_MainActivity_forword(JNIEnv *env, jobject thiz, jint sec) {
    player->seek(sec);
}extern "C"
JNIEXPORT void JNICALL
Java_onlyou_pw_faceu_MainActivity_back(JNIEnv *env, jobject thiz, jint sec) {
    // TODO: implement back()
}extern "C"
JNIEXPORT void JNICALL
Java_onlyou_pw_faceu_MainActivity_cut(JNIEnv *env, jobject thiz) {
    player->cut();
}



void SaveFrame(AVFrame *pFrame, int width, int height,int index)
{
    FILE *pFile;
    char szFilename[32];
    int  x, y;
    uint8_t *pTemp;

//    sprintf(szFilename, "debug/frame%d.ppm", index);
    pFile=fopen("/storage/emulated/0/ffmpeg.jpg", "wb+");

    if(pFile == NULL)
        return;

    //写文件头, ppm格式: https://blog.csdn.net/MACMACip/article/details/105378600
//    fprintf(pFile, "P6 %d %d 255", width, height);

    /**
     * 写像素数据
     * 使用for(y=0; y<height; y++){
     *     fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
     * } 也可, 如果你的RGB排布刚好是正确的像素的话~
     */
    for(y=0; y<height; y++) {
        pTemp = pFrame->data[0] + y*pFrame->linesize[0];
        for(x=0; x<width; x++) {
            fwrite(pTemp+2, 1, 1, pFile);
            fwrite(pTemp+0, 1, 1, pFile);
            fwrite(pTemp+1, 1, 1, pFile);
            pTemp += 3;
        }
    }

    fclose(pFile);
}


#define START_FRAME 100
extern "C"
JNIEXPORT void JNICALL
Java_onlyou_pw_faceu_MainActivity_test(JNIEnv *env, jobject thiz,jstring path_src) {
    //获取输入输出文件名
    const char *file_path =  env->GetStringUTFChars(path_src, 0);
LOGD("path: %s",file_path);
    int ret = -1, index = 0, image_size = -1;
    int videoStream = -1, got_picture = -1, numBytes= -1;
    uint8_t *out_buffer = NULL;

    AVFormatContext *pFormatCtx = NULL;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    AVPacket *packet = NULL;
    AVFrame *pFrame = NULL, *pFrameRGB = NULL;
    struct SwsContext *img_convert_ctx = NULL;

    av_register_all();
    pFormatCtx = avformat_alloc_context();
    if(NULL == pFormatCtx){
        return ;
    }

    // 1. 打开视频文件;
    ret = avformat_open_input(&pFormatCtx, file_path, NULL, NULL);
    if(ret < 0){
        avformat_free_context(pFormatCtx);
        return;
    }

    // 2. 读取视频文件信息;
    ret = avformat_find_stream_info(pFormatCtx, NULL);
    if (ret < 0) {
        return;
    }

    // 3. 获取视频流
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
        }
    }
    if (videoStream == -1) {

        return;
    }

    // 4. 根据上面得到的视频流类型打开对应的解码器
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        return;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        return;
    }

    // 5. 分配并初始化一个视频packet
    image_size = pCodecCtx->width * pCodecCtx->height;
    packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    if(!packet){
        return;
    }
    ret = av_new_packet(packet, image_size);
    if(ret < 0){
        return;
    }

    // 6. 分配两个frame分别存放yuv和rgb数据
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    if(!pFrame || !pFrameRGB){
        return;
    }

    // 7. 分配一个struct SwsContext结构体, 填充源图像和目标图像信息(为接下来的转换做准备)
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                     pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                                     AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

    // 8. 根据指定的图像参数和提供的数组设置数据指针和行宽(linesizes).  https://blog.csdn.net/MACMACip/article/details/105463390
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,pCodecCtx->height);
    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    ret = avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_RGB24,
                         pCodecCtx->width, pCodecCtx->height);
    if (!img_convert_ctx || numBytes<0 || !out_buffer || ret<0) {
        return;
    }

    while(1){
        if (av_read_frame(pFormatCtx, packet) < 0) {
            break;
        }
        if (packet->stream_index == videoStream) {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,packet);
            if (ret < 0) {
                printf("decode error.");
                return;
            }
        }


        AVFrame *pFrameYUV420P = av_frame_alloc();
        int num = av_image_get_buffer_size(
                AV_PIX_FMT_YUV420P,
                pCodecCtx->width,
                pCodecCtx->height,
                1);
        uint8_t *buffer = static_cast<uint8_t *>(av_malloc(num * sizeof(uint8_t)));
        av_image_fill_arrays(
                pFrameYUV420P->data,
                pFrameYUV420P->linesize,
                buffer,
                AV_PIX_FMT_YUV420P,
                pCodecCtx->width,
                pCodecCtx->height,
                1);
        SwsContext *sws_ctx = sws_getContext(
                pCodecCtx->width,
                pCodecCtx->height,
                pCodecCtx->pix_fmt,
                pCodecCtx->width,
                pCodecCtx->height,
                AV_PIX_FMT_YUV420P,
                SWS_BICUBIC, NULL, NULL, NULL);

        if(!sws_ctx)
        {
            av_frame_free(&pFrameYUV420P);
            av_free(pFrameYUV420P);
            av_free(buffer);
            continue;
        }
        sws_scale(
                sws_ctx,
                reinterpret_cast<const uint8_t *const *>(pFrame->data),
                pFrame->linesize,
                0,
                pFrame->height,
                pFrameYUV420P->data,
                pFrameYUV420P->linesize);

        LOGD("src frame height :%s",pFrame->data[0]);
        LOGD("dest frame height :%s",pFrameYUV420P->data[0]);
        av_free_packet(packet);

        if(index >START_FRAME+10){
            break;
        }
    }
}