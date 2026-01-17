#include <stdio.h>
#include "utils_communs.h"


int obtenir_type_noeud(arbre a) {
    int num_decl, type_courant, index_rep;
    Nature nature_type;
    int i, nb, num_lex_champ, type_champ, nb_indices, nb_dimensions;
    arbre liste, champ, element, curseur, elem;
    int trouve, continuer;

    
    if (a == NULL) {
        return -1;
    }
    
    switch (a->nature) {
        
        case A_CSTE_ENT:
            return 0;
        
        case A_CSTE_REELLE:
            return 1;
        
        case A_CSTE_BOOL:
            return 2;
        
        case A_CSTE_CHAR:
            return 3;
        

        case A_IDF:
            num_decl = a->num_declaration;
            if (num_decl < 0) {
                return -1;
            }
            
            /* Type de base */
            type_courant = obtenir_type(num_decl);
            
            /* Pas de liste d'accès → retourner type de base */
            if (a->filsGauche == NULL) {
                return type_courant;
            }
            
            /* Parcourir les listes d'accès chaînées horizontalement */
            liste = a->filsGauche;
            
            while (liste != NULL) {
                if (liste->nature == A_LISTE_CHAMPS) {
                    /* Accès champ */
                    champ = liste->filsGauche;
                    
                    /* Vérifier que type_courant est une structure */
                    if (type_courant >= 0 && type_courant <= 3) {
                        return -1;  /* Type simple n'a pas de champs */
                    }
                    
                    nature_type = obtenir_nature(type_courant);
                    if (nature_type != TYPE_STRUCT) {
                        return -1;
                    }
                    
                    /* Chercher le champ dans la structure */
                    index_rep = obtenir_type(type_courant);
                    nb = obtenir_nb_champs_struct(index_rep);
                    
                    trouve = 0;
                    i = 0;
                    while (i < nb && trouve == 0) {
                        obtenir_info_champ(index_rep, i, &num_lex_champ, &type_champ);
                        if (num_lex_champ == champ->valeur) {
                            type_courant = type_champ;
                            trouve = 1;
                        } else {
                            i++;
                        }
                    }
                    
                    /* Champ non trouvé */
                    if (trouve == 0) {
                        return -1;
                    }
                    
                    /* Passer à la liste suivante */
                    liste = champ->frereDroit;

                } else if (liste->nature == A_LISTE_INDICES) {
                    /* Accès tableau - traiter TOUS les indices consécutifs */
                    
                    /* Vérifier que type_courant est un tableau */
                    if (type_courant >= 0 && type_courant <= 3) {
                        return -1;
                    }
                    
                    nature_type = obtenir_nature(type_courant);
                    if (nature_type != TYPE_ARRAY) {
                        return -1;
                    }
                    
                    /* Obtenir infos du tableau */
                    index_rep = obtenir_type(type_courant);
                    nb_dimensions = obtenir_nb_dimensions_array(index_rep);
                    
                    /* Compter les indices consécutifs */
                    nb_indices = 0;
                    curseur = liste;
                    continuer = 1;
                    
                    while (curseur != NULL && continuer == 1) {
                        if (curseur->nature == A_LISTE_INDICES) {
                            nb_indices++;
                            elem = curseur->filsGauche;
                            curseur = elem->frereDroit;
                        } else {
                            continuer = 0;
                        }
                    }
                    
                    /* Vérifier que le nombre d'indices correspond */
                    if (nb_indices != nb_dimensions) {
                        return -1;  /* Erreur : mauvais nombre d'indices */
                    }
                    
                    /* Type après accès = type des éléments */
                    type_courant = obtenir_type_elements_array(index_rep);
                    
                    /* Passer à la liste suivante (peut être A_LISTE_CHAMPS) */
                    liste = curseur;
                }
            }
            
            return type_courant;
        
        case A_PLUS:
        case A_MOINS:
        case A_MULT:
        case A_DIV:
            return obtenir_type_noeud(a->filsGauche);
        
        case A_MOINS_UNAIRE:
            return obtenir_type_noeud(a->filsGauche);
        
        case A_ET:
        case A_OU:
        case A_NON:
            return 2;
        
        case A_EGAL:
        case A_DIFF:
        case A_INF:
        case A_SUP:
        case A_INF_EGAL:
        case A_SUP_EGAL:
            return 2;
        
        case A_APPEL_FCT:
            num_decl = a->num_declaration;
            if (num_decl < 0) {
                return -1;
            }
            index_rep = obtenir_type(num_decl);
            return obtenir_type_retour_fonction(index_rep);
        
        case A_APPEL_PROC:
            return -2;
        
        
        default:
            return -1;
    }
}

/* Compte les spécificateurs valides (%d, %f, %c) dans un format */
int compter_specs(const char* format) {
    int count;
    const char* p;
    char c;
    
    count = 0;
    p = format;
    
    while (*p != '\0') {
        if (*p == '%') {
            p++;
            if (*p == '\0') {
                return count;
            }
            
            c = *p;
            if (c == 'd' || c == 'f' || c == 'c' || c == 's') {
                count++;
            }
            p++;
        } else {
            p++;
        }
    }
    
    return count;
}

const char* obtenir_nom_variable(arbre a) {
    arbre liste, champ, element;
    const char* dernier_nom;
    
    if (a == NULL) {
        return "???";
    }
    
    if (a->nature != A_IDF) {
        return "???";
    }
    
    /* Nom initial = nom de la variable */
    dernier_nom = obtenir_lexeme(a->valeur);
    
    /* Parcourir les listes d'accès pour trouver le dernier champ */
    liste = a->filsGauche;
    
    while (liste != NULL) {
        if (liste->nature == A_LISTE_CHAMPS) {
            champ = liste->filsGauche;
            dernier_nom = obtenir_lexeme(champ->valeur);
            liste = champ->frereDroit;
        } else if (liste->nature == A_LISTE_INDICES) {
            element = liste->filsGauche;
            liste = element->frereDroit;
        } else {
            liste = NULL;
        }
    }
    
    return dernier_nom;
}

/* Vérifie si un type est une chaîne de caractères
 * Une chaîne = array[...] of char à UNE dimension
 * 
 */
int est_type_chaine(int type) {
    Nature nature;
    int index_rep, type_elements, nb_dimensions;

    /* Vérifier que ce n'est pas un type simple */
    if (type >= 0 && type <= 3) {
        return 0;  /* Types simples (int, real, bool, char) ne sont pas des chaînes */
    }
    
    /* Vérifier que c'est un tableau */
    nature = obtenir_nature(type);
    if (nature != TYPE_ARRAY) {
        return 0;  /* Structure ou autre chose, pas une chaîne */
    }
    
    /* Vérifier une seule dimension */
    index_rep = obtenir_type(type);
    nb_dimensions = obtenir_nb_dimensions_array(index_rep);
    
    if (nb_dimensions != 1) {
        return 0;  /* Tableau multidimensionnel, pas une chaîne */
    }

    /* Vérifier que les éléments sont des char */
    type_elements = obtenir_type_elements_array(index_rep);
    
    if (type_elements != 3) {  /* char = 3 */
        return 0;  /* Tableau d'autre chose que des char */
    }
    
    /* Toutes les conditions sont remplies : c'est une chaîne ! */
    return 1;
}