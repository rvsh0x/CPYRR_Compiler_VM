#include <stdio.h>
#include <stdlib.h>
#include "tab_representations.h"
#include "tab_declarations.h"
#include "tab_lexico.h"
#include "couleurs.h"
#include "erreurs.h"

int tab_representation[MAX_REPRESENTATION];
int ipcv;

int nb_champs;
int inc;

int nb_params;
int inp;

int nb_dimensions;
int ina;
int type_base;

static int proc_fonc; /* 0 = procédure, 1 = fonction */

PointEntree points_entree[MAX_POINTS_ENTREE];
int nb_points_entree;

void initialiser_tab_representation() {
    int i;
    
    i = 0;
    while (i < MAX_REPRESENTATION) {
        tab_representation[i] = -1;
        i++;
    }
    
    ipcv = 0;
    nb_points_entree = 0;
    
    nb_champs = 0;
    inc = 0;
    
    nb_params = 0;
    inp = 0;
    
    nb_dimensions = 0;
    ina = 0;
    type_base = -1;
}

static void enregistrer_point_entree(int index, char type) {
    if (nb_points_entree >= MAX_POINTS_ENTREE) {
        fprintf(stderr, ROUGE "Erreur : trop de points d'entrée\n" RESET);
        exit(EXIT_FAILURE);
    }
    
    points_entree[nb_points_entree].index = index;
    points_entree[nb_points_entree].type = type;
    nb_points_entree++;
}

void commencer_struct() {
    nb_champs = 0;
    inc = ipcv;
    ipcv++;
}

void ajouter_champ_struct(int num_lex, int type) {
    int i;
    /* Vérification sémantique ... peut être elle va bouger : */
    /* Verifier doublon */
    for (i = 0; i < nb_champs; i++) {
        if (tab_representation[inc + 1 + i * 3] == num_lex) {
            ajouter_erreur(ERR_SEMANTIQUE, ligne_courante,
                           "Champ '%s' déclaré plusieurs fois dans la structure",
                           obtenir_lexeme(num_lex));
            return;  /* Ne pas ajouter */
        }
    }
    if (ipcv + 3 > MAX_REPRESENTATION) {
        fprintf(stderr, ROUGE "Erreur : table de représentation pleine\n" RESET);
        exit(EXIT_FAILURE);
    }
    
    tab_representation[ipcv++] = num_lex;
    tab_representation[ipcv++] = type;
    tab_representation[ipcv++] = -1;
    nb_champs++;
}

int finaliser_struct() {
    tab_representation[inc] = nb_champs;
    enregistrer_point_entree(inc, 'S');
    return inc;
}

void commencer_array() {
    nb_dimensions = 0;
    ina = ipcv;
    ipcv += 2;
}

void ajouter_dimension_array(int borne_inf, int borne_sup) {
    if (ipcv + 2 > MAX_REPRESENTATION) {
        fprintf(stderr, ROUGE "Erreur : table de représentation pleine\n" RESET);
        exit(EXIT_FAILURE);
    }
    
    tab_representation[ipcv++] = borne_inf;
    tab_representation[ipcv++] = borne_sup;
    nb_dimensions++;
}

int finaliser_array(int type_elements) {
    tab_representation[ina] = type_elements;
    tab_representation[ina + 1] = nb_dimensions;
    enregistrer_point_entree(ina, 'A');
    return ina;
}

void ajouter_parametre_courant(int num_lex, int type) {
    if (ipcv + 2 > MAX_REPRESENTATION) {
        fprintf(stderr, ROUGE "Erreur : table de représentation pleine\n" RESET);
        exit(EXIT_FAILURE);
    }
    
    /* Écrire num_lex et type dans la table */
    tab_representation[ipcv++] = num_lex;
    tab_representation[ipcv++] = type;
    nb_params++;

}

void commencer_procedure() {
    nb_params = 0;
    inp = ipcv;
    ipcv++;
    proc_fonc = 0;
}


int finaliser_procedure() {
    tab_representation[inp] = nb_params;
    enregistrer_point_entree(inp, 'P');
    return inp;
}

void commencer_fonction() {
    nb_params = 0;
    inp = ipcv;
    ipcv += 2;
    proc_fonc = 1;
}


int finaliser_fonction(int type_retour) {
    tab_representation[inp] = type_retour;
    tab_representation[inp + 1] = nb_params;
    enregistrer_point_entree(inp, 'F');
    return inp;
}

int obtenir_nb_champs_struct(int index) {
    if (index < 0 || index >= ipcv) {
        return -1;
    }
    return tab_representation[index];
}

