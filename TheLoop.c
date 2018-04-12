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
	sigset_t mask;

	//masquage tous les signaux
	sigemptyset(&mask);
  sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGALRM);	sigprocmask(SIG_SETMASK, &mask, NULL);
	sigprocmask(SIG_SETMASK, &mask, NULL);

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

	//Redirection des signaux SIGUSR1 & SIGALRM
  struct sigaction sigAct;
  sigAct.sa_handler = Alrm_Usr1;
	sigAct.sa_flags = 0;
	sigaction(SIGALRM, &sigAct, NULL);
  sigaction(SIGUSR1, &sigAct, NULL);

  //création de la key variable specifique
  pthread_key_create(&key, NULL);

  //prise des mutex echange
  pthread_mutex_lock(&mutexEchangeL);
  pthread_mutex_lock(&mutexEchangeE);

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

	/*création du thread threadChrono*/
	pthread_create(&chrono, NULL, threadChrono, NULL);
	DBG("création du thread threadChrono %d\n", expendable_t);

	/*création du thread threadPoseurBilles*/
	pthread_create(&poseur, NULL, threadPoseurBilles, NULL);
	DBG("création du thread threadPoseurBilles %d\n", poseur);

	/*création du thread threadEvent*/
	pthread_create(&expendable_t, NULL, threadEvent, NULL);
	DBG("création du thread threadEvent %d\n", expendable_t);

	/*création des 4 threads threadStatues*/
	for(int i = 0; i < 4; i++)
	{
		pthread_create(&statue[i], NULL, threadStatues, &caseTab[i]);
		DBG("création du thread threadStatues[%d] %d\n", i + 1, statue[i]);
	}

  /*Création du thread threadMage1*/
  pthread_create(&Mage1, NULL, threadMage1, NULL);
	DBG("création du thread threadMage1 %d\n", Mage1);

  /*Création du thread threadMage2*/
  pthread_create(&Mage2, NULL, threadMage2, NULL);
	DBG("création du thread threadMage2 %d\n", Mage2);

  /*Création du thread threadPiston*/
  pthread_create(&expendable_t, NULL, threadPiston, NULL);
	DBG("création du thread threadPiston %d\n", expendable_t);

	/*Création du thread threadPoseurMur*/
	pthread_create(&expendable_t, NULL, threadPoseurMur, NULL);
	DBG("création du thread threadPoseurMur %d\n", expendable_t);

	alarm(tAlarm);

	pthread_join(poseur, NULL);

	DBG("GAME OVER\n");
	setTitreGrilleSDL("GAME OVER");
	DessineImageFond("./images/game-over.bmp");
	DessineChiffre(3, 10, compteur / 100);
	DessineChiffre(3, 10, (compteur % 100) / 10);
	DessineChiffre(3, 10,  compteur % 10);
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
	int billeColor, stop, blocked = 0, cmptColor = 0;
	pthread_t billeTid;
	sigset_t mask;
	int resultat;

	//masquage de SIGHUP & SIGALRM
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_SETMASK, &mask, NULL);

	//nombre de billes au départ
	nbBilles = NBILLESMAX;

	pthread_mutex_lock(&mutexNbBilles);

	//dessine les dizaines puis unités de nbBilles
	DessineChiffre(11,12,nbBilles / 10);
	DessineChiffre(11,13,nbBilles % 10);
	DessineBille(11, 11, ROUGE);


	while(nbBilles > 0)
	{
		pthread_mutex_unlock(&mutexNbBilles);

		//temps aléatoire entre deux billes
		resultat = (rand() % (billeTmax - billeTmin + 1)) + billeTmin;
		sleepTime.tv_sec = resultat / 1000;
	  sleepTime.tv_nsec = (resultat % 1000) * 1000000;

		DBG("Prochaine bille dans %dmillisec\n", resultat);

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
	timespec attenteBille;

	attenteBille.tv_sec =  clicBille / 1000;
	attenteBille.tv_nsec = (clicBille % 1000) * 1000000;

	sigset_t mask;

	//masquage de SIGHUP & SIGALRM
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_SETMASK, &mask, NULL);

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
	if(tab[row][column] <= VIOLET && tab[row][column] >= JAUNE)
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
	sigset_t mask;

	//masquage de SIGHUP & SIGALRM
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_SETMASK, &mask, NULL);

	EVENT_GRILLE_SDL event;
	while(1)
	{
		event = ReadEvent();

		switch(event.type)
		{
			case CROIX :
			DBG("Event CROIX\n");
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
								tab[event.ligne][event.colonne] = PRISE(tab[event.ligne][event.colonne]);
								DBG("bille prise %d\n",tab[event.ligne][event.colonne]);

								pthread_cond_signal(&condRequetes);
							}
							else
							{
								pthread_mutex_unlock(&mutexRequetes);
								DessineCroix(event.ligne, event.colonne);
								DBG("catpture impossible\n");
							}
						}
						else if(tab[event.ligne][event.colonne]  < 0 && tab[event.ligne][event.colonne] > -JAUNE)
						{
							pthread_kill(tab[event.ligne][event.colonne], SIGUSR1);
							DBG("signal envoyé a %d\n", tab[event.ligne][event.colonne]);
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
  timespec tPile, tDecrementation;
	sigset_t mask;

	//masquage de SIGHUP
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_SETMASK, &mask, NULL);

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
		// initilisation de la tempo place pile
		tPile.tv_sec = attentePFile / 1000;
		tPile.tv_nsec = (attentePFile % 1000) * 1000000;

		//initilisation de la tempo decrementation nbRequetesNonTraites
		tDecrementation.tv_sec = decrementation / 1000;
		tDecrementation.tv_nsec = (decrementation % 1000) * 1000000;

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
    sID->bille = deplacement(caseArrive,statueT);
    DessineStatue(sID->position.L,sID->position.C, BAS,sID->bille );

    //on donne la couleurs de la bille qui remonte
    pthread_setspecific(key, (void*)sID);

		nanosleep(&tDecrementation,NULL);

    //décrémentation du nombre de requete non traitées
    pthread_mutex_lock(&mutexRequetes);
    nbRequetesNonTraites--;
    pthread_mutex_unlock(&mutexRequetes);

    //deplacement vers place statue
    deplacement(*caseStatue,statueT);

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
    tab[0][indPile + 12] = PRISE(sID->bille);
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
  waitTime.tv_nsec = (delai % 1000) * 1000000;

  //récupération des données de la v specifique
  sID = (S_IDENTITE *)pthread_getspecific(key);

  //si Mage couleur [0][C] dans pile
  if(sID->id == STATUE)
    couleur = PPRISE(tab[destination.L][destination.C]);
  else
    couleur = PPRISE(tab[0][destination.C]);

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
	sigset_t mask;

	//masquage de SIGHUP
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_SETMASK, &mask, NULL);

  //initilisation cases données
  caseMage1.L = 1;
  caseMage1.C =11;
  caseEchange.L = 5;
  caseEchange.C = 12;

  DessineMage(caseMage1.L, caseMage1.C, DROITE, 0);
	pthread_mutex_lock(&mutexTab);
	tab[caseMage1.L][caseMage1.C] = pthread_self();
	pthread_mutex_unlock(&mutexTab);


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
    couleur = deplacement(casePile, mageT);
    pthread_mutex_lock(&mutexPile);

    //vérification ajout bille pendant deplacement
    while (casePile.C != indPile + 11)
    {
      casePile.C++;
      pthread_mutex_unlock(&mutexPile);
      couleur = deplacement(casePile, mageT);
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
    deplacement(caseEchange, mageT);
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
    deplacement(caseMage1, mageT);

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
	sigset_t mask;
  ARGS args;

	//masquage de SIGHUP
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_SETMASK, &mask, NULL);

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

  DessineMage(caseMage2.L, caseMage2.C, DROITE, 0);
	pthread_mutex_lock(&mutexTab);
	tab[caseMage2.L][caseMage2.C] = pthread_self();
	pthread_mutex_unlock(&mutexTab);

  while(1)
  {
		// initilisation de la tempo
		waitTemp.tv_sec = attentePFile / 1000;
		waitTemp.tv_nsec = (attentePFile % 1000) * 1000000;

    //attente de notification de Mage1
    sigwait(&set, &sig);

    //deplacement vers la case d'echange
    deplacement(caseEchange2, mageT);
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

    deplacement(caseFile, mageT);
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
    pthread_create(&expendable_t, NULL, threadBilleQuiRoule, &args);
		DBG("création du thread threadBilleQuiRoule %d\n", expendable_t);

    //retour sans bille
    sID->bille = 0;
    pthread_setspecific(key, (void*)sID);

    //retourne a sa case
    deplacement(caseMage2, mageT);

    DessineMage(caseMage2.L, caseMage2.C, DROITE, 0);

  }
}

