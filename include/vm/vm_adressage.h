/**
 * Fichier : vm_adressage.h
 * Description : Calculs d'adresse 
 */
#ifndef _VM_ADRESSAGE_H_
#define _VM_ADRESSAGE_H_

/* Calcul d'adresse variable simple */
int calculer_adresse_variable(int num_declaration);

/* Calcul d'adresse tableau */
int calculer_adresse_tableau(int adresse_base, int num_representation, 
                             int* indices, int nb_indices);

/* Calcul d'adresse champ structure (à partir du déplacement) */
int calculer_adresse_champ_par_deplacement(int adresse_base, int deplacement);

#endif /* _VM_ADRESSAGE_H_ */