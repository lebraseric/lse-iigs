# build/Makefile — construction du LSE-IIGS via Golden Gate / ORCA/C 2.2.1
# Décisions appliquées : IMP-0135, IMP-0136, IMP-0140 à IMP-0143, IMP-0192, IMP-0193.
# À lancer depuis build/ : « make tests » (suites de tests), « make » (exécutable LSE).
# Chaque fichier est compilé depuis son propre répertoire : ORCA/C résout les
# #include "..." relatifs par rapport au répertoire courant (Construction §7).
# -I : aucun fichier .sym (en-têtes précompilés) n'est lu ni écrit.

IIX       := iix
SRC       := ../src
TST       := ../tests
TESTS_BIN := tests
PROFIL    ?= dev
PROGRAMME := LSE
KEEPTYPE  := EXE
CFLAGS    := -P -I +T
LFLAGS    := -P

ifeq ($(PROFIL),dev)
PROFILFLAG := cc=-dLSE_PROFIL_DEV=1
else
PROFILFLAG :=
endif

# Sondes portées par l'image d'essai ESSAIS.po ; PERF ne mesure que le profil
# cible, et n'est pas construite dans le profil dev. NOMBRES est la suite
# composants_nombres, à passer sur cible : sous Golden Gate, le format étendu
# de SANE n'a que 53 bits (audit du code, AP-04).
SONDES_IMAGE := $(TESTS_BIN)/sonde_console:SONDE:EXE $(TESTS_BIN)/sonde_fichiers:SONDEF:EXE \
                $(TESTS_BIN)/sonde_erreur:SONDEE:EXE
SONDES_DEP   := $(TESTS_BIN)/sonde_console $(TESTS_BIN)/sonde_fichiers $(TESTS_BIN)/sonde_erreur
ifneq ($(PROFIL),dev)
SONDES_IMAGE += $(TESTS_BIN)/sonde_performance:PERF:EXE $(TESTS_BIN)/composants_nombres:NOMBRES:EXE
SONDES_DEP   += $(TESTS_BIN)/sonde_performance $(TESTS_BIN)/composants_nombres
endif

