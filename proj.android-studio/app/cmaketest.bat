
@echo off
set current_dir=%cd%

if not exist tmpNativeBuild md tmpNativeBuild
cd tmpNativeBuild
:: rd /s /q %cd%
@echo on
rem cmake -G "Visual Studio 15 2017 Win64" ../
cmake -DBOOST_DIR=E\:\\ext\\Boost-for-Android\\build\\boost\\1.65.1 ../
@echo off

cd %current_dir%



rem @echo on
rem D:/Android/sdk/cmake/3.6.4111459/bin/cmake.exe -G "Visual Studio 15 2017 Win64" -HE:/ext/xpy/proj.android-studio/app -BE:/ext/xpy/proj.android-studio/app/.externalNativeBuild/cmake/debug/armeabi-v7a -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-15 -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=E:/ext/xpy/proj.android-studio/app/build/intermediates/cmake/debug/obj/armeabi-v7a -DCMAKE_BUILD_TYPE=Debug -DANDROID_NDK=D:/Android/sdk/ndk-bundle -DCMAKE_CXX_FLAGS=-std=c++14 -D__STDC_FORMAT_MACROS -fexceptions -frtti -DCMAKE_TOOLCHAIN_FILE=D:/Android/sdk/ndk-bundle/build/cmake/android.toolchain.cmake -DCMAKE_MAKE_PROGRAM=D:/Android/sdk/cmake/3.6.4111459/bin/ninja.exe -GAndroid Gradle - Ninja -DANDROID_ARM_NEON=TRUE -DANDROID_TOOLCHAIN=clang -DANDROID_NATIVE_API_LEVEL=26 ./
rem @echo off


