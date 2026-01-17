#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm/charg_reg.h"
#include "tab_regions.h"

static int nb_attendu = 0;
static int nb_charge = 0;

void init_chargement_regions() {
    nb_attendu = 0;
    nb_charge = 0;
}

void traiter_entete_regions(int nb_regions) {
    if (nb_regions < 0 || nb_regions > MAX_REGIONS) {
        fprintf(stderr, "Erreur: nombre de régions invalide (%d)\n", nb_regions);
        return;
    }
    nb_attendu = nb_regions;
}

void traiter_region(int index, int nis, int taille) {
    if (index < 0 || index >= MAX_REGIONS) {
        fprintf(stderr, "Erreur: index région hors limites (%d)\n", index);
        return;
    }
    
    if (nis < 0) {
        fprintf(stderr, "Attention: NIS négatif (%d) pour région %d\n", nis, index);
    }
    
    if (taille < 0) {
        fprintf(stderr, "Attention: taille négative (%d) pour région %d\n", taille, index);
    }
    
    tab_regions[index].numero = index;
    tab_regions[index].nis = nis;
    tab_regions[index].taille = taille;
    tab_regions[index].region_parent = -1;
    tab_regions[index].instructions = NULL;
    
    nb_charge++;
}

void finaliser_chargement_regions() {
    if (nb_charge != nb_attendu) {
        fprintf(stderr, "Attention: %d/%d regions chargées\n", 
                nb_charge, nb_attendu);
    }
}

int charger_regions(const char* chemin) {
    extern FILE* yyin;
    extern int yyparse();
    int resultat;
    
    if (chemin == NULL) {
        fprintf(stderr, "Erreur: chemin NULL\n");
        return 0;
    }

    yyin = fopen(chemin, "r");
    if (!yyin) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", chemin);
        return 0;
    }
    
    init_chargement_regions();
    resultat = yyparse();
    fclose(yyin);

    if (resultat != 0) {
        fprintf(stderr, "Erreur: parsing échoué\n");
        return 0;
    }
    
    finaliser_chargement_regions();
    return 1;
}