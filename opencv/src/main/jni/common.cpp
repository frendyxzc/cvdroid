//
// Created by frendy on 2018/7/10.
//
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <android/bitmap.h>
#include "common.h"
using namespace cv;
using namespace std;

int min(int x, int y) {
    return (x <= y) ? x : y;
}
int max(int x,int y) {
    return (x >= y) ? x : y;
}
int alpha(int color) {
    return (color >> 24) & 0xFF;
}
int red(int color) {
    return (color >> 16) & 0xFF;
}
int green(int color) {
    return (color >> 8) & 0xFF;
}
int blue(int color) {
    return color & 0xFF;
}
int ARGB(int alpha, int red, int green, int blue) {
    return (alpha << 24) | (red << 16) | (green << 8) | blue;
}


//mat转bitmap
jobject mat_to_bitmap(JNIEnv * env, Mat & src, bool needPremultiplyAlpha, jobject bitmap_config) {
    jclass java_bitmap_class = (jclass)env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetStaticMethodID(java_bitmap_class,
                                           "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    jobject bitmap = env->CallStaticObjectMethod(java_bitmap_class,
                                                 mid, src.size().width, src.size().height, bitmap_config);
    AndroidBitmapInfo  info;
    void*              pixels = 0;

    try {
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(src.type() == CV_8UC1 || src.type() == CV_8UC3 || src.type() == CV_8UC4);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        if(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888){
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if(src.type() == CV_8UC1){
                cvtColor(src, tmp, CV_GRAY2RGBA);
            }else if(src.type() == CV_8UC3){
                cvtColor(src, tmp, CV_RGB2RGBA);
            }else if(src.type() == CV_8UC4){
                if(needPremultiplyAlpha){
                    cvtColor(src, tmp, COLOR_RGBA2mRGBA);
                }else{
                    src.copyTo(tmp);
                }
            }
        }else{
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            if(src.type() == CV_8UC1){
                cvtColor(src, tmp, CV_GRAY2BGR565);
            }else if(src.type() == CV_8UC3){
                cvtColor(src, tmp, CV_RGB2BGR565);
            }else if(src.type() == CV_8UC4){
                cvtColor(src, tmp, CV_RGBA2BGR565);
            }
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return bitmap;
    }catch(cv::Exception e){
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("org/opencv/core/CvException");
        if(!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return bitmap;
    }catch (...){
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nMatToBitmap}");
        return bitmap;
    }
}


Vec4b get_subpixel4(const cv::Mat& src, cv::Point2f pt) {
    //Simple bilinear interpolation
    const int x = (int)pt.x;
    const int y = (int)pt.y;

    const int x0 = cv::borderInterpolate(x,     src.cols, cv::BORDER_REFLECT_101);
    const int x1 = cv::borderInterpolate(x + 1, src.cols, cv::BORDER_REFLECT_101);
    const int y0 = cv::borderInterpolate(y,     src.rows, cv::BORDER_REFLECT_101);
    const int y1 = cv::borderInterpolate(y + 1, src.rows, cv::BORDER_REFLECT_101);

    const float a = pt.x - (float)x;
    const float c = pt.y - (float)y;

    const float one_minus_a = 1.f - a;
    const float one_minus_c = 1.f - c;

    const Vec4b y0_x0 = src.at<Vec4b>(y0, x0);
    const Vec4b y1_x0 = src.at<Vec4b>(y1, x0);
    const Vec4b y0_x1 = src.at<Vec4b>(y0, x1);
    const Vec4b y1_x1 = src.at<Vec4b>(y1, x1);

    const uchar b = (uchar)cvRound(
            (one_minus_a * (float)(y0_x0[0]) + a * (float)(y0_x1[0])) * one_minus_c +
            (one_minus_a * (float)(y1_x0[0]) + a * (float)(y1_x1[0])) * c
    );
    const uchar g = (uchar)cvRound(
            (one_minus_a * (float)(y0_x0[1]) + a * (float)(y0_x1[1])) * one_minus_c +
            (one_minus_a * (float)(y1_x0[1]) + a * (float)(y1_x1[1])) * c
    );
    const uchar r = (uchar)cvRound(
            (one_minus_a * (float)(y0_x0[2]) + a * (float)(y0_x1[2])) * one_minus_c +
            (one_minus_a * (float)(y1_x0[2]) + a * (float)(y1_x1[2])) * c
    );
    const uchar t = (uchar)cvRound(
            (one_minus_a * (float)(y0_x0[3]) + a * (float)(y0_x1[3])) * one_minus_c +
            (one_minus_a * (float)(y1_x0[3]) + a * (float)(y1_x1[3])) * c
    );
    return Vec4b(b, g, r, t);
}
