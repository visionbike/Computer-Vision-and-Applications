#include "reconstructor_3d.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantParameter"
Reconstructor3D::Reconstructor3D() :
    m_hHalfSize(H_HALF_SIZE), m_vHalfSize(V_HALF_SIZE), m_thresh(THRESHOLD), m_objHeight(0)
{

}

Reconstructor3D::Reconstructor3D(const char* fn, const char* data_root, int height) :
    m_hHalfSize(H_HALF_SIZE), m_vHalfSize(V_HALF_SIZE), m_thresh(THRESHOLD), m_objHeight(height)
{
    m_data_root = data_root;
    m_leftK = cv::Mat::zeros(3, 3, CV_64F);
    m_rightK = cv::Mat::zeros(3, 3, CV_64F);
    m_leftRT = cv::Mat::zeros(3, 4, CV_64F);
    m_rightRT = cv::Mat::zeros(3, 4, CV_64F);
    m_F = cv::Mat::zeros(3, 3, CV_64F);
    std::ifstream f(fn);
    if (!f.is_open()) {
        std::cout << "Cannot read " << fn << std::endl;
        return;
    }
    std::string line;
    if (!f.eof()) {
        while (std::getline(f, line)) {
            if ((line == "\n") || (line.find("#Calibration") != std::string::npos)) {
                continue;
            }
            if (line.find("#Left Camera K (3x3)") != std::string::npos) {
                _readMatrix(f, m_leftK, 3, 3);
            } else if (line.find("#Left Camera RT (3x4)") != std::string::npos) {
                _readMatrix(f, m_leftRT, 3, 4);
            } else if (line.find("#Right Camera K (3x3)") != std::string::npos) {
                _readMatrix(f, m_rightK, 3, 3);
            } else if (line.find("#Right Camera RT (3x4)") != std::string::npos) {
                _readMatrix(f, m_rightRT, 3, 4);
            } else if (line.find("#Fundamental Matrix (3x3)") != std::string::npos) {
                _readMatrix(f, m_F, 3, 3);
            }
        }
    }
    f.close();
    m_leftP = m_leftK * m_leftRT;
    m_rightP = m_rightK * m_rightRT;

    std::cout << "Calibration Information" << std::endl;
    std:: cout << "Left K:\n";
    std:: cout << m_leftK << std::endl;
    std:: cout << "Right K:\n";
    std:: cout << m_rightK << std::endl;
    std:: cout << "Left RT:\n";
    std:: cout << m_leftRT  << std::endl;
    std:: cout << "Right RT:\n";
    std:: cout << m_rightRT << std::endl;
    std:: cout << "F:\n";
    std:: cout << m_F << std::endl << std::endl;
}

Reconstructor3D::~Reconstructor3D()
{
    if (!m_leftK.empty()) { m_leftK.release(); }
    if (!m_rightK.empty()) { m_rightK.release(); }
    if (!m_leftRT.empty()) { m_leftRT.release(); }
    if (!m_rightRT.empty()) { m_rightRT.release(); }
    if (!m_F.empty()) { m_F.release(); }
    if (!m_leftP.empty()) { m_leftP.release(); }
    if (!m_rightP.empty()) { m_rightP.release(); }
}

void Reconstructor3D::_readMatrix(std::ifstream& f, cv::Mat& m, int nrows, int ncols)
{
    std::string line;
    for (int i = 0; i < nrows; ++i) {
        std::getline(f, line);
        std::istringstream iss(line);
        if (ncols == 3) {
            iss >> m.at<double>(i, 0) >> m.at<double>(i, 1) >> m.at<double>(i, 2);
        }
        else {
            iss >> m.at<double>(i, 0) >> m.at<double>(i, 1) >> m.at<double>(i, 2) >> m.at<double>(i, 3);
        }
    }
}

cv::Point3d Reconstructor3D::_estimate3DPoint(double x1, double y1, double x2, double y2)
{
    cv::Mat A = cv::Mat::zeros(4, 4, CV_64F);
    for (int i = 0; i < 4; ++i) {
        A.at<double>(0, i) = x1 * m_leftP.at<double>(2, i) - m_leftP.at<double>(0, i);
        A.at<double>(1, i) = y1 * m_leftP.at<double>(2, i) - m_leftP.at<double>(1, i);
        A.at<double>(2, i) = x2 * m_rightP.at<double>(2, i) - m_rightP.at<double>(0, i);
        A.at<double>(3, i) = y2 * m_rightP.at<double>(2, i) - m_rightP.at<double>(1, i);
    }
    cv::Mat S, U, Vt;
    cv::SVD::compute(A, S, U, Vt, cv::SVD::FULL_UV);
    cv::Mat X = Vt.row(3).t();
    X /= X.at<double>(3, 0);
    return cv::Point3d(X.at<double>(0, 0), X.at<double>(1, 0), X.at<double>(2, 0));
}

void Reconstructor3D::_verify3DPoint(int width, int height)
{
    std::vector<cv::Point3d> pts3d = m_pts3d;
    m_pts3d.clear();
    for (auto& pt : pts3d) {
        cv::Mat pt3d = (cv::Mat_<double>(4, 1) << pt.x, pt.y, pt.z, 1.);
        cv::Mat lpt = m_leftP * pt3d;
        cv::Mat rpt = m_rightP * pt3d;
        lpt /= lpt.at<double>(2, 0);
        rpt /= rpt.at<double>(2, 0);
        if ((0 <= lpt.at<double>(0, 0) <= width) &&
            (0 <= lpt.at<double>(1, 0) <= height) &&
            (0 <= rpt.at<double>(0, 0) <= width) &&
            (0 <= rpt.at<double>(1, 0) <= height)) {
            m_pts3d.push_back(pt);
        }
    }
}

