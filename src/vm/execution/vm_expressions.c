#include "vm/vm_exec.h"
#include "vm/vm_expressions.h"
#include "vm/vm_instructions.h"
#include "vm/vm_pile.h"
#include "vm/vm_adressage.h"
#include "vm/vm_utils.h"
#include "tab_declarations.h"
#include "tab_lexico.h"
#include "tab_representations.h"
#include "couleurs.h"
#include "utils_communs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Valeur evaluer_constante(arbre a) {
    Valeur resultat;
    char* lexeme;
    
    switch (a->nature) {
        case A_CSTE_ENT:
            resultat.entier = a->valeur;
            printf("  Constante entière : %d\n", resultat.entier);
            break;
        
        case A_CSTE_REELLE:
            /* Récupérer le lexème et convertir */
            lexeme = obtenir_lexeme(a->valeur);
            resultat.reel = atof(lexeme);
            printf("  Constante réelle : %f\n", resultat.reel);
            break;
        
        case A_CSTE_BOOL:
            resultat.booleen = (char)a->valeur;
            printf("  Constante booléenne : %s\n", 
                   resultat.booleen ? "true" : "false");
            break;
        
        case A_CSTE_CHAR:
            resultat.caractere = (char)a->valeur;
            printf("  Constante caractère : '%c'\n", resultat.caractere);
            break;
        
        case A_CSTE_CHAINE:
            /* Pour une chaîne, a->valeur contient le num_lex */
            resultat.entier = a->valeur;  /* Stocker num_lex temporairement */
            printf("  Constante chaîne : \"%s\"\n", obtenir_lexeme(a->valeur));
            break;
        
        default:
            fprintf(stderr, ROUGE "Erreur : type constante inconnu\n" RESET);
            exit(1);
    }
    
    return resultat;
}

Valeur evaluer_variable(arbre a) {
    int adresse;
    Valeur resultat;
    char* nom_var;
    int type;

    /* verif propre au cas ou */
    if (a->nature != A_IDF) {
        fprintf(stderr, ROUGE "Erreur : evaluer_variable sur non-IDF\n" RESET);
        exit(1);
    }
    
    nom_var = obtenir_lexeme(a->valeur);
    printf("  Lecture variable '%s'\n", nom_var);
    
    /* Calculer l'adresse complète (avec accès) */
    adresse = calculer_adresse_complete(a);
    printf("    adresse pile = %d\n", adresse);
    
    /* Lire la valeur */
    resultat = lire_pile(adresse);
    
    /* Déterminer le type pour affichage */
    type = obtenir_type_noeud(a);
    
    printf("    valeur = ");
    afficher_valeur(resultat, type);
    printf("\n");
    
    return resultat;
}


/* Évalue un appel de fonction */
Valeur evaluer_appel_fonction(arbre noeud) {
    int num_decl, num_region;
    arbre liste_args, curseur, arg, corps_fonction;
    Valeur arguments[100];
    int types_args[100];
    int nb_args, i, num_decl_param, adresse_param;
    Valeur valeur_retour;
    int decalage_retour; /* !!!! important pour la pile */

    /* Récupérer numéro de déclaration */
    num_decl = noeud->num_declaration;
    
    /* Récupérer numéro de région */
    num_region = tab_declarations[num_decl].execution;
    
    /* on va évaluer les arguments */
    liste_args = noeud->filsGauche;
    curseur = liste_args;
    nb_args = 0;
    
    while (curseur != NULL && curseur->nature == A_LISTE_ARGUMENTS) {
        arg = curseur->filsGauche;
        
        arguments[nb_args] = evaluer_arbre(arg);
        types_args[nb_args] = obtenir_type_noeud(arg);
        
        nb_args++;
        curseur = arg->frereDroit;
    }

    /* empiler la zone d'exécution */
    empiler_zone(num_region);

    decalage_retour = determiner_si_fonction(num_region) ? 1 : 0;
    /* il faut placer les arguments dans la pile !!!*/
    for (i = 0; i < nb_args; i++) {
        num_decl_param = trouver_parametre(num_region, i);
        
        if (num_decl_param == -1) {
            erreur_runtime("Paramètre introuvable");
        }
        
        adresse_param = BC + tab_declarations[num_decl_param].execution + decalage_retour;
        
        if (types_args[i] == 0) {
            ecrire_pile_entier(adresse_param, arguments[i].entier);
        } else if (types_args[i] == 1) {
            ecrire_pile_reel(adresse_param, arguments[i].reel);
        } else if (types_args[i] == 2) {
            ecrire_pile_booleen(adresse_param, arguments[i].booleen);
        } else if (types_args[i] == 3) {
            ecrire_pile_caractere(adresse_param, arguments[i].caractere);
        }
        
        printf("  Paramètre %d : pile[%d] = ", i, adresse_param);
        afficher_valeur(arguments[i], types_args[i]);
        printf("\n");
    }

    /* Exécuter le corps de la fonction */
    corps_fonction = obtenir_arbre_region(num_region);
    executer_arbre(corps_fonction);

    /* Afficher la pile */
    printf("\n");
    afficher_pile_complete();

    /* faut pas oublier de dépiler la zone d'exécution */
    valeur_retour = depiler_zone_fonction();
    
    return valeur_retour;
}


