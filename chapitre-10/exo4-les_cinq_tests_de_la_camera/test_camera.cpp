#include <cmath>
#include <iostream>
#include "NkRef/NkRefView.h"

using nkref::NkRefView;
using nkentseu::math::NkVec2f;

bool proche(float a, float b) {
    return std::fabs(a - b) < 0.001f;
}

bool aller_retour() {
    NkRefView camera;
    NkVec2f viewport{1024.0f, 768.0f};
    NkVec2f pixel{230.0f, 410.0f};

    NkVec2f monde = camera.PixelToWorld(pixel, viewport);
    NkVec2f retour = camera.WorldToPixel(monde, viewport);

    return proche(pixel.x, retour.x) && proche(pixel.y, retour.y);
}

bool point_fixe() {
    NkRefView camera;
    NkVec2f viewport{1024.0f, 768.0f};
    NkVec2f pixel{200.0f, 300.0f};

    NkVec2f avant = camera.PixelToWorld(pixel, viewport);
    camera.ZoomAtPixel(2.0f, pixel, viewport);
    NkVec2f apres = camera.PixelToWorld(pixel, viewport);

    return proche(avant.x, apres.x) && proche(avant.y, apres.y);
}

bool bornes() {
    NkRefView camera;
    NkVec2f viewport{1024.0f, 768.0f};
    NkVec2f pixel{200.0f, 300.0f};

    camera.ZoomAtPixel(1000.0f, pixel, viewport);
    bool maximum = camera.zoom <= NkRefView::kZoomMax;

    camera.ZoomAtPixel(0.00001f, pixel, viewport);
    bool minimum = camera.zoom >= NkRefView::kZoomMin;

    return maximum && minimum;
}

bool grille() {
    NkRefView camera;
    float avant = camera.GridSpacing(20.0f);

    camera.zoom = 0.5f;
    float apres = camera.GridSpacing(20.0f);

    return apres >= avant;
}

bool origine() {
    NkRefView camera;
    NkVec2f viewport{1024.0f, 768.0f};
    NkVec2f pixel{200.0f, 300.0f};

    camera.ZoomAtPixel(2.0f, pixel, viewport);
    camera.PanByPixels(50.0f, 30.0f);
    camera.Reset();

    return proche(camera.center.x, 0.0f)
        && proche(camera.center.y, 0.0f)
        && proche(camera.zoom, 1.0f);
}

int main() {
    std::cout << "TEST aller_retour : " << (aller_retour() ? "OK" : "ECHEC") << '\n';
    std::cout << "TEST point_fixe : " << (point_fixe() ? "OK" : "ECHEC") << '\n';
    std::cout << "TEST bornes : " << (bornes() ? "OK" : "ECHEC") << '\n';
    std::cout << "TEST grille : " << (grille() ? "OK" : "ECHEC") << '\n';
    std::cout << "TEST origine : " << (origine() ? "OK" : "ECHEC") << '\n';
}
