#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "Ecran.h"
#include "GrilleSDL.h"
#include "Ressources.h"
#include "AStar.h"

// Dimensions de la grille de jeu
#define NB_LIGNES   15
#define NB_COLONNES 20

// Macros utilisees dans le tableau tab
#define VIDE     0
#define MUR      1

// Macros utilisees pour l'identite d'un thread
#define STATUE   2
#define MAGE     3
#define PISTON   4

int tab[NB_LIGNES][NB_COLONNES]
={ {0,1,1,0,1,1,0,1,1,0,1,1,0,0,0,0,0,0,1,1},
   {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
   {1,1,0,1,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0},
   {0,1,1,1,0,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1},
   {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
   {0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0},
   {0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
   {1,1,0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,1},
   {0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
   {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1},
   {0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
   {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1},
   {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1}};

void initGrille();
bool CaseReservee(CASE Case);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc,char* argv[])
{
  srand((unsigned)time(NULL));

  // Ouverture de la fenetre graphique
  Trace("(THREAD MAIN %d) Ouverture de la fenetre graphique",pthread_self()); fflush(stdout);
  if (OuvertureFenetreGraphique() < 0)
  {
    Trace("Erreur de OuvrirGrilleSDL\n");
    exit(1);
  }

  //*******************
  initGrille();


  // Exemple d'utilisations des libriaires --> code à supprimer
  struct timespec delai;
  delai.tv_sec = 0;
  delai.tv_nsec = 200000000;
  nanosleep(&delai,NULL);

  CASE position;
  position.L = 0;
  position.C = 0;
  tab[position.L][position.C] = STATUE;
  DessineStatue(position.L,position.C,BAS,0);

  bool ok = false;
  EVENT_GRILLE_SDL event;
  while(!ok)
  {
    event = ReadEvent();
    if (event.type == CLIC_GAUCHE) ok = true;
  } 
    
  tab[event.ligne][event.colonne] = ROUGE;
  DessineBille(event.ligne,event.colonne,ROUGE);

  int  valeursAutorisees[1];
  CASE *chemin = NULL;  // Futur chemin
  int  nbCases;
  valeursAutorisees[0] = VIDE;

  CASE depart,arrivee;
  depart.L = position.L;
  depart.C = position.C;
  arrivee.L = event.ligne;
  arrivee.C = event.colonne; 

  nbCases = RechercheChemin(&tab[0][0],NB_LIGNES,NB_COLONNES,valeursAutorisees,1,depart,arrivee,&chemin);
  if (nbCases >= 1)
  {
    for (int i=0 ; i<nbCases ; i++)
    {
      tab[position.L][position.C] = VIDE;
      EffaceCarre(position.L,position.C);
      position.L = chemin[i].L;
      position.C = chemin[i].C;
      tab[position.L][position.C] = STATUE;
      DessineStatue(position.L,position.C,BAS,0);
      nanosleep(&delai,NULL);
    }
    EffaceCarre(position.L,position.C);
    DessineStatue(position.L,position.C,BAS,ROUGE);
  }
  else Trace("(THREAD MAIN %d) Pas de chemin...",pthread_self()); 
  if (chemin) free(chemin);

  ok = false;
  while(!ok)
  {
    event = ReadEvent();
    if (event.type == CROIX) ok = true;
  } 

  // Fermeture de la grille de jeu (SDL)
  Trace("(THREAD MAIN %d) Fermeture de la fenetre graphique...",pthread_self()); fflush(stdout);
  FermetureFenetreGraphique();
  Trace("(THREAD MAIN %d) OK Fin",pthread_self());

  exit(0);
}

//*********************************************************************************************
void  initGrille()
{
  for (int L=0 ; L<NB_LIGNES ; L++)
    for (int C=0 ; C<NB_COLONNES ; C++)
      if (tab[L][C] == MUR) DessineMur(L,C,PIERRE);
      
  DessineMur(11,11,METAL);
  DessineMur(13,11,METAL);
  DessineBille(13,11,GRIS);
}

//*********************************************************************************************
bool CaseReservee(CASE Case)
{
  // Cases depart des statues
  for (int i=0 ; i<4 ; i++)
    if ((Case.L == 0) && (Case.C == i*3)) return true;

  // Pile
  for (int C=12 ; C<=17 ; C++)
    if ((Case.L == 0) && (Case.C == C)) return true;

  // Depart Mage 1 et positions recupération bille hors de la pile
  for (int C=11 ; C<=17 ; C++)
    if ((Case.L == 1) && (Case.C == C)) return true;

  // Cases échange des mages
  for (int C=12 ; C<=13 ; C++)
    if ((Case.L == 5) && (Case.C == C)) return true;

  // Bonus vitesse, depart Mage 2, positions lancement bille par Mage 2
  for (int C=11 ; C<=18; C++)
    if ((Case.L == 7) && (Case.C == C)) return true;

  // Score (chrono)
  for (int C=11 ; C<=13; C++)
    if ((Case.L == 9) && (Case.C == C)) return true;

  // Compteur Billes
  for (int C=11 ; C<=13; C++)
    if ((Case.L == 11) && (Case.C == C)) return true;

  // Files
  for (int L=8 ; L<=12 ; L++)
    for (int C=15 ; C<=18 ; C++)
      if ((Case.L == L) && (Case.C == C)) return true;
  
  return false;
}

