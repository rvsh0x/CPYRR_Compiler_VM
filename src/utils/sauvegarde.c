#include "sauvegarde.h"

/* Crée le nom du dossier à partir du nom source */
char* creer_nom_dossier(const char* nom_source) {
    char* dossier;
    const char* point;
    int longueur;
    
    point = strrchr(nom_source, '.');
    if (point != NULL) {
        longueur = point - nom_source;
    } else {
        longueur = strlen(nom_source);
    }
    
    dossier = malloc(longueur + 4);
    if (dossier == NULL) {
        return NULL;
    }
    
    strncpy(dossier, nom_source, longueur);
    dossier[longueur] = '\0';
    strcat(dossier, "_ti");
    
    return dossier;
}

/* Crée le dossier pour les fichiers texte intermédiaire */
int creer_dossier_ti(const char* chemin_dossier) {
    #ifdef _WIN32
        return mkdir(chemin_dossier);
    #else
        return mkdir(chemin_dossier, 0755);
    #endif
}

/* Construit le chemin complet : dossier/nom_fichier */
char* construire_chemin_fichier(const char* dossier, const char* nom_fichier) {
    char* chemin;
    int longueur;
    
    longueur = strlen(dossier) + strlen(nom_fichier) + 2;
    chemin = malloc(longueur);
    if (chemin == NULL) {
        return NULL;
    }
    
    sprintf(chemin, "%s/%s", dossier, nom_fichier);
    return chemin;
}

/* Sauvegarde tous les arbres de toutes les régions.
 * 
 * Parcourt tab_regions et sauvegarde l'arbre de chaque région active.
 */
static int sauvegarder_tous_les_arbres(const char* chemin_fichier) {
    FILE* f;
    int i, nb_regions;
    arbre arbre_region;
    
    f = fopen(chemin_fichier, "w");
    if (f == NULL) {
        return 0;
    }
    
    /* Compter les régions actives */
    nb_regions = 0;
    while (nb_regions < MAX_REGIONS && tab_regions[nb_regions].numero != -1) {
        nb_regions++;
    }
    
    /* En-tête du fichier */
    fprintf(f, "# Arbres abstraits de toutes les régions\n");
    fprintf(f, "# Format : NATURE NUM_LEX NUM_DECL NB_ENFANTS\n");
    fprintf(f, "# NB_ENFANTS = nombre de nœuds au niveau inférieur\n\n");
    
    /* Sauvegarder l'arbre de chaque région */
    for (i = 0; i < nb_regions; i++) {
        arbre_region = obtenir_arbre_region(i);
        sauvegarder_arbre_region(f, arbre_region, i);
    }
    
    fclose(f);
    return 1;
}

/* Fonction principale de sauvegarde */
int sauvegarder_programme(const char* nom_fichier_source) {
    char* dossier;
    char* chemin;
    int succes;
    
    /* Créer le nom du dossier */
    dossier = creer_nom_dossier(nom_fichier_source);
    if (dossier == NULL) {
        fprintf(stderr, ROUGE "Erreur : allocation mémoire\n" RESET);
        return 0;
    }
    
    /* Créer le dossier */
    if (creer_dossier_ti(dossier) != 0) {
        /* Le dossier existe peut-être déjà, ce n'est pas grave */
    }
    
    printf(VERT "Sauvegarde dans : %s/\n" RESET, dossier);
    
    succes = 1;
    
    /* 1. Sauvegarder le lexique */
    chemin = construire_chemin_fichier(dossier, "lexique.txt");
    if (chemin != NULL) {
        if (sauvegarder_lexique(chemin)) {
            printf("  ✓ lexique.txt\n");
        } else {
            fprintf(stderr, ROUGE "  ✗ Échec lexique.txt\n" RESET);
            succes = 0;
        }
        free(chemin);
    }
    
    /* 2. Sauvegarder les déclarations */
    chemin = construire_chemin_fichier(dossier, "declarations.txt");
    if (chemin != NULL) {
        if (sauvegarder_declarations(chemin)) {
            printf("  ✓ declarations.txt\n");
        } else {
            fprintf(stderr, ROUGE "  ✗ Échec declarations.txt\n" RESET);
            succes = 0;
        }
        free(chemin);
    }
    
    /* 3. Sauvegarder les représentations */
    chemin = construire_chemin_fichier(dossier, "representations.txt");
    if (chemin != NULL) {
        if (sauvegarder_representations(chemin)) {
            printf("  ✓ representations.txt\n");
        } else {
            fprintf(stderr, ROUGE "  ✗ Échec representations.txt\n" RESET);
            succes = 0;
        }
        free(chemin);
    }
    
    /* 4. Sauvegarder les régions */
    chemin = construire_chemin_fichier(dossier, "regions.txt");
    if (chemin != NULL) {
        if (sauvegarder_regions(chemin)) {
            printf("  ✓ regions.txt\n");
        } else {
            fprintf(stderr, ROUGE "  ✗ Échec regions.txt\n" RESET);
            succes = 0;
        }
        free(chemin);
    }
    
    /* 5. Sauvegarder tous les arbres */
    chemin = construire_chemin_fichier(dossier, "arbres.txt");
    if (chemin != NULL) {
        if (sauvegarder_tous_les_arbres(chemin)) {
            printf("  ✓ arbres.txt\n");
        } else {
            fprintf(stderr, ROUGE "  ✗ Échec arbres.txt\n" RESET);
            succes = 0;
        }
        free(chemin);
    }
    
    free(dossier);
    
    if (succes) {
        printf(VERT "Sauvegarde terminée avec succès\n" RESET);
    }
    
    return succes;
}