 Le conflit provoqué

 Contexte

Deux clones du même dépôt (`clone1` et `clone2`), simulant deux personnes travaillant en parallèle. Les deux ont modifié la même ligne du fichier `partage.txt`, sans savoir que l'autre le faisait aussi.

 1. Push réussi depuis clone1

```bash
git push
```

03f3b4a..3951e13 main -> main


 2. Refus du push depuis clone2

```bash
git push
```

! [rejected] main -> main (fetch first)
error: failed to push some refs to 'https://github.com/bokoalbert08-web/ani-2053.git'
hint: Updates were rejected because the remote contains work that you do not
hint: have locally. This is usually caused by another repository pushing to
hint: the same ref. If you want to integrate the remote changes, use
hint: 'git pull' before pushing again.


 3. Conflit lors du git pull

```bash
git pull
```

Auto-merging chapitre-02/exo6-le_conflit_provoque/partage.txt
CONFLICT (content): Merge conflict in chapitre-02/exo6-le_conflit_provoque/partage.txt
Automatic merge failed; fix conflicts and then commit the result.


 4. Résolution du conflit

Marqueurs trouvés dans `partage.txt` :
<<<<<<< HEAD
version modifiee depuis clone2

version modifiee depuis clone1

3951e13


Décision : conserver les deux versions, l'une après l'autre.

```bash
git add partage.txt
git commit -m "Résolution du conflit : conservation des deux versions"
git push
```

 5. Push final réussi

3951e13..48a4ecd main -> main


 Ce que j'ai retenu

Un conflit peut venir de deux sources différentes :
- **Local** : deux branches fusionnées sur la même machine (vu dans les exercices précédents)
- **Distant** : deux copies de travail séparées (deux clones, ou deux personnes), qui modifient la même chose sans se synchroniser

GitHub refuse toujours un push qui ferait perdre du travail déjà présent sur le serveur (`rejected... fetch first`). La solution est de `pull` d'abord, ce qui peut déclencher un conflit local à résoudre, exactement comme entre deux branches.