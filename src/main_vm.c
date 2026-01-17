#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm/vm_exec.h"
#include "vm/charg_lex.h"
#include "vm/charg_decl.h"
#include "vm/charg_reg.h"
#include "vm/charg_rep.h"
#include "vm/charg_arbres.h"
#include "tab_regions.h"
#include "tab_declarations.h"
#include "tab_lexico.h"
#include "tab_representations.h"
#include "couleurs.h"

void afficher_usage(const char* prog) {
    printf("Usage : %s <dossier_ti>\n", prog);
    printf("\n");
    printf("Exemple : %s tests/test_phase1_ti\n", prog);
    printf("\n");
}

int main(int argc, char* argv[]) {
    char chemin_lex[512], chemin_decl[512], chemin_reg[512], chemin_rep[512], chemin_arbres[512];
    
    if (argc != 2) {
        afficher_usage(argv[0]);
        exit(1);
    }
    
    /* Construire chemins fichiers */
    snprintf(chemin_lex, sizeof(chemin_lex), "%s/lexique.txt", argv[1]);
    snprintf(chemin_decl, sizeof(chemin_decl), "%s/declarations.txt", argv[1]);
    snprintf(chemin_reg, sizeof(chemin_reg), "%s/regions.txt", argv[1]);
    snprintf(chemin_rep, sizeof(chemin_rep), "%s/representations.txt", argv[1]);
    snprintf(chemin_arbres, sizeof(chemin_arbres), "%s/arbres.txt", argv[1]);
    
    printf("\n");
    printf(BLEU "Chargement tables...\n" RESET);
    
    /* Charger tables */
    if (!charger_lexique(chemin_lex)) {
        fprintf(stderr, ROUGE "Erreur : chargement lexique\n" RESET);
        exit(1);
    }
    printf("  Lexique chargé\n");
    
    if (!charger_declarations(chemin_decl)) {
        fprintf(stderr, ROUGE "Erreur : chargement déclarations\n" RESET);
        exit(1);
    }
    printf("  Déclarations chargées\n");
    
    if (!charger_regions(chemin_reg)) {
        fprintf(stderr, ROUGE "Erreur : chargement régions\n" RESET);
        exit(1);
    }
    printf("  Régions chargées\n");
    
    if (!charger_representations(chemin_rep)) {
        fprintf(stderr, ROUGE "Erreur : chargement représentations\n" RESET);
        exit(1);
    }
    printf("  Représentations chargées\n");
    
    if (!charger_arbres(chemin_arbres)) {
        fprintf(stderr, ROUGE "Erreur : chargement arbres\n" RESET);
        exit(1);
    }
    printf("  Arbres chargés\n");
    
    printf(VERT "Tout a été chargé avec succès\n" RESET);

    /* affichages des tables pour voir si le contenu est correctement chargé */
    afficher_tab_declarations();
    afficher_tab_lexico();
    afficher_tab_representation();
    afficher_tab_regions();

    /* Interpréter */
    interpreter();
    
    exit(0);
}