#include <stdio.h>
#include "semantique.h"

int verifier_variable(int num_lex) {
    int index;
    
    index = chercher_param(num_lex);
    if (index != -1) {
        return index;
    }
    
    index = chercher_var(num_lex);
    if (index != -1) {
        return index;
    }
    
    ajouter_erreur(ERR_SEMANTIQUE, ligne_courante,
                   "Variable '%s' non déclarée",
                   obtenir_lexeme(num_lex));
    return -1;
}

int resoudre_type(int num_lex) {
    int index;
    
    index = chercher_struct(num_lex);
    if (index != -1) {
        return index;
    }
    
    index = chercher_array(num_lex);
    if (index != -1) {
        return index;
    }
    
    ajouter_erreur(ERR_SEMANTIQUE, ligne_courante,
                   "Type '%s' non déclaré",
                   obtenir_lexeme(num_lex));
    return -1;
}

/* un cas d'exception ... */
arbre creer_noeud_appel(int num_lex, arbre arguments) {
    int index_fct, index_proc, nature, num_decl;
    arbre n;
    
    index_fct = chercher_fonction(num_lex);
    index_proc = chercher_procedure(num_lex);
    
    if (index_fct == -1 && index_proc == -1) {
        ajouter_erreur(ERR_SEMANTIQUE, ligne_courante,
                       "Fonction ou procédure '%s' non déclarée",
                       obtenir_lexeme(num_lex));
        nature = A_APPEL_PROC;
        num_decl = -1;
    } else if (index_fct != -1) {
        nature = A_APPEL_FCT;
        num_decl = index_fct;
    } else {
        nature = A_APPEL_PROC;
        num_decl = index_proc;
    }
    
    n = creer_noeud(nature, num_lex);
    definir_declaration(n, num_decl);
    
    return concat_pere_fils(n, arguments);
}

/* Compte les utilisations de chaque variable dans l'AST */
static void compter_utilisations(arbre a, int compteur[MAX_LEXEMES]) {
    if (a == NULL) {
        return;
    }
    
    /* Si c'est un IDF, incrémenter le compteur */
    if (a->nature == A_IDF && a->num_declaration >= 0 && a->num_declaration < MAX_LEXEMES) {
        compteur[a->num_declaration]++;
    }
    
    /* Parcourir fils et frères */
    compter_utilisations(a->filsGauche, compteur);
    compter_utilisations(a->frereDroit, compteur);
}

void verifier_variables_non_utilisees() {
    int compteur[MAX_LEXEMES];
    int i, num_region;
    arbre instructions;
    Nature nature;
    const char* nom;
    
    /* Initialiser le compteur à 0 */
    i = 0;
    while (i < MAX_LEXEMES) {
        compteur[i] = 0;
        i++;
    }
    
    /* Parcourir toutes les régions et compter les utilisations */
    num_region = 0;
    while (num_region < MAX_REGIONS) {
        if (tab_regions[num_region].numero != -1) {
            instructions = obtenir_arbre_region(num_region);
            compter_utilisations(instructions, compteur);
        }
        num_region++;
    }
    
    /* Vérifier les variables déclarées mais non utilisées */
    i = 0;
    while (i < MAX_LEXEMES) {
        nature = tab_declarations[i].nature;
        
        /* Uniquement variables et paramètres */
        if (nature == NATURE_VAR || nature == NATURE_PARAM) {
            /* Si compteur = 0, la variable n'est jamais utilisée */
            if (compteur[i] == 0) {
                nom = obtenir_lexeme(i);
                num_region = tab_declarations[i].region;
                
                /* Utiliser la région comme approximation */
                if (nature == NATURE_VAR) {
                    ajouter_erreur(ERR_WARNING, 0,  /* Ligne approximative
                        ici on a pas vraiment accès à la ligne -> on fait avec la region
                        pour eviter de modifier la table de declarations ..... */
                        "Variable '%s' (région %d) déclarée mais non utilisée", nom, num_region);
                } else {
                    ajouter_erreur(ERR_WARNING, 0,
                        "Paramètre '%s' (région %d) déclaré mais non utilisé", nom, num_region);
                }
            }
        }
        i++;
    }
}