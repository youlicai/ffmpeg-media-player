cmake_minimum_required(VERSION 3.4.1)

include_directories( /include)

# 设置cpp目录路径
set(CPP_DIR .)
# 设置jniLibs目录路径
set(LIBS_DIR /jniLibs)
add_library(native-lib
            SHARED
            cpp/native-lib.cpp
            cpp/UPlayer.cpp
            cpp/UAudio.cpp
            cpp/UVideo.cpp
            cpp/UEGL.cpp
            cpp/ShaderUtils.cpp
            cpp/UOpenSLES.cpp
            cpp/UQueue.cpp
            cpp/UDecode.cpp
            cpp/UCallJava.cpp
            cpp/UCut.cpp
            cpp/UFilters.cpp
            cpp/UChange.cpp
            cpp/ULog.h
        )
# 用于各种类型声音、图像编解码
add_library( # 库名称
        libavcodec
        # 动态库，生成so文件
        SHARED
        # 表示该库是引用的不是生成的
        IMPORTED )

# 引用库文件
set_target_properties(
        # 库名称
        libavcodec
        # 库的路径
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libavcodec.so)

# 用于各种音视频封装格式的生成和解析，读取音视频帧等功能
add_library( libavformat
        SHARED
        IMPORTED )

set_target_properties( libavformat
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libavformat.so)

# 包含一些公共的工具函数
add_library( libavutil
        SHARED
        IMPORTED )

set_target_properties( libavutil
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libavutil.so)

# 提供了各种音视频过滤器
add_library( libavfilter
        SHARED
        IMPORTED )

set_target_properties( libavfilter
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libavfilter.so)

# 用于音频重采样，采样格式转换和混合
add_library(libswresample SHARED IMPORTED )

set_target_properties(libswresample PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libswresample.so)

# 用于视频场景比例缩放、色彩映射转换
add_library( libswscale
        SHARED
        IMPORTED )

set_target_properties( libswscale
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libswscale.so)
# 用于音频重采样，采样格式转换和混合
add_library(libx264 SHARED IMPORTED )

set_target_properties(libx264 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libx264.161.so)

add_library(proc SHARED IMPORTED )

set_target_properties(proc PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libpostproc.so)

find_library( # Sets the name of the path variable.
              log-lib
              log )


target_link_libraries( # Specifies the target library.
        native-lib
        libswresample
        libavutil
                        libavcodec
                        libavfilter
                        libswscale
                        libavformat
        libx264
        proc
        EGL
        android
        GLESv2
        OpenSLES

                       ${log-lib} )