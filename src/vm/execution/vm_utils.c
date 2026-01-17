#include "vm/vm_utils.h"
#include "couleurs.h"
#include "erreurs.h"
#include "tab_declarations.h"
#include "tab_representations.h"
#include "utils_communs.h"
#include "vm/vm_expressions.h"
#include <stdio.h>

/* Affiche une valeur selon son type */
void afficher_valeur(Valeur val, int type) {
    if (type == 0) {
        printf(CYAN "%d" RESET, val.entier);
        return;
    }
    
    if (type == 1) {
        printf(CYAN "%f" RESET, val.reel);
        return;
    }
    
    if (type == 2) {
        printf(CYAN "%s" RESET, val.booleen ? "true" : "false");
        return;
    }
    
    if (type == 3) {
        printf(CYAN "%c" RESET, val.caractere);
        return;
    }
    
    /* Ne devrait jamais arriver si sémantique OK */
    printf(ROUGE "???" RESET);
}

/* Applique une opération arithmétique binaire */
Valeur appliquer_operation_arithmetique(int operateur, Valeur val1, Valeur val2, int type) {
    Valeur resultat;
    
    if (type == 0) {
        /* Opération sur entiers */
        switch (operateur) {
            case A_PLUS:
                resultat.entier = val1.entier + val2.entier;
                break;
            
            case A_MOINS:
                resultat.entier = val1.entier - val2.entier;
                break;
            
            case A_MULT:
                resultat.entier = val1.entier * val2.entier;
                break;
            
            case A_DIV:
                if (val2.entier == 0) {
                    erreur_runtime("Division par zéro");
                }
                resultat.entier = val1.entier / val2.entier;
                break;
            
            default:
                erreur_runtime("Opérateur arithmétique inconnu");
        }
        
    } else if (type == 1) {
        /* Opération sur réels */
        switch (operateur) {
            case A_PLUS:
                resultat.reel = val1.reel + val2.reel;
                break;
            
            case A_MOINS:
                resultat.reel = val1.reel - val2.reel;
                break;
            
            case A_MULT:
                resultat.reel = val1.reel * val2.reel;
                break;
            
            case A_DIV:
                if (val2.reel == 0.0) {
                    erreur_runtime("Division par zéro");
                }
                resultat.reel = val1.reel / val2.reel;
                break;
            
            default:
                erreur_runtime("Opérateur arithmétique inconnu");
        }
        
    } else {
        /* ne doit pas arriver  ... mais on gère quand même proprement */
        erreur_runtime("Type non numérique dans opération arithmétique");
    }
    
    return resultat;
}

/* Applique une négation arithmétique unaire (- unaire) */
Valeur appliquer_negation_unaire(Valeur val, int type) {
    Valeur resultat;
    
    if (type == 0) {
        /* Négation d'entier */
        resultat.entier = -val.entier;
    } else if (type == 1) {
        /* Négation de réel */
        resultat.reel = -val.reel;
    } else {
        erreur_runtime("Type non numérique dans négation unaire");
    }
    
    return resultat;
}

