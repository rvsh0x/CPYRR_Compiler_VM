%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "vm/charg_lex.h"
#include "vm/charg_decl.h"
#include "vm/charg_reg.h"
#include "vm/charg_rep.h"
#include "vm/charg_arbres.h"
#include "erreurs.h"

extern int yylex();
void yyerror(const char *s);
%}

%union {
    int entier;
    char* chaine;
    arbre noeud;
}

%token NB_ENTREES NB_DECL NB_REGIONS
%token DECL REGION
%token DEUX_POINTS EGAL
%token NATURE REGION_FIELD DESCRIPTION EXECUTION
%token NIS TAILLE
%token <entier> NOMBRE
%token <chaine> CHAINE NATURE_VAL

%token NB_POINTS_ENTREE
%token POINT_ENTREE TYPE INDEX
%token STRUCT NB_CHAMPS CHAMP NUM_LEX DEPLACEMENT
%token ARRAY TYPE_ELEM NB_DIMS DIM BORNE_INF BORNE_SUP
%token NB_PARAMS TYPE_RETOUR
%token PROCEDURE FONCTION PARAMETRE 
%token <chaine> TYPE_LETTRE
%token <chaine> NATURE_ARBRE


%start fichier

%%

fichier
    : fichier_lexique /* { printf(" DEBUG : je suis ici ... Fin du parsing du fichier lexique\n"); } */
    | fichier_declarations /* { printf(" DEBUG : je suis ici ... Fin du parsing du fichier declarations\n"); } */
    | fichier_regions /* { printf(" DEBUG : je suis ici ... Fin du parsing du fichier regions\n"); } */
    | fichier_representations /* { printf(" DEBUG : je suis ici ... Fin du parsing du fichier representations\n"); } */
    | fichier_arbres /* { printf(" DEBUG : je suis ici ... Fin du parsing du fichier arbres\n"); } */
    ;

/* ----------- Lexique ------------ */

fichier_lexique
    : entete_lexique liste_entrees
    ;

entete_lexique
    : NB_ENTREES DEUX_POINTS NOMBRE
      { traiter_entete_lexique($3); }
    ;

liste_entrees
    : 
    | liste_entrees une_entree
    ;

une_entree
    : NOMBRE DEUX_POINTS CHAINE
      { 
          traiter_entree_lexique($1, $3); 
          free($3); 
      }
    ;

/* ----------- Declaration ------------ */

fichier_declarations
    : entete_declarations liste_decls
    ;

entete_declarations
    : NB_DECL DEUX_POINTS NOMBRE
      { traiter_entete_declarations($3); }
    ;

liste_decls
    : 
    | liste_decls une_decl
    ;

une_decl
    : DECL NOMBRE DEUX_POINTS 
      NATURE EGAL NATURE_VAL
      REGION_FIELD EGAL NOMBRE
      DESCRIPTION EGAL NOMBRE
      EXECUTION EGAL NOMBRE
      { 
          traiter_declaration($2, $6, $9, $12, $15); 
          if ($6 != NULL) free($6);
      }
    ;

/* ----------- Regions ------------ */

fichier_regions
    : entete_regions liste_regions
    ;

entete_regions
    : NB_REGIONS DEUX_POINTS NOMBRE
      { traiter_entete_regions($3); }
    ;

liste_regions
    : 
    | liste_regions une_region
    ;

une_region
    : REGION NOMBRE DEUX_POINTS
      NIS EGAL NOMBRE
      TAILLE EGAL NOMBRE
      { traiter_region($2, $6, $9); }
    ;

/* ----------- Representations ------------ */

fichier_representations
    : entete_representations liste_entrees_avec_blocs
      { finaliser_chargement_representations(); }
    ;

entete_representations
    : NB_ENTREES DEUX_POINTS NOMBRE
      NB_POINTS_ENTREE DEUX_POINTS NOMBRE
      { traiter_entete_representations($3, $6); }
    ;

