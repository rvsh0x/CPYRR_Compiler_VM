#include "ast.h"

const char* nature_noeud_vers_chaine(int nature) {
    switch (nature) {
        case A_PROGRAMME:          return "A_PROGRAMME";
        case A_LISTE_INSTRUCTIONS: return "A_LISTE_INSTRUCTIONS";
        case A_LISTE_ARGUMENTS:    return "A_LISTE_ARGUMENTS";
        case A_LISTE_INDICES:      return "A_LISTE_INDICES";
        case A_LISTE_VARIABLES:    return "A_LISTE_VARIABLES";
        case A_OPAFF:              return "A_OPAFF";
        case A_IF_THEN_ELSE:       return "A_IF_THEN_ELSE";
        case A_WHILE:              return "A_WHILE";
        case A_APPEL_PROC:         return "A_APPEL_PROC";
        case A_APPEL_FCT:          return "A_APPEL_FCT";
        case A_RETURN:             return "A_RETURN";
        case A_LIRE:               return "A_LIRE";
        case A_ECRIRE:             return "A_ECRIRE";
        case A_VIDE:               return "A_VIDE";
        case A_PLUS:               return "A_PLUS";
        case A_MOINS:              return "A_MOINS";
        case A_MULT:               return "A_MULT";
        case A_DIV:                return "A_DIV";
        case A_MOINS_UNAIRE:       return "A_MOINS_UNAIRE";
        case A_ET:                 return "A_ET";
        case A_OU:                 return "A_OU";
        case A_NON:                return "A_NON";
        case A_EGAL:               return "A_EGAL";
        case A_DIFF:               return "A_DIFF";
        case A_INF:                return "A_INF";
        case A_SUP:                return "A_SUP";
        case A_INF_EGAL:           return "A_INF_EGAL";
        case A_SUP_EGAL:           return "A_SUP_EGAL";
        case A_IDF:                return "A_IDF";
        case A_ACCES_TABLEAU:      return "A_ACCES_TABLEAU";
        case A_ACCES_CHAMP:        return "A_ACCES_CHAMP";
        case A_CSTE_ENT:           return "A_CSTE_ENT";
        case A_CSTE_REELLE:        return "A_CSTE_REELLE";
        case A_CSTE_BOOL:          return "A_CSTE_BOOL";
        case A_CSTE_CHAR:          return "A_CSTE_CHAR";
        case A_CSTE_CHAINE:        return "A_CSTE_CHAINE";
        case A_CHAMP:              return "A_CHAMP";
        case A_LISTE_CHAMPS:       return "A_LISTE_CHAMPS";
        default:                   return "A_INCONNU";
    }
}

void definir_declaration(arbre a, int num_decl) {
    if (a != NULL) {
        a->num_declaration = num_decl;
    }
}

arbre creer_noeud(int nature, int valeur) {
    arbre n;
    
    n = (arbre)malloc(sizeof(noeud));
    if (n == NULL) {
        fprintf(stderr, ROUGE "Erreur : allocation mémoire échouée\n" RESET);
        exit(EXIT_FAILURE);
    }
    
    n->nature = nature;
    n->valeur = valeur;
    n->num_declaration = -1;
    n->ligne = ligne_courante;
    n->colonne = cdt;
    n->longueur = ltc;
    n->filsGauche = NULL;
    n->frereDroit = NULL;
    return n;
}

arbre concat_pere_fils(arbre pere, arbre fils) {
    if (pere != NULL) {
        pere->filsGauche = fils;
    }
    return pere;
}

arbre concat_pere_frere(arbre pere, arbre frere) {
    if (pere != NULL) {
        pere->frereDroit = frere;
    }
    return pere;
}

arbre trouver_dernier_frere(arbre a) {
    if (a == NULL) {
        return NULL;
    }
    
    while (a->frereDroit != NULL) {
        a = a->frereDroit;
    }
    
    return a;
}

