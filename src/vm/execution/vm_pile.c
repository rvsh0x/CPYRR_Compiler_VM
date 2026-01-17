#include "vm/vm_pile.h"
#include "vm/vm_adressage.h"
#include "tab_regions.h"
#include "tab_declarations.h"
#include "tab_representations.h"
#include "couleurs.h"
#include "erreurs.h"
#include <string.h>

/* Variables globales */
Cellule pile[TAILLE_PILE];
int BC = 0;
int region_courante = 0;

/* Pile auxiliaire pour sauvegarder les régions lors des appels
pourquoi ??? : 
en effet l'idée de garder une case dans la pile a l'execution était une mauvaise idée
car elle écrase la valeur contenue dans [BC-1] .. fallait pas faire ça ou
mettre le numéro de région après les chaînages ... Bon  */

static int pile_regions_vm[500];
static int sommet_pile_regions_vm = 0;

/* Initialise la pile */
void initialiser_pile_execution() {
    int i;
    
    BC = 0;
    region_courante = 0;
    
    i = 0;
    while (i < TAILLE_PILE) {
        pile[i].valeur.entier = 0;
        pile[i].est_initialisee = 0;
        i++;
    }
    
    printf("Pile initialisée : BC=0, taille=%d\n", TAILLE_PILE);
}

static void copier_chainages_statiques(int BC_source, int BC_dest, int nb_chainages) {
    int i = 1;
    while (i <= nb_chainages) {
        pile[BC_dest + i].valeur.entier = pile[BC_source + i].valeur.entier;
        pile[BC_dest + i].est_initialisee = 1;
        i++;
    }
}

int determiner_si_fonction(int num_region) {
    int num_decl = 0;
    while (num_decl < MAX_DECLARATIONS) {
        if (tab_declarations[num_decl].nature == NATURE_FCT &&
            tab_declarations[num_decl].execution == num_region) {
            return 1;
        }
        num_decl++;
    }
    return 0;
}

void empiler_zone(int num_region) {
    int BC_ancien, BC_nouveau;
    int taille_courante;
    int NIS_appelant;
    int NIS_appele;
    int est_fonction;
    int remontee, BC_cible;
    int i;
    
    if (num_region < 0 || num_region >= MAX_REGIONS) {
        fprintf(stderr, "Erreur : numéro région invalide (%d)\n", num_region);
        exit(1);
    }
    
    est_fonction = determiner_si_fonction(num_region);
    
    BC_ancien = BC;
    taille_courante = tab_regions[region_courante].taille;
    BC_nouveau = BC_ancien + taille_courante;
    
    if (BC_nouveau >= TAILLE_PILE) {
        fprintf(stderr, "Erreur : débordement pile (BC=%d)\n", BC_nouveau);
        exit(1);
    }

    /* Sauvegarder la région courante dans la pile auxiliaire */
    if (sommet_pile_regions_vm >= 500) {
        fprintf(stderr, "Erreur : débordement pile régions\n");
        exit(1);
    }
    pile_regions_vm[sommet_pile_regions_vm] = region_courante;
    sommet_pile_regions_vm++;
    
    pile[BC_nouveau].valeur.entier = BC_ancien;
    pile[BC_nouveau].est_initialisee = 1;
    
    NIS_appelant = tab_regions[region_courante].nis;
    NIS_appele = tab_regions[num_region].nis;
    
    if (NIS_appele > 0) {
        if (NIS_appele > NIS_appelant) {
            /* NIS augmente : mettre BC_ancien puis copier anciens chaînages */
            
            /* Position 1 : BC_ancien */
            pile[BC_nouveau + 1].valeur.entier = BC_ancien;
            pile[BC_nouveau + 1].est_initialisee = 1;
            
            /* Positions 2 à NIS_appele : copier anciens chaînages décalés */
            i = 1;
            while (i <= NIS_appelant) {
                pile[BC_nouveau + 1 + i].valeur.entier = pile[BC_ancien + i].valeur.entier;
                pile[BC_nouveau + 1 + i].est_initialisee = 1;
                i++;
            }
            
            printf("NIS augmente : %d -> %d\n", NIS_appelant, NIS_appele);
            
        } else if (NIS_appele == NIS_appelant) {
            /* NIS stagne : recopier tous */
            copier_chainages_statiques(BC_ancien, BC_nouveau, NIS_appele);
            printf("NIS stagne : %d\n", NIS_appelant);
            
        } else if (NIS_appele < NIS_appelant) {
            /* NIS décroît : remonter */
            remontee = NIS_appelant - NIS_appele;
            BC_cible = pile[BC_ancien + remontee].valeur.entier;
            copier_chainages_statiques(BC_cible, BC_nouveau, NIS_appele);
            printf("NIS decroit : %d -> %d\n", NIS_appelant, NIS_appele);
        }
    }
    
    if (est_fonction) {
        pile[BC_nouveau + NIS_appele + 1].valeur.entier = 0;
        pile[BC_nouveau + NIS_appele + 1].est_initialisee = 0;
    }
    
    BC = BC_nouveau;
    region_courante = num_region;
}


