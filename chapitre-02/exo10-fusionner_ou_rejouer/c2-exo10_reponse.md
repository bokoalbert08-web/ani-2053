
 Fusionner ou rejouer

 Scénario de départ (identique pour les deux tests)

Un commit initial, une branche `feature` avec deux commits, et un commit parallèle sur `master` — créant une divergence volontaire.
ad057c3 (master) Master : travail en parallele
| * 2a91b30 (feature) Feature : deuxieme ajout
| * b650801 Feature : premier ajout
|/
7b4b62c Commit initial

 Intégration par MERGE

Commande : `git merge feature -m "Fusion de feature dans master"`

Un conflit est apparu (même fichier modifié des deux côtés), résolu manuellement en gardant les deux contributions.

Graphe obtenu :
8ce5cf9 (master) Fusion de feature dans master
|
| * 2a91b30 (feature) Feature : deuxieme ajout
| * b650801 Feature : premier ajout
| ad057c3 Master : travail en parallele
|/
7b4b62c Commit initial

 Intégration par REBASE

Commandes : `git rebase master` (depuis feature), puis `git merge feature` (fast-forward)

Chaque commit de `feature` a été rejoué individuellement par-dessus le dernier commit de `master`. Un conflit est apparu à chaque commit rejoué (deux conflits au total), résolus un par un avec `git add` + `git rebase --continue`.

Graphe obtenu :
0aa7383 (master, feature) Feature : deuxieme ajout
98cef5f Feature : premier ajout
a8ec60d Master : travail en parallele
7b4b62c Commit initial

 Comparaison

| | Merge | Rebase |
|---|---|---|
| Forme du graphe | En losange, avec divergence et réunion visibles | Ligne parfaitement droite |
| Nombre de conflits à résoudre | 1 (les deux branches en une fois) | 2 (un par commit rejoué) |
| Commit de fusion créé | Oui, avec deux parents | Non, historique linéaire |
| Historique d'origine | Préservé tel quel | Réécrit (nouveaux hash pour les commits de feature) |

 Ce que je préfère lire, et pourquoi

Je préfère lire le graphe obtenu par **rebase**. La ligne droite est plus simple à suivre dans l'ordre chronologique : chaque commit s'enchaîne logiquement au précédent, sans avoir à interpréter une divergence puis une réunion. Le merge est plus honnête sur ce qui s'est réellement passé (deux personnes ont travaillé en parallèle), mais pour une lecture rapide de "qu'est-ce qui a changé, dans quel ordre", le rebase est plus direct.

En contrepartie, le rebase a demandé plus d'efforts de résolution de conflit (un par commit, au lieu d'un seul global), et il réécrit l'historique — ce qui le rend risqué sur une branche déjà partagée avec d'autres personnes (contrairement au merge, toujours sûr dans ce cas).