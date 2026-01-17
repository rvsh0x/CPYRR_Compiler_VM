/**
 * Fichier : vm_exec.h
 * Description : Gestion de l'exécution de la machine virtuelle
 */

#ifndef _VM_EXEC_H_
#define _VM_EXEC_H_

#include "ast.h"
#include "vm_pile.h"


/* Point d'entrée VM */
void interpreter();

/** 
 * Fonctions principales : 
 * ces fonctions sont respectivement dans les fichiers vm_instructions.c et vm_evaluation.c 
 * Tout le groupe a participé à leur développement car elles contiennent plusieurs cas ... 
*/
void executer_arbre(arbre a);
Valeur evaluer_arbre(arbre a);

#endif /* _VM_EXEC_H_ */