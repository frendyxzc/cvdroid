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
#define APPNAME "cvdroid"
using namespace cv;
using namespace std;


int min(int x, int y) {
    return (x <= y) ? x : y;
}
int max(int x,int y){
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

