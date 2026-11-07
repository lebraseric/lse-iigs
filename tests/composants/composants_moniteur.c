/* composants_moniteur.c — sessions complètes du moniteur
   EXI: EXI-0028, EXI-0029, EXI-0051, EXI-0052  VM: VM-2201
   IMP: IMP-0227 à IMP-0239
   Spécification des tests : chapitre Moniteur et console §10 ; dialogues
   d'INRDP75 (PG1, PG3) ; essais d'Éric sur LSE 8086 du 2026-10-24.
   Niveau : composants. Console : tests/bouchons/console_bouchon.c (une
   réponse terminée par 0x13 est validée par X-OFF, par 0x1B par ESC, sinon
   par RETURN) ; horloge : tests/bouchons/horloge_bouchon.c (3/11/72
   13:03:52). Chaque session se termine par AU REVOIR. */

#include <string.h>
#include "../commun/verif.h"
#include "../../src/noyau/moniteur.h"
#include "../../src/interfaces/interface_horloge.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/symboles.h"
#include "../../src/noyau/contexte_exec.h"
#include "../../src/interfaces/types_communs.h"
#include "../../src/interfaces/interface_fichiers.h"
#include "../../src/noyau/taches.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

segment "suite";            /* le segment blanc dépasserait 64 Kio en profil dev */

extern char bouchon_affichage[];
void bouchon_console(const char *const *lignes);
void bouchon_interruption(unsigned int rang);

#define X "\x13"     /* X-OFF */
#define ESC "\x1B"
#define XON "\x11"   /* X-ON : lecteur de ruban (T1600) */
#define ID1 "ID" X, "1" X, ""   /* identification 1 ; espace temporaire standard */
#define BANNIERE_F4 "CONSOLE NO. 01.\r\nLSE F4\r\nRUB-OUT REMPLACE CTRL X-OFF\r\n"   /* IMP-0266 */
#define BLANCS_F4   "\r\n\r\n\r\n\r\n\r\n"                                         /* MORSE, MULT */

static const char *session(const char *const *reponses)
{
    bouchon_console(reponses);
    moniteur_session();
    return bouchon_affichage;
}

/* La session a-t-elle affiché attendu ? Sinon, montre l'affichage. */
static int contient(const char *attendu)
{
    if (strstr(bouchon_affichage, attendu) != NULL) return 1;
    printf("  [%s]\n    ne contient pas [%s]\n", bouchon_affichage, attendu);
    return 0;
}

static int egal(const char *attendu)
{
    if (strcmp(bouchon_affichage, attendu) == 0) return 1;
    printf("  [%s]\n    attendu [%s]\n", bouchon_affichage, attendu);
    return 0;
}

#define SESSION(...) \
    do { static const char *const r_[] = { __VA_ARGS__, NULL }; session(r_); } while (0)

/* Remplace le ruban par le texte donné (IMP-0262). */
static void ruban_ecrire(const char *texte)
{
    FichierPlateforme f;
    plateforme_fichier_supprimer(ESPACE_RUBAN, "RUBAN");
    if (*texte == '\0') return;
    plateforme_fichier_creer_texte(ESPACE_RUBAN, "RUBAN");
    if (plateforme_fichier_ouvrir(ESPACE_RUBAN, "RUBAN", FICHIER_MISE_A_JOUR, &f) != PLAT_OK) return;
    plateforme_fichier_ecrire(f, 0, texte, (unsigned int) strlen(texte));
    plateforme_fichier_fermer(f);
}

/* Le ruban contient-il exactement ces lg octets ? */
static int ruban_vaut_n(const char *attendu, unsigned long lg)
{
    FichierPlateforme f;
    char t[200];
    unsigned long n;
    unsigned long i;
    if (plateforme_fichier_ouvrir(ESPACE_RUBAN, "RUBAN", FICHIER_LECTURE, &f) != PLAT_OK) return 0;
    n = plateforme_fichier_taille(f);
    if (n < sizeof t) plateforme_fichier_lire(f, 0, t, (unsigned int) n);
    plateforme_fichier_fermer(f);
    if (n == lg && memcmp(t, attendu, (size_t) n) == 0) return 1;
    printf("  ruban de %lu octets :", n);
    for (i = 0; i < n && i < sizeof t; i++) printf(" %02X", (unsigned char) t[i]);
    printf("\n");
    return 0;
}

static int ruban_vaut(const char *attendu)
{
    return ruban_vaut_n(attendu, strlen(attendu));
}