void depiler_zone() {
    int BC_ancien, BC_nouveau;
    int taille_region, debut_variables, i;
    
    if (BC == 0) {
        fprintf(stderr, "Erreur : tentative dépiler région principale\n");
        return;
    }
    
    BC_ancien = BC;
    taille_region = tab_regions[region_courante].taille;
    debut_variables = BC + tab_regions[region_courante].nis + 1;
    
    /* Réinitialiser les variables de la région dépiée */
    i = debut_variables;
    while (i < BC + taille_region) {
        pile[i].valeur.entier = 0;
        pile[i].est_initialisee = 0;
        i++;
    }
    
    /* Restaurer BC */
    BC_nouveau = pile[BC].valeur.entier;
    BC = BC_nouveau;
    
    /* Restaurer région depuis la pile auxiliaire */
    if (sommet_pile_regions_vm > 0) {
        sommet_pile_regions_vm--;
        region_courante = pile_regions_vm[sommet_pile_regions_vm];
    } else {
        region_courante = 0;
    }
    
    printf("Dépilement : BC %d -> %d (région %d)\n", 
           BC_ancien, BC_nouveau, region_courante);
}

void ecrire_valeur_retour(int valeur) {
    int NIS_courante;
    int adresse_retour;
    
    NIS_courante = tab_regions[region_courante].nis;
    adresse_retour = BC + NIS_courante + 1;

    verifier_adresse(adresse_retour);
    pile[adresse_retour].valeur.entier = valeur;
    pile[adresse_retour].est_initialisee = 1;
    printf("Valeur retour écrite : pile[%d] = %d\n", adresse_retour, valeur);
}


Valeur depiler_zone_fonction() {
    int BC_ancien, BC_nouveau;
    int taille_region, debut_variables, i;
    int NIS_courante;
    int adresse_retour;
    Valeur valeur_retour;
    int est_fonction;
    
    if (BC == 0) {
        fprintf(stderr, "Erreur : tentative dépiler région principale\n");
        exit(1);
    }
    
    /* Déterminer si c'est une fonction */
    est_fonction = determiner_si_fonction(region_courante);

    /* Récupérer valeur retour si fonction */
    valeur_retour.entier = 0;  /* Par défaut */
    
    if (est_fonction) {
        NIS_courante = tab_regions[region_courante].nis;
        adresse_retour = BC + NIS_courante + 1;
        
        if (pile[adresse_retour].est_initialisee) {
            valeur_retour = pile[adresse_retour].valeur;
        } else {
            /* Gérer le cas où la fonction n'a pas de valeur de retour 
            mais normalement ce cas est géré par le compilateur et produit 
            un warning ... logiquement l'utilisateur doit le corriger 
            si non on est obligé de sortir du programme je pense ....
            a Revoir ....*/
            erreur_runtime("Fonction sans valeur de retour initialisée (adresse pile : %d)", adresse_retour);
        }
    }
    
    BC_ancien = BC;
    taille_region = tab_regions[region_courante].taille;
    
    /* Commencer APRÈS la case retour pour les fonctions */
    if (est_fonction) {
        debut_variables = BC + tab_regions[region_courante].nis + 2; 
    } else {
        debut_variables = BC + tab_regions[region_courante].nis + 1;
    }
    
    /* Réinitialiser les variables */
    i = debut_variables;
    while (i < BC + taille_region) {
        pile[i].valeur.entier = 0;
        pile[i].est_initialisee = 0;
        i++;
    }
    
    /* Restaurer BC */
    BC_nouveau = pile[BC].valeur.entier;
    BC = BC_nouveau;
    
    /* Restaurer région depuis la pile auxiliaire */
    if (sommet_pile_regions_vm > 0) {
        sommet_pile_regions_vm--;
        region_courante = pile_regions_vm[sommet_pile_regions_vm];
    } else {
        region_courante = 0;
    }
    
    printf("Dépilement : BC %d -> %d (région %d)\n", 
           BC_ancien, BC_nouveau, region_courante);
    
    return valeur_retour;
}

void verifier_adresse(int adresse) {
    if (adresse < 0 || adresse >= TAILLE_PILE) {
        erreur_runtime("Adresse pile invalide (%d)", adresse);
    }
}

/* Lit une cellule (avec vérification initialisation) */

