/**
 * Fichier : vm_pile.h
 * Description : Gestion pile d'execution
 */
#ifndef _VM_PILE_H_
#define _VM_PILE_H_

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

#define TAILLE_PILE 5000
#define MAX_NIS 20

/* Union pour les valeurs */
typedef union {
    int entier;
    float reel;
    char booleen;
    char caractere;
} Valeur;

/* Cellule avec flag initialisation */
typedef struct {
    Valeur valeur;
    int est_initialisee;
} Cellule;

/* Pile globale */
extern Cellule pile[TAILLE_PILE];
extern int BC;
extern int region_courante;


/* Initialisation */
void initialiser_pile_execution();

/* Gestion des zones */
void empiler_zone(int num_region);
void depiler_zone();

/* Acces aux cellules */
Valeur lire_pile(int adresse);
void verifier_adresse(int adresse);
void ecrire_pile_entier(int adresse, int valeur);
void ecrire_pile_reel(int adresse, float valeur);
void ecrire_pile_booleen(int adresse, char valeur);
void ecrire_pile_caractere(int adresse, char valeur);

/* Verification initialisation */
int est_initialisee(int adresse);
void marquer_initialisee(int adresse);

/* pour le cas d'une fonction */
int determiner_si_fonction(int num_region);
void ecrire_valeur_retour(int valeur);
Valeur depiler_zone_fonction();

/* Affichage */
void afficher_pile_zone(int debut, int fin);
void afficher_pile_complete();

#endif /* _VM_PILE_H_ */