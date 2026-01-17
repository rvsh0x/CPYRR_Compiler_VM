 /**
    * Fichier : charg_reg.h
    * Description : fonctions utiles pour le chargement du fichier regions.txt
*/
#ifndef _CHARG_REG_H_
#define _CHARG_REG_H_

/* Initialise le chargement du fichier regions.txt */
void init_chargement_regions();

/* Traite l'en-tête du fichier regions (ligne "nb_regions: N") */
void traiter_entete_regions(int nb_regions);

/* Traite une région (ligne "REGION index: nis=X taille=Y") */
void traiter_region(int index, int nis, int taille);

/* Finalise le chargement et vérifie la cohérence */
void finaliser_chargement_regions();

/* Charge un fichier regions.txt complet */
int charger_regions(const char* chemin);

#endif /* _CHARG_REG_H_ */