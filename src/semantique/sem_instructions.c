#include <stdio.h>
#include "semantique.h"
#include "construction.h"
#include "utils_communs.h"

arbre verifier_affectation(NoeudType destination, NoeudType source) {
    int type_src;
    const char* nom_dest;
    char note[256];
    arbre resultat;
    
    /* Calculer le type de l'expression source */
    type_src = source.type;
    nom_dest = obtenir_nom_variable(destination.tarbre);
    
    /* Si type inconnu, erreur déjà signalée ailleurs */
    if (destination.type == -1 || type_src == -1) {
        /* Construire quand même l'arbre pour continuer */
        resultat = concat_pere_fils(creer_noeud(A_OPAFF, -1),
                                   concat_pere_frere(destination.tarbre, source.tarbre));
        return resultat;
    }
    
    /* Vérifier que la destination est de type simple
     * On ne peut pas affecter à une struct ou un tableau entier */
    if (!est_type_simple(destination.type) && !est_type_chaine(destination.type)) {
        ajouter_erreur_complete(ERR_SEMANTIQUE,
            destination.ligne, destination.colonne, destination.longueur,
            "Affectation interdite : type complexe (struct ou tableau non-chaîne)",
            NULL);
        
        /* Construire quand même pour continuer */
        resultat = concat_pere_fils(creer_noeud(A_OPAFF, -1),
                                   concat_pere_frere(destination.tarbre, source.tarbre));
        return resultat;
    }
    
    /* Vérifier que la source n'est pas un appel de procédure
     * Les procédures ne retournent pas de valeur */
    if (type_src == -2) {
        snprintf(note, sizeof(note),
            "Variable '%s' attend une valeur", nom_dest);
        
        ajouter_erreur_complete(ERR_SEMANTIQUE,
            source.ligne, source.colonne, source.longueur,
            "Affectation invalide : appel de procédure (pas de valeur de retour)",
            note);
        
        /* Construire quand même pour continuer */
        resultat = concat_pere_fils(creer_noeud(A_OPAFF, -1),
                                   concat_pere_frere(destination.tarbre, source.tarbre));
        return resultat;
    }
    
    /* Vérifier que les types sont compatibles
    * Exemple : on ne peut pas faire   a:int := 6.8:real */
    if (destination.type != type_src) {
        /* Exception : autoriser affectation entre chaînes de tailles différentes */
        if (est_type_chaine(destination.type) && est_type_chaine(type_src)) {
            /* Vérifier si les chaînes ont la même taille réelle */
            if (chaines_meme_taille(destination.type, type_src)) {
                /* Même taille : OK, pas d'erreur ni warning */
            } else {
                /* Tailles différentes : WARNING */
                snprintf(note, sizeof(note),
                    "Affectation entre chaînes de tailles différentes\n"
                    "      Risque de débordement ou perte de données");
                
                ajouter_erreur_complete(ERR_WARNING,
                    source.ligne, source.colonne, source.longueur,
                    "Affectation : tailles de chaînes différentes",
                    note);
            }
        } else {
            /* Erreur normale pour les autres types incompatibles */
            snprintf(note, sizeof(note),
                "Variable '%s' de type '%s'\n      Expression de type '%s'",
                nom_dest, nom_type(destination.type), nom_type(type_src));
            
            /* Pointer sur l'expression source (ce qu'on essaie d'affecter) */
            ajouter_erreur_complete(ERR_SEMANTIQUE,
                source.ligne, source.colonne, source.longueur,
                "Affectation invalide : types incompatibles",
                note);
        }
    }
    
    /* Construire l'arbre A_OPAFF
     * Même s'il y a eu des erreurs, on construit pour continuer ... */
    resultat = concat_pere_fils(creer_noeud(A_OPAFF, -1),
                               concat_pere_frere(destination.tarbre, source.tarbre));

    return resultat;
}

arbre construire_condition(NoeudType condition, arbre bloc_alors, arbre bloc_sinon) {
    arbre resultat;
    int valeur;

    /* Vérifier si la condition est une constante booléenne */
    if (condition.tarbre != NULL && condition.tarbre->nature == A_CSTE_BOOL) {
        valeur = condition.tarbre->valeur;

        if (valeur == 1) {
            /* Condition toujours vraie */
            ajouter_erreur_complete(ERR_WARNING,
                condition.ligne, condition.colonne, condition.longueur,
                "Condition toujours vraie : le bloc 'else ' ne sera jamais exécuté",
                NULL);
        } else {
            /* Condition toujours fausse */
            ajouter_erreur_complete(ERR_WARNING,
                condition.ligne, condition.colonne, condition.longueur,
                "Condition toujours fausse : le bloc 'then' ne sera jamais exécuté",
                NULL);
        }
    }
    
    /* Construire l'arbre normalement */
    resultat = concat_pere_fils(creer_noeud(A_IF_THEN_ELSE, -1),
                               concat_pere_frere(condition.tarbre, 
                                   concat_pere_frere(bloc_alors, bloc_sinon)));
    
    return resultat;
}

