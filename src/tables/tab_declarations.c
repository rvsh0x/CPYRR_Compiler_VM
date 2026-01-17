#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tab_declarations.h"
#include "association_nom.h"

/* Indices ... */
static int index_procedure_courante = -1;
static int index_fonction_courante = -1;

/* Stack pour gérer l'imbrication */
static int stack_indices_fonctions[100];
static int sommet_stack_fonctions = -1;

static int stack_indices_procedures[100];
static int sommet_stack_procedures = -1;

Declaration tab_declarations[MAX_DECLARATIONS];
int prochaine_case_libre;

void initialiser_tab_declarations() {
    int i;
    
    i = 0;
    while (i < MAX_DECLARATIONS) {
        tab_declarations[i].nature = -1;
        tab_declarations[i].suivant = -1;
        tab_declarations[i].region = -1;
        tab_declarations[i].description = -1;
        tab_declarations[i].execution = -1;
        i++;
    }
    
    tab_declarations[0].nature = TYPE_BASE;
    tab_declarations[0].region = 0;
    tab_declarations[0].execution = 1;
    
    tab_declarations[1].nature = TYPE_BASE;
    tab_declarations[1].region = 0;
    tab_declarations[1].execution = 1;
    
    tab_declarations[2].nature = TYPE_BASE;
    tab_declarations[2].region = 0;
    tab_declarations[2].execution = 1;
    
    tab_declarations[3].nature = TYPE_BASE;
    tab_declarations[3].region = 0;
    tab_declarations[3].execution = 1;
    
    prochaine_case_libre = MAX_LEXEMES;
    
    /* Initialiser les stacks */
    sommet_stack_fonctions = -1;
    sommet_stack_procedures = -1;
}


static int ajouter_declaration(int num_lex, Nature nature, int region, int description) {
    int doublon;
    int index;
    int dernier;
    
    /* Vérifier doublon dans la région */
    doublon = chercher_dans_region(num_lex, region, nature);
    if (doublon != -1) {
        ajouter_erreur(ERR_SEMANTIQUE, ligne_courante,
                      "'%s' déjà déclaré dans cette région",
                      obtenir_lexeme(num_lex));
        return -1;
    }
    
    /* Première déclaration de ce nom */
    if (tab_declarations[num_lex].nature == -1) {
        tab_declarations[num_lex].nature = nature;
        tab_declarations[num_lex].suivant = -1;
        tab_declarations[num_lex].region = region;
        tab_declarations[num_lex].description = description;
        tab_declarations[num_lex].execution = -1;
        return num_lex;
    }
    
    /* Zone de débordement */
    if (prochaine_case_libre >= MAX_DECLARATIONS) {
        fprintf(stderr, ROUGE "Erreur : table des declarations pleine\n" RESET);
        exit(1);
    }
    
    /* Parcourir jusqu'à la FIN de la chaîne */
    index = num_lex;
    dernier = index;
    while (index != -1) {
        dernier = index;
        index = tab_declarations[index].suivant;
    }
    
    /* Ajouter à la fin */
    tab_declarations[prochaine_case_libre].nature = nature;
    tab_declarations[prochaine_case_libre].suivant = -1;  /* Fin de chaîne */
    tab_declarations[prochaine_case_libre].region = region;
    tab_declarations[prochaine_case_libre].description = description;
    tab_declarations[prochaine_case_libre].execution = -1;
    
    /* Chaîner depuis le dernier */
    tab_declarations[dernier].suivant = prochaine_case_libre;
    
    prochaine_case_libre++;
    return prochaine_case_libre - 1;
}

int ajouter_variable(int num_lex, int region, int type_index) {
    return ajouter_declaration(num_lex, NATURE_VAR, region, type_index);
}

int ajouter_procedure(int num_lex, int region) {
    return ajouter_declaration(num_lex, NATURE_PROC, region, -1);
}

int ajouter_fonction(int num_lex, int region) {
    return ajouter_declaration(num_lex, NATURE_FCT, region, -1);
}

int ajouter_type_struct(int num_lex, int region) {
    return ajouter_declaration(num_lex, TYPE_STRUCT, region, -1);
}

int ajouter_type_array(int num_lex, int region) {
    return ajouter_declaration(num_lex, TYPE_ARRAY, region, -1);
}

int ajouter_parametre(int num_lex, int region, int type_index) {
    return ajouter_declaration(num_lex, NATURE_PARAM, region, type_index);
}

