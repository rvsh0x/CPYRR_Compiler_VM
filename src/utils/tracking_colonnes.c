/* 
 * Variables globales pour le tracking des colonnes
 * Utilisées par le lexer et les fonctions d'erreur/AST
 */

/* Colonne courante dans la ligne */
int cc = 1;

/* Colonne de début du token courant */
int cdt = 1;

/* Longueur du token courant */
int ltc = 0;