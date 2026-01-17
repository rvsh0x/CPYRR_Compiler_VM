#include <stdio.h>
#include "semantique.h"
#include "utils_communs.h"
#include "tab_representations.h"

const char* nom_type(int type) {
    switch (type) {
        case 0:  return "entier";
        case 1:  return "réel";
        case 2:  return "booléen";
        case 3:  return "caractère";
        case -1: return "inconnu";
        case -2: return "void (procédure)";
        default: return "type complexe (struct/tableau)";
    }
}

int est_type_simple(int type) {
    return (type >= 0 && type <= 3);
}

int est_type_numerique(int type) {
    return (type == 0 || type == 1);
}

/* Vérifie si deux chaînes ont les mêmes tailles
 * Retourne 1 si oui, 0 sinon
 */
int chaines_meme_taille(int type1, int type2) {
    int index_rep1, index_rep2;
    int nb_dim1, nb_dim2;
    int borne_inf1, borne_sup1, borne_inf2, borne_sup2;
    
    /* Vérifier que les deux sont des chaînes */
    if (!est_type_chaine(type1) || !est_type_chaine(type2)) {
        return 0;
    }
    
    /* Obtenir les représentations */
    index_rep1 = obtenir_type(type1);
    index_rep2 = obtenir_type(type2);
    
    /* Vérifier le nombre de dimensions (devrait être 1 pour les chaînes) */
    nb_dim1 = obtenir_nb_dimensions_array(index_rep1);
    nb_dim2 = obtenir_nb_dimensions_array(index_rep2);
    
    if (nb_dim1 != 1 || nb_dim2 != 1) {
        return 0;  /* Pas des chaînes valides */
    }
    
    /* Comparer les bornes */
    obtenir_bornes_dimension(index_rep1, 0, &borne_inf1, &borne_sup1);
    obtenir_bornes_dimension(index_rep2, 0, &borne_inf2, &borne_sup2);

    /* Même taille si même nombre de cases */
    return ((borne_sup1 - borne_inf1) == (borne_sup2 - borne_inf2));
}