liste_entrees_avec_blocs
    : 
    | liste_entrees_avec_blocs entree_avec_bloc
    ;

entree_avec_bloc
    : un_point_entree bloc_representation
    ;

un_point_entree
    : POINT_ENTREE NOMBRE DEUX_POINTS
      TYPE EGAL TYPE_LETTRE
      INDEX EGAL NOMBRE
      { 
          traiter_point_entree($2, $6[0], $9);
          if ($6 != NULL) free($6);
      }
    ;

bloc_representation
    : bloc_struct
    | bloc_array
    | bloc_proc
    | bloc_fct
    ;

/* Structures : */

bloc_struct
    : entete_struct liste_champs
      { traiter_struct_fin(); }
    ;

entete_struct
    : STRUCT NOMBRE DEUX_POINTS
      NB_CHAMPS EGAL NOMBRE
      { traiter_struct_debut($2, $6); }
    ;

liste_champs
    : 
    | liste_champs un_champ
    ;

un_champ
    : CHAMP NOMBRE DEUX_POINTS
      NUM_LEX EGAL NOMBRE
      TYPE EGAL NOMBRE
      DEPLACEMENT EGAL NOMBRE
      { traiter_champ($2, $6, $9, $12); }
    ;

/* Array : */

bloc_array
    : entete_array liste_dims
      { traiter_array_fin(); }
    ;

entete_array
    : ARRAY NOMBRE DEUX_POINTS
      TYPE_ELEM EGAL NOMBRE
      NB_DIMS EGAL NOMBRE
      { traiter_array_debut($2, $6, $9); }
    ;

liste_dims
    : 
    | liste_dims une_dim
    ;

une_dim
    : DIM NOMBRE DEUX_POINTS
      BORNE_INF EGAL NOMBRE
      BORNE_SUP EGAL NOMBRE
      { traiter_dim($2, $6, $9); }
    ;

/* Procedures : */

bloc_proc
    : entete_proc liste_params_proc
      { traiter_proc_fin(); }
    ;

entete_proc
    : PROCEDURE NOMBRE DEUX_POINTS
      NB_PARAMS EGAL NOMBRE
      { traiter_proc_debut($2, $6); }
    ;

liste_params_proc
    : /* vide */
    | liste_params_proc PARAMETRE NOMBRE DEUX_POINTS
      NUM_LEX EGAL NOMBRE
      TYPE EGAL NOMBRE
      { traiter_param($3, $7, $10); }
    ;

/* Fonctions : */

bloc_fct
    : entete_fct liste_params_fct
      { traiter_fct_fin(); }
    ;

entete_fct
    : FONCTION NOMBRE DEUX_POINTS
      TYPE_RETOUR EGAL NOMBRE
      NB_PARAMS EGAL NOMBRE
      { traiter_fct_debut($2, $6, $9); }
    ;

liste_params_fct
    : /* vide */
    | liste_params_fct PARAMETRE NOMBRE DEUX_POINTS
      NUM_LEX EGAL NOMBRE
      TYPE EGAL NOMBRE
      { traiter_param($3, $7, $10); }
    ;

/* ----------- Arbres ------------ */

fichier_arbres
    : liste_regions_arbres
    ;

liste_regions_arbres
    : /* vide */
    | liste_regions_arbres une_region_arbre
    ;

une_region_arbre
    : entete_region_arbre liste_noeuds_region
    ;

entete_region_arbre
    : REGION NOMBRE DEUX_POINTS
      {
          traiter_entete_region_arbre($2); 
      }
    ;

liste_noeuds_region
    : noeud
    | liste_noeuds_region noeud
    ;

noeud
    : NATURE_ARBRE NOMBRE NOMBRE NOMBRE
      {
          traiter_noeud_arbre($1, $2, $3, $4);
          free($1);
      }
    ;

%%

void yyerror(const char *s) {
    ajouter_erreur(ERR_SYNTAXIQUE, ligne_courante,
                  "Erreur syntaxe: %s", s);
}