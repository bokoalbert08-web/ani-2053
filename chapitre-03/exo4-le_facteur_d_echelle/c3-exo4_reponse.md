# Exercice — Le facteur d'échelle

## Objectif

Afficher côte à côte trois valeurs : la taille rendue par la fenêtre, la taille rendue par la cible de rendu, et le facteur d'échelle entre les deux.

## Code

```cpp
#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkWindow.h"
#include <windows.h>
#include <cstdio>

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    // Console de debogage pour afficher les resultats (utile car ceci est une
    // application fenetree Windows : sans elle, printf ne s'affiche nulle part).
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);

    NkWindowConfig cfg;
    cfg.title  = "Facteur d'echelle";
    cfg.width  = 1280;
    cfg.height = 720;

    Window window(cfg);
    if (!window.IsOpen()) {
        printf("[app] creation fenetre echouee\n");
        return -1;
    }

    NkVec2u windowSize    = window.GetSize();
    NkSurfaceDesc surface = window.GetSurfaceDesc();
    float scale           = window.GetDpiScale();

    printf("Taille fenetre     : %u x %u\n", windowSize.x, windowSize.y);
    printf("Taille cible rendu : %u x %u\n", surface.width, surface.height);
    printf("Facteur d'echelle  : %.2f\n", scale);

    while (window.IsOpen()) {}
    return 0;
}
```

## Résultat obtenu à l'exécution

```
Taille fenetre     : 1281 x 721
Taille cible rendu : 1281 x 721
Facteur d'echelle  : 1.25
```

L'écran de test a un facteur d'échelle de 1.25 (125%), ce qui confirme la mise en garde du chapitre sur la différence entre pixels logiques et pixels physiques. La légère différence entre la taille demandée (1280x720) et la taille obtenue (1281x721) vient de la manière dont la bibliothèque calcule la zone cliente incluant la bordure.

## Méthodes utilisées et leur origine

Ces méthodes ont été retrouvées directement dans le header `NkWindow.h` de la bibliothèque (classe `Window`, dans le namespace `nkentseu`), le chapitre ne les détaillant pas explicitement :

| Méthode | Rôle |
|---|---|
| `window.GetSize()` | Renvoie la taille de la **fenêtre** (`NkVec2u`, champs `.x` / `.y`) |
| `window.GetSurfaceDesc()` | Renvoie le descripteur de la **cible de rendu** (`NkSurfaceDesc`, champs `.width` / `.height`) |
| `window.GetDpiScale()` | Renvoie le **facteur d'échelle** de l'écran (`float`) |

Un `using namespace nkentseu;` a été ajouté après les includes pour éviter de préfixer chaque type.

## Difficultés de compilation rencontrées et corrections apportées

La compilation ne fonctionnait initialement pour aucun exercice du chapitre (y compris l'exercice 1), à cause de plusieurs bugs indépendants du code de l'exercice, présents dans le matériel du cours (dossier `NKWindow` fourni avec l'exemple Jenga) :

1. **Trois signatures incompatibles** entre l'interface `IEventImpl` et son implémentation Win32 (`NkWin32EventImpl`) : `Front()`, `PushEvent()` et `DispatchEvent()` utilisaient des types différents (référence au lieu de pointeur, valeur au lieu de `unique_ptr`). Corrigé en alignant l'implémentation sur l'interface.
2. **Une faute de frappe** dans un chemin d'include (`NKPatform` au lieu de `NKPlatform`) dans `NkEntry.h`.
3. **Un chemin d'include incorrect** pour le même fichier une fois la faute corrigée (le dossier `NKPlatform` n'existe pas dans cette copie de la bibliothèque ; le fichier `NkPlatformDetect.h` est directement dans `Core/`).
4. **Une erreur de structure dans le fichier de configuration Jenga** (`27_nk_window.jenga`) : les appels `configure_sandbox_app(...)` qui déclarent les projets d'exercice (dont celui-ci) étaient indentés à l'intérieur du corps de la fonction censée les recevoir, donc jamais exécutés.
5. **Une DLL manquante** : la bibliothèque lie `XInput` (support manette) via `XINPUT1_3.dll`, absente de mon système (seules des versions plus récentes comme `XInput1_4.dll` sont présentes). Contournement : génération d'une bibliothèque d'import (`libxinput.a`) via `dlltool` pointant vers `XInput1_4.dll`, avec uniquement les 3 fonctions utilisées par le code (`XInputGetState`, `XInputSetState`, `XInputGetBatteryInformation`).

Aucune de ces corrections ne touche à la logique de l'exercice lui-même — elles réparent uniquement l'environnement de compilation fourni.