void * threadBilleQuiRoule(void * param)
{
  ARGS * args = (ARGS *)param;
  timespec waitTemp;
	sigset_t mask;

	//masquage de SIGHUP & SIGALRM
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_SETMASK, &mask, NULL);

	// initilisation de la tempo
	waitTemp.tv_sec = enfile / 1000;
	waitTemp.tv_nsec = (enfile % 1000) * 1000000;

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
    tab[args->lance.L][args->lance.C] = PRISE(args->couleur);

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
  timespec waitTemp,effacebille;
	sigset_t mask;

	//masquage de SIGHUP & SIGALRM
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_SETMASK, &mask, NULL);

  while (1)
  {

		// initilisation de la tempo
		waitTemp.tv_sec = enfile / 1000;
		waitTemp.tv_nsec = (enfile % 1000) * 1000000;

		// initilisation de la tempo
		effacebille.tv_sec = tEfface / 1000;
		effacebille.tv_nsec = (tEfface % 1000) * 1000000;

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
			tab[i + 1][cBille] = PRISE(couleur);
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
      nanosleep(&effacebille,NULL);
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

void Alrm_Usr1(int sig)
{
	timespec prisonTime;
	int couleurPrison;
	S_IDENTITE * sID = (S_IDENTITE *)malloc(sizeof(S_IDENTITE));

	sID = (S_IDENTITE *)pthread_getspecific(key);

	//pid negatif dans tab pour ne plus pouvoir cliquer
	tab[sID->position.L][sID->position.C] = -tab[sID->position.L][sID->position.C];

	prisonTime.tv_nsec = 0;

	//SIGUSR1
	if(sig == SIGUSR1)
	{
		DBG("SIGUSR1 recu par %d\n", pthread_self());
		prisonTime.tv_sec = 10;
		couleurPrison = JAUNE;
	}
	else//SIGALRM
	{
		DBG("SIGALRM recu par %d\n", pthread_self());
		prisonTime.tv_sec = 5;
		couleurPrison = VERT;
	}

	//MAGE
	if (sID->id == MAGE)
		DessineMage(sID->position.L, sID->position.C, BAS, 0);
	else//STATUE
		DessineStatue(sID->position.L, sID->position.C, BAS, 0);

		//dessine la prison de la bonne couleur
		DessinePrison(sID->position.L, sID->position.C, couleurPrison);

		//attente de la fin de la peine
		nanosleep(&prisonTime, NULL);

		//efface la STATUE/MAge et la prison
		EffaceCarre(sID->position.L, sID->position.C);

		// redessine MAGE
		if (sID->id == MAGE)
			DessineMage(sID->position.L, sID->position.C, BAS, 0);
		else//redessine STATUE
			DessineStatue(sID->position.L, sID->position.C, BAS, 0);

		//remise du PID correct dans tab
		tab[sID->position.L][sID->position.C] = -tab[sID->position.L][sID->position.C];

		//tirage du prochain SIGALRM
		if (sig = SIGALRM)
		{
			tAlarm = (rand() % (15 - 5 + 1)) + 5;
			alarm(tAlarm);
			DBG("Prochain alarm() dans %ds\ns", tAlarm);
		}
}

void * threadChrono(void * param)
{
	sigset_t mask;
	timespec timeWait;

	//masquage SIGHUP & SIGALRM
	sigemptyset(&mask);
  sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGALRM);	sigprocmask(SIG_SETMASK, &mask, NULL);
	sigprocmask(SIG_SETMASK, &mask, NULL);

	//dessine les dizaines puis unités de nbBilles
	DessineChiffre(9, 11, 0);
	DessineChiffre(9, 12, 0);
	DessineChiffre(9, 13, 0);

	timeWait.tv_nsec = 0;
	timeWait.tv_sec = 1;

	while(1)
	{
		pthread_mutex_lock(&mutexNbBilles);
		if(nbBilles == 0)
		{
			pthread_mutex_unlock(&mutexNbBilles);
			pthread_exit(NULL);
		}
		pthread_mutex_unlock(&mutexNbBilles);

		nanosleep(&timeWait, NULL);
		compteur++;
		DessineChiffre(9, 11, compteur / 100);
		DessineChiffre(9, 12, (compteur % 100) / 10);
		DessineChiffre(9, 13,  compteur % 10);

		//si le compteur est multiple de 60
		if(!(compteur % 60))
		{
			DBG("accélération\n");
			billeTmax *= 0.9;
			billeTmin *= 0.9;
			statueT *= 0.9;
			mageT *= 0.9;
			clicBille *= 0.9;
			attentePFile *= 0.9;
			enfile *= 0.9;
			decrementation *= 0.9;
			tEfface *= 0.9;
		}
	}

}

