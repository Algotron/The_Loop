// ***** Librairie AStar *****
// Permet de rechercher un chemin le plus direct possible entre une case depart et une case arrivee
// dans un tableau d'entiers

// Wagner Jean-Marc
// Decembre 2011
// ***************************

#ifndef A_STAR_H
#define A_STAR_H

typedef struct _case
{
  int L;  // ligne d'une case du tableau
  int C;  // colonne d'une case du tableau
} CASE;   // represente une case du tableau

int RechercheChemin(int* tab,int nbLignes,int nbColonnes,int *valeursAutorisees,int nbValeursAutorisees,CASE depart,CASE arrivee,CASE** pChemin);

// tab = tableau 2D d'entiers representant le terrain sur lequel le chemin le plus court doit etre recherche
// nbLignes,nbColonnes = dimensions du tableau 2D tab
// valeursAutorisees = vecteur de valeurs entieres sur lesquelles on peut se deplacer lors de la recherche du
//                     chemin le plus court
// nbValeursAutorisees = taille du vecteur valeursAutorisees
// depart = case depart du chemin dans le tableau tab
// arrivee = case arrivee souhaitee dans le tableau tab
// pChemin = adresse du pointeur sur un vecteur de CASE. Ce vecteur contiendra les cases successives du tableau tab pour atteindre
//           l'arrivee en partant de depart. Il contient la case d'arrivee mais pas la case depart. Sa dimension est donc egale
//           au nombre de cases a parcourir. C'est la methode RechercheChemin qui alloue le vecteur, mais il faudra le desallouer
//           manuellement apres son utilisation.
// code de retour = nombre de cases a parcourir pour atteindre l'arrivee, il s'agit aussi de la taille du vecteur pointe par *pChemin
//                = 0 si l'arrivee coincide avec la case depart, dans ce cas *pChemin est NULL
//                = -1 s'il n'y a pas de chemin possible entre la case depart et la case arrivee, dans ce cas *pChemin est NULL
//                = -1 si les cases depart et arrivee sont hors tableau, dans ce cas *pChemin est NULL

#endif