void obtenir_info_champ(int index_struct, int num_champ, int* num_lex, int* type) {
    int offset;
    
    offset = 1 + num_champ * 3;
    
    if (num_lex != NULL) {
        *num_lex = tab_representation[index_struct + offset];
    }
    if (type != NULL) {
        *type = tab_representation[index_struct + offset + 1];
    }
}

/* Recherche un champ par son num_lex (pour la VM) */
void obtenir_info_champ_par_lex(int index_struct, int num_lex_cherche, 
                                 int* type, int* deplacement) {
    int nb_ch, i, position, num_lex_actuel;
    int trouve;
    
    /* Vérifier index valide */
    if (index_struct < 0 || index_struct >= ipcv) {
        if (type != NULL) *type = -1;
        if (deplacement != NULL) *deplacement = -1;
        return;
    }
    
    /* Nombre de champs */
    nb_ch = tab_representation[index_struct];
    
    /* Rechercher le champ */
    trouve = 0;
    i = 0;
    position = index_struct + 1;
    
    while (i < nb_ch && trouve == 0) {
        num_lex_actuel = tab_representation[position];
        
        if (num_lex_actuel == num_lex_cherche) {
            /* Champ trouvé */
            if (type != NULL) {
                *type = tab_representation[position + 1];
            }
            if (deplacement != NULL) {
                *deplacement = tab_representation[position + 2];
            }
            trouve = 1;
        } else {
            /* Passer au champ suivant */
            position = position + 3;
            i++;
        }
    }
    
    /* Champ non trouvé */
    if (trouve == 0) {
        if (type != NULL) *type = -1;
        if (deplacement != NULL) *deplacement = -1;
    }
}


int obtenir_nb_dimensions_array(int index) {
    if (index < 0 || index + 1 >= ipcv) {
        return -1;
    }
    return tab_representation[index + 1];
}

void obtenir_bornes_dimension(int index_array, int num_dim, int* borne_inf, int* borne_sup) {
    int offset;
    
    offset = 2 + num_dim * 2;
    
    if (borne_inf != NULL) {
        *borne_inf = tab_representation[index_array + offset];
    }
    if (borne_sup != NULL) {
        *borne_sup = tab_representation[index_array + offset + 1];
    }
}

int obtenir_type_elements_array(int index) {
    if (index < 0 || index >= ipcv) {
        return -1;
    }
    return tab_representation[index];
}

int obtenir_nb_params(int index) {
    int point_type, i;
    
    point_type = ' ';
    i = 0;
    while (i < nb_points_entree) {
        if (points_entree[i].index == index) {
            point_type = points_entree[i].type;
            i = nb_points_entree;
        }
        i++;
    }
    
    if (point_type == 'F') {
        return tab_representation[index + 1];
    } else {
        return tab_representation[index];
    }
}

void obtenir_info_param(int index_fonction, int num_param, int* num_lex, int* type) {
    int offset, point_type, i;
    
    point_type = ' ';
    i = 0;
    while (i < nb_points_entree) {
        if (points_entree[i].index == index_fonction) {
            point_type = points_entree[i].type;
            i = nb_points_entree;
        }
        i++;
    }
    
    if (point_type == 'F') {
        offset = 2 + num_param * 2;
    } else {
        offset = 1 + num_param * 2;
    }
    
    if (num_lex != NULL) {
        *num_lex = tab_representation[index_fonction + offset];
    }
    if (type != NULL) {
        *type = tab_representation[index_fonction + offset + 1];
    }
}

int obtenir_type_retour_fonction(int index) {
    if (index < 0 || index >= ipcv) {
        return -1;
    }
    return tab_representation[index];
}


void remplir_deplacements_structures() {
    int i, index_struct, nb_ch, deplacement, type_champ, taille;
    int j, offset;
    
    i = 0;
    while (i < nb_points_entree) {
        if (points_entree[i].type == 'S') {
            index_struct = points_entree[i].index;
            nb_ch = tab_representation[index_struct];
            
            deplacement = 0;
            
            j = 0;
            while (j < nb_ch) {
                offset = index_struct + 1 + j * 3;
                type_champ = tab_representation[offset + 1];
                
                /* Stocker le déplacement */
                tab_representation[offset + 2] = deplacement;
                
                /* Calculer la taille avec la fonction partagée */
                taille = calculer_taille_type(type_champ);  /* ← Réutilisation ! */
                deplacement += taille;
                
                j++;
            }
        }
        i++;
    }
}