arbre ajouter_element_liste(arbre liste_existante, arbre nouvel_element, int nature_liste) {
    arbre nouveau_liste, curseur;
    
    /* Cas 1: Premier élément */
    if (liste_existante == NULL) {
        return concat_pere_fils(creer_noeud(nature_liste, -1), nouvel_element);
    }
    
    /* Cas 2: Ajouter à une liste existante */
    /* Créer nouveau nœud A_LISTE avec l'élément */
    nouveau_liste = concat_pere_fils(creer_noeud(nature_liste, -1), nouvel_element);
    
    /* Parcourir la chaîne horizontale pour trouver le dernier ÉLÉMENT (pas A_LISTE)  */
    curseur = liste_existante->filsGauche;
    
    while (curseur != NULL && curseur->frereDroit != NULL) {
        /* Sauter les A_LISTE et aller directement à l'élément suivant */
        if (curseur->frereDroit->nature == nature_liste) {
            /* curseur pointe sur un élément, son frère est un A_LISTE */
            /* On va au fils de ce A_LISTE */
            curseur = curseur->frereDroit->filsGauche;
        } else {
            /* Cas normal : avancer dans la chaîne  */
            curseur = curseur->frereDroit;
        }
    }
    
    /* Accrocher le nouveau A_LISTE comme frère du dernier élément */
    if (curseur != NULL) {
        curseur->frereDroit = nouveau_liste;
    }
    
    return liste_existante;
} 

/* Trouve le dernier élément d'accès dans une chaîne d'accès */
arbre trouver_dernier_element_acces(arbre base) {
    arbre curseur, element;
    
    if (base == NULL) {
        return NULL;
    }
    
    /* Si pas encore d'accès, retourner la base */
    if (base->filsGauche == NULL) {
        return base;
    }
    
    /* Parcourir les listes d'accès */
    curseur = base->filsGauche;
    
    while (curseur != NULL) {
        /* Descendre au fils (l'élément) */
        element = curseur->filsGauche;
        
        if (element == NULL) {
            return curseur;
        }
        
        /* Si l'élément a un frère, continuer */
        if (element->frereDroit != NULL) {
            curseur = element->frereDroit;
        } else {
            /* Dernier élément trouvé */
            return element;
        }
    }
    
    return base;
}

arbre ajouter_liste_acces(arbre base, arbre nouvelle_liste) {
    arbre dernier;
    
    if (base == NULL || nouvelle_liste == NULL) {
        return base;
    }
    
    /* Si pas encore de liste d'accès, ajouter comme fils */
    if (base->filsGauche == NULL) {
        base->filsGauche = nouvelle_liste;
        return base;
    }
    
    /* Sinon, trouver le dernier élément et ajouter comme frère */
    dernier = trouver_dernier_element_acces(base);
    dernier->frereDroit = nouvelle_liste;
    
    return base;
}


/* Compte le nombre d'enfants directs d'un nœud.
 * Un enfant = le fils gauche + tous ses frères droits.
 * 
 * Exemple : A_OPAFF avec fils A_IDF et frère A_CSTE → retourne 2
 */
static int compter_enfants(arbre noeud) {
    int count = 0;
    arbre enfant;
    
    if (noeud == NULL || noeud->filsGauche == NULL) {
        return 0;
    }
    
    /* Compter le fils gauche */
    count = 1;
    
    /* Compter tous les frères du fils gauche */
    enfant = noeud->filsGauche->frereDroit;
    while (enfant != NULL) {
        count++;
        enfant = enfant->frereDroit;
    }
    
    return count;
}

/* Sauvegarde récursivement un arbre en format préfixe.
 * 
 * Format d'une ligne : NATURE NUM_LEX NUM_DECL NB_ENFANTS
 * 
 * Le nombre d'enfants permet au parser de savoir combien de nœuds
 * suivent au niveau inférieur. L'indentation est optionnelle (lisibilité).
 */
static void sauvegarder_arbre_recursif(FILE* f, arbre noeud, int niveau) {
    int i;
    int nb_enfants;
    arbre enfant;
    
    if (noeud == NULL) {
        return;
    }
    
    /* Indentation (optionnelle, pour lisibilité) */
    for (i = 0; i < niveau; i++) {
        fprintf(f, "  ");
    }
    
    /* Écrire : nature, num_lex, num_decl, nb_enfants */
    fprintf(f, "%s %d %d %d\n", 
            nature_noeud_vers_chaine(noeud->nature),
            noeud->valeur,
            noeud->num_declaration,
            compter_enfants(noeud));
    
    /* Sauvegarder tous les enfants (fils gauche + ses frères) */
    enfant = noeud->filsGauche;
    while (enfant != NULL) {
        sauvegarder_arbre_recursif(f, enfant, niveau + 1);
        enfant = enfant->frereDroit;
    }
}