# Dépendances de toute compilation (audit du code, AC-34) : tous les en-têtes
# du dépôt, les inclusions indirectes n'étant pas suivies règle par règle, et
# le témoin du profil, réécrit seulement quand PROFIL change ; modifier un
# en-tête ou changer de profil recompile donc tout ce que la cible demande.
ENTETES := $(wildcard $(SRC)/interfaces/*.h $(SRC)/noyau/*.h $(SRC)/plateforme/*.h \
                      $(TST)/commun/*.h $(TST)/composants/*.h)
TEMOIN  := profil.temoin
$(shell test "`cat $(TEMOIN) 2>/dev/null`" = "$(PROFIL)" || echo "$(PROFIL)" > $(TEMOIN))
DEPENDANCES := $(ENTETES) $(TEMOIN)

IMAGE     := LSE.po
VOLUME    := LSE
ESSAIS    := ESSAIS.po
RECONST   := ../02-HISTORIQUE/Reconstitution-PERFO-CHARG
RUBANS    := ruban_CHARG ruban_PERFO

.PHONY: all tests clean sonde image

all: $(PROGRAMME) $(IMAGE) $(ESSAIS)

# --- point d'entrée (seul .root, cf. IMP-0142) ---
main.root main.a: $(SRC)/plateforme/main.c $(DEPENDANCES)
	cd $(SRC)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) main.c keep=../../build/main

# --- src/noyau/ ---
analyseur.a: $(SRC)/noyau/analyseur.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) analyseur.c keep=../../build/analyseur

moniteur.a: $(SRC)/noyau/moniteur.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) moniteur.c keep=../../build/moniteur

programme.a: $(SRC)/noyau/programme.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) programme.c keep=../../build/programme

compilateur.a: $(SRC)/noyau/compilateur.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) compilateur.c keep=../../build/compilateur

interprete.a: $(SRC)/noyau/interprete.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) interprete.c keep=../../build/interprete

pile.a: $(SRC)/noyau/pile.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) pile.c keep=../../build/pile

contexte_exec.a: $(SRC)/noyau/contexte_exec.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) contexte_exec.c keep=../../build/contexte_exec

decompilateur.a: $(SRC)/noyau/decompilateur.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) decompilateur.c keep=../../build/decompilateur

bibliotheque.a: $(SRC)/noyau/bibliotheque.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) bibliotheque.c keep=../../build/bibliotheque

erreurs.a: $(SRC)/noyau/erreurs.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) erreurs.c keep=../../build/erreurs

donnees.a: $(SRC)/noyau/donnees.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) donnees.c keep=../../build/donnees

objets.a: $(SRC)/noyau/objets.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) objets.c keep=../../build/objets

rcim.a: $(SRC)/noyau/rcim.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) rcim.c keep=../../build/rcim

symboles.a: $(SRC)/noyau/symboles.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) symboles.c keep=../../build/symboles

codage.a: $(SRC)/noyau/codage.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) codage.c keep=../../build/codage

ruban.a: $(SRC)/noyau/ruban.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) ruban.c keep=../../build/ruban

taches.a: $(SRC)/noyau/taches.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) taches.c keep=../../build/taches

fichiers.a: $(SRC)/noyau/fichiers.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) fichiers.c keep=../../build/fichiers

# --- src/plateforme/ ---
console_iigs.a: $(SRC)/plateforme/console_iigs.c $(DEPENDANCES)
	cd $(SRC)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) console_iigs.c keep=../../build/console_iigs

fichiers_iigs.a: $(SRC)/plateforme/fichiers_iigs.c $(DEPENDANCES)
	cd $(SRC)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) fichiers_iigs.c keep=../../build/fichiers_iigs

horloge_iigs.a: $(SRC)/plateforme/horloge_iigs.c $(DEPENDANCES)
	cd $(SRC)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) horloge_iigs.c keep=../../build/horloge_iigs

memoire_iigs.a: $(SRC)/plateforme/memoire_iigs.c $(DEPENDANCES)
	cd $(SRC)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) memoire_iigs.c keep=../../build/memoire_iigs

# --- édition de liens : main en premier (manuel ORCA/C, chap. 7) ---
$(PROGRAMME): main.root main.a moniteur.a analyseur.a programme.a compilateur.a \
              interprete.a contexte_exec.a pile.a decompilateur.a bibliotheque.a fichiers.a codage.a ruban.a taches.a fichiers_iigs.a erreurs.a \
              donnees.a objets.a rcim.a symboles.a console_iigs.a horloge_iigs.a memoire_iigs.a
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) main moniteur analyseur programme compilateur interprete bibliotheque fichiers codage ruban taches fichiers_iigs contexte_exec pile decompilateur erreurs donnees objets rcim symboles console_iigs horloge_iigs memoire_iigs keep=$(PROGRAMME)

# --- images disque ProDOS pour l'Apple IIGS ou l'émulateur (IMP-0241) ---
# LSE.po, image de distribution (volume /LSE/) : LSE, de type EXE, et les
# rubans des programmes CHARG et PERFO, de type TXT, qui se rangent eux-mêmes
# à la lecture. ESSAIS.po (volume /ESSAIS/) : les sondes, de type EXE.
image: $(IMAGE) $(ESSAIS)

ruban_%: $(RECONST)/%.lse ../scripts/creer_ruban_programme.sh
	../scripts/creer_ruban_programme.sh $< $* $@

$(IMAGE): $(PROGRAMME) $(RUBANS) ../scripts/creer_image.sh
	../scripts/creer_image.sh $(IMAGE) $(VOLUME) $(PROGRAMME):LSE:EXE \
	    ruban_CHARG:CHARG:TXT ruban_PERFO:PERFO:TXT

$(ESSAIS): $(SONDES_DEP) ../scripts/creer_image.sh
	../scripts/creer_image.sh $(ESSAIS) ESSAIS $(SONDES_IMAGE)

# --- suites de tests (IMP-0192) ---
# Une règle explicite par suite : compilation du fichier de la suite (qui
# contient main), puis édition de liens, la suite en premier.

SUITES := types_plateforme composants_objets representation_rcim composants_symboles composants_erreurs \
          composants_compilateur_2a composants_compilateur_2b composants_compilateur_2c \
          composants_compilateur_2d composants_compilateur_t1600 \
          composants_compilateur_limites historiques_erreurs \
          composants_decompilateur historiques_decompilation \
          composants_programme composants_contexte composants_donnees \
          composants_interprete composants_entrees_sorties composants_horloge \
          plateforme_horloge composants_analyseur composants_moniteur composants_nombres plateforme_fichiers composants_bibliotheque composants_fichiers \
          composants_codage composants_taches plateforme_pile

$(TESTS_BIN)/types_plateforme: $(TST)/representation/types_plateforme.c $(DEPENDANCES)
	mkdir -p $(TESTS_BIN)
	cd $(TST)/representation && $(IIX) compile $(CFLAGS) $(PROFILFLAG) types_plateforme.c keep=../../build/$(TESTS_BIN)/types_plateforme
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/types_plateforme keep=$(TESTS_BIN)/types_plateforme

$(TESTS_BIN)/composants_objets: $(TST)/composants/composants_objets.c $(DEPENDANCES) objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_objets.c keep=../../build/$(TESTS_BIN)/composants_objets
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_objets objets memoire_iigs keep=$(TESTS_BIN)/composants_objets

$(TESTS_BIN)/representation_rcim: $(TST)/representation/representation_rcim.c $(DEPENDANCES) rcim.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/representation && $(IIX) compile $(CFLAGS) $(PROFILFLAG) representation_rcim.c keep=../../build/$(TESTS_BIN)/representation_rcim
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/representation_rcim rcim keep=$(TESTS_BIN)/representation_rcim

$(TESTS_BIN)/composants_symboles: $(TST)/composants/composants_symboles.c $(DEPENDANCES) symboles.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_symboles.c keep=../../build/$(TESTS_BIN)/composants_symboles
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_symboles symboles objets memoire_iigs keep=$(TESTS_BIN)/composants_symboles

$(TESTS_BIN)/composants_erreurs: $(TST)/composants/composants_erreurs.c $(DEPENDANCES) erreurs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_erreurs.c keep=../../build/$(TESTS_BIN)/composants_erreurs
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_erreurs erreurs keep=$(TESTS_BIN)/composants_erreurs

$(TESTS_BIN)/composants_compilateur_2a: $(TST)/composants/composants_compilateur_2a.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_2a.c keep=../../build/$(TESTS_BIN)/composants_compilateur_2a
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_2a compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_2a

$(TESTS_BIN)/composants_compilateur_2b: $(TST)/composants/composants_compilateur_2b.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_2b.c keep=../../build/$(TESTS_BIN)/composants_compilateur_2b
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_2b compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_2b

$(TESTS_BIN)/composants_compilateur_2c: $(TST)/composants/composants_compilateur_2c.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_2c.c keep=../../build/$(TESTS_BIN)/composants_compilateur_2c
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_2c compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_2c

$(TESTS_BIN)/composants_compilateur_t1600: $(TST)/composants/composants_compilateur_t1600.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_t1600.c keep=../../build/$(TESTS_BIN)/composants_compilateur_t1600
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_t1600 compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_t1600

$(TESTS_BIN)/composants_compilateur_2d: $(TST)/composants/composants_compilateur_2d.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_2d.c keep=../../build/$(TESTS_BIN)/composants_compilateur_2d
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_2d compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_2d

$(TESTS_BIN)/composants_compilateur_limites: $(TST)/composants/composants_compilateur_limites.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_limites.c keep=../../build/$(TESTS_BIN)/composants_compilateur_limites
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_limites compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_limites

$(TESTS_BIN)/historiques_erreurs: $(TST)/historiques/historiques_erreurs.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/historiques && $(IIX) compile $(CFLAGS) $(PROFILFLAG) historiques_erreurs.c keep=../../build/$(TESTS_BIN)/historiques_erreurs
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/historiques_erreurs compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/historiques_erreurs

$(TESTS_BIN)/composants_decompilateur: $(TST)/composants/composants_decompilateur.c $(DEPENDANCES) compilateur.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_decompilateur.c keep=../../build/$(TESTS_BIN)/composants_decompilateur
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_decompilateur compilateur decompilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_decompilateur

$(TESTS_BIN)/historiques_decompilation: $(TST)/historiques/historiques_decompilation.c $(DEPENDANCES) compilateur.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/historiques && $(IIX) compile $(CFLAGS) $(PROFILFLAG) historiques_decompilation.c keep=../../build/$(TESTS_BIN)/historiques_decompilation
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/historiques_decompilation compilateur decompilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/historiques_decompilation

$(TESTS_BIN)/composants_programme: $(TST)/composants/composants_programme.c $(DEPENDANCES) compilateur.a programme.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_programme.c keep=../../build/$(TESTS_BIN)/composants_programme
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_programme compilateur programme rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_programme

$(TESTS_BIN)/composants_contexte: $(TST)/composants/composants_contexte.c $(DEPENDANCES) compilateur.a programme.a contexte_exec.a pile.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a donnees.a decompilateur.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_contexte.c keep=../../build/$(TESTS_BIN)/composants_contexte
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_contexte compilateur programme contexte_exec pile donnees decompilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_contexte

$(TESTS_BIN)/composants_donnees: $(TST)/composants/composants_donnees.c $(DEPENDANCES) donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_donnees.c keep=../../build/$(TESTS_BIN)/composants_donnees
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_donnees donnees decompilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_donnees

$(TESTS_BIN)/composants_interprete: $(TST)/composants/composants_interprete.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_interprete.c keep=../../build/$(TESTS_BIN)/composants_interprete
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_interprete compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_interprete

$(TESTS_BIN)/console_bouchon.a: $(TST)/bouchons/console_bouchon.c $(DEPENDANCES)
	mkdir -p $(TESTS_BIN)
	cd $(TST)/bouchons && $(IIX) compile $(CFLAGS) $(PROFILFLAG) console_bouchon.c keep=../../build/$(TESTS_BIN)/console_bouchon

$(TESTS_BIN)/composants_entrees_sorties: $(TST)/composants/composants_entrees_sorties.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_entrees_sorties.c keep=../../build/$(TESTS_BIN)/composants_entrees_sorties
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_entrees_sorties compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_entrees_sorties

$(TESTS_BIN)/horloge_bouchon.a: $(TST)/bouchons/horloge_bouchon.c $(DEPENDANCES)
	mkdir -p $(TESTS_BIN)
	cd $(TST)/bouchons && $(IIX) compile $(CFLAGS) $(PROFILFLAG) horloge_bouchon.c keep=../../build/$(TESTS_BIN)/horloge_bouchon

$(TESTS_BIN)/composants_horloge: $(TST)/composants/composants_horloge.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_horloge.c keep=../../build/$(TESTS_BIN)/composants_horloge
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_horloge compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_horloge

$(TESTS_BIN)/plateforme_horloge: $(TST)/plateforme/plateforme_horloge.c $(DEPENDANCES) horloge_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) plateforme_horloge.c keep=../../build/$(TESTS_BIN)/plateforme_horloge
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/plateforme_horloge horloge_iigs keep=$(TESTS_BIN)/plateforme_horloge

$(TESTS_BIN)/composants_nombres: $(TST)/composants/composants_nombres.c $(DEPENDANCES) donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_nombres.c keep=../../build/$(TESTS_BIN)/composants_nombres
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_nombres donnees decompilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_nombres

$(TESTS_BIN)/plateforme_fichiers: $(TST)/plateforme/plateforme_fichiers.c $(DEPENDANCES) fichiers_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) plateforme_fichiers.c keep=../../build/$(TESTS_BIN)/plateforme_fichiers
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/plateforme_fichiers fichiers_iigs keep=$(TESTS_BIN)/plateforme_fichiers

$(TESTS_BIN)/composants_bibliotheque: $(TST)/composants/composants_bibliotheque.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_bibliotheque.c keep=../../build/$(TESTS_BIN)/composants_bibliotheque
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_bibliotheque compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_bibliotheque

$(TESTS_BIN)/composants_fichiers: $(TST)/composants/composants_fichiers.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_fichiers.c keep=../../build/$(TESTS_BIN)/composants_fichiers
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_fichiers compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_fichiers

$(TESTS_BIN)/composants_codage: $(TST)/composants/composants_codage.c $(DEPENDANCES) codage.a compilateur.a programme.a bibliotheque.a fichiers.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_codage.c keep=../../build/$(TESTS_BIN)/composants_codage
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_codage codage compilateur programme bibliotheque fichiers fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_codage

$(TESTS_BIN)/composants_taches: $(TST)/composants/composants_taches.c $(DEPENDANCES) taches.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_taches.c keep=../../build/$(TESTS_BIN)/composants_taches
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_taches taches keep=$(TESTS_BIN)/composants_taches

$(TESTS_BIN)/composants_analyseur: $(TST)/composants/composants_analyseur.c $(DEPENDANCES) analyseur.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_analyseur.c keep=../../build/$(TESTS_BIN)/composants_analyseur
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_analyseur analyseur keep=$(TESTS_BIN)/composants_analyseur

$(TESTS_BIN)/composants_moniteur: $(TST)/composants/composants_moniteur.c $(DEPENDANCES) moniteur.a analyseur.a compilateur.a programme.a interprete.a bibliotheque.a fichiers.a codage.a ruban.a taches.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_moniteur.c keep=../../build/$(TESTS_BIN)/composants_moniteur
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_moniteur moniteur analyseur compilateur programme interprete bibliotheque fichiers codage ruban taches fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_moniteur

# Pile du 65816 (IMP-0272 ; audit du code, AC-11) : profondeur des sessions les plus exigeantes.
$(TESTS_BIN)/plateforme_pile: $(TST)/plateforme/plateforme_pile.c $(DEPENDANCES) moniteur.a analyseur.a compilateur.a programme.a interprete.a bibliotheque.a fichiers.a codage.a ruban.a taches.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) plateforme_pile.c keep=../../build/$(TESTS_BIN)/plateforme_pile
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/plateforme_pile moniteur analyseur compilateur programme interprete bibliotheque fichiers codage ruban taches fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/plateforme_pile

# Sonde du pilote .CONSOLE (Moniteur IMP-0240) : hors de « make tests », car
# Golden Gate ne fournit pas le pilote ; à lancer sur cible.
sonde: $(TESTS_BIN)/sonde_console $(TESTS_BIN)/sonde_performance $(TESTS_BIN)/sonde_fichiers

$(TESTS_BIN)/sonde_fichiers: $(TST)/plateforme/sonde_fichiers.c $(DEPENDANCES)
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) sonde_fichiers.c keep=../../build/$(TESTS_BIN)/sonde_fichiers
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/sonde_fichiers keep=$(TESTS_BIN)/sonde_fichiers

$(TESTS_BIN)/sonde_performance: $(TST)/plateforme/sonde_performance.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a horloge_iigs.a $(TESTS_BIN)/console_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) sonde_performance.c keep=../../build/$(TESTS_BIN)/sonde_performance
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/sonde_performance compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon horloge_iigs rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/sonde_performance

$(TESTS_BIN)/sonde_console: $(TST)/plateforme/sonde_console.c $(DEPENDANCES) memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) sonde_console.c keep=../../build/$(TESTS_BIN)/sonde_console
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/sonde_console memoire_iigs keep=$(TESTS_BIN)/sonde_console

# SystemError rend la console (IMP-0273 ; audit du code, AC-12) : hors de « make tests »
# (le programme s'arrête volontairement sur « Stack overflow », code 255).
$(TESTS_BIN)/sonde_erreur: $(TST)/plateforme/sonde_erreur.c $(DEPENDANCES) console_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) sonde_erreur.c keep=../../build/$(TESTS_BIN)/sonde_erreur
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/sonde_erreur console_iigs keep=$(TESTS_BIN)/sonde_erreur

tests: $(addprefix $(TESTS_BIN)/,$(SUITES))
	@echecs=0; \
	for s in $(SUITES); do \
	    $(IIX) $(TESTS_BIN)/$$s || echecs=$$((echecs + 1)); \
	done; \
	echo "BILAN : $(words $(SUITES)) suite(s), $$echecs suite(s) en echec"; \
	test $$echecs -eq 0

clean:
	rm -f *.root *.a *.sym $(PROGRAMME) $(IMAGE) $(ESSAIS) $(RUBANS) $(TEMOIN)
	rm -rf $(TESTS_BIN)
