#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "C3Exo3LesBornes";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

// Ouvre une fenetre, affiche sa taille courante dans le titre a chaque
// redimensionnement (pour lire en direct la plus petite taille atteinte),
// et attend sa fermeture par l'utilisateur.
static void RunBorneTest(const std::string& label, NkWindowConfig cfg) {
    Window window;
    if (!window.Create(cfg)) {
        logger.Error(("[c3-exo3] Creation fenetre KO : " + label).c_str());
        return;
    }

    NkEventSystem &events = NkEventSystem::Instance();
    while (window.IsOpen()) {
        while (NkEvent* ev = events.PollEvent()) {
            if (ev->As<NkWindowCloseEvent>()) {
                window.Close();
            }
            else if (auto* re = ev->As<NkWindowResizeEvent>()) {
                std::string title = label + " - " +
                    std::to_string(re->GetWidth()) + " x " +
                    std::to_string(re->GetHeight());
                window.SetTitle(title);
            }
        }
    }

    logger.Info(("[c3-exo3] Fenetre '" + label + "' fermee.").c_str());
}

int nkmain(const NkEntryState &state) {
    (void)state;

    // ------------------------------------------------------------------
    // 1) Borne personnalisee : minWidth/minHeight fixes explicitement.
    //    Reduisez la fenetre au maximum en tirant son coin : le titre
    //    affichera la taille la plus petite que le systeme accepte.
    //    Notez cette taille avant de fermer la fenetre.
    // ------------------------------------------------------------------
    {
        NkWindowConfig cfg;
        cfg.title     = "1 - Borne perso 400x300";
        cfg.width     = 800;
        cfg.height    = 600;
        cfg.minWidth  = 400;
        cfg.minHeight = 300;
        RunBorneTest("Borne perso 400x300", cfg);
    }

    // ------------------------------------------------------------------
    // 2) Borne par defaut du moteur (NkWindowConfig.h : 160 x 90),
    //    sans surcharge. Meme test, pour comparer avec le cas 1.
    // ------------------------------------------------------------------
    {
        NkWindowConfig cfg;
        cfg.title  = "2 - Borne par defaut";
        cfg.width  = 800;
        cfg.height = 600;
        RunBorneTest("Borne par defaut", cfg);
    }

    return 0;
}
