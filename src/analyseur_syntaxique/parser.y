%{
    /**
      * Fichier : parser.y
      * Auteur : Toute l'équipe
      * Description : Analyseur syntaxique du compilateur CPYRR
    */
#include <stdio.h>
#include <stdlib.h>
#include "erreurs.h"
#include "tab_lexico.h"
#include "tab_declarations.h"
#include "tab_representations.h"
#include "tab_regions.h"
#include "pile_regions.h"
#include "association_nom.h"
#include "ast.h"
#include "semantique.h"
#include "construction.h"
#include "utils_communs.h"

extern int yylex();
void yyerror(const char *s);

%}

%union {
    NoeudType noeud_type; /* pour les expressions ... Ont un TYPE qui doit être propagé */
    arbre a; /* pour les instructions qui effectuent des actions */
    int entier;
}

/* ---- Déclaration des tokens ---- */

/* Mots-clés du langage */
%token PROG VARIABLE TYPE
%token STRUCT FSTRUCT
%token TABLEAU DE
%token PROCEDURE FONCTION RETOURNE VIDE
%token LIRE ECRIRE
%token ENTIER REEL CARACTERE BOOLEEN CHAINE
%token DEBUT FIN
%token SI ALORS SINON TANT_QUE FAIRE

/* Symboles de ponctuation */
%token DEUX_POINTS POINT_VIRGULE OPAFF POINT
%token PARENTHESE_OUVRANTE PARENTHESE_FERMANTE
%token CROCHET_OUVRANT CROCHET_FERMANT
%token VIRGULE POINT_POINT

/* Opérateurs arithmétiques */
%token PLUS MOINS MULT DIV

/* Opérateurs booléens */
%token ET OU NON

/* Opérateurs de comparaison */
%token EGAL DIFFERENT INFERIEUR SUPERIEUR INFERIEUR_EGAL SUPERIEUR_EGAL

/* Identificateurs et constantes */
%token <entier> IDF
%token <entier> CSTE_ENTIERE CSTE_REELLE CSTE_CARACTERE CSTE_CHAINE
%token VRAI FAUX

%type <entier> nom_type type_simple suite_declaration_type
%type <a> corps 
%type <a> liste_instructions suite_liste_inst instruction
%type <a> affectation condition tant_que appel lecture ecriture
%type <a> format suite_ecriture
%type <noeud_type> resultat_retourne
%type <noeud_type> variable
%type <a> liste_variables liste_indices
%type <noeud_type> expression
%type <noeud_type> expression_booleenne expr_bool_et_cond expr_bool_fact_cond
%type <noeud_type> comparaison
%type <a> operateur_comp
%type <noeud_type> expr_arith expr_term expr_un expr_atom
%type <noeud_type> const_arith const_bool
%type <a> liste_arguments liste_args un_arg

/* Pour avoir une meilleure gestion des erreurs (avoir plus de détails) */
%define parse.error verbose

/* Point d'entrée de la grammaire */
%start programme

%%

/* ---- Structure générale du programme ---- */

programme
    : PROG 
      {
          initialiser_pile_regions();
          initialiser_tab_declarations();
          initialiser_tab_representation();
      }
      corps
      {
          associer_arbre_region(0, $3);
          finaliser_region_principale();
      }
    ;

corps
    : liste_decl_types liste_decl_variables liste_decl_proc_fonc liste_instructions
      {
          $$ = $4;
      }
    ;

/* ---- déclarations de types ---- */

liste_decl_types
    : /* vide */
    | liste_decl_types declaration_type POINT_VIRGULE
    ;

declaration_type
    : TYPE IDF DEUX_POINTS suite_declaration_type
      { finaliser_declaration_type($2, $4); }
    ;

/* ---- déclarations de variables ---- */

liste_decl_variables
    : /* vide */
    | liste_decl_variables declaration_variable POINT_VIRGULE
    ;

declaration_variable
    : VARIABLE IDF DEUX_POINTS nom_type
      {
          ajouter_variable($2, 
                          obtenir_region_courante(), 
                          $4);
      }
    ;

/* ---- déclarations de procédures/fonctions ---- */

