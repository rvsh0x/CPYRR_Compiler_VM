#include "vm/vm_exec.h"
#include "vm/vm_instructions.h"
#include "vm/vm_expressions.h"
#include "vm/vm_utils.h"
#include "tab_declarations.h"
#include "tab_lexico.h"
#include "couleurs.h"
#include "utils_communs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

void executer_affectation(arbre a) {
    arbre expr_gauche, expr_droite;
    Valeur valeur;
    int adresse, type_dest;
    char* nom_var;
    int adresse_source, taille_chaine;
    
    expr_gauche = a->filsGauche;
    expr_droite = expr_gauche->frereDroit;
    
    nom_var = obtenir_lexeme(expr_gauche->valeur);
    printf("  Affectation : %s := ...\n", nom_var);
    
    /* Vérifier que c'est un IDF */
    if (expr_gauche->nature != A_IDF) {
        fprintf(stderr, ROUGE "Erreur : destination doit être IDF\n" RESET);
        exit(1);
    }
    
    /* Calculer l'adresse complète de destination */
    adresse = calculer_adresse_complete(expr_gauche);
    
    /* Déterminer le type de destination */
    type_dest = obtenir_type_noeud(expr_gauche);
    
    /* Gestion spéciale pour les chaînes de caractères : 
     * Pourquoi gérer ça ici et pas dans evaluer_arbre() ? 
     * 
     * - Une chaîne occupe N cases consécutives dans pile[] (ex: 20 cases)
     * - evaluer_arbre() retourne une seule "Valeur" (1 int/float/char/bool)
     * - Impossible de retourner 20 caractères dans une seule Valeur !
     * 
     * Solution : Traiter directement dans executer_affectation()
     * - On connaît déjà la destination (adresse calculée)
     * - On écrit directement le résultat dans pile[adresse...]
     * - Pas besoin de "retourner" quoi que ce soit */
    
    if (est_type_chaine(type_dest)) {
        printf("  Type chaîne de caractères détecté\n");
        
        /* Cas A : Concaténation (s3 := s1 + s2 ou s1 + s2 + s3) */
        if (expr_droite->nature == A_PLUS) {
            /* Obtenir la taille de la destination */
            taille_chaine = obtenir_taille_chaine(type_dest);
            
            /* Appeler la fonction de concaténation récursive */
            concatener_chaines_dans_destination(adresse, expr_droite, taille_chaine);
        } 
        /* Cas B : Affectation simple (s1 := s2) */
        else if (expr_droite->nature == A_IDF) {
            /* Calculer l'adresse de la source */
            adresse_source = calculer_adresse_complete(expr_droite);
            
            /* Obtenir la taille de la chaîne */
            taille_chaine = obtenir_taille_chaine(type_dest);
            
            /* Copier la chaîne */
            copier_chaine(adresse, adresse_source, taille_chaine);
            
            printf("    Chaîne '%s' : %d caractères copiés\n", nom_var, taille_chaine);
        }
        else { return; /* On peut lever une erreur ... Mais on va dire qu'on va pas arriver à ce cas */ }

    } else {
        /* Cas normal : évaluation d'une expression simple */
        /* Évaluer la source */
        valeur = evaluer_arbre(expr_droite);
        
        /* Écrire selon le type */
        printf("    Variable '%s' : pile[%d] = ", nom_var, adresse);
        afficher_valeur(valeur, type_dest);
        printf("\n");
        
        if (type_dest == 0) {
            /* Entier */
            ecrire_pile_entier(adresse, valeur.entier);
        } else if (type_dest == 1) {
            /* Réel */
            ecrire_pile_reel(adresse, valeur.reel);
        } else if (type_dest == 2) {
            /* Booléen */
            ecrire_pile_booleen(adresse, valeur.booleen);
        } else if (type_dest == 3) {
            /* Caractère */
            ecrire_pile_caractere(adresse, valeur.caractere);
        } else {
            /* Type inconnu, traiter comme entier par défaut */
            ecrire_pile_entier(adresse, valeur.entier);
        }
    }
    
    /* Afficher la pile */
    printf("\n");
    afficher_pile_complete();
}

