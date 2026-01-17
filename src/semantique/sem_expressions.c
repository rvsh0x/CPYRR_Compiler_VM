#include <stdio.h>
#include "semantique.h"
#include "construction.h"
#include "utils_communs.h"


NoeudType verifier_operation_arith(NoeudType gauche, NoeudType droite, 
                                   int op, int ligne, int col, int len) {
    NoeudType resultat;
    char note[256];
    const char* nom_op;
    int ligne_err, col_err, len_err;
    
    /* Déterminer nom opération */
    switch (op) {
        case A_PLUS:  nom_op = "+"; break;
        case A_MOINS: nom_op = "-"; break;
        case A_MULT:  nom_op = "*"; break;
        case A_DIV:   nom_op = "/"; break;
        default:      nom_op = "?"; break;
    }
    
    /* Position par défaut : l'opérateur (entre les deux) */
    ligne_err = ligne;
    col_err = col;
    len_err = len;
    
    /* Cas spécial : Concaténation de chaînes avec + */
    if (op == A_PLUS && est_type_chaine(gauche.type) && est_type_chaine(droite.type)) {
        /* Autoriser toute concaténation de chaînes
         * Pas de vérification de taille ici (on ne connaît pas la destination)
         * Les débordements seront gérés à l'exécution ou lors de l'affectation */
        
        /* Construire l'arbre de concaténation */
        resultat.tarbre = concat_pere_fils(creer_noeud(A_PLUS, -1),
                                          concat_pere_frere(gauche.tarbre, droite.tarbre));
        
        /* Le résultat garde le type de la chaîne gauche */
        resultat.type = gauche.type;
        resultat.ligne = ligne;
        resultat.colonne = col;
        resultat.longueur = len;
        
        return resultat;
    }
    
    /* Vérifier types numériques */
    if (!est_type_numerique(gauche.type) || !est_type_numerique(droite.type)) {
        /* Pointer sur l'opérande non numérique */
        if (!est_type_numerique(gauche.type)) {
            ligne_err = gauche.ligne;
            col_err = gauche.colonne;
            len_err = gauche.longueur;
        } else {
            ligne_err = droite.ligne;
            col_err = droite.colonne;
            len_err = droite.longueur;
        }
        
        snprintf(note, sizeof(note),
            "Gauche : %s | Droite : %s (attendu : entier ou réel)",
            nom_type(gauche.type), nom_type(droite.type));
        
        ajouter_erreur_complete(ERR_SEMANTIQUE, ligne_err, col_err, len_err,
            "Opération arithmétique invalide : opérandes non numériques", note);
    }
    
    /* Vérifier même type */
    if (gauche.type != droite.type && 
        est_type_numerique(gauche.type) && est_type_numerique(droite.type)) {
        
        /* Pointer sur l'opérande droite (on attend le type de gauche) */
        ligne_err = droite.ligne;
        col_err = droite.colonne;
        len_err = droite.longueur;
        
        snprintf(note, sizeof(note),
            "Gauche : %s | Droite : %s",
            nom_type(gauche.type), nom_type(droite.type));
        
        ajouter_erreur_complete(ERR_SEMANTIQUE, ligne_err, col_err, len_err,
            "Opération arithmétique invalide : types incompatibles", note);
    }
    
    /* Construire l'arbre */
    resultat.tarbre = concat_pere_fils(creer_noeud(op, -1),
                                      concat_pere_frere(gauche.tarbre, droite.tarbre));
    
    /* Type résultat = type opérande gauche (si valide) */
    resultat.type = gauche.type;
    resultat.ligne = ligne;
    resultat.colonne = col;
    resultat.longueur = len;
    
    return resultat;
}


NoeudType verifier_comparaison(NoeudType gauche, NoeudType droite, 
                               int operateur, int ligne, int col, int len) {
    char note[256];
    
    /* Si types inconnus, erreur déjà signalée ailleurs */
    if (gauche.type == -1 || droite.type == -1) {
        /* Construire quand même l'arbre pour continuer */
        return construire_comparaison(gauche, droite, operateur, ligne, col, len);
    }
    
    /* Vérification 1 : Même type (ou chaînes de même taille) */
    if (gauche.type != droite.type) {
        /* Exception : chaînes de même taille sont compatibles */
        if (est_type_chaine(gauche.type) && est_type_chaine(droite.type) /* &&
            chaines_meme_taille(gauche.type, droite.type) */) {
            /* OK : chaînes de , on continue */
        } else {
            /* Types vraiment incompatibles */
            snprintf(note, sizeof(note),
                "Gauche : %s | Droite : %s",
                nom_type(gauche.type), nom_type(droite.type));

            /* Pointer sur l'opérande droite (erreur de type) */
            ajouter_erreur_complete(ERR_SEMANTIQUE,
                droite.ligne, droite.colonne, droite.longueur,
                "Comparaison invalide : types incompatibles",
                note);

            /* Construire quand même pour continuer */
            return construire_comparaison(gauche, droite, operateur, ligne, col, len);
        }
    }

    /* Vérification 2 : Type comparable (types simples ou chaînes) */
    if (!est_type_simple(gauche.type) && !est_type_chaine(gauche.type)) {
        /* Pointer sur l'opérande gauche (type non comparable) */
        ajouter_erreur_complete(ERR_SEMANTIQUE,
            gauche.ligne, gauche.colonne, gauche.longueur,
            "Comparaison invalide : type non comparable (struct ou tableau non-chaîne)",
            NULL);
    }
    
    /* Construire et retourner l'arbre de comparaison */
    return construire_comparaison(gauche, droite, operateur, ligne, col, len);
}