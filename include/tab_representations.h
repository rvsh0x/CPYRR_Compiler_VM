 /**
    * Fichier : tab_representations.h
    * Description : Structure et fonctions pour la table des représentations.
*/

#ifndef _TAB_REPRESENTATIONS_H_
#define _TAB_REPRESENTATIONS_H_

#define MAX_REPRESENTATION 10000
#define MAX_POINTS_ENTREE 500

/* Tableau linéaire d'entiers stockant les représentations des types */
extern int tab_representation[MAX_REPRESENTATION];

/* ipcv : Indice Prochaine Case Vide */
extern int ipcv;

/* Variables temporaires pour construction progressive */
extern int nb_champs;        
extern int inc;              

extern int nb_params;        
extern int inp;              

extern int nb_dimensions;    
extern int ina;             
extern int type_base;        

/* Structure pour suivre les points d'entrée (utilisée pour l'affichage)  */
typedef struct {
    int index;     /* Index de début dans tab_representation */
    char type;     /* Type : 'S' struct, 'A' array, 'F' fonction, 'P' procédure */
} PointEntree;

extern PointEntree points_entree[MAX_POINTS_ENTREE];
extern int nb_points_entree;

/* Initialise la table de représentation */
void initialiser_tab_representation();

/**
 * Fonctions pour STRUCT
 * Format : [inc] nb_champs | num_lex₁ | type₁ | dépl₁ | num_lex₂ | type₂ | dépl₂ | ...
 */
void commencer_struct();                           /* Initialise nb_champs et réserve 1 case */
void ajouter_champ_struct(int num_lex, int type);  /* Ajoute un champ */
int finaliser_struct();                            /* Finalise et retourne l'index */

/* Ajoute un paramètre (fonctionne pour fonction ET procédure) */
void ajouter_parametre_courant(int num_lex, int type);
/** 
 * Fonctions pour ARRAY
 * Format : [ina] type_elem | nb_dim | borne_inf₁ | borne_sup₁ | borne_inf₂ | borne_sup₂ | ...
 */
void commencer_array();                                        /* Initialise nb_dimensions et réserve 2 cases */
void ajouter_dimension_array(int borne_inf, int borne_sup);    /* Ajoute une dimension */
int finaliser_array(int type_elements);                        /* Finalise et retourne l'index */

/** Fonctions pour PROCÉDURE
 * Format : [inp] nb_params | num_lex₁ | type₁ | num_lex₂ | type₂ | ...
 */
void commencer_procedure();                                    /* Initialise nb_params et réserve 1 case */
int finaliser_procedure();                                     /* Finalise et retourne l'index */

/** Fonctions pour FONCTION
 * Format : [inp] type_retour | nb_params | num_lex₁ | type₁ | num_lex₂ | type₂ | ...
 */
void commencer_fonction();                                     /* Initialise nb_params et réserve 2 cases */
void ajouter_parametre_fonction(int num_lex, int type);        /* Ajoute un paramètre */
int finaliser_fonction(int type_retour);                       /* Finalise et retourne l'index */

/** 
 * Fonctions de lecture (pour vérifications sémantiques)
 */
int obtenir_nb_champs_struct(int index);
void obtenir_info_champ(int index_struct, int num_champ, int* num_lex, int* type);
/* Pour la VM - Recherche par num_lex au lieu de numéro */
void obtenir_info_champ_par_lex(int index_struct, int num_lex_cherche, 
                                 int* type, int* deplacement);

int obtenir_nb_dimensions_array(int index);
void obtenir_bornes_dimension(int index_array, int num_dim, int* borne_inf, int* borne_sup);
int obtenir_type_elements_array(int index);

int obtenir_nb_params(int index);
void obtenir_info_param(int index_fonction, int num_param, int* num_lex, int* type);
int obtenir_type_retour_fonction(int index);

/* Affiche la table de représentation avec code couleurs */
void afficher_tab_representation();

/**
 * Remplit les déplacements des champs dans les structures
 */
void remplir_deplacements_structures();

/** 
 * Sauvegarde la table de représentation 
 */
int sauvegarder_representations(const char* chemin_fichier);

#endif /* _TAB_REPRESENTATIONS_H_ */