# Chapitre 3 - Exercice 3 : les bornes

## Les deux fenêtres testées

### 1. Borne personnalisée : `minWidth = 400`, `minHeight = 300`
En tirant le coin de la fenêtre vers le centre pour la réduire, le
système devrait bloquer la réduction dès que la taille atteint
400 x 300 : impossible de descendre en dessous, quelle que soit la
vitesse ou la direction du glissement. Le titre (mis à jour à chaque
`NkWindowResizeEvent`) devrait donc afficher au minimum "400 x 300" et
ne jamais descendre plus bas.

### 2. Borne par défaut du moteur (`minWidth`/`minHeight` non modifiés)
D'après les valeurs par défaut de `NkWindowConfig.h`, la fenêtre devrait
se bloquer à 160 x 90 : une taille bien plus petite que le cas 1, qui
laisse à peine la place pour une barre de titre et son contenu. C'est
la taille plancher que le système impose de lui-même dès qu'aucune
borne personnalisée n'est fournie.

### Comparaison
La différence entre les deux cas montre que `minWidth`/`minHeight` ne
sont pas des bornes optionnelles activées seulement si on les définit :
elles existent toujours (160 x 90 par défaut), et les fixer explicitement
ne fait que remplacer cette valeur plancher par une autre, plus grande,
adaptée au contenu réel de la fenêtre.
