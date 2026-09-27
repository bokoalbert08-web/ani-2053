#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkEventSystem.h"
#include <windows.h>
#include <cstdio>

using namespace nkentseu;

// Renvoie une forme de curseur Win32 differente selon la zone (0 a 6),
// en decoupant la largeur de la fenetre en 7 bandes verticales egales.
static HCURSOR CursorForZone(int zone)
{
    switch (zone)
    {
        case 0: return LoadCursorA(nullptr, IDC_ARROW);    // zone 0 : fleche normale
        case 1: return LoadCursorA(nullptr, IDC_HAND);     // zone 1 : main (lien)
        case 2: return LoadCursorA(nullptr, IDC_CROSS);    // zone 2 : croix
        case 3: return LoadCursorA(nullptr, IDC_WAIT);     // zone 3 : sablier
        case 4: return LoadCursorA(nullptr, IDC_IBEAM);    // zone 4 : texte
        case 5: return LoadCursorA(nullptr, IDC_SIZEALL);  // zone 5 : deplacer
        default: return LoadCursorA(nullptr, IDC_NO);      // zone 6 : interdit
    }
}

int nkmain(const NkEntryState &state) {
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);

    NkWindowConfig cfg;
    cfg.title  = "Les sept curseurs";
    cfg.width  = 1400;
    cfg.height = 700;

    Window window(cfg);
    if (!window.IsOpen()) {
        printf("[app] creation fenetre echouee\n");
        return -1;
    }

    NkU32 windowWidth = window.GetSize().x;

    // ------------------------------------------------------------------
    // Partie 2 de l'exercice : poser le curseur UNE SEULE FOIS au demarrage.
    // Decommentez ce bloc et commentez la boucle d'evenements ci-dessous
    // pour observer ce qui se passe (voir c3-exo6_reponse.md).
    // ------------------------------------------------------------------
    // SetCursor(CursorForZone(3));
    // while (window.IsOpen()) {}
    // return 0;

    // ------------------------------------------------------------------
    // Partie 1 : mise a jour du curseur a chaque deplacement de souris.
    // ------------------------------------------------------------------
    EventSystem &events = EventSystem::Instance();
    int lastZone = -1;

    while (window.IsOpen()) {
        while (NkEvent* ev = events.PollEvent()) {
            if (ev->type == NkEventType::NK_WINDOW_CLOSE) {
                window.Close();
            }
            else if (ev->type == NkEventType::NK_MOUSE_MOVE) {
                NkI32 x = ev->data.mouseMove.x;
                int zone = (windowWidth > 0)
                    ? static_cast<int>((static_cast<NkU32>(x < 0 ? 0 : x) * 7) / windowWidth)
                    : 0;
                if (zone > 6) zone = 6;
                if (zone < 0) zone = 0;

                if (zone != lastZone) {
                    SetCursor(CursorForZone(zone));
                    printf("Zone %d : curseur change\n", zone);
                    lastZone = zone;
                }
                else {
                    // Le chapitre insiste : la forme du curseur se "redemande"
                    // a chaque image, sinon Windows la reinitialise. On la
                    // repose donc a chaque passage, meme dans la meme zone.
                    SetCursor(CursorForZone(zone));
                }
            }
        }
    }

    return 0;
}
