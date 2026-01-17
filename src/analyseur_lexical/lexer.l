%{
       /**
        * Fichier : lexer.l
        * Auteur : Toute l'équipe
        * Description : Analyseur lexical du compilateur CPYRR
       */
#include "ast.h"
#include "construction.h"
#include "y.tab.h"
#include "erreurs.h"
#include "tab_lexico.h"
%}

%%

[ \t]+                         { cc += yyleng; }
\n                              { ligne_courante++; cc = 1; cdt = 1; ltc = 0; }



#.*                             { cc += yyleng; }


"/*"([^*]|\*+[^*/])*\*+"/"      {
    /* le cas de commentaires multi-lignes : 
    attention ici si un commentaire n'est pas fermé 
    -> erreur syntaxique au lieu de lexicale ... mais bon */
    int i;
    for (i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') {
            ligne_courante++;
            cc = 1;
        } else {
            cc++;
        }
    }
}


"prog"                           { cdt = cc; ltc = yyleng; cc += yyleng; return PROG; }
"var"                            { cdt = cc; ltc = yyleng; cc += yyleng; return VARIABLE; }
"type"                           { cdt = cc; ltc = yyleng; cc += yyleng; return TYPE; }
"struct"                         { cdt = cc; ltc = yyleng; cc += yyleng; return STRUCT; }
"endstruct"                      { cdt = cc; ltc = yyleng; cc += yyleng; return FSTRUCT; }
"array"                          { cdt = cc; ltc = yyleng; cc += yyleng; return TABLEAU; }
"of"                             { cdt = cc; ltc = yyleng; cc += yyleng; return DE; }
"procedure"                      { cdt = cc; ltc = yyleng; cc += yyleng; return PROCEDURE; }
"function"                       { cdt = cc; ltc = yyleng; cc += yyleng; return FONCTION; }
"return"                         { cdt = cc; ltc = yyleng; cc += yyleng; return RETOURNE; }
"void"                           { cdt = cc; ltc = yyleng; cc += yyleng; return VIDE; }
"read"                           { cdt = cc; ltc = yyleng; cc += yyleng; return LIRE; }
"write"                          { cdt = cc; ltc = yyleng; cc += yyleng; return ECRIRE; }

"int"                            { cdt = cc; ltc = yyleng; cc += yyleng; return ENTIER; }
"real"                           { cdt = cc; ltc = yyleng; cc += yyleng; return REEL; }
"char"                           { cdt = cc; ltc = yyleng; cc += yyleng; return CARACTERE; }
"bool"                           { cdt = cc; ltc = yyleng; cc += yyleng; return BOOLEEN; }
"string"                         { cdt = cc; ltc = yyleng; cc += yyleng; return CHAINE; }

"true"                           { cdt = cc; ltc = yyleng; cc += yyleng; return VRAI; }
"false"                          { cdt = cc; ltc = yyleng; cc += yyleng; return FAUX; }

"if"                             { cdt = cc; ltc = yyleng; cc += yyleng; return SI; }
"then"                           { cdt = cc; ltc = yyleng; cc += yyleng; return ALORS; }
"else"                           { cdt = cc; ltc = yyleng; cc += yyleng; return SINON; }
"while"                          { cdt = cc; ltc = yyleng; cc += yyleng; return TANT_QUE; }
"do"                             { cdt = cc; ltc = yyleng; cc += yyleng; return FAIRE; }

"{"                              { cdt = cc; ltc = yyleng; cc += yyleng; return DEBUT; }
"}"                              { cdt = cc; ltc = yyleng; cc += yyleng; return FIN; }

":"                              { cdt = cc; ltc = yyleng; cc += yyleng; return DEUX_POINTS; }
";"                              { cdt = cc; ltc = yyleng; cc += yyleng; return POINT_VIRGULE; }
":="                             { cdt = cc; ltc = yyleng; cc += yyleng; return OPAFF; }

"."                              { cdt = cc; ltc = yyleng; cc += yyleng; return POINT; }
"["                              { cdt = cc; ltc = yyleng; cc += yyleng; return CROCHET_OUVRANT; }
"]"                              { cdt = cc; ltc = yyleng; cc += yyleng; return CROCHET_FERMANT; }
"("                              { cdt = cc; ltc = yyleng; cc += yyleng; return PARENTHESE_OUVRANTE; }
")"                              { cdt = cc; ltc = yyleng; cc += yyleng; return PARENTHESE_FERMANTE; }
","                              { cdt = cc; ltc = yyleng; cc += yyleng; return VIRGULE; }
".."                             { cdt = cc; ltc = yyleng; cc += yyleng; return POINT_POINT; }

"+"                              { cdt = cc; ltc = yyleng; cc += yyleng; return PLUS; }
"-"                              { cdt = cc; ltc = yyleng; cc += yyleng; return MOINS; }
"*"                              { cdt = cc; ltc = yyleng; cc += yyleng; return MULT; }
"/"                              { cdt = cc; ltc = yyleng; cc += yyleng; return DIV; }

"&&"                             { cdt = cc; ltc = yyleng; cc += yyleng; return ET; }
"||"                             { cdt = cc; ltc = yyleng; cc += yyleng; return OU; }
"!"                              { cdt = cc; ltc = yyleng; cc += yyleng; return NON; }

"=="                             { cdt = cc; ltc = yyleng; cc += yyleng; return EGAL; }
"!="                             { cdt = cc; ltc = yyleng; cc += yyleng; return DIFFERENT; }
"<="                             { cdt = cc; ltc = yyleng; cc += yyleng; return INFERIEUR_EGAL; }
">="                             { cdt = cc; ltc = yyleng; cc += yyleng; return SUPERIEUR_EGAL; }
"<"                              { cdt = cc; ltc = yyleng; cc += yyleng; return INFERIEUR; }
">"                              { cdt = cc; ltc = yyleng; cc += yyleng; return SUPERIEUR; }

[a-zA-Z][a-zA-Z0-9]*  { 
    cdt = cc; ltc = yyleng; cc += yyleng;
    yylval.entier = inserer_lexeme(yytext, yyleng);
    return IDF; 
}

[0-9]+                { 
    cdt = cc; ltc = yyleng; cc += yyleng;
    yylval.entier = atoi(yytext);  
    return CSTE_ENTIERE;
}

[0-9]+\.[0-9]+  { 
    cdt = cc; ltc = yyleng; cc += yyleng;
    yylval.entier = inserer_lexeme(yytext, yyleng);
    return CSTE_REELLE; 
}

'[^']'  { 
    cdt = cc; ltc = yyleng; cc += yyleng;
    yylval.entier = yytext[1];
    return CSTE_CARACTERE; 
}

\"[^\"]*\"  { 
    cdt = cc; ltc = yyleng; cc += yyleng;
    yylval.entier = inserer_lexeme(yytext, yyleng);
    return CSTE_CHAINE; 
}

.                               { ajouter_erreur(ERR_LEXICAL, ligne_courante, "Caractère inconnu: '%s'", yytext); }

%%

int yywrap() {
    return 1;
}