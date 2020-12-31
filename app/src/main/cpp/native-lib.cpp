#include <jni.h>

#include <stdio.h>
#include <string>
#include <sstream>

#include <android/log.h>
#include <android/bitmap.h>

#include "GonzalesThinning.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_ctyeung_skeletonex_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}