#include "Ressources.h"
#include "GrilleSDL.h"

// Macros pour les sprites
#define BILLE_GRISE                    300000
#define BILLE_JAUNE                    300001
#define BILLE_ROUGE                    300002
#define BILLE_VERTE                    300003
#define BILLE_VIOLETTE                 300004

#define STATUE_DOS                     300009
#define STATUE_DROITE_SANS_DIAMANT     300010
#define STATUE_DROITE_DIAMANT_JAUNE    300011
#define STATUE_DROITE_DIAMANT_ROUGE    300012
#define STATUE_DROITE_DIAMANT_VERT     300013
#define STATUE_DROITE_DIAMANT_VIOLET   300014
#define STATUE_GAUCHE_SANS_DIAMANT     300015
#define STATUE_GAUCHE_DIAMANT_JAUNE    300016
#define STATUE_GAUCHE_DIAMANT_ROUGE    300017
#define STATUE_GAUCHE_DIAMANT_VERT     300018
#define STATUE_GAUCHE_DIAMANT_VIOLET   300019
#define STATUE_FACE_SANS_DIAMANT       300020
#define STATUE_FACE_DIAMANT_JAUNE      300021
#define STATUE_FACE_DIAMANT_ROUGE      300022
#define STATUE_FACE_DIAMANT_VERT       300023
#define STATUE_FACE_DIAMANT_VIOLET     300024

#define MAGE_DOS                       300030
#define MAGE_DROITE_SANS_DIAMANT       300031
#define MAGE_DROITE_DIAMANT_JAUNE      300032
#define MAGE_DROITE_DIAMANT_ROUGE      300033
#define MAGE_DROITE_DIAMANT_VERT       300034
#define MAGE_DROITE_DIAMANT_VIOLET     300035
#define MAGE_GAUCHE_SANS_DIAMANT       300036
#define MAGE_GAUCHE_DIAMANT_JAUNE      300037
#define MAGE_GAUCHE_DIAMANT_ROUGE      300038
#define MAGE_GAUCHE_DIAMANT_VERT       300039
#define MAGE_GAUCHE_DIAMANT_VIOLET     300040
#define MAGE_FACE_SANS_DIAMANT         300041
#define MAGE_FACE_DIAMANT_JAUNE        300042
#define MAGE_FACE_DIAMANT_ROUGE        300043
#define MAGE_FACE_DIAMANT_VERT         300044
#define MAGE_FACE_DIAMANT_VIOLET       300045

#define MUR_PIERRE                     300050
#define MUR_METAL                      300051

#define PISTON_TETE                    300060
#define PISTON_TIGE                    300061

#define S_ZERO                         300070
#define S_UN                           300071
#define S_DEUX                         300072
#define S_TROIS                        300073
#define S_QUATRE                       300074
#define S_CINQ                         300075
#define S_SIX                          300076
#define S_SEPT                         300077
#define S_HUIT                         300078
#define S_NEUF                         300079

#define S_PRISON_JAUNE                 300080
#define S_PRISON_VERTE                 300081
#define S_CROIX                        300082

