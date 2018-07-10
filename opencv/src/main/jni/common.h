//
// Created by frendy on 2018/7/10.
//

#ifndef CVDROID_COMMON_H
#define CVDROID_COMMON_H

#include <opencv2/opencv.hpp>

jobject mat_to_bitmap(JNIEnv * env, cv::Mat & src, bool needPremultiplyAlpha, jobject bitmap_config);

#endif //CVDROID_COMMON_H
