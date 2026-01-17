#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm/charg_lex.h"
#include "tab_lexico.h"

static int nb_attendu = 0;
static int nb_charge = 0;


void init_chargement_lexique(void) {
    nb_attendu = 0;
    nb_charge = 0;
    nb_lexemes = 0;
}


void traiter_entete_lexique(int nb_entrees) {
    if (nb_entrees < 0 || nb_entrees > MAX_LEXEMES) {
        fprintf(stderr, "Erreur: nombre d'entrees invalide (%d)\n", nb_entrees);
        return;
    }
    nb_attendu = nb_entrees;
}


char* traiter_chaine_echappee(const char* str, int longueur) {
    char* resultat;
    int i;
    int j;
    
    resultat = (char*)malloc(longueur + 1);
    if (resultat == NULL) {
        fprintf(stderr, "Erreur: allocation memoire dans traiter_chaine_echappee\n");
        exit(EXIT_FAILURE);
    }
    
    i = 0;
    j = 0;
    while (i < longueur) {
        if (str[i] == '\\' && i + 1 < longueur) {
            i++;
            
            if (str[i] == 'n') {
                resultat[j] = '\n';
            } else if (str[i] == 't') {
                resultat[j] = '\t';
            } else if (str[i] == 'r') {
                resultat[j] = '\r';
            } else if (str[i] == '\\') {
                resultat[j] = '\\';
            } else if (str[i] == '"') {
                resultat[j] = '"';
            } else {
                /* Échappement non reconnu, garder tel quel */
                resultat[j] = '\\';
                j++;
                resultat[j] = str[i];
            }
            
            j++;
            i++;
        } else {
            resultat[j] = str[i];
            j++;
            i++;
        }
    }
    
    resultat[j] = '\0';
    return resultat;
}


void traiter_entree_lexique(int index, const char* lexeme) {
    char* lexeme_traite;
    
    if (nb_charge >= MAX_LEXEMES) {
        fprintf(stderr, "Erreur: table lexique pleine\n");
        return;
    }
    
    if (lexeme == NULL) {
        fprintf(stderr, "Erreur: lexeme NULL\n");
        return;
    }
    
    /* Traiter les séquences d'échappement */
    lexeme_traite = traiter_chaine_echappee(lexeme, strlen(lexeme));
    
    /* Copier dans la table */
    strncpy(tab_lexico[nb_charge].lexeme, lexeme_traite, MAX_LONGUEUR_LEXEME - 1);
    tab_lexico[nb_charge].lexeme[MAX_LONGUEUR_LEXEME - 1] = '\0';
    tab_lexico[nb_charge].longueur = strlen(tab_lexico[nb_charge].lexeme);
    tab_lexico[nb_charge].suivant = -1;
    
    /* Libérer la mémoire temporaire */
    free(lexeme_traite);
    
    nb_charge++;
    nb_lexemes = nb_charge;
}


void finaliser_chargement_lexique(void) {
    if (nb_charge != nb_attendu) {
        fprintf(stderr, "Attention: %d/%d lexemes charges\n", nb_charge, nb_attendu);
    }
}


int charger_lexique(const char* chemin) {
    extern FILE* yyin;
    extern int yyparse(void);
    FILE* fichier;
    int resultat;
    
    if (chemin == NULL) {
        fprintf(stderr, "Erreur: chemin NULL\n");
        return 0;
    }
    
    fichier = fopen(chemin, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", chemin);
        return 0;
    }
    
    yyin = fichier;
    init_chargement_lexique();
    resultat = yyparse();
    fclose(fichier);
    
    if (resultat != 0) {
        fprintf(stderr, "Erreur: parsing echoue\n");
        return 0;
    }
    
    finaliser_chargement_lexique();
    return 1;
}