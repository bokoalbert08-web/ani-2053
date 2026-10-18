// Le futur qui ne bloque pas : une grande image est chargée dans un autre fil
// pendant que la fenêtre dessine un carré qui avance d'un pixel par trame.
// Deux façons de récupérer le résultat, choisies par une constante.
// Note : avec le kit du 13 octobre, NkFuture.h ne compile pas tel quel : NkScopedLock
// y est écrit sans modèle (il faut NkScopedLockMutex) et memory::MakeSharedPtr n'existe pas
// (c'est memory::NkMakeShared). J'ai corrigé ces dix lignes dans ma copie du kit.

#include "NKWindow/NKMain.h"
#include "NKCanvas/App/NkCanvasApp.h"
#include "NKCanvas/Renderer/Shapes/NkRectangleShape.h"
#include "NKImage/Core/NkImage.h"
#include "NKThreading/NkThreadPool.h"
#include "NKThreading/NkFuture.h"
#include "NKTime/NkChrono.h"
#include "NKCore/NkTraits.h"

#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::renderer;
using namespace nkentseu::threading;

// true : version Get (on attend tout de suite).
// false : version IsReady (on demande à chaque trame).
static const bool kVersionGet = false;

static const char* kImage = "grande-photo.png";
static const int kTrameLancement = 60;   // le chargement démarre à cette trame
static const int kTramesApres = 60;      // trames dessinées après l'arrivée, puis on quitte
static const int kTramesMesurees = 3;    // trames d'après l'arrivée comptées dans la plus longue

class FuturApp : public NkCanvasApp {
public:
    FuturApp() {
        Config().title = kVersionGet ? "Futur : Get" : "Futur : IsReady";
        Config().width = 960;
        Config().height = 540;
        Config().clearColor = NkColor2D{18, 18, 24, 255};
    }

    void OnUpdate(float32) override {
        // Durée de la trame précédente : temps écoulé depuis le dernier OnUpdate.
        const double ms = mChrono.Reset().milliseconds;
        ++mTrame;

        // Le carré avance d'un pixel par trame.
        mX = static_cast<float32>(mTrame % 900);

        if (mEtat == Etat::Avant) {
            if (mTrame == kTrameLancement) {
                Lancer();
                mEtat = Etat::Charge;
                if (kVersionGet) {
                    // Version Get : on attend tout de suite, la boucle est bloquée ici.
                    Recue(mFutur.Get());
                    mEtat = Etat::Apres;
                }
            }
        } else if (mEtat == Etat::Charge) {
            // Version IsReady : une question par trame, jamais d'attente.
            Retenir(ms);
            if (mFutur.IsReady()) {
                Recue(mFutur.Get());
                mEtat = Etat::Apres;
            } else {
                ++mTramesPendant;
            }
        } else if (mEtat == Etat::Apres) {
            if (mApres < kTramesMesurees) {
                Retenir(ms);   // la trame qui a payé le blocage de Get se lit ici
            }
            ++mApres;
            if (mApres >= kTramesApres) {
                Ecrire();
                mEtat = Etat::Fini;
                Quit();
            }
        }
    }

    void OnRender(NkRenderWindow& cible) override {
        NkRectangleShape carre(NkVec2f{40.f, 40.f});
        carre.SetPosition(20.f + mX, 250.f);
        carre.SetFillColor(NkColor2D{240, 180, 40, 255});
        cible.Draw(carre);
    }

private:
    enum class Etat { Avant, Charge, Apres, Fini };

    // Le chargement tourne dans la réserve : la tâche charge des pixels,
    // elle ne dessine pas. Promesse et réserve vivent autant que l'application.
    void Lancer() {
        mFutur = mPromesse.GetFuture();
        mPool.Enqueue([this]() {
            NkChrono chrono;
            NkImage img;
            img.Load(kImage);
            mDureeChargement = chrono.Elapsed().milliseconds;
            mPromesse.SetValue(nkentseu::traits::NkMove(img));
        });
    }

    void Recue(const NkImage& img) {
        mLargeur = img.Width();
        mHauteur = img.Height();
        mValide = img.IsValid();
    }

    void Retenir(double ms) {
        if (ms > mPlusLongue) {
            mPlusLongue = ms;
        }
    }

    void Ecrire() {
        std::FILE* f = std::fopen("futur-resultats.txt", "w");
        if (f == nullptr) {
            return;
        }
        std::fprintf(f, "version : %s\n", kVersionGet ? "Get" : "IsReady");
        std::fprintf(f, "image : %d x %d, valide : %d\n", mLargeur, mHauteur, mValide ? 1 : 0);
        std::fprintf(f, "chargement : %.3f ms\n", mDureeChargement);
        std::fprintf(f, "trame la plus longue : %.3f ms\n", mPlusLongue);
        std::fprintf(f, "trames pendant le chargement : %d\n", mTramesPendant);
        std::fclose(f);
    }

    NkChrono mChrono;
    Etat mEtat = Etat::Avant;
    int mTrame = 0;
    int mApres = 0;
    int mTramesPendant = 0;
    double mPlusLongue = 0.0;
    double mDureeChargement = 0.0;
    int mLargeur = 0;
    int mHauteur = 0;
    bool mValide = false;
    float32 mX = 0.f;

    // Déclarés en dernier : la réserve est détruite en premier, donc la tâche
    // est finie avant que la promesse ne disparaisse.
    NkPromise<NkImage> mPromesse;
    NkFuture<NkImage> mFutur;
    NkThreadPool mPool;
};

int nkmain(const NkEntryState& state) {
    return NkCanvasApp::Run<FuturApp>(state);
}
