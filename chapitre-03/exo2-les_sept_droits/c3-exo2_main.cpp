#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "C3Exo2SeptDroits";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

// Ouvre une fenetre avec la configuration donnee, attend qu'elle soit
// fermee par l'utilisateur, puis revient. Utilisee sept fois de suite,
// une fois par droit desactive.
static void RunWindowTest(const std::string& label, NkWindowConfig cfg) {
    cfg.title = label;

    NkWindow window(cfg);
    if (!window.IsValid()) {
        logger.Error(("[c3-exo2] Creation fenetre KO : " + label).c_str());
        return;
    }

    bool running = true;
    NkEventSystem &events = NkEvents();
    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent*) { running = false; });

    while (running && window.IsOpen()) {
        events.PollEvents();
    }

    window.Close();
    logger.Info(("[c3-exo2] Fenetre '" + label + "' fermee.").c_str());
}

int nkmain(const NkEntryState &state) {
    (void)state;

    // ------------------------------------------------------------------
    // Sept fenetres successives, chacune avec un seul droit desactive.
    // Toutes les autres proprietes restent aux valeurs par defaut de
    // NkWindowConfig (voir NkWindowConfig.h).
    // ------------------------------------------------------------------

    {
        NkWindowConfig cfg;
        cfg.resizable = false;
        RunWindowTest("1 - resizable = false", cfg);
    }

    {
        NkWindowConfig cfg;
        cfg.movable = false;
        RunWindowTest("2 - movable = false", cfg);
    }

    {
        NkWindowConfig cfg;
        cfg.closable = false;
        RunWindowTest("3 - closable = false", cfg);
    }

    {
        NkWindowConfig cfg;
        cfg.minimizable = false;
        RunWindowTest("4 - minimizable = false", cfg);
    }

    {
        NkWindowConfig cfg;
        cfg.maximizable = false;
        RunWindowTest("5 - maximizable = false", cfg);
    }

    {
        NkWindowConfig cfg;
        cfg.canFullscreen = false;
        RunWindowTest("6 - canFullscreen = false", cfg);
    }

    {
        NkWindowConfig cfg;
        cfg.frame = false;
        RunWindowTest("7 - frame = false", cfg);
    }

    return 0;
}
