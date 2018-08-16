#include "vip_frendy_opencv_OpenCVManager.h"
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <android/bitmap.h>
#include "common.h"
#include "ColorBlobDetector.h"
#include "ProcessImageData.h"
#define APPNAME "cvdroid"
using namespace cv;
using namespace std;

//锐化
void sharpen(const Mat& myImage, Mat& result)
{
    CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images
    const int nChannels = myImage.channels();
    result.create(myImage.size(),myImage.type());
    for(int j = 1 ; j < myImage.rows-1; ++j)
    {
        const uchar* previous = myImage.ptr<uchar>(j - 1);
        const uchar* current  = myImage.ptr<uchar>(j    );
        const uchar* next     = myImage.ptr<uchar>(j + 1);
        uchar* output = result.ptr<uchar>(j);
        for(int i= nChannels;i < nChannels*(myImage.cols-1); ++i)
        {
            *output++ = saturate_cast<uchar>(5*current[i]
                                             -current[i-nChannels] - current[i+nChannels] - previous[i] - next[i]);
        }
    }
    result.row(0).setTo(Scalar(0));
    result.row(result.rows-1).setTo(Scalar(0));
    result.col(0).setTo(Scalar(0));
    result.col(result.cols-1).setTo(Scalar(0));
}

//圆形遮罩
Mat circularMask(Mat img, int row, int col, int r) {
    Mat cirMask = img.clone();
    cirMask.setTo(Scalar::all(0));
    circle(cirMask, Point(col, row), r, Scalar(255, 255, 255), -1, 8, 0);
    return cirMask;
}


JNIEXPORT jobject JNICALL Java_vip_frendy_opencv_OpenCVManager_toBW
        (JNIEnv *env, jobject thiz, jobject bitmap)
{
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "toBW");
    int ret;
    AndroidBitmapInfo info;
    void* pixels = 0;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 )
    {       __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"Bitmap format is not RGBA_8888!");
        return NULL;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    Mat mbgra(info.height, info.width, CV_8UC4, pixels);
    // init our output image
    Mat dst = mbgra.clone();

    cvtColor(mbgra, dst, CV_BGR2GRAY);

    //threshold(dst,dst,0,255,THRESH_BINARY | THRESH_OTSU);
    adaptiveThreshold(dst, dst, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 25, 13);

    //get source bitmap's config
    jclass java_bitmap_class = (jclass)env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetMethodID(java_bitmap_class, "getConfig", "()Landroid/graphics/Bitmap$Config;");
    jobject bitmap_config = env->CallObjectMethod(bitmap, mid);
    jobject _bitmap = mat_to_bitmap(env,dst,false,bitmap_config);

    AndroidBitmap_unlockPixels(env, bitmap);
    return _bitmap;

}

//TODO: 背景虚化，目前通过指定的遮罩来实现，后续可添加边缘检测抠出遮罩
JNIEXPORT jobject JNICALL Java_vip_frendy_opencv_OpenCVManager_toBokeh
        (JNIEnv *env, jobject thiz, jobject bitmap, jint x, jint y, jint w, jint h, jint blurSize, jint type)
{
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "toBokeh");
    int ret;
    AndroidBitmapInfo info;
    void* pixels = 0;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 )
    {       __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"Bitmap format is not RGBA_8888!");
        return NULL;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    Mat mbgra(info.height, info.width, CV_8UC4, pixels);
    //init our output image
    Mat img = mbgra.clone();
    Mat dst = mbgra.clone();

    /* 部分区域模糊
    //选择和截取一段行范围的图片
    Mat target = dst.rowRange(info.height / 3, 2 * info.width / 3);
    //均值滤波
    blur(target, target, Size(85, 85));
    //将opencv图片转化成c图片数据，RGBA转化成灰度图4通道颜色数据
    cvtColor(target, target, CV_RGBA2GRAY, 4);
    */

    //利用遮罩来实现
    Mat mask;
    Rect rect(x, y, w, h);
    //zero为与image相同大小的全0图像
    mask = Mat::zeros(img.size(), CV_8UC1);
    //把mask图像的rect区域设为255，即rect大小的白色块
    mask(rect).setTo(255);
    //滤波dst
    if(type == 1) {
        //高斯
        if(blurSize % 2 == 0) blurSize += 1;
        GaussianBlur(dst, dst, Size(blurSize, blurSize), 0);
    } else {
        //均值
        blur(dst, dst, Size(blurSize, blurSize));
    }
    //把img与mask合并保存到dst中，即img与mask的非0区域合并
    img.copyTo(dst, mask);

    //get source bitmap's config
    jclass java_bitmap_class = (jclass)env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetMethodID(java_bitmap_class, "getConfig", "()Landroid/graphics/Bitmap$Config;");
    jobject bitmap_config = env->CallObjectMethod(bitmap, mid);
    jobject _bitmap = mat_to_bitmap(env, dst, false, bitmap_config);

    AndroidBitmap_unlockPixels(env, bitmap);
    return _bitmap;

}

