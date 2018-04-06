#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "GrilleSDL.h"
#include "Ressources.h"
#include "AStar.h"
#include "TheLoop.h"

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

  //masquage de SIGHUP
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGHUP);
  sigprocmask(SIG_SETMASK, &mask, NULL);

  //création de la key variable specifique
  pthread_key_create(&key, NULL);

  //prise des mutex echange
  pthread_mutex_lock(&mutexEchangeL);
  pthread_mutex_lock(&mutexEchangeE);

	srand(time(NULL));
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

  /*Création du thread threadMage1*/
  DBG("création du thread threadMage1\n");
  pthread_create(&Mage1, NULL, threadMage1, NULL);

  /*Création du thread threadMage2*/
  DBG("création du thread threadMage2\n");
  pthread_create(&Mage2, NULL, threadMage2, NULL);

  /*Création du thread threadPiston*/
  DBG("création du thread threadPiston\n");
  pthread_create(&expendable_t, NULL, threadPiston, NULL);

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

void  initGrille()
{
  for (int L=0 ; L<NB_LIGNES ; L++)
    for (int C=0 ; C<NB_COLONNES ; C++)
      if (tab[L][C] == MUR) DessineMur(L,C,PIERRE);

  DessineMur(11,11,METAL);
  DessineMur(13,11,METAL);
  DessineBille(13,11,GRIS);
}

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

