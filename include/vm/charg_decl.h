 /**
    * Fichier : charg_decl.h
    * Description : fonctions utiles pour le chargement du fichier declarations.txt
*/
#ifndef _CHARG_DECL_H_
#define _CHARG_DECL_H_

/**
 * Initialise le chargement du fichier declarations.txt
 * Réinitialise les compteurs.
 */
void init_chargement_declarations();

/* Traite l'en-tête du fichier declarations (ligne "nb_declarations: N") */
void traiter_entete_declarations(int nb_declarations);

/* Traite une déclaration (ligne "DECL index: nature=X region=Y ...") */
void traiter_declaration(int index, const char* nature_str, int region,
                         int description, int execution);

/* Finalise le chargement et vérifie la cohérence */
void finaliser_chargement_declarations();

/* Charge un fichier declarations.txt complet */
int charger_declarations(const char* chemin);

#endif /* _CHARG_DECL_H_ */