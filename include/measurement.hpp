
/**
 * @file measurement.hpp
 * @author 
 * @brief "1D测量类"的声明与实现
 * @version 0.1
 * @date 2021-06-07
 * 
 */

#ifndef _MEASUREMENT_H_
#define _MEASUREMENT_H_

#include <iostream>
#include <filesystem> // C++17标准库
#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"

class Measurement
{
private:
    std::vector<cv::Mat> imgs_roi_;
    std::string imgs_src_directory_;
    std::string imgs_dst_directory_;

public:
    Measurement(const std::string &imgs_src_directory = "../share/src", // 默认参数是与含有exe文件的文件夹同级的“share”文件夹里的"src"文件夹
                const std::string &imgs_dst_directory = "../share/dst",
                const int &x_upper_left = 0,
                const int &y_upper_left = 0,
                const int &width = 2592,
                const int &height = 1944);
    ~Measurement();
    void Line();
    void Semicircle();
};

/**
 * @brief 构造“1D测量类”的对象
 * 
 * @param imgs_src_directory: 存放原图像的文件夹路径
 * @param imgs_dst_directory: 存放处理后图像的文件夹路径
 * @param x_upper_left: 矩形ROI左上角的横坐标
 * @param y_upper_left: 矩形ROI左上角的纵坐标
 * @param width: 矩形ROI的宽
 * @param height: 矩形ROI的高
 * 
 */
Measurement::Measurement(const std::string &imgs_src_directory,
                         const std::string &imgs_dst_directory,
                         const int &x_upper_left,
                         const int &y_upper_left,
                         const int &width,
                         const int &height)
{
    this->imgs_src_directory_ = imgs_src_directory;
    this->imgs_dst_directory_ = imgs_dst_directory;
    for (const auto &entry : std::filesystem::directory_iterator(this->imgs_src_directory_))
    {
        if (entry.path().extension() == ".bmp")
        {
            // std::cout << entry.path() << std::endl;
            cv::Mat img_src = cv::imread(entry.path().string());
            this->imgs_roi_.push_back(img_src(cv::Rect(x_upper_left, y_upper_left, width, height)));
        }
    }
}

/**
 * @brief 析构“1D测量类”的对象，释放内存前将结果保存到指定路径
 * 
 * 
 */
Measurement::~Measurement()
{
    for (size_t i = 0; i < this->imgs_roi_.size(); i++)
    {
        cv::imwrite(this->imgs_dst_directory_ + "/" + std::to_string(i) + ".bmp", this->imgs_roi_[i]);
    }
}

/**
 * @brief 测量直线
 * 
 * 
 */
void Measurement::Line()
{
}

/**
 * @brief 测量半圆
 * 
 * 
 */
void Measurement::Semicircle()
{
}

#endif
