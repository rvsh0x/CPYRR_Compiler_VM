#include <stdio.h>
#include <stdlib.h>
#include "vm/charg_rep.h"
#include "tab_representations.h"

/* Variables globales pour le suivi du chargement */
static int nb_cases_attendues = 0;
static int nb_points_attendu = 0;
static int ipcv_courant = 0;
static int nb_points_charge = 0;

/* Variables pour le traitement en cours */
static int index_courant = -1;
static int nb_attendu = 0;
static int compteur = 0;


void init_chargement_representations(void) {
    nb_cases_attendues = 0;
    nb_points_attendu = 0;
    ipcv_courant = 0;
    nb_points_charge = 0;
    
    index_courant = -1;
    nb_attendu = 0;
    compteur = 0;
    
    initialiser_tab_representation();
}


void traiter_entete_representations(int nb_entrees, int nb_points) {
    if (nb_entrees < 0 || nb_entrees > MAX_REPRESENTATION) {
        fprintf(stderr, "Erreur: nb_entrees invalide (%d)\n", nb_entrees);
        return;
    }
    
    if (nb_points < 0 || nb_points > MAX_POINTS_ENTREE) {
        fprintf(stderr, "Erreur: nb_points invalide (%d)\n", nb_points);
        return;
    }
    
    nb_cases_attendues = nb_entrees;
    nb_points_attendu = nb_points;
}


void traiter_point_entree(int num, char type, int index) {
    if (num < 0 || num >= MAX_POINTS_ENTREE) {
        fprintf(stderr, "Erreur: num point d'entree hors limites (%d)\n", num);
        return;
    }
    
    if (index < 0 || index >= MAX_REPRESENTATION) {
        fprintf(stderr, "Erreur: index representation hors limites (%d)\n", index);
        return;
    }
    
    if (nb_points_charge >= MAX_POINTS_ENTREE) {
        fprintf(stderr, "Erreur: trop de points d'entree\n");
        return;
    }
    
    points_entree[nb_points_charge].index = index;
    points_entree[nb_points_charge].type = type;
    nb_points_charge++;
}


void traiter_struct_debut(int index, int nb_champs) {
    if (index < 0 || index >= MAX_REPRESENTATION) {
        fprintf(stderr, "Erreur: index struct hors limites (%d)\n", index);
        return;
    }
    
    if (nb_champs < 0) {
        fprintf(stderr, "Erreur: nb_champs negatif (%d)\n", nb_champs);
        return;
    }
    
    index_courant = index;
    nb_attendu = nb_champs;
    compteur = 0;
    
    tab_representation[index] = nb_champs;
    
    /* Mettre à jour le compteur de cases utilisées */
    ipcv_courant = index + 1 + nb_champs * 3;
}


void traiter_champ(int num, int num_lex, int type, int deplacement) {
    int offset;
    
    if (index_courant < 0) {
        fprintf(stderr, "Erreur: pas de struct courante\n");
        return;
    }
    
    if (num < 0 || num >= nb_attendu) {
        fprintf(stderr, "Erreur: num champ hors limites (%d)\n", num);
        return;
    }
    
    offset = index_courant + 1 + num * 3;
    
    if (offset + 2 >= MAX_REPRESENTATION) {
        fprintf(stderr, "Erreur: depassement table\n");
        return;
    }
    
    tab_representation[offset] = num_lex;
    tab_representation[offset + 1] = type;
    tab_representation[offset + 2] = deplacement;
    
    compteur++;
}


void traiter_struct_fin(void) {
    if (index_courant < 0) {
        return;
    }
    
    if (compteur != nb_attendu) {
        fprintf(stderr, "Attention: %d/%d champs charges\n", compteur, nb_attendu);
    }
    
    index_courant = -1;
    nb_attendu = 0;
    compteur = 0;
}


void traiter_array_debut(int index, int type_elem, int nb_dims) {
    if (index < 0 || index >= MAX_REPRESENTATION) {
        fprintf(stderr, "Erreur: index array hors limites (%d)\n", index);
        return;
    }
    
    if (nb_dims < 0) {
        fprintf(stderr, "Erreur: nb_dims negatif (%d)\n", nb_dims);
        return;
    }
    
    index_courant = index;
    nb_attendu = nb_dims;
    compteur = 0;
    
    tab_representation[index] = type_elem;
    tab_representation[index + 1] = nb_dims;
    
    /* Mettre à jour le compteur de cases utilisées */
    ipcv_courant = index + 2 + nb_dims * 2;
}


void traiter_dim(int num, int borne_inf, int borne_sup) {
    int offset;
    
    if (index_courant < 0) {
        fprintf(stderr, "Erreur: pas d'array courant\n");
        return;
    }
    
    if (num < 0 || num >= nb_attendu) {
        fprintf(stderr, "Erreur: num dim hors limites (%d)\n", num);
        return;
    }
    
    offset = index_courant + 2 + num * 2;
    
    if (offset + 1 >= MAX_REPRESENTATION) {
        fprintf(stderr, "Erreur: depassement table\n");
        return;
    }
    
    tab_representation[offset] = borne_inf;
    tab_representation[offset + 1] = borne_sup;
    
    compteur++;
}


