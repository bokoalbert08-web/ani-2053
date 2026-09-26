#include "NKWindow/NKWindow.h"
#include "NKWindow/NKMain.h"
#include <iostream>

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    NkWindowConfig cfg;
    cfg.title  = "Facteur d'echelle";
    cfg.width  = 1280;
    cfg.height = 720;

    NkWindow window(cfg);
    if (!window.IsOpen()) {
        logger.Error("[app] creation fenetre echouee");
        return -1;
    }

    math::NkVec2u windowSize = window.GetSize();
    NkSurfaceDesc surface    = window.GetSurfaceDesc();
    float32 scale            = window.GetDpiScale();

    std::cout << "Taille fenetre     : " << windowSize.x << " x " << windowSize.y << std::endl;
    std::cout << "Taille cible rendu : " << surface.width << " x " << surface.height << std::endl;
    std::cout << "Facteur d'echelle  : " << scale << std::endl;

    while (window.IsOpen()) {}
    return 0;
}
