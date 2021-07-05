#pragma once
#ifndef _RECONSTRUCTOR_3D_H_
#define _RECONSTRUCTOR_3D_H_

#include <iostream>
#include <fstream>
#include <opencv2/highgui.hpp>
#include "voxel.h"

class Reconstructor3D
{
public:
    Reconstructor3D() = default;
    explicit Reconstructor3D(const std::string& fn,
                             const std::string& data_root,
                             int xmin, int ymin, int zmin,
                             int xmax, int ymax, int zmax,
                             const cv::Point3d& origin);
    ~Reconstructor3D();
    void reconstruct();
    void saveXYZ(const std::string& filename);

private:
    static cv::Mat _readMatrix(std::ifstream& f, int nrows, int ncols);
private:
    std::vector<Camera> m_cameras;
    VoxelGrid m_grid;
};

#endif //!_RECONSTRUCTOR_3D_H_
