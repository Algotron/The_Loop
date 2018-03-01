#include "AStar.h"
#include <stdlib.h>

///// Declaration des structures et fonctions opaques /////////////////////////////////////////////
typedef struct noeud
{
  int L,C;
  int wG; // distance parcourue depuis le depart pour arriver a ce noeud
  int wH; // distance entre ce noeud et le noeud d'arrivee
  int wF; // somme des deux poids precedents
  struct noeud *parent;
  struct noeud *suivant;
} NOEUD;

int     distance(int L1,int C1,int L2,int c2);
void    insereListe(NOEUD **ppTete,NOEUD *pNoeud);
NOEUD*  RecupereFmin(NOEUD **ppListeOuverte);
NOEUD*  estPresent(NOEUD* pTete,int L,int C);
char    valeurAutorisee(int valeur,int *valeursAutorisees,int nbValeursAutorisees);

///// Definition des fonctions ////////////////////////////////////////////////////////////////////
int RechercheChemin(int* tab,int nbLignes,int nbColonnes,int *valeursAutorisees,int nbValeursAutorisees,CASE depart,CASE arrivee,CASE **pChemin)
{
  char fini=0,trouve=0;
  int backup,i,L,C,newWG,nbCases;
  NOEUD* pListeOuverte;
  NOEUD* pListeFermee;
  NOEUD* courant,*temp;
  CASE*  cheminI;

  // Verification des positions de depart et d'arrivee
  if ((depart.L<0) || (depart.L>=nbLignes) || (arrivee.C<0) || (arrivee.C>=nbColonnes))
  {
    *pChemin = NULL;
    return -1;
  }

  // Verification des valeurs autorisees
  if ((valeursAutorisees == NULL) || nbValeursAutorisees <= 0)
  {
    *pChemin = NULL;
    return -2;
  }

  // Est-on deja arrive ?
  if ((depart.L==arrivee.L) && (depart.C==arrivee.C))
  {
    *pChemin = NULL;
    return 0;
  }

  // On memorise et libere la case d'arrivee
  backup = *(tab+arrivee.L*nbColonnes+arrivee.C);
  *(tab+arrivee.L*nbColonnes+arrivee.C) = valeursAutorisees[0];

  // Initialisation des listes et du premier noeud
  pListeOuverte = NULL;
  pListeFermee  = NULL;

  NOEUD *p = (NOEUD*)malloc(sizeof(NOEUD));
  p->L = depart.L;
  p->C = depart.C;
  p->parent = NULL;
  p->wG = 0;
  p->wH = distance(depart.L,depart.C,arrivee.L,arrivee.C);
  p->wF = p->wG + p->wH;
  p->suivant = NULL;
  insereListe(&pListeOuverte,p);

  while (!fini)
  {
    // On retire le noeud qui a le wF minimum de la liste ouverte
    courant = RecupereFmin(&pListeOuverte);
    if (courant == NULL) 
    {
      fini = 1;
      trouve = 0;
      continue;
    }

    // On l'insere dans la liste fermée
    insereListe(&pListeFermee,courant);
    if ((courant->L==arrivee.L) && (courant->C==arrivee.C))
    {
      fini = 1;
      trouve = 1;
      continue;
    }

    // on analyse les noeuds adjacents
    for (L=(courant->L-1) ; L<=(courant->L+1) ; L++)
      for (C=(courant->C-1) ; C<=(courant->C+1) ; C++)
      {
        if ((L==(courant->L-1)) && (C==(courant->C-1))) continue; 
        if ((L==(courant->L+1)) && (C==(courant->C+1))) continue; 
        if ((L==(courant->L-1)) && (C==(courant->C+1))) continue; 
        if ((L==(courant->L+1)) && (C==(courant->C-1))) continue; 
        if ((L==(courant->L)) && (C==(courant->C))) continue;                   // noeud courant
        if ((L<0) || (L>=nbLignes) || (C<0) || (C>=nbColonnes)) continue;       // hors tableau
        if (!valeurAutorisee(*(tab+L*nbColonnes+C),valeursAutorisees,nbValeursAutorisees)) continue; // obstacle

        // le noeud etudie est-il dans la liste fermee ?
        temp = estPresent(pListeFermee,L,C);
        if (temp != NULL) continue;

        // le noeud etudie est-il dans la liste ouverte ?
        temp = estPresent(pListeOuverte,L,C);
        if (temp != NULL)
        {
          // Oui, on recalcule son wG
          newWG = courant->wG + 1;
          if (newWG < temp->wG)
          {
            // le nouveau est mieux, on le met a jour
            temp->wG = newWG;
            temp->wF = temp->wG + temp->wH;
            temp->parent = courant;
          }
        }
        else
        {
          // Non, on l'ajoute a la liste ouverte
          temp = (NOEUD*)malloc(sizeof(NOEUD));
          temp->L = L;
          temp->C = C;
          temp->parent = courant;
          temp->wG = courant->wG+1;
          temp->wH = distance(L,C,arrivee.L,arrivee.C);
          temp->wF = temp->wG + temp->wH;
          temp->suivant = NULL;
          insereListe(&pListeOuverte,temp);
        }
      }    
  }

  if (trouve)
  {
    // Comptage du nombre de cases a parcourir
    nbCases = -1;
    p = courant;
    while(p != NULL)
    {
      nbCases++;
      p = p->parent;
    }

    cheminI = (CASE*)malloc(nbCases*sizeof(CASE));
    p = courant;
    i = 0;
    while((p->L != depart.L) || (p->C != depart.C))
    {
      cheminI[i].L = p->L;
      cheminI[i].C = p->C;
      i++;
      p = p->parent;
    }
        
    // Allocation et remplissage du chemin solution
    *pChemin = (CASE*)malloc(nbCases*sizeof(CASE));
    for(i=0 ; i<nbCases ; i++)
    {
      (*pChemin)[i].L = cheminI[nbCases-1-i].L;
      (*pChemin)[i].C = cheminI[nbCases-1-i].C;
    }
    free(cheminI);
  }
  else
  {
    nbCases = -1;
    *pChemin = NULL;
  }   

  // Liberation memoire
  while (pListeOuverte != NULL)
  {
    temp = pListeOuverte;
    pListeOuverte = pListeOuverte->suivant;
    free(temp);
  }
  while (pListeFermee != NULL)
  {
    temp = pListeFermee;
    pListeFermee = pListeFermee->suivant;
    free(temp);
  }

  // On restore la case d'arrivee
  *(tab+arrivee.L*nbColonnes+arrivee.C) = backup;

  return nbCases; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int distance(int L1,int C1,int L2,int C2)
{
  return (abs(L1-L2) + abs(C1-C2));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void insereListe(NOEUD **ppTete,NOEUD *pNoeud)
{
  pNoeud->suivant = *ppTete;
  *ppTete = pNoeud;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NOEUD* RecupereFmin(NOEUD** ppListeOuverte)
{
  int min;
  NOEUD *p,*p2,*pMin;

  if (*ppListeOuverte == NULL) return NULL;

  // On recherche le noeud de wF minimum
  min = (*ppListeOuverte)->wF;
  pMin = *ppListeOuverte;

  p = *ppListeOuverte;
  while (p != NULL)
  {
    if (p->wF < min)
    {
      min = p->wF;
      pMin = p;
    }
    p = p->suivant;
  }

  // on dechaine pMin
  if (pMin == *ppListeOuverte)
  {
    *ppListeOuverte = (*ppListeOuverte)->suivant;
    pMin->suivant = NULL;
    return pMin;    
  }

  p2 = *ppListeOuverte;
  while (p2->suivant != pMin) p2 = p2->suivant;
  p2->suivant = pMin->suivant;
  pMin->suivant = NULL;
  return pMin; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NOEUD* estPresent(NOEUD* pTete,int L,int C)
{
  NOEUD* p;
  p = pTete;
  while (p != NULL)
  {
    if ((p->L==L) && (p->C==C)) return p;
    else p = p->suivant;
  } 
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
char valeurAutorisee(int valeur,int *valeursAutorisees,int nbValeursAutorisees)
{
  char trouve = 0;
  int  i = 0;

  while (i<nbValeursAutorisees && !trouve)
  {
    if (valeur == valeursAutorisees[i]) trouve = 1;
    else i++;
  } 
  return trouve;
}
