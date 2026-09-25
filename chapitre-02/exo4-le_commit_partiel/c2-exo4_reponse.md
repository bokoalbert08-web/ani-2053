 Le commit partiel

 Contexte

Un fichier `document.txt` avec trois lignes (commit initial 2d9a5a9). Deux modifications sans rapport ont été faites en même temps :
- Ligne A : "ligne A" → "lion et chat" (sujet animaux)
- Ligne C : "ligne C" → "il fait tres chaud" (sujet météo)

 Utilisation de git add -p

Commande utilisée :
```bash
git add -p document.txt
```

Git a d'abord regroupé les deux changements dans un seul "hunk" car ils étaient trop proches. Réponse `s` (split) pour les séparer en deux morceaux distincts :

(1/2) lion et chat → y (accepté)
(2/2) il fait tres chaud → n (refusé, pour l'instant)


 Deux commits séparés

```bash
git commit -m "Modification du sujet animaux (ligne A)"
git add document.txt
git commit -m "Modification du sujet meteo (ligne C)"
```

 Vérification dans l'historique

5cefb0c Modification du sujet meteo (ligne C)
d0c58ba Modification du sujet animaux (ligne A)
2d9a5a9 document.txt


Chaque commit affiche `1 insertion(+), 1 deletion(-)` : la preuve qu'il ne contient qu'une seule des deux modifications, pas les deux mélangées.

 Ce que j'ai retenu

`git add -p` permet de préparer (stage) seulement une partie des changements d'un fichier, même quand les modifications sont proches. L'option `s` (split) découpe un bloc de changement en morceaux plus petits et indépendants. Ça permet de garder chaque commit centré sur un seul sujet, même quand on a modifié plusieurs choses dans le même fichier en une seule session de travail.