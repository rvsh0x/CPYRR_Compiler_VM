#include "erreurs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Variable globale */
ContexteSource contexte_source = {NULL, 0, NULL};

void charger_fichier_source(const char* nom_fichier) {
    FILE* fichier;
    char buffer[1024];
    int capacite = 100;
    char** nouveau_tableau;
    
    /* Ouvrir fichier */
    fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", nom_fichier);
        return;
    }
    
    /* Allouer tableau initial */
    contexte_source.lignes = malloc(capacite * sizeof(char*));
    if (contexte_source.lignes == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire échouée\n");
        fclose(fichier);
        return;
    }
    
    contexte_source.nb_lignes = 0;
    contexte_source.nom_fichier = strdup(nom_fichier);
    if (contexte_source.nom_fichier == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire échouée\n");
        free(contexte_source.lignes);
        fclose(fichier);
        return;
    }
    
    /* Lire chaque ligne */
    while (fgets(buffer, sizeof(buffer), fichier)) {
        /* Agrandir si nécessaire */
        if (contexte_source.nb_lignes >= capacite) {
            capacite *= 2;
            nouveau_tableau = realloc(contexte_source.lignes, capacite * sizeof(char*));
            if (nouveau_tableau == NULL) {
                fprintf(stderr, "Erreur : réallocation mémoire échouée\n");
                liberer_fichier_source();
                fclose(fichier);
                return;
            }
            contexte_source.lignes = nouveau_tableau;
        }
        
        /* Copier la ligne */
        contexte_source.lignes[contexte_source.nb_lignes] = strdup(buffer);
        if (contexte_source.lignes[contexte_source.nb_lignes] == NULL) {
            fprintf(stderr, "Erreur : allocation mémoire échouée\n");
            liberer_fichier_source();
            fclose(fichier);
            return;
        }
        contexte_source.nb_lignes++;
    }
    
    fclose(fichier);
}

const char* obtenir_ligne_source(int numero_ligne) {
    if (numero_ligne < 1 || numero_ligne > contexte_source.nb_lignes) {
        return NULL;
    }
    return contexte_source.lignes[numero_ligne - 1];
}

void liberer_fichier_source(void) {
    int i;
    
    if (contexte_source.lignes) {
        for (i = 0; i < contexte_source.nb_lignes; i++) {
            free(contexte_source.lignes[i]);
        }
        free(contexte_source.lignes);
    }
    
    if (contexte_source.nom_fichier) {
        free(contexte_source.nom_fichier);
    }
    
    contexte_source.lignes = NULL;
    contexte_source.nb_lignes = 0;
    contexte_source.nom_fichier = NULL;
}