Valeur lire_pile(int adresse) {
    verifier_adresse(adresse);
    
    if (!pile[adresse].est_initialisee) {
        erreur_runtime("Lecture d'une variable non initialisée (adresse pile : %d)", adresse);
    }
    
    return pile[adresse].valeur;
}

/* Ecrit un entier dans la pile */
void ecrire_pile_entier(int adresse, int valeur) {
    verifier_adresse(adresse);
    pile[adresse].valeur.entier = valeur;
    pile[adresse].est_initialisee = 1;
}

/* Ecrit un réel dans la pile */
void ecrire_pile_reel(int adresse, float valeur) {
    verifier_adresse(adresse);
    pile[adresse].valeur.reel = valeur;
    pile[adresse].est_initialisee = 1;
}

/* Ecrit un booléen dans la pile */
void ecrire_pile_booleen(int adresse, char valeur) {
    verifier_adresse(adresse);
    pile[adresse].valeur.booleen = valeur;
    pile[adresse].est_initialisee = 1;
}

/* Ecrit un caractère dans la pile */
void ecrire_pile_caractere(int adresse, char valeur) {
    verifier_adresse(adresse);
    pile[adresse].valeur.caractere = valeur;
    pile[adresse].est_initialisee = 1;
}

/* Marque une cellule comme initialisée */
void marquer_initialisee(int adresse) {
    verifier_adresse(adresse);
    pile[adresse].est_initialisee = 1;
}


/* pour améliorer la lisibilité  et l'affichage de notre pile : */
/* Trouve le type d'une variable à une adresse donnée dans la région courante
 * Retourne 1 si trouvé, 0 sinon
 */
static int trouver_type_a_adresse(int adresse, int* type_resultat, Nature* nature_resultat) {
    int num_decl, adresse_var;
    int decalage_retour;
    
    num_decl = 0;
    while (num_decl < MAX_DECLARATIONS) {
        if (tab_declarations[num_decl].nature == NATURE_VAR &&
            tab_declarations[num_decl].region == region_courante) {
            
            if (region_courante == 0) {
                adresse_var = tab_declarations[num_decl].execution;
            } else {
                decalage_retour = determiner_si_fonction(region_courante) ? 1 : 0;
                adresse_var = BC + tab_declarations[num_decl].execution + decalage_retour;
            }
            
            if (adresse_var == adresse) {
                *type_resultat = obtenir_type(num_decl);
                *nature_resultat = obtenir_nature(*type_resultat);
                return 1;
            }
        }
        num_decl++;
    }
    
    return 0;
}

