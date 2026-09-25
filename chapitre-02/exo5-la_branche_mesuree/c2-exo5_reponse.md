La branche, mesurée

 Mesures

- Taille de `.git` avant la branche et les commits : **394K**
- Taille de `.git` après la création de la branche `mesure` et trois commits : **415K**
- Différence : **21K**

 Commandes utilisées

```bash
du -sh .git
git checkout -b mesure
# trois commits successifs (niveau1, niveau2, niveau3)
du -sh .git
```

 Explication du résultat

Trois commits contenant chacun une seule petite ligne de texte ne représentent que quelques centaines d'octets de contenu réel. Pourtant, le dépôt a grossi de 21K. Cela s'explique par la façon dont Git stocke chaque commit :

- Un objet **blob** : le contenu du fichier modifié
- Un objet **tree** : une photo de toute la structure des fichiers et dossiers à cet instant, pas seulement du fichier modifié
- Un objet **commit** : les métadonnées (auteur, date, message, pointeur vers le commit parent)

Pour trois commits, ce sont donc au moins neuf objets qui ont été créés. Chaque objet possède un en-tête et des métadonnées qui pèsent plus lourd que le contenu réel d'une seule ligne de texte.

 Ce que j'ai retenu

Le coût de stockage de Git n'est pas proportionnel à la taille du contenu ajouté, mais surtout aux métadonnées et à la structure que Git enregistre à chaque commit. Créer une branche et quelques commits, même minuscules, a donc un coût mesurable sur le disque — mais ce coût reste faible comparé à la sécurité et à l'historique complet que Git nous fournit en échange.