/* Remplace un spécificateur par une variable */
static const char* remplacer_spec(const char* p, arbre* var_ptr) {
    char spec;
    Valeur val;
    int type;
    arbre var;
    
    /* Avancer après % */
    p++;
    
    /* %% → afficher % */
    if (*p == '%') {
        printf("%%");
        return p + 1;
    }
    
    spec = *p;
    
    /* Spécificateur valide → remplacer */
    if (spec == 'd' || spec == 'f' || spec == 'c') {
        var = (*var_ptr)->filsGauche;
        *var_ptr = var->frereDroit;
        
        val = evaluer_arbre(var);
        type = obtenir_type_noeud(var);
        
        afficher_valeur(val, type);
        
        return p + 1;
    }
    
    /* Pas un spécificateur → afficher tel quel */
    printf("%%");
    return p;
}

/* Affiche le format en mode parsing (avec spécificateurs) */
static void afficher_format_parse(const char* format, arbre liste_vars) {
    const char* p;
    arbre var_node;
    
    p = format;
    var_node = liste_vars;
    
    while (*p != '\0') {
        if (*p == '%' && *(p + 1) != '\0') {
            p = remplacer_spec(p, &var_node);
        } else {
            printf("%c", *p);
            p++;
        }
    }
}

/* Affiche le format en mode simple (sans spécificateurs) */
static void afficher_format_simple(const char* format, arbre liste_vars) {
    arbre var;
    Valeur val;
    int type;
    int premier;
    
    printf("%s", format);
    
    var = liste_vars;
    premier = 1;
    
    while (var != NULL && var->nature == A_LISTE_VARIABLES) {
        if (premier == 0) {
            printf(" ");
        }
        premier = 0;
        
        val = evaluer_arbre(var->filsGauche);
        type = obtenir_type_noeud(var->filsGauche);
        
        afficher_valeur(val, type);
        
        var = var->filsGauche->frereDroit;
    }
}

/* Instruction ECRIRE  */
void executer_ecriture(arbre a) {
    arbre format_node, liste_vars;
    char* format;
    int nb_specs; /* nombre de %d , %f , %c ...*/
    
    format_node = a->filsGauche;
    format = obtenir_lexeme(format_node->valeur);
    liste_vars = format_node->frereDroit;
    
    printf("  Write : %s\n", format);
    printf(GRAS "Sortie : " RESET);
    
    /* Compter spécificateurs */
    nb_specs = compter_specs(format);
    
    /* Choisir le mode d'affichage */
    if (nb_specs > 0) {
        afficher_format_parse(format, liste_vars);
    } else {
        afficher_format_simple(format, liste_vars);
    }
    
    printf("\n\n");
}

/* Instruction LIRE (read )
 * Principe : 
 *   1. Parcourir la liste des variables
 *   2. Pour chaque variable : calculer adresse, lire stdin selon type, écrire pile
 */