/* Sauvegarde l'arbre d'une région.
 * 
 * Format : REGION X:
 *          [arbre indenté]
 *          [ligne vide]
 */
void sauvegarder_arbre_region(FILE* fichier, arbre racine, int num_region) {
    fprintf(fichier, "REGION %d:\n", num_region);
    
    if (racine == NULL) {
        fprintf(fichier, "  (vide) 0\n");
    } else {
        sauvegarder_arbre_recursif(fichier, racine, 0);
    }
    
    fprintf(fichier, "\n");
}



static void afficher_arbre_rec(arbre n, int profondeur, int est_frere) {
    int i;
    char* lexeme;
    
    if (n == NULL) {
        return;
    }
    
    /* Afficher l'indentation */
    i = 0;
    while (i < profondeur) {
        printf("  ");
        i++;
    }
    
    /* Indiquer si c'est un fils (|) ou un frère (→) */
    if (profondeur > 0) {
        if (est_frere) {
            printf("→ ");  /* Frère */
        } else {
            printf("├─ ");  /* Fils */
        }
    }
    
    /* Afficher la nature en cyan */
    printf(CYAN "%s" RESET " : ", nature_noeud_vers_chaine(n->nature));

    /* Afficher la valeur selon la nature */
    if (n->nature == A_IDF || n->nature == A_APPEL_FCT || 
        n->nature == A_APPEL_PROC || n->nature == A_CHAMP || 
        n->nature == A_CSTE_REELLE) {
        /* Ces natures stockent num_lex */
        lexeme = obtenir_lexeme(n->valeur);
        if (lexeme != NULL) {
            printf(JAUNE "%d" RESET " (" VERT "%s" RESET ")", n->valeur, lexeme);
        } else {
            printf(JAUNE "%d" RESET, n->valeur);
        }
    } else if (n->nature == A_CSTE_BOOL) {
        printf(JAUNE "%s" RESET, n->valeur ? "true" : "false");
    } else if (n->nature == A_CSTE_CHAR) {
        printf(JAUNE "'%c'" RESET, (char)n->valeur);
    } else {
        printf(JAUNE "%d" RESET, n->valeur);
    }
    
    if (n->num_declaration != -1) {
        printf(" " MAGENTA "[decl:%d]" RESET, n->num_declaration);
    }
    printf("\n");
    
    /* Parcourir récursivement fils (profondeur + 1) et frères (même profondeur) */
    if (n->filsGauche != NULL) {
        afficher_arbre_rec(n->filsGauche, profondeur + 1, 0);  /* 0 = fils */
    }
    
    if (n->frereDroit != NULL) {
        afficher_arbre_rec(n->frereDroit, profondeur, 1);  /* 1 = frère */
    }
}

void afficher_arbre(arbre racine) {
    if (racine == NULL) {
        printf("(Arbre vide)\n");
        return;
    }

    printf("\n--- Arbre Abstrait ---\n");
    afficher_arbre_rec(racine, 0, 0);
    printf("-------------------------\n\n");
}

void liberer_arbre(arbre a) {
    if (a == NULL) {
        return;
    }
    
    /* Libérer récursivement fils et frères */
    if (a->filsGauche != NULL) {
        liberer_arbre(a->filsGauche);
        /* a->filsGauche = NULL; ... Souvenir de la L2 ...*/
    }
    
    if (a->frereDroit != NULL) {
        liberer_arbre(a->frereDroit);
    }
    
    /* Libérer le nœud lui-même */
    free(a);
}

void debug_pointeurs(arbre a, int profondeur) {
    int i;
    if (a == NULL) return;
    
    for (i = 0; i < profondeur; i++) printf("  ");
    printf("Noeud %p: %s | fils=%p | frere=%p\n", 
           (void*)a, nature_noeud_vers_chaine(a->nature),
           (void*)a->filsGauche, (void*)a->frereDroit);
    
    if (a->filsGauche) debug_pointeurs(a->filsGauche, profondeur + 1);
    if (a->frereDroit) debug_pointeurs(a->frereDroit, profondeur);
}