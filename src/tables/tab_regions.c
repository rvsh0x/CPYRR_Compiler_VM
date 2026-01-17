#include <stdio.h>
#include <stdlib.h>
#include "tab_regions.h"
#include "pile_regions.h"
#include "tab_declarations.h"
#include "tab_representations.h"
#include "couleurs.h"

info_region tab_regions[MAX_REGIONS];

void initialiser_tab_regions() {
    int i;
    
    /* Initialiser toutes les entrées */
    i = 0;
    while (i < MAX_REGIONS) {
        tab_regions[i].numero = -1;
        tab_regions[i].region_parent = -1;
        tab_regions[i].nis = -1;
        tab_regions[i].taille = -1;
        tab_regions[i].instructions = NULL;
        i++;
    }
    
    /* Créer la région 0 (programme principal) */
    tab_regions[0].numero = 0;
    tab_regions[0].region_parent = -1;
    tab_regions[0].nis = 0;
    tab_regions[0].taille = -1;
    tab_regions[0].instructions = NULL;
}

int creer_region(int parent) {
    int nouveau_numero, nis_parent;
    
    /* Obtenir le nouveau numéro via pile_regions */
    nouveau_numero = nouvelle_region();
    
    if (nouveau_numero >= MAX_REGIONS) {
        fprintf(stderr, ROUGE "Erreur : nombre maximum de régions atteint\n" RESET);
        exit(EXIT_FAILURE);
    }
    
    /* Calculer le NIS */
    if (parent == -1 || parent < 0) {
        nis_parent = -1;
    } else if (parent < nouveau_numero) {
        nis_parent = tab_regions[parent].nis;
    } else {
        fprintf(stderr, ROUGE "Erreur : région parente invalide (%d)\n" RESET, parent);
        exit(EXIT_FAILURE);
    }
    
    /* Créer l'entrée dans la table */
    tab_regions[nouveau_numero].numero = nouveau_numero;
    tab_regions[nouveau_numero].region_parent = parent;
    tab_regions[nouveau_numero].nis = nis_parent + 1;
    tab_regions[nouveau_numero].taille = -1;
    tab_regions[nouveau_numero].instructions = NULL;
    
    return nouveau_numero;
}

void associer_arbre_region(int num_region, arbre a) {
    if (num_region < 0 || num_region >= MAX_REGIONS) {
        fprintf(stderr, ROUGE "Erreur : numéro de région invalide (%d)\n" RESET, num_region);
        return;
    }
    
    if (tab_regions[num_region].numero == -1) {
        fprintf(stderr, ROUGE "Erreur : région %d non initialisée\n" RESET, num_region);
        return;
    }
    
    tab_regions[num_region].instructions = a;
}

arbre obtenir_arbre_region(int num_region) {
    if (num_region < 0 || num_region >= MAX_REGIONS) {
        return NULL;
    }
    
    if (tab_regions[num_region].numero == -1) {
        return NULL;
    }
    
    return tab_regions[num_region].instructions;
}

int obtenir_nis_region(int num_region) {
    if (num_region < 0 || num_region >= MAX_REGIONS) {
        return -1;
    }
    
    if (tab_regions[num_region].numero == -1) {
        return -1;
    }
    
    return tab_regions[num_region].nis;
}

int obtenir_parent_region(int num_region) {
    if (num_region < 0 || num_region >= MAX_REGIONS) {
        return -1;
    }
    
    if (tab_regions[num_region].numero == -1) {
        return -1;
    }
    
    return tab_regions[num_region].region_parent;
}

/* Format du fichier regions.txt :
 * 
 * nb_regions: 3
 * REGION 0: nis=0 taille=10
 * REGION 1: nis=1 taille=5
 * REGION 2: nis=2 taille=3
 * ...
 */
int sauvegarder_regions(const char* chemin_fichier) {
    FILE* f;
    int i, nb_actives;
    
    f = fopen(chemin_fichier, "w");
    if (f == NULL) {
        return 0;
    }
    
    /* Compter les régions actives */
    nb_actives = 0;
    while (nb_actives < MAX_REGIONS && tab_regions[nb_actives].numero != -1) {
        nb_actives++;
    }
    
    fprintf(f, "nb_regions: %d\n", nb_actives);
    
    /* Sauvegarder chaque région */
    for (i = 0; i < nb_actives; i++) {
        fprintf(f, "REGION %d: nis=%d taille=%d\n",
                i,
                tab_regions[i].nis,
                tab_regions[i].taille);
    }
    
    fclose(f);
    return 1;
}

void afficher_tab_regions() {
    int i, nb_regions_actives;
    arbre a;

    
    /* Compter les régions actives */
    nb_regions_actives = 0;
    i = 0;
    while (i < MAX_REGIONS && tab_regions[i].numero != -1) {
        nb_regions_actives++;
        i++;
    }

    printf("\n--- TABLE DES RÉGIONS ---\n");
    printf("Nombre de régions : %d\n\n", nb_regions_actives);
    
    if (nb_regions_actives == 0) {
        printf("(Table vide)\n");
        printf(CYAN "-------------------------\n\n" RESET);
        return;
    }
    
    i = 0;
    while (i < nb_regions_actives) {
        /* En-tête de région */
        if (i == 0) {
            printf(VERT "--- Région %d (Programme Principal) ---\n" RESET, i);
        } else {
            printf(VERT "--- Région %d ---\n" RESET, i);
        }
        
        /* Informations de base */
        printf("  Parent : ");
        if (tab_regions[i].region_parent == -1) {
            printf(GRAS "(aucun)\n" RESET);
        } else {
            printf("%d\n", tab_regions[i].region_parent);
        }
        
        printf("  NIS    : %d\n", tab_regions[i].nis);
        
        printf("  Taille : ");
        if (tab_regions[i].taille == -1) {
            printf(GRAS "(non calculée)\n" RESET);
        } else {
            printf("%d\n", tab_regions[i].taille);
        }
        
        /* Arbre d'instructions */
        a = tab_regions[i].instructions;
        
        if (a == NULL) {
            printf("  Arbre  : " GRAS "(vide)\n" RESET);
        } else {
            printf("  Arbre  :\n");
            afficher_arbre(a);
        }
        
        printf("\n");
        i++;
    }

    printf(CYAN "-------------------------\n\n" RESET);
}