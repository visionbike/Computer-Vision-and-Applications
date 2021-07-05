#include "reconstructor_3d.h"

Reconstructor3D::Reconstructor3D(const std::string& fn,
                                 const std::string& data_root,
                                 int xmin, int ymin, int zmin,
                                 int xmax, int ymax, int zmax,
                                 const cv::Point3d& origin)
{
    // Load camera's parameters
    std::ifstream f(fn);
    if (!f.is_open()) {
        std::cout << "Cannot read " << fn << std::endl;
        return;
    }
    std::string line;
    cv::Mat K;
    std::vector<cv::Mat> RTs;
    if (!f.eof()) {
        while (std::getline(f, line)) {
            if (line == "\n") { continue; }
            if (line.find("Intrinsic Parameter") != std::string::npos) {
                K = _readMatrix(f, 3, 3);
            } else if (line.find("Extrinsic Parameter") != std::string::npos) {
                cv::Mat RT = _readMatrix(f, 3, 4);
                RTs.push_back(RT);
            }
        }
    }
    f.close();
    // Load silhouette image filenames
    std::vector<std::string> fns;
    cv::glob(std::string(data_root) + "/*.bmp", fns, false);
    assert(fns.size() == RTs.size());
    // Get list of Cameras
    if (!m_cameras.empty()) { m_cameras.clear(); }
    for (int i = 0; i < RTs.size(); ++i) {
        cv::Mat img = cv::imread(fns[i], cv::IMREAD_GRAYSCALE);
        m_cameras.emplace_back(K, RTs[i], img);
    }
    // Workspace initiation
    int xdim = xmax - xmin;
    int ydim = ymax - ymin;
    int zdim = zmax - zmin;
    // Initiate voxel grid
    m_grid = VoxelGrid(xdim, ydim, zdim, origin);
}

Reconstructor3D::~Reconstructor3D()
{
    if (!m_cameras.empty()) { m_cameras.clear(); }
}

cv::Mat Reconstructor3D::_readMatrix(std::ifstream &f, int nrows, int ncols)
{
    cv::Mat mat = cv::Mat::zeros(nrows, ncols, CV_64FC1);
    std::string line;
    for (int i = 0; i < nrows; ++i) {
        std::getline(f, line);
        std::istringstream iss(line);
        if (ncols == 3) {
            iss >> mat.at<double>(i, 0) >> mat.at<double>(i, 1) >> mat.at<double>(i, 2);
        }
        else if (ncols == 4) {
            iss >> mat.at<double>(i, 0) >> mat.at<double>(i, 1) >> mat.at<double>(i, 2) >> mat.at<double>(i, 3);
        }
    }
    return mat;
}

void Reconstructor3D::reconstruct()
{
    m_grid.carve(m_cameras);
}

void Reconstructor3D::saveXYZ(const std::string& filename)
{
    m_grid.saveXYZ(filename);
}