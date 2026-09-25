 Règles Git du projet

*Applicables dès demain, pour une équipe de quatre personnes.*

 1. Nommage des branches

- `main` : la branche principale, toujours stable. Personne ne travaille dessus directement.
- Une branche par tâche, nommée `type/description-courte` :
  - `feature/nom-de-la-fonctionnalite` pour une nouveauté
  - `fix/nom-du-bug` pour une correction
  - `test/nom-du-sujet` pour une expérimentation
- Pas d'espace, pas d'accent, tout en minuscules, mots séparés par des tirets.

 2. Contenu d'un commit

- Un commit = **un seul sujet**. Si on a modifié deux choses sans rapport dans la même session, on sépare avec `git add -p` (voir l'exercice sur le commit partiel).
- Message de commit clair, à l'impératif, qui dit **quoi** et pourquoi si nécessaire : "Corrige l'affichage du prix sur mobile", pas "fix" ou "modif".
- Avant tout `push`, on relit son propre `git diff` pour vérifier qu'on n'envoie que ce qu'on voulait.
- Chaque commit doit pouvoir être relu par n'importe quel membre de l'équipe en moins d'une minute.

 3. Ce qui est interdit

- **Committer des fichiers lourds** (plus de 1 Mo) sans en parler au groupe avant. Un fichier lourd committé une fois reste dans l'historique pour toujours, même après suppression (voir l'exercice sur le fichier qu'on n'aurait pas dû).
- **Utiliser `git push --force`** sur `main`, ou sur une branche que quelqu'un d'autre utilise déjà. Ça peut effacer le travail des autres sans prévenir.
- **Résoudre un conflit à la va-vite**, sans lire les marqueurs (`<<<<<<<`, `=======`, `>>>>>>>`) ou sans comprendre pourquoi il est apparu.
- **Travailler directement sur `main`.** Toujours passer par une branche, même pour une petite modification.
- **Laisser une branche `feature` vivre plus d'une semaine** sans la fusionner ou la mettre à jour depuis `main` — plus on attend, plus le risque de conflit grossit.

 4. Que faire quand quelqu'un casse la branche principale

1. **Ne pas paniquer, ne pas forcer un `push` pour "corriger vite".**
2. Prévenir immédiatement le reste du groupe (message sur le canal de discussion commun).
3. Identifier le commit fautif avec `git log --oneline`.
4. Si le commit fautif est déjà poussé et que d'autres ont pu le récupérer : utiliser `git revert <hash>` pour créer un commit correctif, jamais réécrire l'historique de `main`.
5. Si un commit important semble avoir disparu par erreur (reset accidentel) : utiliser `git reflog` pour le retrouver avant de conclure qu'il est perdu.
6. Une fois corrigé, écrire un court message dans l'historique expliquant ce qui s'est passé, pour que le groupe comprenne en relisant plus tard.

 Résumé pour les pressés

- Une branche par tâche, jamais de travail direct sur `main`.
- Un commit = un sujet, un message clair.
- Jamais de fichier lourd, jamais de `push --force` sur du code partagé.
- En cas de casse : `revert`, pas de réécriture forcée, et on prévient le groupe.