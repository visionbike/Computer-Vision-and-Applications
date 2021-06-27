#include <ctime>
#include <iomanip>
#include "reconstructor_3d.h"

int main() {
    // Time function returns the time since the Epoch(jan 1 1970). Returned time is in seconds.
    std::time_t start(NULL), end(NULL);
    Reconstructor3D recon("SidebySide/CalibrationData.txt", "SidebySide", 140);
    time(&start);
    recon.reconstruct();
    time(&end);
    // Calculating total time taken by the program.
    auto time_taken = double(end - start);
    std::cout << "Time taken by program is : " << std::fixed << time_taken << std::setprecision(5) << " sec " << std::endl;
    recon.exportXYZ();
    std::cout << "Done!" << std::endl;
    return 0;
}