/* Applique une opération de comparaison */
Valeur appliquer_comparaison(int operateur, Valeur val1, Valeur val2, int type) {
    Valeur resultat;
    int res_bool = 0;
    
    if (type == 0) {
        /* Comparaison d'entiers */
        switch (operateur) {
            case A_EGAL:      res_bool = (val1.entier == val2.entier); break;
            case A_DIFF:      res_bool = (val1.entier != val2.entier); break;
            case A_INF:       res_bool = (val1.entier < val2.entier);  break;
            case A_SUP:       res_bool = (val1.entier > val2.entier);  break;
            case A_INF_EGAL:  res_bool = (val1.entier <= val2.entier); break;
            case A_SUP_EGAL:  res_bool = (val1.entier >= val2.entier); break;
            default:
                erreur_runtime("Opérateur de comparaison inconnu");
        }
        
    } else if (type == 1) {
        /* Comparaison de réels */
        switch (operateur) {
            case A_EGAL:      res_bool = (val1.reel == val2.reel); break;
            case A_DIFF:      res_bool = (val1.reel != val2.reel); break;
            case A_INF:       res_bool = (val1.reel < val2.reel);  break;
            case A_SUP:       res_bool = (val1.reel > val2.reel);  break;
            case A_INF_EGAL:  res_bool = (val1.reel <= val2.reel); break;
            case A_SUP_EGAL:  res_bool = (val1.reel >= val2.reel); break;
            default:
                erreur_runtime("Opérateur de comparaison inconnu");
        }
        
    } else if (type == 2) {
        /* Comparaison de booléens (seulement = et <>) */
        switch (operateur) {
            case A_EGAL: res_bool = (val1.booleen == val2.booleen); break;
            case A_DIFF: res_bool = (val1.booleen != val2.booleen); break;
            default:
                erreur_runtime("Opérateur de comparaison non supporté pour bool");
        }
        
    } else if (type == 3) {
        /* Comparaison de caractères */
        switch (operateur) {
            case A_EGAL:      res_bool = (val1.caractere == val2.caractere); break;
            case A_DIFF:      res_bool = (val1.caractere != val2.caractere); break;
            case A_INF:       res_bool = (val1.caractere < val2.caractere);  break;
            case A_SUP:       res_bool = (val1.caractere > val2.caractere);  break;
            case A_INF_EGAL:  res_bool = (val1.caractere <= val2.caractere); break;
            case A_SUP_EGAL:  res_bool = (val1.caractere >= val2.caractere); break;
            default:
                /* ne doit pas arriver  ... mais on gère quand même proprement */
                erreur_runtime("Opérateur de comparaison inconnu");
        }
    } else {
        /* ne doit pas arriver  ... mais on gère quand même proprement ... sémantiquement c'est fait */
        erreur_runtime("Type non comparable");
    }
    
    /* Résultat toujours booléen */
    resultat.booleen = (char)res_bool;
    return resultat;
}

/* Applique une opération booléenne (AND, OR) */
Valeur appliquer_operation_booleenne(int operateur, Valeur val1, Valeur val2) {
    Valeur resultat;
    
    if (operateur == A_ET) {
        /* AND logique */
        resultat.booleen = val1.booleen && val2.booleen;
    } else if (operateur == A_OU) {
        /* OR logique */
        resultat.booleen = val1.booleen || val2.booleen;
    } else {
        /* ne doit pas arriver  ... mais on gère quand même proprement */
        erreur_runtime("Opérateur booléen inconnu");
    }
    
    return resultat;
}

/* Applique une négation booléenne (NOT) */
Valeur appliquer_negation_booleenne(Valeur val) {
    Valeur resultat;
    
    /* NOT logique */
    resultat.booleen = !val.booleen;
    
    return resultat;
}

/* Trouve le num_decl du i-ème paramètre d'une région
 * Retourne -1 si non trouvé
 */
int trouver_parametre(int num_region, int index_param) {
    int j, compteur;
    
    compteur = 0;
    j = 0;
    
    while (j < MAX_DECLARATIONS) {
        if (tab_declarations[j].nature == NATURE_PARAM &&
            tab_declarations[j].region == num_region) {
            if (compteur == index_param) {
                return j;
            }
            compteur++;
        }
        j++;
    }
    
    return -1;
}

/* Obtient la taille d'une chaîne (nombre de caractères)
 * type : l'index de type de la chaîne
 * Retourne : taille (borne_sup - borne_inf + 1)
 */
int obtenir_taille_chaine(int type) {
    int index_rep;
    int borne_inf, borne_sup;
    
    /* Récupérer la représentation */
    index_rep = obtenir_type(type);
    
    /* Une chaîne a forcément 1 dimension */
    obtenir_bornes_dimension(index_rep, 0, &borne_inf, &borne_sup);

    /* Calculer la taille */
    return (borne_sup - borne_inf + 1);
}


/* Copie une chaîne caractère par caractère
 * taille : nombre de caractères à copier
 */
void copier_chaine(int adresse_dest, int adresse_source, int taille) {
    int i;
    
    printf("  Copie chaîne : pile[%d..%d] := pile[%d..%d]\n",
           adresse_dest, adresse_dest + taille - 1,
           adresse_source, adresse_source + taille - 1);
    
    /* Copier caractère par caractère */
    i = 0;
    while (i < taille) {
        pile[adresse_dest + i].valeur.caractere = pile[adresse_source + i].valeur.caractere;
        pile[adresse_dest + i].est_initialisee = pile[adresse_source + i].est_initialisee;
        i++;
    }
}


/* Compare deux chaînes de caractères de manière lexicographique
 * Principe : Compare caractère par caractère de gauche à droite
 *            Comme dans un dictionnaire : "apple" < "banana"
 */
