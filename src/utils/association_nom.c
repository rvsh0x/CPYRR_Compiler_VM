#include <stdio.h>
#include "association_nom.h"
#include "tab_declarations.h"
#include "tab_lexico.h"
#include "pile_regions.h"

int association_nom(int num_lex, Nature nature_recherchee) {
    int* pile;
    int taille_pile;
    int i, index;
    int region_courante;
    
    if (num_lex < 0 || num_lex >= MAX_LEXEMES) {
        return -1;
    }
    
    pile = obtenir_pile(&taille_pile);
    
    if (taille_pile == 0) {
        return -1;
    }
    
    /* Parcourir regions du sommet vers le fond */
    i = taille_pile - 1;
    while (i >= 0) {
        region_courante = pile[i];
        
        /* Chercher dans la region courante */
        index = chercher_dans_region(num_lex, region_courante, nature_recherchee);
        
        if (index != -1) {
            return index;
        }
        
        i--;
    }
    
    return -1;
}

int chercher_struct(int num_lex) {
    return association_nom(num_lex, TYPE_STRUCT);
}

int chercher_array(int num_lex) {
    return association_nom(num_lex, TYPE_ARRAY);
}

int chercher_var(int num_lex) {
    return association_nom(num_lex, NATURE_VAR);
}

int chercher_param(int num_lex) {
    return association_nom(num_lex, NATURE_PARAM);
}

int chercher_fonction(int num_lex) {
    return association_nom(num_lex, NATURE_FCT);
}

int chercher_procedure(int num_lex) {
    return association_nom(num_lex, NATURE_PROC);
}