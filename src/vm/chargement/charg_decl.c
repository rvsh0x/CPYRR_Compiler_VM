#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm/charg_decl.h"
#include "tab_declarations.h"

static int nb_attendu = 0;
static int nb_charge = 0;

static Nature str_vers_nature(const char* str) {
    if (strcmp(str, "TYPE_BASE") == 0) return TYPE_BASE;
    if (strcmp(str, "TYPE_STRUCT") == 0) return TYPE_STRUCT;
    if (strcmp(str, "TYPE_ARRAY") == 0) return TYPE_ARRAY;
    if (strcmp(str, "VAR") == 0) return NATURE_VAR;
    if (strcmp(str, "PARAM") == 0) return NATURE_PARAM;
    if (strcmp(str, "PROC") == 0) return NATURE_PROC;
    if (strcmp(str, "FCT") == 0) return NATURE_FCT;

    fprintf(stderr, "Attention: nature inconnue '%s'\n", str);
    return TYPE_BASE;
}

void init_chargement_declarations() {
    int i;
    
    nb_attendu = 0;
    nb_charge = 0;
    
    /* Réinitialiser comme initialiser_tab_declarations() */
    i = 0;
    while (i < MAX_DECLARATIONS) {
        tab_declarations[i].nature = -1;
        tab_declarations[i].suivant = -1;
        tab_declarations[i].region = -1;
        tab_declarations[i].description = -1;
        tab_declarations[i].execution = -1;
        i++;
    }
}

void traiter_entete_declarations(int nb_declarations) {
    if (nb_declarations < 0 || nb_declarations > MAX_LEXEMES) {
        fprintf(stderr, "Erreur: nombre de declarations invalide (%d)\n", 
                nb_declarations);
        return;
    }
    nb_attendu = nb_declarations;
}

void traiter_declaration(int index, const char* nature_str, int region,
                        int description, int execution) {
    if (nature_str == NULL) {
        fprintf(stderr, "Erreur: nature NULL\n");
        return;
    }
    
    if (index < 0) {
        fprintf(stderr, "Erreur: index negatif (%d)\n", index);
        return;
    }
    
    if (index >= MAX_DECLARATIONS) {
        fprintf(stderr, "Erreur: index >= MAX_DECLARATIONS (%d >= %d)\n", 
                index, MAX_DECLARATIONS);
        return;
    }
    
    /* Mise à jour prochaine_case_libre pour zone débordement */
    if (index >= MAX_LEXEMES && index >= prochaine_case_libre) {
        prochaine_case_libre = index + 1;
    }
    
    /* Charger la déclaration */
    tab_declarations[index].nature = str_vers_nature(nature_str);
    tab_declarations[index].suivant = -1;
    tab_declarations[index].region = region;
    tab_declarations[index].description = description;
    tab_declarations[index].execution = execution;
    
    nb_charge++;
}

void finaliser_chargement_declarations() {
    if (nb_charge != nb_attendu) {
        fprintf(stderr, "Attention: %d/%d declarations chargees\n", 
                nb_charge, nb_attendu);
    }
}

int charger_declarations(const char* chemin) {
    extern FILE* yyin;
    extern int yyparse();
    int resultat;
    
    if (chemin == NULL) {
        fprintf(stderr, "Erreur: chemin NULL\n");
        return 0;
    }

    yyin = fopen(chemin, "r");
    if (!yyin) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", chemin);
        return 0;
    }
    
    init_chargement_declarations();  /* Réinitialise tout à -1 */
    resultat = yyparse();
    fclose(yyin);
    
    if (resultat != 0) {
        fprintf(stderr, "Erreur: parsing echoue\n");
        return 0;
    }
    
    finaliser_chargement_declarations();
    return 1;
}
