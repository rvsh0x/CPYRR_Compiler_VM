#include <stdio.h>
#include <stdlib.h>
#include "pile_regions.h"
#include "couleurs.h"

int pile_regions[MAX_REGIONS];
int sommet_pile = -1;
int compteur_regions = 0;

void initialiser_pile_regions() {
    sommet_pile = -1;
    compteur_regions = 0;
    empiler_region(0);
}

void empiler_region(int num_region) {
    if (sommet_pile >= MAX_REGIONS - 1) {
        fprintf(stderr, ROUGE "Erreur : pile des régions pleine\n" RESET);
        exit(EXIT_FAILURE);
    }
    sommet_pile++;
    pile_regions[sommet_pile] = num_region;
}

int depiler_region() {
    int region;
    
    if (sommet_pile < 0) {
        fprintf(stderr, ROUGE "Erreur : pile des régions vide\n" RESET);
        exit(EXIT_FAILURE);
    }
    
    region = pile_regions[sommet_pile];
    sommet_pile--;
    return region;
}

int obtenir_region_courante() {
    if (sommet_pile < 0) {
        fprintf(stderr, ROUGE "Erreur : pile des régions vide\n" RESET);
        exit(EXIT_FAILURE);
    }
    return pile_regions[sommet_pile];
}

int nouvelle_region() {
    compteur_regions++;
    return compteur_regions;
}

int* obtenir_pile(int* taille) {
    *taille = sommet_pile + 1;
    return pile_regions;
}

void afficher_pile_regions() {
    int i;

    printf("\n--- PILE DES RÉGIONS ---\n");
    printf("Sommet : %d\n", sommet_pile);
    printf("Contenu : [");
    
    i = 0;
    while (i <= sommet_pile) {
        printf("%d", pile_regions[i]);
        if (i < sommet_pile) {
            printf(", ");
        }
        i++;
    }
    
    printf("]\n");
    printf("------------------------\n\n");
}