#include <stdio.h>
#include "semantique.h"
#include "construction.h"
#include "utils_communs.h"

arbre verifier_et_construire_appel(int num_lex, arbre liste_arguments) {
    arbre arbre_appel, curseur, argument, premier_arg;
    int num_decl, index_rep;
    int nb_params, nb_args;
    int type_arg, type_param;
    int num_lex_param;
    int i;
    const char* nom_fonction;
    char note[256];
    
    /* Construction de l'arbre de l'appel
     * Cette fonction vérifie aussi que l'IDF est bien une fonction/procédure */
    arbre_appel = creer_noeud_appel(num_lex, liste_arguments);
    
    /* Si la fonction/procédure n'existe pas, erreur déjà signalée
     * On retourne quand même l'arbre pour continuer le parsing */
    num_decl = arbre_appel->num_declaration;
    if (num_decl < 0) {
        return arbre_appel;
    }
    
    /* Récupérer les informations de la fonction depuis la table de représentation */
    nom_fonction = obtenir_lexeme(arbre_appel->valeur);
    index_rep = obtenir_type(num_decl);
    nb_params = obtenir_nb_params(index_rep);
    
    /* Récupérer le premier argument pour un meilleur pointage d'erreur */
    premier_arg = NULL;
    if (arbre_appel->filsGauche != NULL) {
        premier_arg = arbre_appel->filsGauche->filsGauche;
    }
    
    /* Compter le nombre d'arguments fournis dans l'appel
     * On parcourt la liste chaînée des arguments */
    curseur = arbre_appel->filsGauche;
    nb_args = 0;
    
    while (curseur != NULL && curseur->nature == A_LISTE_ARGUMENTS) {
        nb_args++;
        argument = curseur->filsGauche;
        
        /* Passer à l'argument suivant en suivant les frères */
        if (argument != NULL) {
            curseur = argument->frereDroit;
        } else {
            curseur = NULL;
        }
    }
    
    /* Vérification 1 : Le nombre d'arguments doit correspondre
     * Exemple : calcul(5, 10, 15) avec calcul(a:int, b:int) → Erreur */
    if (nb_args != nb_params) {
        snprintf(note, sizeof(note), "Attendu : %d | Fourni : %d", nb_params, nb_args);
        
        /* Pointer sur le premier argument si présent pour un meilleur indicateur */
        if (premier_arg != NULL) {
            ajouter_erreur_complete(ERR_SEMANTIQUE,
                premier_arg->ligne, premier_arg->colonne, premier_arg->longueur,
                "Appel invalide : nombre d'arguments incorrect", note);
        } else {
            /* Pas d'arguments fournis, pointer sur l'appel */
            ajouter_erreur_complete(ERR_SEMANTIQUE,
                arbre_appel->ligne, arbre_appel->colonne, arbre_appel->longueur,
                "Appel invalide : nombre d'arguments incorrect", note);
        }
        
        /* On continue quand même pour vérifier les types des arguments présents */
    }
    
    /* Vérification 2 : Le type de chaque argument doit correspondre
     * On parcourt la liste des arguments ET la signature en parallèle */
    curseur = arbre_appel->filsGauche;
    i = 0;
    
    while (curseur != NULL && curseur->nature == A_LISTE_ARGUMENTS && i < nb_params) {
        argument = curseur->filsGauche;
        
        if (argument != NULL) {
            /* Calculer le type de l'argument fourni
             * obtenir_type_noeud fonctionne sur n'importe quelle expression :
             * - Constante : type connu directement
             * - Variable : type depuis la déclaration
             * - Expression : type propagé (a + b)
             * - Appel imbriqué : type de retour de la fonction */
            type_arg = obtenir_type_noeud(argument);
            
            /* Récupérer le type attendu depuis la signature */
            obtenir_info_param(index_rep, i, &num_lex_param, &type_param);
            
            /* Si le type de l'argument est inconnu (-1), c'est qu'il y a
             * déjà une erreur signalée ailleurs, on ignore */
            if (type_arg != -1 && type_arg != type_param) {
                snprintf(note, sizeof(note),
                    "Paramètre '%s' attend '%s'\n      Argument fourni : '%s'",
                    obtenir_lexeme(num_lex_param), nom_type(type_param), nom_type(type_arg));
                
                /* Pointer sur l'argument fautif (a déjà les bonnes positions) */
                ajouter_erreur_complete(ERR_SEMANTIQUE,
                    argument->ligne, argument->colonne, argument->longueur,
                    "Appel invalide : type de l'argument incorrect", note);
            }
            
            /* Passer au paramètre et argument suivants */
            i++;
            curseur = argument->frereDroit;
        } else {
            curseur = NULL;
        }
    }
    
    /* Retourner l'arbre construit
     * Même s'il y a des erreurs, on retourne l'arbre pour continuer le parsing */
    return arbre_appel;
}

