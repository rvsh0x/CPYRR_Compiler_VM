 /**
    * Fichier : sauvegarde.h
    * Description : Coordination des sauvegardes pour générer le texte intermédiaire.
*/
#ifndef _SAUVEGARDE_H_
#define _SAUVEGARDE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "tab_lexico.h"
#include "tab_declarations.h"
#include "tab_representations.h"
#include "tab_regions.h"
#include "couleurs.h"
#include "ast.h"

/* Sauvegarde complète du programme compilé */
int sauvegarder_programme(const char* nom_fichier_source);

/* Utilitaires pour gestion des chemins */
char* creer_nom_dossier(const char* nom_source);
int creer_dossier_ti(const char* chemin_dossier);
char* construire_chemin_fichier(const char* dossier, const char* nom_fichier);


#endif /* _SAUVEGARDE_H_ */