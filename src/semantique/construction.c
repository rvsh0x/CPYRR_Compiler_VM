#include "construction.h"
#include "ast.h"
#include "tab_declarations.h"
#include "tab_lexico.h"
#include "semantique.h"
#include "utils_communs.h"

NoeudType construire_constante(int nature, int valeur, int ligne, int col, int len) {
    NoeudType resultat;
    
    /* Créer le noeud */
    resultat.tarbre = creer_noeud(nature, valeur);
    
    /* Déterminer le type selon la nature */
    switch (nature) {
        case A_CSTE_ENT:
            resultat.type = 0;  /* int */
            break;
        case A_CSTE_REELLE:
            resultat.type = 1;  /* real */
            break;
        case A_CSTE_CHAR:
            resultat.type = 3;  /* char */
            break;
        case A_CSTE_BOOL:
            resultat.type = 2;  /* bool */
            break;
        default:
            resultat.type = -1;
            break;
    }
    
    /* Capturer la position */
    resultat.ligne = ligne;
    resultat.colonne = col;
    resultat.longueur = len;
    
    return resultat;
}

NoeudType construire_variable_simple(int num_lex, int ligne, int col, int len) {
    NoeudType resultat;
    int num_decl;
    
    /* Créer le noeud IDF */
    resultat.tarbre = creer_noeud(A_IDF, num_lex);
    
    /* Vérifier que la variable existe */
    num_decl = verifier_variable(num_lex);
    definir_declaration(resultat.tarbre, num_decl); /* pour associer la déclaration */

    /* Calculer le type */
    if (num_decl >= 0) {
        resultat.type = obtenir_type(num_decl);
    } else {
        resultat.type = -1;  /* Erreur déjà signalée par verifier_variable */
    }
    
    /* Capturer la position */
    resultat.ligne = ligne;
    resultat.colonne = col;
    resultat.longueur = len;
    
    return resultat;
}

NoeudType construire_operation_bool(NoeudType gauche, NoeudType droite, 
                                    int operateur, int ligne, int col, int len) {
    NoeudType resultat;
    
    /* Construire l'opération booléenne (ET ou OU) */
    resultat.tarbre = concat_pere_fils(creer_noeud(operateur, -1),
                                      concat_pere_frere(gauche.tarbre, droite.tarbre));
    
    /* Les opérations booléennes retournent toujours bool */
    resultat.type = 2;
    
    /* Capturer la position de l'opérateur */
    resultat.ligne = ligne;
    resultat.colonne = col;
    resultat.longueur = len;
    
    return resultat;
}

NoeudType construire_comparaison(NoeudType gauche, NoeudType droite, 
                                 int operateur, int ligne, int col, int len) {
    NoeudType resultat;
    arbre noeud_op;
    
    /* Construire l'arbre de comparaison */
    noeud_op = creer_noeud(operateur, -1);
    resultat.tarbre = concat_pere_fils(noeud_op,
                                      concat_pere_frere(gauche.tarbre, droite.tarbre));
    
    /* Une comparaison retourne TOUJOURS un booléen */
    resultat.type = 2;
    resultat.ligne = ligne;
    resultat.colonne = col;
    resultat.longueur = len;
    
    return resultat;
}

NoeudType construire_negation(NoeudType operande, int ligne, int col, int len) {
    NoeudType resultat;
    
    /* Construire la négation */
    resultat.tarbre = concat_pere_fils(creer_noeud(A_NON, -1), operande.tarbre);
    
    /* La négation retourne toujours bool */
    resultat.type = 2;
    
    /* Capturer la position de l'opérateur NON */
    resultat.ligne = ligne;
    resultat.colonne = col;
    resultat.longueur = len;
    
    return resultat;
}

NoeudType construire_acces_champ(NoeudType variable, int num_lex_champ, int type_champ) {
    NoeudType resultat;
    arbre liste_champs;
    
    /* Construire la liste des champs */
    liste_champs = concat_pere_fils(
        creer_noeud(A_LISTE_CHAMPS, -1), 
        creer_noeud(A_CHAMP, num_lex_champ)
    );
    
    /* Ajouter l'accès au champ à l'arbre de la variable */
    resultat.tarbre = ajouter_liste_acces(variable.tarbre, liste_champs);
    
    /* Propager le type du champ */
    resultat.type = type_champ;
    
    /* Conserver la position de la variable de base */
    resultat.ligne = variable.ligne;
    resultat.colonne = variable.colonne;
    resultat.longueur = variable.longueur;
    
    return resultat;
}

NoeudType construire_acces_tableau(NoeudType variable, arbre liste_indices, int type_elements) {
    NoeudType resultat;
    
    /* Ajouter l'accès aux indices à l'arbre de la variable */
    resultat.tarbre = ajouter_liste_acces(variable.tarbre, liste_indices);
    
    /* Propager le type des éléments du tableau */
    resultat.type = type_elements;
    
    /* Conserver la position de la variable de base */
    resultat.ligne = variable.ligne;
    resultat.colonne = variable.colonne;
    resultat.longueur = variable.longueur;
    
    return resultat;
}

NoeudType construire_moins_unaire(NoeudType operande, int ligne, int col, int len) {
    NoeudType resultat;
    
    /* Construire le noeud moins unaire */
    resultat.tarbre = concat_pere_fils(creer_noeud(A_MOINS_UNAIRE, -1), operande.tarbre);
    resultat.type = operande.type;
    resultat.ligne = ligne;
    resultat.colonne = col;
    resultat.longueur = len;
    
    return resultat;
}

NoeudType construire_noeud_depuis_appel(arbre appel, int ligne, int col, int len) {
    NoeudType resultat;
    
    /* Construire le NoeudType depuis l'arbre */
    resultat.tarbre = appel;
    resultat.type = obtenir_type_noeud(appel);
    resultat.ligne = ligne;
    resultat.colonne = col;
    resultat.longueur = len;
    
    return resultat;
}