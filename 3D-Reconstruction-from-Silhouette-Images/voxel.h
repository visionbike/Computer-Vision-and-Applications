#pragma once
#ifndef _VOXEL_H_
#define _VOXEL_H_

#include <iostream>
#include <fstream>
#include <opencv2/core.hpp>
#include "camera.h"

struct Voxel
{
    double x;
    double y;
    double z;
};

class VoxelGrid
{
public:
    VoxelGrid() = default;
    VoxelGrid(int xdim, int ydim, int zdim, const cv::Point3d& origin);
    ~VoxelGrid();
    void carve(const std::vector<Camera>& cams);
    void saveXYZ(const std::string& filename);

private:
    void _init(int xdim, int ydim, int zdim, const cv::Point3d& origin);

private:
    std::vector<Voxel> m_grid;
};

#endif //!_VOXEL_H_
