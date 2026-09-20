 Six manières de défaire

 1. Une modification non voulue

Provoqué avec :
```bash
echo "modification non voulue !" >> situation1.txt
```

Défait avec :
```bash
git restore situation1.txt
```

`git restore` ramène un fichier modifié (non préparé) à l'état du dernier commit.

 2. Un add de trop

Provoqué avec :
```bash
git add situation2.txt
```

Défait avec :
```bash
git restore --staged situation2.txt
```

Retire le fichier de la zone de préparation, sans effacer les modifications.

 3. Un commit de trop

Provoqué avec un commit contenant `erreur.txt`.

Défait avec :
```bash
git reset --soft HEAD~1
```

Annule le dernier commit, mais garde les changements dans la zone de préparation. Rien n'est perdu, contrairement à `--hard`.

 4. Un commit poussé qu'il faut annuler

Provoqué en poussant un commit vers GitHub.

Défait avec :
```bash
git revert <hash-du-commit>
```

Contrairement aux méthodes précédentes, `revert` ne réécrit pas l'historique : il crée un nouveau commit qui applique l'inverse exact du commit ciblé. C'est la méthode sûre pour un commit déjà partagé, que d'autres ont pu récupérer.

 5. Un travail en cours à mettre de côté

Provoqué avec un fichier non suivi, non terminé.

Mis de côté avec :
```bash
git stash -u
```

Récupéré avec :
```bash
git stash pop
```

`git stash` range temporairement les changements en cours (l'option `-u` inclut aussi les fichiers non suivis), pour revenir à un dossier de travail propre sans rien commit. `pop` réapplique le dernier stash et le retire de la liste.

 6. Un commit perdu à retrouver

Provoqué avec :
```bash
git reset --hard HEAD~1
```

Retrouvé avec :
```bash
git reflog
git reset --hard <hash-retrouve>
```

Le commit n'est jamais vraiment supprimé : `git reflog` trace tous les déplacements de HEAD, et permet de revenir à un commit qu'on croyait perdu.

 Tableau récapitulatif

| Situation | Commande de correction |
|---|---|
| Modification non voulue | `git restore <fichier>` |
| Add de trop | `git restore --staged <fichier>` |
| Commit de trop (local) | `git reset --soft HEAD~1` |
| Commit poussé à annuler | `git revert <hash>` |
| Travail en cours à ranger | `git stash` / `git stash pop` |
| Commit perdu à retrouver | `git reflog` + `git reset --hard <hash>` |

 Ce que j'ai retenu

Il existe plusieurs façons d'annuler un changement dans Git, et le choix dépend d'un facteur clé : **est-ce que ce changement a déjà été partagé (poussé) avec d'autres ?**
- Si non : on peut réécrire l'historique librement (`restore`, `reset`).
- Si oui : il faut ajouter un nouveau commit correctif (`revert`), pour ne jamais perturber le travail des autres avec un historique qui change sous leurs pieds.