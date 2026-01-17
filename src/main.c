#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "erreurs.h"
#include "tab_lexico.h"
#include "tab_declarations.h"
#include "tab_representations.h"
#include "tab_regions.h"
#include "pile_regions.h"
#include "ast.h"
#include "semantique.h"
#include "sauvegarde.h"

extern int yyparse();
extern FILE *yyin;

int afficher_tables = 0;

void afficher_aide(char *nom_prog) {
    printf("Usage: %s [OPTIONS] fichier.cpyrr\n\n", nom_prog);
    printf("Options:\n");
    printf("  -t    Afficher les tables (lexicographique, declarations, etc.)\n");
    printf("  -h    Afficher cette aide\n");
}

int main(int argc, char *argv[]) {
    int opt;
    char *fichier;
    int nb_erreurs_bloquantes;
    int nb_warnings;
    
    fichier = NULL;
    
    /* Traitement des options avec getopt */
    while ((opt = getopt(argc, argv, "th")) != -1) {
        if (opt == 't') {
            afficher_tables = 1;
        } else if (opt == 'h') {
            afficher_aide(argv[0]);
            exit(0);
        } else {
            afficher_aide(argv[0]);
            exit(1);
        }
    }
    
    /* Récupérer le nom du fichier */
    if (optind < argc) {
        fichier = argv[optind];
    } else {
        fprintf(stderr, ROUGE "Erreur: aucun fichier spécifié\n" RESET);
        afficher_aide(argv[0]);
        exit(1);
    }
    
    /* Ouvrir le fichier */
    nom_fichier = fichier;
    yyin = fopen(nom_fichier, "r");
    if (!yyin) {
        fprintf(stderr, ROUGE "Erreur: impossible d'ouvrir %s\n" RESET, nom_fichier);
        exit(1);
    }
    
    charger_fichier_source(nom_fichier);
    
    /* Initialisation */
    init_erreurs();
    initialiser_tab_lexico();
    initialiser_pile_regions();
    initialiser_tab_regions();
    initialiser_tab_declarations();
    initialiser_tab_representation();
    
    /* Analyse */
    if (yyparse() == 0) {
        /* Analyse sémantique après parsing */
        analyser_semantique();
    }
    
    fclose(yyin);
    
    /* Compter erreurs vs warnings */
    nb_erreurs_bloquantes = compter_erreurs_bloquantes();
    nb_warnings = compter_warnings();

        /* Affichage conditionnel des tables (inclut les arbres) */
    if (afficher_tables == 1) {
        printf("\n");
        afficher_tab_lexico();
        afficher_tab_hashage();
        afficher_tab_declarations();
        afficher_tab_representation();
        afficher_tab_regions(); 
    }
    
    /* Affichage des erreurs/warnings */
    if (nb_erreurs > 0) {
        afficher_erreurs();
    }
    


    /* Vérifier si compilation a échoué */
    if (nb_erreurs_bloquantes > 0) {
        printf(ROUGE "\n✗ Compilation échouée : %d erreur(s)\n" RESET, nb_erreurs_bloquantes);
        if (nb_warnings > 0) {
            printf(JAUNE "  + %d warning(s)\n" RESET, nb_warnings);
        }
        
        liberer_fichier_source();
        exit(EXIT_FAILURE);
    }
    
    /* Compilation réussie (warnings possibles) */
    printf(VERT "\n✓ Compilation réussie\n" RESET);
    if (nb_warnings > 0) {
        printf(JAUNE "  ⚠ %d warning(s)\n" RESET, nb_warnings);
    }
    
    /* Sauvegarde du programme compilé */
    printf("\n");
    if (sauvegarder_programme(nom_fichier)) {
        printf(VERT "\n✓ Programme sauvegardé avec succès\n" RESET);
    } else {
        fprintf(stderr, ROUGE "\n✗ Erreur lors de la sauvegarde\n" RESET);
        liberer_fichier_source();
        exit(EXIT_FAILURE);
    }
    
    /* Libération mémoire */
    liberer_fichier_source();
    exit(0);
}
