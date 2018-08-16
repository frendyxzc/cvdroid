//
// Created by frendy on 2018/8/15.
//

#include "ColorBlobDetector.h"

ColorBlobDetector::ColorBlobDetector() { }

ColorBlobDetector::ColorBlobDetector(ColorBlobDetector *local, cv::Point _clickedAt) {
    this->clickedAt = _clickedAt;
}

void ColorBlobDetector::setHsvColor(cv::Scalar hsvColor) {
    this->hsvColor = hsvColor;
    //DLOG(INFO) << "Entered setHsvColor";

    double minH = (hsvColor.val[0] >= mColorRadius[0]) ? hsvColor.val[0] -
                                                         mColorRadius.val[0] : 0;

    double maxH = (hsvColor.val[0] <= 255) ? hsvColor.val[0] +
                                             mColorRadius.val[0] : 255;

    //Set bounds
    mLowerBound.val[0] = minH;
    //DLOG(INFO) << "mLowerBound val[0]" << mLowerBound.val[0];

    mUpperBound.val[0] = maxH;
    //DLOG(INFO) << "mUpperBound val[0]" << mUpperBound.val[0];

    mLowerBound.val[1] = hsvColor.val[1] - mColorRadius.val[1];
    //DLOG(INFO) << "mLowerBound val[1]" << mLowerBound.val[1];

    mUpperBound.val[1] = hsvColor.val[1] + mColorRadius.val[1];
    //DLOG(INFO) << "mUpperBound val[1]" << mUpperBound.val[1];

    mLowerBound.val[2] = hsvColor.val[2] - mColorRadius.val[2];
    //DLOG(INFO) << "mLowerBound val[2]" << mLowerBound.val[2];

    mUpperBound.val[2] = hsvColor.val[2] + mColorRadius.val[2];
    //DLOG(INFO) << "mUpperBound val[2]" << mUpperBound.val[2];

    mLowerBound.val[3] = 0;
    mLowerBound.val[3] = 255;

    //DLOG(INFO) << "Exited setHsvColor";
}

/**
 * @brief ColorBlobDetector::isContour
 *          Check if clickedPoint is inside desired contour
 *
 * @param _checkMe clickedPoint
 * @param _lContour Contour to be checked
 * @return true(1) When Point is inside contour
 */
bool ColorBlobDetector::isContour(cv::Point _checkMe, std::vector<cv::Point> _lContour) {
    double r = cv::pointPolygonTest(_lContour, _checkMe, false);

    //Return only when r == 1 - then Point is inside contour
    //If it's not then it is r == -1
    return r == 1;
}

void ColorBlobDetector::processImage(cv::Mat _inputImage) {
    double minContourArea = 0.1;

    cv::cvtColor(_inputImage, mHsvMat, cv::COLOR_RGB2HSV_FULL);

    cv::inRange(mHsvMat, mLowerBound, mUpperBound, mMask);

    cv::dilate(mMask, mDilatedMask, cv::Mat());

    std::vector<std::vector<cv::Point> > lContours;
    cv::findContours(mDilatedMask, lContours, mHierarchy, cv::RETR_TREE,
                     cv::CHAIN_APPROX_SIMPLE);

    //qDebug() << "Number of contours" << lContours.size();
    std::vector<std::vector<cv::Point> >::iterator lEach = lContours.begin();

    //Find max contourArea
    double maxArea = 0;
    for(int i = 0; i < lContours.size(); i ++) {
        double _area = cv::contourArea(lContours[i]);
        if(_area > maxArea) {
            maxArea = _area;
        }
    }

    mContours.clear();
    lEach = mContours.begin();
    bool isInside = false;
    for(int i = 0; i < lContours.size(); i ++) {
        if((cv::contourArea(lContours[i]) > minContourArea * maxArea)) {
            if(isInside = isContour(clickedAt, lContours[i])) {
                mContours.push_back(lContours[i]);
            }
        }
    }

    cv::RNG rng(12345);
    cv::Mat drawing = cv::Mat::zeros(mDilatedMask.size(), CV_8UC3);
    for(int i = 0; i < mContours.size(); i ++) {
        cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255),
                                       rng.uniform(0,255) );
        cv::drawContours(drawing, mContours, i, color, 2, 8, mHierarchy,
                         0, cv::Point());
    }
}

void ColorBlobDetector::setPoint(cv::Point _point) {
    this->clickedAt = _point;
}

cv::Scalar ColorBlobDetector::lowerBound() const {
    return mLowerBound;
}

void ColorBlobDetector::setLowerBound(const cv::Scalar &lowerBound) {
    mLowerBound = lowerBound;
}

cv::Scalar ColorBlobDetector::upperBound() const {
    return mUpperBound;
}

void ColorBlobDetector::setUpperBound(const cv::Scalar &upperBound) {
    mUpperBound = upperBound;
}

cv::Point ColorBlobDetector::getClickedAt() const
{
    return clickedAt;
}

void ColorBlobDetector::setClickedAt(const cv::Point &value)
{
    clickedAt = value;
}

cv::Scalar ColorBlobDetector::getHsvColor() const
{
    return hsvColor;
}

cv::Scalar ColorBlobDetector::getBlueColor() const
{
    return blueColor;
}

void ColorBlobDetector::setBlueColor(const cv::Scalar &value)
{
    blueColor = value;
}

cv::Scalar ColorBlobDetector::getGreenColor() const
{
    return greenColor;
}

void ColorBlobDetector::setGreenColor(const cv::Scalar &value)
{
    greenColor = value;
}

cv::Scalar ColorBlobDetector::getYellowColor() const
{
    return yellowColor;
}

void ColorBlobDetector::setYellowColor(const cv::Scalar &value)
{
    yellowColor = value;
}

cv::Scalar ColorBlobDetector::getOrangeColor() const
{
    return orangeColor;
}

void ColorBlobDetector::setOrangeColor(const cv::Scalar &value)
{
    orangeColor = value;
}

cv::Scalar ColorBlobDetector::getRedColor() const
{
    return redColor;
}

void ColorBlobDetector::setRedColor(const cv::Scalar &value)
{
    redColor = value;
}