/* Return - Helpers */

/* Trouve la declaration par sa région d'exécution
    Direction : region -> declaration */
static int trouver_declaration_region(int region) {
    int i;
    Nature nature;
    
    /* Chercher une fonction ou procédure ayant cette région comme région d'exécution */
    for (i = 0; i < MAX_DECLARATIONS; i++) {
        nature = tab_declarations[i].nature;
        
        if (tab_declarations[i].execution == region && 
            (nature == NATURE_FCT || nature == NATURE_PROC)) {
            return i;
        }
    }
    
    return -1;
}

static int contient_return(arbre a) {
    if (a == NULL) {
        return 0;
    }
    if (a->nature == A_RETURN) {
        return 1;
    }
    return contient_return(a->filsGauche) || contient_return(a->frereDroit);
}


arbre verifier_et_construire_return(NoeudType expression_retour) {
    int region_courante, num_decl, index_rep;
    int type_retour, type_expr;
    Nature nature;
    arbre arbre_return;
    char note[256];
    
    /* Récupérer la région courante (fonction/procédure englobante) */
    region_courante = obtenir_region_courante();
    
    /* Trouver la déclaration correspondant à cette région */
    num_decl = trouver_declaration_region(region_courante);
    
    
    /* Si on est dans la région principale (0) */
    if (num_decl == -1 || region_courante == 0) {     
        ajouter_erreur(ERR_SEMANTIQUE, ligne_courante,
            "Return invalide : en dehors d'une fonction");
        
        return concat_pere_fils(creer_noeud(A_RETURN, -1), expression_retour.tarbre);
    }
    
    /* Récupérer la nature de la fonction/procédure */
    nature = obtenir_nature(num_decl);
    
    /* Vérification 1 : Return avec valeur dans procédure */
    if (nature == NATURE_PROC && expression_retour.tarbre != NULL) {
        ajouter_erreur(ERR_SEMANTIQUE, ligne_courante,
            "Return invalide : procédure ne peut pas retourner de valeur");
        
        /* Construire quand même l'arbre */
        return concat_pere_fils(creer_noeud(A_RETURN, -1), expression_retour.tarbre);
    }
    
    /* Vérification 2 : Return sans valeur dans fonction */
    if (nature == NATURE_FCT && expression_retour.tarbre == NULL) {
        index_rep = obtenir_type(num_decl);
        type_retour = obtenir_type_retour_fonction(index_rep);
        
        snprintf(note, sizeof(note), "Type attendu : '%s'", nom_type(type_retour));
        
        ajouter_erreur_complete(ERR_SEMANTIQUE,
            ligne_courante, 0, 0,
            "Return invalide : fonction doit retourner une valeur",
            note);
        
        /* Construire quand même l'arbre */
        return concat_pere_fils(creer_noeud(A_RETURN, -1), NULL);
    }
    
    /* Vérification 3 : Return type incompatible dans fonction */
    if (nature == NATURE_FCT && expression_retour.tarbre != NULL) {
        index_rep = obtenir_type(num_decl);
        type_retour = obtenir_type_retour_fonction(index_rep);
        type_expr = expression_retour.type;
        
        /* Si le type de l'expression est inconnu (-1), erreur déjà signalée */
        if (type_expr != -1 && type_expr != type_retour) {
            snprintf(note, sizeof(note),
                "Attendu : '%s' | Expression : '%s'",
                nom_type(type_retour), nom_type(type_expr));
            
            ajouter_erreur_complete(ERR_SEMANTIQUE,
                expression_retour.ligne,
                expression_retour.colonne,
                expression_retour.longueur,
                "Return invalide : type incompatible",
                note);
        }
    }
    
    /* Construire et retourner l'arbre A_RETURN */
    arbre_return = concat_pere_fils(creer_noeud(A_RETURN, -1), expression_retour.tarbre);
    
    return arbre_return;
}

void verifier_region_fonction(int num_region, arbre corps) {
    int num_decl, index_rep, type_retour;
    Nature nature;
    char message[256];
    
    if (corps == NULL) {
        return;
    }
    
    /* Vérifier si cette région est une fonction */
    num_decl = trouver_declaration_region(num_region);
    
    if (num_decl == -1) {
        return;
    }
    
    nature = obtenir_nature(num_decl);

    /* Fonction sans return (WARNING) */
    if (nature == NATURE_FCT && !contient_return(corps)) {
        index_rep = obtenir_type(num_decl);
        type_retour = obtenir_type_retour_fonction(index_rep);

        snprintf(message, sizeof(message), 
            "Fonction sans return (type attendu : '%s')", nom_type(type_retour));
        
        ajouter_erreur_complete(ERR_WARNING,
            corps->ligne, corps->colonne, corps->longueur,
            message, NULL);
    }
}