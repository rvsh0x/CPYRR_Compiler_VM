/**
 * Fichier : vm_instructions.h
 * Description : Auxiliaires instructions
 */

#ifndef _VM_INSTRUCTIONS_H_
#define _VM_INSTRUCTIONS_H_

#include "ast.h"
#include "vm_pile.h"

/* Fonctions auxiliaires instructions */
void executer_affectation(arbre a);
void traiter_affectation_avec_fonction(arbre noeud_affectation, arbre noeud_fonction);
void executer_ecriture(arbre a);
void executer_lecture(arbre a);
void executer_si(arbre a);
void executer_tantque(arbre a);
void executer_retour(arbre a);
void preparer_appel_procedure(arbre noeud);
void traiter_liste_instructions(arbre liste);


#endif /* _VM_INSTRUCTIONS_H_ */