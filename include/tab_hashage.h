 /**
    * Fichier : tab_hashage.h
    * Description : Structure et fonctions pour la table de hashage des lexèmes.
*/

#ifndef _TAB_HASHAGE_H_
#define _TAB_HASHAGE_H_

#define TAILLE_HASHAGE 32

/* Table de hashage : indices vers les lexemes */
extern int tab_hashage[TAILLE_HASHAGE];

/* Initialise la table de hashage a -1 */
void initialiser_tab_hashage();

/* Calcule le hash-code d'un lexeme */
int calculer_hashage(const char* lexeme);

/* Affiche la table de hashage */
void afficher_tab_hashage();

#endif /* _TAB_HASHAGE_H_ */