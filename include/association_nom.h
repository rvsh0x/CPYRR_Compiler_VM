 /**
    * Fichier : association_nom.h
    * Description : Fichier d'entête pour la gestion de l'association de noms. 
*/
#ifndef _ASSOCIATION_NOM_H_
#define _ASSOCIATION_NOM_H_

#include "tab_declarations.h"



/* fonction generale de resolution de noms */
int association_nom(int num_lex, Nature nature_recherchee);

/* on va spécifier pour chaque nature*/
int chercher_struct(int num_lex);
int chercher_array(int num_lex);
int chercher_var(int num_lex);
int chercher_param(int num_lex);
int chercher_fonction(int num_lex);
int chercher_procedure(int num_lex);

#endif /* _ASSOCIATION_NOM_H_ */