void executer_lecture(arbre a) {
    arbre curseur, var_noeud;
    int adresse, type, temp_int, resultat;
    Valeur valeur;
    char buffer[256];
    
    curseur = a->filsGauche;  /* A_LISTE_VARIABLES */
    
    while (curseur != NULL && curseur->nature == A_LISTE_VARIABLES) {
        var_noeud = curseur->filsGauche;
        
        /* Calculer l'adresse de la variable */
        adresse = calculer_adresse_complete(var_noeud);
        
        /* Obtenir le type */
        type = obtenir_type_noeud(var_noeud);
        
        /* Lire selon le type */
        if (type == 0) {
            /* Entier */
            resultat = scanf("%d", &valeur.entier);
            if (resultat != 1) {
                /* Vider le buffer en cas d'erreur */
                while (getchar() != '\n');
                erreur_runtime("Erreur de lecture : entier attendu");
                /* ici peut etre on doit pas utiliser erreur_runtime !! 
                pourquoi ? : en gros on peut imaginer des situations ou 
                l'utilisateur a plusieurs tentatives pour entrer un entier
                valide, et on pourrait vouloir lui donner une autre chance
                au lieu de planter le programme.... A REVOIR */
            }
            ecrire_pile_entier(adresse, valeur.entier);
            printf("  Lecture : %s = %d\n", obtenir_nom_variable(var_noeud), valeur.entier);
            
        } else if (type == 1) {
            /* Réel */
            resultat = scanf("%f", &valeur.reel);
            if (resultat != 1) {
                while (getchar() != '\n');
                erreur_runtime("Erreur de lecture : réel attendu");
            }
            ecrire_pile_reel(adresse, valeur.reel);
            printf("  Lecture : %s = %f\n", obtenir_nom_variable(var_noeud), valeur.reel);
            
        } else if (type == 2) {
            /* Booléen (lire 0 ou 1) */
            resultat = scanf("%d", &temp_int);
            if (resultat != 1) {
                while (getchar() != '\n');
                erreur_runtime("Erreur de lecture : booléen (0 ou 1) attendu");
            }
            valeur.booleen = (temp_int != 0) ? 1 : 0;
            ecrire_pile_booleen(adresse, valeur.booleen);
            printf("  Lecture : %s = %s\n", obtenir_nom_variable(var_noeud), 
                   valeur.booleen ? "true" : "false");
            
        } else if (type == 3) {
            /* Caractère */
            resultat = scanf(" %c", &valeur.caractere);  /* Espace avant %c ! */
            if (resultat != 1) {
                while (getchar() != '\n');
                erreur_runtime("Erreur de lecture : caractère attendu");
            }
            ecrire_pile_caractere(adresse, valeur.caractere);
            printf("  Lecture : %s = '%c'\n", obtenir_nom_variable(var_noeud), valeur.caractere);
            
        } else {
            /* devrait pas arriver mais bon ... gestion propre */
            erreur_runtime("Type non supporté pour lecture");
        }
        
        /* Vider le buffer après chaque lecture */
        while (getchar() != '\n');
        
        /* Passer à la variable suivante */
        curseur = var_noeud->frereDroit;
    }
    
    printf("\n");
    /* Afficher la pile */
    printf("\n");
    afficher_pile_complete();
}

/* Instruction IF/THEN/ELSE
 * Principe : 
 *   1. Évaluer la condition (retourne un booléen)
 *   2. Si true : exécuter branche THEN, sinon : exécuter branche ELSE
 */
void executer_si(arbre a) {
    arbre condition, branche_then, branche_else;
    Valeur resultat_condition;
    
    /* Extraire les parties */
    condition = a->filsGauche;
    branche_then = condition->frereDroit;
    branche_else = branche_then->frereDroit;
    
    /* Évaluer la condition */
    printf("  Évaluation condition IF...\n");
    resultat_condition = evaluer_arbre(condition);
    
    /* Exécuter la branche appropriée */
    if (resultat_condition.booleen) {
        printf("  Condition : " VERT "true" RESET " -> Branche THEN\n");
        executer_arbre(branche_then);
        printf("\n");
    } else {
        printf("  Condition : " ROUGE "false" RESET " -> Branche ELSE\n");
        if (branche_else != NULL) {
            executer_arbre(branche_else);
            printf("\n");
        }
    }
    
    printf("  Fin IF\n\n");
    /* Afficher la pile */
    printf("\n");
    afficher_pile_complete();
}

/* Instruction WHILE
 * Principe :
 *   1. Évaluer la condition
 *   2. Tant que true : exécuter le corps puis réévaluer
 *   3. Si un return est exécuté dans le corps, sortir immédiatement
 */
void executer_tantque(arbre a) {
    arbre condition, corps;
    Valeur resultat_condition;
    int iteration;
    
    /* Extraire les parties */
    condition = a->filsGauche;
    corps = condition->frereDroit;
    
    /* Première évaluation */
    printf("  Évaluation condition WHILE...\n");
    resultat_condition = evaluer_arbre(condition);
    
    iteration = 0;
    
    /* Boucle */
    while (resultat_condition.booleen) {
        iteration++;
        printf("  Condition : " VERT "true" RESET " -> Itération %d\n", iteration);
        
        /* Exécuter le corps */
        executer_arbre(corps);
        
        /* Réévaluer la condition */
        printf("  Réévaluation condition WHILE...\n");
        resultat_condition = evaluer_arbre(condition);
    }
    
    printf("  Condition : " ROUGE "false" RESET " -> Sortie WHILE (après %d itération(s))\n\n", iteration);
    /* Afficher la pile */
    printf("\n");
    afficher_pile_complete();
}