JNIEXPORT jobject JNICALL Java_vip_frendy_opencv_OpenCVManager_toBokehWithCircle
        (JNIEnv *env, jobject thiz, jobject bitmap, jint r, jint blurSize, jint type)
{
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "toBokehWithCircle");
    int ret;
    AndroidBitmapInfo info;
    void* pixels = 0;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 )
    {       __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"Bitmap format is not RGBA_8888!");
        return NULL;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    Mat mbgra(info.height, info.width, CV_8UC4, pixels);
    //init our output image
    Mat img = mbgra.clone();
    Mat dst = mbgra.clone();

    //利用圆形遮罩来实现
    Mat mask = circularMask(img, img.rows / 2, img.cols / 2, r);
    //滤波dst
    if(type == 1) {
        //高斯
        if(blurSize % 2 == 0) blurSize += 1;
        GaussianBlur(dst, dst, Size(blurSize, blurSize), 0);
    } else {
        //均值
        blur(dst, dst, Size(blurSize, blurSize));
    }
    //把img与mask合并保存到dst中，即img与mask的非0区域合并
    img.copyTo(dst, mask);

    //get source bitmap's config
    jclass java_bitmap_class = (jclass)env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetMethodID(java_bitmap_class, "getConfig", "()Landroid/graphics/Bitmap$Config;");
    jobject bitmap_config = env->CallObjectMethod(bitmap, mid);
    jobject _bitmap = mat_to_bitmap(env, dst, false, bitmap_config);

    AndroidBitmap_unlockPixels(env, bitmap);
    return _bitmap;

}

int detect(Mat frame, CascadeClassifier cascade) {
    Mat frame_gray;
    vector<Rect> detections;

    cvtColor(frame, frame_gray, CV_BGR2GRAY);

    cascade.detectMultiScale(frame_gray, detections,
                             1.01, 10, 0 | CV_HAAR_SCALE_IMAGE, Size(40, 90));
    int detected = (int)detections.size();
    for (int i = 0; i < detected; i++) {
        int width2 = detections[i].width / 2;
        int height2 = detections[i].height / 2;
        Point center(detections[i].x + width2, detections[i].y + height2);
        ellipse(frame, center, Size(width2, height2), 0, 0, 360,
                Scalar(255, 0, 255), 2, 5, 0);
    }
    return(detected);
}

JNIEXPORT jint JNICALL Java_vip_frendy_opencv_OpenCVManager_classifier
        (JNIEnv *env, jobject thiz, jobject bitmap, jstring cascadePath)
{
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "classifier");
    int ret;
    AndroidBitmapInfo info;
    void* pixels = 0;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 )
    {       __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"Bitmap format is not RGBA_8888!");
        return NULL;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    Mat mbgra(info.height, info.width, CV_8UC4, pixels);
    // init our output count
    int retCount = 0;
    Mat dst = mbgra.clone();

    const char* cascade_path = env->GetStringUTFChars(cascadePath, 0);

    CascadeClassifier cascade;
    if (!cascade.load(std::string(cascade_path))) {
        return(-1);
    }
    retCount = detect(dst, cascade);
    env->ReleaseStringUTFChars(cascadePath, cascade_path);

    AndroidBitmap_unlockPixels(env, bitmap);
    return retCount;
}

JNIEXPORT jint JNICALL Java_vip_frendy_opencv_OpenCVManager_getFingerCount
        (JNIEnv *env, jobject thiz, jobject bitmap, jint iThreshold)
{
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "getFingerCount");
    int ret;
    AndroidBitmapInfo info;
    void* pixels = 0;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 )
    {       __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"Bitmap format is not RGBA_8888!");
        return NULL;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    Mat mbgra(info.height, info.width, CV_8UC4, pixels);
    // init our output count
    int retCount = 0;
    Mat dst = mbgra.clone();

    GaussianBlur(dst, dst, Size(3, 3), 0);

    ColorBlobDetector* detector = new ColorBlobDetector();
    detector->setPoint(cv::Point(info.width / 2, info.height / 2));

    ProcessImageData _temp(cv::Point(info.width / 2, info.height / 2));
    detector->setHsvColor(_temp.calculateHsv(dst));
    detector->processImage(dst);
    retCount = detector->mContours.size();

    if(retCount <= 0) {
        AndroidBitmap_unlockPixels(env, bitmap);
        return retCount;
    }

    RotatedRect rect = minAreaRect(detector->mContours[0]);
    double boundWidth = rect.size.width;
    double boundHeight = rect.size.height;
    int boundPos = 0;

    for (int i = 1; i < detector->mContours.size(); i++) {
        rect = minAreaRect(detector->mContours[i]);
        if (rect.size.width * rect.size.height > boundWidth * boundHeight) {
            boundWidth = rect.size.width;
            boundHeight = rect.size.height;
            boundPos = i;
        }
    }

    Rect boundRect = boundingRect(detector->mContours[boundPos]);
    double a = boundRect.br().y - boundRect.tl().y;
    a = a * 0.7;
    a = boundRect.tl().y + a;

    std::vector<Point> pointMat = std::vector<Point>();
    approxPolyDP(detector->mContours[boundPos], pointMat, 3, true);
    detector->mContours[boundPos] = pointMat;

    std::vector<int> hull = std::vector<int>();
    std::vector<Vec4i> convexDefect = std::vector<Vec4i>();
    convexHull(detector->mContours[boundPos], hull);

    if(hull.size() < 3) {
        AndroidBitmap_unlockPixels(env, bitmap);
        return 0;
    }

    convexityDefects(detector->mContours[boundPos], hull, convexDefect);

    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"convexDefect size = %d", convexDefect.size());

    retCount = 0;

    for (int j = 0; j < convexDefect.size(); j++) {
        Vec4i &def = convexDefect.at(j);

        Point farPoint = detector->mContours[boundPos][def[2]];
        int depth = def[3];
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"depth = %d", depth);
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"farPoint.y = %d, a = %f", farPoint.y, a);
        if(depth > iThreshold && farPoint.y < a){
            retCount += 1;
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,"ret count = %d", retCount);
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    return retCount;
}