/* Fonction principale exportée */
Valeur evaluer_arbre(arbre a) {
    Valeur resultat;
    Valeur val_gauche, val_droite;
    int type;
    const char* nom_op;
    
    if (a == NULL) {
        fprintf(stderr, ROUGE "Erreur : evaluer_arbre NULL\n" RESET);
        exit(1);
    }
    
    printf(BLEU_CLAIR "Évaluation : %s\n" RESET, nature_noeud_vers_chaine(a->nature));
    
    switch (a->nature) {
        case A_CSTE_ENT:
        case A_CSTE_REELLE:
        case A_CSTE_BOOL:
        case A_CSTE_CHAR:
        case A_CSTE_CHAINE:
            resultat = evaluer_constante(a);
            break;
        
        case A_IDF:
            resultat = evaluer_variable(a);
            break;

        /* Opérations arithmétiques : */
        case A_PLUS:
        case A_MOINS:
        case A_MULT:
        case A_DIV:
            /* Évaluer les deux opérandes */
            val_gauche = evaluer_arbre(a->filsGauche);
            val_droite = evaluer_arbre(a->filsGauche->frereDroit);
            
            /* Déterminer le type (garanti identique par sémantique) */
            type = obtenir_type_noeud(a->filsGauche);
            
            /* Appliquer l'opération */
            resultat = appliquer_operation_arithmetique(a->nature, val_gauche, val_droite, type);
            
            /* Afficher le résultat .. pour debug */
            switch (a->nature) {
                case A_PLUS:  nom_op = "+"; break;
                case A_MOINS: nom_op = "-"; break;
                case A_MULT:  nom_op = "*"; break;
                case A_DIV:   nom_op = "/"; break;
                default:      nom_op = "?"; break;
            }
            
            printf("  Opération %s : ", nom_op);
            if (type == 0) {
                printf(CYAN "%d %s %d = %d\n" RESET, 
                       val_gauche.entier, nom_op, val_droite.entier, resultat.entier);
            } else if (type == 1) {
                printf(CYAN "%f %s %f = %f\n" RESET, 
                       val_gauche.reel, nom_op, val_droite.reel, resultat.reel);
            }
            break;
        
        /* Négation unaire */
        case A_MOINS_UNAIRE:
            /* Évaluer l'opérande */
            val_gauche = evaluer_arbre(a->filsGauche);
            type = obtenir_type_noeud(a->filsGauche);
            
            /* Appliquer la négation */
            resultat = appliquer_negation_unaire(val_gauche, type);
            
            /* Affichage debug (temporaire) */
            printf("  Négation unaire : ");
            if (type == 0) {
                printf("-%d = %d\n", val_gauche.entier, resultat.entier);
            } else if (type == 1) {
                printf("-%f = %f\n", val_gauche.reel, resultat.reel);
            }
            break;

        /* Comparaisons */
        case A_EGAL:
        case A_DIFF:
        case A_INF:
        case A_SUP:
        case A_INF_EGAL:
        case A_SUP_EGAL:
            /* Obtenir le type des opérandes */
            type = obtenir_type_noeud(a->filsGauche);

            /* Comparaison de chaînes : appel spécifique */
            if (est_type_chaine(type)) {
                resultat = comparer_chaines(a->filsGauche,
                                        a->filsGauche->frereDroit,
                                        a->nature);
            } else {
                /* Cas normal : types simples */

                /* Évaluer les deux opérandes */
                val_gauche = evaluer_arbre(a->filsGauche);
                val_droite = evaluer_arbre(a->filsGauche->frereDroit);
                
                /* Appliquer la comparaison */
                resultat = appliquer_comparaison(a->nature, val_gauche, val_droite, type);
                
                /* Affichage debug */
                printf("  Comparaison : %s\n", resultat.booleen ? "true" : "false");
            }
            break;

        /* Opérations booléennes */
        case A_ET:
        case A_OU:
            /* Évaluer les deux opérandes */
            val_gauche = evaluer_arbre(a->filsGauche);
            val_droite = evaluer_arbre(a->filsGauche->frereDroit);
            
            /* Appliquer l'opération booléenne */
            resultat = appliquer_operation_booleenne(a->nature, val_gauche, val_droite);
            
            /* Affichage debug (temporaire) */
            printf("  Opération booléenne %s : %s %s %s = %s\n",
                   a->nature == A_ET ? "AND" : "OR",
                   val_gauche.booleen ? "true" : "false",
                   a->nature == A_ET ? "and" : "or",
                   val_droite.booleen ? "true" : "false",
                   resultat.booleen ? "true" : "false");
            break;

        /* Négation booléenne */
        case A_NON:
            /* Évaluer l'opérande */
            val_gauche = evaluer_arbre(a->filsGauche);
            
            /* Appliquer la négation booléenne */
            resultat = appliquer_negation_booleenne(val_gauche);
            
            /* Affichage debug (temporaire) */
            printf("  Negation booleenne : not %s = %s\n",
                   val_gauche.booleen ? "true" : "false",
                   resultat.booleen ? "true" : "false");
            break;

        /* Appel de fonction */
        case A_APPEL_FCT:
            resultat = evaluer_appel_fonction(a);
            
            /* Afficher le retour (pour debug) */
            type = obtenir_type_noeud(a);  /* Type de retour de la fonction */
            printf("  Retour fonction : ");
            afficher_valeur(resultat, type);
            printf("\n");
            break;

        default:
            fprintf(stderr, ROUGE "Erreur : expression non gérée : %s\n" RESET,
                    nature_noeud_vers_chaine(a->nature));
            exit(1);
    }
    
    return resultat;
}

