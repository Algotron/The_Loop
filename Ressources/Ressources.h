#ifndef RESSOURCES_H
#define RESSOURCES_H

// Macros (direction) pour les mages et les statues
#define HAUT            400001
#define BAS             400002
#define GAUCHE          400003
#define DROITE          400004

// Macros (couleur) 
#define GRIS            400010
#define JAUNE           400011
#define ROUGE           400012
#define VERT            400013
#define VIOLET          400014

// Macros (type) pour les murs
#define PIERRE          400020
#define METAL           400021

// Macros (type) pour le piston
#define TETE            400025
#define TIGE            400026

// Macros (type) pour le bouton
#define PLUS            400030
#define MOINS           400031

// Macros (etat) pour le voyant
#define ON              400035
#define OFF             400036

int  OuvertureFenetreGraphique();
int  FermetureFenetreGraphique();

void DessineBille(int l,int c,int couleur);
void DessineDiamant(int l,int c,int couleur);
void DessineStatue(int l,int c,int dir,int couleur);
void DessineMage(int l,int c,int dir,int couleur);
void DessineMur(int l,int c,int type);
void DessinePiston(int l,int c,int type);
void DessineChiffre(int l,int c,int chiffre);
void DessinePrison(int l,int c,int couleur);
void DessineCroix(int l,int c);
void DessineBouton(int l,int c,int type);
void DessineVoyant(int l,int c,int etat);

// Pour effacer une case, utiliser la fonction EffaceCarre definie dans GrilleSDL.h

#endif
