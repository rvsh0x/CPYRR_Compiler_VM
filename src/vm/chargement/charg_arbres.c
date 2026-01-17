#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm/charg_arbres.h"
#include "tab_regions.h"
#include "ast.h"
#include "erreurs.h"

/* Compteurs pour le chargement */
static int nb_regions_chargees = 0;
static int region_courante = -1;

/* Pile pour construire l'arbre */
#define MAX_PILE 1000

typedef struct {
    arbre noeud;
    int nb_enfants_attendus;
    int nb_enfants_recus;
} element_pile;

static element_pile pile[MAX_PILE];
static int sommet_pile = -1;

/* Convertit une chaîne "A_OPAFF" en constante A_OPAFF */
static int chaine_vers_nature(const char* str) {
    if (strcmp(str, "A_PROGRAMME") == 0)          return A_PROGRAMME;
    if (strcmp(str, "A_LISTE_INSTRUCTIONS") == 0) return A_LISTE_INSTRUCTIONS;
    if (strcmp(str, "A_LISTE_ARGUMENTS") == 0)    return A_LISTE_ARGUMENTS;
    if (strcmp(str, "A_LISTE_INDICES") == 0)      return A_LISTE_INDICES;
    if (strcmp(str, "A_LISTE_VARIABLES") == 0)    return A_LISTE_VARIABLES;
    if (strcmp(str, "A_OPAFF") == 0)              return A_OPAFF;
    if (strcmp(str, "A_IF_THEN_ELSE") == 0)       return A_IF_THEN_ELSE;
    if (strcmp(str, "A_WHILE") == 0)              return A_WHILE;
    if (strcmp(str, "A_APPEL_PROC") == 0)         return A_APPEL_PROC;
    if (strcmp(str, "A_APPEL_FCT") == 0)          return A_APPEL_FCT;
    if (strcmp(str, "A_RETURN") == 0)             return A_RETURN;
    if (strcmp(str, "A_LIRE") == 0)               return A_LIRE;
    if (strcmp(str, "A_ECRIRE") == 0)             return A_ECRIRE;
    if (strcmp(str, "A_VIDE") == 0)               return A_VIDE;
    if (strcmp(str, "A_PLUS") == 0)               return A_PLUS;
    if (strcmp(str, "A_MOINS") == 0)              return A_MOINS;
    if (strcmp(str, "A_MULT") == 0)               return A_MULT;
    if (strcmp(str, "A_DIV") == 0)                return A_DIV;
    if (strcmp(str, "A_MOINS_UNAIRE") == 0)       return A_MOINS_UNAIRE;
    if (strcmp(str, "A_ET") == 0)                 return A_ET;
    if (strcmp(str, "A_OU") == 0)                 return A_OU;
    if (strcmp(str, "A_NON") == 0)                return A_NON;
    if (strcmp(str, "A_EGAL") == 0)               return A_EGAL;
    if (strcmp(str, "A_DIFF") == 0)               return A_DIFF;
    if (strcmp(str, "A_INF") == 0)                return A_INF;
    if (strcmp(str, "A_SUP") == 0)                return A_SUP;
    if (strcmp(str, "A_INF_EGAL") == 0)           return A_INF_EGAL;
    if (strcmp(str, "A_SUP_EGAL") == 0)           return A_SUP_EGAL;
    if (strcmp(str, "A_IDF") == 0)                return A_IDF;
    if (strcmp(str, "A_ACCES_TABLEAU") == 0)      return A_ACCES_TABLEAU;
    if (strcmp(str, "A_ACCES_CHAMP") == 0)        return A_ACCES_CHAMP;
    if (strcmp(str, "A_CSTE_ENT") == 0)           return A_CSTE_ENT;
    if (strcmp(str, "A_CSTE_REELLE") == 0)        return A_CSTE_REELLE;
    if (strcmp(str, "A_CSTE_BOOL") == 0)          return A_CSTE_BOOL;
    if (strcmp(str, "A_CSTE_CHAR") == 0)          return A_CSTE_CHAR;
    if (strcmp(str, "A_CSTE_CHAINE") == 0)        return A_CSTE_CHAINE;
    if (strcmp(str, "A_CHAMP") == 0)              return A_CHAMP;
    if (strcmp(str, "A_LISTE_CHAMPS") == 0)       return A_LISTE_CHAMPS;

    fprintf(stderr, "Erreur: nature inconnue '%s'\n", str);
    return A_VIDE;
}

