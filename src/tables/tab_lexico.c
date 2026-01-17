#include <stdio.h>
#include <string.h>
#include "tab_lexico.h"


Lexeme tab_lexico[MAX_LEXEMES];
int nb_lexemes = 0;

void initialiser_tab_lexico() {
    initialiser_tab_hashage();
    
    nb_lexemes = 0;
    
    /* Types predefinis aux indices 0-3 */
    inserer_lexeme("int", 3);
    inserer_lexeme("real", 4);
    inserer_lexeme("bool", 4);
    inserer_lexeme("char", 4);
}

int rechercher_lexeme(const char* lexeme, int longueur) {
    int hash;
    int indice;
    
    /* Si longueur non fournie, la calculer -> au cas où ... */
    if (longueur < 0) {
        longueur = strlen(lexeme);
    }
    
    hash = calculer_hashage(lexeme);
    indice = tab_hashage[hash];
    
    while (indice != -1) {
        if (tab_lexico[indice].longueur == longueur) {
            if (strcmp(tab_lexico[indice].lexeme, lexeme) == 0) {
                return indice;
            }
        }
        indice = tab_lexico[indice].suivant;
    }
    
    return -1;
}

int inserer_lexeme(const char* lexeme, int longueur) {
    int numero;
    int hash;
    
    /* Si longueur non fournie, la calculer UNE SEULE FOIS */
    if (longueur < 0) {
        longueur = strlen(lexeme);
    }
    
    /* Passer la longueur à rechercher (PAS DE DOUBLON strlen) */
    numero = rechercher_lexeme(lexeme, longueur);
    if (numero != -1) {
        return numero;
    }
    
    if (nb_lexemes >= MAX_LEXEMES) {
        fprintf(stderr, ROUGE "Erreur : table lexicographique pleine\n" RESET);
        return -1;
    }
    
    hash = calculer_hashage(lexeme);
    numero = nb_lexemes;
    
    strncpy(tab_lexico[numero].lexeme, lexeme, MAX_LONGUEUR_LEXEME - 1);
    tab_lexico[numero].lexeme[MAX_LONGUEUR_LEXEME - 1] = '\0';
    
    /* Utiliser la longueur déjà calculée (PAS DE DOUBLON strlen) */
    tab_lexico[numero].longueur = longueur;
    
    tab_lexico[numero].suivant = tab_hashage[hash];
    tab_hashage[hash] = numero;
    
    nb_lexemes++;
    
    return numero;
}

char* obtenir_lexeme(int numero) {
    if (numero < 0 || numero >= nb_lexemes) {
        return NULL;
    }
    return tab_lexico[numero].lexeme;
}

/* Format du fichier lexique.txt :
 * 
 * nb_entrees: 15
 * 0: "PROG"
 * 1: "x"
 * 2: "test"
 * ...
 */
int sauvegarder_lexique(const char* chemin_fichier) {
    FILE* f;
    int i;
    
    f = fopen(chemin_fichier, "w");
    if (f == NULL) {
        return 0;
    }
    
    /* Nombre d'entrées */
    fprintf(f, "nb_entrees: %d\n", nb_lexemes);
    
    /* Chaque lexème sur une ligne */
    for (i = 0; i < nb_lexemes; i++) {
        fprintf(f, "%d: %s\n", i, tab_lexico[i].lexeme);
    }
    
    fclose(f);
    return 1;
}

void afficher_tab_lexico() {
    int i;
    int hash;
    
    printf("\n--- TABLE LEXICOGRAPHIQUE ---\n");
    printf("Nombre de lexemes : %d\n\n", nb_lexemes);
    
    printf("INDEX | HASH | LEXEME                | LONG. | SUIV.\n");
    printf("------|------|-----------------------|-------|-------\n");
    
    i = 0;
    while (i < nb_lexemes) {
        hash = calculer_hashage(tab_lexico[i].lexeme);
        printf("%-5d | %-4d | %-21s | %-5d | %-5d\n",
               i,
               hash,
               tab_lexico[i].lexeme,
               tab_lexico[i].longueur,
               tab_lexico[i].suivant);
        i++;
    }

    printf("-----------------------------\n");
}