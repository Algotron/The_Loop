#ifndef THELOOP_H
#define THELOOP_H

//structure variable specifique
typedef struct {
  int id;
  CASE position;
  int bille;
} S_IDENTITE;

typedef struct {
  int couleur;
  CASE lance;
}ARGS;

//debug macro
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

//nobre maximum de requete non/traitees
#define NB_MAX_REQUETES 4

//nobre maximum de billes empiles
#define NB_MAX_PILE 6

//emplacement de tete PISTON
#define lPiston 13
#define cPiston 19

pthread_t tab[NB_LIGNES][NB_COLONNES]={
   {0,1,1,0,1,1,0,1,1,0,1,1,0,0,0,0,0,0,1,1},
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

//macros de timining
#define BILLETMAX 1 //temps aléatoire max entre deux billes
#define BILLETMIN 1 //temps aléatoire max entre deux billes
#define STATUEMSEC 300 //milisec entre deux déplacements d'une statue
#define MAGEMSEC 200 //milisec entre deux déplacements d'un mage

//variables globales
int nbBilles;
timespec attenteBille;
CASE requetes[NB_MAX_REQUETES + 1];
int indRequetesE = 0;
int indRequetesL  = 0;
int nbRequetesNonTraites = 0;
int videTab[] = {VIDE};
int indPile = 0;
int billeEchange;
unsigned int tAlarm = 5;

//nombre de bille dans chaque file
int nbFileJaune = 0;
int nbFileRouge = 0;
int nbFileVert = 0;
int nbFileViolet = 0;

//clef de la v specifique
pthread_key_t key;

//tid des threads
pthread_t poseur,Mage1, Mage2, expendable_t;
pthread_t statue[4];

//déclaration & initilisation des mutex
pthread_mutex_t mutexNbBilles = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexTab = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexRequetes = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexPile = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexEchangeL = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexEchangeE = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexFile = PTHREAD_MUTEX_INITIALIZER;


//Déclaration & initilisation des variables de conditions
pthread_cond_t condRequetes = PTHREAD_COND_INITIALIZER;
pthread_cond_t condPile = PTHREAD_COND_INITIALIZER;
pthread_cond_t condFile = PTHREAD_COND_INITIALIZER;

//prototypes threads
void * threadPoseurBilles(void*);
void * threadBille(void *);
void * threadEvent(void *);
void * threadStatues(void *);
void * threadMage1(void *);
void * threadMage2(void *);
void * threadBilleQuiRoule(void *);
void * threadPiston(void *);

//prototypes de fonctions
void initGrille();
bool CaseReservee(CASE Case);
int deplacement(CASE destination,int delai);

//prototype handler
void Alrm_Usr1(int);


#define STATUEMAGE (tab[event.ligne][event.colonne] == statue[0]) || (tab[event.ligne][event.colonne] == statue[1]) || (tab[event.ligne][event.colonne] == statue[2]) || (tab[event.ligne][event.colonne] == statue[3]) || (tab[event.ligne][event.colonne] == Mage1) || (tab[event.ligne][event.colonne] == Mage2)
#endif
