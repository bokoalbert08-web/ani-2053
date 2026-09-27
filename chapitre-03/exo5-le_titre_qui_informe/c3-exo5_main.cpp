#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKWindow/Core/NkEventSystem.h"
#include "NKWindow/Core/NkTypedEvents.h"
#include <windows.h>

using namespace nkentseu;

// Nom du "document" simule (pas de vrai fichier ouvert dans cet exercice).
static const std::string kDocumentName = "sans_titre.txt";

// Construit le titre a partir de l'etat courant : nom, astérisque si modifie, taille.
static std::string BuildTitle(bool modified, NkU32 width, NkU32 height) {
    std::string title = kDocumentName;
    if (modified) {
        title += "*";
    }
    title += " - " + std::to_string(width) + " x " + std::to_string(height);
    return title;
}

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title  = kDocumentName; // titre initial, sera remplace des l'ouverture
    cfg.width  = 1280;
    cfg.height = 720;

    Window window;
    if (!window.Create(cfg)) {
        MessageBoxA(nullptr, "Echec creation fenetre", "c3-exo5", MB_OK);
        return -1;
    }

    // Etat suivi manuellement : on ne redessine le titre QUE quand l'un des deux change.
    bool modified = false;
    NkVec2u size = window.GetSize();

    // Premier affichage du titre, une seule fois, avant la boucle.
    window.SetTitle(BuildTitle(modified, size.x, size.y));

    EventSystem &events = EventSystem::Instance();

    while (window.IsOpen()) {
        while (NkEvent* ev = events.PollEvent()) {

            if (ev->As<NkWindowCloseEvent>()) {
                window.Close();
            }
            else if (auto* re = ev->As<NkWindowResizeEvent>()) {
                // La taille a change : on met a jour le titre a cet instant precis,
                // pas a chaque tour de boucle.
                size.x = re->GetWidth();
                size.y = re->GetHeight();
                window.SetTitle(BuildTitle(modified, size.x, size.y));
            }
            else if (auto* kp = ev->As<NkKeyPressEvent>()) {
                bool wasModified = modified;

                if (kp->GetKey() == NkKey::NK_S) {
                    // Touche S = "sauvegarde" simulee : le document redevient propre.
                    modified = false;
                } else {
                    // Toute autre touche = "frappe" simulee : le document devient modifie.
                    modified = true;
                }

                // On ne touche au titre que si l'etat a reellement change,
                // pas a chaque evenement clavier recu.
                if (modified != wasModified) {
                    window.SetTitle(BuildTitle(modified, size.x, size.y));
                }
            }
        }
    }

    return 0;
}
