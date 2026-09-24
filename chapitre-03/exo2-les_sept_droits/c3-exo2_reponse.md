# Chapitre 3 - Exercice 2 : les sept droits

## Remarque préalable

Le moteur NKWindow fourni dans ce dépôt d'exemple (`27_nk_window`) ne compile
pas en l'état : la construction du projet `NKWindow` échoue avec 17 erreurs,
dont plusieurs viennent d'une incohérence de type déjà présente dans le code
du moteur, indépendamment de `c3-exo2_main.cpp`. Par exemple, dans
`NkEventSystem.cpp` :

```
error: no viable conversion from 'NkEvent *' to 'NkEvent'
```

et dans `NkWin32EventImpl.h` :

```
error: non-virtual member function marked 'override' hides virtual member function
```

Le moteur mélange `NkEvent*` et `NkEvent&` selon les fichiers (interface vs
implémentation Win32), ce qui casse la compilation avant même d'atteindre le
code de cet exercice.

Les observations ci-dessous sont donc raisonnées à partir de la structure de
`NkWindowConfig` (fichier `NkWindowConfig.h`) et du comportement habituel de
ces propriétés dans une application de bureau, plutôt que constatées à
l'exécution.

## Les sept fenêtres

### 1. `resizable = false`
L'utilisateur ne peut plus agrandir ni rétrécir la fenêtre en tirant ses
bords ou son coin.

### 2. `movable = false`
L'utilisateur ne peut plus déplacer la fenêtre en cliquant-glissant sa barre
de titre ; elle reste bloquée à sa position initiale.

### 3. `closable = false`
L'utilisateur ne peut plus fermer la fenêtre en cliquant sur la croix rouge
de la barre de titre.

### 4. `minimizable = false`
L'utilisateur ne peut plus réduire la fenêtre (bouton "_" de la barre de
titre) pour l'envoyer dans la barre des tâches.

### 5. `maximizable = false`
L'utilisateur ne peut plus agrandir la fenêtre pour qu'elle occupe tout
l'écran (bouton "▢" de la barre de titre, ou double-clic sur la barre de
titre).

### 6. `canFullscreen = false`
L'utilisateur ne peut plus passer la fenêtre en plein écran sans bordure.
À la différence de `maximizable`, le plein écran fait disparaître
complètement la barre de titre et les bordures ; `maximizable`, lui, agrandit
la fenêtre en gardant ce cadre visible.

### 7. `frame = false`
La fenêtre n'a plus de cadre du tout : ni barre de titre, ni bordures, ni
boutons réduire/agrandir/fermer. Par conséquent, aucune de ces actions n'est
possible à la souris, même si les droits correspondants (`closable`,
`minimizable`, etc.) restent activés dans la configuration : sans cadre, il
n'y a plus rien à cliquer pour les déclencher.
