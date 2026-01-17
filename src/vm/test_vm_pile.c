#include <stdio.h>
#include <stdlib.h>
#include "vm/vm_pile.h"
#include "tab_regions.h"
#include "tab_declarations.h"

static void verifier_chainages(int BC_pos, int attendu1, int attendu2, int attendu3, int nis) {
    int erreur = 0;
    
    if (nis >= 1 && pile[BC_pos + 1].valeur.entier != attendu1) {
        printf("ERREUR: pile[BC+1]=%d (attendu %d)\n", pile[BC_pos + 1].valeur.entier, attendu1);
        erreur = 1;
    }
    
    if (nis >= 2 && pile[BC_pos + 2].valeur.entier != attendu2) {
        printf("ERREUR: pile[BC+2]=%d (attendu %d)\n", pile[BC_pos + 2].valeur.entier, attendu2);
        erreur = 1;
    }
    
    if (nis >= 3 && pile[BC_pos + 3].valeur.entier != attendu3) {
        printf("ERREUR: pile[BC+3]=%d (attendu %d)\n", pile[BC_pos + 3].valeur.entier, attendu3);
        erreur = 1;
    }
    
    if (!erreur) {
        printf("Verification OK\n");
    }
}

int main() {
    int BC_avant, ch1, ch2, ch3, BC_cible;
    
    printf("Test chainages statiques\n\n");
    
    initialiser_pile_execution();
    
    tab_regions[0].nis = 0;
    tab_regions[0].taille = 3;
    
    tab_regions[1].nis = 1;
    tab_regions[1].taille = 4;
    
    tab_regions[2].nis = 2;
    tab_regions[2].taille = 5;
    
    tab_regions[3].nis = 3;
    tab_regions[3].taille = 6;
    
    tab_regions[4].nis = 3;
    tab_regions[4].taille = 5;
    
    tab_regions[5].nis = 1;
    tab_regions[5].taille = 4;
    
    printf("Configuration regions:\n");
    printf("  0: NIS=0, taille=3\n");
    printf("  1: NIS=1, taille=4\n");
    printf("  2: NIS=2, taille=5\n");
    printf("  3: NIS=3, taille=6\n");
    printf("  4: NIS=3, taille=5\n");
    printf("  5: NIS=1, taille=4\n");
    
    printf("\nTest 1: NIS 0 -> 1 (augmente)\n");
    afficher_pile_complete();
    empiler_zone(1);
    verifier_chainages(BC, 0, 0, 0, 1);
    afficher_pile_complete();
    
    printf("\nTest 2: NIS 1 -> 2 (augmente)\n");
    BC_avant = BC;
    empiler_zone(2);
    verifier_chainages(BC, BC_avant, 0, 0, 2);
    afficher_pile_complete();
    
    printf("\nTest 3: NIS 2 -> 3 (augmente)\n");
    BC_avant = BC;
    ch1 = pile[BC + 1].valeur.entier;
    empiler_zone(3);
    verifier_chainages(BC, BC_avant, ch1, 0, 3);
    afficher_pile_complete();
    
    printf("\nTest 4: NIS 3 -> 3 (stagne)\n");
    ch1 = pile[BC + 1].valeur.entier;
    ch2 = pile[BC + 2].valeur.entier;
    ch3 = pile[BC + 3].valeur.entier;
    empiler_zone(4);
    verifier_chainages(BC, ch1, ch2, ch3, 3);
    afficher_pile_complete();
    
    printf("\nTest 5: NIS 3 -> 1 (decroit)\n");
    BC_cible = pile[BC + 2].valeur.entier;
    ch1 = pile[BC_cible + 1].valeur.entier;
    empiler_zone(5);
    verifier_chainages(BC, ch1, 0, 0, 1);
    afficher_pile_complete();
    
    printf("\nTest 6: Depilements\n");
    depiler_zone();
    printf("Retour region 4\n");
    afficher_pile_complete();
    
    depiler_zone();
    printf("Retour region 3\n");
    afficher_pile_complete();
    
    depiler_zone();
    printf("Retour region 2\n");
    afficher_pile_complete();
    
    depiler_zone();
    printf("Retour region 1\n");
    afficher_pile_complete();
    
    depiler_zone();
    printf("Retour region 0\n");
    afficher_pile_complete();
    
    printf("\nTests termines\n");
    return 0;
}