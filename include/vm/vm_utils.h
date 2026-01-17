/**
 * Fichier : vm_utils.h
 * Auteur : LA MONICA Rayan & LAICHE Khayr Eddine & BENAMRAOUI Badis
 * Description : Fonctions utilitaires essentielles pour la machine virtuelle.
 */
#ifndef _VM_UTILS_H_
#define _VM_UTILS_H_

#include "vm_pile.h"
#include "ast.h"

/* Affiche une valeur selon son type */
void afficher_valeur(Valeur val, int type);

/* Applique une opération arithmétique binaire */
Valeur appliquer_operation_arithmetique(int operateur, Valeur val1, Valeur val2, int type);

/* Applique une négation arithmétique unaire (- unaire) */
Valeur appliquer_negation_unaire(Valeur val, int type);

/* Applique une opération de comparaison */
Valeur appliquer_comparaison(int operateur, Valeur val1, Valeur val2, int type);

/* Applique une opération booléenne (AND, OR) */
Valeur appliquer_operation_booleenne(int operateur, Valeur val1, Valeur val2);

/* Applique une négation booléenne (NOT) */
Valeur appliquer_negation_booleenne(Valeur val);

/* Trouve le num_decl du i-ème paramètre d'une région */
int trouver_parametre(int num_region, int index_param);

/* Fonctions pour chaînes de caractères */
int obtenir_taille_chaine(int type);
void copier_chaine(int adresse_dest, int adresse_source, int taille);
Valeur comparer_chaines(arbre noeud_gauche, arbre noeud_droite, int operateur);
void concatener_chaines_dans_destination(int adresse_dest, arbre noeud_plus, int taille_dest);

#endif /* _VM_UTILS_H_ */