/* Instruction RETURN
 * Principe : 
 *   1. Si expression de retour : évaluer et écrire dans la case de retour
 *   2. Sinon : rien à faire (procédure)
 *  IL NE FAUT PAS OUBLIER LA SORTIE ANTICIPEE
 */
void executer_retour(arbre noeud) {
    arbre expression;
    Valeur valeur;
    int type;
    int NIS, adresse_retour;
    
    expression = noeud->filsGauche;
    
    if (expression != NULL) {
        /* Return avec valeur (fonction) */
        valeur = evaluer_arbre(expression);
        type = obtenir_type_noeud(expression);
        
        /* Écrire dans la case de retour */
        NIS = tab_regions[region_courante].nis;
        adresse_retour = BC + NIS + 1;

        if (type == 0) {
            ecrire_pile_entier(adresse_retour, valeur.entier);
        } else if (type == 1) {
            ecrire_pile_reel(adresse_retour, valeur.reel);
        } else if (type == 2) {
            ecrire_pile_booleen(adresse_retour, valeur.booleen);
        } else if (type == 3) {
            ecrire_pile_caractere(adresse_retour, valeur.caractere);
        }
    }
    /* Sinon : return vide (procédure), rien à faire */
        /* Afficher la pile */
    printf("\n");
    afficher_pile_complete();
}

/* Exécute un appel de procédure */
void executer_appel_procedure(arbre noeud) {
    int num_decl, num_region;
    arbre liste_args, curseur, arg, corps_procedure;
    Valeur arguments[100];  /* Tableau temporaire pour stocker les valeurs */
    int types_args[100];
    int nb_args, i, num_decl_param, adresse_param;
    
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
        
        /* Évaluer l'argument (region_courante est encore correcte) */
        arguments[nb_args] = evaluer_arbre(arg);
        types_args[nb_args] = obtenir_type_noeud(arg);
        
        nb_args++;
        curseur = arg->frereDroit;
    }

    /* empiler la zone d'exécution */
    empiler_zone(num_region);

    /* il faut placer les arguments dans la pile !!!*/
    for (i = 0; i < nb_args; i++) {
        /* Trouver la déclaration du i-ème paramètre */
        num_decl_param = trouver_parametre(num_region, i);
        
        if (num_decl_param == -1) {
            erreur_runtime("Paramètre introuvable");
        }
        
        /* Calculer adresse : BC + execution */
        adresse_param = BC + tab_declarations[num_decl_param].execution;
        
        /* Écrire la valeur sauvegardée */
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

    /* Exécuter le corps de la procédure */
    corps_procedure = obtenir_arbre_region(num_region);
    executer_arbre(corps_procedure);

    /* Afficher la pile */
    printf("\n");
    afficher_pile_complete();

    /* faut pas oublier de dépiler la zone d'exécution */
    depiler_zone();
}

void executer_arbre(arbre a) {
    arbre fils_gauche, frere_droit;
    
    if (a == NULL) {
        return;
    }
    
    printf(BLEU_CLAIR "Exécution : %s\n" RESET, nature_noeud_vers_chaine(a->nature));
    
    switch (a->nature) {
        case A_OPAFF:
            executer_affectation(a);
            break;
        
        case A_ECRIRE:
            executer_ecriture(a);
            break;
        
        case A_LIRE:
            executer_lecture(a);
            break;
        
        case A_IF_THEN_ELSE:
            executer_si(a);
            break;
        
        case A_WHILE: 
            executer_tantque(a);
            break;

        case A_LISTE_INSTRUCTIONS:
            fils_gauche = a->filsGauche;
            
            if (fils_gauche != NULL) {
                executer_arbre(fils_gauche);
                
                frere_droit = fils_gauche->frereDroit;
                if (frere_droit != NULL) {
                    executer_arbre(frere_droit);
                }
            }
            break;

        case A_RETURN:
            executer_retour(a);
            break;

        case A_APPEL_PROC:
            executer_appel_procedure(a);
            break;

        case A_VIDE:
            printf("  (instruction vide)\n");
            break;
        
        default:
            fprintf(stderr, ROUGE "Erreur : instruction non gérée : %s\n" RESET,
                    nature_noeud_vers_chaine(a->nature));
            exit(1);
    }
}
