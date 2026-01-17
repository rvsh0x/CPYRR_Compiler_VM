#include "vm/vm_exec.h"
#include "vm/vm_expressions.h"
#include "vm/vm_instructions.h"
#include "tab_regions.h"
#include "couleurs.h"
#include <stdio.h>

void interpreter() {
    arbre arbre_programme;
    
    printf("\n");
    printf(BLEU "Début interprétation\n" RESET);
    printf("\n");
    
    /* Initialiser pile */
    initialiser_pile_execution();
    
    /* Région 0 : programme principal */
    BC = 0;
    region_courante = 0;
    
    /* Récupérer arbre */
    arbre_programme = obtenir_arbre_region(0);
    
    if (arbre_programme == NULL) {
        fprintf(stderr, ROUGE "Erreur : pas d'arbre pour région 0\n" RESET);
        return;
    }
    
    /* affichage de l'état inital de la pile */
    afficher_pile_complete();
    
    /* Exécuter */
    executer_arbre(arbre_programme);
    
    printf("\n");
    printf(VERT "Fin interprétation\n" RESET);
    printf("\n");
}