liste_decl_proc_fonc
    : /* vide */
    | liste_decl_proc_fonc declaration_procedure POINT_VIRGULE
    | liste_decl_proc_fonc declaration_fonction POINT_VIRGULE
    ;

declaration_procedure
    : PROCEDURE IDF 
      { debut_declaration_procedure($2); }
      liste_parametres 
      { lier_procedure_representation(); }
      corps
      { fin_declaration_procedure($6); }
    ;

declaration_fonction
    : FONCTION IDF 
      { debut_declaration_fonction($2);}
      liste_parametres RETOURNE type_simple
      { lier_fonction_representation($6);}
      corps
      { fin_declaration_fonction($8);}
    ;

/* ---- les paramètres ---- */

liste_parametres
    : PARENTHESE_OUVRANTE liste_param PARENTHESE_FERMANTE
    ;

liste_param
    : 
    | un_param
    | liste_param POINT_VIRGULE un_param
    ;

un_param
    : IDF DEUX_POINTS type_simple
      {
          ajouter_parametre($1, obtenir_region_courante(), $3);
          ajouter_parametre_courant($1, $3);
      }
    ;

/* ---- Types ---- */

suite_declaration_type
    : STRUCT 
      {
          commencer_struct();
      }
      liste_champs FSTRUCT
      { 
          finaliser_struct();
          $$ = 1;
      }
    | TABLEAU 
      {
          commencer_array();
      }
      dimension DE nom_type
      { 
          finaliser_array($5);
          $$ = 2;
      }
    ;

liste_champs
    : un_champ
    | liste_champs POINT_VIRGULE un_champ
    ;

un_champ
    : IDF DEUX_POINTS nom_type
      {
          ajouter_champ_struct($1, $3);
      }
    ;

dimension
    : CROCHET_OUVRANT liste_dimensions CROCHET_FERMANT
    ;

liste_dimensions
    : une_dimension
    | liste_dimensions VIRGULE une_dimension
    ;

une_dimension
    : CSTE_ENTIERE POINT_POINT CSTE_ENTIERE
      { verifier_et_ajouter_dimension($1, $3); }
    ;

nom_type
    : type_simple  
      { 
          $$ = $1; 
      }
    | IDF          
      { $$ = resoudre_type($1); }
    ;

type_simple
    : ENTIER    { $$ = 0; }
    | REEL      { $$ = 1; }
    | BOOLEEN   { $$ = 2; }
    | CARACTERE { $$ = 3; }
    | CHAINE CROCHET_OUVRANT CSTE_ENTIERE CROCHET_FERMANT  { $$ = -1; }
    ;

/* ---- Liste des instructions ---- */

liste_instructions
    : DEBUT suite_liste_inst FIN
      {
          $$ = $2;
      }
    ;

suite_liste_inst
    : instruction POINT_VIRGULE
      {
          /* $$ = concat_pere_fils(creer_noeud(A_LISTE_INSTRUCTIONS, -1), concat_pere_frere($1,creer_noeud(A_LISTE_INSTRUCTIONS,-1))); */
           $$ = ajouter_element_liste(NULL, $1, A_LISTE_INSTRUCTIONS);
      }
    | suite_liste_inst instruction POINT_VIRGULE
      {
          /* $$ = concat_pere_fils(creer_noeud(A_LISTE_INSTRUCTIONS, -1),
                               concat_pere_frere($2, $1)); */
          $$ = ajouter_element_liste($1, $2, A_LISTE_INSTRUCTIONS);
      }
    ;

instruction
    : affectation
      {
          $$ = $1;
      }
    | condition
      {
          $$ = $1;
      }
    | tant_que
      {
          $$ = $1;
      }
    | appel
      {
          $$ = $1;
      }
    | RETOURNE resultat_retourne
      {
          $$ = verifier_et_construire_return($2);
      }
    | VIDE
      {
          $$ = creer_noeud(A_VIDE, -1);
      }
    | lecture
      {
          $$ = $1;
      }
    | ecriture
      {
          $$ = $1;
      }
    ;

affectation
    : variable OPAFF expression
      {
          $$ = verifier_affectation($1, $3);
      }
    ;

condition
    : SI expression_booleenne ALORS liste_instructions SINON liste_instructions
      {
          /* Vérifier et avertir si condition constante */
          $$ = construire_condition($2, $4, $6);
      }
    ;
