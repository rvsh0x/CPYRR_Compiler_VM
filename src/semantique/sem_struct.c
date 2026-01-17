#include "semantique.h"
#include "construction.h"
#include "utils_communs.h"

NoeudType verifier_et_construire_acces_champ(NoeudType variable, int num_lex_champ,
                                             int ligne, int col, int len) {
    int type_base, type_champ;
    Nature nature_type;
    int index_rep, nb_champs;
    int i, num_lex, trouve;
    const char* nom_variable;
    const char* nom_champ;
    char note[128];

    /* Si le type de la variable est inconnu, erreur déjà signalée ailleurs */
    if (variable.type == -1) {
        return construire_acces_champ(variable, num_lex_champ, -1);
    }
    
    type_base = variable.type;
    nom_variable = obtenir_nom_variable(variable.tarbre);
    nom_champ = obtenir_lexeme(num_lex_champ);
    
    /* Vérifier que le type de base n'est pas un type simple */
    if (type_base >= 0 && type_base <= 3) {
        snprintf(note, sizeof(note), 
            "Tentative d'accès au champ '%s' sur un type '%s'",
            nom_champ, nom_type(type_base));
        
        ajouter_erreur_complete(ERR_SEMANTIQUE,
            ligne, col, len,
            "Accès champ invalide : type simple n'a pas de champs",
            note);
        
        return construire_acces_champ(variable, num_lex_champ, -1);
    }
    
    /* Vérifier que le type est bien une structure */
    nature_type = obtenir_nature(type_base);
    if (nature_type != TYPE_STRUCT) {
        const char* type_str = (nature_type == TYPE_ARRAY) ? "tableau" : "type complexe";
        snprintf(note, sizeof(note), "Type actuel : %s", type_str);
        
        ajouter_erreur_complete(ERR_SEMANTIQUE,
            ligne, col, len,
            "Accès champ invalide : pas une structure",
            note);
        
        return construire_acces_champ(variable, num_lex_champ, -1);
    }
    
    /* Chercher le champ dans la structure */
    index_rep = obtenir_type(type_base);
    nb_champs = obtenir_nb_champs_struct(index_rep);
    
    trouve = 0;
    type_champ = -1;
    
    for (i = 0; i < nb_champs && !trouve; i++) {
        obtenir_info_champ(index_rep, i, &num_lex, &type_champ);
        if (num_lex == num_lex_champ) {
            trouve = 1;
        }
    }
    
    /* Si le champ n'existe pas dans la structure */
    if (!trouve) {
        char note[256];
        snprintf(note, sizeof(note), 
            "La structure '%s' ne contient pas le champ '%s'",
            nom_variable, nom_champ);
        
        ajouter_erreur_complete(ERR_SEMANTIQUE,
            ligne, col, len,
            "Accès champ invalide : champ inexistant",
            note);
        
        type_champ = -1;
    }
    
    /* Construire et retourner le NoeudType */
    return construire_acces_champ(variable, num_lex_champ, type_champ);
}