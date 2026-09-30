#include "NKWindow/NKMain.h"
#include "NKCanvas/App/NkCanvasApp.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

class FenetreNue : public NkCanvasApp {
public:
    FenetreNue() {
        Config().title = "Fenetre nue";
        Config().width = 960;
        Config().height = 540;
        Config().clearColor = NkColor2D{18, 18, 24, 255};
    }
};

int nkmain(const NkEntryState &state) {
    return NkCanvasApp::Run<FenetreNue>(state);
}