tant_que
    : TANT_QUE expression_booleenne FAIRE liste_instructions
      {
          /* Vérifier et avertir si condition constante */
          $$ = construire_boucle($2, $4);
      }
    ;

/* ---- Entrées/Sorties ---- */

lecture
    : LIRE PARENTHESE_OUVRANTE liste_variables PARENTHESE_FERMANTE
      {
          /* Vérifier que les variables sont de type simple */
          $$ = verifier_et_construire_lecture($3);
      }
    ;

liste_variables
    : variable
      {
          $$ = ajouter_element_liste(NULL, $1.tarbre, A_LISTE_VARIABLES);
      }
    | liste_variables VIRGULE variable
      {
          $$ = ajouter_element_liste($1, $3.tarbre, A_LISTE_VARIABLES);
      }
    ;

ecriture
    : ECRIRE PARENTHESE_OUVRANTE format suite_ecriture PARENTHESE_FERMANTE
      {
          /* Vérifier correspondance format/variables */
          $$ = verifier_et_construire_ecriture($3, $4);
      }
    ;

format
    : CSTE_CHAINE
      {
          $$ = creer_noeud(A_CSTE_CHAINE, $1);
      }
    ;

suite_ecriture
    :
      {
          $$ = NULL;
      }
    | suite_ecriture VIRGULE variable
      {
          $$ = ajouter_element_liste($1, $3.tarbre, A_LISTE_VARIABLES);
      }
    ;
/* ---- Appels de fonctions ---- */

appel
    : IDF liste_arguments
      {
          /* Construire l'appel ET vérifier nombre et types des arguments */
          $$ = verifier_et_construire_appel($1, $2);
      }
    ;

liste_arguments
    : PARENTHESE_OUVRANTE liste_args PARENTHESE_FERMANTE
      {
          $$ = $2;
      }
    ;

liste_args
    :
      {
          $$ = NULL;
      }
    | un_arg
      {
          $$ = ajouter_element_liste(NULL, $1, A_LISTE_ARGUMENTS);
      }
    | liste_args VIRGULE un_arg
      {
          $$ = ajouter_element_liste($1, $3, A_LISTE_ARGUMENTS);
      }
    ;

un_arg
    : expression
      {
          /* Extraire .tarbre pour la liste d'arguments */
          $$ = $1.tarbre;
      }
    ;

resultat_retourne
    :
      {
          $$.tarbre = NULL;
      }
    | expression
      {
          /* Extraire .tarbre pour le return */
          $$ = $1;
      }
    ;
/* ---- Variables ---- */

variable
    : IDF
      { 
          $$ = construire_variable_simple($1, ligne_courante, cdt, ltc);
      }
    
    | variable POINT IDF
      {
          $$ = verifier_et_construire_acces_champ($1, $3, 
                                                  ligne_courante, cdt, ltc);
      }

    | variable CROCHET_OUVRANT liste_indices CROCHET_FERMANT
      {
          $$ = verifier_et_construire_acces_tableau($1, $3,
                                                    ligne_courante, cdt, ltc);
      }

liste_indices
    : expr_arith
      {
          $$ = ajouter_element_liste(NULL, $1.tarbre, A_LISTE_INDICES);
      }
    | liste_indices VIRGULE expr_arith
      {
          $$ = ajouter_element_liste($1, $3.tarbre, A_LISTE_INDICES);
      }
    ;

/* ---- Expressions ---- */

expression
    : expr_arith
      {
          /* Propager expression arithmétique */
          $$ = $1;
      }
    | expression_booleenne
      {
          /* Propager expression booléenne */
          $$ = $1;
      }
    ;

/* Expressions booléennes */
expression_booleenne
    : expression_booleenne OU expr_bool_et_cond
      {
          $$ = construire_operation_bool($1, $3, A_OU,
                                        ligne_courante, cdt, ltc);
      }
    | expr_bool_et_cond
      {
          $$ = $1;
      }
    ;

expr_bool_et_cond
    : expr_bool_et_cond ET expr_bool_fact_cond
      {
          $$ = construire_operation_bool($1, $3, A_ET, 
                                        ligne_courante, cdt, ltc);
      }
    | expr_bool_fact_cond
      {
          $$ = $1;
      }
    ;