void afficher_pile_zone(int debut, int fin) {
    int i, type_var, trouve;
    int NIS_courante, debut_champs;
    Nature nature_var;
    
    if (debut < 0) debut = 0;
    if (fin >= TAILLE_PILE) fin = TAILLE_PILE - 1;
    if (debut > fin) return;
    
    printf("\n");
    printf("Index  Init  Valeur                Type\n");
    printf("-----  ----  --------------------  ----------\n");
    
    if (region_courante == 0) {
        i = debut;
        while (i <= fin) {
            printf("%5d   %c    ", i, pile[i].est_initialisee ? 'O' : 'N');
            
            trouve = trouver_type_a_adresse(i, &type_var, &nature_var);
            
            if (trouve) {
                if (pile[i].est_initialisee) {
                    if (type_var == 0) {
                        printf(JAUNE "%-20d" RESET, pile[i].valeur.entier);
                        printf("  int");
                    } else if (type_var == 1) {
                        printf(JAUNE "%-20.6f" RESET, pile[i].valeur.reel);
                        printf("  real");
                    } else if (type_var == 2) {
                        printf(JAUNE "%-20s" RESET, pile[i].valeur.booleen ? "true" : "false");
                        printf("  bool");
                    } else if (type_var == 3) {
                        printf(JAUNE "'%-19c" RESET, pile[i].valeur.caractere);
                        printf("  char");
                    } else if (nature_var == TYPE_ARRAY) {
                        printf(JAUNE "%-20d" RESET, pile[i].valeur.entier);
                        printf("  tableau");
                    } else if (nature_var == TYPE_STRUCT) {
                        printf(JAUNE "%-20d" RESET, pile[i].valeur.entier);
                        printf("  structure");
                    } else {
                        printf("%-20d", pile[i].valeur.entier);
                        printf("  ?");
                    }
                } else {
                    printf("%-20s", "(non init)");
                    if (type_var == 0) printf("  int");
                    else if (type_var == 1) printf("  real");
                    else if (type_var == 2) printf("  bool");
                    else if (type_var == 3) printf("  char");
                    else if (nature_var == TYPE_ARRAY) printf("  tableau");
                    else if (nature_var == TYPE_STRUCT) printf("  structure");
                }
            } else {
                if (pile[i].est_initialisee) {
                    printf(JAUNE "%-20d" RESET, pile[i].valeur.entier);
                } else {
                    printf("%-20s", "(non init)");
                }
            }
            
            printf("\n");
            i++;
        }
        printf("\n");
        return;
    }
    
    NIS_courante = tab_regions[region_courante].nis;
    
    if (determiner_si_fonction(region_courante)) {
        debut_champs = BC + NIS_courante + 2;
    } else {
        debut_champs = BC + NIS_courante + 1;
    }
    
    i = debut;
    while (i <= fin) {
        printf("%5d   %c    ", i, pile[i].est_initialisee ? 'O' : 'N');
        
        if (i == BC) {
            printf(BLEU "%-20d" RESET, pile[i].valeur.entier);
            printf("  " BLEU "BC (chaine dyn)" RESET);
            
        } else if (i > BC && i <= BC + NIS_courante) {
            printf(VERT "%-20d" RESET, pile[i].valeur.entier);
            printf("  chaine stat[%d]", i - BC);
            
        } else if (i == BC + NIS_courante + 1 && determiner_si_fonction(region_courante)) {
            if (pile[i].est_initialisee) {
                printf(GRIS GRAS "%-20d" RESET, pile[i].valeur.entier);
            } else {
                printf("%-20s", "(non init)");
            }
            printf("  " GRIS GRAS "retour fct" RESET);
            
        } else if (i >= debut_champs) {
            trouve = trouver_type_a_adresse(i, &type_var, &nature_var);
            
            if (trouve) {
                if (pile[i].est_initialisee) {
                    if (type_var == 0) {
                        printf(JAUNE "%-20d" RESET, pile[i].valeur.entier);
                        printf("  int");
                    } else if (type_var == 1) {
                        printf(JAUNE "%-20.6f" RESET, pile[i].valeur.reel);
                        printf("  real");
                    } else if (type_var == 2) {
                        printf(JAUNE "%-20s" RESET, pile[i].valeur.booleen ? "true" : "false");
                        printf("  bool");
                    } else if (type_var == 3) {
                        printf(JAUNE "'%-19c" RESET, pile[i].valeur.caractere);
                        printf("  char");
                    } else if (nature_var == TYPE_ARRAY) {
                        printf(JAUNE "%-20d" RESET, pile[i].valeur.entier);
                        printf("  tableau");
                    } else if (nature_var == TYPE_STRUCT) {
                        printf(JAUNE "%-20d" RESET, pile[i].valeur.entier);
                        printf("  structure");
                    } else {
                        printf("%-20d", pile[i].valeur.entier);
                        printf("  ?");
                    }
                } else {
                    printf("%-20s", "(non init)");
                    if (type_var == 0) printf("  int");
                    else if (type_var == 1) printf("  real");
                    else if (type_var == 2) printf("  bool");
                    else if (type_var == 3) printf("  char");
                    else if (nature_var == TYPE_ARRAY) printf("  tableau");
                    else if (nature_var == TYPE_STRUCT) printf("  structure");
                }
            } else {
                if (pile[i].est_initialisee) {
                    printf(JAUNE "%-20d" RESET, pile[i].valeur.entier);
                } else {
                    printf("%-20s", "(non init)");
                }
            }
            
        } else {
            printf("%-20d", pile[i].valeur.entier);
        }
        
        printf("\n");
        i++;
    }
    printf("\n");
}

void afficher_pile_complete() {
    int NIS_courante, debut_zone, i, fin_zone;
    
    printf("\n");
    printf("Pile execution\n");
    printf("BC = %d, Region = %d\n", BC, region_courante);
    
    if (region_courante == 0) {
        printf("\n");
        printf("Zone programme principal [0..%d]\n", tab_regions[0].taille - 1);
        afficher_pile_zone(0, tab_regions[0].taille - 1);
        return;
    }

    NIS_courante = tab_regions[region_courante].nis;
    
    printf("\n");
    printf("Zone region %d\n", region_courante);
    printf("Chainage dynamique : pile[%d] = %d\n", BC, pile[BC].valeur.entier);
    
    i = 1;
    while (i <= NIS_courante) {
        printf("Chainage statique[%d] : pile[%d] = %d\n", 
               i, BC + i, pile[BC + i].valeur.entier);
        i++;
    }
    
    printf("Variables : pile[%d..]\n", BC + NIS_courante + 1);
    
    debut_zone = BC;
    
    fin_zone = BC + tab_regions[region_courante].taille - 1;
    if (determiner_si_fonction(region_courante)) {
        fin_zone++;  /* Inclure la dernière variable locale */
    }
    
    afficher_pile_zone(debut_zone, fin_zone);
}