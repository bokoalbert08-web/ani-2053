#include <cstdio>

#include "NKWindow/NKMain.h"
#include "NKCanvas/App/NkCanvasApp.h"
#include "NKCanvas/Renderer/Resources/NkSprite.h"
#include "NKFont/Embedded/NkFontEmbedded.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

class InterfaceFixe : public NkCanvasApp {
public:
    InterfaceFixe() {
        Config().title = "Interface fixe";
        Config().width = 800;
        Config().height = 450;
        Config().clearColor = NkColor2D{18, 18, 24, 255};
    }

protected:
    bool OnInit() override {
        return mPolice.LoadEmbedded(*Target().GetRenderer(), NkEmbeddedFontId::DroidSans);
    }

    void OnUpdate(float32 dt) override {
        mCentreX += kVitesse * dt;
        mHorloge += dt;
    }

    void OnRender(NkRenderWindow &target) override {
        // 1. Le monde : une rangee de carres, vue par une vue dont le centre avance avec dt.
        NkView2D vue;
        vue.center = {mCentreX, 225.f};
        vue.size = {800.f, 450.f};
        target.SetView(vue);

        for (int i = 0; i < 80; ++i) {
            const NkColor2D couleur = (i % 2 == 0) ? NkColor2D::Red : NkColor2D{60, 120, 220, 255};
            target.GetRenderer2D().DrawFilledRect({i * 100.f, 200.f, 60.f, 60.f}, couleur);
        }

        // 2. L'interface. Une vue n'est pas un objet qu'on dessine : c'est un etat du
        //    renderer, qui s'applique a tout ce qui suit.
        //
        //    VERSION FAUTIVE (sans l'appel) : la barre est dessinee avec la vue du monde,
        //    donc elle defile avec les carres.
        //        target.GetRenderer2D().DrawFilledRect({0.f, 0.f, 800.f, 40.f}, ...);
        //
        //    VERSION CORRECTE : on revient a la vue par defaut avant l'interface.
        target.ResetView();
        target.GetRenderer2D().DrawFilledRect({0.f, 0.f, 800.f, 40.f}, NkColor2D{40, 40, 60, 255});

        NkText texte(mPolice, "Interface fixe", 20);
        texte.SetPosition({300.f, 27.f});  // la position d'un texte est sa ligne de base
        target.Draw(texte);

        // 3. Le journal : une ligne par seconde, avec la position ECRAN de la barre.
        if (mHorloge >= mProchaineLigne) {
            mProchaineLigne += 1.f;
            const int barreX = target.MapCoordsToPixel({0.f, 0.f}).x;
            std::printf("reset : %s, centre_x : %d, barre_x : %d\n",
                        target.IsViewCustom() ? "non" : "oui", static_cast<int>(mCentreX), barreX);
            std::fflush(stdout);
        }
    }

private:
    static constexpr float32 kVitesse = 50.f;

    renderer::NkFont mPolice;
    float32 mCentreX = 400.f;
    float32 mHorloge = 0.f;
    float32 mProchaineLigne = 1.f;
};

int nkmain(const NkEntryState &state) {
    return NkCanvasApp::Run<InterfaceFixe>(state);
}
