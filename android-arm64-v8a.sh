#!/bin/bash
make clean
export NDK_HOME=/Users/apple/Desktop/android-ndk-r14b
    export NDK_HOST_PLATFORM=darwin-x86_64
    
    COMMON_OPTIONS="\
        --prefix=android/ \
        --target-os=android \
        --disable-static \
        --enable-shared \
        --enable-gpl \
        --enable-ffmpeg \
        --enable-cross-compile \
        --disable-ffplay \
        --disable-ffprobe \
        --disable-doc \
        --disable-symver \
        --disable-yasm \
        "
 function build_android {

     ./configure \
        --libdir=${PREFIX}/libs/arm64-v8a \
        --incdir=${PREFIX}/includes/arm64-v8a \
        --pkgconfigdir=${PREFIX}/pkgconfig/arm64-v8a \
        --arch=aarch64 \
        --cpu=armv8-a \
        --cross-prefix="${NDK_HOME}/toolchains/aarch64-linux-android-4.9/prebuilt/${NDK_HOST_PLATFORM}/bin/aarch64-linux-android-" \
        --sysroot="${NDK_HOME}/platforms/android-24/arch-arm64/" \
        --extra-ldexeflags=-pie \
        --extra-cflags="-I/Users/apple/Desktop/x264-master/android_ndk_x264/arm/include" 
        --extra-ldflags="-L/Users/apple/Desktop/x264-master/android_ndk_x264/arm/lib"
        ${COMMON_OPTIONS} 
        make clean
        make -j8 
        make install
 };

    PREFIX=$(pwd)/android/$CPU 
    build_android

