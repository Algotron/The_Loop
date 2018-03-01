/* Exercice 1 :
   Denys MERCENIER
   Ecran.h
*/

#ifndef        _ECRAN_H_
#define        _ECRAN_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define        NORM      0
#define        GRAS      1

#ifdef CPP
#include <iostream>
using namespace std;
#else
#endif

#ifdef SUN
#include <sys/varargs.h>
#endif

#ifdef LINUX
#include <stdarg.h>
#endif


/* fonctions simplifiees de gestion d'ecran */

void EffEcran(void);
int LitChaine(char *,int,int,int,int);
int LitEntier(int *,int,int,int,int);
void AffChaine(const char *,int,int,int);
void Pause(void);
void Trace1(const char*,int,const char*,...);

#define Trace(...) Trace1(__FILE__,__LINE__,__VA_ARGS__)

void Affiche(const char *,...);
void SortieErreur(char *,int);
void SauveCurseur(void);
void RestitueCurseur(void);

#endif     /* _ECRAN_H_ */