int chercher_dans_region(int num_lex, int region, Nature nature) {
    int index;
    
    if (num_lex < 0 || num_lex >= MAX_LEXEMES) {
        return -1;
    }
    
    index = num_lex;
    
    while (index != -1 && tab_declarations[index].nature != -1) {
        if (tab_declarations[index].region == region && 
            tab_declarations[index].nature == nature) {
            return index; /* Trouvé */
        }
        index = tab_declarations[index].suivant;
    }
    
    return -1; /* Non trouvé */
}

Nature obtenir_nature(int index) {
    if (index < 0 || index >= MAX_DECLARATIONS) {
        return -1;
    }
    return tab_declarations[index].nature;
}

int obtenir_type(int index) {
    if (index < 0 || index >= MAX_DECLARATIONS) {
        return -1;
    }
    return tab_declarations[index].description;
}

/* Calcule la taille d'un type récursivement */
int calculer_taille_type(int type_index) {
    Nature nature;
    int index_rep, nb_champs, i, type_champ, total;
    int nb_dims, nb_elem, borne_inf, borne_sup, type_elem;
    
    if (type_index >= 0 && type_index <= 3) {
        return 1;
    }
    
    if (type_index < 0 || type_index >= MAX_DECLARATIONS) {
        return 1;
    }
    
    nature = tab_declarations[type_index].nature;
    
    if (nature == TYPE_STRUCT) {
        index_rep = tab_declarations[type_index].description;
        nb_champs = obtenir_nb_champs_struct(index_rep);
        total = 0;
        
        i = 0;
        while (i < nb_champs) {
            obtenir_info_champ(index_rep, i, NULL, &type_champ);
            total += calculer_taille_type(type_champ);
            i++;
        }
        return total;
    }
    
    if (nature == TYPE_ARRAY) {
        index_rep = tab_declarations[type_index].description;
        nb_dims = obtenir_nb_dimensions_array(index_rep);
        type_elem = obtenir_type_elements_array(index_rep);
        
        nb_elem = 1;
        i = 0;
        while (i < nb_dims) {
            obtenir_bornes_dimension(index_rep, i, &borne_inf, &borne_sup);
            nb_elem = nb_elem * (borne_sup - borne_inf + 1);
            i++;
        }
        
        return nb_elem * calculer_taille_type(type_elem);
    }
    
    return 1;
}

void finaliser_declaration_type(int num_lex, int type_nature) {
    int index_decl, taille;
    
    if (type_nature == 1) {
        index_decl = ajouter_type_struct(num_lex, obtenir_region_courante());
        if (index_decl != -1) {
            tab_declarations[index_decl].description = inc;
            /* Calcule de la taille ici */
            taille = calculer_taille_type(index_decl);
            tab_declarations[index_decl].execution = taille;
        }
    } else if (type_nature == 2) {
        index_decl = ajouter_type_array(num_lex, obtenir_region_courante());
        if (index_decl != -1) {
            tab_declarations[index_decl].description = ina;
            /* Calcule de la taille ici */
            taille = calculer_taille_type(index_decl);
            tab_declarations[index_decl].execution = taille;
        }
    }
}


void debut_declaration_procedure(int num_lex) {
    int nouvelle_reg, parent;
    
    /* Empiler l'ancien index */
    if (index_procedure_courante != -1) {
        sommet_stack_procedures++;
        stack_indices_procedures[sommet_stack_procedures] = index_procedure_courante;
    }
    
    index_procedure_courante = ajouter_procedure(num_lex, obtenir_region_courante());
    
    parent = obtenir_region_courante();
    nouvelle_reg = creer_region(parent);
    
    if (index_procedure_courante != -1) {
        tab_declarations[index_procedure_courante].execution = nouvelle_reg;
    }
    
    empiler_region(nouvelle_reg);
    commencer_procedure();
}

void lier_procedure_representation() {
    if (index_procedure_courante != -1) {
        tab_declarations[index_procedure_courante].description = finaliser_procedure();
    }
}

