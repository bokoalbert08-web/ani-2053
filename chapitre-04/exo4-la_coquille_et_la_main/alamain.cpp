#include <algorithm>

#include "NKWindow/NKMain.h"
#include "NKCanvas/App/NkCanvasApp.h"
#include "NKTime/NkClock.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

int nkmain(const NkEntryState &) {
    NkWindowConfig config;
    config.title = "Carre a la main";
    config.width = 800;
    config.height = 450;

    NkWindow window(config);
    if (!window.IsOpen()) {
        return 1;
    }

    NkRenderWindow target(window, NkContextDesc::MakeDirectX11());
    if (!target.IsValid()) {
        return 1;
    }

    NkClock horloge;
    float32 x = 0.f;
    bool running = true;

    while (running && window.IsOpen()) {
        // Le meme plafond que la coquille : au retour de veille, dt peut valoir des secondes.
        const float32 dt = std::min(horloge.Tick().delta, 0.1f);

        while (NkEvent *ev = NkEvents().PollEvent()) {
            if (ev->As<NkWindowCloseEvent>() != nullptr) {
                running = false;
            }
        }

        x += 100.f * dt;
        if (x > 800.f) {
            x = -50.f;
        }

        target.Clear(NkColor2D{18, 18, 24, 255});
        target.GetRenderer2D().DrawFilledRect({x, 200.f, 50.f, 50.f}, NkColor2D::Red);
        target.Display();
    }
    return 0;
}