/* Calcule l'adresse complète d'une variable avec tous ses accès (champs + tableaux)
 * 
 * PRINCIPE ... TRÈS IMPORTANT !!! 
 * 1. Partir de l'adresse de base de l'IDF
 * 2. Parcourir les listes d'accès (fils de l'IDF)
 * 3. Pour chaque A_LISTE_CHAMPS : ajouter le déplacement du champ
 * 4. Pour chaque A_LISTE_INDICES : calculer l'offset du tableau (TOUS les indices consécutifs)
 * 5. Retourner l'adresse finale
 * 
 * exemple : Pour p.x[0,1].y
 *   - Adresse de p
 *   - + déplacement de x dans p
 *   - + offset de [0,1] dans x
 *   - + déplacement de y dans x[0,1]
 */
int calculer_adresse_complete(arbre noeud_idf) {
    int adresse_courante, type_courant;
    int num_decl, index_rep;
    arbre liste, champ, element, curseur_indices;
    int num_lex_champ, type_champ, deplacement_champ;
    int indices[10];  /* Maximum 10 dimensions */
    int nb_indices, continuer;
    Valeur val_indice;
    
    /* Vérifier que c'est bien un IDF */
    if (noeud_idf == NULL || noeud_idf->nature != A_IDF) {
        fprintf(stderr, ROUGE "Erreur : calculer_adresse_complete sur non-IDF\n" RESET);
        exit(1);
    }
    
    /* Adresse de base de la variable */
    num_decl = noeud_idf->num_declaration;
    adresse_courante = calculer_adresse_variable(num_decl);
    type_courant = obtenir_type(num_decl);
    
    /* Parcourir les listes d'accès */
    liste = noeud_idf->filsGauche;
    
    while (liste != NULL) {
        
        if (liste->nature == A_LISTE_CHAMPS) {
            /* --- ACCÈS CHAMP --- */
            champ = liste->filsGauche;  /* A_CHAMP */
            num_lex_champ = champ->valeur;
            
            /* Obtenir le type et déplacement du champ */
            index_rep = obtenir_type(type_courant);
            obtenir_info_champ_par_lex(index_rep, num_lex_champ, 
                                      &type_champ, &deplacement_champ);
            
            if (deplacement_champ == -1) {
                fprintf(stderr, ROUGE "Erreur exécution : champ non trouvé\n" RESET);
                exit(1);
            }
            
            /* Mettre à jour l'adresse et le type */
            adresse_courante = calculer_adresse_champ_par_deplacement(adresse_courante, deplacement_champ);
            type_courant = type_champ;
            
            /* Passer à la liste suivante */
            liste = champ->frereDroit;
            
        } else if (liste->nature == A_LISTE_INDICES) {
            /* --- ACCÈS TABLEAU --- */
            
            /* Collecter TOUS les indices consécutifs */
            nb_indices = 0;
            curseur_indices = liste;
            continuer = 1;
            
            while (curseur_indices != NULL && continuer == 1) {
                if (curseur_indices->nature == A_LISTE_INDICES) {
                    element = curseur_indices->filsGauche;
                    
                    /* Évaluer l'indice */
                    val_indice = evaluer_arbre(element);
                    indices[nb_indices] = val_indice.entier;
                    nb_indices++;
                    
                    /* Passer au suivant */
                    curseur_indices = element->frereDroit;
                } else {
                    /* Ce n'est plus une LISTE_INDICES */
                    continuer = 0;
                }
            }
            
            /* Calculer la nouvelle adresse ... */
            index_rep = obtenir_type(type_courant);
            adresse_courante = calculer_adresse_tableau(
                adresse_courante,
                index_rep,
                indices,
                nb_indices
            );
            
            /* Mettre à jour le type (éléments du tableau) */
            type_courant = obtenir_type_elements_array(index_rep);
            
            /* Passer à la liste suivante */
            liste = curseur_indices;
            
        } else {
            /* Autre chose, arrêter */
            liste = NULL;
        }
    }
    
    return adresse_courante;
}