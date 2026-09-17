Résolution d'un conflit de fusion

 Contexte

- Un fichier `conflit.txt` créé avec une ligne de départ (commit A : premmier commit)
- Une branche `conflit-branche` créée à partir de A, où j'ai modifié la ligne (commit B)
- Retour sur `main`, où j'ai modifié la même ligne différemment (commit C)
- Fusion de `conflit-branche` dans `main` → conflit provoqué

 1. Lecture des marqueurs

Au moment de la fusion, Git a inséré ceci dans `conflit.txt` :
<<<<<<< HEAD
version modifiee sur main
=======
version modifiee sur la branche
>>>>>>> conflit-branche


- `<<<<<<< HEAD` : début de la version présente sur ma branche actuelle (main)
- `=======` : séparateur entre les deux versions
- `>>>>>>> conflit-branche` : fin, avec le nom de la branche apportant l'autre version

 2. Décision

J'ai choisi de garder uniquement la version de `main` ("version modifiee sur main"), et d'écarter celle de `conflit-branche`.

 3. Reconstruction

J'ai réécrit le fichier `conflit.txt` pour qu'il ne contienne plus que :
en supprimant tous les marqueurs (`<<<<<<<`, `=======`, `>>>>>>>`) ainsi que la ligne écartée.

 4. Validation

```bash
git add conflit.txt
git commit -m "Résolution du conflit : conservation de la version main"
```

 Résultat (git log --graph)
c1951a5 (HEAD -> main) Résolution du conflit : conservation de la version main
|
| * 9a671de (conflit-branche) Modification sur la branche
| c1d42f9 Modification sur main
|/
95fa104 premmier commit