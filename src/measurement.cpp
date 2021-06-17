
/**
 * @file measurement.cpp
 * @author 
 * @brief "1D测量类"的实现
 * @version 0.1
 * @date 2021-06-07
 * 
 */

#include "measurement.h"

#include <iostream>
#include <fstream>
#include <random>
#include <filesystem> // C++17标准库
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

/**
 * @brief 水平、垂直方向等距采样
 * 
 * @param src INPUT 包含圆弧的边沿图片
 * @param sample_pts OUTPUT 采集到的像素点
 * 
 */
void GetSamplePoints(const cv::Mat &src, std::vector<cv::Point2f> &sample_pts)
{
    for (size_t y = 0; y < src.rows; y += 5)
    {
        for (size_t x = 0; x < src.cols; x += 5)
        {
            if (src.at<uchar>(y, x) > 0)
            {
                sample_pts.push_back(cv::Point2f(x, y));
            }
        }
    }
}

/**
 * @brief 评估圆拟合的精确度
 * 
 * @param sample_pts INPUT 圆弧边沿的采样点集
 * @param center INPUT 待评估的圆心
 * @param radius INPUT 待评估的半径
 * 
 * @return float OUTPUT 圆拟合精确度，取值范围为0~1
 * 
 * @note RANSAC方法
 */
float CircleVerification(const std::vector<cv::Point2f> &sample_pts, const cv::Point2f &center, const float &radius)
{
    size_t cnt_total = sample_pts.size();
    size_t cnt_inlier = 0;
    float temp_distance;
    for (size_t i = 0; i < cnt_total; ++i)
    {
        temp_distance = cv::norm(sample_pts[i] - center);
        if (fabs(temp_distance - radius) < 1.0f)
        {
            cnt_inlier++;
        }
    }
    return static_cast<float>(cnt_inlier) / static_cast<float>(cnt_total);
}

/**
 * @brief 新建一个“测量”类的对象，并指定文件路径
 * 
 * @param imgs_src_directory INPUT 存储源图像的文件夹路径
 * @param imgs_dst_directory INPUT 待保存测量结果图像的文件夹路径
 * 
 */
Measurement::Measurement(const std::string &imgs_src_directory,
                         const std::string &imgs_dst_directory)
{
    this->imgs_src_directory_ = imgs_src_directory;
    this->imgs_dst_directory_ = imgs_dst_directory;
    for (const auto &entry : std::filesystem::directory_iterator(this->imgs_src_directory_))
    {
        if (entry.path().extension() == ".bmp")
        {
            this->imgs_src_.push_back(cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE));
            this->imgs_filename_.push_back(entry.path().filename().string());
        }
    }
    this->imgs_total_num = this->imgs_src_.size();
}

/**
 * @brief 析构“测量”类的对象
 * 
 * 
 */
Measurement::~Measurement()
{
}

void Measurement::Line()
{
    // TODO
}

/**
 * @brief RANSAC方法测量半圆的半径与圆心
 * 
 * @param x_upper_left 矩形ROI左上角的横坐标
 * @param y_upper_left 矩形ROI左上角的纵坐标
 * @param width 矩形ROI的宽度
 * @param height 矩形ROI的高度
 * @param semicircle_type 半圆的种类 "left" / "right"
 * @param iteration_times RANSAC迭代次数
 * 
 */
