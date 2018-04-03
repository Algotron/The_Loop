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

typedef struct {
  int id ;
  CASE position ;
  int bille ;
} S_IDENTITE ;

#ifdef DEBUG
#define DBG(...) fprintf(stderr, " DBG(%s, %s(), %d): ", __FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__)
#else
#define DBG(...)
#endif

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

//nombre de billes au lencement
#define NBILLESMAX 60

#define NB_MAX_REQUETES 4

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

//macros de timining
#define BILLETMAX 1 //temps aléatoire max entre deux billes
#define BILLETMIN 1 //temps aléatoire max entre deux billes
#define STATUEMSEC 300 //milisec entre deux déplacements d'une statue

//variables globales
int nbBilles;
timespec attenteBille;
CASE requetes[NB_MAX_REQUETES + 1];
int indRequetesE = 0;
int indRequetesL  = 0;
int nbRequetesNonTraites = 0;
int videTab[] = {VIDE};
pthread_key_t key;

//déclaration & initilisation des mutex
pthread_mutex_t mutexNbBilles = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexTab = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexRequetes = PTHREAD_MUTEX_INITIALIZER;

//Déclaration & initilisation des variables de conditions
pthread_cond_t condRequetes = PTHREAD_COND_INITIALIZER;

//prototypes threads
void * threadPoseurBilles(void*);
void * threadBille(void *);
void * threadEvent(void *);
void * threadStatues(void *);
void * threadMage1(void *);
void * threadMage2(void *);

