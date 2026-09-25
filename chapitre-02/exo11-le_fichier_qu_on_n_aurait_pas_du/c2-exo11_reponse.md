 Le fichier qu'on n'aurait pas dû

 Manipulation effectuée (dans un dépôt de test séparé, non poussé sur GitHub)

 1. Taille de .git avant le fichier lourd

```bash
du -sh .git
```

73K


 2. Création et commit d'un fichier de 10 Mo

```bash
head -c 10M /dev/urandom > fichier_lourd.bin
git add fichier_lourd.bin
git commit -m "Ajout du fichier lourd (10 Mo)"
```

 3. Taille de .git après le commit du fichier lourd

```bash
du -sh .git
```

11M


 4. Retrait du fichier au commit suivant

```bash
git rm fichier_lourd.bin
git commit -m "Retrait du fichier lourd"
```

 5. Taille de .git après le retrait

```bash
du -sh .git
```

11M


 Conclusion

Retirer un fichier avec `git rm` et committer ce retrait **ne libère pas l'espace** occupé par ce fichier dans l'historique. La taille de `.git` est restée à 11M, alors qu'elle était de 73K avant l'ajout du fichier lourd.

Ceci s'explique par le fonctionnement même de Git : chaque commit est une photo complète de l'état du dépôt à un instant donné. Le commit qui a ajouté le fichier lourd continue d'exister dans l'historique, et avec lui, la version complète du fichier de 10 Mo reste stockée et consultable (par exemple avec `git checkout <hash-du-commit>`). Le commit de retrait ajoute seulement une nouvelle photo où le fichier n'apparaît plus, sans effacer les photos précédentes.

Pour réellement libérer cet espace, il faudrait réécrire l'historique avec des outils comme `git filter-repo` (ou l'ancien `git filter-branch`), qui reconstruisent tous les commits concernés en supprimant le fichier de chacun d'eux — une opération plus complexe et risquée, car elle change les hash de tous les commits affectés.

**Leçon pratique** : mieux vaut ne jamais committer de gros fichiers par erreur (utiliser un fichier `.gitignore`, ou vérifier la taille avant un `git add`), plutôt que d'avoir à nettoyer après coup.