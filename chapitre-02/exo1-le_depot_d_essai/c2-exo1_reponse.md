 Le dépôt d'essai

 Contexte

Un dépôt vide, avec trois fichiers ajoutés dans trois commits séparés :
- fichier1.txt (commit 120975c)
- fichier2.txt (commit 3b9ba9e)
- fichier3.txt (commit 16715f3)

1. Historique en une ligne par commit

Commande utilisée :
```bash
git log --oneline
```

Résultat :
16715f3 (HEAD -> main) fichier3.txt
3b9ba9e fichier2.txt
120975c fichier1.txt

2. Affichage du graphe

Commande utilisée :
```bash
git log --oneline --graph
```

Résultat :
.16715f3 (HEAD -> main) fichier3.txt
.3b9ba9e fichier2.txt
.120975c fichier1.txt


## Ce que j'ai observé

Comme il n'y a ni branche ni fusion dans ce dépôt, le graphe est une simple ligne droite : chaque commit a un seul parent, et les `*` s'alignent verticalement sans jamais se séparer (`|\`) ni se rejoindre (`|/`).