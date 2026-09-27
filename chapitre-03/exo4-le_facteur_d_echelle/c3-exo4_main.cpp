#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkWindow.h"
#include <windows.h>
#include <cstdio>

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    // Console de debogage pour afficher les resultats (utile car ceci est une
    // application fenetree Windows : sans elle, std::cout ne s'affiche nulle part).
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);

    NkWindowConfig cfg;
    cfg.title  = "Facteur d'echelle";
    cfg.width  = 1280;
    cfg.height = 720;

    Window window(cfg);
    if (!window.IsOpen()) {
        printf("[app] creation fenetre echouee\n");
        return -1;
    }

    NkVec2u windowSize    = window.GetSize();
    NkSurfaceDesc surface = window.GetSurfaceDesc();
    float scale           = window.GetDpiScale();

    printf("Taille fenetre     : %u x %u\n", windowSize.x, windowSize.y);
    printf("Taille cible rendu : %u x %u\n", surface.width, surface.height);
    printf("Facteur d'echelle  : %.2f\n", scale);

    while (window.IsOpen()) {}
    return 0;
}