void * threadPoseurBilles(void* param)
{
	timespec sleepTime;
	sleepTime.tv_nsec = 0;
	int billeColor, stop, blocked = 0, cmptColor = 0;
	pthread_t billeTid;

	//nombre de billes au départ
	nbBilles = NBILLESMAX;

	pthread_mutex_lock(&mutexNbBilles);

	//dessine les dizaines puis unités de nbBilles
	DessineChiffre(11,12,nbBilles / 10);
	DessineChiffre(11,13,nbBilles % 10);
	DessineBille(11, 11, ROUGE);


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

void * threadBille(void * param)
{
	int row, column;
	int * couleur = (int *)param;
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

void * threadEvent(void * param)
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

void * threadStatues(void * param)
{
	int i;
	CASE * caseStatue, caseCourante, caseArrive;
	caseStatue = (CASE*) param;
	caseCourante.L = caseStatue->L;
	caseCourante.C = caseStatue->C;
  S_IDENTITE * sID = (S_IDENTITE *)malloc(sizeof(S_IDENTITE));
  timespec tPile;

  tPile.tv_nsec = 0;
  tPile.tv_sec = 1;

	/*mise du tid threadStatues dans tab*/
	pthread_mutex_lock(&mutexTab);
	tab[caseStatue->L][caseStatue->C] = pthread_self();
	pthread_mutex_unlock(&mutexTab);

	DessineStatue(caseStatue->L, caseStatue->C, BAS, 0);

  //initilisation v specifique
  sID->position.L = caseStatue->L;
  sID->position.C = caseStatue->C;
  sID->id = STATUE;
  //

	while(1)
	{
    //(ré)initilisation bille V specifique
    sID->bille = 0;
    pthread_setspecific(key, (void*)sID);

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
    sID->bille = deplacement(caseArrive,STATUEMSEC);
    DessineStatue(sID->position.L,sID->position.C, BAS,sID->bille );

    //on donne la couleurs de la bille qui remonte
    pthread_setspecific(key, (void*)sID);

    //décrémentation du nombre de requete non traitées
    pthread_mutex_lock(&mutexRequetes);
    nbRequetesNonTraites--;
    pthread_mutex_unlock(&mutexRequetes);

    //deplacement vers place statue
    deplacement(*caseStatue,STATUEMSEC);

    //dessine la statue en attente avec la bille en main
    DessineStatue(sID->position.L, sID->position.C, BAS, sID->bille);

    //attente d'une place dans la pile
    pthread_mutex_lock(&mutexPile);
    while(indPile >= NB_MAX_PILE)
    {
      pthread_mutex_unlock(&mutexPile);
      nanosleep(&tPile, NULL);
      pthread_mutex_lock(&mutexPile);
    }

    //insertion couleurBille dans tab
    pthread_mutex_lock(&mutexTab);
    tab[0][indPile + 12] = -sID->bille;
    pthread_mutex_unlock(&mutexTab);

    //Dessine la bille case 12-13-14-15-16-17
    DessineBille(0, indPile + 12, sID->bille);

    indPile++;

    pthread_mutex_unlock(&mutexPile);

    //la statue a déposée la bille on la redessine sans bille
    DessineStatue(sID->position.L, sID->position.C, BAS, 0);

    //notification d'une nouvelle bille dans la pille à Mage1
    pthread_cond_signal(&condPile);
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
  S_IDENTITE * sID = (S_IDENTITE *)malloc(sizeof(S_IDENTITE));
  int couleur;

	waitTime.tv_sec = delai / 1000;
  waitTime.tv_nsec = (delai - waitTime.tv_sec) * 1000000;

  //récupération des données de la v specifique
  sID = (S_IDENTITE *)pthread_getspecific(key);

  //si Mage couleur [0][C] dans pile
  if(sID->id == STATUE)
    couleur = -tab[destination.L][destination.C];
  else
    couleur = -tab[0][destination.C];

  // tant que la position est differente de la destination
  while(sID->position.L != destination.L || sID->position.C != destination.C)
  {
    pthread_mutex_lock(&mutexTab);
    //recherche du prochain déplacement
    RechercheChemin(&tab[0][0], NB_LIGNES, NB_COLONNES, videTab, 1, sID->position, destination, &chemin);

    if (chemin)
    {
      //orientation de la statue
      if(sID->position.L < chemin->L)
      {
        if(sID->id == STATUE)
          DessineStatue(chemin->L, chemin->C, BAS, sID->bille);
        else
          DessineMage(chemin->L, chemin->C, BAS, sID->bille);
      }
      else if (sID->position.L > chemin->L)
      {
        if(sID->id == STATUE)
          DessineStatue(chemin->L, chemin->C, HAUT, sID->bille);
        else
          DessineMage(chemin->L, chemin->C, HAUT, sID->bille);
      }
      else if (sID->position.C > chemin->C)
      {
        if(sID->id == STATUE)
          DessineStatue(chemin->L, chemin->C, GAUCHE, sID->bille);
        else
          DessineMage(chemin->L, chemin->C, GAUCHE, sID->bille);
      }
      else
      {
        if(sID->id == STATUE)
          DessineStatue(chemin->L, chemin->C, DROITE, sID->bille);
        else
          DessineMage(chemin->L, chemin->C, DROITE, sID->bille);
      }

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
  pthread_mutex_unlock(&mutexTab);

  //mise a jour de bille dans v specifique
  pthread_setspecific(key, (void*)sID);

  return couleur;

}

void * threadMage1(void * param)
{
  S_IDENTITE * sID = (S_IDENTITE *)malloc(sizeof(S_IDENTITE));
  CASE caseMage1, caseEchange, casePile;
  int couleur;

  //initilisation cases données
  caseMage1.L = 1;
  caseMage1.C =11;
  caseEchange.L = 5;
  caseEchange.C = 12;

  DessineMage(caseMage1.L, caseMage1.C, DROITE, 0);

  // initilisation V specifique
  sID->position.L = caseMage1.L;
  sID->position.C = caseMage1.C;
  sID->id = MAGE;
  sID->bille = 0;
  pthread_setspecific(key, (void*)sID);

  while(1)
	{
		//attente d'une bille
		pthread_mutex_lock(&mutexPile);
		while(indPile == 0)
			pthread_cond_wait(&condPile, &mutexPile);

    //récupération du déplacement
    casePile.L = 1;
    casePile.C = indPile + 11;
    pthread_mutex_unlock(&mutexPile);

    //déplacement haut de la pile
    couleur = deplacement(casePile, MAGEMSEC);
    pthread_mutex_lock(&mutexPile);

    //vérification ajout bille pendant deplacement
    while (casePile.C != indPile + 11)
    {
      casePile.C++;
      pthread_mutex_unlock(&mutexPile);
      couleur = deplacement(casePile, MAGEMSEC);
      pthread_mutex_lock(&mutexPile);
    }

    //modification de la couleur dans V specifique
    sID->bille = couleur;
    pthread_setspecific(key, (void*)sID);

    //supression de la bille dans tab
    pthread_mutex_lock(&mutexTab);
    tab[0][sID->position.C] = VIDE;
    pthread_mutex_unlock(&mutexTab);

    indPile--;
    EffaceCarre(0, sID->position.C);

    pthread_mutex_unlock(&mutexPile);

    //dessine mage avec la bille
    DessineMage(sID->position.L, sID->position.C, DROITE, couleur);

    //notification au Mage2
    pthread_kill(Mage2, SIGHUP);

    //se deplace vers la case d'échange
    deplacement(caseEchange, MAGEMSEC);
    DessineMage(caseEchange.L, caseEchange.C, DROITE, couleur);

    //attente de Mage2
    pthread_mutex_lock(&mutexEchangeE);

    //Mage1 depose la bille
    billeEchange = sID->bille;

    //Mage1 sans bille
    DessineMage(caseEchange.L, caseEchange.C, DROITE, 0);

    //Mage2 peut prendre la bille
    pthread_mutex_unlock(&mutexEchangeL);

    //retour sans bille
    sID->bille = 0;
    pthread_setspecific(key, (void*)sID);

    //retourne vers sa case de départ
    deplacement(caseMage1, MAGEMSEC);

    //redessine le mage sans bille vers la DROITE
    DessineMage(caseMage1.L, caseMage1.C, DROITE, 0);

  }
}

void * threadMage2(void * param)
{
  S_IDENTITE * sID = (S_IDENTITE *)malloc(sizeof(S_IDENTITE));
  CASE caseMage2, caseEchange2, caseFile;
  timespec waitTemp;
  int sig;
  int * pFile;
  sigset_t set;
  ARGS args;

  //creation du set de signaux pour sigwait
  sigemptyset(&set);
  sigaddset(&set, SIGHUP);

  //initilisation cases données
  caseMage2.L = 7;
  caseMage2.C = 14;
  caseEchange2.L = 5;
  caseEchange2.C = 13;
  caseFile.L = 7;

  // initilisation V specifique
  sID->position.L = caseMage2.L;
  sID->position.C = caseMage2.C;
  sID->id = MAGE;
  sID->bille = 0;
  pthread_setspecific(key, (void*)sID);

  waitTemp.tv_nsec = 300000000;
  waitTemp.tv_sec = 0;

  DessineMage(caseMage2.L, caseMage2.C, DROITE, 0);

  while(1)
  {
    //attente de notification de Mage1
    sigwait(&set, &sig);

    //deplacement vers la case d'echange
    deplacement(caseEchange2, MAGEMSEC);
    DessineMage(caseEchange2.L, caseEchange2.C, GAUCHE, 0);

    //débloque le mutex pour Mage1
    pthread_mutex_unlock(&mutexEchangeE);

    //attente de Mage1
    pthread_mutex_lock(&mutexEchangeL);

    //Mage2 récupère la bille
     sID->bille = billeEchange;

    //Mage2 avec bille
    DessineMage(caseEchange2.L, caseEchange2.C, GAUCHE, sID->bille);

    //modifie la bille dans V specifique
    pthread_setspecific(key, (void*)sID);

    //deplacement vers la bonne fille
    if(sID->bille == JAUNE)
      caseFile.C = 15;
    else if(sID->bille == ROUGE)
      caseFile.C = 16;
    else if(sID->bille == VERT)
      caseFile.C = 17;
    else
      caseFile.C = 18;

    deplacement(caseFile, MAGEMSEC);
    DessineMage(caseFile.L, caseFile.C, BAS, sID->bille);

    //attente de place dans la file
    pthread_mutex_lock(&mutexTab);
    while (tab[caseFile.L + 1][caseFile.C] != 0)
    {
      pthread_mutex_unlock(&mutexTab);
      nanosleep(&waitTemp, NULL);
      pthread_mutex_lock(&mutexTab);
    }
    pthread_mutex_unlock(&mutexTab);

    //initilisation des arguments Pour threadBilleQuiRoule
    args.couleur = sID->bille;
    args.lance.L = caseFile.L + 1;
    args.lance.C = caseFile.C;

    /*Création du thread threadBilleQuiRoule*/
    DBG("création du thread threadBilleQuiRoule\n");
    pthread_create(&expendable_t, NULL, threadBilleQuiRoule, &args);

    //retour sans bille
    sID->bille = 0;
    pthread_setspecific(key, (void*)sID);

    //retourne a sa case
    deplacement(caseMage2, MAGEMSEC);

    DessineMage(caseMage2.L, caseMage2.C, DROITE, 0);

  }
}

void * threadBilleQuiRoule(void * param)
{
  ARGS * args = (ARGS *)param;
  timespec waitTemp;

  waitTemp.tv_nsec = 300000000;
  waitTemp.tv_sec = 0;

  pthread_mutex_lock(&mutexFile);
  if (args->couleur == JAUNE)
    nbFileJaune++;
  else if(args->couleur == ROUGE)
    nbFileRouge++;
  else if(args->couleur == VERT)
    nbFileVert++;
  else
    nbFileViolet++;

  pthread_mutex_unlock(&mutexFile);

  //tant qu'il n'y a pas de bille dans la case suivante et que ligne < 13
  pthread_mutex_lock(&mutexTab);
  while(tab[args->lance.L][args->lance.C] == 0 && args->lance.L <= 12)
  {
    tab[args->lance.L][args->lance.C] = -args->couleur;

    //supression du mouvement precedent
    if (args->lance.L > 8)
    {
      tab[args->lance.L -1][args->lance.C] = VIDE;
      EffaceCarre(args->lance.L -1, args->lance.C);
    }
    pthread_mutex_unlock(&mutexTab);
    DessineBille(args->lance.L, args->lance.C, args->couleur);

    nanosleep(&waitTemp, NULL);

    //incrementation de la ligne
    args->lance.L++;
    pthread_mutex_lock(&mutexTab);
  }
  pthread_mutex_unlock(&mutexTab);

  pthread_cond_signal(&condFile);

}

void * threadPiston(void * param)
{
  CASE caseTete, caseBille;
  int cBille, i, j, couleur, compteur = 11;
  timespec waitTemp;

  waitTemp.tv_nsec = 600000000;
  waitTemp.tv_sec = 0;

  while (1)
  {
    caseTete.L = lPiston;
    caseTete.C = cPiston;

    DessinePiston(caseTete.L,caseTete.C, TETE);
    //attente d'une notification et qu'une des fille soit pleine
    pthread_mutex_lock(&mutexFile);
    while (nbFileJaune < 4 && nbFileRouge < 4 && nbFileVert < 4 && nbFileViolet < 4)
      pthread_cond_wait(&condFile,&mutexFile);

    DBG("threadPiston Debloqué\n");
		//détermine la couleur de la bille et sa colonne
    if (nbFileJaune >= 4)
    {
      cBille = 15;
      couleur = JAUNE;
    }
    else if (nbFileRouge >= 4)
    {
      cBille = 16;
      couleur = ROUGE;
    }
    else if (nbFileVert >= 4)
    {
      cBille = 17;
      couleur = VERT;
    }
    else
    {
      cBille = 18;
      couleur = VIOLET;
    }

    pthread_mutex_unlock(&mutexFile);

    i = 0;
		//déplacement du pistion jusqu'a la case avant la file
    while (caseTete.C -1 != cBille)
    {
      caseTete.C--;
      DessinePiston(caseTete.L,caseTete.C, TETE);
      EffaceCarre(caseTete.L, caseTete.C + 1);
      pthread_mutex_lock(&mutexTab);
      tab[caseTete.L][caseTete.C] = PISTON;
      pthread_mutex_unlock(&mutexTab);

			//dessine les tiges / i
      for (j = 1; j <= i +1; j++)
      {
        EffaceCarre(caseTete.L, caseTete.C + j);
        DessinePiston(caseTete.L, caseTete.C + j, TIGE);
        pthread_mutex_lock(&mutexTab);
        tab[caseTete.L][caseTete.C + j] = PISTON;
        pthread_mutex_unlock(&mutexTab);
      }
      i++;
      nanosleep(&waitTemp,NULL);
    }

		i = 12; //case la plus basse de la file
		//on decale la file
		pthread_mutex_lock(&mutexTab);
    while(tab[i][cBille] != 0 && tab[i -1][cBille] != Mage2)
    {
			EffaceCarre(i, cBille);
			DessineBille(i + 1, cBille, couleur);
			tab[i][cBille] = VIDE;
			tab[i + 1][cBille] = -couleur;
			pthread_mutex_unlock(&mutexTab);
			i--;
			nanosleep(&waitTemp,NULL);
    }

		//decrementation du bon compteur
    pthread_mutex_lock(&mutexFile);
    if (couleur == JAUNE)
      nbFileJaune--;
    else if (couleur == ROUGE)
      nbFileRouge--;
    else if (couleur == VERT)
      nbFileVert--;
    else
      nbFileViolet--;
    pthread_mutex_unlock(&mutexFile);

    i = 0;
		//avance jusqu'au mur
    while (caseTete.C -1 != 11)
    {
      caseTete.C--;
			EffaceCarre(caseTete.L,caseTete.C);
			DessineBille(caseTete.L,caseTete.C -1, couleur);
      DessinePiston(caseTete.L,caseTete.C, TETE);
      EffaceCarre(caseTete.L, caseTete.C + 1);
      pthread_mutex_lock(&mutexTab);
      tab[caseTete.L][caseTete.C] = PISTON;
      pthread_mutex_unlock(&mutexTab);

      for (j = 1; j <= i + 1; j++)
      {
        EffaceCarre(caseTete.L, caseTete.C + j);
        DessinePiston(caseTete.L, caseTete.C + j, TIGE);
        pthread_mutex_lock(&mutexTab);
        tab[caseTete.L][caseTete.C + j] = PISTON;
        pthread_mutex_unlock(&mutexTab);
      }
      i++;
      nanosleep(&waitTemp,NULL);
    }

		DessineBille(lPiston,11,GRIS);

		pthread_mutex_lock(&mutexNbBilles);
		nbBilles++;
		//dessine les dizaines puis unités de nbBilles
		DessineChiffre(11,12,nbBilles / 10);
		DessineChiffre(11,13,nbBilles % 10);
		pthread_mutex_unlock(&mutexNbBilles);


		while(caseTete.C != cPiston)
		{
			EffaceCarre(caseTete.L, caseTete.C);
			EffaceCarre(caseTete.L, caseTete.C + 1);
			DessinePiston(caseTete.L, caseTete.C + 1, TETE);
			caseTete.C++;

			pthread_mutex_lock(&mutexTab);
			tab[caseTete.L][caseTete.C] = VIDE;
			pthread_mutex_unlock(&mutexTab);

			nanosleep(&waitTemp,NULL);
		}
  }


}
