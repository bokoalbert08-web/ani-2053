// Qui appelle vraiment : une classe qui hérite de NkDrawable et redéfinit Draw
// (quatre sommets en NK_LINES), dessinée par target.Draw(objet) dans OnRender.

#include "NKWindow/NKMain.h"
#include "NKCanvas/App/NkCanvasApp.h"
#include "NKCanvas/Renderer/Core/NkDrawable.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static NkVertex Sommet(float x, float y, uint8 r, uint8 g, uint8 b)
{
    NkVertex s;
    s.x = x;
    s.y = y;
    s.u = 0.f;
    s.v = 0.f;
    s.r = r;
    s.g = g;
    s.b = b;
    s.a = 255;
    return s;
}

// Une croix : deux lignes, donc quatre sommets en NK_LINES.
class Croix : public NkDrawable {
public:
    void Draw(NkRenderTarget &target, const NkRenderStates &states) const override
    {
        NkVertex sommets[4];
        sommets[0] = Sommet(300.f, 150.f, 240, 180, 40);
        sommets[1] = Sommet(660.f, 390.f, 240, 180, 40);
        sommets[2] = Sommet(660.f, 150.f, 80, 200, 240);
        sommets[3] = Sommet(300.f, 390.f, 80, 200, 240);
        target.Draw(sommets, 4, NkPrimitiveType::NK_LINES, states);
    }
};

class AppelantsApp : public NkCanvasApp {
public:
    AppelantsApp() {
        Config().title = "Qui appelle vraiment";
        Config().width = 960;
        Config().height = 540;
        Config().clearColor = NkColor2D{18, 18, 24, 255};
    }

    void OnUpdate(float32) override {
        // La fenêtre se ferme toute seule après environ 3 secondes.
        if (++mTrame > 180) {
            Quit();
        }
    }

    void OnRender(NkRenderWindow &target) override {
        target.Draw(mCroix);
    }

private:
    int mTrame = 0;
    Croix mCroix;
};

int nkmain(const NkEntryState &state) {
    return NkCanvasApp::Run<AppelantsApp>(state);
}
