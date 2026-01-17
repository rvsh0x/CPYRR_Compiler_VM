 /**
    * Fichier : charg_rep.h
    * Description : fonctions utiles pour le chargement du fichier representations.txt
*/
#ifndef _CHARG_REP_H_
#define _CHARG_REP_H_

/* Initialise le chargement du fichier representations.txt */
void init_chargement_representations();

/* Traite l'en-tête du fichier representations */
void traiter_entete_representations(int nb_entrees, int nb_points);

/* Traite un point d'entrée */
void traiter_point_entree(int num, char type, int index);

/* Démarre le traitement d'une structure */
void traiter_struct_debut(int index, int nb_champs);

/* Traite un champ de structure */
void traiter_champ(int num, int num_lex, int type, int deplacement);

/* Finalise le traitement d'une structure */
void traiter_struct_fin();

/* Démarre le traitement d'un tableau */
void traiter_array_debut(int index, int type_elem, int nb_dims);

/* Traite une dimension de tableau */
void traiter_dim(int num, int borne_inf, int borne_sup);

/* Finalise le traitement d'un tableau */
void traiter_array_fin();

/* Démarre le traitement d'une procédure */
void traiter_proc_debut(int index, int nb_params);

/* Traite un paramètre (proc ou fct) */
void traiter_param(int num, int num_lex, int type);

/* Finalise le traitement d'une procédure */
void traiter_proc_fin();

/* Démarre le traitement d'une fonction */
void traiter_fct_debut(int index, int type_retour, int nb_params);

/* Finalise le traitement d'une fonction */
void traiter_fct_fin();

/* Finalise le chargement */
void finaliser_chargement_representations();

/* Charge un fichier representations.txt complet */
int charger_representations(const char* chemin);

#endif /* _CHARG_REP_H_ */