void Reconstructor3D::reconstruct()
{
    // Scan through each image.
    if (!m_pts3d.empty()) { m_pts3d.clear(); }
    for (auto& entry : std::filesystem::directory_iterator(m_data_root)) {
        // Read image
        cv::Mat img = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
        // Skip if cannot read image
        if (img.empty()) { continue; }
        std::cout << "Processing " << entry.path() << "..." << std::endl;
        // Keep most brightest values.
        img.setTo(0, img < MAX_VALUE);
        // Get left and right image from input
        cv::Mat left = img(cv::Rect(0, 0, img.cols / 2, img.rows));
        left.convertTo(left, CV_64F);
        cv::Mat right = img(cv::Rect(img.cols / 2, 0, img.cols / 2, img.rows));
        right.convertTo(right, CV_64F);
        // Find max values in each row
        std::vector<double> leftMax, rightMax;
        for (int i = 0; i < left.rows; ++i) {
            double max;
            cv::minMaxIdx(left.row(i), nullptr, &max);
            leftMax.push_back(max);
            max = 0;
            cv::minMaxIdx(right.row(i), nullptr, &max);
            rightMax.push_back(max);
        }
        // Scan each row to find corresponding left/right points and construct 3D point
        int vBoundMin, vBoundMax, hBoundMin, hBoundMax;
        for (int i = 0; i < left.rows; ++i) {
            for (int j = 0; j < left.cols; ++j) {
                vBoundMin = (i - m_vHalfSize < 0) ? 0 : i - m_vHalfSize;
                vBoundMax = (i + m_vHalfSize > right.rows) ? right.rows : i + m_vHalfSize;
                hBoundMin = (j - m_hHalfSize < 0) ? 0 : j - m_hHalfSize;
                hBoundMax = (j + m_hHalfSize > right.cols) ? right.cols : j + m_hHalfSize;
                // Find corresponding right point for left point
                if (leftMax[i] != 0 && left.at<double>(i, j) >= leftMax[i] - VALUE_OFFSET) {
                    cv::Mat l = m_F * (cv::Mat_<double>(3, 1) << double(j), double(i), 1.);
                    for (int y = vBoundMin; y < vBoundMax; ++y) {
                        for (int x = hBoundMin; x < hBoundMax; ++x) {
                            if (rightMax[y] == 0 || right.at<double>(y, x) < rightMax[y] - VALUE_OFFSET) { continue; }
                            if (std::abs(l.at<double>(0, 0) * x + l.at<double>(1, 0) * y + l.at<double>(2, 0)) >= m_thresh) { continue; }
                            cv::Point3d pt3d = _estimate3DPoint(j, i, x, y);
                            // Ignore the point with y > 130 (height of 3D model)
                            if (std::abs(pt3d.y) > m_objHeight) { continue; }
                            // Pass the point with same coordinates
                            if (std::any_of(m_pts3d.begin(), m_pts3d.end(), _compare(pt3d))) { continue; }
                            m_pts3d.push_back(pt3d);
                        }
                    }
                }
                // Find corresponding left point for right point
                if (rightMax[i] != 0 && right.at<double>(i, j) >= rightMax[i] - VALUE_OFFSET) {
                    cv::Mat l = m_F * (cv::Mat_<double>(3, 1) << double(j), double(i), 1.);
                    for (int y = vBoundMin; y < vBoundMax; ++y) {
                        for (int x = hBoundMin; x < hBoundMax; ++x) {
                            if (leftMax[y] == 0 || left.at<double>(y, x) < leftMax[y] - VALUE_OFFSET) { continue; }
                            if (std::abs(l.at<double>(0, 0) * x + l.at<double>(1, 0) * y + l.at<double>(2, 0)) >= m_thresh) { continue; }
                            cv::Point3d pt3d = _estimate3DPoint(x, y, j, i);
                            // Ignore the point with y > 130 (height of 3D model)
                            if (std::abs(pt3d.y) > m_objHeight) { continue; }
                            // Pass the point with same coordinates
                            if (std::any_of(m_pts3d.begin(), m_pts3d.end(), _compare(pt3d))) { continue; }
                            m_pts3d.push_back(pt3d);
                        }
                    }
                }
            }
        } 
    }
    // Verify valid 3d points
    std::cout << "Number of obtained 3D points: " << m_pts3d.size() << std::endl;
    _verify3DPoint(1280, 720);
    std::cout << "Number of valid 3D points: " << m_pts3d.size() << std::endl;
    std::cout << "Done!" << std::endl;
}

void Reconstructor3D::exportXYZ()
{
    std::cout << "Export to XYZ file..." << std::endl;
    std::ofstream f("reconstruct3d.xyz");
    if (!f.is_open()) {
        std::cout << "Cannot save reconstruct3D.xyz" << std::endl;
        return;
    }
    for(auto & pt : m_pts3d) {
        f << pt.x << " " << pt.y << " " << pt.z << std::endl;
    }
    f.close();
    std::cout << "Saved to reconstruct3D.xyz!" << std::endl;
}