void * threadPoseurMur(void * param)
{
	timespec waitTemp;
	int temps;

	while (1)
	{
		temps = (rand() % (lanceMurMax - lanceMurMin + 1 )) + lanceMurMin;
		waitTemp.tv_sec =	temps / 1000;
		waitTemp.tv_nsec = (temps % 1000) * 1000000;

		nanosleep(&waitTemp, NULL);

		//création d'un thread mur
		pthread_create(&expendable_t, NULL, threadMur, NULL);

	}
}


void * threadMur(void * param)
{
	timespec timeWait;
	CASE randCase;
	int cMin, cMax, sig;
	sigset_t set;

	//creation du set de signaux pour sigwait
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	timeWait.tv_sec =	attenteMur / 1000;
	timeWait.tv_nsec = (attenteMur % 1000) * 1000000;


	if (((rand() % 3) + 1) == 3)
	{
		cMin = 11;
		cMax = 19;
	}
	else
	{
		cMin = 0;
		cMax = 9;
	}


	do
	{
		//row min =0  row max = NB_LIGNES -1
		randCase.L = (rand() %((NB_LIGNES - 1) - 0 + 1)) + 0;
		//column min = 0 | 11 column max = 9 | 19
		randCase.C = (rand() % (cMax - cMin + 1)) + cMin;
	}
	while(CaseReservee(randCase));

	//pid négatif dans tab pour empecher le clic
	pthread_mutex_lock(&mutexTab);
	tab[randCase.L][randCase.C] = -pthread_self();
	pthread_mutex_unlock(&mutexTab);

	DessineMur(randCase.L, randCase.C , METAL);

	nanosleep(&timeWait, NULL);

	//pid négatif dans tab pour autoriser le clic
	pthread_mutex_lock(&mutexTab);
	tab[randCase.L][randCase.C] = pthread_self();
	pthread_mutex_unlock(&mutexTab);

	DessineCroix(randCase.L, randCase.C);

	sigwait(&set, &sig);

	pthread_mutex_lock(&mutexTab);
	tab[randCase.L][randCase.C] = VIDE;
	pthread_mutex_unlock(&mutexTab);

	EffaceCarre(randCase.L, randCase.C);

}
