 /**
    * Fichier : tab_lexico.h
    * Description : Structure et fonctions pour la table des lexèmes.
*/

#ifndef _TAB_LEXICO_H_
#define _TAB_LEXICO_H_

#include "tab_hashage.h"
#include "couleurs.h"

#define MAX_LEXEMES 500
#define MAX_LONGUEUR_LEXEME 100

/* Structure d'un lexeme */
typedef struct {
    char lexeme[MAX_LONGUEUR_LEXEME];
    int longueur;
    int suivant;
} Lexeme;

/* Table lexicographique */
extern Lexeme tab_lexico[MAX_LEXEMES];
extern int nb_lexemes;

/* Initialise la table lexicographique */
void initialiser_tab_lexico();

/* Recherche un lexeme dans la table */
int rechercher_lexeme(const char* lexeme, int longueur);

/* Insere un lexeme dans la table */
int inserer_lexeme(const char* lexeme, int longueur);

/* Retourne le lexeme correspondant au numero */
char* obtenir_lexeme(int numero);

/* Affiche la table lexicographique */
void afficher_tab_lexico();

/* Sauvegarde la table lexicographique */
int sauvegarder_lexique(const char* chemin_fichier);

#endif /* _TAB_LEXICO_H_ */