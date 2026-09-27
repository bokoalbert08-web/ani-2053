# Exercice — Les sept curseurs

## Objectif

Découper la fenêtre en sept zones et changer la forme du curseur selon la zone survolée. Puis ne poser le curseur qu'une seule fois, au démarrage, et décrire ce qui se passe.

## Code (partie 1 — mise à jour continue)

```cpp
#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkEventSystem.h"
#include <windows.h>
#include <cstdio>

using namespace nkentseu;

static HCURSOR CursorForZone(int zone)
{
    switch (zone)
    {
        case 0: return LoadCursorA(nullptr, IDC_ARROW);
        case 1: return LoadCursorA(nullptr, IDC_HAND);
        case 2: return LoadCursorA(nullptr, IDC_CROSS);
        case 3: return LoadCursorA(nullptr, IDC_WAIT);
        case 4: return LoadCursorA(nullptr, IDC_IBEAM);
        case 5: return LoadCursorA(nullptr, IDC_SIZEALL);
        default: return LoadCursorA(nullptr, IDC_NO);
    }
}

int nkmain(const NkEntryState &state) {
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);

    NkWindowConfig cfg;
    cfg.title  = "Les sept curseurs";
    cfg.width  = 1400;
    cfg.height = 700;

    Window window(cfg);
    if (!window.IsOpen()) {
        printf("[app] creation fenetre echouee\n");
        return -1;
    }

    NkU32 windowWidth = window.GetSize().x;

    EventSystem &events = EventSystem::Instance();
    int lastZone = -1;

    while (window.IsOpen()) {
        while (NkEvent* ev = events.PollEvent()) {
            if (ev->type == NkEventType::NK_WINDOW_CLOSE) {
                window.Close();
            }
            else if (ev->type == NkEventType::NK_MOUSE_MOVE) {
                NkI32 x = ev->data.mouseMove.x;
                int zone = (windowWidth > 0)
                    ? static_cast<int>((static_cast<NkU32>(x < 0 ? 0 : x) * 7) / windowWidth)
                    : 0;
                if (zone > 6) zone = 6;
                if (zone < 0) zone = 0;

                if (zone != lastZone) {
                    SetCursor(CursorForZone(zone));
                    printf("Zone %d : curseur change\n", zone);
                    lastZone = zone;
                }
                else {
                    SetCursor(CursorForZone(zone));
                }
            }
        }
    }

    return 0;
}
```

## Découpage retenu

La fenêtre (1400 px de large) est divisée en 7 bandes verticales égales (200 px chacune), chacune associée à une forme de curseur Win32 distincte : flèche, main, croix, sablier, texte, déplacement, interdit.

## Partie 2 — poser le curseur une seule fois au démarrage

En remplaçant la boucle d'événements par un simple `SetCursor(...)` appelé une fois avant la boucle `while (window.IsOpen()) {}` (sans le rappeler ensuite), le comportement attendu d'après la documentation Win32 est le suivant :

- `SetCursor()` ne change l'apparence du curseur que jusqu'au **prochain traitement du message `WM_SETCURSOR`**.
- Ce message est envoyé automatiquement par Windows dès que la souris bouge au-dessus de la zone cliente de la fenêtre.
- Si l'application ne le traite pas explicitement, le comportement par défaut (`DefWindowProc`) **réinitialise le curseur au curseur associé à la classe de la fenêtre** (celui déclaré à la création, `IDC_ARROW` dans notre cas).
- Donc en ne posant le curseur qu'une seule fois, la forme personnalisée ne serait visible qu'un instant avant que la souris ne bouge, puis reviendrait à la flèche normale dès le premier mouvement.

C'est exactement ce que le chapitre annonçait : *« le curseur, sa forme se redemande à chaque image, il faut le savoir »* — `SetCursor()` n'est pas un réglage permanent, c'est une réponse à renouveler à chaque événement `WM_SETCURSOR` (ou, comme dans notre code, à chaque déplacement de souris).

## Remarque importante sur les tests effectués

**Je n'ai pas pu confirmer ce comportement par une observation directe.** La fenêtre affichait "ne répond pas" une fois lancée, sans que je puisse vérifier avec certitude si les changements de curseur et les messages de la console (`Zone X : curseur change`) s'affichaient correctement pendant les déplacements de souris. Le code ci-dessus compile sans erreur avec la bibliothèque du cours, et le raisonnement de la partie 2 s'appuie sur la documentation officielle de l'API Win32 (`SetCursor`, `WM_SETCURSOR`).