/* Helpers pour sauvegarder chaque type d'entrée */
static void sauvegarder_struct(FILE* f, int index) {
    int nb_ch, i, offset;
    
    nb_ch = tab_representation[index];
    fprintf(f, "STRUCT %d: nb_champs=%d\n", index, nb_ch);
    
    for (i = 0; i < nb_ch; i++) {
        offset = index + 1 + i * 3;
        fprintf(f, "  CHAMP %d: num_lex=%d type=%d deplacement=%d\n",
                i,
                tab_representation[offset],
                tab_representation[offset + 1],
                tab_representation[offset + 2]);
    }
}

static void sauvegarder_array(FILE* f, int index) {
    int type_elem, nb_dims, i, offset;
    
    type_elem = tab_representation[index];
    nb_dims = tab_representation[index + 1];
    fprintf(f, "ARRAY %d: type_elem=%d nb_dims=%d\n", index, type_elem, nb_dims);
    
    for (i = 0; i < nb_dims; i++) {
        offset = index + 2 + i * 2;
        fprintf(f, "  DIM %d: borne_inf=%d borne_sup=%d\n",
                i,
                tab_representation[offset],
                tab_representation[offset + 1]);
    }
}

static void sauvegarder_procedure(FILE* f, int index) {
    int nb_params, i, offset;
    
    nb_params = tab_representation[index];
    fprintf(f, "PROCEDURE %d: nb_params=%d\n", index, nb_params);

    for (i = 0; i < nb_params; i++) {
        offset = index + 1 + i * 2;
        fprintf(f, "  PARAMETRE %d: num_lex=%d type=%d\n",
                i,
                tab_representation[offset],
                tab_representation[offset + 1]);
    }
}

static void sauvegarder_fonction(FILE* f, int index) {
    int type_retour, nb_params, i, offset;
    
    type_retour = tab_representation[index];
    nb_params = tab_representation[index + 1];
    fprintf(f, "FONCTION %d: type_retour=%d nb_params=%d\n", index, type_retour, nb_params);
    
    for (i = 0; i < nb_params; i++) {
        offset = index + 2 + i * 2;
        fprintf(f, "  PARAMETRE %d: num_lex=%d type=%d\n",
                i,
                tab_representation[offset],
                tab_representation[offset + 1]);
    }
}

/* Format du fichier representations.txt :
 * 
 * nb_entrees: 50
 * nb_points_entree: 3
 * 
 * POINT_ENTREE 0: type=S index=10
 * STRUCT 10: nb_champs=2
 *   CHAMP 0: num_lex=5 type=0 deplacement=0
 *   CHAMP 1: num_lex=6 type=1 deplacement=1
 * 
 * POINT_ENTREE 1: type=P index=20
 * PROC 20: nb_params=2
 *   PARAM 0: num_lex=7 type=0
 *   PARAM 1: num_lex=8 type=1
 * ...
 */
int sauvegarder_representations(const char* chemin_fichier) {
    FILE* f;
    int i;
    
    f = fopen(chemin_fichier, "w");
    if (f == NULL) {
        return 0;
    }
    
    /* Métadonnées */
    fprintf(f, "nb_entrees: %d\n", ipcv);
    fprintf(f, "nb_points_entree: %d\n\n", nb_points_entree);
    
    /* Sauvegarder chaque point d'entrée */
    for (i = 0; i < nb_points_entree; i++) {
        fprintf(f, "POINT_ENTREE %d: type=%c index=%d\n",
                i,
                points_entree[i].type,
                points_entree[i].index);
        
        switch (points_entree[i].type) {
            case 'S':
                sauvegarder_struct(f, points_entree[i].index);
                break;
            case 'A':
                sauvegarder_array(f, points_entree[i].index);
                break;
            case 'P':
                sauvegarder_procedure(f, points_entree[i].index);
                break;
            case 'F':
                sauvegarder_fonction(f, points_entree[i].index);
                break;
        }
        
        if (i < nb_points_entree - 1) {
            fprintf(f, "\n");
        }
    }
    
    fclose(f);
    return 1;
}

static const char* type_base_vers_chaine(int type) {
    switch (type) {
        case 0: return "int";
        case 1: return "real";
        case 2: return "bool";
        case 3: return "char";
        default: return "???";
    }
}

