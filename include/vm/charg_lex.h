 /**
    * Fichier : charg_lex.h
    * Description : fonctions utiles pour le chargement du fichier lexique.txt
*/
#ifndef _CHARG_LEX_H_
#define _CHARG_LEX_H_

/**
 * Initialise le chargement du fichier lexique.txt
 * Réinitialise les compteurs et prépare la table lexicographique.
 */
void init_chargement_lexique();


/* Traite l'en-tête du fichier lexique (ligne "nb_entrees: N") */
void traiter_entete_lexique(int nb_entrees);

/* Traite une entrée lexicale (ligne "index: \"lexeme\"") */
void traiter_entree_lexique(int index, const char* lexeme);

/* Traite une chaîne avec séquences d'échappement ... Cas spécial */
char* traiter_chaine_echappee(const char* str, int longueur);

/* Finalise le chargement et vérifie la cohérence */
void finaliser_chargement_lexique();

/* Charge un fichier lexique.txt complet */
int charger_lexique(const char* chemin);

#endif /* _CHARG_LEX_H_ */