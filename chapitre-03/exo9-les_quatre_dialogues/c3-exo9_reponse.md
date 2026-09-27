# c3-exo9 — Les quatre dialogues

## Ce qui a été fait

Le fichier `c3-exo9_main.cpp` utilise les quatre dialogues natifs exposés
par `NkDialogs` :

- `OpenFileDialog` (annulable)
- `SaveFileDialog` (annulable)
- `ColorPicker` (annulable)
- `OpenMessageBox` (ne renvoie rien, donc pas d'annulation à gérer)

Pour les trois dialogues annulables, le champ `confirmed` de
`NkDialogResult` est systématiquement vérifié avant toute utilisation de
`path`/`color`. Si l'utilisateur ferme la boîte sans rien choisir,
`confirmed` vaut `false` et le code prend la branche `else`, qui affiche
un message d'annulation via `OpenMessageBox` sans jamais toucher à
`path`/`color` — donc sans risque de plantage.

Le test d'annulation en conditions réelles (fermer chaque boîte sans
rien choisir) n'a pas pu être effectué faute de build fonctionnel dans
l'environnement local au moment du rendu ; le comportement décrit
ci-dessus repose sur la lecture du code et de l'API `NkDialogs`.