/* Empile un nœud avec son nombre d'enfants attendus */
static void empiler(arbre n, int nb_enfants) {
    if (sommet_pile >= MAX_PILE - 1) {
        fprintf(stderr, "Erreur: pile d'arbres pleine\n");
        return;
    }
    
    sommet_pile++;
    pile[sommet_pile].noeud = n;
    pile[sommet_pile].nb_enfants_attendus = nb_enfants;
    pile[sommet_pile].nb_enfants_recus = 0;
}

/* Dépile et retourne le nœud */
static arbre depiler() {
    arbre n;
    
    if (sommet_pile < 0) {
        return NULL;
    }
    
    n = pile[sommet_pile].noeud;
    sommet_pile--;
    return n;
}

/* Vérifie si le sommet de pile attend encore des enfants */
static int attente_enfants() {
    if (sommet_pile < 0) {
        return 0;
    }
    
    return pile[sommet_pile].nb_enfants_recus < pile[sommet_pile].nb_enfants_attendus;
}

/* Initialise le chargement */
void init_chargement_arbres() {
    nb_regions_chargees = 0;
    region_courante = -1;
    sommet_pile = -1;
}

/* Traite l'en-tête d'une région */
void traiter_entete_region_arbre(int num_region) {
    region_courante = num_region;
    sommet_pile = -1;
    nb_regions_chargees++;
}

/* Traite un nœud et le rattache à l'arbre en construction */
arbre traiter_noeud_arbre(const char* nature_str, int num_lex, int num_decl, int nb_enfants) {
    arbre nouveau, parent, frere;
    int nature, position, i;

    /* Convertir la chaîne nature en constante */
    nature = chaine_vers_nature(nature_str);

    /* Créer le nouveau nœud */
    nouveau = creer_noeud(nature, num_lex);
    nouveau->num_declaration = num_decl;
    
    /* Si la pile est vide, c'est la racine de la région */
    if (sommet_pile < 0) {
        associer_arbre_region(region_courante, nouveau);
        empiler(nouveau, nb_enfants);
        return nouveau;
    }
    
    /* Sinon, l'attacher au parent en haut de pile */
    if (attente_enfants()) {
        parent = pile[sommet_pile].noeud;
        position = pile[sommet_pile].nb_enfants_recus;
        
        /* Premier enfant → fils gauche */
        if (position == 0) {
            parent->filsGauche = nouveau;
        } 
        /* Autres enfants → chaîner par frereDroit */
        else {
            frere = parent->filsGauche;
            i = 1;
            while (i < position) {
                frere = frere->frereDroit;
                i++;
            }
            frere->frereDroit = nouveau;
        }
        
        /* Incrémenter le compteur d'enfants reçus */
        pile[sommet_pile].nb_enfants_recus++;
        
        /* Si le parent a reçu tous ses enfants, le dépiler */
        if (pile[sommet_pile].nb_enfants_recus >= pile[sommet_pile].nb_enfants_attendus) {
            depiler();
        }
    }
    
    /* Empiler le nouveau nœud s'il attend des enfants */
    if (nb_enfants > 0) {
        empiler(nouveau, nb_enfants);
    }
    
    return nouveau;
}

/* Finalise le chargement */
void finaliser_chargement_arbres() {
    printf("Arbres: %d régions chargées\n", nb_regions_chargees);
}

/* Charge un fichier arbres.txt complet via le parser */
int charger_arbres(const char* chemin) {
    extern FILE* yyin;
    extern int yyparse();
    
    init_chargement_arbres();
    
    yyin = fopen(chemin, "r");
    if (yyin == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier %s\n", chemin);
        return 0;
    }
    
    yyparse();
    fclose(yyin);
    
    finaliser_chargement_arbres();
    return 1;
}