void fin_declaration_procedure(arbre a) {
    int num_region, nis, deplacement, i;
    int index_rep, nb_params, p, num_lex_param, type_param, index_param;
    int taille, taille_totale;
    Nature nature;
    
    num_region = obtenir_region_courante();
    associer_arbre_region(num_region, a);
    
    nis = tab_regions[num_region].nis;
    deplacement = nis + 1;

    if (index_procedure_courante != -1) {
        index_rep = tab_declarations[index_procedure_courante].description;
        nb_params = obtenir_nb_params(index_rep);
        
        p = 0;
        while (p < nb_params) {
            obtenir_info_param(index_rep, p, &num_lex_param, &type_param);
            index_param = chercher_dans_region(num_lex_param, num_region, NATURE_PARAM);
            
            if (index_param != -1) {
                taille = calculer_taille_type(type_param);
                tab_declarations[index_param].execution = deplacement;
                deplacement += taille;
            }
            p++;
        }
    }
    
    i = 0;
    while (i < MAX_DECLARATIONS) {
        nature = tab_declarations[i].nature;
        if (tab_declarations[i].region == num_region && nature == NATURE_VAR) {
            taille = calculer_taille_type(tab_declarations[i].description);
            tab_declarations[i].execution = deplacement;
            deplacement += taille;
        }
        i++;
    }
    
    taille_totale = (nis + 1);
    i = 0;
    while (i < MAX_DECLARATIONS) {
        nature = tab_declarations[i].nature;
        if (tab_declarations[i].region == num_region &&
            (nature == NATURE_VAR || nature == NATURE_PARAM)) {
            taille_totale += calculer_taille_type(tab_declarations[i].description);
        }
        i++;
    }
    tab_regions[num_region].taille = taille_totale;
    
    depiler_region();
    
    /* Restaurer l'ancien index */
    if (sommet_stack_procedures >= 0) {
        index_procedure_courante = stack_indices_procedures[sommet_stack_procedures];
        sommet_stack_procedures--;
    } else {
        index_procedure_courante = -1;
    }
}

void debut_declaration_fonction(int num_lex) {
    int nouvelle_reg, parent;
    
    /* Empiler l'ancien index */
    if (index_fonction_courante != -1) {
        sommet_stack_fonctions++;
        stack_indices_fonctions[sommet_stack_fonctions] = index_fonction_courante;
    }
    
    index_fonction_courante = ajouter_fonction(num_lex, obtenir_region_courante());
    
    parent = obtenir_region_courante();
    nouvelle_reg = creer_region(parent);
    
    if (index_fonction_courante != -1) {
        tab_declarations[index_fonction_courante].execution = nouvelle_reg;
    }
    
    empiler_region(nouvelle_reg);
    commencer_fonction();
}

void lier_fonction_representation(int type_retour) {
    if (index_fonction_courante != -1) {
        tab_declarations[index_fonction_courante].description = finaliser_fonction(type_retour);
    }
}

void fin_declaration_fonction(arbre a) {
    int num_region, nis, deplacement, i;
    int index_rep, nb_params, p, num_lex_param, type_param, index_param;
    int taille, taille_totale;
    Nature nature;
    
    num_region = obtenir_region_courante();
    associer_arbre_region(num_region, a);
    
    nis = tab_regions[num_region].nis;
    deplacement = nis + 1;
    
    if (index_fonction_courante != -1) {
        index_rep = tab_declarations[index_fonction_courante].description;
        nb_params = obtenir_nb_params(index_rep);
        
        p = 0;
        while (p < nb_params) {
            obtenir_info_param(index_rep, p, &num_lex_param, &type_param);
            index_param = chercher_dans_region(num_lex_param, num_region, NATURE_PARAM);
            
            if (index_param != -1) {
                taille = calculer_taille_type(type_param);
                tab_declarations[index_param].execution = deplacement;
                deplacement += taille;
            }
            p++;
        }
    }
    
    i = 0;
    while (i < MAX_DECLARATIONS) {
        nature = tab_declarations[i].nature;
        if (tab_declarations[i].region == num_region && nature == NATURE_VAR) {
            taille = calculer_taille_type(tab_declarations[i].description);
            tab_declarations[i].execution = deplacement;
            deplacement += taille;
        }
        i++;
    }
    
    taille_totale = (nis + 1);
    i = 0;
    while (i < MAX_DECLARATIONS) {
        nature = tab_declarations[i].nature;
        if (tab_declarations[i].region == num_region &&
            (nature == NATURE_VAR || nature == NATURE_PARAM)) {
            taille_totale += calculer_taille_type(tab_declarations[i].description);
        }
        i++;
    }
    tab_regions[num_region].taille = taille_totale;
    
    depiler_region();
    
    /* Restaurer l'ancien index */
    if (sommet_stack_fonctions >= 0) {
        index_fonction_courante = stack_indices_fonctions[sommet_stack_fonctions];
        sommet_stack_fonctions--;
    } else {
        index_fonction_courante = -1;
    }
}
void finaliser_region_principale() {
    int i, deplacement, taille, taille_totale;
    Nature nature;

    remplir_deplacements_structures();

    /* Déplacements des variables */
    deplacement = 0;
    i = 0;
    while (i < MAX_DECLARATIONS) {
        nature = tab_declarations[i].nature;
        if (tab_declarations[i].region == 0 && nature == NATURE_VAR) {
            taille = calculer_taille_type(tab_declarations[i].description);
            tab_declarations[i].execution = deplacement;
            deplacement += taille;
        }
        i++;
    }
    
    /* Taille de la région 0 */
    taille_totale = 0;
    i = 0;
    while (i < MAX_DECLARATIONS) {
        nature = tab_declarations[i].nature;
        if (tab_declarations[i].region == 0 && nature == NATURE_VAR) {
            taille_totale += calculer_taille_type(tab_declarations[i].description);
        }
        i++;
    }
    tab_regions[0].taille = taille_totale;
}

