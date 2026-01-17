/**
    * Fichier : ast.h
    * Description : Structure et fonctions pour l'arbre abstrait.
*/

#ifndef _AST_H_
#define _AST_H_


#include <stdio.h>
#include <stdlib.h>
#include "tab_lexico.h"
#include "couleurs.h"
#include "erreurs.h"


/* Instructions */
#define A_PROGRAMME           1
#define A_LISTE_INSTRUCTIONS  2   /* Liste d'instructions */
#define A_LISTE_ARGUMENTS     3   /* Liste d'arguments d'appel */
#define A_LISTE_INDICES       4   /* Liste d'indices de tableau */
#define A_LISTE_VARIABLES     5   /* Liste de variables (read/write) */

#define A_OPAFF               6   /* Affectation */
#define A_IF_THEN_ELSE        7   /* Condition */
#define A_WHILE               8   /* Boucle */
#define A_APPEL_PROC          9   /* Appel procédure */
#define A_APPEL_FCT          10   /* Appel fonction */
#define A_RETURN             11   /* Return */
#define A_LIRE               12   /* Read */
#define A_ECRIRE             13   /* Write */
#define A_VIDE               14   /* Instruction vide */

/* Expressions arithmétiques */
#define A_PLUS               15
#define A_MOINS              16
#define A_MULT               17
#define A_DIV                18
#define A_MOINS_UNAIRE       19

/* Expressions booléennes */
#define A_ET                 20
#define A_OU                 21
#define A_NON                22

/* Comparaisons */
#define A_EGAL               23
#define A_DIFF               24
#define A_INF                25
#define A_SUP                26
#define A_INF_EGAL           27
#define A_SUP_EGAL           28

/* Variables et accès */
#define A_IDF                29  /* Identificateur */
#define A_ACCES_TABLEAU      30  /* Accès tableau */
#define A_ACCES_CHAMP        31  /* Accès champ */


/* Constantes */
#define A_CSTE_ENT            32  /* Constante entière */
#define A_CSTE_REELLE         33  /* Constante réelle */
#define A_CSTE_BOOL           34  /* Constante booléenne */
#define A_CSTE_CHAR           35  /* Constante caractère */
#define A_CSTE_CHAINE         36  /* Constante chaine (pour write) */

#define A_CHAMP               37  /* Accès champ */
#define A_LISTE_CHAMPS        38  /* Liste de champs */


/* Structure d'un noeud d'arbre abstrait */
typedef struct noeud {
    int nature;
    int valeur;
    int num_declaration;
    int ligne; /* pour les erreurs sémantiques */
    int colonne; /* colone de début */
    int longueur; /* longueur du token */
    struct noeud* filsGauche;
    struct noeud* frereDroit;
} noeud;

/* Type arbre (pointeur vers noeud) */
typedef struct noeud* arbre;

/** 
  * Définit le numéro de déclaration d'un nœud
*/
void definir_declaration(arbre a, int num_decl);

/** 
 * Crée un nouveau noeud
*/
arbre creer_noeud(int nature, int valeur);

/** 
 * Attache fils comme fils gauche de pere 
*/
arbre concat_pere_fils(arbre pere, arbre fils);

/**
 * Attache frere comme frère droit de pere
 */
arbre concat_pere_frere(arbre pere, arbre frere);

/**  Trouve le dernier frère d'une chaîne (fonction utilitaire)
 */
arbre trouver_dernier_frere(arbre a);

/** 
 * Ajoute un élément à une liste
 */
arbre ajouter_element_liste(arbre liste_existante, arbre nouvel_element, int nature_liste);

/** 
 * une fonction pour débuguer les pointeurs d'un arbre
 */
void debug_pointeurs(arbre a, int profondeur);

/* on doit penser à gérer les cas des acces tableaux et champs (A_LISTE_, A_ACCES_CHAMP) */
 /**  
 * Trouve le dernier élément d'accès dans une chaîne d'accès (fonction utilitaire)
 */
arbre trouver_dernier_element_acces(arbre base);

/** 
 * pas très sûr que ça soit la solution optimale
 */
arbre ajouter_liste_acces(arbre base, arbre nouvelle_liste);


/* Sauvegarde la position d'un appel (ligne, colonne, longueur) */
void sauvegarder_position_appel(int ligne, int colonne, int longueur);

/** 
 * Conversion nature -> chaine
 */
const char* nature_noeud_vers_chaine(int nature);

/** 
 * Affiche l'arbre en profondeur (avec une fonction auxiliaire)
 */
void afficher_arbre(arbre racine);

/**
 * Libère la mémoire de l'arbre
 */
void liberer_arbre(arbre racine);



/** 
 * Sauvegarde l'arbre d'une région
 */
void sauvegarder_arbre_region(FILE* fichier, arbre racine, int num_region);



#endif /* _AST_H_ */