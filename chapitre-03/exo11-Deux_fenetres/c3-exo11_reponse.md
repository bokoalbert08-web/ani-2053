# Exercice — Deux fenêtres

## Objectif

Ouvrir deux fenêtres et afficher, pour chaque clic, laquelle l'a reçu. Dire ensuite ce qui manquerait pour dessiner dans les deux.

## Code

```cpp
#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkSystem.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkEventSystem.h"
#include <windows.h>
#include <cstdio>

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);

    // Initialise la plateforme et attache l'EventImpl au systeme d'evenements.
    // Sans cet appel, personne ne traite les messages Windows.
    if (!NkInitialise()) {
        printf("[app] NkInitialise a echoue\n");
        return -1;
    }

    NkWindowConfig cfgA;
    cfgA.title  = "Fenetre A";
    cfgA.width  = 600;
    cfgA.height = 400;
    cfgA.x      = 100;
    cfgA.y      = 100;

    NkWindowConfig cfgB;
    cfgB.title  = "Fenetre B";
    cfgB.width  = 600;
    cfgB.height = 400;
    cfgB.x      = 800;
    cfgB.y      = 100;

    Window winA;
    Window winB;
    if (!winA.Create(cfgA) || !winB.Create(cfgB)) {
        printf("[app] creation d'une des fenetres echouee\n");
        return -1;
    }

    bool closeA = false;
    bool closeB = false;

    // Un callback par fenetre : c'est lui qui sait quelle fenetre l'a recu.
    winA.SetEventCallback([&](NkEvent* ev) {
        if (ev->type == NkEventType::NK_MOUSE_BUTTON_PRESS)
            printf("Clic recu par la fenetre A (x=%d, y=%d)\n",
                   ev->data.mouseButton.x, ev->data.mouseButton.y);
        else if (ev->type == NkEventType::NK_WINDOW_CLOSE)
            closeA = true;
    });

    winB.SetEventCallback([&](NkEvent* ev) {
        if (ev->type == NkEventType::NK_MOUSE_BUTTON_PRESS)
            printf("Clic recu par la fenetre B (x=%d, y=%d)\n",
                   ev->data.mouseButton.x, ev->data.mouseButton.y);
        else if (ev->type == NkEventType::NK_WINDOW_CLOSE)
            closeB = true;
    });

    EventSystem& events = EventSystem::Instance();
    while (winA.IsOpen() || winB.IsOpen()) {
        events.PollEvents();
        if (closeA && winA.IsOpen()) winA.Close();
        if (closeB && winB.IsOpen()) winB.Close();
        Sleep(1);
    }

    NkClose();
    return 0;
}
```

## Résultat observé à l'exécution

Les deux fenêtres « Fenetre A » et « Fenetre B » s'ouvrent côte à côte, répondent (on peut les déplacer) et chaque clic est signalé dans la console avec le nom de la fenêtre qui l'a reçu. Extrait de la console :

```
Clic recu par la fenetre A (x=432, y=185)
Clic recu par la fenetre B (x=500, y=259)
Clic recu par la fenetre B (x=484, y=260)
Clic recu par la fenetre A (x=424, y=315)
```

Les coordonnées sont celles de la zone cliente de la fenêtre concernée. Dans le journal complet, certaines lignes apparaissent plusieurs fois de suite avec les mêmes coordonnées ; je n'ai pas cherché à savoir s'il s'agit de clics répétés au même endroit ou d'une double livraison de l'événement.

## Comment la fenêtre source est identifiée

`NkEvent` possède bien un champ `Window* window` (`NkEvent.h`, ligne 120), mais en cherchant dans le code source de la bibliothèque (`.window =` et `->window =`), je n'ai trouvé aucune affectation de ce champ dans le chemin Win32 : les événements arrivent donc sans indiquer leur fenêtre. Je n'ai pas vérifié cette valeur à l'exécution.

À la place, j'utilise `Window::SetEventCallback(...)`, qui enregistre un callback pour **cette fenêtre uniquement** (elle passe par `IEventImpl::SetWindowCallback` avec le `HWND` de la fenêtre). Chaque fenêtre a sa propre lambda, qui connaît son nom (« A » ou « B ») : c'est elle qui affiche « Clic recu par la fenetre A/B ». La fermeture est traitée de la même façon : le callback lève un indicateur, et la boucle principale appelle `Close()` sur la bonne fenêtre.

## Correctifs nécessaires pour obtenir ce résultat

Deux problèmes empêchaient les événements d'arriver ; ils concernent la bibliothèque fournie, pas la logique de l'exercice :

1. **`NkInitialise()` doit être appelé** au début de `nkmain`. C'est lui qui attache l'`EventImpl` au système d'événements (`NkSystem.cpp`, ligne 57), et rien dans le point d'entrée `WinMain` ne l'appelle. Sans lui, mes exercices 6 et 10 avaient des fenêtres qui se figeaient ; l'exercice 11, écrit avec cet appel, a des fenêtres qui répondent. Je n'ai pas isolé cet effet par un test comparatif sur un même programme : c'est cohérent, mais pas démontré.
2. **Un défaut dans `WindowProcStatic`** (`NkWin32EventImpl.cpp`) : la variable `sCurrentImpl` n'était jamais assignée, car `sPendingEventImpl` était remis à `nullptr` avant d'être copié. Tous les messages partaient donc vers `DefWindowProc`, et aucun `NkEvent` n'était produit (la fenêtre se déplaçait, car Windows gère cela seul, mais les clics ne produisaient rien). J'ai corrigé en déclarant `sCurrentImpl` en tête de fonction et en lui copiant `sPendingEventImpl` avant de le remettre à zéro. Avec ce seul changement (même programme, simplement recompilé), les clics se sont mis à arriver : c'est une observation directe.

## Ce qui manquerait pour dessiner dans les deux fenêtres

Ce que j'ai vu : `GetSurfaceDesc()` renvoie, pour chaque fenêtre, un descripteur avec son propre `hwnd` ; les deux fenêtres restent noires, car mon programme ne dessine rien. Le reste est une déduction à partir de la lecture du code, non testée :

- **Un moteur de rendu (ou un contexte graphique) par fenêtre**, relié à la surface de cette fenêtre. Je n'ai pas étudié `NkRenderer` en détail ; je ne sais donc pas s'il gère plusieurs fenêtres.
- **Un moyen de désigner la fenêtre cible avant de dessiner** (contexte courant, ou cible de rendu explicite), puisque les deux fenêtres tournent dans le même thread.
- **Un événement de rafraîchissement lié à sa fenêtre** : comme `ev->window` n'est pas rempli côté Win32, il faudrait, comme pour les clics, passer par un callback par fenêtre pour savoir laquelle redessiner ou redimensionner.
- **Un état par fenêtre** (taille, contenu à afficher), à mettre à jour indépendamment pour chacune.
