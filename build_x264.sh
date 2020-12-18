#这里的路径需要修改为你自己的ndk路径
NDK=/Users/apple/Desktop/android-ndk-r14b
#注意android-9文件夹的版本号，替换好自己的版本号
SYSROOT=$NDK/platforms/android-21/arch-arm64/
#把这里的路径替换好自己的版本
TOOLCHAIN=$NDK/toolchains/aarch64-linux-android-4.9/prebuilt/darwin-x86_64
function build_one
{
  ./configure \
--prefix=$PREFIX \
--cross-prefix=$TOOLCHAIN/bin/aarch64-linux-android- \
--sysroot=$SYSROOT \
--host=aarch64-linux \
--enable-pic \
--enable-shared \
--enable-static \
--disable-cli
make
make install

}
CPU=arm
PREFIX=$(pwd)/android_ndk_x264/$CPU
build_one