 /**
    * Fichier : tab_declarations.h
    * Description : Structure et fonctions pour la table des déclarations.
*/

#ifndef _TAB_DECLARATIONS_H_
#define _TAB_DECLARATIONS_H_

#include <stdio.h>
#include "tab_lexico.h"  /* Pour MAX_LEXEMES */
#include "tab_representations.h"
#include "tab_regions.h"
#include "pile_regions.h"
#include "erreurs.h"
#include "couleurs.h"


#define MAX_DECLARATIONS 5000

/* Nature des déclarations */
typedef enum {
    TYPE_BASE = 0,
    TYPE_STRUCT = 1,
    TYPE_ARRAY = 2,
    NATURE_VAR = 3,
    NATURE_PARAM = 4,
    NATURE_PROC = 5,
    NATURE_FCT = 6
} Nature;

/* Structure d'une déclaration */
typedef struct {
    Nature nature;
    int suivant;
    int region;
    int description;
    int execution;
} Declaration;

/* [0..MAX_LEXEMES-1] = table primaire */
/* [MAX_LEXEMES..MAX_DECLARATIONS-1] = zone débordement */
extern Declaration tab_declarations[MAX_DECLARATIONS];
extern int prochaine_case_libre;  /* Compteur pour zone débordement */

/* Initialise et pré-remplit 0-3 */
void initialiser_tab_declarations();

/* Ajoute une variable */
int ajouter_variable(int num_lex, int region, int type_index);

/* Ajoute une procédure */
int ajouter_procedure(int num_lex, int region);

/* Ajoute une fonction */
int ajouter_fonction(int num_lex, int region);

/* Ajoute un type struct */
int ajouter_type_struct(int num_lex, int region);

/* Ajoute un type array */
int ajouter_type_array(int num_lex, int region);

/* Ajoute un paramètre */
int ajouter_parametre(int num_lex, int region, int type_index);

/* Cherche dans UNE région -> premiere version de association_nom */
int chercher_dans_region(int num_lex, int region, Nature nature);

/* Retourne la nature */
Nature obtenir_nature(int index);

/* Retourne le type */
int obtenir_type(int index);

/* Affiche la table */
void afficher_tab_declarations();

/* Convertit nature en chaîne */
const char* nature_decl_vers_chaine(Nature nature);

/* Quelques ajouts après corrections : */
/* Déclaration de type complète ->  */
void finaliser_declaration_type(int num_lex, int type_nature);

/* Procédure : début, milieu, fin */
void debut_declaration_procedure(int num_lex);
void lier_procedure_representation();
void fin_declaration_procedure(arbre a);

/* Fonction : début, milieu, fin */
void debut_declaration_fonction(int num_lex);
void lier_fonction_representation(int type_retour);
void fin_declaration_fonction(arbre a);

/* Paramètre complet */
void declarer_parametre(int num_lex, int type);

/* Calculer la taille d'un type */
int calculer_taille_type(int type_index);

/* Finaliser la région principale */
void finaliser_region_principale();

/* Sauvegarde la table des déclarations */
int sauvegarder_declarations(const char* chemin_fichier);

#endif /* _TAB_DECLARATIONS_H_ */