/* Exercice 1 :
   Gestion d'un fichier.
   In.Pr.E.S.
   Denys MERCENIER
   Ecran.c
*/

#include "Ecran.h"

void EffEcran(void)
{
#ifdef CPP
cout << "0\33[2J" ;
#else
printf("\033[2J");
#endif
}

int LitChaine(char *pChaine,int Lig,int Col,int Attrib,int Lg)
{
char        Buffer[255];
int         L;

#ifdef CPP
cout << "\033[" <<  Lig <<  ";" <<  Col <<  "H\033[" <<  Attrib <<  "m" ;
cin.getline(Buffer,255);
cout << "\033[0m";
L = strlen(Buffer) ;
if (L >= Lg) L = Lg -1;
Buffer[L] = '\0';
strcpy(pChaine,Buffer);
#else
printf("\033[%d;%dH\033[%dm",Lig,Col,Attrib);
if (fgets(Buffer,255,stdin) == NULL) return -1;
fflush(stdin);
L = strlen(Buffer) -1;
if (L >= Lg) L = Lg -1;
Buffer[L] = '\0';
strcpy(pChaine,Buffer);
printf("\033[%dm",NORM);
fflush(stdout);
#endif
return L;
}

int LitEntier(int *Nombre,int Lig,int Col,int Attrib,int Lg)
{
char        Buffer[255];
int         L;

#ifdef CPP
cout << "\033[" <<  Lig <<  ";" <<  Col <<  "H\033[" <<  Attrib <<  "m" ;
cin.getline(Buffer,255);
cout << "\033[0m";
*Nombre = atoi(Buffer);
if (*Nombre == 0)
   if (Buffer[0] != '0') return -1;
#else
printf("\033[%d;%dH\033[%dm",Lig,Col,Attrib);
if (fgets(Buffer,255,stdin) == NULL) return -1;
fflush(stdin);
printf("\033[%dm",NORM);
fflush(stdout);
*Nombre  = atoi(Buffer);
if (*Nombre == 0)
   if (Buffer[0] != '0') return -1;
#endif
return 0;
}

void AffChaine(const char *pChaine,int Lig,int Col, int Attrib)
{
#ifdef CPP
cout << "\033[" <<  Lig <<  ";" <<  Col <<  "H\033[" <<  Attrib <<  "m" <<  pChaine << "\033[0m";
#else
printf("\033[%d;%dH\033[%dm%s\033[%dm",Lig,Col,Attrib,pChaine,NORM);
fflush(stdout);
#endif
}

void Pause(void)
{
char	Buffer[255];
#ifdef CPP
cin.getline(Buffer,255);
#else
fflush(stdout);
getchar();
fflush(stdin);
#endif
return;
}

void Trace1(const char* pFichier,int Ligne,const char *pTrace, ... )
{
char Buffer[255];
va_list arg;
va_start(arg,pTrace);
vsprintf(Buffer,pTrace,arg);
#ifdef CPP
cerr << "(" << pFichier << " - " << Ligne << ") " << Buffer << endl;
#else
fprintf(stderr,"(%s - %d) %s\n",pFichier,Ligne,Buffer);
va_end(arg);
#endif
return ;
}

void Affiche(const char *pTrace, ... )
{
//#ifdef CPP
//#else
char Buffer[80];
va_list arg;
va_start(arg,pTrace);
vsprintf(Buffer,pTrace,arg);
printf("%s\n",Buffer);
fflush(stdout);
va_end(arg);
return ;
//#endif
}

void SortieErreur(char *p, int Rc)
{
#ifdef CPP
#else
perror(p);
exit(Rc);
#endif
}

void RestitueCurseur()
{
#ifdef CPP
#else
printf("\0338");
fflush(stdout);
#endif
}

void SauveCurseur()
{
#ifdef CPP
#else
printf("\0337");
fflush(stdout);
#endif
}
