//
// Created by frendy on 2018/7/10.
//

#ifndef CVDROID_COMMON_H
#define CVDROID_COMMON_H

#include <opencv2/opencv.hpp>

int min(int x, int y);
int max(int x,int y);
int alpha(int color);
int red(int color);
int green(int color);
int blue(int color);
int ARGB(int alpha, int red, int green, int blue);

jobject mat_to_bitmap(JNIEnv * env, cv::Mat & src, bool needPremultiplyAlpha, jobject bitmap_config);

cv::Vec4b get_subpixel4(const cv::Mat& src, cv::Point2f pt);

#endif //CVDROID_COMMON_H
