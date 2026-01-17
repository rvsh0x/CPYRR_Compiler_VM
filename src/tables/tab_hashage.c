#include <stdio.h>
#include <string.h>
#include "tab_hashage.h"
#include "tab_lexico.h"

int tab_hashage[TAILLE_HASHAGE];

void initialiser_tab_hashage() {
    int i;
    
    i = 0;
    while (i < TAILLE_HASHAGE) {
        tab_hashage[i] = -1;
        i++;
    }
}

int calculer_hashage(const char* lexeme) {
    int somme;
    int i;
    
    somme = 0;
    i = 0;
    while (lexeme[i] != '\0') {
        somme = somme + (int)lexeme[i];
        i++;
    }
    
    return somme % TAILLE_HASHAGE;
}

void afficher_tab_hashage() {
    int i;
    int indice;

    printf("\n--- TABLE DE HASHAGE ---\n");

    i = 0;
    while (i < TAILLE_HASHAGE) {
        if (tab_hashage[i] != -1) {
            printf("[%2d] -> ", i);
            
            indice = tab_hashage[i];
            while (indice != -1) {
                printf("%d (%s)", indice, tab_lexico[indice].lexeme);
                indice = tab_lexico[indice].suivant;
                if (indice != -1) {
                    printf(" -> ");
                }
            }
            printf("\n");
        }
        i++;
    }

    printf("------------------------\n");
}