void Measurement::Semicircle(const int &x_upper_left,
                             const int &y_upper_left,
                             const int &width,
                             const int &height,
                             const std::string &semicircle_type,
                             const int &iteration_times)
{
    cv::Mat img_roi, img_edge;
    for (size_t i = 0; i < imgs_total_num; i++)
    {
        if (semicircle_type == "left")
        {
            this->left_semicircle_roi_.x = x_upper_left;
            this->left_semicircle_roi_.y = y_upper_left;
            this->left_semicircle_roi_.width = width;
            this->left_semicircle_roi_.height = height;
            img_roi = this->imgs_src_[i](this->left_semicircle_roi_);
        }
        else if (semicircle_type == "right")
        {
            this->right_semicircle_roi_.x = x_upper_left;
            this->right_semicircle_roi_.y = y_upper_left;
            this->right_semicircle_roi_.width = width;
            this->right_semicircle_roi_.height = height;
            img_roi = this->imgs_src_[i](this->right_semicircle_roi_);
        }

        img_edge = cv::Mat::zeros(img_roi.size(), img_roi.type());
        cv::GaussianBlur(img_roi, img_roi, cv::Size(3, 3), 2, 2, 4);
        cv::Canny(img_roi, img_edge, 30, 70, 3);

        if (semicircle_type == "left")
        {
            cv::Rect center_zone{img_edge.cols / 2, img_edge.rows / 4, img_edge.cols / 2, img_edge.rows / 2};
            cv::rectangle(img_edge, center_zone, cv::Scalar(0), -1);
        }
        else if (semicircle_type == "right")
        {
            cv::Rect center_zone{0, img_edge.rows / 4, img_edge.cols / 2, img_edge.rows / 2};
            cv::rectangle(img_edge, center_zone, cv::Scalar(0), -1);
        }

        // cv::imwrite(this->imgs_dst_directory_ + "/" + std::to_string(img_idx + 1) + "_" + "edge" + "_" + semicircle_type + ".jpg", img_edge);

        std::vector<cv::Point2f> sample_pts;
        GetSamplePoints(img_edge, sample_pts);

        float best_inlier_percentage = 0;
        float best_radius;
        cv::Point2f best_center;

        float min_inlier_percentage = 0.60;

        cv::Point2f center;
        float radius;
        for (size_t it = 0; it < iteration_times; ++it)
        {
            unsigned int idx1 = rand() % sample_pts.size();
            unsigned int idx2 = rand() % sample_pts.size();
            unsigned int idx3 = rand() % sample_pts.size();
            if (idx1 == idx2 || idx1 == idx3 || idx3 == idx2)
                continue;

            CircleFitting(sample_pts[idx1], sample_pts[idx2], sample_pts[idx3], center, radius);
            float temp_inlier_percentage = CircleVerification(sample_pts, center, radius);

            if (temp_inlier_percentage >= min_inlier_percentage)
            {
                if (temp_inlier_percentage > best_inlier_percentage)
                {
                    best_inlier_percentage = temp_inlier_percentage;
                    best_radius = radius;
                    best_center = center;

                    if (i == 3 && semicircle_type == "left")
                    {
                        cv::Mat img_display = img_roi.clone();
                        cv::circle(img_display, sample_pts[idx1], 5, cv::Scalar(0), -1);
                        cv::circle(img_display, sample_pts[idx2], 5, cv::Scalar(0), -1);
                        cv::circle(img_display, sample_pts[idx3], 5, cv::Scalar(0), -1);
                        cv::circle(img_display, best_center, best_radius, cv::Scalar(0), 1);
                        cv::imwrite(this->imgs_dst_directory_ + "/gif/" + std::to_string(it + 1) + ".jpg", img_display);
                    }
                }
            }
        }

        if (semicircle_type == "left")
        {
            this->left_semicircle_center_.push_back(best_center);
            this->left_semicircle_inlier_percentage_.push_back(best_inlier_percentage);
            this->left_semicircle_radius_.push_back(best_radius);
        }
        else if (semicircle_type == "right")
        {
            this->right_semicircle_center_.push_back(best_center);
            this->right_semicircle_inlier_percentage_.push_back(best_inlier_percentage);
            this->right_semicircle_radius_.push_back(best_radius);
        }
    }
}

/**
 * @brief 将测量结果保存
 * 
 * @note 包含.jpg图像与.txt文件
 * 
 */
void Measurement::Save()
{
    std::ofstream results_txt_file;
    results_txt_file.open(this->imgs_dst_directory_ + "/" + "results.txt", std::ios::ate);
    for (size_t i = 0; i < imgs_total_num; ++i)
    {
        results_txt_file << "Image filename: " << this->imgs_filename_[i] << std::endl
                         << std::endl
                         << "Left semicircle: " << std::endl
                         << "center: " << this->left_semicircle_center_[i] << std::endl
                         << "radius: " << this->left_semicircle_radius_[i] << std::endl
                         << "inlier percentage: " << this->left_semicircle_inlier_percentage_[i] << std::endl
                         << std::endl
                         << "Right semicircle: " << std::endl
                         << "center: " << this->right_semicircle_center_[i] << std::endl
                         << "radius: " << this->right_semicircle_radius_[i] << std::endl
                         << "inlier percentage: " << this->right_semicircle_inlier_percentage_[i] << std::endl
                         << std::endl;
    }
    results_txt_file.close();

    for (size_t i = 0; i < this->imgs_total_num; ++i)
    {
        cv::Point2f left_center{this->left_semicircle_center_[i].x + this->left_semicircle_roi_.x, this->left_semicircle_center_[i].y + this->left_semicircle_roi_.y};
        cv::circle(this->imgs_src_[i], left_center, this->left_semicircle_radius_[i], cv::Scalar(0), 2);
        cv::circle(this->imgs_src_[i], left_center, 2, cv::Scalar(0), -1);
        cv::Point2f right_center{this->right_semicircle_center_[i].x + this->right_semicircle_roi_.x, this->right_semicircle_center_[i].y + this->right_semicircle_roi_.y};
        cv::circle(this->imgs_src_[i], right_center, this->right_semicircle_radius_[i], cv::Scalar(0), 2);
        cv::circle(this->imgs_src_[i], right_center, 2, cv::Scalar(0), -1);
        cv::imwrite(this->imgs_dst_directory_ + "/" + std::to_string(i + 1) + ".jpg", this->imgs_src_[i]);
    }
}
