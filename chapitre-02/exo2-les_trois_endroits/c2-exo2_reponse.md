 Les trois endroits

 Contexte

Un fichier `notes.txt` déjà suivi par Git, modifié pour observer les trois états de `git status`.

 1. Après modification (avant tout add)

Commande : `echo "nouvelle ligne pour refaire l'exercice" >> notes.txt` puis `git status`

On branch main
Your branch is up to date with 'origin/main'.

Changes not staged for commit:
(use "git add <file>..." to update what will be committed)
(use "git restore <file>..." to discard changes in working directory)
modified: notes.txt

no changes added to commit (use "git add" and/or "git commit -a")


 2. Après git add

Commande : `git add notes.txt` puis `git status`

On branch main
Your branch is up to date with 'origin/main'.

Changes to be committed:
(use "git restore --staged <file>..." to unstage)
modified: notes.txt


 3. Après git commit

Commande : `git commit -m "Ajout d'une nouvelle ligne pour refaire l'exercice"` puis `git status`

On branch main
Your branch is ahead of 'origin/main' by 1 commit.
(use "git push" to publish your local commits)

nothing to commit, working tree clean


 Ce qui change entre les trois sorties

- **Étape 1 → 2** : le fichier passe de « Changes not staged for commit » à « Changes to be committed ». Il quitte le répertoire de travail pour entrer dans la zone de préparation — c'est l'effet de `git add`.
- **Étape 2 → 3** : le fichier disparaît complètement de `git status`, qui affiche « nothing to commit, working tree clean ». Il quitte la zone de préparation pour être enregistré définitivement dans l'historique — c'est l'effet de `git commit`.

 Ce que j'ai retenu

Un fichier modifié dans Git passe par trois endroits successifs :

1. **Répertoire de travail** : modifications non suivies pour le prochain commit
2. **Zone de préparation** : modifications prêtes, en attente de commit
3. **Historique** : modifications enregistrées définitivement

`git status` permet de savoir à tout moment dans lequel de ces trois endroits se trouve chaque fichier.