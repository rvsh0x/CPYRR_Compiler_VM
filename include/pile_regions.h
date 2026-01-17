/**
    * Fichier : pile_regions.h
    * Description : Fichier d'entête pour la gestion de la pile des régions.
*/
#ifndef _PILE_REGIONS_H_
#define _PILE_REGIONS_H_

#define MAX_REGIONS 100

/* Pile des régions */
extern int pile_regions[MAX_REGIONS];
extern int sommet_pile;
extern int compteur_regions;

/* Initialise la pile et empile la région 0 */
void initialiser_pile_regions();

/* Empile un numéro de région */
void empiler_region(int num_region);

/* Dépile et retourne le sommet */
int depiler_region();

/* Retourne le sommet sans dépiler */
int obtenir_region_courante();

/* Incrémente et retourne un nouveau numéro de région */
int nouvelle_region();

/* Retourne la pile complète pour parcours */
int* obtenir_pile(int* taille);

/* Affiche la pile */
void afficher_pile_regions();

#endif /* _PILE_REGIONS_H_ */