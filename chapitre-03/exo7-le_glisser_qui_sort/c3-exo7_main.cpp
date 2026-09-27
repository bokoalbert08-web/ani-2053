#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKWindow/Core/NkEventSystem.h"
#include "NKWindow/Core/NkTypedEvents.h"

using namespace nkentseu;

// Construit un titre qui montre, pendant un glisser, la position du curseur
// a la fois relative a la fenetre (GetX/GetY) et relative a l'ecran entier
// (GetScreenX/GetScreenY) - les deux divergent des que le curseur sort de
// la fenetre.
static std::string BuildTitle(const std::string& label, bool dragging,
                               NkI32 x, NkI32 y, NkI32 screenX, NkI32 screenY) {
    std::string title = label;
    if (dragging) {
        title += " - fenetre(" + std::to_string(x) + "," + std::to_string(y) +
                  ") ecran(" + std::to_string(screenX) + "," + std::to_string(screenY) + ")";
    } else {
        title += " - clic gauche puis glissez hors de la fenetre pour tester";
    }
    return title;
}

static void RunDragTest(const std::string& label, bool useCapture) {
    NkWindowConfig cfg;
    cfg.title  = label;
    cfg.width  = 800;
    cfg.height = 600;

    Window window;
    if (!window.Create(cfg)) {
        return;
    }

    bool dragging = false;
    window.SetTitle(BuildTitle(label, dragging, 0, 0, 0, 0));

    EventSystem &events = EventSystem::Instance();

    while (window.IsOpen()) {
        while (NkEvent* ev = events.PollEvent()) {

            if (ev->As<NkWindowCloseEvent>()) {
                window.Close();
            }
            else if (auto* bp = ev->As<NkMouseButtonPressEvent>()) {
                if (bp->IsLeft()) {
                    dragging = true;
                    if (useCapture) {
                        window.CaptureMouse(true);
                    }
                    window.SetTitle(BuildTitle(label, dragging,
                        bp->GetX(), bp->GetY(), bp->GetScreenX(), bp->GetScreenY()));
                }
            }
            else if (auto* mv = ev->As<NkMouseMoveEvent>()) {
                // On ne met a jour le titre QUE pendant un glisser en cours,
                // pas a chaque mouvement de souris hors glisser.
                if (dragging) {
                    window.SetTitle(BuildTitle(label, dragging,
                        mv->GetX(), mv->GetY(), mv->GetScreenX(), mv->GetScreenY()));
                }
            }
            else if (auto* br = ev->As<NkMouseButtonReleaseEvent>()) {
                if (br->IsLeft() && dragging) {
                    dragging = false;
                    if (useCapture) {
                        window.CaptureMouse(false);
                    }
                    window.SetTitle(BuildTitle(label, dragging,
                        br->GetX(), br->GetY(), br->GetScreenX(), br->GetScreenY()));
                }
            }
        }
    }
}

int nkmain(const NkEntryState &state) {
    (void)state;

    RunDragTest("1 - SANS capture", false);
    RunDragTest("2 - AVEC capture", true);

    return 0;
}
