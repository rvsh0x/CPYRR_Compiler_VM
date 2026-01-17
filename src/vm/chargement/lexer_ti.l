%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "vm_parser_ti.tab.h"
#include "erreurs.h"
%}

%option noyywrap

%%

[ \t]+              { /* Ignorer espaces et tabulations */ }
\n                  { ligne_courante++; }
#.*                 { /* Ignorer commentaires */ }

nb_entrees          { return NB_ENTREES; }
nb_declarations     { return NB_DECL; }
nb_regions          { return NB_REGIONS; }

DECL                { return DECL; }
REGION              { return REGION; }

nature              { return NATURE; }
region              { return REGION_FIELD; }
description         { return DESCRIPTION; }
execution           { return EXECUTION; }

nis                 { return NIS; }
taille              { return TAILLE; }

nb_points_entree    { return NB_POINTS_ENTREE; }

POINT_ENTREE        { return POINT_ENTREE; }
type                { return TYPE; }
index               { return INDEX; }

STRUCT              { return STRUCT; }
nb_champs           { return NB_CHAMPS; }
CHAMP               { return CHAMP; }
num_lex             { return NUM_LEX; }
deplacement         { return DEPLACEMENT; }

ARRAY               { return ARRAY; }
type_elem           { return TYPE_ELEM; }
nb_dims             { return NB_DIMS; }
DIM                 { return DIM; }
borne_inf           { return BORNE_INF; }
borne_sup           { return BORNE_SUP; }

nb_params           { return NB_PARAMS; }
type_retour         { return TYPE_RETOUR; }

PROCEDURE           { return PROCEDURE; }
FONCTION            { return FONCTION; }
PARAMETRE           { return PARAMETRE; }

[SAPF]              { yylval.chaine = strdup(yytext); return TYPE_LETTRE; }

TYPE_BASE           { yylval.chaine = strdup(yytext); return NATURE_VAL; }
TYPE_STRUCT         { yylval.chaine = strdup(yytext); return NATURE_VAL; }
TYPE_ARRAY          { yylval.chaine = strdup(yytext); return NATURE_VAL; }
VAR                 { yylval.chaine = strdup(yytext); return NATURE_VAL; }
PARAM               { yylval.chaine = strdup(yytext); return NATURE_VAL; }
PROC                { yylval.chaine = strdup(yytext); return NATURE_VAL; }
FCT                 { yylval.chaine = strdup(yytext); return NATURE_VAL; }
A_[A-Z_]+           { yylval.chaine = strdup(yytext); return NATURE_ARBRE; }

":"                 { return DEUX_POINTS; }
"="                 { return EGAL; }

-?[0-9]+            { 
                        yylval.entier = atoi(yytext); 
                        return NOMBRE; 
                    }

[0-9]+\.[0-9]+      { 
                        yylval.chaine = strdup(yytext); 
                        return CHAINE; 
                    }

\"([^\"]|\\.)*\"    { 
                        yylval.chaine = strndup(yytext + 1, strlen(yytext) - 2);
                        return CHAINE; 
                    }

[a-zA-Z_][a-zA-Z0-9_]*  { 
                        yylval.chaine = strdup(yytext); 
                        return CHAINE; 
                    }

.                   { 
                        ajouter_erreur(ERR_LEXICAL, ligne_courante,
                                      "Caractere invalide: '%s'", yytext);
                    }

%%