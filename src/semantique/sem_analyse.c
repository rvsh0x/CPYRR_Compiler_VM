#include <stdio.h>
#include "semantique.h"
#include "tab_regions.h"

/* Analyse sémantique qui reste à faire
 * Cette fonction est appelée APRÈS la construction complète de l'AST.
 * Elle effectue les vérifications qui nécessitent l'arbre complet :
 */
void analyser_semantique() {
    int i;
    extern int compteur_regions;
    
    /* Vérification 1 : Fonctions sans return (POST-PARSING)
     * 
     * Pour chaque région de fonction, vérifier qu'elle contient au moins
     * un A_RETURN. Cette vérification nécessite un parcours récursif de
     * l'arbre.
     * 
     * Note : La région 0 (programme principal) est ignorée car ce n'est
     * pas une fonction.
     */
    for (i = 1; i < compteur_regions; i++) {
        if (tab_regions[i].numero != -1) {
            verifier_region_fonction(i, tab_regions[i].instructions);
        }
    }
    
    /* Vérification 2 : Variables non utilisées
     * 
     * Parcourir toutes les déclarations et vérifier si chaque variable
     * est utilisée au moins une fois dans l'arbre. Cette vérification
     * nécessite l'arbre complet.
     */
    verifier_variables_non_utilisees();
}