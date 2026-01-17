/**
 * Fichier : vm_expressions.h
 * Description : Auxiliaires évaluation
 */
#ifndef _VM_EXPRESSIONS_H_
#define _VM_EXPRESSIONS_H_

#include "ast.h"
#include "vm_pile.h"

/* Fonctions auxiliaires évaluation */
Valeur evaluer_constante(arbre a);
Valeur evaluer_variable(arbre a);
Valeur evaluer_arbre(arbre a);
void preparer_appel_fonction(arbre noeud);
Valeur evaluer_appel_fonction(arbre noeud);

/* Calcul de l'adresse complète d'une variable avec accès */
int calculer_adresse_complete(arbre noeud_idf);

#endif /* _VM_EXPRESSIONS_H_ */ 