int main(void)
{
    VERIF_DEBUT("composants/moniteur");
    programme_initialiser();
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBMON"));
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_espace_vider(ESPACE_DONNEES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
    plateforme_fichier_supprimer(ESPACE_RACINE, "IDENTIFICATIONS");

    /* --- ouverture et fermeture, machine de bureau (INRDP75 §3-2, 2e partie §1) --- */
    SESSION("BO" X, "?38-24+47" X, "AU" X);
    VERIFIER(egal("BONJOUR\r\nLSE-MI15  CONSOLE NO.01  03/11/72 13:03:52\r\n?38-24+47\r\n61 \r\n"
                  "AU REVOIR  13:03:52\r\n"));                       /* MITRA 15 : INRDP75 p. 189 */
    SESSION("XY", "LI" X, "BO", "AU");                    /* état dormant (M6) ; RETURN */
    VERIFIER(egal("XY\r\n????\r\nLISTE ID :\r\n    1,P,D,SECRET,ETENDU\r\nBO\r\nBONJOUR\r\n"
                  "LSE-MI15  CONSOLE NO.01  03/11/72 13:03:52\r\nAU\r\nAU REVOIR  13:03:52\r\n"));

    /* --- PG1 d'INRDP75, profil T1600 : TERMINE EN LIGNE 007 --------------------- */
    configuration_session.profil = PROFIL_T1600_V4;
    SESSION("BO" X, "1 LIRE A,B" X, "3 AFFICHER A+B" X, "5 AFFICHER A*B^2,A/B" X, "7 TERMINER" X,
            "EX" X, "1" X, "6 2" X, "AU" X);
    VERIFIER(contient("EXECUTER A PARTIR DE 1\r\n\a6 2\r\n8 \r\n24 3 \r\nTERMINE EN LIGNE 007\r\n"));
    configuration_session.profil = PROFIL_T1600_F4;                    /* IMP-0276 */
    SESSION("BO" X, "1 LIRE A,B" X, "3 AFFICHER A+B" X, "5 AFFICHER A*B^2,A/B" X, "7 TERMINER" X,
            "EX" X, "1" X, "6 2" X, "AU" X);
    VERIFIER(contient("EXECUTER A PARTIR DE 1\r\n\a6 2\r\n8 \r\n24 3 \r\nTERMINE EN LIGNE 007\r\n"));
    configuration_session.profil = PROFIL_MITRA15_V8;
    SESSION("BO" X, "1 LIRE A" X, "2 TERMINER" X, "EX" X, "1" X, "5" X, "AU" X);
    VERIFIER(contient("\a5\r\nTERMINE\r\n"));              /* MITRA 15 : sans numéro (PG3) */

    /* --- erreurs de compilation et du dialogue ---------------------------------- */
    SESSION("BO" X, "LIS" X, "10 A_" X, "XY" X, X, "", "BO" X, "LI" X, "" X, "AU" X);
    VERIFIER(contient("LIS\r\nERREUR  C "));
    VERIFIER(contient("XY\r\n????\r\n"));
    VERIFIER(contient("\r\nBO\r\n????\r\nLISTER"));         /* BO en session : sans complétion */
    VERIFIER(!contient("BONJOUR\r\n????"));
    VERIFIER(contient("LISTER A PARTIR DE \r\n????\r\n"));   /* sans numéro (Victor Duruy) */
    configuration_session.profil = PROFIL_T1600_F4;             /* SPHIN 0033, PA1 0019 (IMP-0235) */
    SESSION("BO" X, "BO" X, "BO", "AU" X);
    VERIFIER(contient("VOUS ETES SOUS ID") == 0 && contient("\r\nBO\r\n????\r\nBO\r\n????\r\nAU REVOIR"));
    configuration_session.profil = PROFIL_T1600_V4;
    SESSION("BO" X, "BO" X, "AU" X);
    VERIFIER(contient("\r\nBO\r\n????\r\nAU REVOIR"));
    configuration_session.profil = PROFIL_MITRA15_V8;

    /* --- LISTER, NUMERO, EFFACER (IMP-0234) ------------------------------------- */
    SESSION("BO" X, "30 TERMINER" X, "10 A_1" X, "20 B_2" X, "LI" X, "1" X, "NU" X, "1" X,
            "EF" X, "10 A 20" X, "LI" X, "1" X, "EF" X, "*" X, "LI" X, "1" X, "AU" X);
    VERIFIER(contient("LISTER A PARTIR DE 1\r\n10 A_1\r\n20 B_2\r\n30 TERMINER\r\n"));
    VERIFIER(contient("NUMERO A PARTIR DE 1\r\n      10      20      30\r\n"));
    VERIFIER(contient("EFFACER LIGNES 10 A 20\r\nLISTER A PARTIR DE 1\r\n30 TERMINER\r\nEFFACER"));
    VERIFIER(contient("EFFACER LIGNES *\r\nLISTER A PARTIR DE 1\r\nAU REVOIR"));

    /* --- EXECUTER, ligne d'arrêt, CONTINUER, POURSUIVRE (M1, M5, K8) ------------- */
    SESSION("BO" X, "10 A_1" X, "20 A_2" X, "30 A_3" X, "40 TERMINER" X,
            "EX" X, "99" X, "EX" X, "10,30" X, "?A" X, "CO" X, "CO" X, "AU" X);
    VERIFIER(contient("EXECUTER A PARTIR DE 99\r\n????\r\n"));
    VERIFIER(contient("ARRET EN LIGNE 030\r\n?A\r\n2 \r\nCONTINUER\r\nTERMINE\r\nCONTINUER\r\n????\r\n"));
    SESSION("BO" X, "10 A_1" X, "20 A_2;PAUSE" X, "30 A_3" X, "40 TERMINER" X,
            "EX" X, "10" X, "25 B_1" X, "CO" X, "PO" X, "30" X, "AU" X);
    VERIFIER(contient("PAUSE EN LIGNE 020\r\n"));
    VERIFIER(contient("CONTINUER\r\n????\r\nPOURSUIVRE JUSQU'EN 30\r\n????\r\n"));   /* ligne entrée */

    /* --- REPRENDRE après une erreur dans une procédure (LSE15 §9-2, M2) ---------- */
    SESSION("BO" X, "10 &P()" X, "20 TERMINER" X, "30 PROCEDURE &P()" X, "40 A_1/0" X,
            "50 RETOUR" X, "EX" X, "10" X, "RE" X, "20" X, "AU" X);
    VERIFIER(contient(" EN LIGNE 040\r\nREPRENDRE EN 20\r\nTERMINE\r\n"));   /* sans ligne d'appel */

    /* --- PAS A PAS (M3, IMP-0238) -------------------------------------------------- */
    SESSION("BO" X, "10 A_1" X, "20 A_2" X, "30 TERMINER" X, "PA" X, "EX" X, "10" X,
            "", X, "NO" X, "CO" X, "AU" X);
    VERIFIER(contient("PAS A PAS\r\nEXECUTER A PARTIR DE 10\r\nLIGNE 010\r\n\r\nLIGNE 020\r\n"
                      "NORMAL\r\nCONTINUER\r\nTERMINE\r\n"));
    /* boucle FAIRE d'une ligne : arrêt au retour sur la ligne FAIRE, à chaque itération
       (IMP-0232 ; LSE 8086, 2026-10-30 ; audit du code, AC-03) */
    SESSION("BO" X, "10 FAIRE 10 POUR I_1 JUSQUA 3;X_I" X, "20 TERMINER" X, "PA" X, "EX" X, "10" X,
            X, X, X, X, X, "NO" X, "AU" X);
    VERIFIER(contient("EXECUTER A PARTIR DE 10\r\nLIGNE 010\r\nLIGNE 010\r\nLIGNE 010\r\nLIGNE 010\r\n"
                      "LIGNE 020\r\nTERMINE\r\n"));

    /* --- description d'AFFICHER conservée à travers un appel et une suspension
           (IMP-0221 ; LSE 8086, 2026-10-30 ; audit du code, AC-09) ------------------- */
    SESSION("BO" X, "10 AFFICHER[2F6.2] &F(1),3" X, "20 TERMINER" X,
            "30 PROCEDURE &F(A) LOCAL A;AFFICHER['*'];RESULTAT A" X, "EX" X, "10" X, "AU" X);
    VERIFIER(contient("*     1.00     3.00"));
    SESSION("BO" X, "10 AFFICHER[2F6.2] &F(1),3" X, "20 TERMINER" X, "30 PROCEDURE &F(A) LOCAL A" X,
            "40 RESULTAT A" X, "PA" X, "EX" X, "10" X, X, X, X, "NO" X, "AU" X);
    VERIFIER(contient("LIGNE 040\r\n") && contient("     1.00     3.00"));

    /* --- ESC pendant LIRE : PRET EN LIGNE, CONTINUER bipe (L1) --------------------- */
    SESSION("BO" X, "10 LIRE A,B" X, "20 AFFICHER A,B" X, "30 TERMINER" X, "EX" X, "10" X,
            "1", ESC, "CO" X, "2", "AU" X);
    VERIFIER(contient("\a1\r\nPRET EN LIGNE 010\r\nCONTINUER\r\n\a2\r\n\r\n1 2 \r\nTERMINE"));

    /* --- ligne de bureau en erreur ou interrompue pendant un programme suspendu :
           la pile du programme est rendue intacte (IMP-0233 ; audit du code, AC-02) --- */
    SESSION("BO" X, "10 LIRE A,B" X, "20 AFFICHER A,B" X, "30 TERMINER" X, "EX" X, "10" X,
            "1", ESC, "?A+C" X, "CO" X, "2", "AU" X);
    VERIFIER(contient("CONTINUER\r\n\a2\r\n\r\n1 2 \r\nTERMINE"));
    SESSION("BO" X, "10 LIRE A,B" X, "20 AFFICHER A,B" X, "30 TERMINER" X, "EX" X, "10" X,
            "1", ESC, "LIRE X" X, ESC, "CO" X, "2", "AU" X);
    VERIFIER(contient("CONTINUER\r\n\a2\r\n\r\n1 2 \r\nTERMINE"));
    SESSION("BO" X, "10 X_&F(1)+1" X, "20 AFFICHER X" X, "30 TERMINER" X,
            "40 PROCEDURE &F(A) LOCAL A" X, "50 PAUSE" X, "60 RESULTAT A*10" X,
            "EX" X, "10" X, "?A+C" X, "CO" X, "AU" X);
    VERIFIER(contient("CONTINUER\r\n\r\n11 \r\nTERMINE"));
    SESSION("BO" X, "10 X_&F(1)+1" X, "20 AFFICHER X" X, "30 TERMINER" X,
            "40 PROCEDURE &F(A) LOCAL A" X, "50 PAUSE" X, "60 RESULTAT A*10" X,
            "EX" X, "10" X, "LIRE Y" X, ESC, "CO" X, "AU" X);
    VERIFIER(contient("CONTINUER\r\n\r\n11 \r\nTERMINE"));

    /* --- LIRE en machine de bureau : état de lecture vierge, celui du programme
           rétabli (IMP-0222, IMP-0233, IMP-0237 ; audit du code, AC-13, AC-14) --- */
    SESSION("BO" X, "10 TABLEAU T[3]" X, "20 LIRE T" X, "30 AFFICHER T" X, "40 TERMINER" X,
            "EX" X, "10" X, "1 2", ESC, "LIRE X" X, "9", "CO" X, "3", "?X" X, "AU" X);
    VERIFIER(contient("CONTINUER\r\n\a3\r\n") && contient("1 \r\n2 \r\n3 \r\n") && contient("?X\r\n9 "));
    SESSION("BO" X, "10 LIRE A" X, "20 TERMINER" X, "EX" X, "10" X, "1 2", "LIRE X" X, "7",
            "?X" X, "AU" X);
    VERIFIER(contient("?X\r\n7 ") && strstr(bouchon_affichage, "C 07") == NULL);

    /* --- ESC au début d'une ligne ; ESC dans le moniteur --------------------------- */
    {
        static const char *const r_[] = { "BO" X, "10 A_1" X, "20 A_2" X, "30 TERMINER" X,
                                           "EX" X, "10" X, ESC, "AU" X, NULL };
        bouchon_console(r_);
        bouchon_interruption(2);
        moniteur_session();
        VERIFIER(contient("EXECUTER A PARTIR DE 10\r\nPRET EN LIGNE 020\r\nPRET\r\nAU REVOIR"));
    }
    {   /* ESC dans une boucle FAIRE d'une ligne : consultée à chaque itération (IMP-0239 ; AC-03) */
        static const char *const r_[] = { "BO" X, "10 FAIRE 10 POUR I_1 JUSQUA 300;X_I" X,
                                           "20 TERMINER" X, "EX" X, "10" X, "?X" X, "CO" X, "?X" X,
                                           "AU" X, NULL };
        bouchon_console(r_);
        bouchon_interruption(3);                 /* début, puis deux retours sur la ligne 10 */
        moniteur_session();
        VERIFIER(contient("PRET EN LIGNE 010\r\n?X\r\n2 \r\nCONTINUER\r\nTERMINE\r\n?X\r\n300 \r\n"));
    }
    {   /* ESC dans une attente FAIRE sans corps, comme SINUS 105 (IMP-0239, IMP-0277) */
        static const char *const r_[] = { "BO" X, "10 FAIRE 10 TANT QUE 1=1" X, "20 TERMINER" X,
                                           "EX" X, "10" X, "AU" X, NULL };
        configuration_session.profil = PROFIL_T1600_F4;
        bouchon_console(r_);
        bouchon_interruption(3);
        moniteur_session();
        VERIFIER(contient("EXECUTER A PARTIR DE 10\r\nPRET EN LIGNE 010\r\n"));
        configuration_session.profil = PROFIL_MITRA15_V8;
    }

    /* --- correction « \ », ABREGER (IMP-0230, IMP-0231) ----------------------------- */
    SESSION("BO" X, "10 A_1\\2" X, "LI" X, "1" X, "AB" X, "LI" X, "1" X, "IN" X, "AU" X);
    VERIFIER(contient("LISTER A PARTIR DE 1\r\n10 A_2\r\n"));
    VERIFIER(contient("ABREGER\r\nLI 1\r\n10 A_2\r\nIN\r\n"));

    /* --- PRENDRE ETAT CONSOLE, commandes différées (IMP-0236) ----------------------- */
    SESSION("BO" X, "PR" X, "1" X, "PR" X, "2" X, "AP" X, "TOTO" X, "AU" X);
    VERIFIER(contient("PRENDRE ETAT CONSOLE 1\r\nPRENDRE ETAT CONSOLE 2\r\n????\r\n"));
    VERIFIER(contient("APPELER TOTO\r\nERREUR F 05\r\n"));

    /* --- fichiers-programmes (étape 7b) : RANGER, APPELER, MODIFIER, SUPPRIMER,
       EXECUTER ; profil MITRA 15 ------------------------------------------------ */
    SESSION("BO" X, ID1, "1 X_5" X, "2 TERMINER" X, "RA" X, "MOD" X, "RA" X, "MOD" X,
            "EF" X, "*" X, "10 EXECUTER 'MOD'" X, "20 TERMINER" X, "EX" X, "10" X, "?X" X,
            "AP" X, "MOD" X, "LI" X, "1" X, "1 X_7" X, "MO" X, "MOD" X, "AP" X, "MOD" X, "LI" X, "1" X,
            "SU" X, "MOD" X, "SU" X, "MOD" X, "EXECUTER 'MOD'" X, "AU" X);
    VERIFIER(contient("RANGER MOD\r\nRANGER MOD\r\nERREUR F 03\r\n"));
    VERIFIER(contient("EXECUTER A PARTIR DE 10\r\nTERMINE\r\n?X\r\n5 \r\n"));
    VERIFIER(contient("APPELER MOD\r\nLISTER A PARTIR DE 1\r\n1 X_5\r\n2 TERMINER\r\n"));
    VERIFIER(contient("MODIFIER MOD\r\nAPPELER MOD\r\nLISTER A PARTIR DE 1\r\n1 X_7\r\n2 TERMINER\r\n"));
    VERIFIER(contient("SUPPRIMER MOD\r\nSUPPRIMER MOD\r\nERREUR F 05\r\n"));
    VERIFIER(contient("EXECUTER 'MOD'\r\nERREUR  C 58\r\n"));       /* point B6 */

    /* --- CATALOGUER, purge des temporaires à AU REVOIR (étape 7c) ---------------- */
    plateforme_espace_vider(ESPACE_DONNEES);
    SESSION("BO" X, ID1, "1 A_3;GARER A,1,'TT';GARER A,1,'T2'" X, "2 TERMINER" X, "EX" X, "1" X,
            "CA" X, "TT,PP" X, "CA" X, "TT,PP" X, "CA" X, "T2,PP" X, "AU" X);
    VERIFIER(contient("CATALOGUER TT,PP\r\nCATALOGUER TT,PP\r\nERREUR F 04\r\n"));
    VERIFIER(contient("CATALOGUER T2,PP\r\nERREUR F 04\r\n"));
    VERIFIER(plateforme_fichier_existe(ESPACE_DONNEES, "PP"));
    VERIFIER(!plateforme_fichier_existe(ESPACE_TEMPORAIRES, "T2"));      /* AU REVOIR */

    /* --- étape 7d : initialisateur (MEMENTO82 §3.6, IMP-0257) ---------------------- */
    SESSION("CR" X, "25402,P,D,!" X, "CR" X, "31046,P,S,>,X" X, "CR" X, "7425,D,30" X,
            "CR" X, "1,P" X, "CR" X, "500,D,P" X, "LI" X, "SU" X, "7425" X, "SU" X, "7425" X,
            "BO" X, "AU" X);
    VERIFIER(contient("CREATION ID : 25402,P,D,!\r\nCREATION ID : 31046,P,S,>,X\r\n"));
    VERIFIER(contient("CREATION ID : 1,P\r\n????\r\nCREATION ID : 500,D,P\r\n????\r\n"));
    VERIFIER(contient("LISTE ID :\r\n    1,P,D,SECRET,ETENDU\r\n25402,P,D,PROTEGE\r\n"
                      "31046,P,SECRET,PRIVILEGIE,ETENDU\r\n 7425,D,30\r\n"));
    VERIFIER(contient("SUPPRESSION ID : 7425\r\nSUPPRESSION ID : 7425\r\nERREUR F 10\r\n"));

    /* --- IDENTIFICATION : sans écho, ILLEGALE, espace « x » (LSE15 §9-3) ------------ */
    SESSION("BO" X, "ID" X, "99" X, "ID" X, "1" X, "350" X, "ID" X, "1", "", "AU" X);
    VERIFIER(contient("IDENTIFICATION \r\n????\r\n"));            /* 99 : ni écho ni identification */
    VERIFIER(contient("IDENTIFICATION \r\nNOMBRE DE SECTEURS DISPONIBLES:0\r\n"
                      "ESPACE TEMPORAIRE DESIRE:350\r\n"));            /* LAFOND83 p. 105, 110 */
    VERIFIER(contient("IDENTIFICATION \r\nNOMBRE DE SECTEURS DISPONIBLES:0\r\n"
                      "ESPACE TEMPORAIRE DESIRE:\r\n"));               /* RETURN */

    /* --- droits : P, D, propriété, protection « ! » (IMP-0255) ---------------------- */
    SESSION("BO" X, "1 A_1;GARER A,1,'#PERM'" X, "2 TERMINER" X, "RA" X, "P1" X, "EX" X, "1" X,
            "ID" X, "25402" X, "RA" X, "Q2" X, "EX" X, "1" X,
            ID1, "AP" X, "Q2" X, "SU" X, "Q2" X, "SU" X, "PERM" X, "1 CHARGER A,1,'#PERM'" X, "EX" X, "1" X,
            "AU" X);
    VERIFIER(contient("RANGER P1\r\nERREUR F 07\r\nEXECUTER A PARTIR DE 1\r\nERREUR F 08 EN LIGNE 001\r\n"));
    VERIFIER(contient("RANGER Q2\r\nEXECUTER A PARTIR DE 1\r\nTERMINE\r\n"));
    VERIFIER(contient("APPELER Q2\r\nERREUR F 06\r\nSUPPRIMER Q2\r\nERREUR F 22\r\n"
                      "SUPPRIMER PERM\r\nERREUR F 22\r\n"));
    VERIFIER(contient("ERREUR (EX.) 54 EN LIGNE 001\r\n"));     /* PERM protégé (« ! ») */
    VERIFIER(plateforme_fichier_existe(ESPACE_PROGRAMMES, "Q2"));

    /* --- programme secret, T1600 : RANGER ω,E (IMP-0255) ---------------------------- */
    configuration_session.profil = PROFIL_T1600_V4;
    SESSION("BO" X, ID1, "1 A_1" X, "2 TERMINER" X, "RA" X, "SEC,E" X, "ID" X, "25402" X,
            "RA" X, "SEC2,E" X, "AP" X, "SEC" X, "LI" X, "1" X, "3 B_2" X, "EF" X, "1" X,
            "RA" X, "SEC3" X, "EX" X, "1" X, "ID" X, "0" X, "LI" X, "1" X, "EF" X, "*" X, "AU" X);
    VERIFIER(contient("RANGER SEC2,E\r\nERREUR F 11\r\n"));
    VERIFIER(contient("APPELER SEC\r\nLISTER A PARTIR DE 1\r\nERREUR F 06\r\n"));
    VERIFIER(contient("3 B_2\r\nERREUR F 06\r\nEFFACER LIGNES 1\r\nERREUR F 06\r\n"
                      "RANGER SEC3\r\nERREUR F 06\r\nEXECUTER A PARTIR DE 1\r\nTERMINE EN LIGNE 002\r\n"
                      "IDENTIFICATION \r\nLISTER A PARTIR DE 1\r\nERREUR F 06\r\n"));   /* reste secret */

    /* --- TABLE DES FICHIERS et UTILISATION DISQUE, T1600 (INRDP75 p. 133-134 ;
       LAFOND83 p. 17, 110 : « A PARTIR DE XXXX[,P|D|T] ») -------------------------- */
    SESSION("BO" X, ID1, "1 A_1;GARER A,1,'TT'" X, "2 TERMINER" X, "EX" X, "1" X, "TA" X, "*" X,
            "TA" X, "SEC" X, "UT" X, "*" X, "UT" X, "PERM,D" X, "AU" X);
    VERIFIER(contient("TABLE DES FICHIERS A PARTIR DE *\r\n\r\nESPACE TEMPORAIRE( 200 SECT.) OCCUPE A   1 %\r\n\r\n"
                      " NOM       TYPE        LONGUEUR MOTS   PROTECTION\r\n"
                      "TT        TEMPORAIRE         256\r\n"
                      "PP         DONNEE            256\r\n"
                      "SEC       PROGRAMME           17             E\r\n"));
    VERIFIER(contient("TABLE DES FICHIERS A PARTIR DE SEC\r\n\r\nESPACE TEMPORAIRE( 200 SECT.) OCCUPE A   1 %\r\n\r\n"
                      " NOM       TYPE        LONGUEUR MOTS   PROTECTION\r\n"
                      "SEC       PROGRAMME           17             E\r\nUTILISATION"));   /* P par défaut */
    VERIFIER(contient("UTILISATION DISQUE A PARTIR DE *\r\n\r\nESPACE PERMANENT ("));
    VERIFIER(contient(" FICHIERS-PROGRAMMES\r\n\r\n NOM     NO COMPTE     LONGUEUR MOTS   PROTECTION\r\n"
                      "Q2            02              23             L\r\n"
                      "SEC           01              17             E\r\n"));      /* ordre alphabétique */
    VERIFIER(contient(" CONSOLE 1\r\n\r\n NOM                   LONGUEUR MOTS\r\nTT                           256\r\n"));
    VERIFIER(contient("UTILISATION DISQUE A PARTIR DE PERM,D\r\n\r\nESPACE PERMANENT ("));
    VERIFIER(contient("\r\n FICHIERS-DONNEES PERMANENTS\r\n\r\n NOM     NO COMPTE     LONGUEUR MOTS   PROTECTION\r\n"
                      "PERM          02             256             L\r\n"
                      "PP            01             256\r\n\r\nAU REVOIR"));      /* le seul type D */
    SESSION("BO" X, ID1, "DU" X, "AU" X);                        /* DUPLIQUER : LSE F4 seul */
    VERIFIER(!contient("DUPLIQUER"));
    configuration_session.profil = PROFIL_T1600_F4;
    SESSION("BO" X, ID1, "DU" X, "$SEC,P" X, "AU" X);
    VERIFIER(contient("DUPLIQUER $SEC,P\r\nERREUR F 29\r\n"));     /* aucune unité */
    configuration_session.profil = PROFIL_MITRA15_V8;

    /* --- MITRA 15 : « A » pour tout, date sous la commande, UTILISATION sous « > » -- */
    SESSION("BO" X, ID1, "TA" X, "A" X, "TA" X, "*" X, "UT" X, "A" X, "ID" X, "31046" X, "",
            "UT" X, "A" X, "UT" X, "PERM,D" X, "UT" X, "Q" X, "UT" X, "1A" X, "AU" X);
    VERIFIER(contient("TABLE DES FICHIERS A PARTIR DE A\r\n03/11/72 13:03:52\r\n\r\nFICHIERS-PROGRAMME\r\n"
                      "******************\r\n\r\n NOM NO.COMPTE  DATE  NB.MOTS\r\n\r\n"
                      "SEC     01    03/11/72  17\r\n\r\n\r\nFICHIERS-DONNEE PERMANENTS\r\n"));
    VERIFIER(contient("FICHIERS-DONNEE TEMPORAIRES\r\n***************************\r\n\r\n"
                      " NOM CONSOLE NB.SECTEURS\r\n\r\nTABLE DES FICHIERS A PARTIR DE *\r\n????\r\n"
                      "UTILISATION DISQUE A PARTIR DE A\r\n????\r\n"));
    VERIFIER(contient("UTILISATION DISQUE A PARTIR DE A\r\n03/11/72 13:03:52\r\n"));
    VERIFIER(contient("Q2      02    03/11/72  23\r\nSEC     01    03/11/72  17\r\n"));
    VERIFIER(contient("ESPACE TEMPORAIRE ALLOUE\r\n************************\r\n\r\n"
                      "CONSOLE NB.SECTEURS\r\n\r\n  01         200\r\n"));
    VERIFIER(contient("UTILISATION DISQUE A PARTIR DE PERM,D\r\n03/11/72 13:03:52\r\n\r\n"
                      "FICHIERS-DONNEE PERMANENTS\r\n**************************\r\n\r\n"
                      " NOM NO.COMPTE  DATE  NB.SECTEURS\r\n\r\n"
                      "PERM     02   03/11/72     2\r\nPP       01   03/11/72     2\r\nUTILISATION"));
    VERIFIER(contient("UTILISATION DISQUE A PARTIR DE Q\r\n03/11/72 13:03:52\r\n\r\nFICHIERS-PROGRAMME\r\n"
                      "******************\r\n\r\n NOM NO.COMPTE  DATE  NB.MOTS\r\n\r\n"
                      "Q2      02    03/11/72  23\r\nSEC     01    03/11/72  17\r\nUTILISATION"));
    VERIFIER(contient("UTILISATION DISQUE A PARTIR DE 1A\r\n????\r\n"));

    /* --- ESC pendant TABLE DES FICHIERS (IMP-0239) ---------------------------------- */
    {
        static const char *const r_[] = { "BO" X, ID1, "TA" X, "A" X, "AU" X, NULL };
        bouchon_console(r_);
        bouchon_interruption(2);
        moniteur_session();
        VERIFIER(contient("SEC     01    03/11/72  17\r\n"));
        VERIFIER(contient(" NOM NO.COMPTE  DATE  NB.SECTEURS\r\n\r\nPRET\r\nAU REVOIR"));
    }

    /* --- DUPLIQUER (LSE F4, LAFOND83 p. 109, IMP-0258) ------------------------------ */
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBUNI"));
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_espace_vider(ESPACE_DONNEES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
    configuration_session.profil = PROFIL_T1600_F4;
    SESSION("BO" X, ID1, "1 X_9" X, "2 TERMINER" X, "RA" X, "DUP" X, "RA" X, "DUQ" X,
            "1 A_1;GARER A,1,'#DD'" X, "EX" X, "1" X, "AU" X);
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBMON"));
    plateforme_unite_designer("tests:BIBUNI");
    SESSION("BO" X, ID1, "DU" X, "$DUP,P" X, "DU" X, "$DUP,P" X, "DU" X, "$*,D" X, "DU" X, "$XYZ,P" X,
            "DU" X, "$DUQ" X, "DU" X, "Q2,P" X, "DU" X, "SE*" X, "SU" X, "SEC,P" X, "DU" X, "$SEC" X,
            "TA" X, "SEC" X, "DU" X, "Z*" X, "AP" X, "DUP" X, "LI" X, "1" X, "AU" X);
    VERIFIER(contient("DUPLIQUER $DUP,P\r\nDUPLIQUER $DUP,P\r\nERREUR F 03\r\n"));
    VERIFIER(contient("DUPLIQUER $*,D\r\nDUPLIQUER $XYZ,P\r\nERREUR F 05\r\nDUPLIQUER $DUQ\r\n"
                      "DUPLIQUER Q2,P\r\nERREUR F 06\r\nDUPLIQUER SE*\r\nSUPPRIMER SEC,P\r\nDUPLIQUER $SEC\r\n"));
    VERIFIER(contient("SEC       PROGRAMME           17             E\r\n"));   /* protection gardée (D4) */
    VERIFIER(contient("DUPLIQUER Z*\r\nERREUR F 05\r\n"));
    VERIFIER(contient("LISTER A PARTIR DE 1\r\n1 X_9\r\n2 TERMINER\r\n"));
    VERIFIER(plateforme_fichier_existe(ESPACE_DONNEES, "DD") && plateforme_fichier_existe(ESPACE_PROGRAMMES, "DUQ"));
    VERIFIER(plateforme_fichier_existe(ESPACE_UNITE_PROGRAMMES, "SEC")
             && !plateforme_fichier_existe(ESPACE_UNITE_PROGRAMMES, "Q2"));

    /* --- « $ » : fichier sur l'unité amovible (LSE F4, IMP-0278) ---------------------- */
    /* RANGER, APPELER, MODIFIER : un même nom sur l'unité et dans la bibliothèque */
    SESSION("BO" X, ID1, "1 X_7" X, "2 TERMINER" X, "RA" X, "$PUN" X, "RA" X, "$PUN" X, "RA" X, "PUN" X,
            "EF" X, "*" X, "AP" X, "$PUN" X, "LI" X, "1" X, "3 Y_1" X, "MO" X, "$PUN" X, "AP" X, "$ABS" X,
            "AP" X, "PUN" X, "LI" X, "1" X, "AP" X, "$PUN" X, "LI" X, "1" X, "AU" X);
    VERIFIER(contient("RANGER $PUN\r\nRANGER $PUN\r\nERREUR F 03\r\nRANGER PUN\r\n"));
    VERIFIER(contient("APPELER $PUN\r\nLISTER A PARTIR DE 1\r\n1 X_7\r\n2 TERMINER\r\n3 Y_1\r\n"
                      "MODIFIER $PUN\r\nAPPELER $ABS\r\nERREUR F 05\r\n"));
    VERIFIER(contient("APPELER PUN\r\nLISTER A PARTIR DE 1\r\n1 X_7\r\n2 TERMINER\r\n"
                      "APPELER $PUN\r\nLISTER A PARTIR DE 1\r\n1 X_7\r\n2 TERMINER\r\n3 Y_1\r\n"));
    VERIFIER(plateforme_fichier_existe(ESPACE_UNITE_PROGRAMMES, "PUN")
             && plateforme_fichier_existe(ESPACE_PROGRAMMES, "PUN"));
    /* CATALOGUER, TABLE, UTILISATION, DECODER, ENCODER, SUPPRIMER */
    SESSION("BO" X, ID1, "1 A_1;GARER A,1,'TT2'" X, "2 TERMINER" X, "EX" X, "1" X,
            "CA" X, "TT2,$DON" X, "CA" X, "$TT2,X" X, "TA" X, "$*" X, "UT" X, "$DON,D" X, "TA" X, "$PUN,T" X,
            "DE" X, "$TEX" X, "EF" X, "*" X, "EN" X, "$TEX" X, "LI" X, "1" X,
            "SU" X, "$PUN,T" X, "SU" X, "$*" X, "SU" X, "$PUN,P" X, "SU" X, "$PUN,P" X, "SU" X, "$DON,D" X,
            "AU" X);
    VERIFIER(contient("CATALOGUER TT2,$DON\r\nCATALOGUER $TT2,X\r\n????\r\n"));
    VERIFIER(contient("TABLE DES FICHIERS A PARTIR DE $*\r\n\r\n NOM       TYPE        LONGUEUR MOTS   PROTECTION\r\n"));
    VERIFIER(contient("DON        DONNEE            256\r\n"));
    VERIFIER(contient("UTILISATION DISQUE A PARTIR DE $DON,D\r\n\r\n FICHIERS-DONNEES PERMANENTS\r\n\r\n"
                      " NOM     NO COMPTE     LONGUEUR MOTS   PROTECTION\r\nDON           01             256\r\n"));
    VERIFIER(contient("TABLE DES FICHIERS A PARTIR DE $PUN,T\r\n????\r\nDECODER $TEX\r\n"));
    VERIFIER(contient("LISTER A PARTIR DE 1\r\n1 A_1;GARER A,1,'TT2'\r\n2 TERMINER\r\n"));
    VERIFIER(contient("SUPPRIMER $PUN,T\r\n????\r\nSUPPRIMER $*\r\n????\r\n"
                      "SUPPRIMER $PUN,P\r\nSUPPRIMER $PUN,P\r\nERREUR F 05\r\nSUPPRIMER $DON,D\r\nAU REVOIR"));
    VERIFIER(plateforme_fichier_existe(ESPACE_UNITE_DONNEES, "TEX")
             && !plateforme_fichier_existe(ESPACE_TEMPORAIRES, "TEX")
             && !plateforme_fichier_existe(ESPACE_UNITE_DONNEES, "DON")
             && !plateforme_fichier_existe(ESPACE_UNITE_PROGRAMMES, "PUN")
             && plateforme_fichier_existe(ESPACE_PROGRAMMES, "PUN"));
    /* droits : les attributs et la propriété valent sur l'unité */
    SESSION("BO" X, "1 A_1" X, "RA" X, "$ZED" X, "DE" X, "$ZED" X, "AU" X);
    VERIFIER(contient("RANGER $ZED\r\nERREUR F 07\r\nDECODER $ZED\r\nERREUR F 08\r\n"));
    SESSION("BO" X, ID1, "1 A_1" X, "RA" X, "$PRO,L" X, "ID" X, "25402" X, "AP" X, "$PRO" X, "MO" X, "$PRO" X,
            "SU" X, "$PRO,P" X, "RA" X, "$PRP" X, "AU" X);
    VERIFIER(contient("APPELER $PRO\r\nERREUR F 06\r\nMODIFIER $PRO\r\nERREUR F 22\r\n"
                      "SUPPRIMER $PRO,P\r\nERREUR F 22\r\nRANGER $PRP\r\nAU REVOIR"));
    /* unité absente */
    plateforme_unite_designer(NULL);
    SESSION("BO" X, ID1, "1 A_1" X, "RA" X, "$A" X, "MO" X, "$A" X, "AP" X, "$A" X, "SU" X, "$A,P" X,
            "CA" X, "T,$A" X, "DE" X, "$A" X, "EN" X, "$A" X, "TA" X, "$*" X, "UT" X, "$*" X, "AU" X);
    VERIFIER(contient("RANGER $A\r\nERREUR F 29\r\nMODIFIER $A\r\nERREUR F 29\r\nAPPELER $A\r\nERREUR F 29\r\n"
                      "SUPPRIMER $A,P\r\nERREUR F 29\r\nCATALOGUER T,$A\r\nERREUR F 29\r\n"
                      "DECODER $A\r\nERREUR F 29\r\nENCODER $A\r\nERREUR F 29\r\n"));
    VERIFIER(contient("TABLE DES FICHIERS A PARTIR DE $*\r\nERREUR F 29\r\n"
                      "UTILISATION DISQUE A PARTIR DE $*\r\nERREUR F 29\r\n"));
    /* autres profils : « $ » refusé */
    plateforme_unite_designer("tests:BIBUNI");
    configuration_session.profil = PROFIL_T1600_V4;
    SESSION("BO" X, ID1, "1 A_1" X, "RA" X, "$PRP" X, "AP" X, "$PRP" X, "TA" X, "$*" X, "AU" X);
    VERIFIER(contient("RANGER $PRP\r\n????\r\nAPPELER $PRP\r\n????\r\nTABLE DES FICHIERS A PARTIR DE $*\r\n????\r\n"));
    configuration_session.profil = PROFIL_MITRA15_V8;
    SESSION("BO" X, ID1, "1 A_1" X, "RA" X, "$PRP" X, "AP" X, "$PRP" X, "AU" X);
    VERIFIER(contient("RANGER $PRP\r\n????\r\nAPPELER $PRP\r\n????\r\n"));
    plateforme_unite_designer(NULL);

    /* --- DECODER, ENCODER (étape 7e1, IMP-0260, IMP-0261) --------------------------- */
    SESSION("BO" X, "1 CHAINE C;C_'5 X_1'!EQC(19)!'6 Y_'!EQC(19)!'7 Z_3'!EQC(0);GARER C,1,'EE'" X,
            "2 TERMINER" X, "EX" X, "1" X, "EN" X, "EE" X, "LI" X, "1" X,
            "10 A_1" X, "20 B_2" X, "DE" X, "XX" X, "EF" X, "*" X, "EN" X, "XX" X, "LI" X, "1" X,
            "EN" X, "XX,20" X, "EN" X, "ZZ" X, "AU" X);
    VERIFIER(contient("ENCODER EE\r\n6 Y_\r\nERREUR "));        /* la première erreur arrête */
    VERIFIER(contient("LISTER A PARTIR DE 1\r\n5 X_1\r\n10 A_1"));  /* MITRA 15 : remplacé */
    VERIFIER(contient("ENCODER XX\r\nLISTER A PARTIR DE 1\r\n5 X_1\r\n10 A_1\r\n20 B_2\r\nENCODER"));
    VERIFIER(contient("ENCODER ZZ\r\nERREUR F 05\r\n"));
    configuration_session.profil = PROFIL_T1600_V4;
    SESSION("BO" X, "1 CHAINE C;C_'5 X_1'!EQC(19)!'7 Z_3'!EQC(19)!'X'!EQC(0);GARER C,1,'EE'" X,
            "2 TERMINER" X, "EX" X, "1" X, "EN" X, "EE" X, "LI" X, "1" X, "AU" X);
    VERIFIER(contient("ENCODER EE\r\n????\r\nLISTER A PARTIR DE 1\r\n1 CHAINE C;"));   /* T1600 : ajout */
    VERIFIER(contient("\r\n2 TERMINER\r\n5 X_1\r\n7 Z_3\r\nAU REVOIR"));
    configuration_session.profil = PROFIL_MITRA15_V8;
    SESSION("BO" X, "1 CHAINE C;C_'5 X_1'!EQC(19)!'7 Z_3'!EQC(0);GARER C,1,'EE'" X,
            "2 TERMINER" X, "EX" X, "1" X, "EN" X, "EE,6" X, "LI" X, "1" X, "AU" X);
    VERIFIER(contient("ENCODER EE,6\r\nLISTER A PARTIR DE 1\r\n1 CHAINE C;"));    /* avec n1 : ajout */
    VERIFIER(contient("\r\n2 TERMINER\r\n7 Z_3\r\nAU REVOIR"));
    /* état « aucun » seulement si une ligne est entrée (LSE15 §9-2 ; audit du code, §9 point 13) */
    SESSION("BO" X, "1 CHAINE C;C_'5 X_1'!EQC(19)!'7 Z_3'!EQC(0);GARER C,1,'EE'" X,
            "2 PAUSE" X, "3 TERMINER" X, "EX" X, "1" X, "EN" X, "EE,90" X, "CO" X, "AU" X);
    VERIFIER(contient("ENCODER EE,90\r\nCONTINUER\r\nTERMINE"));                 /* rien d'entré */
    SESSION("BO" X, "1 CHAINE C;C_'5 X_1'!EQC(19)!'7 Z_3'!EQC(0);GARER C,1,'EE'" X,
            "2 PAUSE" X, "3 TERMINER" X, "EX" X, "1" X, "EN" X, "EE,6" X, "CO" X, "AU" X);
    VERIFIER(contient("ENCODER EE,6\r\nCONTINUER\r\n????"));                     /* ligne 7 entrée */

    /* --- PERFORER, RUBAN, X-ON, SILENCE (étape 7e2, IMP-0262 à IMP-0265) ------------ */
    ruban_ecrire("");
    SESSION("BO" X, "10 A_1" X, "20 B_2" X, "PE" X, "1" X, "EF" X, "*" X, "RU" X, "LI" X, "1" X,
            "" ESC, "LI" X, "10" X, "AU" X);
    VERIFIER(contient("PERFORER A PARTIR DE 1\r\n10 A_1\r\n20 B_2\r\nEFFACER"));
    VERIFIER(ruban_vaut("10 A_1\r20 B_2\r"));
    VERIFIER(contient("RUBAN\r\nSTER A PARTIR DE \r\n10 A_1\r\n20 B_2\r\n"));   /* rien de lu ni de tapé */
    VERIFIER(contient("PRET\r\nLISTER A PARTIR DE 10\r\n10 A_1\r\n"));           /* ESC : fin de SILENCE */
    configuration_session.profil = PROFIL_T1600_V4;
    SESSION("BO" X, XON, "LI" X, "1" X, "AU" X);
    VERIFIER(contient("BONJOUR  03/11/72 13:03:52\r\n10 A_1\r\n20 B_2\r\nLISTER A PARTIR DE 1\r\n10 A_1\r\n20 B_2\r\n"));
    configuration_session.profil = PROFIL_MITRA15_V8;

    ruban_ecrire("EX\x13" "1\x13" "5\r" "30 C_3\r\x1B" "40 D_4\r\0");
    SESSION("BO" X, "1 LIRE A" X, "2 AFFICHER A*2" X, "3 TERMINER" X, "RU" X, "LI" X, "30" X,
            "RU" X, "LI" X, "30" X, "AU" X);
    VERIFIER(contient("RUBAN\r\nECUTER A PARTIR DE \r\n\a\r\n10 \r\nTERMINE\r\nPRET\r\n"));   /* EX 1 et LIRE lus */
    VERIFIER(contient("LISTER A PARTIR DE 30\r\n30 C_3\r\nRUBAN\r\n"));   /* ESC perforé : arrêt */
    VERIFIER(contient("RUBAN\r\nSTER A PARTIR DE \r\n30 C_3\r\n40 D_4\r\n"));   /* reprise */

    SESSION("BO" X, "SI" X, "?1+1" X, "" ESC, "?2+2" X, "AU" X);
    VERIFIER(contient("SILENCE\r\n\r\n2 \r\nPRET\r\n?2+2\r\n4 \r\n"));   /* ?1+1 sans écho (IMP-0265) */
    plateforme_fichier_supprimer(ESPACE_RUBAN, "RUBAN");

    /* --- perforateur piloté par programme (INRDP75 annexe V, IMP-0274, IMP-0275) ---- */
    ruban_ecrire("");                                     /* ni DC2, ni DC4 ; X-OFF perforé */
    SESSION("BO" X, "10 AFFICHER[U]'A'!EQC(18)!'B'!EQC(19)!EQC(20)!'C'" X, "20 TERMINER" X,
            "EX" X, "10" X, "AU" X);
    VERIFIER(contient("\r\nABC\r\nTERMINE\r\n"));
    VERIFIER(ruban_vaut("B\x13"));
    configuration_session.profil = PROFIL_T1600_V4;       /* les deux profils ; ajout à la fin */
    SESSION("BO" X, "10 AFFICHER[U]'A'!EQC(18)!'B'!EQC(20)!'C'" X, "20 TERMINER" X, "EX" X, "10" X, "AU" X);
    VERIFIER(contient("\r\nABC\r\nTERMINE EN LIGNE 020\r\n"));
    VERIFIER(ruban_vaut("B\x13" "B"));
    configuration_session.profil = PROFIL_MITRA15_V8;
    ruban_ecrire("X\r");                                  /* NUL, CR et LF de / perforés ;
                                                             Golden Gate range CR en LF dans un
                                                             fichier TXT et relit tout LF en CR */
    SESSION("BO" X, "10 AFFICHER[U,/,U]EQC(18)!'A'!EQC(0),'B'" X, "20 AFFICHER[U]EQC(20)!'C'" X,
            "30 TERMINER" X, "EX" X, "10" X, "AU" X);
    VERIFIER(ruban_vaut_n("X\rA\0\r\rB", 7));
    ruban_ecrire("");                                     /* frappe et messages non perforés */
    SESSION("BO" X, "10 AFFICHER[U]EQC(18)!'A'" X, "20 LIRE B" X, "30 AFFICHER[U]'C'" X, "40 TERMINER" X,
            "EX" X, "10" X, "5" X, "?'D'" X, "AU" X);       /* la lecture n'arrête pas, le retour si */
    VERIFIER(ruban_vaut("AC"));
    ruban_ecrire("");
    SESSION("BO" X, "10 AFFICHER[U]EQC(18)!'A'" X, "20 X_1/0" X, "30 TERMINER" X, "EX" X, "10" X,
            "?'B'" X, "AU" X);                              /* arrêté après une erreur */
    VERIFIER(ruban_vaut("A"));
    ruban_ecrire("");
    SESSION("BO" X, "?EQC(18)" X, "?'A'" X, "AU" X);        /* après une ligne de bureau */
    VERIFIER(!plateforme_fichier_existe(ESPACE_RUBAN, "RUBAN"));
    plateforme_ruban_designer("tests:ABSENT:RUBAN");        /* écriture impossible : EX 52 */
    SESSION("BO" X, "10 AFFICHER[U]EQC(18)!'A'" X, "20 TERMINER" X, "EX" X, "10" X, "AU" X);
    VERIFIER(contient("(EX.) 52 EN LIGNE 010"));
    plateforme_ruban_designer(NULL);
    plateforme_fichier_supprimer(ESPACE_RUBAN, "RUBAN");

    /* --- éditeur de ligne (LAFOND83 p. 133-134, MEMENTO82 §4, IMP-0267) ------------- */
    SESSION("BO" X, "/A/B" X, "10 AFICHER A" X, "/F/FF" X, "10 X_SCH(A,1,3)!SCH(B,1)" X, "X2/)/,3)" X,
            "11 X_SCH(A,1)!SCH(B,1)" X, "2X/)/,3)" X, "1 ALLER EN AYAYA" X, "LI" X, "1,1" X,
            "3X2/A/B" X, "/ZZ/Y" X, "//B" X, "LI" X, "10,10" X, "/10/20" X, "LI" X, "1" X, "AU" X);
    VERIFIER(contient("BONJOUR\r\nLSE-MI15  CONSOLE NO.01  03/11/72 13:03:52\r\n/A/B\r\n????\r\n"));
    VERIFIER(contient("/F/FF\r\n10 AFFICHER A\r\n"));
    VERIFIER(contient("X2/)/,3)\r\n10 X_SCH(A,1,3)!SCH(B,1,3)\r\n"));
    VERIFIER(contient("2X/)/,3)\r\n11 X_SCH(A,1,3)!SCH(B,1,3)\r\n"));
    VERIFIER(contient("3X2/A/B\r\n1 ALLER EN BYBYB\r\n/ZZ/Y\r\n//B\r\n????\r\n"));
    VERIFIER(contient("/10/20\r\n20 X_SCH(A,1,3)!SCH(B,1,3)\r\nLISTER A PARTIR DE 1\r\n1 ALLER EN BYBYB\r\n"
                      "10 X_SCH(A,1,3)!SCH(B,1,3)\r\n11 X_SCH(A,1,3)!SCH(B,1,3)\r\n"
                      "20 X_SCH(A,1,3)!SCH(B,1,3)\r\n"));            /* autre numéro : nouvelle ligne */

    /* --- IDENTIFICATION et temporaires : détruits sous MITRA 15 (LSE15 §9-3),
       conservés sous T1600 (INRDP75 annexe II p. 192, Listage 1 ; IMP-0256) ---------- */
    SESSION("BO" X, "1 A_1;GARER A,1,'TT'" X, "2 TERMINER" X, "EX" X, "1" X, ID1, "CA" X, "TT,TP" X, "AU" X);
    VERIFIER(contient("CATALOGUER TT,TP\r\nERREUR F 05\r\n"));
    configuration_session.profil = PROFIL_T1600_V4;
    SESSION("BO" X, "1 A_1;GARER A,1,'TT'" X, "2 TERMINER" X, "EX" X, "1" X, ID1, "CA" X, "TT,TP" X,
            "SU" X, "TP,D" X, "AU" X);
    VERIFIER(contient("CATALOGUER TT,TP\r\nSUPPRIMER TP,D\r\nAU REVOIR"));
    configuration_session.profil = PROFIL_MITRA15_V8;

    /* --- démarrage de LSE F4 (IMP-0279) ------------------------------------------------ */
    {
        static const InstantHorloge defaut = { 1972, 11, 3, 13, 3, 52 };
        static const InstantHorloge nbor = { 0, 0, 0, 28, 26, 42 };

        moniteur_fixer_demarrage(1);
        configuration_session.profil = PROFIL_T1600_F4;
        SESSION("15/10/80 08:36:17" X, "BO" X, "AU" X);
        VERIFIER(egal("LSE F4\r\n\r\nDATE HEURES MINUTES : 15/10/80 08:36:17\r\nEN LIGNE....\r\n"
                      "BONJOUR 15/10/80 08:36:17\r\n" BANNIERE_F4 BLANCS_F4
                      "AU REVOIR 15/10/80 08:36:17\r\n" BANNIERE_F4 BLANCS_F4));
        /* date non réglée : champ vide ; heures et minutes seules */
        SESSION("00/00/00 00:05" X, "BO" X, "?DAT" X, "AU" X);
        VERIFIER(contient("DATE HEURES MINUTES : 00/00/00 00:05\r\nEN LIGNE....\r\nBONJOUR  00:05:00\r\n"));
        VERIFIER(contient("?DAT\r\n00/00/00 00:05:00"));
        VERIFIER(contient("AU REVOIR  00:05:00\r\n"));
        /* réponses incorrectes : la question est reposée */
        SESSION("XY" X, "32/01/80 10:00:00" X, "15/13/80 10:00" X, "00/10/80 10:00" X, "15/10/80 24:00" X,
                "15/10/80 10:60" X, "15/10/80 10:00:60" X, "15/10/80 10:00:00" X, "BO" X, "AU" X);
        VERIFIER(contient("LSE F4\r\n\r\nDATE HEURES MINUTES : XY\r\nLSE F4\r\n\r\nDATE HEURES MINUTES : 32/01/80"));
        VERIFIER(contient("DATE HEURES MINUTES : 15/10/80 10:00:60\r\nLSE F4\r\n\r\n"
                          "DATE HEURES MINUTES : 15/10/80 10:00:00\r\nEN LIGNE....\r\nBONJOUR 15/10/80 10:00:00\r\n"));
        /* RETURN seul : l'horloge de l'Apple IIGS */
        horloge_fixer(&defaut);
        SESSION("", "BO" X, "AU" X);
        VERIFIER(contient("DATE HEURES MINUTES : \r\nEN LIGNE....\r\nBONJOUR 03/11/72 13:03:52\r\n"));
        /* autres profils : ni question ni « EN LIGNE » */
        configuration_session.profil = PROFIL_T1600_V4;
        SESSION("BO" X, "AU" X);
        VERIFIER(egal("BONJOUR  03/11/72 13:03:52\r\nAU REVOIR  03/11/72 13:03:52\r\n"));
        configuration_session.profil = PROFIL_MITRA15_V8;
        SESSION("BO" X, "AU" X);
        VERIFIER(egal("BONJOUR\r\nLSE-MI15  CONSOLE NO.01  03/11/72 13:03:52\r\nAU REVOIR  13:03:52\r\n"));
        moniteur_fixer_demarrage(0);
        /* heures au-delà de 24 (NBOR) */
        configuration_session.profil = PROFIL_T1600_F4;
        horloge_fixer(&nbor);
        SESSION("BO" X, "AU" X);
        VERIFIER(contient("BONJOUR  28:26:42\r\n"));
        horloge_fixer(&defaut);
        configuration_session.profil = PROFIL_MITRA15_V8;
    }

    /* --- option -ID (Victor Duruy : « VOUS ETES SOUS ID 1 ») ; x sous MITRA 15 seul
       (INRDP75 §2-2, option c) ; IMP-0268 --------------------------------------------- */
    configuration_session.profil = PROFIL_T1600_V4;
    moniteur_fixer_identification(1);
    SESSION("BO" X, "AU" X);                                    /* absent d'INRDP75 */
    VERIFIER(egal("BONJOUR  03/11/72 13:03:52\r\nAU REVOIR  03/11/72 13:03:52\r\n"));
    configuration_session.profil = PROFIL_T1600_F4;
    SESSION("BO" X, "1 A_1" X, "RA" X, "VD1" X, "SU" X, "VD1,P" X, "ID" X, "1" X, "AU" X);
    VERIFIER(contient("BONJOUR 03/11/72 13:03:52\r\n" BANNIERE_F4 "VOUS ETES SOUS ID 1\r\n" BLANCS_F4
                      "1 A_1\r\nRANGER VD1\r\nSUPPRIMER VD1,P\r\n"));
    VERIFIER(contient("IDENTIFICATION \r\nAU REVOIR"));        /* T1600 : pas de question */
    VERIFIER(contient("AU REVOIR 03/11/72 13:03:52\r\n" BANNIERE_F4 "VOUS ETES SOUS ID 1\r\n" BLANCS_F4));
    /* identification de la console, non celle prise en cours de session (IMP-0268) */
    SESSION("BO" X, "ID" X, "25402" X, "AU" X);
    VERIFIER(contient("AU REVOIR 03/11/72 13:03:52\r\n" BANNIERE_F4 "VOUS ETES SOUS ID 1\r\n" BLANCS_F4));
    moniteur_fixer_identification(4321);                        /* non définie : 0 */
    SESSION("BO" X, "AU" X);
    VERIFIER(egal("BONJOUR 03/11/72 13:03:52\r\n" BANNIERE_F4 BLANCS_F4
                  "AU REVOIR 03/11/72 13:03:52\r\n" BANNIERE_F4 BLANCS_F4));
    moniteur_fixer_identification(0);
    SESSION("BO" X, "AU" X);                                    /* -ID 0 : ni ligne ni droit */
    VERIFIER(egal("BONJOUR 03/11/72 13:03:52\r\n" BANNIERE_F4 BLANCS_F4
                  "AU REVOIR 03/11/72 13:03:52\r\n" BANNIERE_F4 BLANCS_F4));
    configuration_session.profil = PROFIL_T1600_V4;             /* T1600 version 4 : sans bannière */
    SESSION("BO" X, "AU" X);
    VERIFIER(egal("BONJOUR  03/11/72 13:03:52\r\nAU REVOIR  03/11/72 13:03:52\r\n"));
    configuration_session.profil = PROFIL_MITRA15_V8;
    moniteur_fixer_identification(1);
    SESSION("BO" X, "AU" X);
    VERIFIER(egal("BONJOUR\r\nLSE-MI15  CONSOLE NO.01  03/11/72 13:03:52\r\nAU REVOIR  13:03:52\r\n"));
    moniteur_fixer_identification(0);

    /* --- tâche unique : une suspension par demande d'entrée (IMP-0269) ---------------- */
    SESSION("BO" X, "10 LIRE A" X, "20 TERMINER" X, "EX" X, "10" X, "5" X, "AU" X);
    VERIFIER_EGAL(TACHE_ABSENTE, taches_etat());
    VERIFIER_EGAL(6ul, taches_swap_outs());            /* 10 LIRE, 20, EX, 10, réponse « 5 », AU */
    VERIFIER_EGAL(taches_swap_outs(), taches_swap_ins());

    VERIF_FIN();
}