expr_bool_fact_cond
    : NON expr_bool_fact_cond
      {
          $$ = construire_negation($2, ligne_courante, cdt, ltc);
      }
    | PARENTHESE_OUVRANTE expression_booleenne PARENTHESE_FERMANTE
      {
          $$ = $2;
      }
    | comparaison
      {
          $$ = $1;
      }
    | const_bool
      {
          $$ = $1;
      }
    ;

const_bool
    : VRAI
      {
          $$ = construire_constante(A_CSTE_BOOL, 1,
                                   ligne_courante, cdt, ltc);
      }
    | FAUX
      {
          $$ = construire_constante(A_CSTE_BOOL, 0,
                                   ligne_courante, cdt, ltc);
      }
    ;


comparaison
    : expr_arith operateur_comp expr_arith
      {
          /* Vérifier que les types sont compatibles AVANT de construire */
          $$ = verifier_comparaison($1, $3, $2->nature, 
                                   ligne_courante, cdt, ltc);
      }
    | expr_arith EGAL const_bool
      {
          /* Cas spécial : comparer expr_arith avec const_bool */
          $$ = verifier_comparaison($1, $3, A_EGAL,
                                   ligne_courante, cdt, ltc);
      }
    ;

operateur_comp
    : EGAL
      {
          $$ = creer_noeud(A_EGAL, -1);
      }
    | DIFFERENT
      {
          $$ = creer_noeud(A_DIFF, -1);
      }
    | INFERIEUR
      {
          $$ = creer_noeud(A_INF, -1);
      }
    | SUPERIEUR
      {
          $$ = creer_noeud(A_SUP, -1);
      }
    | INFERIEUR_EGAL
      {
          $$ = creer_noeud(A_INF_EGAL, -1);
      }
    | SUPERIEUR_EGAL
      {
          $$ = creer_noeud(A_SUP_EGAL, -1);
      }
    ;

expr_arith
    : expr_arith PLUS expr_term
      {
          $$ = verifier_operation_arith($1, $3, A_PLUS, 
                                       ligne_courante, cdt, ltc);
      }
    | expr_arith MOINS expr_term
      {
          $$ = verifier_operation_arith($1, $3, A_MOINS,
                                       ligne_courante, cdt, ltc);
      }
    | expr_term
      {
          $$ = $1;
      }
    ;

expr_term
    : expr_term MULT expr_un
      {
          $$ = verifier_operation_arith($1, $3, A_MULT,
                                       ligne_courante, cdt, ltc);
      }
    | expr_term DIV expr_un
      {
          $$ = verifier_operation_arith($1, $3, A_DIV,
                                       ligne_courante, cdt, ltc);
      }
    | expr_un
      {
          $$ = $1;
      }
    ;

expr_un
    : PLUS expr_un
      {
          $$ = $2;
      }
    | MOINS expr_un
      {
          $$ = construire_moins_unaire($2, ligne_courante, cdt, ltc);
      }
    | expr_atom
      {
          $$ = $1;
      }
    ;

expr_atom
    : const_arith
      {
          $$ = $1;
      }
    | variable
      {
          $$ = $1;
      }
    | appel
      {
          $$ = construire_noeud_depuis_appel($1, ligne_courante, cdt, ltc);
      }
    | PARENTHESE_OUVRANTE expr_arith PARENTHESE_FERMANTE
      {
          $$ = $2;
      }
    ;

const_arith
    : CSTE_ENTIERE
      {
          $$ = construire_constante(A_CSTE_ENT, $1, 
                                   ligne_courante, cdt, ltc);
      }
    | CSTE_REELLE
      {
          $$ = construire_constante(A_CSTE_REELLE, $1,
                                   ligne_courante, cdt, ltc);
      }
    | CSTE_CARACTERE
      {
          $$ = construire_constante(A_CSTE_CHAR, $1,
                                   ligne_courante, cdt, ltc);
      }
    ;
%%

/* ---- Fonction d'erreur ---- */

void yyerror(const char *s) {
    ajouter_erreur(ERR_SYNTAXIQUE, ligne_courante, "%s", s);
}