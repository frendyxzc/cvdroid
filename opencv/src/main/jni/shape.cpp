//
// Created by frendy on 2018/7/10.
//
#include "vip_frendy_opencv_OpenCVManager.h"
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
#define APPNAME "shape"
using namespace cv;
using namespace std;


jintArray Java_vip_frendy_opencv_OpenCVManager_toEnlarge
        (JNIEnv* env,jobject thiz, jintArray buf, jint width, jint height, jint centerX, jint centerY, jint radius, jfloat multiple)
{
    jint * cbuf;
    cbuf = env->GetIntArrayElements(buf, 0);

    int newSize = width * height;
    jint rbuf[newSize]; // 新图像像素值

    float xishu = multiple;
    int real_radius = (int)(radius / xishu);

    int i = 0, j = 0;
    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            int curr_color = cbuf[j * width + i];

            int pixR = red(curr_color);
            int pixG = green(curr_color);
            int pixB = blue(curr_color);
            int pixA = alpha(curr_color);

            int newR = pixR;
            int newG = pixG;
            int newB = pixB;
            int newA = pixA;

            int distance = (int) ((centerX - i) * (centerX - i) + (centerY - j) * (centerY - j));
            if (distance < radius * radius)
            {
                // 放大镜的凹凸效果
                int src_x = (int) ((float) (i - centerX) / xishu);
                int src_y = (int) ((float) (j - centerY) / xishu);
                src_x = (int)(src_x * (sqrt(distance) / real_radius));
                src_y = (int)(src_y * (sqrt(distance) / real_radius));
                src_x = src_x + centerX;
                src_y = src_y + centerY;

                int src_color = cbuf[src_y * width + src_x];
                newR = red(src_color);
                newG = green(src_color);
                newB = blue(src_color);
                newA = alpha(src_color);
            }

            newR = min(255, max(0, newR));
            newG = min(255, max(0, newG));
            newB = min(255, max(0, newB));
            newA = min(255, max(0, newA));

            int modif_color = ARGB(newA, newR, newG, newB);
            rbuf[j * width + i] = modif_color;
        }
    }

    jintArray result = env->NewIntArray(newSize);
    env->SetIntArrayRegion(result, 0, newSize, rbuf);
    env->ReleaseIntArrayElements(buf, cbuf, 0);
    return result;
}

JNIEXPORT jobject JNICALL Java_vip_frendy_opencv_OpenCVManager_toStretch
        (JNIEnv *env, jobject thiz, jobject bitmap)
{
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "toStretch");
    int ret;
    AndroidBitmapInfo info;
    void* pixels = 0;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 )
    {       __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Bitmap format is not RGBA_8888!");
        return NULL;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    Mat mbgra(info.height, info.width, CV_8UC4, pixels);
    //init our output image
    Mat dst = mbgra.clone();

    //拉伸变换
    std::vector<cv::Point2f> src_corners(4);
    std::vector<cv::Point2f> dst_corners(4);

    src_corners[0]= cv::Point2f(0, 0);
    src_corners[1]= cv::Point2f(dst.cols - 1, 0);
    src_corners[2]= cv::Point2f(0, dst.rows - 1);
    src_corners[3]= cv::Point2f(dst.cols - 1, dst.rows - 1);

    dst_corners[0] = cv::Point2f(dst.cols / 4, 0);
    dst_corners[1] = cv::Point2f(dst.cols * 3 / 4, 0);
    dst_corners[2] = cv::Point2f(0, dst.rows - 1);
    dst_corners[3] = cv::Point2f(dst.cols - 1, dst.rows - 1);

    cv::Mat transMtx = cv::getPerspectiveTransform(src_corners, dst_corners);
    cv::warpPerspective(dst, dst, transMtx, cv::Size(dst.cols - 1, dst.rows - 1));

    //get source bitmap's config
    jclass java_bitmap_class = (jclass)env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetMethodID(java_bitmap_class, "getConfig", "()Landroid/graphics/Bitmap$Config;");
    jobject bitmap_config = env->CallObjectMethod(bitmap, mid);
    jobject _bitmap = mat_to_bitmap(env,dst,false,bitmap_config);

    AndroidBitmap_unlockPixels(env, bitmap);
    return _bitmap;

}