void declarer_parametre(int num_lex, int type) {
    ajouter_parametre(num_lex, obtenir_region_courante(), type);
    ajouter_parametre_courant(num_lex, type);
}




const char* nature_decl_vers_chaine(Nature nature) {
    switch (nature) {
        case TYPE_BASE:    return "TYPE_BASE";
        case TYPE_STRUCT:  return "TYPE_STRUCT";
        case TYPE_ARRAY:   return "TYPE_ARRAY";
        case NATURE_VAR:   return "VAR";
        case NATURE_PARAM: return "PARAM";
        case NATURE_PROC:  return "PROC";
        case NATURE_FCT:   return "FCT";
        default:           return "VIDE";
    }
}

/* Format du fichier declarations.txt :
 * 
 * nb_declarations: 10
 * DECL 0: nature=VAR region=0 description=0 execution=0
 * DECL 1: nature=VAR region=0 description=1 execution=1
 * ...
 */
int sauvegarder_declarations(const char* chemin_fichier) {
    FILE* f;
    int i, nb_actives;
    
    f = fopen(chemin_fichier, "w");
    if (f == NULL) {
        return 0;
    }
    
    /* Compter les déclarations actives */
    nb_actives = 0;
    for (i = 0; i < MAX_DECLARATIONS; i++) {
        if (tab_declarations[i].region >= 0) {
            nb_actives++;
        }
    }
    
    fprintf(f, "nb_declarations: %d\n", nb_actives);
    
    /* Sauvegarder chaque déclaration active */
    for (i = 0; i < MAX_DECLARATIONS; i++) {
        if (tab_declarations[i].region >= 0) {
            fprintf(f, "DECL %d: ", i);
            fprintf(f, "nature=%s ", nature_decl_vers_chaine(tab_declarations[i].nature));
            fprintf(f, "region=%d ", tab_declarations[i].region);
            fprintf(f, "description=%d ", tab_declarations[i].description);
            fprintf(f, "execution=%d\n", tab_declarations[i].execution);
        }
    }
    
    fclose(f);
    return 1;
}

void afficher_tab_declarations() {
    int i;
    int nb_affichees;

    printf("\n--- TABLE DES DECLARATIONS ---\n");
    printf("Prochaine case libre : %d\n\n", prochaine_case_libre);
    
    printf("INDEX | NATURE      | SUIVANT | REGION | DESCR. | EXEC.\n");
    printf("------|-------------|---------|--------|--------|-------\n");
    
    /* Table primaire */
    nb_affichees = 0;
    i = 0;
    while (i < MAX_LEXEMES) {
        if (tab_declarations[i].nature != -1) {
            /* Types de base en bleu */
            if (i < 4) {
                printf(BLEU);
            }
            
            printf("%-5d | %-11s | %-7d | %-6d | %-6d | %-5d\n",
                   i,
                   nature_decl_vers_chaine(tab_declarations[i].nature),
                   tab_declarations[i].suivant,
                   tab_declarations[i].region,
                   tab_declarations[i].description,
                   tab_declarations[i].execution);
            
            if (i < 4) {
                printf(RESET);
            }
            
            nb_affichees++;
        }
        i++;
    }
    
    /* Séparateur zone débordement */
    if (prochaine_case_libre > MAX_LEXEMES) {
        printf("------|-------------|---------|--------|--------|-------\n");
        printf(JAUNE "      ZONE DE DEBORDEMENT\n" RESET);
        printf("------|-------------|---------|--------|--------|-------\n");
        
        i = MAX_LEXEMES;
        while (i < prochaine_case_libre) {
            printf("%-5d | %-11s | %-7d | %-6d | %-6d | %-5d\n",
                   i,
                   nature_decl_vers_chaine(tab_declarations[i].nature),
                   tab_declarations[i].suivant,
                   tab_declarations[i].region,
                   tab_declarations[i].description,
                   tab_declarations[i].execution);
            i++;
        }
    }

    printf("------------------------------\n");
    printf("Total declarations : %d\n", nb_affichees);
}