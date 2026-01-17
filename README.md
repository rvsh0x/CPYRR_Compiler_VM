# Compilateur CPYRR et Machine Virtuelle

Compilateur complet en langage C, pour le langage de programmation CPYRR, incluant une machine virtuelle pour l'exécution des programmes compilés.

## Table des matières

- [Description](#description)
- [Architecture](#architecture)
- [Prérequis](#prérequis)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [Structure du projet](#structure-du-projet)
- [Format des fichiers générés](#format-des-fichiers-générés)
- [Langage CPYRR](#langage-cpyrr)
- [Détails techniques](#détails-techniques)

## Description

Ce projet implémente un compilateur complet pour le langage CPYRR, comprenant :

- **Analyseur lexical** : Reconnaissance des tokens du langage
- **Analyseur syntaxique** : Construction de l'arbre syntaxique abstrait (AST)
- **Analyseur sémantique** : Vérification des types, déclarations et règles sémantiques
- **Génération de code intermédiaire** : Production de fichiers texte intermédiaire (TI)
- **Machine virtuelle** : Interprétation et exécution des programmes compilés

Le compilateur transforme un fichier source CPYRR en représentation intermédiaire stockée dans plusieurs fichiers texte, puis la machine virtuelle charge et exécute ces fichiers.

## Architecture

Le projet est organisé en deux parties principales :

1. **Compilateur** (`compilateur`) : Analyse et compilation d'un fichier source CPYRR
2. **Machine virtuelle** (`vm_interprete`) : Chargement et exécution des fichiers texte intermédiaire

### Phases de compilation

1. **Analyse lexicale** : Transformation du texte source en tokens
2. **Analyse syntaxique** : Construction de l'AST selon la grammaire CPYRR
3. **Analyse sémantique** : Vérifications de types, déclarations, portées
4. **Génération TI** : Sauvegarde des tables et arbres en fichiers texte

### Exécution

La machine virtuelle charge les fichiers TI et exécute le programme sur une pile d'exécution.

## Prérequis

- **GCC** : Compilateur C (gcc)
- **Lex** : Générateur d'analyseurs lexicaux (lex)
- **Yacc/Bison** : Générateur d'analyseurs syntaxiques (yacc)
- **Make** : Outil de construction
- **Système d'exploitation** : Linux, macOS ou Unix-like

### Installation des dépendances

**Sur Ubuntu/Debian :**
```bash
sudo apt-get update
sudo apt-get install build-essential flex bison
```

**Sur macOS :**
```bash
brew install flex bison
```

**Sur Fedora/RHEL :**
```bash
sudo dnf install gcc make flex bison
```

## Installation

1. Cloner ou télécharger le projet
2. Compiler le projet :
```bash
make
```

Cette commande génère trois exécutables dans le dossier `bin/` :
- `compilateur` : Compilateur CPYRR
- `vm_interprete` : Machine virtuelle
- `vm_test` : Programme de test pour la VM

### Nettoyage

Pour supprimer les fichiers générés :
```bash
make clean
```

## Utilisation

### Compilation d'un programme CPYRR

```bash
./bin/compilateur [OPTIONS] fichier.cpyrr
```

**Options disponibles :**
- `-t` : Afficher les tables (lexicographique, déclarations, représentations, régions)
- `-h` : Afficher l'aide

**Exemple :**
```bash
./bin/compilateur -t exemple.cpyrr
```

Si la compilation réussit, un dossier `exemple_ti/` est créé contenant les fichiers texte intermédiaire.

### Exécution d'un programme compilé

```bash
./bin/vm_interprete <dossier_ti>
```

**Exemple :**
```bash
./bin/vm_interprete exemple_ti
```

La machine virtuelle charge les fichiers TI et exécute le programme.

### Exemple complet

```bash
# 1. Compiler un programme
./bin/compilateur programme.cpyrr

# 2. Si la compilation réussit, exécuter
./bin/vm_interprete programme_ti
```

## Structure du projet

```
CPYRR_Compiler_VM/
├── include/                    # Fichiers d'en-tête
│   ├── ast.h                  # Structures et fonctions pour l'AST
│   ├── semantique.h           # Analyse sémantique
│   ├── tab_*.h                # Tables (lexico, declarations, etc.)
│   ├── erreurs.h              # Gestion des erreurs
│   ├── construction.h         # Construction de nœuds AST
│   └── vm/                    # En-têtes machine virtuelle
│       ├── vm_exec.h
│       ├── vm_pile.h
│       └── charg_*.h          # Chargeurs de fichiers TI
├── src/
│   ├── main.c                 # Point d'entrée compilateur
│   ├── main_vm.c              # Point d'entrée machine virtuelle
│   ├── analyseur_lexical/
│   │   └── lexer.l            # Grammaire lexicale (Flex)
│   ├── analyseur_syntaxique/
│   │   └── parser.y           # Grammaire syntaxique (Yacc)
│   ├── semantique/            # Analyse sémantique
│   ├── tables/                # Implémentation des tables
│   ├── utils/                 # Utilitaires
│   └── vm/
│       ├── chargement/        # Chargeurs fichiers TI
│       └── execution/         # Exécution VM
├── Makefile                   # Fichier de construction
└── README.md                  # Ce fichier
```

## Format des fichiers générés

Lors de la compilation, un dossier `<nom_fichier>_ti/` est créé contenant :

- **lexique.txt** : Table lexicographique (identifiants et mots-clés)
- **declarations.txt** : Table des déclarations (variables, fonctions, types)
- **representations.txt** : Représentations des types (structures, tableaux, fonctions)
- **regions.txt** : Table des régions (portées)
- **arbres.txt** : Arbres syntaxiques abstraits par région

Ces fichiers sont au format texte et peuvent être inspectés manuellement.

## Langage CPYRR

### Types de base

- `int` : Entiers
- `real` : Nombres réels
- `char` : Caractères
- `bool` : Booléens (true/false)
- `string` : Chaînes de caractères

### Structures de données

**Types personnalisés :**
```cpyrr
type Point : struct
    x : int;
    y : int;
endstruct;
```

**Tableaux :**
```cpyrr
type TableauEntiers : array of int[1..10];
type Matrice : array of int[1..5, 1..5];
```

### Déclarations

**Variables :**
```cpyrr
var a : int;
var b : real;
```

**Types :**
```cpyrr
type MonType : int;
```

### Procédures et fonctions

**Procédure :**
```cpyrr
procedure afficher(x : int);
begin
    write(x);
end;
```

**Fonction :**
```cpyrr
function addition(a : int, b : int) : int;
begin
    return a + b;
end;
```

### Instructions

**Affectation :**
```cpyrr
a := 10;
```

**Conditionnelle :**
```cpyrr
if a > 0 then
    write("positif");
else
    write("negatif ou zero");
end;
```

**Boucle :**
```cpyrr
while i < 10 do
    i := i + 1;
end;
```

**Entrée/Sortie :**
```cpyrr
read(x);
write("Valeur : ", x);
```

**Retour :**
```cpyrr
return resultat;
```

### Expressions

**Arithmétiques :** `+`, `-`, `*`, `/`, `-` (unaire)

**Booléennes :** `&&` (ET), `||` (OU), `!` (NON)

**Comparaisons :** `=`, `<>`, `<`, `>`, `<=`, `>=`

**Accès :**
- Variable : `x`
- Tableau : `tab[i]`
- Champ : `point.x`
- Chaînage : `tab[i].x`

### Structure d'un programme

```cpyrr
prog
    // Déclarations de types
    type ... ;
    
    // Déclarations de variables
    var ... ;
    
    // Déclarations de procédures/fonctions
    procedure ... ;
    function ... ;
    
    // Instructions principales
begin
    // Code principal
end
```

## Détails techniques

### Tables de compilation

- **Table lexicographique** : Stockage des identifiants et mots-clés
- **Table de déclarations** : Gestion des déclarations avec chaînage
- **Table de représentations** : Représentation des types complexes
- **Table des régions** : Gestion de la portée (scope)
- **Table de hashage** : Accès rapide aux lexèmes

### Gestion de la mémoire

- Allocation dynamique pour l'AST
- Pile d'exécution pour la machine virtuelle
- Zones mémoire par région

### Gestion des erreurs

Le compilateur détecte et signale :
- Erreurs lexicales
- Erreurs syntaxiques
- Erreurs sémantiques (types, déclarations, portées)
- Warnings (variables non utilisées, etc.)

Les erreurs sont affichées avec :
- Numéro de ligne et colonne
- Contexte du code source
- Message d'erreur détaillé

### Machine virtuelle

La VM implémente :
- Pile d'exécution avec zones par région
- Évaluation d'expressions
- Exécution d'instructions
- Gestion des appels de fonctions/procédures
- Accès aux variables (simples, tableaux, structures)

## Licence

Voir le fichier LICENSE pour plus de détails.

## Support

Pour signaler des bugs ou proposer des améliorations, veuillez créer une issue dans le dépôt du projet.
