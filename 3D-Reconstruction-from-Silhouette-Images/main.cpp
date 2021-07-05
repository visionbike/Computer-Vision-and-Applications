#include <ctime>
#include <iomanip>
#include <filesystem>
#include <iostream>
#include "reconstructor_3d.h"

int main() {
    cv::Point3d origin(-50, -50, 0);
    int xmin = -50, ymin = -50, zmin = 0;
    int xmax = 50, ymax = 50, zmax = 100;
    std::string param_fn = "Camera Parameter.txt";
    std::string data_root = "data";
    std::filesystem::directory_iterator iterator(data_root);
    for (auto& entry : iterator) {
        if (entry.is_directory()) {
            // Time function returns the time since the Epoch(jan 1 1970). Returned time is in seconds.
            std::time_t start(NULL), end(NULL);
            std::cout << "Processing " << entry.path().string() << "..." << std::endl;
            time(&start);
            Reconstructor3D recon(param_fn, entry.path().string(),
                                  xmin, ymin, zmin,
                                  xmax, ymax, zmax,
                                  origin);
            recon.reconstruct();
            time(&end);
            // Calculating total time taken by the program.
            auto time_taken = double(end - start);
            std::cout << "Time taken by program is : " << std::fixed << time_taken << std::setprecision(5) << " sec " << std::endl;
            recon.saveXYZ(entry.path().filename().string() + ".xyz");
        }
    }
    std::cout << std::endl;
    return 0;
}
