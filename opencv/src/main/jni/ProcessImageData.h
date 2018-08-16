//
// Created by frendy on 2018/8/15.
//

#ifndef CVDROID_PROCESSIMAGEDATA_H
#define CVDROID_PROCESSIMAGEDATA_H

//OpenCv
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class ProcessImageData {
public:
    ProcessImageData(cv::Point );
    cv::Scalar calculateHsv(cv::Mat);
private:
    cv::Point clickedAt;
    int x, y;
};


#endif //CVDROID_PROCESSIMAGEDATA_H