Valeur comparer_chaines(arbre noeud_gauche, arbre noeud_droite, int operateur) {
    Valeur resultat;
    int adresse_s1, adresse_s2;
    int type_s1, type_s2;
    int taille_s1, taille_s2;
    int taille_min;
    int i;
    char char1, char2;
    int res_bool;
    
    printf("\n");
    printf(" Comparaison de chaînes :\n");

    /* on calcule les adresses  */
    adresse_s1 = calculer_adresse_complete(noeud_gauche);
    adresse_s2 = calculer_adresse_complete(noeud_droite);
    
    printf("  Chaîne gauche : pile[%d..]\n", adresse_s1);
    printf("  Chaîne droite : pile[%d..]\n", adresse_s2);
    
    /* Après, on obtient les types et tailles */
    type_s1 = obtenir_type_noeud(noeud_gauche);
    type_s2 = obtenir_type_noeud(noeud_droite);
    
    taille_s1 = obtenir_taille_chaine(type_s1);
    taille_s2 = obtenir_taille_chaine(type_s2);
    
    printf("  Taille gauche : %d caractères\n", taille_s1);
    printf("  Taille droite : %d caractères\n", taille_s2);
    
    /* taille min ??? */
    taille_min = (taille_s1 < taille_s2) ? taille_s1 : taille_s2;
    
    printf("  Comparaison sur : %d caractères\n", taille_min);
    
    /* Il faut aller caractère par caractère */
    i = 0;
    while (i < taille_min) {
        char1 = pile[adresse_s1 + i].valeur.caractere;
        char2 = pile[adresse_s2 + i].valeur.caractere;
        
        /* Si les caractères sont différents, on a trouvé la réponse */
        if (char1 != char2) {
            printf("  Différence trouvée à l'index %d : '%c' vs '%c'\n", 
                   i, char1, char2);
            
            /* Déterminer le résultat selon l'opérateur */
            switch (operateur) {
                case A_EGAL:      res_bool = 0; break;  /* Différents donc pas égaux */
                case A_DIFF:      res_bool = 1; break;  /* Différents donc != est vrai */
                case A_INF:       res_bool = (char1 < char2); break;
                case A_SUP:       res_bool = (char1 > char2); break;
                case A_INF_EGAL:  res_bool = (char1 < char2); break;
                case A_SUP_EGAL:  res_bool = (char1 > char2); break;
                default:
                    break;
            }
            
            resultat.booleen = (char)res_bool;
            printf("  Résultat : %s\n", resultat.booleen ? "true" : "false");
            return resultat;
        }
        
        i++;
    }

    /* Tous les caractères comparés sont identiques */
    /* On départage avec les tailles */
    
    printf("  Tous les %d premiers caractères sont identiques\n", taille_min);
    printf("  Départage avec les tailles : %d vs %d\n", taille_s1, taille_s2);
    
    /* Déterminer le résultat selon l'opérateur ET les tailles */
    switch (operateur) {
        case A_EGAL:
            /* Égaux seulement si même taille */
            res_bool = (taille_s1 == taille_s2);
            break;
        
        case A_DIFF:
            /* Différents si tailles différentes */
            res_bool = (taille_s1 != taille_s2);
            break;
        
        case A_INF:
            /* Plus petit si taille plus petite */
            res_bool = (taille_s1 < taille_s2);
            break;
        
        case A_SUP:
            /* Plus grand si taille plus grande */
            res_bool = (taille_s1 > taille_s2);
            break;
        
        case A_INF_EGAL:
            /* Plus petit ou égal si taille <= */
            res_bool = (taille_s1 <= taille_s2);
            break;
        
        case A_SUP_EGAL:
            /* Plus grand ou égal si taille >= */
            res_bool = (taille_s1 >= taille_s2);
            break;
        
        default:
            break;
    }
    
    resultat.booleen = (char)res_bool;
    printf("  Résultat : %s\n", resultat.booleen ? "true" : "false");
    
    return resultat;
}

/* Concatène récursivement les chaînes d'un arbre A_PLUS
 * 
 * Principe : Parcourt l'arbre récursivement et copie chaque chaîne
 *            séquentiellement dans la destination
 * IMPORTANT : On s'arrête dès qu'on rencontre une case NON initialisée
 * Pourquoi récursif ? Pour gérer s1 + s2 + s3 + ... (n chaînes)
 * L'arbre est : A_PLUS(A_PLUS(s1, s2), s3) pour s1 + s2 + s3
 */
