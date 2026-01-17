/**
 * Fichier d'entête pour la gestion de la table des régions
 */
#ifndef _TAB_REGIONS_H_
#define _TAB_REGIONS_H_

#include "ast.h"

#define MAX_REGIONS 100

typedef struct {
    int numero;
    int region_parent;
    int nis;
    int taille;
    arbre instructions;
} info_region;

extern info_region tab_regions[MAX_REGIONS];

/* Initialise la table et crée la région 0 */
void initialiser_tab_regions();

/* Crée une nouvelle région et retourne son numéro */
int creer_region(int parent);

/* Associe un arbre d'instructions à une région */
void associer_arbre_region(int num_region, arbre a);

/* Retourne l'arbre d'une région */
arbre obtenir_arbre_region(int num_region);

/* Retourne le NIS d'une région */
int obtenir_nis_region(int num_region);

/* Retourne le parent d'une région */
int obtenir_parent_region(int num_region);

/* Affiche la table des régions */
void afficher_tab_regions();

/* Sauvegarde la table des régions */
int sauvegarder_regions(const char* chemin_fichier);

#endif /* _TAB_REGIONS_H_ */