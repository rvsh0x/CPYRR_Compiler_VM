#include <stdio.h>
#include <string.h>
#include "erreurs.h"
#include "ast.h"

/* Définition des variables globales */
Erreur tab_erreurs[MAX_ERREURS];
int nb_erreurs = 0;
int ligne_courante = 1;
char* nom_fichier = NULL;

extern int cc;
extern int cdt;
extern int ltc;

/* Initialise le système d'erreurs */
void init_erreurs() {
    nb_erreurs = 0;
    ligne_courante = 1;
}

/* Ajoute une erreur avec message formaté */
void ajouter_erreur(TypeErr type, int ligne, const char* format, ...) {
    Erreur* e;
    va_list args;
    
    if (nb_erreurs >= MAX_ERREURS) {
        return; 
    }
    
    e = &tab_erreurs[nb_erreurs];
    e->type = type;
    e->ligne = ligne;
    e->colonne = cdt;       
    e->longueur = ltc;      
    e->note[0] = '\0';      
    
    va_start(args, format);
    vsnprintf(e->message, sizeof(e->message), format, args);
    va_end(args);
    
    nb_erreurs++;
}


void ajouter_erreur_complete(TypeErr type, int ligne, int colonne, 
                            int longueur, const char* message, const char* note) {
    Erreur* e;
    
    if (nb_erreurs >= MAX_ERREURS) {
        return; 
    }
    
    e = &tab_erreurs[nb_erreurs];
    e->type = type;
    e->ligne = ligne;
    e->colonne = colonne;
    e->longueur = longueur;
    strncpy(e->message, message, sizeof(e->message) - 1);
    e->message[sizeof(e->message) - 1] = '\0';
    
    /* Note optionnelle */
    if (note != NULL) {
        strncpy(e->note, note, sizeof(e->note) - 1);
        e->note[sizeof(e->note) - 1] = '\0';
    } else {
        e->note[0] = '\0';
    }
    
    nb_erreurs++;
}


/* Erreur d'exécution sans contexte (arrêt immédiat) 
    vu que c'est compliqué de gérer les contextes 
    ici on affiche juste un message d'erreur adapté 
    (il faut sauvegarder la ligne colone ... etc dans l'ast ... )*/
void erreur_runtime(const char* format, ...) {
    va_list args;
    char buffer[256];
    
    /* Formater le message */
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    /* Créer l'erreur (sans ligne/colonne) */
    ajouter_erreur_complete(ERR_RUNTIME, 0, 0, 0, buffer, NULL);

    /* Afficher immédiatement */
    printf("\n");
    afficher_erreurs();
    printf(ROUGE "Programme arrêté." RESET "\n");
    printf("\n");
    
    exit(1);
}


/* Compte les erreurs qui bloquent la compilation */
int compter_erreurs_bloquantes() {
    int i, compteur;
    
    compteur = 0;
    for (i = 0; i < nb_erreurs; i++) {
        if (tab_erreurs[i].type != ERR_WARNING) {
            compteur++;
        }
    }
    
    return compteur;
}

/* Compte uniquement les warnings */
int compter_warnings() {
    int i, compteur;
    
    compteur = 0;
    for (i = 0; i < nb_erreurs; i++) {
        if (tab_erreurs[i].type == ERR_WARNING) {
            compteur++;
        }
    }
    
    return compteur;
}


/* Fonctions auxiliaires ... gestion de l'affichage correct et propre  */

/* Génère une chaîne d'espaces pour alignement */
static char* generer_espaces(int nombre) {
    static char buffer[512];
    int i;
    
    if (nombre >= (int)sizeof(buffer)) {
        nombre = sizeof(buffer) - 1;
    }
    
    for (i = 0; i < nombre; i++) {
        buffer[i] = ' ';
    }
    buffer[nombre] = '\0';
    
    return buffer;
}

/* Génère les indicateurs visuels (^, ~) */
static void afficher_indicateurs(int colonne, int longueur) {
    int i;
    
    /* Espaces jusqu'à la colonne */
    printf("%s", generer_espaces(colonne - 1));
    
    /* Premier caractère : ^ en vert */
    if (longueur > 0) {
        printf(VERT GRAS "^" RESET);
        
        /* Caractères suivants : ~ en vert */
        for (i = 1; i < longueur; i++) {
            printf(VERT "~" RESET);
        }
    }
    
    printf("\n");
}



/* Affiche une ligne source avec numérotation */
static void afficher_ligne_source(int numero_ligne) {
    const char* ligne;
    
    ligne = obtenir_ligne_source(numero_ligne);
    if (ligne == NULL) {
        return;
    }

    /* Numéro de ligne */
    printf("   %2d | ", numero_ligne);
    
    /* Ligne source */
    printf("%s", ligne);
    
    /* Ajouter \n si la ligne n'en a pas */
    if (ligne[strlen(ligne) - 1] != '\n') {
        printf("\n");
    }
}

static void afficher_erreur_complete(Erreur* e) {
    /* Format : fichier:ligne:colonne: */
    if (nom_fichier) {
        printf("%s:%d:%d: ", nom_fichier, e->ligne, e->colonne);
    } else {
        printf("ligne %d:%d: ", e->ligne, e->colonne);
    }
    
    /* Type d'erreur */
    switch (e->type) {
        case ERR_LEXICAL:
            printf(ROUGE GRAS "lexical error" RESET ": ");
            break;
        case ERR_SYNTAXIQUE:
            printf(ROUGE GRAS "syntax error" RESET ": ");
            break;
        case ERR_SEMANTIQUE:
            printf(ROUGE GRAS "semantic error" RESET ": ");
            break;
        case ERR_WARNING:
            printf(JAUNE GRAS "warning" RESET ": ");
            break;
        case ERR_RUNTIME: 
            printf(ROUGE GRAS "runtime error" RESET ": ");
            break;
        default:
            printf(ROUGE GRAS "error" RESET ": ");
            break;
    }
    
    /* Message */
    printf("%s\n", e->message);

    /* Afficher ligne source SEULEMENT si longueur > 0 */
    if (contexte_source.lignes != NULL && e->longueur >= 0 && e->ligne > 0) {
        afficher_ligne_source(e->ligne);
        printf("      | ");
        afficher_indicateurs(e->colonne, e->longueur);
    }

    /* Note */
    if (e->note[0] != '\0') {
        printf(CYAN GRAS "note" RESET ": %s\n", e->note);
    }
    
    printf("\n");
}

/* Fonction de comparaison pour qsort */
static int comparer_erreurs(const void* a, const void* b) {
    const Erreur* e1 = (const Erreur*)a;
    const Erreur* e2 = (const Erreur*)b;
    
    /* Trier par ligne */
    if (e1->ligne != e2->ligne) {
        return e1->ligne - e2->ligne;
    }
    
    /* Si même ligne, trier par colonne */
    return e1->colonne - e2->colonne;
}

void afficher_erreurs() {
    int i;
    
    if (nb_erreurs == 0) {
        return;
    }
    /* Trier les erreurs par ligne AVANT affichage */
    qsort(tab_erreurs, nb_erreurs, sizeof(Erreur), comparer_erreurs);
    
    printf("\n");
    printf(ROUGE GRAS "Erreurs détectées : " RESET "\n");
    printf("\n");
    
    for (i = 0; i < nb_erreurs; i++) {
        afficher_erreur_complete(&tab_erreurs[i]);
    }
}