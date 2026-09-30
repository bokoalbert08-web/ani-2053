#include "NKWindow/NKMain.h"
#include "NKCanvas/App/NkCanvasApp.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

class CarreCoquille : public NkCanvasApp {
public:
    CarreCoquille() {
        Config().title = "Carre avec la coquille";
        Config().width = 800;
        Config().height = 450;
        Config().clearColor = NkColor2D{18, 18, 24, 255};
    }

protected:
    void OnUpdate(float32 dt) override {
        mX += 100.f * dt;
        if (mX > 800.f) {
            mX = -50.f;
        }
    }

    void OnRender(NkRenderWindow &target) override {
        target.GetRenderer2D().DrawFilledRect({mX, 200.f, 50.f, 50.f}, NkColor2D::Red);
    }

private:
    float32 mX = 0.f;
};

int nkmain(const NkEntryState &state) {
    return NkCanvasApp::Run<CarreCoquille>(state);
}
