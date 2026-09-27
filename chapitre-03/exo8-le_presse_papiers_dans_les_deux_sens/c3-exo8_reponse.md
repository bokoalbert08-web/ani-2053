# Chapitre 3 - Exercice 8 : le presse-papiers, dans les deux sens

## Implémentation

NKWindow ne fournit pas d'API presse-papiers dans cet exemple (aucun
fichier consacré au presse-papiers trouvé dans `NKWindow\src`, malgré une
recherche large sur "Clipboard" et "Transfer" dans tout le dossier de
l'exemple). Le programme utilise donc directement l'API Win32 native
(`OpenClipboard`, `GetClipboardData`, `SetClipboardData`), comme déjà fait
pour `MessageBoxA` dans un exercice précédent.

Une fenêtre NKWindow s'ouvre et attend une touche :
- `T` doit lire le texte du presse-papiers (`CF_TEXT`), le mettre en
  majuscules, puis le remettre.
- `I` doit lire une image du presse-papiers (`CF_DIB`), inverser ses
  couleurs octet par octet (en laissant le canal alpha intact sur les
  images 32 bits), puis la remettre.

Le titre de la fenêtre est censé afficher le résultat de l'opération
(nombre de caractères, dimensions de l'image, ou message d'erreur si le
presse-papiers ne contient pas le type attendu).

## Compilation

Le fichier compile sans erreur (`Build Successful`, `Projects Built: 2/2`).

## Observation réelle à l'exécution : aucune réaction constatée

Un texte a été placé dans le presse-papiers (`Set-Clipboard -Value
"test"`), puis `C3Exo8.exe` a été lancé. La fenêtre s'ouvre mais Windows
l'étiquette "ne répond pas". Un clic a été fait dans la fenêtre pour lui
donner le focus, suivi d'un appui sur la touche `T`.

**Le titre de la fenêtre n'a pas changé.** Aucune différence visible avant
et après l'appui sur la touche, alors que le code est censé le modifier
dès que l'événement clavier est traité.

Ce même symptôme (fenêtre étiquetée "ne répond pas", aucune réaction
observable aux entrées clavier ou souris) a déjà été rencontré sur les
exercices précédents (`c3-exo5`, `c3-exo7`), y compris sur un exécutable
d'un exercice antérieur qui avait pourtant déjà fonctionné et été noté
lors d'une session précédente. Il s'agit donc probablement d'un blocage
au niveau de l'environnement d'exécution plutôt que d'un défaut propre à
ce fichier - plusieurs pistes ont été explorées sans confirmation
définitive (utilisation CPU non nulle du processus, possible
incompatibilité entre le compilateur MSVC utilisé pour `NKWindow.lib` et
le linker mingw utilisé pour l'exécutable final), mais aucune n'a permis
de rétablir une fenêtre qui répond.

**Par conséquent, le comportement réel du round-trip presse-papiers
(texte mis en majuscules, image aux couleurs inversées) n'a pas pu être
vérifié à l'écran ni confirmé en collant le contenu du presse-papiers
ailleurs après coup.**
