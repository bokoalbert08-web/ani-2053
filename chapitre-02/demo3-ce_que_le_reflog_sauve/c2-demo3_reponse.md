Ce que le reflog sauve

Contexte

- Commit 1 (7404c48) : "travail.txt" contenant "travail important version 1"
- Commit 2 (01537c7) : "travail.txt" contenant en plus "travail important version 2"

 1. Destruction volontaire

Commande utilisée pour détruire le travail :
```bash
git reset --hard 7404c48
```

Cette commande a ramené `main` au commit 1, en effaçant à la fois l'historique visible du commit 2 et les changements dans le fichier.

 2. Constat de la perte

Après le reset :

- `git log --oneline` ne montre plus le commit `01537c7` : l'historique commence directement à `7404c48`.
- Le contenu de `travail.txt` ne contient plus que :
La ligne "version 2" a disparu.

 3. Récupération par le reflog

Commande utilisée pour retrouver le commit perdu :
```bash
git reflog
```

Extrait pertinent du résultat :7404c48 HEAD@{0}: reset: moving to 7404c48
01537c7 HEAD@{1}: commit: travail.txt

Ceci a montré que le commit `01537c7` n'a jamais été réellement supprimé : Git en garde la trace dans le reflog, même après un `reset --hard`.

Commande utilisée pour le récupérer :
```bash
git reset --hard 01537c7
```

 Résultat final

Après récupération, `travail.txt` contient de nouveau :
travail important version 1
travail important version 2

Et `git log --oneline` montre à nouveau le commit `01537c7` en tête de l'historique.

 Ce que j'ai retenu

`git reset --hard` semble détruire un commit, mais en réalité il ne fait que déplacer le pointeur de la branche (`main`). Le commit continue d'exister dans la base de données de Git, et reste accessible via son hash grâce au `reflog`, qui journalise tous les déplacements de `HEAD`.