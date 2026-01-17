/* 
    Fichier : utils_communs.h
    Description : Fonctions utilitaires communes entre compilateur et machine virtuelle
*/

#ifndef _UTILS_COMMUNS_H_
#define _UTILS_COMMUNS_H_

#include "ast.h"
#include "tab_declarations.h"
#include "tab_representations.h"



/* Calcule le type d'un nœud de l'arbre */
int obtenir_type_noeud(arbre a);

/* pour compter les %d , %f , %c  ... */
int compter_specs(const char* format);

/* Obtenir le nom complet d'une variable (avec champs) */
const char* obtenir_nom_variable(arbre a);

/* Vérifie si un type est une chaîne de caractères */
int est_type_chaine(int type);

#endif /* _UTILS_COMMUNS_H_ */