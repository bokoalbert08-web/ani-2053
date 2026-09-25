
 Le conflit qui n'en est pas un

 Contexte

Deux clones du même dépôt (`clone1` et `clone2`), simulant deux personnes travaillant en parallèle sur le même fichier `document.txt` (10 lignes), mais à des endroits éloignés :
- `clone2` a modifié la ligne 1 (le début)
- `clone1` a modifié la ligne 10 (la fin)

 1. Push réussi depuis clone1

45c07a0..73aa3fa main -> main


 2. Refus du push depuis clone2

```bash
git push
```

! [rejected] main -> main (fetch first)
error: failed to push some refs to 'https://github.com/bokoalbert08-web/ani-2053.git'
hint: Updates were rejected because the remote contains work that you do not
hint: have locally...


 3. git pull : fusion automatique, sans conflit

```bash
git pull
```

Résultat : Git a ouvert directement un éditeur pour le message du commit de fusion, **sans afficher aucun message CONFLICT**. La fusion s'est faite automatiquement.

Contenu final de `document.txt` après fusion :

Debut modifie par clone2
ligne 2
ligne 3
ligne 4
ligne 5
ligne 6
ligne 7
ligne 8
ligne 9
fin modifiee par clone1


Les deux modifications sont présentes, combinées automatiquement.

 4. Vérification avec git log --graph
47be455 (HEAD -> main) Merge branch 'main' of ...
|
| * 73aa3fa document.txt
| db768bd document.txt
|/
45c07a0 document.txt

Un vrai commit de fusion (deux parents) a bien été créé, mais sans qu'aucune résolution manuelle n'ait été nécessaire.

 5. Push final réussi

73aa3fa..47be455 main -> main


 Ce que j'ai retenu

Git ne déclenche un conflit que lorsque deux modifications touchent la **même zone** d'un fichier (ou des zones trop proches, sans assez de lignes identiques entre elles pour que Git les distingue clairement). Quand les modifications sont **éloignées**, Git est capable de les assembler automatiquement dans un seul commit de fusion, sans rien demander à l'utilisateur — même si un vrai commit de fusion (avec deux parents) est tout de même créé dans l'historique.