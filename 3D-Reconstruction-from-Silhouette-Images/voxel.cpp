#include "voxel.h"

VoxelGrid::VoxelGrid(int xdim, int ydim, int zdim, const cv::Point3d& origin) :
        m_grid(xdim * ydim * zdim)
{
    _init(xdim, ydim, zdim, origin);
}

VoxelGrid::~VoxelGrid()
{
    if (!m_grid.empty()) { m_grid.clear(); }
}

void VoxelGrid::carve(const std::vector<Camera>& cams)
{
    for (const auto & cam : cams) {
        std::vector<Voxel> new_grid;
        for (auto & voxel : m_grid) {
            cv::Point coord = cam.project(voxel.x, voxel.y, voxel.z);
            // Skip scan voxels that are out of range of camera's FOV
            if (coord.x < 0 || coord.x >= cam.getImageWidth() || coord.y < 0 || coord.y >= cam.getImageHeight()) { continue; }
            // Only consider foreground
            if (cam.getImageValue(coord.x, coord.y)) { new_grid.push_back(voxel); }
        }
        m_grid.clear();
        m_grid = new_grid;
    }
    std::cout << m_grid.size() << std::endl;
}

void VoxelGrid::saveXYZ(const std::string& filename)
{
    std::fstream f(filename, std::ios::out);
    assert(f.is_open());
    for (auto & i : m_grid) {
        f << i.x << " " << i.y << " " << i.z << std::endl;
    }
    f.close();
}

void VoxelGrid::_init(int xdim, int ydim, int zdim, const cv::Point3d& origin)
{
    int k = 0;
    for (int x = 0; x < xdim; ++x) {
        for (int y = 0; y < ydim; ++y) {
            for (int z = 0; z < zdim; ++z) {
                m_grid[k].x = origin.x + float(x);
                m_grid[k].y = origin.y + float(y);
                m_grid[k].z = origin.z + float(z);
                ++k;
            }
        }
    }
}