#include "NkRef/NkRefView.h"

int main() {
    const nkref::NkRefView vue{};

    const nkref::NkVec2f pixel{100.0f, 100.0f};
    const nkref::NkVec2f viewport{800.0f, 600.0f};

    (void)vue.PixelToWorld(pixel, viewport);
    (void)vue.GridSpacing(10.0f);

    // Nous ajouterons PanByPixels pour provoquer l'erreur du compilateur.
    // vue.PanByPixels(1.0f, 0.0f);

    return 0;
}
