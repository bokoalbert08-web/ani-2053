#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkSystem.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkEventSystem.h"
#include <windows.h>
#include <cstdio>

using namespace nkentseu;

// Exercice : afficher, pour l'ecran qui porte la fenetre, sa taille,
// sa position et son facteur d'echelle. Deplacer la fenetre d'un
// ecran a l'autre doit faire changer ces valeurs.
//
// Limite constatee dans l'API NKWindow actuelle (voir c3-exo12_reponse.md) :
// il n'existe pas de fonction d'enumeration de TOUS les ecrans branches
// (type GetDisplayCount()/GetDisplayAt(i)). Seules trois methodes de
// NkWindow donnent des informations d'affichage, et elles sont toutes
// relatives a l'ecran qui porte la fenetre courante :
//   - GetDisplaySize()
//   - GetDisplayPosition()
//   - GetDpiScale()
// Ce programme affiche donc ces trois valeurs a chaque deplacement de
// la fenetre, ce qui permet de verifier qu'elles suivent bien quand on
// passe d'un ecran a l'autre, sans pretendre lister tous les ecrans.

static void PrintDisplayInfo(const Window& win, const char* context) {
    NkVec2u dispSize = win.GetDisplaySize();
    NkVec2u dispPos  = win.GetDisplayPosition();
    float   dpiScale = win.GetDpiScale();

    printf("[%s] Ecran qui porte la fenetre :\n", context);
    printf("    Taille   : %u x %u\n", dispSize.x, dispSize.y);
    printf("    Position : (%d, %d)\n", dispPos.x, dispPos.y);
    printf("    Echelle  : %.2f\n", dpiScale);
}

int nkmain(const NkEntryState &state) {
    (void)state;

    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);

    // Initialise la plateforme et attache l'EventImpl au systeme
    // d'evenements. Sans cet appel, personne ne traite les messages
    // Windows.
    if (!NkInitialise()) {
        printf("[app] NkInitialise a echoue\n");
        return -1;
    }

    NkWindowConfig cfg;
    cfg.title  = "c3-exo12 - Inventaire des ecrans";
    cfg.width  = 600;
    cfg.height = 400;
    cfg.x      = 100;
    cfg.y      = 100;

    Window win;
    if (!win.Create(cfg)) {
        printf("[app] creation de la fenetre echouee\n");
        return -1;
    }

    // Affichage initial, avant tout deplacement.
    PrintDisplayInfo(win, "Position initiale");

    bool closeRequested = false;

    win.SetEventCallback([&](NkEvent* ev) {
        if (ev->type == NkEventType::NK_WINDOW_MOVE) {
            printf("\n[app] Fenetre deplacee vers (x=%d, y=%d)\n",
                   ev->data.windowMove.x, ev->data.windowMove.y);
            PrintDisplayInfo(win, "Apres deplacement");
        } else if (ev->type == NkEventType::NK_SYSTEM_DISPLAY_CHANGE) {
            printf("\n[app] Changement systeme d'affichage detecte "
                   "(moniteur ajoute/retire ou resolution changee)\n");
            PrintDisplayInfo(win, "Apres changement systeme");
        } else if (ev->type == NkEventType::NK_WINDOW_CLOSE) {
            closeRequested = true;
        }
    });

    EventSystem& events = EventSystem::Instance();
    while (win.IsOpen() && !closeRequested) {
        events.PollEvents();
        if (closeRequested && win.IsOpen()) win.Close();
        Sleep(1);
    }

    NkClose();
    return 0;
}
