CC = gcc
CFLAGS = -c -I./include
TARGET = compilateur
VM_TARGET = vm_test
LEX = lex
YACC = yacc -d

# Fichiers objets compilateur
OBJS = build/lex.yy.o build/y.tab.o \
       build/erreurs.o build/source_context.o \
       build/tab_hashage.o build/tab_lexico.o build/tab_declarations.o build/tab_representations.o build/tab_regions.o \
       build/pile_regions.o build/association_nom.o \
       build/sem_common.o build/sem_resolution.o build/sem_tableaux.o build/sem_struct.o build/sem_fct_proc.o build/sem_expressions.o build/sem_instructions.o build/sem_analyse.o build/construction.o \
       build/ast.o \
       build/tracking_colonnes.o \
	   build/utils_communs.o \
       build/sauvegarde.o \
       build/main.o

# Fichiers objets VM
VM_OBJS = build/vm_lex.yy.o build/vm_parser_ti.tab.o \
          build/charg_lex.o build/charg_decl.o build/charg_reg.o build/charg_rep.o \
          build/charg_arbres.o \
          build/vm_pile.o build/test_vm_pile.o

# Tables partagées pour VM
VM_SHARED = build/erreurs.o build/source_context.o \
            build/tab_hashage.o build/tab_lexico.o \
            build/tab_declarations.o build/tab_representations.o \
            build/tab_regions.o build/pile_regions.o \
            build/association_nom.o build/ast.o \
            build/tracking_colonnes.o build/utils_communs.o

# Règle principale
all: bin/$(TARGET) bin/$(VM_TARGET) bin/vm_interprete

# Créer dossiers
build bin:
		mkdir -p $@ 

# LEX/YACC compilateur
build/lex.yy.c: src/analyseur_lexical/lexer.l | build
		$(LEX) -o $@ $<

build/y.tab.c build/y.tab.h: src/analyseur_syntaxique/parser.y | build
		$(YACC) -o build/y.tab.c $<

# Compilation LEX/YACC compilateur
build/lex.yy.o: build/lex.yy.c build/y.tab.h
		$(CC) $(CFLAGS) -c $< -o $@

build/y.tab.o: build/y.tab.c
		$(CC) $(CFLAGS) -c $< -o $@

# Compilation utils
build/erreurs.o: src/utils/erreurs.c
		$(CC) $(CFLAGS) -c $< -o $@

build/tracking_colonnes.o: src/utils/tracking_colonnes.c
		$(CC) $(CFLAGS) -c $< -o $@

build/source_context.o: src/utils/source_context.c
		$(CC) $(CFLAGS) -c $< -o $@

build/pile_regions.o: src/utils/pile_regions.c
		$(CC) $(CFLAGS) -c $< -o $@

build/utils_communs.o: src/utils/utils_communs.c
		$(CC) $(CFLAGS) -c $< -o $@

# Compilation tables
build/tab_hashage.o: src/tables/tab_hashage.c
		$(CC) $(CFLAGS) -c $< -o $@

build/tab_lexico.o: src/tables/tab_lexico.c
		$(CC) $(CFLAGS) -c $< -o $@

build/tab_declarations.o: src/tables/tab_declarations.c
		$(CC) $(CFLAGS) -c $< -o $@

build/tab_representations.o: src/tables/tab_representations.c
		$(CC) $(CFLAGS) -c $< -o $@

build/tab_regions.o: src/tables/tab_regions.c
		$(CC) $(CFLAGS) -c $< -o $@

build/association_nom.o: src/utils/association_nom.c
		$(CC) $(CFLAGS) -c $< -o $@

build/ast.o: src/utils/ast.c
		$(CC) $(CFLAGS) -c $< -o $@

# Compilation sémantique
build/construction.o: src/semantique/construction.c
		$(CC) $(CFLAGS) -c $< -o $@

build/sem_common.o: src/semantique/sem_common.c
		$(CC) $(CFLAGS) -c $< -o $@

build/sem_resolution.o: src/semantique/sem_resolution.c
		$(CC) $(CFLAGS) -c $< -o $@

build/sem_tableaux.o: src/semantique/sem_tableaux.c
		$(CC) $(CFLAGS) -c $< -o $@

build/sem_struct.o: src/semantique/sem_struct.c
		$(CC) $(CFLAGS) -c $< -o $@