arbre construire_boucle(NoeudType condition, arbre corps) {
    arbre resultat;
    int valeur;

    /* Vérifier si la condition est une constante booléenne */
    if (condition.tarbre != NULL && condition.tarbre->nature == A_CSTE_BOOL) {
        valeur = condition.tarbre->valeur;
        
        if (valeur == 1) {
            /* Condition toujours vraie */
            ajouter_erreur_complete(ERR_WARNING,
                condition.ligne, condition.colonne, condition.longueur,
                "Boucle infinie : condition toujours vraie",
                NULL);
        } else {
            /* Condition toujours fausse */
            ajouter_erreur_complete(ERR_WARNING,
                condition.ligne, condition.colonne, condition.longueur,
                "Boucle jamais exécutée : condition toujours fausse",
                NULL);
        }
    }
    
    /* Construire l'arbre normalement */
    resultat = concat_pere_fils(creer_noeud(A_WHILE, -1),
                               concat_pere_frere(condition.tarbre, corps));
    
    return resultat;
}

arbre verifier_et_construire_lecture(arbre liste_variables) {
    arbre curseur, variable;
    int type_var;
    const char* nom_var;
    char note[256];
    
    /* Parcourir la liste des variables */
    curseur = liste_variables;
    
    while (curseur != NULL && curseur->nature == A_LISTE_VARIABLES) {
        variable = curseur->filsGauche;
        
        if (variable != NULL) {
            /* Récupérer le type de la variable */
            type_var = obtenir_type_noeud(variable);
            nom_var = obtenir_nom_variable(variable);
            
            /* Vérifier que c'est un type simple (pas struct/array)
             * Si type == -1, erreur déjà signalée ailleurs */
            if (type_var != -1 && !est_type_simple(type_var)) {
                snprintf(note, sizeof(note),
                    "Variable '%s' de type '%s'\nread accepte uniquement les types simples (int, real, bool, char)",
                    nom_var, nom_type(type_var));
                
                ajouter_erreur_complete(ERR_SEMANTIQUE,
                    variable->ligne, variable->colonne, variable->longueur,
                    "Instruction read invalide : variable de type complexe",
                    note);
            }
            
            /* Passer à la variable suivante */
            curseur = variable->frereDroit;
        } else {
            curseur = NULL;
        }
    }
    
    /* Construire et retourner l'arbre A_LIRE */
    return concat_pere_fils(creer_noeud(A_LIRE, -1), liste_variables);
}

/* Compte le nombre de variables dans la liste */
static int compter_variables(arbre liste) {
    int count;
    arbre curseur;
    
    count = 0;
    curseur = liste;
    
    while (curseur != NULL && curseur->nature == A_LISTE_VARIABLES) {
        count++;
        curseur = curseur->filsGauche->frereDroit;
    }
    
    return count;
}

