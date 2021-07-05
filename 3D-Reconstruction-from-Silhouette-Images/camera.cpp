#include "camera.h"

Camera::Camera(const cv::Mat& K, const cv::Mat& T, const cv::Mat& silhouette)
{
    K.copyTo(m_K);
    T.copyTo(m_RT);
    silhouette.copyTo(m_silhouette);
    m_P = m_K * m_RT;
}

Camera::~Camera()
{
    m_K.release();
    m_RT.release();
    m_P.release();
    m_silhouette.release();
}

cv::Point Camera::project(double x, double y, double z) const
{
    cv::Mat pt2d(3, 1, CV_64FC1);
    cv::Mat pt3d(4, 1, CV_64FC1);
    auto pt2dPtr = pt2d.ptr<double>();
    auto pt3dPtr = pt3d.ptr<double>();
    pt3dPtr[0] = x;
    pt3dPtr[1] = y;
    pt3dPtr[2] = z;
    pt3dPtr[3] = 1.;
    // Project the world coord back using corresponding projection matrix
    pt2d = m_P * pt3d;
    // The divide by the w component
    cv::Point pt;
    pt.x = (int)(pt2dPtr[0] / pt2dPtr[2]);
    pt.y = (int)(pt2dPtr[1] / pt2dPtr[2]);
    return pt;
}

int Camera::getImageWidth() const
{
    return m_silhouette.cols;
}

int Camera::getImageHeight() const
{
    return m_silhouette.rows;
}

uchar Camera::getImageValue(int _x, int _y) const
{
    return m_silhouette.at<uchar>(_y, _x);
}