//变形-圆筒
Mat* cylinder(Mat& src)
{
    //Project from a cylindrical background onto a flat screen
    //Keeps width of the stretched image same as the input image
    const int height = src.rows;
    const int width  = src.cols;

    const float r  = (float)width * 0.5f;    //cylinder radius ((width / 2) <= r < inf)
    const float zf = hypot((float)height, (float)width);
    //distance from camera to screen (default: hypothenuse)
    const float xf = (float)width * 0.5f;    //distance from camera to left edge
    const float yf = (float)height * 0.75f;  //distance from camera to ceiling

    //precompute some constants
    const float half_width = (float)width * 0.5f;
    const float zfr = cos(half_width / r); //distance from cylinder center to projection plane
    const float xfr = sin(half_width / r);
    const float x_scale = r * xfr / half_width;
    const int adj_height = (int)ceil((float)height / x_scale);

    Mat *dst = new cv::Mat(cv::Mat::zeros(adj_height, width, src.type()));

    for (int i = 0; i < width; i++)
    {
        //X-axis (columns)
        const float zb = r * (cos((half_width - (float)i) / r) - zfr);
        const float z_ratio = (zb + zf) / zf;

        const float i_src = r * asin(x_scale * ((float)i - xf) * z_ratio / r) + half_width;
        for (int j = 0; j < adj_height; j++)
        {
            //Y-axis (rows)
            const float j_src = yf + z_ratio * (x_scale * (float)j - yf);
            if (i_src >= 0.0f && i_src <= (float)width &&
                j_src >= 0.0f && j_src <= (float)height)
            {
                dst->at<Vec4b>(j, i) = get_subpixel4(src, cv::Point2f(i_src, j_src));
            }
        }
    }
    return dst;
}

JNIEXPORT jobject JNICALL Java_vip_frendy_opencv_OpenCVManager_toCylinder
        (JNIEnv *env, jobject thiz, jobject bitmap)
{
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "toCylinder");
    int ret;
    AndroidBitmapInfo info;
    void* pixels = 0;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 )
    {       __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Bitmap format is not RGBA_8888!");
        return NULL;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    Mat mbgra(info.height, info.width, CV_8UC4, pixels);
    //init our output image
    Mat dst = mbgra.clone();

    dst = *(cylinder(dst));

    //拉伸变换
//    std::vector<cv::Point2f> src_corners(4);
//    std::vector<cv::Point2f> dst_corners(4);
//
//    src_corners[0]= cv::Point2f(0, 0);
//    src_corners[1]= cv::Point2f(dst.cols - 1, 0);
//    src_corners[2]= cv::Point2f(0, dst.rows - 1);
//    src_corners[3]= cv::Point2f(dst.cols - 1, dst.rows - 1);
//
//    dst_corners[0] = cv::Point2f(dst.cols / 4, 0);
//    dst_corners[1] = cv::Point2f(dst.cols * 3 / 4, 0);
//    dst_corners[2] = cv::Point2f(0, dst.rows - 1);
//    dst_corners[3] = cv::Point2f(dst.cols - 1, dst.rows - 1);
//
//    cv::Mat transMtx = cv::getPerspectiveTransform(src_corners, dst_corners);
//    cv::warpPerspective(dst, dst, transMtx, cv::Size(dst.cols - 1, dst.rows - 1));

    //get source bitmap's config
    jclass java_bitmap_class = (jclass)env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetMethodID(java_bitmap_class, "getConfig", "()Landroid/graphics/Bitmap$Config;");
    jobject bitmap_config = env->CallObjectMethod(bitmap, mid);
    jobject _bitmap = mat_to_bitmap(env,dst,false,bitmap_config);

    AndroidBitmap_unlockPixels(env, bitmap);
    return _bitmap;

}