arbre verifier_et_construire_ecriture(arbre format_node, arbre liste_variables) {
    const char* format;
    const char* p;
    arbre curseur, variable;
    int nb_specs, nb_vars;
    int spec_index;
    int type_var;
    const char* nom_var;
    char c;
    char note[512];
    
    /* Récupérer la chaîne de format */
    format = obtenir_lexeme(format_node->valeur);
    
    /* Compter spécificateurs et variables */
    nb_specs = compter_specs(format);
    nb_vars = compter_variables(liste_variables);
    
    /* Vérification 1 : Nombre de spécificateurs = nombre de variables */
    if (nb_specs != nb_vars) {
        /* Cas A : Variable(s) fournie(s) sans aucun spécificateur */
        if (nb_specs == 0 && nb_vars > 0) {
            snprintf(note, sizeof(note), 
                "Aucun spécificateur de format trouvé dans la chaîne\n"
                "      Vous avez fourni %d variable(s)\n"
                "      Solution : Ajoutez des spécificateurs (%%d, %%f, %%c) dans la chaîne\n",
                nb_vars);
            
            ajouter_erreur_complete(ERR_SEMANTIQUE,
                format_node->ligne, format_node->colonne, format_node->longueur,
                "Instruction write invalide : variable(s) sans spécificateur",
                note);
        }
        /* Cas B : Trop de variables */
        else if (nb_specs < nb_vars) {
            snprintf(note, sizeof(note), 
                "La chaîne contient %d spécificateur(s) mais vous avez fourni %d variable(s)\n"
                "      Solution : Ajoutez %d spécificateur(s) ou supprimez %d variable(s)",
                nb_specs, nb_vars, nb_vars - nb_specs, nb_vars - nb_specs);
            
            ajouter_erreur_complete(ERR_SEMANTIQUE,
                format_node->ligne, format_node->colonne, format_node->longueur,
                "Instruction write invalide : trop de variables",
                note);
        }
        /* Cas C : Pas assez de variables */
        else {
            snprintf(note, sizeof(note), 
                "La chaîne contient %d spécificateur(s) mais vous avez fourni %d variable(s)\n"
                "      Solution : Ajoutez %d variable(s) ou supprimez %d spécificateur(s)",
                nb_specs, nb_vars, nb_specs - nb_vars, nb_specs - nb_vars);
            
            ajouter_erreur_complete(ERR_SEMANTIQUE,
                format_node->ligne, format_node->colonne, format_node->longueur,
                "Instruction write invalide : pas assez de variables",
                note);
        }
    }
    
    /* Vérification 2 : Correspondance des types
     * Parcourir format et liste en parallèle */
    p = format;
    curseur = liste_variables;
    spec_index = 0;
    
    while (*p != '\0' && spec_index < nb_specs) {
        if (*p == '%') {
            p++;
            if (*p != '\0') {
                c = *p;
                
                /* Spécificateur valide */
                if (c == 'd' || c == 'f' || c == 'c') {
                    /* Obtenir la variable courante */
                    if (curseur != NULL && curseur->nature == A_LISTE_VARIABLES) {
                        variable = curseur->filsGauche;
                        type_var = obtenir_type_noeud(variable);
                        nom_var = obtenir_nom_variable(variable);
                        
                        /* Vérifier correspondance selon le spécificateur
                         * Si type_var == -1, erreur déjà signalée */
                        if (type_var != -1) {
                            if (c == 'd') {
                                /* %d accepte int (0) OU bool (2) */
                                if (type_var != 0 && type_var != 2) {
                                    snprintf(note, sizeof(note),
                                        "%%d attend 'entier' ou 'booléen'\n      Variable '%s' : '%s'",
                                        nom_var, nom_type(type_var));
                                    
                                    ajouter_erreur_complete(ERR_SEMANTIQUE,
                                        variable->ligne, variable->colonne, variable->longueur,
                                        "Instruction write invalide : type incompatible",
                                        note);
                                }
                            } else if (c == 'f') {
                                /* %f accepte seulement real (1) */
                                if (type_var != 1) {
                                    snprintf(note, sizeof(note),
                                        "%%f attend 'réel'\n      Variable '%s' : '%s'",
                                        nom_var, nom_type(type_var));
                                    
                                    ajouter_erreur_complete(ERR_SEMANTIQUE,
                                        variable->ligne, variable->colonne, variable->longueur,
                                        "Instruction write invalide : type incompatible",
                                        note);
                                }
                            } else if (c == 'c') {
                                /* %c accepte seulement char (3) */
                                if (type_var != 3) {
                                    snprintf(note, sizeof(note),
                                        "%%c attend 'caractère'\n      Variable '%s' : '%s'",
                                        nom_var, nom_type(type_var));
                                    
                                    ajouter_erreur_complete(ERR_SEMANTIQUE,
                                        variable->ligne, variable->colonne, variable->longueur,
                                        "Instruction write invalide : type incompatible",
                                        note);
                                }
                            }
                        }
                        
                        /* Passer à la variable suivante */
                        curseur = variable->frereDroit;
                        spec_index++;
                    }
                }
                
                /* Spécificateur non supporté */
                if (c == 's') {
                    ajouter_erreur_complete(ERR_SEMANTIQUE,
                        format_node->ligne, format_node->colonne, format_node->longueur,
                        "Instruction write invalide : spécificateur non supporté",
                        "%s n'est pas supporté dans ce langage");
                }
                
                p++;
            }
        } else {
            p++;
        }
    }
    
    /* Vérification 3 : Toutes les variables doivent être de type simple */
    curseur = liste_variables;
    
    while (curseur != NULL && curseur->nature == A_LISTE_VARIABLES) {
        variable = curseur->filsGauche;
        
        if (variable != NULL) {
            type_var = obtenir_type_noeud(variable);
            nom_var = obtenir_nom_variable(variable);
            
            /* Si type complexe, erreur */
            if (type_var != -1 && !est_type_simple(type_var)) {
                snprintf(note, sizeof(note),
                    "Variable '%s' de type '%s'\nwrite accepte uniquement les types simples (int, real, bool, char)",
                    nom_var, nom_type(type_var));
                
                ajouter_erreur_complete(ERR_SEMANTIQUE,
                    variable->ligne, variable->colonne, variable->longueur,
                    "Instruction write invalide : variable de type complexe",
                    note);
            }
            
            curseur = variable->frereDroit;
        } else {
            curseur = NULL;
        }
    }
    
    /* Construire et retourner l'arbre A_ECRIRE */
    return concat_pere_fils(creer_noeud(A_ECRIRE, -1),
                           concat_pere_frere(format_node, liste_variables));
}