 Les trois endroits

 Contexte

Un fichier `notes.txt` déjà suivi par Git (commit initial), puis modifié pour observer les trois états de `git status`.

 1. Après modification (avant tout add)

Commande : `echo "ligne ajoutee" >> notes.txt` puis `git status`
On branch main
Changes not staged for commit:
modified: notes.txt

no changes added to commit (use "git add" and/or "git commit -a")


 2. Après git add

Commande : `git add notes.txt` puis `git status`
On branch main
nothing to commit, working tree clean


Ce qui change entre les trois sorties

- **Étape 1 → 2** : le fichier passe de "Changes not staged for commit" à "Changes to be committed". Il quitte le répertoire de travail (working directory) pour entrer dans la zone de préparation (staging area) — c'est l'effet de `git add`.
- **Étape 2 → 3** : le fichier disparaît complètement de `git status`, qui affiche "nothing to commit, working tree clean". Il quitte la zone de préparation pour être enregistré définitivement dans l'historique — c'est l'effet de `git commit`.
- Le message d'aide entre parenthèses change aussi à chaque étape, guidant vers l'action suivante logique (`git add` pour préparer, `git restore --staged` pour annuler une préparation).

Ce que j'ai retenu

Un fichier modifié dans Git passe par trois endroits successifs :
1. **Répertoire de travail** (working directory) : modifications non suivies pour le prochain commit
2. **Zone de préparation** (staging area) : modifications prêtes, en attente de commit
3. **Historique** (repository) : modifications enregistrées définitivement

`git status` permet de savoir à tout moment dans lequel de ces trois endroits se trouve chaque fichier.