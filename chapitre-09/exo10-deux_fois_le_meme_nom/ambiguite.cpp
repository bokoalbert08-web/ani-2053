#include <iostream>

namespace nkentseu {
    namespace canvas {
        class NkTransform {
        public:
            void description() const {
                std::cout << "Transformation affine 2D de NKCanvas\n";
            }
        };
    }

    namespace renderer {
        struct NkTransform {
            void description() const {
                std::cout << "Transformation locale TRS de NKRenderer\n";
            }
        };
    }
}

int main() {
    // NkTransform objet; serait ambigu si les deux namespaces
    // etaient importes avec using namespace.

    nkentseu::canvas::NkTransform transform2D;
    nkentseu::renderer::NkTransform transformTRS;

    transform2D.description();
    transformTRS.description();

    return 0;
}
