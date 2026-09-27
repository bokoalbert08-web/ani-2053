# Chapitre 3 - Exercice 5 : le titre qui informe

## Implémentation

Le titre de la fenêtre est construit à partir de trois éléments, dans l'ordre
demandé par l'énoncé :
1. le nom du document simulé (`sans_titre.txt`) ;
2. un astérisque `*` si le document est "modifié" ;
3. la taille courante de la fenêtre (`largeur x hauteur`).

Comme il n'y a pas de vrai document dans cet exercice, l'état "modifié" est
simulé au clavier : n'importe quelle touche marque le document comme modifié,
la touche `S` simule une sauvegarde et le remet à "propre".

Le titre n'est recalculé et réappliqué (`window.SetTitle(...)`) que dans deux
cas précis :
- à la réception d'un `NkWindowResizeEvent` (la taille a changé) ;
- quand l'état "modifié" bascule réellement (une touche vient de le faire
  changer de valeur, pas à chaque événement clavier reçu).

Il n'est jamais recalculé dans la boucle `while (window.IsOpen())` elle-même,
conformément à la consigne "pas à chaque image".

## Compilation

Le fichier compile sans erreur avec le toolchain MSVC
(`--toolchain msvc`) : `Build Successful`, `Projects Built: 2/2`, `Status:
SUCCESS`. L'exécutable `C3Exo5.exe` est bien généré dans
`Build\Bin\Debug-Windows\C3Exo5\`.

## Blocage rencontré : comportement à l'exécution non observé

**Ce point n'a pas pu être vérifié à l'écran**, et je le signale explicitement
plutôt que de décrire un comportement supposé.

En lançant `C3Exo5.exe`, la fenêtre s'ouvre mais reste figée ("ne répond
pas"), sans réaction visible au clavier ni au redimensionnement. Pour vérifier
si ce blocage était spécifique à ce fichier, `C3Exo4.exe` (déjà compilé,
issu d'un exercice précédent) a été testé en comparaison : il se fige
également - mais c'est attendu de sa part, car son code contient une boucle
`while (window.IsOpen()) {}` totalement vide, sans lecture d'événements, donc
sans pompe de messages Windows.

**Par conséquent, je ne peux pas encore confirmer par l'observation :**
- que le titre initial s'affiche bien comme `sans_titre.txt - 1280 x 720` ;
- que le titre se met à jour en direct pendant un redimensionnement ;
- que l'astérisque apparaît/disparaît correctement avec le clavier ;
- que les deux mises à jour (taille et astérisque) ne s'écrasent pas l'une
  l'autre quand elles doivent coexister.

Le code est écrit pour produire ce comportement, mais faute d'observation
réelle et complète à l'écran, certains de ces points restent à vérifier -
conformément à la méthode de correction déjà appliquée sur cet exercice
(`la plus petite taille que le système ACCEPTE` doit être une mesure, pas
une valeur déduite).

## Observation réelle faite malgré le blocage

Un point a pu être lu à l'écran malgré tout : la taille affichée dans le
titre au lancement est **1281 x 721**, alors que le code demande une
fenêtre de 1280 x 720 (`cfg.width = 1280; cfg.height = 720;`). Il y a donc
un écart de +1 pixel en largeur comme en hauteur entre la taille demandée
à la configuration et la taille effectivement rapportée par
`window.GetSize()`. Cette valeur n'a pas été déduite du code : c'est ce qui
est réellement affiché dans la barre de titre.

