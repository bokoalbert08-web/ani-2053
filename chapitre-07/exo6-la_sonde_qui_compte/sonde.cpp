// La sonde qui compte : un compteur et une ligne de journal dans OnRender,
// pendant dix secondes, avec ou sans plafond de cadence.
//   SONDE_FPS = 60 (plafond par défaut) ou 0 (aucun plafond)
//   SONDE_LOG = 1 (une ligne de journal par passage) ou 0 (compteur seul)

#include "NKWindow/NKMain.h"
#include "NKCanvas/App/NkCanvasApp.h"
#include "NKLogger/NkLog.h"   // en dernier : il définit la macro « logger »

#include <cstdio>
#include <cstdlib>

using namespace nkentseu;
using namespace nkentseu::renderer;

class SondeApp : public NkCanvasApp {
public:
    SondeApp() {
        const char* fps = std::getenv("SONDE_FPS");
        const char* journal = std::getenv("SONDE_LOG");
        mFps = fps != nullptr ? std::atoi(fps) : 60;
        mJournal = journal == nullptr || std::atoi(journal) != 0;

        Config().title = "La sonde qui compte";
        Config().width = 640;
        Config().height = 360;
        Config().clearColor = NkColor2D{18, 18, 24, 255};
        Config().imagesParSeconde = mFps;   // cadence visée : 60, ou 0 pour aucun plafond
    }

    void OnRender(NkRenderWindow&) override {
        ++mPassages;   // le compteur : une unité à chaque passage dans OnRender

        if (mJournal) {
            char ligne[96];
            std::snprintf(ligne, sizeof(ligne), "sonde passage %d, Time = %.3f s",
                          mPassages, static_cast<double>(Time()));
            logger.Info(ligne);
        }

        // À 10 secondes : on écrit le compte dans un fichier, puis on arrête.
        if (!mFini && Time() >= 10.0f) {
            mFini = true;
            std::FILE* f = std::fopen("sonde-resultat.txt", "w");
            if (f != nullptr) {
                std::fprintf(f, "imagesParSeconde=%d journal=%d passages=%d Time=%.3f\n",
                             mFps, mJournal ? 1 : 0, mPassages, static_cast<double>(Time()));
                std::fclose(f);
            }
            Quit();
        }
    }

private:
    int mFps = 60;
    bool mJournal = true;
    int mPassages = 0;
    bool mFini = false;
};

int nkmain(const NkEntryState& state) {
    return NkCanvasApp::Run<SondeApp>(state);
}
