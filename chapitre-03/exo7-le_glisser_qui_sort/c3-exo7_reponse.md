# Chapitre 3 - Exercice 7 : le glisser qui sort

## Implémentation

Deux fenêtres sont ouvertes successivement, comme pour l'exercice des
bornes : une sans capture de souris, une avec `window.CaptureMouse(true)`
activé au moment du clic gauche (et désactivé au relâchement).

Pendant un glisser (entre l'appui et le relâchement du bouton gauche), le
titre de la fenêtre affiche la position du curseur sous deux formes :
- relative à la fenêtre (`GetX()`/`GetY()`) ;
- relative à l'écran entier (`GetScreenX()`/`GetScreenY()`).

Ces deux valeurs sont censées diverger dès que le curseur sort de la
fenêtre : la position "fenêtre" peut alors devenir négative ou dépasser sa
taille, alors que la position "écran" continue de suivre le curseur
normalement.

## Compilation

Le fichier compile sans erreur (`Build Successful`, `Projects Built: 2/2`).
Un premier essai avec `--toolchain msvc` explicite a réussi ; un second
essai sans préciser de toolchain (commande `rebuild`, qui n'accepte pas
l'option `--toolchain`) a résolu automatiquement vers `clang-mingw` et a
également réussi - les deux compilent sans erreur.

## Conclusion (déduite du code et de la documentation de l'API, non vérifiée à l'écran)

Cette conclusion n'a pas été confirmée par observation directe du
comportement à l'exécution - elle est déduite du code écrit et de ce que
les méthodes de l'API sont censées faire d'après leur documentation.

**Sans capture (`CaptureMouse` non appelé) :** dès que le curseur quitte
les limites de la fenêtre pendant le glisser, celle-ci devrait cesser de
recevoir les événements `NkMouseMoveEvent` - le système d'exploitation
route alors les mouvements de souris vers ce qui se trouve sous le
curseur (le bureau, une autre fenêtre), pas vers notre fenêtre qui n'a
plus le focus souris. Le titre devrait donc s'arrêter de se mettre à jour
au moment précis où le curseur sort, même si le bouton gauche reste
enfoncé.

**Avec capture (`window.CaptureMouse(true)` appelé au clic) :** la
capture souris est justement le mécanisme qui force tous les événements
souris à continuer d'être envoyés à la fenêtre d'origine, même quand le
curseur physique sort de ses limites. Le titre devrait donc continuer de
se mettre à jour pendant tout le glisser, avec des coordonnées "fenêtre"
qui deviennent négatives ou supérieures à la taille de la fenêtre une
fois le curseur sorti, alors que les coordonnées "écran" restent
cohérentes avec la position réelle du curseur.

**Différence du point de vue de l'utilisateur :** sans capture, un
glisser qui sort de la fenêtre "perd" la cible au moment où le curseur
franchit le bord - l'application cesse de suivre le geste. Avec capture,
l'application continue de suivre le geste jusqu'au relâchement du bouton,
même très loin en dehors de sa propre fenêtre, ce qui est le comportement
attendu pour des interactions comme un glisser de fichier, un
redimensionnement personnalisé ou un slider qu'on veut pouvoir tirer sans
que le curseur reste strictement confiné aux pixels du contrôle.