static void afficher_entree_struct(int index) {
    int nb_ch, i, num_lex, type;
    
    nb_ch = tab_representation[index];
    
    printf(CYAN "[%d] " RESET, index);
    printf(BLEU "%d" RESET " champs\n", nb_ch);
    
    i = 0;
    while (i < nb_ch) {
        num_lex = tab_representation[index + 1 + i * 3];
        type = tab_representation[index + 1 + i * 3 + 1];
        
        printf("    [%d] " VERT "%d : %s" RESET " : ", 
               index + 1 + i * 3, num_lex, obtenir_lexeme(num_lex));
        
        if (type >= 0 && type <= 3) {
            printf(JAUNE "%s" RESET, type_base_vers_chaine(type));
        } else {
            printf(JAUNE "decl[%d]" RESET, type);
        }
        
        printf(" (déplacement : " ROUGE "%d" RESET ")\n", 
               tab_representation[index + 1 + i * 3 + 2]);
        
        i++;
    }
}

static void afficher_entree_array(int index) {
    int type_elem, nb_dims, i, borne_inf, borne_sup;
    
    type_elem = tab_representation[index];
    nb_dims = tab_representation[index + 1];
    
    printf(CYAN "[%d] " RESET "array of ", index);
    
    if (type_elem >= 0 && type_elem <= 3) {
        printf(JAUNE "%s" RESET, type_base_vers_chaine(type_elem));
    } else {
        printf(JAUNE "decl[%d]" RESET, type_elem);
    }
    
    printf(", " BLEU "%d" RESET " dimension(s)\n", nb_dims);
    
    i = 0;
    while (i < nb_dims) {
        borne_inf = tab_representation[index + 2 + i * 2];
        borne_sup = tab_representation[index + 2 + i * 2 + 1];
        
        printf("    Dimension %d : [" MAGENTA "%d" RESET ".." MAGENTA "%d" RESET "]\n",
               i + 1, borne_inf, borne_sup);
        
        i++;
    }
}

static void afficher_entree_fonction(int index) {
    int type_retour, nb_par, i, num_lex, type;
    
    type_retour = tab_representation[index];
    nb_par = tab_representation[index + 1];
    
    printf(CYAN "[%d] " RESET "fonction -> " GRAS "%s" RESET ", ",
           index, type_base_vers_chaine(type_retour));
    printf(BLEU "%d" RESET " paramètre(s)\n", nb_par);
    
    i = 0;
    while (i < nb_par) {
        num_lex = tab_representation[index + 2 + i * 2];
        type = tab_representation[index + 2 + i * 2 + 1];
        
        printf("    [%d] " VERT "%d : %s" RESET " : " JAUNE "%s" RESET "\n",
               index + 2 + i * 2,
               num_lex,
               obtenir_lexeme(num_lex),
               type_base_vers_chaine(type));
        
        i++;
    }
}

static void afficher_entree_procedure(int index) {
    int nb_par, i, num_lex, type;
    
    nb_par = tab_representation[index];
    
    printf(CYAN "[%d] " RESET "procédure, " BLEU "%d" RESET " paramètre(s)\n",
           index, nb_par);
    
    i = 0;
    while (i < nb_par) {
        num_lex = tab_representation[index + 1 + i * 2];
        type = tab_representation[index + 1 + i * 2 + 1];
        
        printf("    [%d] " VERT "%d : %s" RESET " : " JAUNE "%s" RESET "\n",
               index + 1 + i * 2,
               num_lex,
               obtenir_lexeme(num_lex),
               type_base_vers_chaine(type));
        
        i++;
    }
}

void afficher_tab_representation() {
    int i;

    printf("\n--- TABLE DE REPRÉSENTATION DES TYPES ---\n");
    printf("Prochaine case libre (ipcv) : %d\n", ipcv);
    printf("Nombre d'entrées : %d\n\n", nb_points_entree);
    printf("Contenu de la table (Code couleur) :\n");
    printf(CYAN "index_rep" RESET " | "
           BLEU "nombre de paramètres / champs / dimensions" RESET " | "
           VERT "num_lex : lexeme" RESET " | "
           JAUNE "type" RESET " | "
           GRAS "type_retour_fct" RESET " | "
           MAGENTA "borne_inf / borne_sup" RESET " | "
           ROUGE "déplacement" RESET "\n\n");
    
    if (ipcv == 0) {
        printf("(Table vide)\n");
        printf("------------------------------------------\n\n");
        return;
    }
    
    i = 0;
    while (i < nb_points_entree) {
        switch (points_entree[i].type) {
            case 'S':
                afficher_entree_struct(points_entree[i].index);
                break;
            case 'A':
                afficher_entree_array(points_entree[i].index);
                break;
            case 'F':
                afficher_entree_fonction(points_entree[i].index);
                break;
            case 'P':
                afficher_entree_procedure(points_entree[i].index);
                break;
        }
        
        if (i < nb_points_entree - 1) {
            printf("\n");
        }
        
        i++;
    }

    printf("------------------------------------------\n\n");
}