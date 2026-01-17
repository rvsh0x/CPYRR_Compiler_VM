 /**
    * Fichier : charg_arbres.h
    * Description : fonctions utiles pour le chargement du fichier arbres.txt
*/

#ifndef _CHARG_ARBRES_H_
#define _CHARG_ARBRES_H_

#include "ast.h"

/* Initialise le chargement du fichier arbres.txt */
void init_chargement_arbres();

/* Traite l'en-tête d'une région (ligne "REGION X:") */
void traiter_entete_region_arbre(int num_region);

/* Traite un nœud : nature, num_lex, num_decl, nb_enfants
 * Retourne le nœud créé pour permettre le chaînage */
arbre traiter_noeud_arbre(const char* nature, int num_lex, int num_decl, int nb_enfants);

/* Finalise le chargement et vérifie la cohérence */
void finaliser_chargement_arbres();

/* Charge un fichier arbres.txt complet */
int charger_arbres(const char* chemin);

#endif /* _CHARG_ARBRES_H_ */
