// Le singleton et le test : deux versions d'une fonction de saut,
// et un banc de test lancé depuis OnCommandLine, sans fenêtre.

#include "NKWindow/NKMain.h"
#include "NKCanvas/App/NkCanvasApp.h"
#include "NKEvent/NkEventDispatcher.h"

#include <cstdio>
#include <cstdlib>

using namespace nkentseu;
using namespace nkentseu::renderer;

// ── Version directe : interroge NkInput elle-même.
// Problème : on ne peut pas lui fournir un autre état pour la tester.
static bool PersonnageSauteDirecte(bool estAuSol)
{
    return estAuSol && NkInput.IsKeyDown(NkKey::NK_SPACE);
}

// ── Version testable : la règle de saut est la même, mais l'état
// de la touche arrive en paramètre. La fonction ne lit plus NkInput :
// le test peut lui passer true ou false sans fenêtre ni matériel.
static bool PersonnageSauteTestable(bool estAuSol, bool espacePresse)
{
    return estAuSol && espacePresse;
}

static int LancerBanc(std::FILE* f)
{
    int echecs = 0;
    auto cas = [&](const char* nom, bool attendu, bool obtenu) {
        const bool ok = (obtenu == attendu);
        if (!ok) ++echecs;
        std::fprintf(f, "%s : attendu=%d obtenu=%d %s\n",
                     nom, attendu ? 1 : 0, obtenu ? 1 : 0, ok ? "OK" : "ECHEC");
        std::fflush(f);
    };

    // ── Cas de la version testable (quatre cas)
    cas("testable : au sol + espace enfonce  -> saute",
        true,  PersonnageSauteTestable(true,  true));
    cas("testable : au sol + espace relache  -> ne saute pas",
        false, PersonnageSauteTestable(true,  false));
    cas("testable : en l'air + espace enfonce -> ne saute pas",
        false, PersonnageSauteTestable(false, true));
    cas("testable : en l'air + espace relache -> ne saute pas",
        false, PersonnageSauteTestable(false, false));

    // ── Cas de la version directe (NkInput non initialise : retourne false)
    cas("directe : au sol, sans fenetre (NkInput = false par defaut) -> ne saute pas",
        false, PersonnageSauteDirecte(true));
    cas("directe : en l'air, sans fenetre                            -> ne saute pas",
        false, PersonnageSauteDirecte(false));

    return echecs;
}

class BancApp : public NkCanvasApp {
public:
    BancApp() {
        Config().title = "Banc singleton";
        Config().width  = 320;
        Config().height = 240;
    }

    // OnCommandLine est appelée avant la fenêtre.
    // Si elle rend une valeur, le programme s'arrête avec ce code de sortie.
    NkOptional<int> OnCommandLine(const NkVector<NkString>& args) override {
        (void)args;
        std::FILE* f = std::fopen("banc-resultats.txt", "w");
        if (f == nullptr) {
            return NkOptional<int>(1);
        }
        const int echecs = LancerBanc(f);
        std::fprintf(f, "TOTAL : %d echec(s)\n", echecs);
        std::fclose(f);
        return NkOptional<int>(echecs);   // code de sortie = nombre d'échecs
    }
};

int nkmain(const NkEntryState& state) {
    return NkCanvasApp::Run<BancApp>(state);
}
