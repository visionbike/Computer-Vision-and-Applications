#pragma once
#ifndef _RECONSTRUCTOR_3D_H_
#define _RECONSTRUCTOR_3D_H_

#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <utility>

constexpr auto MAX_VALUE = 80;
constexpr auto VALUE_OFFSET = 10;
constexpr auto H_HALF_SIZE = 300;
constexpr auto V_HALF_SIZE = 300;
constexpr auto THRESHOLD = 0.0001;

class Reconstructor3D
{
public:
    Reconstructor3D();
    explicit Reconstructor3D(const char* fn, const char* data_root, int height);
    ~Reconstructor3D();
    void reconstruct();
    void exportXYZ();

private:
    struct _compare
    {
        cv::Point3d key;
        explicit _compare(cv::Point3d i): key(std::move(i)) {}

        bool operator()(cv::Point3d const &i) const {
            return (i == key);
        }
    };
    static void _readMatrix(std::ifstream& f, cv::Mat& m, int nrows, int ncols);
    cv::Point3d _estimate3DPoint(double x1, double y1, double x2, double y2);
    void _verify3DPoint(int width, int height);
private:
    cv::Mat m_leftK, m_rightK;      // Left and right K matrices
    cv::Mat m_leftRT, m_rightRT;    // Left and right RT matrices
    cv::Mat m_leftP, m_rightP;      // Left and Right P matrices
    cv::Mat m_F;                    // Fundamental matrix
    std::string m_data_root;
    int m_hHalfSize;
    int m_vHalfSize;
    double m_thresh;
    double m_objHeight;
    std::vector<cv::Point3d> m_pts3d;
};

#endif //!_RECONSTRUCTOR_3D_H_
