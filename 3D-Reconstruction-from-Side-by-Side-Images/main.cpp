#include "reconstructor_3d.h"

int main() {
    Reconstructor3D r("SidebySide/CalibrationData.txt", "SidebySide", 140);
    r.reconstruct();
    r.exportXYZ();
    return 0;
}
