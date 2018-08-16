//
// Created by frendy on 2018/8/15.
//

#ifndef CVDROID_COLORBLOBDETECTOR_H
#define CVDROID_COLORBLOBDETECTOR_H

#include <vector>

//OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class ColorBlobDetector {
public:
    ColorBlobDetector();
    ColorBlobDetector(ColorBlobDetector *, cv::Point );

    void setPoint(cv::Point );
    void setHsvColor(cv::Scalar);
    void processImage(cv::Mat);
    void saveAll();
    bool isContour(cv::Point , std::vector<cv::Point> );

    cv::Scalar lowerBound() const;
    void setLowerBound(const cv::Scalar &lowerBound);

    cv::Scalar upperBound() const;
    void setUpperBound(const cv::Scalar &upperBound);

    cv::Point getClickedAt() const;
    void setClickedAt(const cv::Point &value);

    cv::Scalar getHsvColor() const;

    cv::Scalar getBlueColor() const;
    void setBlueColor(const cv::Scalar &value);

    cv::Scalar getGreenColor() const;
    void setGreenColor(const cv::Scalar &value);

    cv::Scalar getYellowColor() const;
    void setYellowColor(const cv::Scalar &value);

    cv::Scalar getOrangeColor() const;
    void setOrangeColor(const cv::Scalar &value);

    cv::Scalar getRedColor() const;
    void setRedColor(const cv::Scalar &value);

private:
    //Used for HSV color space
    cv::Scalar mLowerBound;
    cv::Scalar mUpperBound;

    cv::Mat mMask;
    cv::Mat mHsvMat;
    cv::Mat mDilatedMask;
    std::vector<cv::Vec4i> mHierarchy;

    cv::Scalar hsvColor;

    //For later picture localization
    int it;

    //Colors
    cv::Scalar blueColor;   //front
    cv::Scalar greenColor;  //top
    cv::Scalar yellowColor; //right
    cv::Scalar orangeColor; //left
    cv::Scalar redColor;    //back

    //Contours
    std::vector<cv::Point> topContour;   //greenColor
    std::vector<cv::Point> frontContour; //blueColor
    std::vector<cv::Point> leftContour;  //orangeColor
    std::vector<cv::Point> rightContour; //yellowColor
    std::vector<cv::Point> backContour;  //redColor

    //Clicked point
    cv::Point clickedAt;

    //Color radius for range checking in HSV space
    cv::Scalar mColorRadius = cv::Scalar(20, 25, 25, 0);

public:
    std::vector<std::vector<cv::Point> > mContours;
};


#endif //CVDROID_COLORBLOBDETECTOR_H