void traiter_array_fin(void) {
    if (index_courant < 0) {
        return;
    }
    
    if (compteur != nb_attendu) {
        fprintf(stderr, "Attention: %d/%d dimensions chargees\n", compteur, nb_attendu);
    }
    
    index_courant = -1;
    nb_attendu = 0;
    compteur = 0;
}


void traiter_proc_debut(int index, int nb_params) {
    if (index < 0 || index >= MAX_REPRESENTATION) {
        fprintf(stderr, "Erreur: index proc hors limites (%d)\n", index);
        return;
    }
    
    if (nb_params < 0) {
        fprintf(stderr, "Erreur: nb_params negatif (%d)\n", nb_params);
        return;
    }
    
    index_courant = index;
    nb_attendu = nb_params;
    compteur = 0;
    
    tab_representation[index] = nb_params;
    
    /* Mettre à jour le compteur de cases utilisées */
    ipcv_courant = index + 1 + nb_params * 2;
}


void traiter_param(int num, int num_lex, int type) {
    int offset;
    char point_type;
    int i;
    
    if (index_courant < 0) {
        fprintf(stderr, "Erreur: pas de proc/fct courante\n");
        return;
    }
    
    if (num < 0 || num >= nb_attendu) {
        fprintf(stderr, "Erreur: num param hors limites (%d)\n", num);
        return;
    }
    
    /* Déterminer si c'est une fonction ou une procédure */
    point_type = ' ';
    i = 0;
    while (i < nb_points_charge) {
        if (points_entree[i].index == index_courant) {
            point_type = points_entree[i].type;
            i = nb_points_charge;
        }
        i++;
    }
    
    /* Calcul de l'offset selon le type */
    if (point_type == 'F') {
        /* Fonction: type_retour occupe [index], params commencent à [index+2] */
        offset = index_courant + 2 + num * 2;
    } else {
        /* Procédure: params commencent à [index+1] */
        offset = index_courant + 1 + num * 2;
    }
    
    if (offset + 1 >= MAX_REPRESENTATION) {
        fprintf(stderr, "Erreur: depassement table\n");
        return;
    }
    
    tab_representation[offset] = num_lex;
    tab_representation[offset + 1] = type;
    
    compteur++;
}


void traiter_proc_fin(void) {
    if (index_courant < 0) {
        return;
    }
    
    if (compteur != nb_attendu) {
        fprintf(stderr, "Attention: %d/%d params charges\n", compteur, nb_attendu);
    }
    
    index_courant = -1;
    nb_attendu = 0;
    compteur = 0;
}


void traiter_fct_debut(int index, int type_retour, int nb_params) {
    if (index < 0 || index >= MAX_REPRESENTATION) {
        fprintf(stderr, "Erreur: index fct hors limites (%d)\n", index);
        return;
    }
    
    if (nb_params < 0) {
        fprintf(stderr, "Erreur: nb_params negatif (%d)\n", nb_params);
        return;
    }
    
    index_courant = index;
    nb_attendu = nb_params;
    compteur = 0;
    
    tab_representation[index] = type_retour;
    tab_representation[index + 1] = nb_params;
    
    /* Mettre à jour le compteur de cases utilisées */
    ipcv_courant = index + 2 + nb_params * 2;
}


void traiter_fct_fin(void) {
    if (index_courant < 0) {
        return;
    }
    
    if (compteur != nb_attendu) {
        fprintf(stderr, "Attention: %d/%d params charges\n", compteur, nb_attendu);
    }
    
    index_courant = -1;
    nb_attendu = 0;
    compteur = 0;
}


void finaliser_chargement_representations(void) {
    if (ipcv_courant != nb_cases_attendues) {
        fprintf(stderr, "Attention: %d cases utilisees, %d attendues\n",
                ipcv_courant, nb_cases_attendues);
    }
    
    if (nb_points_charge != nb_points_attendu) {
        fprintf(stderr, "Attention: %d points charges, %d attendus\n",
                nb_points_charge, nb_points_attendu);
    }
    
    /* Mise à jour des variables globales */
    ipcv = nb_cases_attendues;
    nb_points_entree = nb_points_charge;
}


int charger_representations(const char* chemin) {
    extern FILE* yyin;
    extern int yyparse(void);
    FILE* fichier;
    int resultat;
    
    if (chemin == NULL) {
        fprintf(stderr, "Erreur: chemin NULL\n");
        return 0;
    }
    
    fichier = fopen(chemin, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", chemin);
        return 0;
    }
    
    yyin = fichier;
    init_chargement_representations();
    resultat = yyparse();
    fclose(fichier);
    
    if (resultat != 0) {
        fprintf(stderr, "Erreur: parsing echoue\n");
        return 0;
    }
    
    finaliser_chargement_representations();
    return 1;
}