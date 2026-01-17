#include <stdio.h>
#include "semantique.h"
#include "utils_communs.h"
#include "construction.h"

void verifier_et_ajouter_dimension(int borne_inf, int borne_sup) {
    if (borne_inf > borne_sup) {
        ajouter_erreur(ERR_SEMANTIQUE, ligne_courante,
                       "Dimension invalide : borne inf (%d) > borne sup (%d)",
                       borne_inf, borne_sup);
    }
    
    ajouter_dimension_array(borne_inf, borne_sup);
}


NoeudType verifier_et_construire_acces_tableau(NoeudType variable, arbre liste_indices,
                                               int ligne, int col, int len) {
    int type_base, type_elements;
    Nature nature_type;
    int index_rep, nb_dimensions;
    int nb_indices, type_indice, num_indice;
    const char* nom_variable;
    const char* type_str;
    arbre curseur;
    char note[256];
    
    /* Si le type de la variable est inconnu, erreur déjà signalée ailleurs */
    if (variable.type == -1) {
        return construire_acces_tableau(variable, liste_indices, -1);
    }
    
    type_base = variable.type;
    nom_variable = obtenir_nom_variable(variable.tarbre);
    
    /* Vérifier que le type de base n'est pas un type simple */
    if (type_base >= 0 && type_base <= 3) {
        snprintf(note, sizeof(note), 
            "Accès tableau invalide : '%s' est de type '%s', pas un tableau",
            nom_variable, nom_type(type_base));
        
        ajouter_erreur_complete(ERR_SEMANTIQUE,
            ligne, col, len,
            note, NULL);
        
        return construire_acces_tableau(variable, liste_indices, -1);
    }
    
    /* Vérifier que le type est bien un tableau */
    nature_type = obtenir_nature(type_base);
    if (nature_type != TYPE_ARRAY) {
        type_str = (nature_type == TYPE_STRUCT) ? "structure" : "type complexe";
        snprintf(note, sizeof(note), "Type actuel : %s", type_str);
        
        ajouter_erreur_complete(ERR_SEMANTIQUE,
            ligne, col, len,
            "Accès tableau invalide : pas un tableau",
            note);
        
        return construire_acces_tableau(variable, liste_indices, -1);
    }
    
    /* Obtenir les informations du tableau */
    index_rep = obtenir_type(type_base);
    nb_dimensions = obtenir_nb_dimensions_array(index_rep);
    type_elements = obtenir_type_elements_array(index_rep);
    
        /* Compter les indices fournis et vérifier qu'ils sont entiers */
    nb_indices = 0;
    num_indice = 1;
    curseur = liste_indices;
    
    while (curseur != NULL) {
        if (curseur->nature == A_LISTE_INDICES) {
            nb_indices++;
            
            /* Vérifier que l'indice est entier */
            type_indice = obtenir_type_noeud(curseur->filsGauche);
            if (type_indice != 0 && type_indice != -1) {
                if (nb_dimensions == 1) {
                    snprintf(note, sizeof(note), 
                        "L'indice est de type '%s', attendu 'entier'",
                        nom_type(type_indice));
                } else {
                    snprintf(note, sizeof(note), 
                        "L'indice n°%d est de type '%s', attendu 'entier'",
                        num_indice, nom_type(type_indice));
                }
                
                ajouter_erreur_complete(ERR_SEMANTIQUE,
                    curseur->filsGauche->ligne, 
                    curseur->filsGauche->colonne, 
                    curseur->filsGauche->longueur,
                    "Accès tableau invalide : indice non entier",
                    note);
            }
            
            num_indice++;
            
            /* Passer à l'indice suivant */
            curseur = curseur->filsGauche->frereDroit;
        } else {
            curseur = NULL;
        }
    }
    
    /* Vérifier le nombre de dimensions */
    if (nb_indices != nb_dimensions) {
        snprintf(note, sizeof(note), "Attendu : %d | Fourni : %d", 
                 nb_dimensions, nb_indices);
        
        ajouter_erreur_complete(ERR_SEMANTIQUE,
            ligne, col, len,
            "Accès tableau invalide : nombre de dimensions incorrect",
            note);
        
        type_elements = -1;
    }
    
    /* Construire et retourner le NoeudType */
    return construire_acces_tableau(variable, liste_indices, type_elements);
}