void ChargementImages()
{
  // Definition de l'image de fond
  DessineImageFond("./images/fond800x600.bmp");

  // Sprites
  AjouteSpriteAFondTransparent(BILLE_GRISE,"./images/BilleGrise40.bmp",255,255,255);
  AjouteSpriteAFondTransparent(BILLE_JAUNE,"./images/BilleJaune40.bmp",255,255,255);
  AjouteSpriteAFondTransparent(BILLE_ROUGE,"./images/BilleRouge40.bmp",255,255,255);
  AjouteSpriteAFondTransparent(BILLE_VERTE,"./images/BilleVerte40.bmp",255,255,255);
  AjouteSpriteAFondTransparent(BILLE_VIOLETTE,"./images/BilleViolette40.bmp",255,255,255);

  AjouteSpriteAFondTransparent(STATUE_DOS,"./images/StatueDos.bmp",255,255,255);

  AjouteSpriteAFondTransparent(STATUE_FACE_SANS_DIAMANT,"./images/StatueFaceSansDiamant.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_FACE_DIAMANT_JAUNE,"./images/StatueFaceDiamantJaune.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_FACE_DIAMANT_ROUGE,"./images/StatueFaceDiamantRouge.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_FACE_DIAMANT_VERT,"./images/StatueFaceDiamantVert.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_FACE_DIAMANT_VIOLET,"./images/StatueFaceDiamantViolet.bmp",255,255,255);

  AjouteSpriteAFondTransparent(STATUE_DROITE_SANS_DIAMANT,"./images/StatueDroiteSansDiamant.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_DROITE_DIAMANT_JAUNE,"./images/StatueDroiteDiamantJaune.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_DROITE_DIAMANT_ROUGE,"./images/StatueDroiteDiamantRouge.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_DROITE_DIAMANT_VERT,"./images/StatueDroiteDiamantVert.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_DROITE_DIAMANT_VIOLET,"./images/StatueDroiteDiamantViolet.bmp",255,255,255);

  AjouteSpriteAFondTransparent(STATUE_GAUCHE_SANS_DIAMANT,"./images/StatueGaucheSansDiamant.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_GAUCHE_DIAMANT_JAUNE,"./images/StatueGaucheDiamantJaune.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_GAUCHE_DIAMANT_ROUGE,"./images/StatueGaucheDiamantRouge.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_GAUCHE_DIAMANT_VERT,"./images/StatueGaucheDiamantVert.bmp",255,255,255);
  AjouteSpriteAFondTransparent(STATUE_GAUCHE_DIAMANT_VIOLET,"./images/StatueGaucheDiamantViolet.bmp",255,255,255);

  AjouteSpriteAFondTransparent(MAGE_DOS,"./images/MageDos.bmp",255,255,255);

  AjouteSpriteAFondTransparent(MAGE_FACE_SANS_DIAMANT,"./images/MageFaceSansDiamant.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_FACE_DIAMANT_JAUNE,"./images/MageFaceDiamantJaune.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_FACE_DIAMANT_ROUGE,"./images/MageFaceDiamantRouge.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_FACE_DIAMANT_VERT,"./images/MageFaceDiamantVert.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_FACE_DIAMANT_VIOLET,"./images/MageFaceDiamantViolet.bmp",255,255,255);

  AjouteSpriteAFondTransparent(MAGE_DROITE_SANS_DIAMANT,"./images/MageDroiteSansDiamant.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_DROITE_DIAMANT_JAUNE,"./images/MageDroiteDiamantJaune.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_DROITE_DIAMANT_ROUGE,"./images/MageDroiteDiamantRouge.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_DROITE_DIAMANT_VERT,"./images/MageDroiteDiamantVert.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_DROITE_DIAMANT_VIOLET,"./images/MageDroiteDiamantViolet.bmp",255,255,255);

  AjouteSpriteAFondTransparent(MAGE_GAUCHE_SANS_DIAMANT,"./images/MageGaucheSansDiamant.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_GAUCHE_DIAMANT_JAUNE,"./images/MageGaucheDiamantJaune.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_GAUCHE_DIAMANT_ROUGE,"./images/MageGaucheDiamantRouge.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_GAUCHE_DIAMANT_VERT,"./images/MageGaucheDiamantVert.bmp",255,255,255);
  AjouteSpriteAFondTransparent(MAGE_GAUCHE_DIAMANT_VIOLET,"./images/MageGaucheDiamantViolet.bmp",255,255,255);

  AjouteSprite(MUR_PIERRE,"./images/mur40.bmp");
  AjouteSprite(MUR_METAL,"./images/murMetallique40.bmp");

  AjouteSpriteAFondTransparent(PISTON_TETE,"./images/pistonA40_V2.bmp",255,255,255);
  AjouteSpriteAFondTransparent(PISTON_TIGE,"./images/pistonB40_V2.bmp",255,255,255);

  AjouteSpriteAFondTransparent(S_ZERO,"./images/Zero.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_UN,"./images/Un.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_DEUX,"./images/Deux.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_TROIS,"./images/Trois.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_QUATRE,"./images/Quatre.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_CINQ,"./images/Cinq.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_SIX,"./images/Six.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_SEPT,"./images/Sept.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_HUIT,"./images/Huit.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_NEUF,"./images/Neuf.bmp",255,255,255);

  AjouteSpriteAFondTransparent(S_PRISON_JAUNE,"./images/PrisonJaune40.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_PRISON_VERTE,"./images/PrisonVerte40.bmp",255,255,255);
  AjouteSpriteAFondTransparent(S_CROIX,"./images/Croix40.bmp",255,255,255);
}

// Fonctions Ressources
int OuvertureFenetreGraphique()
{
  if (OuvrirGrilleSDL(15,20,40,"The Loop","./images/IconeFenetre256.bmp") < 0)
    return -1;
 
  ChargementImages();
  //DessineSprite(19,14,S_NIVEAU);

  return 0;
}

int FermetureFenetreGraphique()
{
  FermerGrilleSDL();
  return 0;
}

void DessineBille(int l,int c,int couleur)
{
  switch(couleur)
  {
    case GRIS   : DessineSprite(l,c,BILLE_GRISE); break;
    case JAUNE  : DessineSprite(l,c,BILLE_JAUNE); break;
    case ROUGE  : DessineSprite(l,c,BILLE_ROUGE); break;
    case VERT   : DessineSprite(l,c,BILLE_VERTE); break;
    case VIOLET : DessineSprite(l,c,BILLE_VIOLETTE); break;
    default     : DessineSprite(l,c,BILLE_GRISE); break;    
  }
}

void DessineStatue(int l,int c,int dir,int couleur)
{
  if (dir == HAUT)
  {
    DessineSprite(l,c,STATUE_DOS);
    return;
  }

  if (dir == BAS)
  {
    switch(couleur)
    {
      case JAUNE  : DessineSprite(l,c,STATUE_FACE_DIAMANT_JAUNE); break;
      case ROUGE  : DessineSprite(l,c,STATUE_FACE_DIAMANT_ROUGE); break;
      case VERT   : DessineSprite(l,c,STATUE_FACE_DIAMANT_VERT); break;
      case VIOLET : DessineSprite(l,c,STATUE_FACE_DIAMANT_VIOLET); break;
      default     : DessineSprite(l,c,STATUE_FACE_SANS_DIAMANT); break;    
    }
    return;
  }

  if (dir == GAUCHE)
  {
    switch(couleur)
    {
      case JAUNE  : DessineSprite(l,c,STATUE_GAUCHE_DIAMANT_JAUNE); break;
      case ROUGE  : DessineSprite(l,c,STATUE_GAUCHE_DIAMANT_ROUGE); break;
      case VERT   : DessineSprite(l,c,STATUE_GAUCHE_DIAMANT_VERT); break;
      case VIOLET : DessineSprite(l,c,STATUE_GAUCHE_DIAMANT_VIOLET); break;
      default     : DessineSprite(l,c,STATUE_GAUCHE_SANS_DIAMANT); break;    
    }
    return;
  }

  if (dir == DROITE)
  {
    switch(couleur)
    {
      case JAUNE  : DessineSprite(l,c,STATUE_DROITE_DIAMANT_JAUNE); break;
      case ROUGE  : DessineSprite(l,c,STATUE_DROITE_DIAMANT_ROUGE); break;
      case VERT   : DessineSprite(l,c,STATUE_DROITE_DIAMANT_VERT); break;
      case VIOLET : DessineSprite(l,c,STATUE_DROITE_DIAMANT_VIOLET); break;
      default     : DessineSprite(l,c,STATUE_DROITE_SANS_DIAMANT); break;    
    }
    return;
  }
}

void DessineMage(int l,int c,int dir,int couleur)
{
  if (dir == HAUT)
  {
    DessineSprite(l,c,MAGE_DOS);
    return;
  }

  if (dir == BAS)
  {
    switch(couleur)
    {
      case JAUNE  : DessineSprite(l,c,MAGE_FACE_DIAMANT_JAUNE); break;
      case ROUGE  : DessineSprite(l,c,MAGE_FACE_DIAMANT_ROUGE); break;
      case VERT   : DessineSprite(l,c,MAGE_FACE_DIAMANT_VERT); break;
      case VIOLET : DessineSprite(l,c,MAGE_FACE_DIAMANT_VIOLET); break;
      default     : DessineSprite(l,c,MAGE_FACE_SANS_DIAMANT); break;    
    }
    return;
  }

  if (dir == GAUCHE)
  {
    switch(couleur)
    {
      case JAUNE  : DessineSprite(l,c,MAGE_GAUCHE_DIAMANT_JAUNE); break;
      case ROUGE  : DessineSprite(l,c,MAGE_GAUCHE_DIAMANT_ROUGE); break;
      case VERT   : DessineSprite(l,c,MAGE_GAUCHE_DIAMANT_VERT); break;
      case VIOLET : DessineSprite(l,c,MAGE_GAUCHE_DIAMANT_VIOLET); break;
      default     : DessineSprite(l,c,MAGE_GAUCHE_SANS_DIAMANT); break;    
    }
    return;
  }

  if (dir == DROITE)
  {
    switch(couleur)
    {
      case JAUNE  : DessineSprite(l,c,MAGE_DROITE_DIAMANT_JAUNE); break;
      case ROUGE  : DessineSprite(l,c,MAGE_DROITE_DIAMANT_ROUGE); break;
      case VERT   : DessineSprite(l,c,MAGE_DROITE_DIAMANT_VERT); break;
      case VIOLET : DessineSprite(l,c,MAGE_DROITE_DIAMANT_VIOLET); break;
      default     : DessineSprite(l,c,MAGE_DROITE_SANS_DIAMANT); break;    
    }
    return;
  }
}

void DessineMur(int l,int c,int type)
{
  switch(type)
  {
    case PIERRE : DessineSprite(l,c,MUR_PIERRE); break;
    case METAL  : DessineSprite(l,c,MUR_METAL); break;
    default     : DessineSprite(l,c,MUR_METAL); break;    
  }
}

void DessinePiston(int l,int c,int type)
{
  switch(type)
  {
    case TETE : DessineSprite(l,c,PISTON_TETE); break;
    case TIGE : DessineSprite(l,c,PISTON_TIGE); break;
    default     : DessineSprite(l,c,PISTON_TIGE); break;    
  }
}

void DessineChiffre(int l,int c,int chiffre)
{
  switch(chiffre)
  {
    case 0 : DessineSprite(l,c,S_ZERO); break;
    case 1 : DessineSprite(l,c,S_UN); break;
    case 2 : DessineSprite(l,c,S_DEUX); break;
    case 3 : DessineSprite(l,c,S_TROIS); break;
    case 4 : DessineSprite(l,c,S_QUATRE); break;
    case 5 : DessineSprite(l,c,S_CINQ); break;
    case 6 : DessineSprite(l,c,S_SIX); break;
    case 7 : DessineSprite(l,c,S_SEPT); break;
    case 8 : DessineSprite(l,c,S_HUIT); break;
    case 9 : DessineSprite(l,c,S_NEUF); break;
    default : DessineSprite(l,c,S_ZERO); break;    
  }
}

void DessinePrison(int l,int c,int couleur)
{
  switch(couleur)
  {
    case JAUNE : DessineSprite(l,c,S_PRISON_JAUNE); break;
    case VERT  : DessineSprite(l,c,S_PRISON_VERTE); break;
    default : DessineSprite(l,c,S_PRISON_JAUNE); break;
  }
}

void DessineCroix(int l,int c)
{
  DessineSprite(l,c,S_CROIX);
}

