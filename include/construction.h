/**
    * Fichier : construction.h
    * Description : Fichier d'entête pour la construction de nœuds d'AST avec types propagés.
*/
#ifndef _CONSTRUCTION_H_
#define _CONSTRUCTION_H_

#include "ast.h"

/* Structure pour valeur sémantique yacc avec type propagé */
typedef struct {
    arbre tarbre;
    int type;
    int ligne;
    int colonne;
    int longueur;
} NoeudType;

/** 
 * Construction de constantes arithmétiques et booléennes
 */
NoeudType construire_constante(int nature, int valeur, int ligne, int col, int len);

/** 
 * Construction de variable simple (IDF seul)
 */
NoeudType construire_variable_simple(int num_lex, int ligne, int col, int len);

/** 
 * Construction d'un noeud pour moins unaire
 */
NoeudType construire_moins_unaire(NoeudType operande, int ligne, int col, int len);


/** 
 * Construction d'opération booléenne (ET/OU)
 * Retourne NoeudType (type=2 toujours) 
 */
NoeudType construire_operation_bool(NoeudType gauche, NoeudType droite, 
                                    int operateur, int ligne, int col, int len);

/** 
 * Construit un noeud de comparaison
 */
NoeudType construire_comparaison(NoeudType gauche, NoeudType droite, 
                                 int operateur, int ligne, int col, int len);
/** 
 * Construction de négation (NON)
 * Retourne NoeudType (type=2 toujours)
 */
NoeudType construire_negation(NoeudType operande, int ligne, int col, int len);

/** 
 * Construction d'un accès à un champ de structure
 */
NoeudType construire_acces_champ(NoeudType variable, int num_lex_champ, int type_champ);

/** 
 * Construction d'un accès à un tableau
 */
NoeudType construire_acces_tableau(NoeudType variable, arbre liste_indices, int type_elements);



/**  Construit un NoeudType depuis un arbre d'appel
 * Utilisé quand un appel de fonction est utilisé comme expression
 */
NoeudType construire_noeud_depuis_appel(arbre appel, int ligne, int col, int len);

#endif /* _CONSTRUCTION_H_ */