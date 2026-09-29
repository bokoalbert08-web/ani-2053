# Exercice — L'inventaire des écrans

## Objectif

Écrire un programme qui affiche, pour chaque écran branché : sa taille, sa position, son facteur d'échelle, et lequel porte la fenêtre. Déplacer la fenêtre d'un écran à l'autre et vérifier que les valeurs suivent.


## Code

```cpp
#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkSystem.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkEventSystem.h"
#include <windows.h>
#include <cstdio>

using namespace nkentseu;

static void PrintDisplayInfo(const Window& win, const char* context) {
    NkVec2u dispSize = win.GetDisplaySize();
    NkVec2u dispPos  = win.GetDisplayPosition();
    float   dpiScale = win.GetDpiScale();

    printf("[%s] Ecran qui porte la fenetre :\n", context);
    printf("    Taille   : %u x %u\n", dispSize.x, dispSize.y);
    printf("    Position : (%d, %d)\n", dispPos.x, dispPos.y);
    printf("    Echelle  : %.2f\n", dpiScale);
}

int nkmain(const NkEntryState &state) {
    (void)state;

    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);

    if (!NkInitialise()) {
        printf("[app] NkInitialise a echoue\n");
        return -1;
    }

    NkWindowConfig cfg;
    cfg.title  = "c3-exo12 - Inventaire des ecrans";
    cfg.width  = 600;
    cfg.height = 400;
    cfg.x      = 100;
    cfg.y      = 100;

    Window win;
    if (!win.Create(cfg)) {
        printf("[app] creation de la fenetre echouee\n");
        return -1;
    }

    PrintDisplayInfo(win, "Position initiale");

    bool closeRequested = false;

    win.SetEventCallback([&](NkEvent* ev) {
        if (ev->type == NkEventType::NK_WINDOW_MOVE) {
            printf("\n[app] Fenetre deplacee vers (x=%d, y=%d)\n",
                   ev->data.windowMove.x, ev->data.windowMove.y);
            PrintDisplayInfo(win, "Apres deplacement");
        } else if (ev->type == NkEventType::NK_SYSTEM_DISPLAY_CHANGE) {
            printf("\n[app] Changement systeme d'affichage detecte "
                   "(moniteur ajoute/retire ou resolution changee)\n");
            PrintDisplayInfo(win, "Apres changement systeme");
        } else if (ev->type == NkEventType::NK_WINDOW_CLOSE) {
            closeRequested = true;
        }
    });

    EventSystem& events = EventSystem::Instance();
    while (win.IsOpen() && !closeRequested) {
        events.PollEvents();
        if (closeRequested && win.IsOpen()) win.Close();
        Sleep(1);
    }

    NkClose();
    return 0;
}
```

## Résultat observé à l'exécution

Machine de test : **un seul écran physique** branché. Extrait de la console (après quelques déplacements de la fenêtre à la souris) :

```
[Position initiale] Ecran qui porte la fenetre :
    Taille   : 1536 x 864
    Position : (0, 0)
    Echelle  : 1.25

[app] Fenetre deplacee vers (x=604, y=303)
[Apres deplacement] Ecran qui porte la fenetre :
    Taille   : 1920 x 1080
    Position : (0, 0)
    Echelle  : 1.25

[app] Fenetre deplacee vers (x=661, y=303)
[Apres deplacement] Ecran qui porte la fenetre :
    Taille   : 1920 x 1080
    Position : (0, 0)
    Echelle  : 1.25
```

(Les relevés suivants, après d'autres déplacements, restent tous à 1920x1080.)

## Analyse

Comme il n'y a qu'un seul écran, aucun changement de moniteur ne peut expliquer que la taille passe de 1536x864 à 1920x1080 entre le premier relevé et le suivant. Le calcul donne l'explication : `1920 ÷ 1.25 = 1536` et `1080 ÷ 1.25 = 864`. Le premier relevé est exactement le second divisé par le facteur d'échelle affiché — la résolution physique réelle de l'écran, d'après les paramètres Windows, est bien 1920x1080 à 125 %.

Mon hypothèse est que **le tout premier appel à `GetDisplaySize()`**, fait juste après `win.Create(cfg)`, renvoie une valeur en pixels logiques (divisée par l'échelle), alors que **les appels suivants**, faits depuis le callback après un déplacement, renvoient la valeur en pixels physiques. En lisant `NkWin32WindowImpl.cpp`, la fenêtre est créée avec un contexte de sensibilité au DPI par moniteur activé juste avant la création (`NkSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)`) : il est plausible que ce contexte ne soit pas encore pleinement effectif au moment du tout premier appel, avant que la boucle d'événements ait tourné au moins une fois.

Je n'ai pas vérifié cette hypothèse plus loin (par exemple en rappelant `GetDisplaySize()` une seconde fois sans bouger la fenêtre, pour voir si la valeur change toute seule) : c'est une lecture du code et un calcul à partir de deux relevés, pas une certitude établie.

`GetDisplayPosition()` reste à `(0, 0)` dans tous les relevés, ce qui est cohérent avec un seul écran positionné à l'origine dans la configuration Windows — je n'ai pas pu tester le cas de deux écrans côte à côte, n'ayant qu'un seul moniteur disponible.