build/sem_fct_proc.o: src/semantique/sem_fct_proc.c
		$(CC) $(CFLAGS) -c $< -o $@

build/sem_expressions.o: src/semantique/sem_expressions.c
		$(CC) $(CFLAGS) -c $< -o $@

build/sem_instructions.o: src/semantique/sem_instructions.c
		$(CC) $(CFLAGS) -c $< -o $@

build/sem_analyse.o: src/semantique/sem_analyse.c
		$(CC) $(CFLAGS) -c $< -o $@

# Sauvegarde
build/sauvegarde.o: src/utils/sauvegarde.c
		$(CC) $(CFLAGS) -c $< -o $@

# Compilation main
build/main.o: src/main.c build/y.tab.h
		$(CC) $(CFLAGS) -Ibuild -c $< -o $@

# Exécutable compilateur
bin/$(TARGET): $(OBJS) | bin
		$(CC) -o $@ $^ -ll

# LEX/YACC VM
build/vm_lex.yy.c: src/vm/chargement/lexer_ti.l | build
		$(LEX) -o $@ $<

build/vm_parser_ti.tab.c build/vm_parser_ti.tab.h: src/vm/chargement/parser_ti.y | build
		$(YACC) -o build/vm_parser_ti.tab.c $<

# Compilation LEX/YACC VM
build/vm_lex.yy.o: build/vm_lex.yy.c build/vm_parser_ti.tab.h
		$(CC) $(CFLAGS) -Ibuild -c $< -o $@

build/vm_parser_ti.tab.o: build/vm_parser_ti.tab.c
		$(CC) $(CFLAGS) -Ibuild -c $< -o $@

# Compilation chargeurs VM
build/charg_lex.o: src/vm/chargement/charg_lex.c
		$(CC) $(CFLAGS) -Ibuild -c $< -o $@

build/charg_decl.o: src/vm/chargement/charg_decl.c
		$(CC) $(CFLAGS) -Ibuild -c $< -o $@

build/charg_reg.o: src/vm/chargement/charg_reg.c
		$(CC) $(CFLAGS) -Ibuild -c $< -o $@

build/charg_rep.o: src/vm/chargement/charg_rep.c
		$(CC) $(CFLAGS) -Ibuild -c $< -o $@

build/charg_arbres.o: src/vm/chargement/charg_arbres.c
		$(CC) $(CFLAGS) -Ibuild -c $< -o $@

build/test_chargement.o: src/vm/test_chargement.c
		$(CC) $(CFLAGS) -Ibuild -c $< -o $@

# Compilation VM pile 
build/vm_pile.o: src/vm/execution/vm_pile.c
		$(CC) $(CFLAGS) -c $< -o $@

build/test_vm_pile.o: src/vm/test_vm_pile.c
		$(CC) $(CFLAGS) -c $< -o $@

# Compilation VM exécution 
build/vm_exec.o: src/vm/execution/vm_exec.c
		$(CC) $(CFLAGS) -c $< -o $@

build/vm_utils.o: src/vm/execution/vm_utils.c
		$(CC) $(CFLAGS) -c $< -o $@

build/vm_adressage.o: src/vm/execution/vm_adressage.c
		$(CC) $(CFLAGS) -c $< -o $@

build/vm_expressions.o: src/vm/execution/vm_expressions.c
		$(CC) $(CFLAGS) -c $< -o $@

build/vm_instructions.o: src/vm/execution/vm_instructions.c
		$(CC) $(CFLAGS) -c $< -o $@


# Main VM 
build/main_vm.o: src/main_vm.c
		$(CC) $(CFLAGS) -Ibuild -c $< -o $@

# Exécutable VM test (existant)
bin/$(VM_TARGET): $(VM_OBJS) $(VM_SHARED) | bin
		$(CC) -o $@ $^ -ll

# Exécutable VM interprète 
bin/vm_interprete: build/main_vm.o build/vm_utils.o build/vm_adressage.o build/vm_exec.o build/vm_expressions.o build/vm_instructions.o  \
                   build/vm_pile.o build/charg_lex.o build/charg_decl.o build/charg_reg.o \
                   build/charg_rep.o build/charg_arbres.o \
                   build/vm_lex.yy.o build/vm_parser_ti.tab.o \
                   $(VM_SHARED) | bin
		$(CC) -o $@ $^ -ll

# Nettoyage
clean:
		rm -rf build bin

.PHONY: all clean
