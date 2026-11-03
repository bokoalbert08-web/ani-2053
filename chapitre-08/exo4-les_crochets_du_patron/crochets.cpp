#include "NKWindow/NKMain.h"
#include "NKCanvas/App/NkCanvasApp.h"

#include <fstream>
#include <iomanip>

using namespace nkentseu;
using namespace nkentseu::renderer;

class CrochetsApp : public NkCanvasApp {
public:
    CrochetsApp() {
        Config().title = "Mesure des crochets";
        Config().width = 640;
        Config().height = 480;
    }

protected:
    void Journal(const char* nom) {
        if (!mJournal.is_open()) return;

        mJournal << std::setw(2) << std::setfill('0') << ++mOrdre
                 << " | " << nom
                 << " | image=" << mImage
                 << " | Time=" << std::fixed << std::setprecision(3)
                 << Time() << '\n';
        mJournal.flush();
    }

    NkOptional<int> OnCommandLine(const NkVector<NkString>& args) override {
        mJournal.open(
            "C:/Users/O/Documents/Games/crochets-journal.txt",
            std::ios::out | std::ios::trunc
        );
        Journal("OnCommandLine");
        return NkOptional<int>();
    }

    bool OnInit() override {
        Journal("OnInit");
        return true;
    }

    void OnUpdate(float32 deltaTime) override {
        ++mImage;

        if (mImage <= 3) {
            Journal("OnUpdate");
        }

        if (mImage >= 3) {
            Quit();
        }
    }

    void OnRender(NkRenderWindow& target) override {
        if (mImage <= 3) {
            Journal("OnRender");
        }
    }

    void OnShutdown() override {
        Journal("OnShutdown");
        if (mJournal.is_open()) {
            mJournal.close();
        }
    }

    bool OnEvent(const NkEvent& event) override {
        Journal("OnEvent");
        return false;
    }

    bool OnPointer(const NkPointer& pointer) override {
        Journal("OnPointer");
        return false;
    }

    bool OnKeyPress(const NkKeyPressEvent& event) override {
        Journal("OnKeyPress");
        return false;
    }

    void OnLayout(const NkLayoutInfo& layout) override {
        Journal("OnLayout");
    }

    bool OnCloseRequested() override {
        Journal("OnCloseRequested");
        return true;
    }

    void OnPause() override {
        Journal("OnPause");
    }

    void OnResume() override {
        Journal("OnResume");
    }

private:
    std::ofstream mJournal;
    int mOrdre = 0;
    int mImage = 0;
};

int nkmain(const NkEntryState& state) {
    return NkCanvasApp::Run<CrochetsApp>(state);
}
