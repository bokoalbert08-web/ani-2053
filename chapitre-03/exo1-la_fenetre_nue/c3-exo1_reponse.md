# La fenêtre nue — réponse

## Compte des lignes

Le programme fait **9 lignes de code** (une 10e ligne, vide, sépare les includes du corps et n'est pas comptée) :

1. `#include "NKWindow/NKWindow.h"`
2. `#include "NKWindow/NKMain.h"`
3. `int nkmain(const NkEntryState &state) {`
4. `    NkWindowConfig cfg;`
5. `    NkWindow window(cfg);`
6. `    if (!window.IsOpen()) return -1;`
7. `    while (window.IsOpen()) {}`
8. `    return 0;`
9. `}`

## Où chaque ligne se retrouve dans le chapitre

**Ligne 1 — `#include "NKWindow/NKWindow.h"`**
Donne accès à `NkWindow` et `NkWindowConfig`, les deux types utilisés dans « Le plus petit programme » du chapitre.

**Ligne 2 — `#include "NKWindow/NKMain.h"`**
Nécessaire car « vous n'écrivez pas de main, vous écrivez `nkmain` » : ce header fournit le point d'entrée natif de chaque plateforme (`WinMain`, `android_main`, `UIApplicationMain`). L'omettre est justement « le piège du chapitre » : une erreur de lien `undefined reference to WinMain`.

**Ligne 3 — `int nkmain(const NkEntryState &state) {`**
Signature imposée par le module : le point d'entrée n'est pas `main` mais `nkmain`, comme rappelé dans « Trois choses à remarquer ».

**Ligne 4 — `NkWindowConfig cfg;`**
Le chapitre précise que « la configuration se donne au constructeur » : on doit donc construire l'objet de configuration avant de créer la fenêtre. Aucun champ n'est fixé ici puisque le titre, la largeur et la hauteur ne sont pas indispensables à l'ouverture — le programme reste le plus petit possible.

**Ligne 5 — `NkWindow window(cfg);`**
« La fenêtre est créée à ce moment-là » : la construction de `NkWindow` avec la config est l'instant précis de la création.

**Ligne 6 — `if (!window.IsOpen()) return -1;`**
« On vérifie `IsOpen`. Une création peut échouer — pilote absent, serveur graphique inaccessible, permissions. Un programme qui continue après cela travaille dans le vide. » On sort donc immédiatement si l'ouverture a échoué.

**Ligne 7 — `while (window.IsOpen()) {}`**
Correspond à « garder la fenêtre ouverte » : la boucle du modèle du chapitre (`while (window.IsOpen()) { /* les evenements arrivent ici */ }`) est reprise ici sans corps, puisque cet exercice ne demande pas encore de traiter les événements (ce sera l'objet du chapitre 4).

**Ligne 8 — `return 0;`**
Termine proprement le programme, en sortant de `nkmain` avec un code de succès une fois la boucle interrompue (fenêtre fermée par l'utilisateur).

**Ligne 9 — `}`**
Ferme le corps de `nkmain`.

## Remarque

Le programme ne fixe ni titre, ni largeur, ni hauteur dans `NkWindowConfig`, contrairement à l'exemple du cours : l'énoncé demande *le plus petit* programme, et ces champs ne sont pas nécessaires pour qu'une fenêtre s'ouvre, reste ouverte, et se termine proprement.
