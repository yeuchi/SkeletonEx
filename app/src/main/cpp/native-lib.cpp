/*
 * Reference:
 * https://www.ibm.com/developerworks/opensource/tutorials/os-androidndk/os-androidndk-pdf.pdf
 * https://stackoverflow.com/questions/4841345/sending-ints-between-java-and-c
 * ibmphotophun.c
 *
 * Original Author: Frank Ableson
 * Contact Info: fableson@msiservices.com
 */
#include <jni.h>

#include <stdio.h>
#include <string>
#include <sstream>

#include <android/log.h>
#include <android/bitmap.h>

#include "GonzalesThinning.h"

#define  LOG_TAG    "libibmphotophun"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


AndroidBitmapInfo  infoSource;
void*              pixelsSource;
AndroidBitmapInfo  infoDestination;
void*              pixelsDestination;
int                ret;

bool initializeBitmap(JNIEnv *env,
                      jobject bitmapSource)
{
    if ((ret = AndroidBitmap_getInfo(env, bitmapSource, &infoSource)) < 0)
    {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return false;
    }

    LOGI("source image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",infoSource.width,infoSource.height,infoSource.stride,infoSource.format,infoSource.flags);
    if (infoSource.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        LOGE("Bitmap format is not RGBA_8888 !");
        return false;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmapSource, &pixelsSource)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return false;
    }

    return true;
}

bool releaseBitmap(JNIEnv *env,
                   jobject bitmapSource)
{
    if(NULL==bitmapSource)
        return false;

    AndroidBitmap_unlockPixels(env, bitmapSource);
    return true;
}


bool initializeBitmaps(JNIEnv *env,
                       jobject bitmapSource,
                       jobject bitmapDestination)
{
    if ((ret = AndroidBitmap_getInfo(env, bitmapDestination, &infoDestination)) < 0)
    {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return false;
    }

    LOGI("convolved image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",infoDestination.width,infoDestination.height,infoDestination.stride,infoDestination.format,infoDestination.flags);
    if (infoDestination.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        //    if (infoconvolved.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return false;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmapDestination, &pixelsDestination)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return false;
    }

    return initializeBitmap(env, bitmapSource);
}

bool releaseBitmaps(JNIEnv *env,
                    jobject bitmapSource,
                    jobject bitmapDestination)
{
    if(NULL==bitmapDestination)
        return false;

    AndroidBitmap_unlockPixels(env, bitmapDestination);
    return releaseBitmap(env, bitmapSource);
}

int ThinOnce(JNIEnv *env,
              jobject obj,
              jobject bitmapsource,
              jobject bitmapresult)
{

    initializeBitmaps(env, bitmapsource, bitmapresult);

    GonzalesThinning thinning;
    int count = thinning.Apply(infoSource, pixelsSource, infoDestination, pixelsDestination);

    releaseBitmaps(env, bitmapsource, bitmapresult);
    LOGI("unlocking pixels");
    return count;
}

extern "C" JNIEXPORT int JNICALL
Java_com_ctyeung_skeletonex_MainActivity_imageThinOnceFromJNI(JNIEnv * env,
                                 jobject obj,
                                 jobject bmp_in,
                                 jobject bmp_out) {
    return ThinOnce(env, obj, bmp_in, bmp_out);
}
