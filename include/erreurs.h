/*  
    Fichier : erreurs.h
    Description : Gestion des erreurs pour le compilateur CPYRR.
*/

#ifndef _ERREURS_H_
#define _ERREURS_H_

#include "tracking_colonnes.h"
#include "couleurs.h"
#include <stdarg.h>

#define MAX_ERREURS 50

/* Types d'erreurs (phase de détection) */
typedef enum {
    ERR_LEXICAL,     
    ERR_SYNTAXIQUE,
    ERR_SEMANTIQUE,
    ERR_WARNING,
    ERR_RUNTIME /* Erreur d'exécution */
} TypeErr;

/* Contexte fichier source */
typedef struct {
    char** lignes;        /* Tableau des lignes du fichier */
    int nb_lignes;        /* Nombre de lignes */
    char* nom_fichier;    /* Nom du fichier */
} ContexteSource;

/* Structure d'une erreur ENRICHIE */
typedef struct {
    TypeErr type;         /* Type d'erreur (lexical, syntaxique, sémantique) */
    int ligne;            /* Ligne de l'erreur */
    int colonne;          /* Colonne de début */
    int longueur;         /* Longueur du token/expression */
    char message[256];    
    char note[512];      
} Erreur;

/* Variables globales */
extern Erreur tab_erreurs[MAX_ERREURS];
extern int nb_erreurs;
extern int ligne_courante;
extern char* nom_fichier;

/* Variables de tracking colonnes (lexer) : gestion maintenant dans le fichier tracking_colonnes.c 
extern int cc;   // Colonne courante
extern int cdt;  // Colonne début token
extern int ltc;  // Longueur token courant

*/

/* Contexte source */
extern ContexteSource contexte_source;

/** 
 * Fonctions de gestion des erreurs
 */
void init_erreurs();
void ajouter_erreur(TypeErr type, int ligne, const char* format, ...);
void afficher_erreurs();

/**
 * Fonctions de gestion du contexte source
 */
void charger_fichier_source(const char* nom_fichier);
const char* obtenir_ligne_source(int numero_ligne);
void liberer_fichier_source();

/**
 * Amélioration de la fonction d'erreurs 
 */
void ajouter_erreur_complete(TypeErr type, int ligne, int colonne, 
                            int longueur, const char* message, const char* note);

/* Y a encore d'autres fonctions statiques pour afficher les indicateurs ... */

/**
 * Fonctions de comptage d'erreurs
 */

/* Compte uniquement les vraies erreurs (pas les warnings) */
int compter_erreurs_bloquantes();

/* Compte uniquement les warnings */
int compter_warnings();

/** 
 * Erreurs d'exécution (arrêt immédiat du programme)
 */
void erreur_runtime(const char* format, ...);

#endif /* _ERREURS_H_ */