static int concatener_recursif(arbre noeud, int* position_courante, 
                               int adresse_dest, int taille_dest) {
    int adresse_source, type_source, taille_source;
    int nb_chars_max, i;
    int total_copie;
    int chars_reellement_copies;
    
    if (noeud == NULL) {
        return 0;
    }
    
    /* Cas 1 : Noeud A_PLUS -> descendre récursivement */
    if (noeud->nature == A_PLUS) {
        total_copie = 0;
        
        /* Traiter d'abord le fils gauche (première chaîne) */
        total_copie += concatener_recursif(noeud->filsGauche, 
                                          position_courante, 
                                          adresse_dest, 
                                          taille_dest);
        
        /* Puis traiter le fils droit (chaîne suivante) */
        total_copie += concatener_recursif(noeud->filsGauche->frereDroit, 
                                          position_courante, 
                                          adresse_dest, 
                                          taille_dest);
        
        return total_copie;
    }
    
    /* Cas 2 : Noeud IDF -> copier cette chaîne */
    if (noeud->nature == A_IDF) {
        /* Calculer l'adresse de la source */
        adresse_source = calculer_adresse_complete(noeud);
        
        /* Obtenir la taille déclarée de la source */
        type_source = obtenir_type_noeud(noeud);
        taille_source = obtenir_taille_chaine(type_source);
        
        /* Vérifier s'il reste de l'espace dans la destination */
        if (*position_courante >= taille_dest) {
            printf("  Troncature : plus d'espace disponible, %s ignorée\n", 
                   obtenir_lexeme(noeud->valeur));
            return 0;
        }
        
        /* Calculer le nombre maximum de caractères qu'on pourrait copier */
        nb_chars_max = taille_source;
        if (*position_courante + nb_chars_max > taille_dest) {
            nb_chars_max = taille_dest - *position_courante;
        }
        
        /* Copier les caractères JUSQU'À la première case non initialisée */
        i = 0;
        while (i < nb_chars_max) {
            /* Vérifier si la case source est initialisée */
            if (!pile[adresse_source + i].est_initialisee) {
                /* Arrêter : cette chaîne se termine ici */
                break;
            }
            
            /* Copier le caractère */
            pile[adresse_dest + *position_courante + i].valeur.caractere = 
                pile[adresse_source + i].valeur.caractere;
            pile[adresse_dest + *position_courante + i].est_initialisee = 1;
            i++;
        }
        
        /* i contient maintenant le nombre réel de caractères copiés */
        chars_reellement_copies = i;
        
        /* Afficher le message avec le nombre réel */
        if (chars_reellement_copies > 0) {
            printf("  Copie '%s' : %d caractères à la position %d\n", 
                   obtenir_lexeme(noeud->valeur), chars_reellement_copies, *position_courante);
        } else {
            printf("  Copie '%s' : chaîne vide (aucun caractère initialisé)\n",
                   obtenir_lexeme(noeud->valeur));
        }
        
        /* Afficher si troncature à cause de la taille de destination */
        if (chars_reellement_copies == nb_chars_max && nb_chars_max < taille_source) {
            printf("  Troncature : espace destination insuffisant\n");
        }
        
        /* Avancer la position courante du nombre réel de caractères copiés */
        *position_courante += chars_reellement_copies;
        
        return chars_reellement_copies;
    }

    /* Cas 3 : autre type de noeud (ne devrait pas arriver) */
    return 0;
}

/* Concatène plusieurs chaînes dans une destination */
void concatener_chaines_dans_destination(int adresse_dest, arbre noeud_plus, int taille_dest) {
    int position_courante;
    int total_copie;
    
    printf("  Concaténation de chaînes détectée\n");
    printf("  Destination : pile[%d..%d] (%d caractères max)\n", 
           adresse_dest, adresse_dest + taille_dest - 1, taille_dest);
    
    /* Initialiser la position de départ */
    position_courante = 0;
    
    /* Lancer la copie récursive */
    total_copie = concatener_recursif(noeud_plus, &position_courante, 
                                     adresse_dest, taille_dest);
    
    /* Afficher le résultat */
    printf("  Résultat : %d caractères copiés au total\n", total_copie);
    
    if (position_courante < taille_dest) {
        printf("  Espace restant : %d caractères\n", taille_dest - position_courante);
    }
}
