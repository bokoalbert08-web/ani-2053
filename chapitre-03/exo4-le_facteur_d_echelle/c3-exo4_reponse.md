# Exercice — Le facteur d'échelle

## Objectif

Afficher côte à côte trois valeurs : la taille rendue par la fenêtre, la taille rendue par la cible de rendu, et le facteur d'échelle entre les deux.

## Code

```cpp
#include "NKWindow/NKWindow.h"
#include "NKWindow/NKMain.h"
#include <iostream>

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    NkWindowConfig cfg;
    cfg.title  = "Facteur d'echelle";
    cfg.width  = 1280;
    cfg.height = 720;

    NkWindow window(cfg);
    if (!window.IsOpen()) {
        logger.Error("[app] creation fenetre echouee");
        return -1;
    }

    math::NkVec2u windowSize = window.GetSize();
    NkSurfaceDesc surface    = window.GetSurfaceDesc();
    float32 scale            = window.GetDpiScale();

    std::cout << "Taille fenetre     : " << windowSize.x << " x " << windowSize.y << std::endl;
    std::cout << "Taille cible rendu : " << surface.width << " x " << surface.height << std::endl;
    std::cout << "Facteur d'echelle  : " << scale << std::endl;

    while (window.IsOpen()) {}
    return 0;
}
```

## Méthodes utilisées et leur origine

Ces méthodes ont été retrouvées directement dans le header `NkWindow.h` de la bibliothèque (classe `nkentseu::NkWindow`), le chapitre ne les détaillant pas explicitement :

| Méthode | Rôle |
|---|---|
| `window.GetSize()` | Renvoie la taille de la **fenêtre** (`math::NkVec2u`, champs `.x` / `.y`) |
| `window.GetSurfaceDesc()` | Renvoie le descripteur de la **cible de rendu** (`NkSurfaceDesc`, champs `.width` / `.height`) |
| `window.GetDpiScale()` | Renvoie le **facteur d'échelle** de l'écran (`float32`) |

Un `using namespace nkentseu;` a été ajouté après les includes : tous les types de la bibliothèque (`NkWindow`, `NkWindowConfig`, `NkSurfaceDesc`, `NkEntryState`, `float32`, `math::NkVec2u`) sont déclarés dans ce namespace, sans alias global vers la racine.

## Remarque sur la compilation locale

Je n'ai pas pu obtenir de compilation réussie sur ma machine, malgré plusieurs tentatives :

- En liant les headers de NKWindow directement avec `g++` (sans passer par Jenga), l'édition de liens échoue systématiquement (`ld returned 1` ou `5`), car aucune implémentation compilée de la bibliothèque n'est disponible de cette façon — seuls des en-têtes étaient inclus.
- En essayant de builder via l'outil `Jenga` avec la copie de NKWindow fournie dans l'exemple `27_nk_window`, la compilation de la bibliothèque elle-même échoue avec 17 erreurs, dans son propre code (`NkWin32WindowImpl.cpp`, `NkWin32EventImpl.h`) : plusieurs méthodes (`Front()`, `PushEvent()`, `DispatchEvent()`) sont déclarées `override` avec une signature différente de celle de la classe de base `IEventImpl` (type de retour ou type de paramètre incompatible) — donc indépendant de mon code.
- En remplaçant cette copie par celle du dépôt `Nkentseu` (`Kernel/Runtime/NKWindow`), de nouvelles erreurs de chemins d'inclusion manquants (`NKMath`, `NKPlatform`) sont apparues, la configuration `.jenga` de l'exemple ne prévoyant pas ces dépendances.

Le code ci-dessus est donc rédigé à partir des signatures réelles trouvées dans les en-têtes de la bibliothèque (confirmées via recherche dans le code source), mais n'a pas pu être validé par une compilation complète de bout en bout de mon côté.