//prototypes de fonctions
int deplacement(CASE destination,int delai);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc,char* argv[])
{
  srand((unsigned)time(NULL));

  // Ouverture de la fenetre graphique
  DBG("(THREAD MAIN %d) Ouverture de la fenetre graphique\n",pthread_self());
	fflush(stdout);
  if (OuvertureFenetreGraphique() < 0)
  {
    DBG("Erreur de OuvrirGrilleSDL\n");
    exit(1);
  }

  initGrille();

  //création de la key variable specifique
  pthread_key_create(&key, NULL);

	srand(time(NULL));
	pthread_t poseur, expendable_t;
	bool ok = false;
	int col = 0;
	CASE caseTab[4];

	/*insertion de la case pour chaque threadStatues*/
	for(int i = 0; i < 4; i++)
	{
		caseTab[i].L = 0;
		caseTab[i].C = col;
		col += 3;
	}

	/*création du thread threadPoseurBilles*/
	DBG("création du thread threadPoseurBilles\n");
	pthread_create(&poseur, NULL, threadPoseurBilles, NULL);

	/*création du thread threadEvent*/
	DBG("création du thread threadEvent\n");
	pthread_create(&expendable_t, NULL, threadEvent, NULL);

	/*création des 4 threads threadStatues*/
	for(int i = 0; i < 4; i++)
	{
		DBG("création du thread threadStatues[%d]\n"), i + 1;
		pthread_create(&expendable_t, NULL, threadStatues, &caseTab[i]);
	}

	pthread_join(poseur, NULL);

	DBG("GAME OVER\n");
	setTitreGrilleSDL("GAME OVER");
	pause();


  /*Fermeture de la grille de jeu (SDL)*/
  DBG("(THREAD MAIN %d) Fermeture de la fenetre graphique...\n",pthread_self());
	fflush(stdout);
  FermetureFenetreGraphique();
  DBG("(THREAD MAIN %d) OK Fin",pthread_self());

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

void * threadPoseurBilles(void* p)
{
	timespec sleepTime;
	sleepTime.tv_nsec = 0;
	int billeColor, stop, blocked = 0, cmptColor = 0;
	pthread_t billeTid;

	//nombre de billes au départ
	nbBilles = NBILLESMAX;

	//dessine les dizaines puis unités de nbBilles
	DessineChiffre(11,12,nbBilles / 10);
	DessineChiffre(11,13,nbBilles % 10);
	DessineBille(11, 11, ROUGE);

	pthread_mutex_lock(&mutexNbBilles);

	while(nbBilles > 0)
	{
		//DBG("Dans Poseur de billes\n");
		pthread_mutex_unlock(&mutexNbBilles);

		//temps aléatoire entre deux billes
		sleepTime.tv_sec = (rand() % BILLETMAX + BILLETMIN);
		//DBG("temps alléatoire de %d\n", sleepTime.tv_sec);
		nanosleep(&sleepTime, NULL);

		/*blocage de nouveau threadBille si nbRequetesNonTraites = NB_MAX_REQUETES
		reprise si blocké et nbRequetesNonTraites <= NB_MAX_REQUETES / 2*/
		pthread_mutex_lock(&mutexRequetes);
		if(nbRequetesNonTraites == NB_MAX_REQUETES)
		{
			DessineBille(11, 11, GRIS);
			blocked = 1;
		}
		else if(blocked && nbRequetesNonTraites <= NB_MAX_REQUETES / 2)
		{
			blocked = 0;
			DessineBille(11, 11, ROUGE);
		}
		pthread_mutex_unlock(&mutexRequetes);

		if(!blocked)
		{
			//ordre parution couleur billes
			switch(cmptColor)
			{
				case 0 :
						billeColor = JAUNE;
						cmptColor++;
					break;
				case 1 :
						billeColor = ROUGE;
						cmptColor++;
					break;
				case 2 :
						billeColor = VERT;
						cmptColor++;
					break;
				case 3 :
						billeColor = VIOLET;
						cmptColor = 0;
					break;
			}

				/*création et detachement d'un thread threadBille*/
				pthread_create(&billeTid, NULL, threadBille, &billeColor);
				pthread_detach(billeTid);

				pthread_mutex_lock(&mutexNbBilles);

				nbBilles--;
				//dessine les dizaines puis unités de nbBilles
				DessineChiffre(11,12,nbBilles / 10);
				DessineChiffre(11,13,nbBilles % 10);
		}
	}

	pthread_mutex_unlock(&mutexNbBilles);

	DBG("Plus de biles : nbBilles = %d\n", nbBilles);

	DessineBille(11, 11, GRIS);

	sleepTime.tv_sec = 3;
	nanosleep(&sleepTime, NULL);
	pthread_exit;

}

void * threadBille(void * p)
{
	int row, column;
	int * couleur = (int *)p;
	attenteBille.tv_nsec = 0;
	attenteBille.tv_sec = 3;

	pthread_mutex_lock(&mutexTab);

	do //tant qu'il y une bille a la case tirée
	{
		pthread_mutex_unlock(&mutexTab);

		//row min = 9 row max = 14
		row = (rand() % 6 + 9);
		//column min = 0 column max = 9
		column = (rand() % 9);

		pthread_mutex_lock(&mutexTab);

	}while(tab[row][column] != 0);

	pthread_mutex_unlock(&mutexTab);

	DessineBille(row, column, * couleur);

	/*met la couleur de la bille dans la case de tab*/
	pthread_mutex_lock(&mutexTab);
	tab[row][column] = * couleur;
	pthread_mutex_unlock(&mutexTab);

	nanosleep(&attenteBille, NULL);

	/*Si apres le nanosleep la couleur n'est pas négative dans tab elle disparait*/
	pthread_mutex_lock(&mutexTab);
	if(tab[row][column] >= 0)
	{
		pthread_mutex_unlock(&mutexTab);

		EffaceCarre(row,column);

		pthread_mutex_lock(&mutexTab);
		tab[row][column] = 0;
		pthread_mutex_unlock(&mutexTab);

	}
	else
			pthread_mutex_unlock(&mutexTab);


	pthread_exit;
}

void * threadEvent(void *)
{
	EVENT_GRILLE_SDL event;
	while(1)
	{
		event = ReadEvent();

		switch(event.type)
		{
			case CROIX :
					FermetureFenetreGraphique();
					exit(0);
				break;

			case CLIC_GAUCHE :
						DBG("Event CLIC_GAUCHE\n");

						pthread_mutex_lock(&mutexTab);

						/*si case cliquée est une couleur*/
						if(tab[event.ligne][event.colonne] >= JAUNE && tab[event.ligne][event.colonne] <= VIOLET)
						{
							pthread_mutex_lock(&mutexRequetes);

							/*si le nombre de requetes non traitéesn'est pas dépassé */
							if(nbRequetesNonTraites < NB_MAX_REQUETES)
							{
								DBG("Bille capturée\n");

								/*ajout des billes capturées dans le tableau requetes*/
								requetes[indRequetesE].L = event.ligne;
								requetes[indRequetesE].C = event.colonne;

								indRequetesE++;

								//remise à 0 de l'indice requeteE si necessaire
								if(indRequetesE == (NB_MAX_REQUETES + 1))
									indRequetesE = 0;

								nbRequetesNonTraites++;

								pthread_mutex_unlock(&mutexRequetes);

								DessinePrison(event.ligne, event.colonne, JAUNE);
								/*inversion de la valeur couleur dans tab*/
								tab[event.ligne][event.colonne] = -tab[event.ligne][event.colonne];

								pthread_cond_signal(&condRequetes);
							}
							else
							{
								pthread_mutex_unlock(&mutexRequetes);
								DessineCroix(event.ligne, event.colonne);
								DBG("catpture impssible\n");
							}
						}
						pthread_mutex_unlock(&mutexTab);
				break;

			case CLIC_DROIT :
				break;

			case CLAVIER :
				break;

			default :
				break;
		}
	}
}

void * threadStatues(void * p)
{
	int i;
	CASE * caseStatue, caseCourante, caseArrive;
	caseStatue = (CASE*) p;
	caseCourante.L = caseStatue->L;
	caseCourante.C = caseStatue->C;
	int billeCouleur = 0;
  S_IDENTITE * sID = (S_IDENTITE *)malloc(sizeof(S_IDENTITE));

	/*mise du tid threadStatues dans tab*/
	pthread_mutex_lock(&mutexTab);
	tab[caseStatue->L][caseStatue->C] = pthread_self();
	pthread_mutex_unlock(&mutexTab);

	DessineStatue(caseStatue->L, caseStatue->C, BAS, 0);

  //
  sID->position.L = caseStatue->L;
  sID->position.C = caseStatue->C;
  sID->id = STATUE;
  sID->bille = 0;
  //
  pthread_setspecific(key, (void*)sID);

	while(1)
	{
		//attente d'une requete
		pthread_mutex_lock(&mutexRequetes);
		while(indRequetesL == indRequetesE)
			pthread_cond_wait(&condRequetes, &mutexRequetes);

		//recuperation de la case
		caseArrive.L = requetes[indRequetesL].L;
		caseArrive.C = requetes[indRequetesL].C;

		indRequetesL++;

		//remise à 0 de l'indice requeteL si necessaire
		if(indRequetesL == NB_MAX_REQUETES + 1)
			indRequetesL = 0;
		pthread_mutex_unlock(&mutexRequetes);

		DBG("requete: %d case[%d][%d] Tid:%d\n", indRequetesL, caseArrive.L, caseArrive.C, pthread_self());

    //deplacement vers la bille
    deplacement(caseArrive,STATUEMSEC);

    //décrémentation du nombre de requete non traitées
    pthread_mutex_lock(&mutexRequetes);
    nbRequetesNonTraites--;
    pthread_mutex_unlock(&mutexRequetes);

    //deplacement vers place statue
    deplacement(*caseStatue,STATUEMSEC);
  }
}

/**********************************************************************/
/*   deplace un mage ou statue d'une case à une autre.                */
/*   CASE destination : case vers laquelle l'ojbet doit se diriger    */
/*   int delai temps en milisecondes entre chaque déplacement         */
/**********************************************************************/
int deplacement(CASE destination,int delai)
{
  CASE  * chemin;
  timespec waitTime;
	waitTime.tv_sec = 0;
  waitTime.tv_nsec = (delai - 0) * 1000000;

  S_IDENTITE * sID = (S_IDENTITE *)malloc(sizeof(S_IDENTITE));
  int couleur = 0;

//récupération des données de la v specifique
  sID = (S_IDENTITE *)pthread_getspecific(key);

  if(sID->bille == 0);
    couleur = abs(tab[destination.L][destination.C]);

  while(sID->position.L != destination.L || sID->position.C != destination.C)
  {
    pthread_mutex_lock(&mutexTab);
    RechercheChemin(&tab[0][0], NB_LIGNES, NB_COLONNES, videTab, 1, sID->position, destination, &chemin);

    if (chemin)
    {
      //orientation de la statue
      if(sID->position.L < chemin->L)
        DessineStatue(chemin->L, chemin->C, BAS, sID->bille);
      else if (sID->position.L > chemin->L)
        DessineStatue(chemin->L, chemin->C, HAUT, sID->bille);
      else if (sID->position.C > chemin->C)
        DessineStatue(chemin->L, chemin->C, GAUCHE, sID->bille);
      else
        DessineStatue(chemin->L, chemin->C, DROITE, sID->bille);

      EffaceCarre(sID->position.L, sID->position.C);
      tab[sID->position.L][sID->position.C] = VIDE;
      tab[chemin->L][chemin->C] = pthread_self();
      sID->position.L = chemin->L;
      sID->position.C = chemin->C;
      //mise à jour de la position dans la v specifique
      pthread_setspecific(key, (void*)sID);
      free(chemin);
    }
    pthread_mutex_unlock(&mutexTab);
    nanosleep(&waitTime, NULL);
  }

  pthread_mutex_lock(&mutexTab);
  EffaceCarre(sID->position.L, sID->position.C);
  DessineStatue(sID->position.L, sID->position.C, BAS, couleur);
  pthread_mutex_unlock(&mutexTab);

  //determine la  couleur de la bille du prochain évènement. Pas de couleur si pas de bille
  sID->bille = couleur;

  //mise a jour de bille dans v specifique
  pthread_setspecific(key, (void*)sID);
  return 0;

}
