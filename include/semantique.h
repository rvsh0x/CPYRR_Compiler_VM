/**
    * Fichier : semantique.h
    * Description : Structure et fonctions pour l'analyse sémantique.
*/

#ifndef _SEMANTIQUE_H_
#define _SEMANTIQUE_H_

#include "ast.h"
#include "construction.h"
#include "association_nom.h"
#include "tab_lexico.h"
#include "tab_representations.h"
#include "tab_declarations.h"
#include "tab_regions.h"
#include "association_nom.h"
#include "erreurs.h"


/* UTILITAIRES COMMUNS (sem_common.c) */

/* Vérifie si type de base (0,1,2,3) */
int est_type_simple(int type);

/* Vérifie si numérique (0 ou 1) */
int est_type_numerique(int type);

/* Retourne le nom d'un type */
const char* nom_type(int type);

/* pour supporter les chaînes de caractères */
int chaines_meme_taille(int type1, int type2);


/* --------------------------- */
/* Résout un type utilisateur, retourne index ou -1 avec erreur */
int resoudre_type(int num_lex);

/* Vérifie qu'une variable est déclarée, ajoute erreur sinon */
int verifier_variable(int num_lex);

/* Crée un noeud pour un appel de fonction/procédure */
arbre creer_noeud_appel(int num_lex, arbre arguments);

/* Vérifie les variables non utilisées */
void verifier_variables_non_utilisees();

/* Vérifie bornes et ajoute dimension */
void verifier_et_ajouter_dimension(int borne_inf, int borne_sup);

/* --------------------------- */
void verifier_region_fonction(int num_region, arbre corps);

/* Analyse sémantique */
void analyser_semantique();

/* Les fonctions modifiées de la semantique */

/**
 * Vérification et construction d'opération arithmétique
 */
NoeudType verifier_operation_arith(NoeudType gauche, NoeudType droite,
                                   int op, int ligne, int col, int len);
/**
 * Vérification et construction d'affectation
 */
arbre verifier_affectation(NoeudType destination, NoeudType source);

/** 
 * Verifie comparaisons (=, <>, <, >, <=, >=) 
 */
NoeudType verifier_comparaison(NoeudType gauche, NoeudType droite, 
                               int operateur, int ligne, int col, int len);

/**
 * Vérification et construction d'accès à un champ de structure
 */
NoeudType verifier_et_construire_acces_champ(NoeudType variable, int num_lex_champ,
                                             int ligne, int col, int len);

/**
 * Vérification et construction d'accès à un tableau
 */
NoeudType verifier_et_construire_acces_tableau(NoeudType variable, arbre liste_indices,
                                               int ligne, int col, int len);
                          

/**
 * Construction et vérification de condition if/then/else ( avec les warnings ici ...)
 */
arbre construire_condition(NoeudType condition, arbre bloc_alors, arbre bloc_sinon);

/**
 * Construction et vérification de boucle while ( avec les warnings ici ...)
 */
arbre construire_boucle(NoeudType condition, arbre corps);

/** 
 * Vérification et construction d'un appel de fonction/procédure 
 */
arbre verifier_et_construire_appel(int num_lex, arbre liste_arguments);

/** 
 * Vérification et construction d'un return
 */
arbre verifier_et_construire_return(NoeudType expression_retour);

/** 
 * Vérification et construction d'instruction LIRE
 */
arbre verifier_et_construire_lecture(arbre liste_variables);

/** 
 * Vérification et construction d'instruction ECRIRE
 */
arbre verifier_et_construire_ecriture(arbre format, arbre liste_variables);

#endif /* _SEMANTIQUE_H_ */