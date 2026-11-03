#include <NKCanvas/App/NkCanvasApp.h>
#include <NKCanvas/Renderer/Core/NkDrawable.h>
#include <NKCanvas/Renderer/Core/NkRenderStates.h>
#include <NKCanvas/Renderer/Core/NkTransformable.h>
#include <NKCanvas/Renderer/Resources/NkSprite.h>
#include <NKCanvas/Renderer/Resources/NkTexture.h>

#include <array>
#include <iostream>

using namespace nkentseu;
using namespace nkentseu::renderer;

class Composite : public NkTransformable, public NkDrawable {
public:
    explicit Composite(std::array<const NkDrawable*, 3> children)
        : mChildren(children) {}

    void Draw(NkRenderTarget& target,
              const NkRenderStates& states) const override {
        NkRenderStates local = states;
        local.transform *= GetTransform();

        for (const NkDrawable* child : mChildren) {
            if (child != nullptr) {
                target.Draw(*child, local);
            }
        }
    }

private:
    std::array<const NkDrawable*, 3> mChildren;
};

class Probe : public NkTransformable, public NkDrawable {
public:
    void Draw(NkRenderTarget&,
              const NkRenderStates& states) const override {
        NkRenderStates local = states;
        local.transform *= GetTransform();

        if (!mMeasured) {
            const auto point =
                local.transform.TransformPoint({0.f, 0.f});

            std::cout << "Mesure correcte : ("
                      << point.x << ", " << point.y << ")\n";

            mMeasured = true;
        }
    }

private:
    mutable bool mMeasured = false;
};

class BrokenProbe : public NkTransformable, public NkDrawable {
public:
    void Draw(NkRenderTarget&,
              const NkRenderStates&) const override {
        NkRenderStates local = NkRenderStates::Default();
        local.transform *= GetTransform();

        if (!mMeasured) {
            const auto point =
                local.transform.TransformPoint({0.f, 0.f});

            std::cout << "Mesure defectueuse : ("
                      << point.x << ", " << point.y << ")\n";

            mMeasured = true;
        }
    }

private:
    mutable bool mMeasured = false;
};

class CompositeApp : public NkCanvasApp {
public:
    bool OnInit() override {
        texture.Create(*Target().GetRenderer(), 40, 40,
                       NkColor2D(255, 100, 40, 255));

        sprite.SetTexture(texture, true);
        sprite.SetPosition(10.f, 5.f);

        probe.SetPosition(10.f, 5.f);
        brokenProbe.SetPosition(10.f, 5.f);

        middle.SetPosition(30.f, 20.f);
        middle.SetRotation(math::NkAngle(90.f));

        middle = Composite({&sprite, &probe, &brokenProbe});

        root.SetPosition(100.f, 80.f);
        root = Composite({&middle, nullptr, nullptr});

        return true;
    }

    void OnRender(NkRenderWindow& target) override {
        target.Clear();
        target.Draw(root, NkRenderStates::Default());
    }

private:
    NkTexture texture;
    NkSprite sprite;
    Probe probe;
    BrokenProbe brokenProbe;

    Composite middle{{&sprite, &probe, &brokenProbe}};
    Composite root{{&middle, nullptr, nullptr}};
};

int nkmain(const NkEntryState& state) {
    return NkCanvasApp::Run<CompositeApp>(state);
}
