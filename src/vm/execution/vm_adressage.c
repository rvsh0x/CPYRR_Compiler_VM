#include "vm/vm_adressage.h"
#include "vm/vm_pile.h"
#include "tab_declarations.h"
#include "tab_regions.h"
#include "tab_representations.h"
#include "couleurs.h"
#include "erreurs.h"
#include <stdio.h>
#include <stdlib.h>

int calculer_adresse_variable(int num_declaration) {
    int region_declaration;
    int deplacement;
    int NIS_declaration;
    int NIS_utilisation;
    int offset;
    int BC_declaration;
    int adresse;
    int decalage_retour;
    
    if (num_declaration < 0 || num_declaration >= MAX_DECLARATIONS) {
        fprintf(stderr, "Erreur exécution : numéro déclaration invalide (%d)\n", num_declaration);
        exit(1);
    }
    
    region_declaration = tab_declarations[num_declaration].region;
    deplacement = tab_declarations[num_declaration].execution;
    
    NIS_declaration = tab_regions[region_declaration].nis;
    NIS_utilisation = tab_regions[region_courante].nis;
    
    offset = NIS_utilisation - NIS_declaration;
    
    if (offset == 0) {
        BC_declaration = BC;
    } else {
        if (offset < 0 || BC + offset >= TAILLE_PILE) {
            fprintf(stderr, "Erreur exécution : offset chaînage invalide (%d)\n", offset);
            exit(1);
        }
        BC_declaration = pile[BC + offset].valeur.entier;
    }
    
    /* région 0 n'a PAS de chaînage dynamique */
    if (region_declaration == 0) {
        adresse = deplacement;  /* Directement le déplacement */
    } else {
        decalage_retour = determiner_si_fonction(region_declaration) ? 1 : 0;
        adresse = BC_declaration + /* NIS_declaration + 1 */ + deplacement + decalage_retour;
    }
    
    if (adresse < 0 || adresse >= TAILLE_PILE) {
        fprintf(stderr, "Erreur exécution : adresse calculée invalide (%d)\n", adresse);
        exit(1);
    }
    
    return adresse;
}

/* Calcule l'adresse d'un élément de tableau */
int calculer_adresse_tableau(int adresse_base, int num_representation, 
                             int* indices, int nb_indices) {
    int nb_dims;
    int type_elem;
    int T;
    int k;
    int binf, bsup;
    int enjambee;
    int aov;
    int offset;
    int adresse;
    
    if (num_representation < 0 || num_representation >= ipcv) {
        fprintf(stderr, "Erreur exécution : numéro représentation invalide (%d)\n", 
                num_representation);
        exit(1);
    }
    
    /* Format dans tab_representation : [type_elem] [nb_dim] [binf1] [bsup1] [binf2] [bsup2] ... */
    
    /* Récupérer type élément */
    type_elem = tab_representation[num_representation];
    
    /* Récupérer nombre de dimensions */
    nb_dims = tab_representation[num_representation + 1];
    
    if (nb_indices != nb_dims) {
        fprintf(stderr, "Erreur exécution : nombre indices (%d) != nombre dimensions (%d)\n", 
                nb_indices, nb_dims);
        exit(1);
    }
    
    /* Taille d'un élément : utiliser calculer_taille_type() */
    /* qui existe déjà dans tab_declarations.c */
    T = calculer_taille_type(type_elem);
    
    /* Calculer enjambées et origine virtuelle (cours) */
    /* Formule : a[i1,...,in] = aov + Σ(ek × ik) */
    /* avec : e1 = T */
    /*        ek+1 = ek × (bsupk - binfk + 1) */
    /*        aov = aimp - Σ(ek × binfk) */
    
    aov = adresse_base;
    offset = 0;
    enjambee = T;
    
    k = 0;
    while (k < nb_dims) {
        /* Position dans tab_representation : [type_elem] [nb_dim] [binf1] [bsup1] ... */
        binf = tab_representation[num_representation + 2 + k * 2];
        bsup = tab_representation[num_representation + 2 + k * 2 + 1];
        
        /* Vérifier bornes */
        if (indices[k] < binf || indices[k] > bsup) {
             erreur_runtime("Indice tableau hors bornes : [%d] = %d (bornes : [%d..%d])",
                   k, indices[k], binf, bsup);
        }
        
        /* Contribution à l'offset : Σ(ek × ik) */
        offset += enjambee * indices[k];
        
        /* Contribution à aov : - Σ(ek × binfk) */
        aov -= enjambee * binf;
        
        /* Calculer enjambée suivante : ek+1 = ek × (bsupk - binfk + 1) */
        if (k < nb_dims - 1) {
            enjambee = enjambee * (bsup - binf + 1);
        }
        
        k++;
    }
    
    /* Adresse finale : aov + offset */
    adresse = aov + offset;
    
    if (adresse < 0 || adresse >= TAILLE_PILE) {
        fprintf(stderr, "Erreur exécution : adresse tableau calculée invalide (%d)\n", adresse);
        exit(1);
    }
    
    return adresse;
}

/* Calcule l'adresse d'un champ de structure à partir du déplacement */
int calculer_adresse_champ_par_deplacement(int adresse_base, int deplacement) {
    int adresse;
    
    /* Adresse = base + déplacement */
    adresse = adresse_base + deplacement;
    
    if (adresse < 0 || adresse >= TAILLE_PILE) {
        fprintf(stderr, "Erreur exécution : adresse champ calculée invalide (%d)\n", adresse);
        exit(1);
    }
    
    return adresse;
}