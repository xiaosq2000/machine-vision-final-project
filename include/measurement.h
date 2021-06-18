/**
 * @file measurement.h
 * @author 
 * @brief "1D测量"类的声明
 * @version 0.1
 * @date 2021-06-17
 * 
 */

#ifndef _MEASUREMENT_H_
#define _MEASUREMENT_H_

#include <iostream>
#include "opencv2/core/core.hpp"

/**
 * @brief 已知三点坐标拟合圆
 * 
 * @param p1 INPUT
 * @param p2 INPUT
 * @param p3 INPUT
 * @param center OUTPUT
 * @param radius OUTPUT
 * 
 * @note 内联函数，提升运行速度，单张图片节约10ms左右
 */
inline void CircleFitting(const cv::Point2f &p1, const cv::Point2f &p2, const cv::Point2f &p3, cv::Point2f &center, float &radius)
{
    float x1 = p1.x;
    float x2 = p2.x;
    float x3 = p3.x;
    float y1 = p1.y;
    float y2 = p2.y;
    float y3 = p3.y;
    center.x = (x1 * x1 + y1 * y1) * (y2 - y3) + (x2 * x2 + y2 * y2) * (y3 - y1) + (x3 * x3 + y3 * y3) * (y1 - y2);
    center.x /= (2 * (x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2));
    center.y = (x1 * x1 + y1 * y1) * (x3 - x2) + (x2 * x2 + y2 * y2) * (x1 - x3) + (x3 * x3 + y3 * y3) * (x2 - x1);
    center.y /= (2 * (x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2));
    radius = sqrt((center.x - x1) * (center.x - x1) + (center.y - y1) * (center.y - y1));
}

void GetSamplePoints(const cv::Mat &src, std::vector<cv::Point2f> &sample_pts);

float CircleVerification(const std::vector<cv::Point2f> &sample_pts, const cv::Point2f &center, const float &radius);

class Measurement
{
private:
    std::string imgs_dst_directory_;
    std::string imgs_src_directory_;
    std::vector<cv::Mat> imgs_src_;
    std::vector<std::string> imgs_filename_;
    unsigned int imgs_total_num;

    float subpixel_ratio_;

    cv::Rect left_semicircle_roi_;
    std::vector<float> left_semicircle_inlier_percentage_;
    std::vector<cv::Point2f> left_semicircle_center_;
    std::vector<float> left_semicircle_radius_;

    cv::Rect right_semicircle_roi_;
    std::vector<float> right_semicircle_inlier_percentage_;
    std::vector<cv::Point2f> right_semicircle_center_;
    std::vector<float> right_semicircle_radius_;

public:
    Measurement(const std::string &imgs_src_directory = "../share/src",
                const std::string &imgs_dst_directory = "../share/dst");
    ~Measurement();
    void Line();
    void Semicircle(const int &x_upper_left = 520,
                    const int &y_upper_left = 980,
                    const int &width = 220,
                    const int &height = 400,
                    const std::string &semicircle_type = "left",
                    const int &iteration_times = 100,
                    const float &subpixel_ratio = 0.5);
    void Save();
};

#endif