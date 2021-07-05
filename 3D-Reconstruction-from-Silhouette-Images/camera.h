#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <opencv2/core.hpp>

class Camera
{
public:
    Camera() = default;
    Camera(const cv::Mat& K, const cv::Mat& RT, const cv::Mat& silhouette);
    cv::Point project(double x, double y, double z) const;
    int getImageWidth() const;
    int getImageHeight() const;
    uchar getImageValue(int x, int y) const;
    ~Camera();

public:
    cv::Mat m_K;            // Intrinsic matrix (3x3)
    cv::Mat m_RT;            // Extrinsic matrix (3x4)
    cv::Mat m_silhouette;   // Silhouette image
    cv::Mat m_P;            // Projection matrix (3x4)
};

void splitStringToFloat(const std::string &_s, const char *_delim, std::vector<float> &_o);
void loadCameras(const char *_fn_params, const char *_fd_sils, std::vector<Camera>& _cams);

#endif //!_CAMERA_H_
