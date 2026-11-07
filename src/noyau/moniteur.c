/* moniteur.c — environnement conversationnel : session, saisie, commandes
   EXI: EXI-0028, EXI-0029, EXI-0051, EXI-0052
   VM: VM-2201
   IV: IV-IF-0001 (compiler), IV-IF-0002 (insérer), IV-IF-0003 (exécuter),
       IV-IF-0004 (décompiler)
   IMP: IMP-0227 (organisation), IMP-0228 (session), IMP-0231 (correction),
        IMP-0232 (commandes d'exécution), IMP-0233 (messages), IMP-0234
        (listage, effacement), IMP-0235 (erreurs du dialogue), IMP-0236
        (commandes différées), IMP-0237 (attente d'entrée), IMP-0238 (X-OFF
        et RETURN), IMP-0239 (interruption) ; Bibliothécaire et fichiers
        IMP-0245, IMP-0246 (RANGER, APPELER, MODIFIER, SUPPRIMER, étape 7b),
        IMP-0278 (« $ »), IMP-0279 (démarrage), IMP-0254 à IMP-0258 (identifications, droits, initialisateur, tables,
        DUPLIQUER, étape 7d ; IMP-0268, identification initiale), IMP-0260 et IMP-0261 (DECODER, ENCODER, étape 7e1),
        IMP-0263 à IMP-0265 (ruban, PERFORER, SILENCE, étape 7e2), IMP-0274
        (perforateur arrêté au retour au moniteur), IMP-0266
        (bannières), IMP-0267 (éditeur de ligne) ; Gestionnaire des tâches
        IMP-0269 (transitions de la tâche aux demandes d'entrée)
   Tests: tests/composants/composants_moniteur.c
   Spécification : chapitre Moniteur et console. Segment "moniteur" (IMP-0227). */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include "moniteur.h"
#include "analyseur.h"
#include "compilateur.h"
#include "programme.h"
#include "interprete.h"
#include "contexte_exec.h"
#include "decompilateur.h"
#include "objets.h"
#include "erreurs.h"
#include "bibliotheque.h"
#include "fichiers.h"
#include "codage.h"
#include "ruban.h"
#include "taches.h"
#include "../interfaces/interface_fichiers.h"
#include "../interfaces/interface_console.h"
#include "../interfaces/interface_horloge.h"
#include "../interfaces/types_communs.h"

segment "moniteur";
#include "../interfaces/profil.h"

/* État du programme (IMP-0232). */
#define PROG_AUCUN     0
#define PROG_SUSPENDU  1
#define PROG_TERMINE   2
#define PROG_ERREUR    3

static PileEvaluation      pile;
static PositionExecution   pos;
static DiagnosticExecution diag;
static unsigned char       etat_programme = PROG_AUCUN;
static unsigned long       compteur_arret = 0;      /* lignes entrées à l'arrêt (IMP-0207) */
static int                 lecture_interrompue = 0; /* ESC pendant l'attente d'une réponse */
static int                 arret_pas = 0;           /* dernier arrêt : pas à pas */
static int                 abrege = 0;
static int                 silence = 0;             /* MITRA 15 (IMP-0265) */
static char                entree[256];
static unsigned char       fin_entree = SAISIE_RETURN;
static char                visee[256];              /* ligne visée par l'éditeur (IMP-0267) */
static unsigned int        lg_visee = 0;
static int                 a_visee = 0;

/* --- sortie ---------------------------------------------------------------- */

static void ecrire(const char *t)
{
    unsigned int n = (unsigned int) strlen(t);
    if (n == 0) return;
    console_ecrire(t, n);
    interprete_debut_de_ligne(0);
}

static void a_la_ligne(void)
{
    console_retour_chariot();
    console_interligne();
    interprete_debut_de_ligne(1);
}

/* Message sur une ligne à lui (IMP-0233). */
static void message(const char *t)
{
    if (!interprete_en_debut_de_ligne()) a_la_ligne();
    ecrire(t);
    a_la_ligne();
}

static void erreur_dialogue(NatureErreur n)
{
    char t[80];
    erreurs_formater_dialogue(n, configuration_session.libelles, t, sizeof t);
    message(t);
}

/* Erreur survenue pendant l'exécution : classe E, ou F (IMP-0250). */
static void erreur_execution(NatureErreur n, unsigned char ligne)
{
    char t[100];
    if (erreurs_classe(n) == 'F')
        erreurs_formater_fichier(n, 1, ligne, configuration_session.libelles, t, sizeof t);
    else
        erreurs_formater_execution(n, ligne, configuration_session.libelles, t, sizeof t);
    message(t);
}

/* Erreur d'une commande de fichier (IMP-0250). */
static void erreur_fichier(NatureErreur n)
{
    char t[100];
    erreurs_formater_fichier(n, 0, 0, configuration_session.libelles, t, sizeof t);
    message(t);
}

/* « texte lll », numéro sur trois chiffres (IMP-0233). */
static void message_ligne(const char *texte, unsigned char ligne)
{
    char t[40];
    unsigned int n = (unsigned int) strlen(texte);
    memcpy(t, texte, n);
    t[n] = (char) ('0' + ligne / 100 % 10);
    t[n + 1] = (char) ('0' + ligne / 10 % 10);
    t[n + 2] = (char) ('0' + ligne % 10);
    t[n + 3] = '\0';
    message(t);
}

static void deux_chiffres(char *t, unsigned int v)
{
    t[0] = (char) ('0' + v / 10 % 10);
    t[1] = (char) ('0' + v % 10);
}

/* La date et l'heure sous la forme de DAT (IMP-0228) : DATE_SEULE sans
   blancs, DATE_BLANCS précédées de deux espaces, une seule sous F4 (IMP-0266),
   HEURE_BLANCS l'heure seule précédée de deux espaces (AU REVOIR sous MITRA 15,
   IMP-0266). */
#define DATE_SEULE   0
#define DATE_BLANCS  1
#define HEURE_BLANCS 2

static void ecrire_date(int forme)
{
    InstantHorloge h;
    char t[20];

    horloge_lire(&h);
    t[0] = ' ';
    t[1] = ' ';
    deux_chiffres(t + 2, h.jour);
    t[4] = '/';
    deux_chiffres(t + 5, h.mois);
    t[7] = '/';
    deux_chiffres(t + 8, h.annee % 100);
    t[10] = ' ';
    deux_chiffres(t + 11, h.heure);
    t[13] = ':';
    deux_chiffres(t + 14, h.minute);
    t[16] = ':';
    deux_chiffres(t + 17, h.seconde);
    t[19] = '\0';
    if (forme == HEURE_BLANCS
        || (forme == DATE_BLANCS && configuration_session.profil == PROFIL_T1600_F4 && h.jour == 0)) {
        t[9] = ' ';                                 /* F4, date non réglée : champ vide (IMP-0279) */
        t[10] = ' ';
        ecrire(t + 9);
    } else if (forme == DATE_BLANCS) {
        ecrire(configuration_session.profil == PROFIL_T1600_F4 ? t + 1 : t);
    } else {
        ecrire(t + 2);
    }
}

/* La dernière ligne refusée ou listée devient la ligne visée par l'éditeur
   de ligne (IMP-0267, R3). */
static void viser(const char *texte, unsigned int n)
{
    if (n > sizeof visee - 1) n = sizeof visee - 1;
    memcpy(visee, texte, n);
    lg_visee = n;
    a_visee = 1;
}

/* --- saisie (IMP-0231, IMP-0238) ------------------------------------------- */

/* ESC : le lecteur de ruban s'arrête et SILENCE est annulé (INRDP75 §4-1,
   annexe II p. 189 ; IMP-0263, IMP-0265). */
static void echappement(void)
{
    ruban_lecteur(0);
    if (silence) {
        silence = 0;
        console_echo(1);
    }
}

/* Lit une entrée, au clavier ou sur le ruban (IMP-0263), et applique la
   correction « \ » ; rend sa longueur. */
static unsigned int lire_entree(void)
{
    char brut[256];
    unsigned int n = 0;
    unsigned int i;
    unsigned int k = 0;
    int r;
    int lu_ruban = 0;

    taches_suspendre();                    /* demande d'entrée (INRDP75 §3-1, IMP-0269) */
    for (;;) {
        if (ruban_en_marche()) {
            if (console_interruption()) {
                fin_entree = SAISIE_ESC;
                break;
            }
            r = ruban_lire_ligne(brut, sizeof brut, &n, &fin_entree);
            if (r == RUBAN_ESC) {                      /* ESC perforé (INRDP75 p. 191) */
                n = 0;
                fin_entree = SAISIE_ESC;
                break;
            }
            if (r == RUBAN_LIGNE) {
                lu_ruban = 1;
                if (PROFIL_EST_T1600(configuration_session.profil)) {   /* tout est imprimé */
                    if (n > 0) console_ecrire(brut, n);
                    if (fin_entree == SAISIE_RETURN) {
                        console_retour_chariot();
                        console_interligne();
                    }
                }
                break;
            }
            /* fin du ruban : retour au clavier */
        }
        n = console_lire_ligne(brut, sizeof brut, &fin_entree);
        if (fin_entree != SAISIE_XON) break;
        if (PROFIL_EST_T1600(configuration_session.profil))
            ruban_lecteur(1);                          /* mise en route du lecteur */
    }
    taches_reactiver();
    if (fin_entree == SAISIE_ESC) echappement();
    for (i = 0; i < n; i++) {
        if (brut[i] == '\\') {
            if (k > 0) k--;
        } else {
            entree[k] = brut[i];
            k++;
        }
    }
    if (lu_ruban && configuration_session.profil == PROFIL_MITRA15_V8) return k;   /* rien d'affiché */
    if (fin_entree == SAISIE_RETURN) interprete_debut_de_ligne(1);   /* écho de RETURN */
    else if (n > 0 && !silence) interprete_debut_de_ligne(0);         /* SILENCE : rien d'écrit */
    return k;
}

/* --- paramètres ------------------------------------------------------------ */

static unsigned int borne_ligne(void)
{
    return PROFIL_EST_T1600(configuration_session.profil) ? 255u : 250u;
}

static void sauter_espaces(const char *t, unsigned int n, unsigned int *i)
{
    while (*i < n && t[*i] == ' ') *i = *i + 1;
}

/* Un numéro de ligne du profil ; 1 en cas de succès. */
static int numero(const char *t, unsigned int n, unsigned int *i, unsigned int *v)
{
    unsigned long x = 0;
    unsigned int debut;

    sauter_espaces(t, n, i);
    debut = *i;
    while (*i < n && t[*i] >= '0' && t[*i] <= '9') {
        if (x < 100000ul) x = x * 10 + (unsigned long) (t[*i] - '0');
        *i = *i + 1;
    }
    sauter_espaces(t, n, i);
    if (*i == debut || x < 1 || x > borne_ligne()) return 0;
    *v = (unsigned int) x;
    return 1;
}

/* n1{,n2} ; *n2 vaut 0 en l'absence du second. */
static int lignes(const char *t, unsigned int n, unsigned int *n1, unsigned int *n2)
{
    unsigned int i = 0;
    *n2 = 0;
    if (!numero(t, n, &i, n1)) return 0;
    if (i < n && t[i] == ',') {
        i++;
        if (!numero(t, n, &i, n2)) return 0;
    }
    return i == n;
}

/* --- exécution (IMP-0232, IMP-0233, IMP-0237) ------------------------------- */

static void executer(void)
{
    FinExecution f;
    unsigned int n;

    arret_pas = 0;
    lecture_interrompue = 0;
    for (;;) {
        f = interprete_executer(&pile, &pos, &diag);
        if (f == FIN_EXECUTER) {                              /* module chargé (point B5) */
            pile_ramener(&pile, 0);
            interprete_reinitialiser();
            continue;
        }
        if (f != FIN_LECTURE) break;
        n = lire_entree();
        if (fin_entree == SAISIE_ESC) {                     /* IMP-0237, IMP-0239 */
            lecture_interrompue = 1;
            f = FIN_INTERRUPTION;
            break;
        }
        interprete_fournir_saisie(entree, n, fin_entree);
    }
    ruban_perforateur_arreter();                              /* IMP-0274 */
    compteur_arret = programme_lignes_entrees();
    switch (f) {
    case FIN_TERMINER:
        etat_programme = PROG_TERMINE;
        if (PROFIL_EST_T1600(configuration_session.profil)) message_ligne("TERMINE EN LIGNE ", pos.ligne);
        else message("TERMINE");
        break;
    case FIN_PAUSE:
        etat_programme = PROG_SUSPENDU;
        message_ligne("PAUSE EN LIGNE ", pos.ligne);
        break;
    case FIN_ARRET:
        etat_programme = PROG_SUSPENDU;
        message_ligne("ARRET EN LIGNE ", pos.ligne);
        break;
    case FIN_PAS:
        etat_programme = PROG_SUSPENDU;
        arret_pas = 1;
        message_ligne("LIGNE ", pos.ligne);
        break;
    case FIN_INTERRUPTION:
        etat_programme = PROG_SUSPENDU;
        echappement();
        message_ligne("PRET EN LIGNE ", pos.ligne);
        break;
    case FIN_ERREUR:
        etat_programme = PROG_ERREUR;
        erreur_execution(diag.nature, diag.ligne);
        break;
    default:
        etat_programme = PROG_ERREUR;
        erreur_execution(ERR_E_INTERNE, pos.ligne);
        break;
    }
}

/* CONTINUER, POURSUIVRE, X-OFF en pas à pas : reprise sur place. */
static void continuer(void)
{
    if (etat_programme != PROG_SUSPENDU || programme_lignes_entrees() != compteur_arret) {
        erreur_dialogue(ERR_D_CONTINUER_IMPOSSIBLE);
        return;
    }
    if (lecture_interrompue) console_bip();                  /* constaté sur LSE 8086 */
    executer();
}

static void lancer(Commande c, const char *t, unsigned int n)
{
    unsigned int n1;
    unsigned int n2;
    NatureErreur e;

    if (c == CMD_REPRENDRE
        && (etat_programme == PROG_AUCUN || programme_lignes_entrees() != compteur_arret)) {
        erreur_dialogue(ERR_D_REPRENDRE_IMPOSSIBLE);
        return;
    }
    if (!lignes(t, n, &n1, &n2)) {
        erreur_dialogue(ERR_D_PARAMETRE);
        return;
    }
    e = c == CMD_REPRENDRE ? contexte_reprendre((unsigned char) n1, (unsigned char) n2, &pos)
                           : contexte_initialiser((unsigned char) n1, (unsigned char) n2, &pos);
    if (e == ERR_E_LIGNE_INEXISTANTE) {
        erreur_dialogue(ERR_D_LIGNE_DEPART_ABSENTE);         /* point M5 */
        return;
    }
    if (e != ERR_AUCUNE) {
        etat_programme = PROG_AUCUN;            /* cadres déjà abandonnés : plus de reprise */
        erreur_execution(e, 0);
        return;
    }
    pile_ramener(&pile, 0);
    interprete_reinitialiser();
    executer();
}

/* --- listage et effacement (IMP-0234) --------------------------------------- */

/* mode : 0 LISTER, 1 NUMERO, 2 PERFORER (IMP-0264). */
static void lister(const char *t, unsigned int n, int mode)
{
    unsigned int n1;
    unsigned int n2;
    unsigned int i;
    unsigned int k;
    unsigned int colonne = 0;
    unsigned int lg;
    EntreeObjetCompile e;
    char texte[DECOMPILATEUR_LONGUEUR_MAX + 1];
    NatureErreur r = ERR_AUCUNE;

    if (!lignes(t, n, &n1, &n2)) {
        erreur_dialogue(ERR_D_PARAMETRE);
        return;
    }
    if (bibliotheque_programme_secret()) {                    /* IMP-0255 */
        erreur_fichier(ERR_F_PROGRAMME_PROTEGE);
        return;
    }
    if (mode == 2 && (r = ruban_perforer_ouvrir()) != ERR_AUCUNE) {
        erreur_fichier(r);
        return;
    }
    for (i = 0; programme_rang(i, &e) && r == ERR_AUCUNE; i++) {
        if (e.numero_ligne < n1) continue;
        if (n2 != 0 && e.numero_ligne > n2) break;
        if (console_interruption()) {                         /* IMP-0239 */
            echappement();
            message("PRET");
            break;
        }
        if (mode == 1) {                                      /* point M4 */
            for (k = 0; k < 8; k++) texte[k] = ' ';
            texte[8] = '\0';
            k = 7;
            lg = e.numero_ligne;
            do {
                texte[k] = (char) ('0' + lg % 10);
                lg /= 10;
                k--;
            } while (lg != 0);
            ecrire(texte);
            colonne++;
            if (colonne == 10) {
                a_la_ligne();
                colonne = 0;
            }
        } else {
            if (decompilateur_decompiler(&e, texte, &lg) != DECOMP_OK) lg = 0;
            if (lg > 0) {
                console_ecrire(texte, lg);
                interprete_debut_de_ligne(0);
            }
            viser(texte, lg);
            a_la_ligne();
            if (mode == 2) r = ruban_perforer(texte, lg);     /* la télétype imprime et perfore */
        }
    }
    if (colonne != 0) a_la_ligne();
    if (mode == 2) {
        ruban_perforer_fermer();
        if (r != ERR_AUCUNE) erreur_fichier(r);
    }
}

static void effacer(const char *t, unsigned int n)
{
    unsigned int i = 0;
    unsigned int a;
    unsigned int b;
    unsigned int k;

    sauter_espaces(t, n, &i);
    if (i < n && t[i] == '*') {
        i++;
        sauter_espaces(t, n, &i);
        if (i != n) {
            erreur_dialogue(ERR_D_PARAMETRE);
            return;
        }
        contexte_vider();
        programme_vider();
        pile_ramener(&pile, 0);
        etat_programme = PROG_AUCUN;
        bibliotheque_programme_libre();
        return;
    }
    if (bibliotheque_programme_secret()) {                    /* IMP-0255 */
        erreur_fichier(ERR_F_PROGRAMME_PROTEGE);
        return;
    }
    /* vérification complète avant toute suppression (VM-2202) */
    for (k = 0; k < 2; k++) {
        i = 0;
        for (;;) {
            if (!numero(t, n, &i, &a)) {
                erreur_dialogue(ERR_D_PARAMETRE);
                return;
            }
            b = a;
            if (i < n && t[i] == 'A') {
                i++;
                if (!numero(t, n, &i, &b) || b < a) {
                    erreur_dialogue(ERR_D_PARAMETRE);
                    return;
                }
            }
            if (k == 1)
                for (; a <= b; a++) programme_supprimer((unsigned char) a);
            if (i == n) break;
            if (t[i] != ',') {
                erreur_dialogue(ERR_D_PARAMETRE);
                return;
            }
            i++;
        }
    }
}

/* --- commandes de fichiers-programmes (étape 7b) ----------------------------- */

/* Nom de fichier à partir de *i, espaces sautés ; 1 si c'est un nom L.S.E. */
static int nom_fichier(const char *t, unsigned int n, unsigned int *i, char *nom)
{
    unsigned int k = 0;
    char c;

    sauter_espaces(t, n, i);
    while (*i < n && k < 6) {
        c = t[*i];
        if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) break;
        nom[k] = c;
        k++;
        *i = *i + 1;
    }
    nom[k] = '\0';
    sauter_espaces(t, n, i);
    return bibliotheque_nom_valide(nom, k);
}

/* « $ » devant un nom, sous F4 : le fichier est sur l'unité amovible (IMP-0278). */
static int sur_unite(const char *t, unsigned int n, unsigned int *i)
{
    sauter_espaces(t, n, i);
    if (configuration_session.profil != PROFIL_T1600_F4 || *i >= n || t[*i] != '$') return 0;
    *i = *i + 1;
    return 1;
}

/* RANGER ω, MODIFIER ω ; sous T1600, RANGER ω,E et RANGER ω,L (INRDP75, 5e
   partie, §2-4). */
static void ranger(const char *t, unsigned int n, int remplacer)
{
    char nom[8];
    unsigned int i = 0;
    unsigned char protection = 0;
    int unite;
    NatureErreur e;

    unite = sur_unite(t, n, &i);
    if (!nom_fichier(t, n, &i, nom)) { erreur_dialogue(ERR_D_PARAMETRE); return; }
    if (!remplacer && PROFIL_EST_T1600(configuration_session.profil) && i < n && t[i] == ',') {
        i++;
        sauter_espaces(t, n, &i);
        if (i < n && t[i] == 'E') protection = PROTECTION_E;
        else if (i < n && t[i] == 'L') protection = PROTECTION_L;
        else { erreur_dialogue(ERR_D_PARAMETRE); return; }
        i++;
        sauter_espaces(t, n, &i);
    }
    if (i != n) { erreur_dialogue(ERR_D_PARAMETRE); return; }
    if ((e = bibliotheque_ranger(nom, protection, remplacer, unite)) != ERR_AUCUNE) erreur_fichier(e);
}

static void appeler(const char *t, unsigned int n)
{
    char nom[8];
    unsigned int i = 0;
    int unite;
    NatureErreur e;

    unite = sur_unite(t, n, &i);
    if (!nom_fichier(t, n, &i, nom) || i != n) { erreur_dialogue(ERR_D_PARAMETRE); return; }
    e = bibliotheque_appeler(nom, 0, unite);
    if (e == ERR_F_MEMOIRE_PROGRAMME || e == ERR_AUCUNE) {
        pile_ramener(&pile, 0);                      /* programme remplacé ou vidé */
        interprete_reinitialiser();
        etat_programme = PROG_AUCUN;
    }
    if (e != ERR_AUCUNE) erreur_fichier(e);
}

/* SUPPRIMER : ω (MITRA 15, LSE15 §9-3) ; ω,P|D|T et * (T1600, INRDP75, 5e
   partie, §2-4). */
static void supprimer(const char *t, unsigned int n)
{
    char nom[8];
    unsigned int i = 0;
    int unite;
    NatureErreur e;
    EspaceFichier esp;

    sauter_espaces(t, n, &i);
    if (PROFIL_EST_T1600(configuration_session.profil) && i < n && t[i] == '*') {
        i++;
        sauter_espaces(t, n, &i);
        if (i != n) { erreur_dialogue(ERR_D_PARAMETRE); return; }
        if (plateforme_espace_vider(ESPACE_TEMPORAIRES) != PLAT_OK) erreur_fichier(ERR_F_ENTREE_SORTIE);
        return;
    }
    unite = sur_unite(t, n, &i);
    if (!nom_fichier(t, n, &i, nom)) { erreur_dialogue(ERR_D_PARAMETRE); return; }
    if (PROFIL_EST_T1600(configuration_session.profil)) {
        if (i + 1 >= n || t[i] != ',') { erreur_dialogue(ERR_D_PARAMETRE); return; }
        i++;
        sauter_espaces(t, n, &i);
        if (t[i] == 'P') esp = unite ? ESPACE_UNITE_PROGRAMMES : ESPACE_PROGRAMMES;
        else if (t[i] == 'D') esp = unite ? ESPACE_UNITE_DONNEES : ESPACE_DONNEES;
        else if (t[i] == 'T' && !unite) esp = ESPACE_TEMPORAIRES;
        else { erreur_dialogue(ERR_D_PARAMETRE); return; }
        i++;
        sauter_espaces(t, n, &i);
        if (i != n) { erreur_dialogue(ERR_D_PARAMETRE); return; }
        e = bibliotheque_supprimer(esp, nom);
    } else {
        if (i != n) { erreur_dialogue(ERR_D_PARAMETRE); return; }
        e = bibliotheque_supprimer(plateforme_fichier_existe(ESPACE_PROGRAMMES, nom)
                                   ? ESPACE_PROGRAMMES : ESPACE_DONNEES, nom);
    }
    if (e != ERR_AUCUNE) erreur_fichier(e);
}

/* CATALOGUER ω1,ω2 ; sous T1600, CATALOGUER ω1,ω2,L (INRDP75, 5e partie,
   §2-4 ; point C6). */
static void cataloguer(const char *t, unsigned int n)
{
    char temporaire[8];
    char permanent[8];
    unsigned int i = 0;
    unsigned char protection = 0;
    int unite;
    NatureErreur e;

    if (!nom_fichier(t, n, &i, temporaire) || i >= n || t[i] != ',') { erreur_dialogue(ERR_D_PARAMETRE); return; }
    i++;
    unite = sur_unite(t, n, &i);
    if (!nom_fichier(t, n, &i, permanent)) { erreur_dialogue(ERR_D_PARAMETRE); return; }
    if (PROFIL_EST_T1600(configuration_session.profil) && i < n && t[i] == ',') {
        i++;
        sauter_espaces(t, n, &i);
        if (i >= n || t[i] != 'L') { erreur_dialogue(ERR_D_PARAMETRE); return; }
        protection = PROTECTION_L;
        i++;
        sauter_espaces(t, n, &i);
    }
    if (i != n) { erreur_dialogue(ERR_D_PARAMETRE); return; }
    if ((e = fichiers_cataloguer(temporaire, permanent, protection, unite)) != ERR_AUCUNE) erreur_fichier(e);
}

/* --- identifications (IMP-0254, IMP-0256, IMP-0257) --------------------------- */

/* Entier de 0 à max ; 1 en cas de succès. */
static int entier(const char *t, unsigned int n, unsigned int *i, unsigned long max, unsigned long *v)
{
    unsigned long x = 0;
    unsigned int debut;

    sauter_espaces(t, n, i);
    debut = *i;
    while (*i < n && t[*i] >= '0' && t[*i] <= '9') {
        if (x <= max) x = x * 10 + (unsigned long) (t[*i] - '0');
        *i = *i + 1;
    }
    sauter_espaces(t, n, i);
    if (*i == debut || x > max) return 0;
    *v = x;
    return 1;
}

/* v à droite dans un champ de largeur donnée (0 : sans blancs), à partir
   de m[*k]. */
static void champ(char *m, unsigned int *k, unsigned long v, unsigned int largeur)
{
    char c[12];
    unsigned int d = 0;

    do {
        c[d] = (char) ('0' + v % 10);
        v /= 10;
        d++;
    } while (v != 0);
    for (; largeur > d; largeur--) m[(*k)++] = ' ';
    while (d > 0) m[(*k)++] = c[--d];
}

static void texte(char *m, unsigned int *k, const char *s)
{
    while (*s != '\0') m[(*k)++] = *s++;
}

/* Blancs jusqu'à la colonne c (au moins un si la colonne est atteinte). */
static void colonne(char *m, unsigned int *k, unsigned int c)
{
    do m[(*k)++] = ' '; while (*k < c);
}

/* Nombre à droite, son dernier chiffre en colonne fin. */
static void nombre_fin(char *m, unsigned int *k, unsigned long v, unsigned int fin)
{
    unsigned int d = 1;
    unsigned long x = v;

    while (x >= 10) {
        x /= 10;
        d++;
    }
    if (*k + d <= fin) colonne(m, k, fin + 1 - d);
    champ(m, k, v, 0);
}

static void ligne_finie(char *m, unsigned int k)
{
    m[k] = '\0';
    ecrire(m);
    a_la_ligne();
}

/* Identification 0, espace temporaire standard (BONJOUR, AU REVOIR). */
static unsigned int identification_initiale = 0;   /* option -ID (IMP-0268) */

void moniteur_fixer_identification(unsigned int numero)
{
    identification_initiale = numero;
}

/* Identification de la session : 0 après AU REVOIR ; à BONJOUR, celle de
   l'option -ID si elle est définie, sinon 0 (IMP-0268). Espace temporaire
   standard, ou « n ». 1 si l'identification retenue n'est pas 0. */
static int identification_nulle(int bonjour)
{
    Identification id;

    if (!bonjour || !bibliotheque_identification_chercher(identification_initiale, &id))
        bibliotheque_identification_chercher(0, &id);
    bibliotheque_identifier(&id);
    fichiers_fixer_espace(id.secteurs > fichiers_espace_standard() ? id.secteurs
                                                                    : fichiers_espace_standard());
    bibliotheque_programme_libre();
    return id.numero != 0;
}

/* IDENTIFICATION n (LSE15 §9-3 ; INRDP75, 5e partie, §2-2 ; MEMENTO82
   §3.6.1). La saisie est lue sans écho (commande). */
static void identification(const char *t, unsigned int n)
{
    Identification id;
    unsigned int i = 0;
    unsigned int k;
    unsigned long v;
    unsigned long espace = fichiers_espace_standard();
    unsigned long libres;
    char m[60];

    if (!entier(t, n, &i, IDENTIFICATION_MAX, &v) || i != n
        || !bibliotheque_identification_chercher((unsigned int) v, &id)) {
        erreur_dialogue(ERR_D_IDENTIFICATION_ILLEGALE);   /* « ILLEGALE » : sans effet */
        return;
    }
    if ((id.attributs & ATTR_ETENDU) != 0      /* « x » : MITRA 15 seul (INRDP75 option c, IMP-0268) */
        && configuration_session.profil == PROFIL_MITRA15_V8) {
        k = 0;
        texte(m, &k, "NOMBRE DE SECTEURS DISPONIBLES:");
        champ(m, &k, plateforme_secteurs_libres(), 0);
        m[k] = '\0';
        message(m);
        ecrire("ESPACE TEMPORAIRE DESIRE:");
        k = lire_entree();
        if (fin_entree == SAISIE_ESC) {
            message("PRET");
            return;
        }
        if (fin_entree == SAISIE_XOFF) a_la_ligne();
        i = 0;
        if (k > 0) {
            if (!entier(entree, k, &i, 65535ul, &v) || i != k) {
                erreur_dialogue(ERR_D_PARAMETRE);
                return;
            }
            if (v > espace) espace = v;
        }
    } else if (id.secteurs > espace) {                    /* « n » */
        espace = id.secteurs;
    }
    if (configuration_session.profil == PROFIL_MITRA15_V8)
        plateforme_espace_vider(ESPACE_TEMPORAIRES);      /* LSE15 §9-3 ; T1600 : INRDP75 p. 192 */
    bibliotheque_identifier(&id);         /* un programme secret le reste (IMP-0255) */
    if (espace > fichiers_espace_standard()) {
        libres = plateforme_secteurs_libres();
        if (libres != 0 && libres < espace) {             /* LSE15 §9-3, message (3) */
            espace = libres < fichiers_espace_standard() ? fichiers_espace_standard() : libres;
            k = 0;
            texte(m, &k, "ESPACE TEMPORAIRE REDUIT A ");
            champ(m, &k, espace, 0);
            texte(m, &k, ". SECTEURS.");
            m[k] = '\0';
            message(m);
        }
    }
    fichiers_fixer_espace(espace);
}

/* CREATION ID : n,P,D,S,!,>,x|n dans cet ordre, attributs facultatifs
   (MEMENTO82 §3.6.2). */
static void creation_id(const char *t, unsigned int n)
{
    static const char ordre[] = "PDS!>";
    static const unsigned char attribut[5] = { ATTR_P, ATTR_D, ATTR_S, ATTR_PROTEGE, ATTR_PRIVILEGIE };
    Identification id;
    unsigned int i = 0;
    unsigned int rang = 0;
    unsigned int k;
    unsigned long v;
    int fin = 0;
    char c;
    NatureErreur e;

    if (!entier(t, n, &i, IDENTIFICATION_MAX, &v) || v < 2) {   /* 0 et 1 : fixes */
        erreur_dialogue(ERR_D_PARAMETRE);
        return;
    }
    id.numero = (unsigned int) v;
    id.attributs = 0;
    id.secteurs = 0;
    while (i < n) {
        if (t[i] != ',' || fin) {
            erreur_dialogue(ERR_D_PARAMETRE);
            return;
        }
        i++;
        sauter_espaces(t, n, &i);
        c = i < n ? t[i] : '\0';
        for (k = rang; k < 5 && ordre[k] != c; k++)
            ;
        if (k < 5 && c != '\0') {
            id.attributs = id.attributs | attribut[k];
            rang = k + 1;
            i++;
            sauter_espaces(t, n, &i);
        } else if (c == 'X' || c == 'x') {
            id.attributs = id.attributs | ATTR_ETENDU;
            fin = 1;
            i++;
            sauter_espaces(t, n, &i);
        } else if (entier(t, n, &i, 65535ul, &v) && v > 0) {
            id.secteurs = (unsigned int) v;
            fin = 1;
        } else {
            erreur_dialogue(ERR_D_PARAMETRE);
            return;
        }
    }
    if ((e = bibliotheque_identification_creer(&id)) != ERR_AUCUNE) erreur_fichier(e);
}

static void suppression_id(const char *t, unsigned int n)
{
    unsigned int i = 0;
    unsigned long v;
    NatureErreur e;

    if (!entier(t, n, &i, IDENTIFICATION_MAX, &v) || i != n) {
        erreur_dialogue(ERR_D_PARAMETRE);
        return;
    }
    if ((e = bibliotheque_identification_supprimer((unsigned int) v)) != ERR_AUCUNE) erreur_fichier(e);
}

/* LISTE ID : affichage de MEMENTO82 §3.6.4. */
static void liste_id(void)
{
    Identification id;
    unsigned int r;
    unsigned int k;
    char m[60];

    if (!interprete_en_debut_de_ligne()) a_la_ligne();
    for (r = 0; bibliotheque_identification_rang(r, &id); r++) {
        k = 0;
        champ(m, &k, id.numero, 5);
        if (id.attributs & ATTR_P) texte(m, &k, ",P");
        if (id.attributs & ATTR_D) texte(m, &k, ",D");
        if (id.attributs & ATTR_S) texte(m, &k, ",SECRET");
        if (id.attributs & ATTR_PROTEGE) texte(m, &k, ",PROTEGE");
        if (id.attributs & ATTR_PRIVILEGIE) texte(m, &k, ",PRIVILEGIE");
        if (id.attributs & ATTR_ETENDU) texte(m, &k, ",ETENDU");
        if (id.secteurs != 0) {
            texte(m, &k, ",");
            champ(m, &k, id.secteurs, 0);
        }
        ligne_finie(m, k);
    }
}

/* --- TABLE DES FICHIERS, UTILISATION DISQUE (IMP-0257) ------------------------- */

static unsigned long octets(EspaceFichier e, const char *nom)
{
    FichierPlateforme f;
    unsigned long n;

    if (plateforme_fichier_ouvrir(e, nom, FICHIER_LECTURE, &f) != PLAT_OK) return 0;
    n = plateforme_fichier_taille(f);
    plateforme_fichier_fermer(f);
    return n;
}

/* Noms valides d'un espace, lus en une seule ouverture du répertoire et
   triés, pour les appels successifs d'espace_suivant (audit du code, AP-06).
   La liste est refaite au premier appel d'un parcours (strict nul), ou pour
   un autre espace ou un autre préfixe ; elle est rendue à la fin de chaque
   commande. Faute de mémoire, l'espace est relu à chaque appel. */
typedef char NomCourt[6];
static NomCourt *liste_noms = NULL;
static unsigned int liste_nb = 0;
static unsigned int liste_cap = 0;
static EspaceFichier liste_espace = 0;
static char liste_prefixe[6];
static int liste_valide = 0;
static int liste_echec = 0;

static void liste_rendre(void)
{
    free(liste_noms);
    liste_noms = NULL;
    liste_nb = 0;
    liste_cap = 0;
    liste_valide = 0;
}

/* Insertion triée d'un nom de l'espace (rappel de plateforme_espace_parcourir). */
static void liste_ajouter(const char *x, void *contexte)
{
    const char *prefixe = (const char *) contexte;
    unsigned int lp = (unsigned int) strlen(prefixe);
    unsigned int i;
    NomCourt *t;

    if (liste_echec || !bibliotheque_nom_valide(x, (unsigned int) strlen(x))) return;
    if (lp > 0 && strncmp(x, prefixe, lp) != 0) return;
    if (liste_nb == liste_cap) {
        t = (NomCourt *) realloc(liste_noms, (size_t) (liste_cap + 32u) * sizeof(NomCourt));
        if (t == NULL) {
            liste_echec = 1;
            return;
        }
        liste_noms = t;
        liste_cap = liste_cap + 32u;
    }
    i = liste_nb;
    while (i > 0 && strcmp(liste_noms[i - 1], x) > 0) {
        strcpy(liste_noms[i], liste_noms[i - 1]);
        i--;
    }
    strcpy(liste_noms[i], x);
    liste_nb++;
}

/* Fichier de l'espace dont le nom est le plus petit au-delà de apres (et
   commence par prefixe s'il n'est pas vide) ; 1 si trouvé. */
static int espace_suivant(EspaceFichier e, const char *apres, int strict, const char *prefixe,
                          char *nom)
{
    char x[16];
    unsigned int r;
    unsigned int lp = (unsigned int) strlen(prefixe);
    unsigned int bas;
    unsigned int haut;
    unsigned int milieu;
    int trouve = 0;
    int c;

    if (!strict || !liste_valide || liste_espace != e || strcmp(liste_prefixe, prefixe) != 0) {
        liste_rendre();
        liste_echec = 0;
        if (lp < sizeof liste_prefixe
            && plateforme_espace_parcourir(e, liste_ajouter, (void *) prefixe) && !liste_echec) {
            liste_valide = 1;
            liste_espace = e;
            strcpy(liste_prefixe, prefixe);
        } else {
            liste_rendre();
        }
    }
    if (liste_valide) {                       /* premier nom au-delà de apres */
        bas = 0;
        haut = liste_nb;
        while (bas < haut) {
            milieu = (bas + haut) / 2;
            c = strcmp(liste_noms[milieu], apres);
            if (c < 0 || (strict && c == 0)) bas = milieu + 1;
            else haut = milieu;
        }
        if (bas >= liste_nb) return 0;
        strcpy(nom, liste_noms[bas]);
        return 1;
    }

    for (r = 0; plateforme_fichier_lister(e, r, x); r++) {
        if (!bibliotheque_nom_valide(x, (unsigned int) strlen(x))) continue;
        if (lp > 0 && strncmp(x, prefixe, lp) != 0) continue;
        c = strcmp(x, apres);
        if (c < 0 || (strict && c == 0)) continue;
        if (trouve && strcmp(x, nom) >= 0) continue;
        strcpy(nom, x);
        trouve = 1;
    }
    return trouve;
}

/* Paramètre « XXXX[,P|D|T] » (LAFOND83 p. 17, 18, 110 ; IMP-0257) : tout
   pour « * » (T1600) ou « A » (MITRA 15) sans lettre ; sinon le seul type
   indiqué (P par défaut), à partir du nom donné. */
typedef struct {
    int  tout;
    int  unite;              /* « $ » : le catalogue de l'unité (IMP-0278) */
    char type;               /* 'P', 'D' ou 'T' */
    char depart[6];
} Depart;

static int depart_lire(const char *t, unsigned int n, Depart *d)
{
    unsigned int i = 0;
    unsigned int k = 0;
    int joker = 0;
    int lettre = 0;
    char c;

    d->type = 'P';
    d->unite = sur_unite(t, n, &i);
    if (PROFIL_EST_T1600(configuration_session.profil) && i < n && t[i] == '*') {
        joker = 1;
        i++;
    } else {
        while (i < n && k < 5) {
            c = t[i];
            if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) break;
            d->depart[k++] = c;
            i++;
        }
        if (!bibliotheque_nom_valide(d->depart, k)) return 0;
        joker = configuration_session.profil == PROFIL_MITRA15_V8 && k == 1 && d->depart[0] == 'A';
    }
    d->depart[joker ? 0 : k] = '\0';
    sauter_espaces(t, n, &i);
    if (i < n && t[i] == ',') {
        i++;
        sauter_espaces(t, n, &i);
        if (i >= n || (t[i] != 'P' && t[i] != 'D' && (t[i] != 'T' || d->unite))) return 0;
        d->type = t[i];
        lettre = 1;
        i++;
        sauter_espaces(t, n, &i);
    }
    d->tout = joker && !lettre;
    return i == n;
}

/* Fichier suivant d'une section, par ordre alphabétique (T4). */
static int fichier_suivant(int unite, char type, const char *apres, int strict, int compte, char *nom,
                           unsigned char *compte_lu, unsigned char *protection, unsigned char date[3])
{
    *compte_lu = 0;
    *protection = 0;
    if (type == 'T') return espace_suivant(ESPACE_TEMPORAIRES, apres, strict, "", nom);
    return bibliotheque_catalogue_suivant(unite, type == 'P' ? CATALOGUE_PROGRAMME : CATALOGUE_DONNEES,
                                          apres, strict, compte, nom, compte_lu, protection, date);
}

/* ESC entre deux lignes : PRET (IMP-0239). */
static int interrompre(void)
{
    if (!console_interruption()) return 0;
    echappement();
    message("PRET");
    return 1;
}

static void entete(const char *titre, const char *etoiles, const char *colonnes)
{
    ecrire(titre);
    a_la_ligne();
    ecrire(etoiles);
    a_la_ligne();
    a_la_ligne();
    ecrire(colonnes);
    a_la_ligne();
    a_la_ligne();
}

static void date_catalogue(char *m, unsigned int *k, const unsigned char d[3])
{
    deux_chiffres(m + *k, d[0]);
    m[*k + 2] = '/';
    deux_chiffres(m + *k + 3, d[1]);
    m[*k + 5] = '/';
    deux_chiffres(m + *k + 6, d[2]);
    *k = *k + 8;
}

/* Une section MITRA 15 (INRDP75 p. 133 et 134 ; LAFOND83 p. 17, 110) ;
   1 si ESC l'a interrompue. */
static int section_mitra(char type, const char *depart, int compte)
{
    unsigned int k;
    int strict = 0;
    char cle[16];
    char nom[16];
    unsigned char cpt;
    unsigned char protection;
    unsigned char date[3];
    char m[80];

    if (type == 'P')
        entete("FICHIERS-PROGRAMME", "******************", " NOM NO.COMPTE  DATE  NB.MOTS");
    else if (type == 'D')
        entete("FICHIERS-DONNEE PERMANENTS", "**************************",
               " NOM NO.COMPTE  DATE  NB.SECTEURS");
    else
        entete("FICHIERS-DONNEE TEMPORAIRES", "***************************", " NOM CONSOLE NB.SECTEURS");
    strcpy(cle, depart);
    while (fichier_suivant(0, type, cle, strict, compte, nom, &cpt, &protection, date)) {
        if (interrompre()) return 1;
        k = 0;
        texte(m, &k, nom);
        if (type == 'T') {
            colonne(m, &k, 7);
            texte(m, &k, "01");
            champ(m, &k, fichiers_secteurs(0, nom), 10);
        } else {
            colonne(m, &k, type == 'P' ? 8 : 9);
            deux_chiffres(m + k, cpt);
            k = k + 2;
            colonne(m, &k, 14);
            date_catalogue(m, &k, date);
            if (type == 'P') {
                colonne(m, &k, k + 2);
                champ(m, &k, (octets(ESPACE_PROGRAMMES, nom) + 1) / 2, 0);
            } else {
                champ(m, &k, fichiers_secteurs(1, nom), 6);
            }
        }
        ligne_finie(m, k);
        strcpy(cle, nom);
        strict = 1;
    }
    return 0;
}

static void table_mitra(int tout, const Depart *d)
{
    static const char types[3] = { 'P', 'D', 'T' };
    unsigned int s;
    unsigned int k;
    int premiere = 1;
    char m[80];

    if (!interprete_en_debut_de_ligne()) a_la_ligne();
    ecrire_date(DATE_SEULE);             /* date et heure sous la commande (T5) */
    a_la_ligne();
    a_la_ligne();
    for (s = 0; s < 3; s++) {
        if (!d->tout && types[s] != d->type) continue;
        if (!premiere) {
            a_la_ligne();
            a_la_ligne();
        }
        premiere = 0;
        if (section_mitra(types[s], d->depart, tout || types[s] == 'T' ? -1 : (int) bibliotheque_compte()))
            return;
    }
    if (!tout || !d->tout) return;
    a_la_ligne();
    k = 0;
    texte(m, &k, "NOMBRE DE SECTEURS LIBRES:");
    champ(m, &k, plateforme_secteurs_libres(), 0);
    ligne_finie(m, k);
    ecrire("**************************");
    a_la_ligne();
    a_la_ligne();
    entete("ESPACE TEMPORAIRE ALLOUE", "************************", "CONSOLE NB.SECTEURS");
    k = 0;
    texte(m, &k, "  01         ");
    champ(m, &k, fichiers_espace(), 0);
    ligne_finie(m, k);
}

/* « ESPACE …( nnnn SECT.) OCCUPE A ppp % » (T1600). */
static void occupation(const char *titre, unsigned long total, unsigned long occupe)
{
    char m[80];
    unsigned int k = 0;

    texte(m, &k, titre);
    champ(m, &k, total, 4);
    texte(m, &k, " SECT.) OCCUPE A ");
    champ(m, &k, total != 0 ? occupe * 100 / total : 0, 3);
    texte(m, &k, " %");
    ligne_finie(m, k);
    a_la_ligne();
}

/* Ligne T1600 : nom, type ou compte (colonne 10 ou 14), longueur en mots
   (fin en colonne 31), protection (colonne 45). */
static void ligne_t1600(const char *nom, const char *type, int compte, unsigned long mots,
                        unsigned char protection)
{
    char m[80];
    unsigned int k = 0;

    texte(m, &k, nom);
    if (type != 0) {
        colonne(m, &k, 10);
        texte(m, &k, type);
    }
    if (compte >= 0) {
        colonne(m, &k, 14);
        deux_chiffres(m + k, (unsigned int) compte);
        k = k + 2;
    }
    nombre_fin(m, &k, mots, 31);
    if (protection != 0) {
        colonne(m, &k, 45);
        m[k++] = protection == PROTECTION_E ? 'E' : 'L';
    }
    ligne_finie(m, k);
}

/* Lignes T1600 d'un type ; 1 si ESC les a interrompues. */
static int lignes_t1600(int unite, char type, const char *depart, int tout)
{
    static const char *const libelle[3] = { "TEMPORAIRE", " DONNEE", "PROGRAMME" };
    int strict = 0;
    char cle[16];
    char nom[16];
    unsigned char cpt;
    unsigned char protection;
    unsigned char date[3];
    EspaceFichier e = type == 'T' ? ESPACE_TEMPORAIRES
                      : type == 'P' ? (unite ? ESPACE_UNITE_PROGRAMMES : ESPACE_PROGRAMMES)
                                    : (unite ? ESPACE_UNITE_DONNEES : ESPACE_DONNEES);

    strcpy(cle, depart);
    while (fichier_suivant(unite, type, cle, strict, tout || type == 'T' ? -1 : (int) bibliotheque_compte(),
                           nom, &cpt, &protection, date)) {
        if (interrompre()) return 1;
        ligne_t1600(nom, tout ? 0 : libelle[type == 'T' ? 0 : type == 'D' ? 1 : 2],
                    tout && type != 'T' ? (int) cpt : -1, octets(e, nom) / 2, protection);
        strcpy(cle, nom);
        strict = 1;
    }
    return 0;
}

/* Forme T1600 (INRDP75, 5e partie, p. 133 et 134). */
static void table_t1600(int tout, const Depart *d)
{
    static const char ordre_table[3] = { 'T', 'D', 'P' };
    static const char ordre_utilisation[3] = { 'P', 'D', 'T' };
    unsigned int r;
    unsigned int s;
    int libre;
    char cat;
    char nom[16];
    unsigned char compte;
    unsigned char protection;
    unsigned char date[3];
    unsigned long occupe = 0;
    char type;

    if (!interprete_en_debut_de_ligne()) a_la_ligne();
    a_la_ligne();
    if (!tout) {
        if (!d->unite) occupation("ESPACE TEMPORAIRE(", fichiers_espace(), fichiers_secteurs_temporaires());
        ecrire(" NOM       TYPE        LONGUEUR MOTS   PROTECTION");
        a_la_ligne();
        for (s = 0; s < 3; s++) {
            if (d->unite && ordre_table[s] == 'T') continue;
            if ((d->tout || ordre_table[s] == d->type) && lignes_t1600(d->unite, ordre_table[s], d->depart, 0))
                return;
        }
        return;
    }
    for (r = 0; !d->unite && bibliotheque_catalogue_rang(r, &libre, &cat, nom, &compte, &protection, date); r++)
        if (!libre)
            occupe = occupe + (octets(cat == CATALOGUE_PROGRAMME ? ESPACE_PROGRAMMES : ESPACE_DONNEES, nom)
                               + 255) / 256;
    if (!d->unite) occupation("ESPACE PERMANENT (", occupe + plateforme_secteurs_libres(), occupe);
    for (s = 0; s < 3; s++) {
        type = ordre_utilisation[s];
        if (!d->tout && type != d->type) continue;
        if (d->unite && type == 'T') continue;
        if (type == 'T') {
            ecrire(" CONSOLE 1");
            a_la_ligne();
            a_la_ligne();
            ecrire(" NOM                   LONGUEUR MOTS");
        } else {
            ecrire(type == 'P' ? " FICHIERS-PROGRAMMES" : " FICHIERS-DONNEES PERMANENTS");
            a_la_ligne();
            a_la_ligne();
            ecrire(" NOM     NO COMPTE     LONGUEUR MOTS   PROTECTION");
        }
        a_la_ligne();
        if (lignes_t1600(d->unite, type, d->depart, 1)) return;
        if (type != 'T') a_la_ligne();
    }
}

static void table(int tout, const char *t, unsigned int n)
{
    Depart d;

    if (!depart_lire(t, n, &d)) {
        erreur_dialogue(ERR_D_PARAMETRE);
        return;
    }
    if (d.unite && !plateforme_unite_disponible()) {
        erreur_fichier(ERR_F_UNITE);
        return;
    }
    if (PROFIL_EST_T1600(configuration_session.profil)) {
        table_t1600(tout, &d);
        return;
    }
    if (tout && (bibliotheque_identification()->attributs & ATTR_PRIVILEGIE) == 0) {
        erreur_dialogue(ERR_D_COMMANDE_ERRONEE);          /* INRDP75 p. 134 : « certains identificateurs » */
        return;
    }
    table_mitra(tout, &d);
}

/* DUPLIQUER [$]motif[,P|D] : motif = nom, groupe de caractères suivi de
   « * », ou « * » ; « $ » : de l'unité vers la bibliothèque, sinon l'inverse
   (LAFOND83 p. 109 ; témoignage d'Éric ; IMP-0258). */
static void dupliquer(const char *t, unsigned int n)
{
    char motif[8];
    char cle[16];
    char nom[16];
    unsigned int i = 0;
    unsigned int k = 0;
    unsigned int c;
    int depuis_unite = 0;
    int joker = 0;
    int strict;
    int programme;
    int copies = 0;
    char type = 0;
    char x;
    EspaceFichier source;
    NatureErreur e = ERR_AUCUNE;

    sauter_espaces(t, n, &i);
    if (i < n && t[i] == '$') {
        depuis_unite = 1;
        i++;
    }
    while (i < n && k < 5) {
        x = t[i];
        if (!((x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9'))) break;
        motif[k++] = x;
        i++;
    }
    motif[k] = '\0';
    if (i < n && t[i] == '*') {
        joker = 1;
        i++;
    }
    if ((k > 0 || !joker) && !bibliotheque_nom_valide(motif, k)) { erreur_dialogue(ERR_D_PARAMETRE); return; }
    sauter_espaces(t, n, &i);
    if (i < n && t[i] == ',') {
        i++;
        sauter_espaces(t, n, &i);
        if (i >= n || (t[i] != 'P' && t[i] != 'D')) { erreur_dialogue(ERR_D_PARAMETRE); return; }
        type = t[i];
        i++;
        sauter_espaces(t, n, &i);
    }
    if (i != n) { erreur_dialogue(ERR_D_PARAMETRE); return; }
    if (!plateforme_unite_disponible()) { erreur_fichier(ERR_F_UNITE); return; }
    for (c = 0; c < 2 && e == ERR_AUCUNE; c++) {
        programme = c == 0;
        if (type != 0 && (type == 'P') != programme) continue;
        source = depuis_unite ? (programme ? ESPACE_UNITE_PROGRAMMES : ESPACE_UNITE_DONNEES)
                              : (programme ? ESPACE_PROGRAMMES : ESPACE_DONNEES);
        if (!joker) {
            if (plateforme_fichier_existe(source, motif)) {
                e = bibliotheque_dupliquer(depuis_unite, programme, motif);
                copies++;
            }
            continue;
        }
        strcpy(cle, motif);
        strict = 0;
        while (e == ERR_AUCUNE && espace_suivant(source, cle, strict, motif, nom)) {
            e = bibliotheque_dupliquer(depuis_unite, programme, nom);
            copies++;
            strcpy(cle, nom);
            strict = 1;
        }
    }
    if (e == ERR_AUCUNE && copies == 0) e = ERR_F_FICHIER_INEXISTANT;
    if (e != ERR_AUCUNE) erreur_fichier(e);
}

/* --- DECODER, ENCODER (étape 7e1) ------------------------------------------------ */

/* {$}ω{,n1{,n2}} ; *n1 vaut 0 en l'absence de n1, *n2 en l'absence de n2. */
static int nom_et_lignes(const char *t, unsigned int n, char *nom, unsigned int *n1, unsigned int *n2,
                         int *unite)
{
    unsigned int i = 0;

    *n1 = 0;
    *n2 = 0;
    *unite = sur_unite(t, n, &i);
    if (!nom_fichier(t, n, &i, nom)) return 0;
    if (i < n && t[i] == ',') {
        i++;
        if (!numero(t, n, &i, n1)) return 0;
        if (i < n && t[i] == ',') {
            i++;
            if (!numero(t, n, &i, n2) || *n2 < *n1) return 0;
        }
    }
    return i == n;
}

/* DECODER ω{,n1{,n2}} (INRDP75, 5e partie, §4-1 ; LSE15 §9-3 ; IMP-0260). */
static void decoder(const char *t, unsigned int n)
{
    char nom[8];
    unsigned int n1;
    unsigned int n2;
    int unite;
    NatureErreur e;

    if (!nom_et_lignes(t, n, nom, &n1, &n2, &unite)) {
        erreur_dialogue(ERR_D_PARAMETRE);
        return;
    }
    if (bibliotheque_programme_secret()) {                    /* IMP-0255 */
        erreur_fichier(ERR_F_PROGRAMME_PROTEGE);
        return;
    }
    e = codage_decoder(nom, (unsigned char) (n1 == 0 ? 1 : n1), (unsigned char) n2, unite);
    if (e == ERR_AUCUNE) return;
    if (erreurs_classe(e) == 'F') erreur_fichier(e);
    else erreur_execution(e, 0);
}

/* ENCODER ω{,n1{,n2}} (INRDP75, 5e partie, §4-2 ; LSE15 §9-3 ; IMP-0261) :
   sous MITRA 15 sans n1, le programme courant est remplacé ; sinon les
   lignes de n1 à n2 s'ajoutent. La première erreur arrête l'encodage. Le
   programme passe à l'état « aucun » seulement s'il change : remplacement,
   ou première ligne ajoutée ou remplacée (LSE15 §9-2 ; audit du code, §9
   point 13). */
static void encoder(const char *t, unsigned int n)
{
    char nom[8];
    char m[100];
    unsigned int n1;
    unsigned int n2;
    unsigned int k;
    unsigned int i;
    unsigned int num;
    int remplacer;
    int modifie;
    int unite;
    ResultatCompilation r;
    DiagnosticCompilation d;
    NatureErreur e;

    if (!nom_et_lignes(t, n, nom, &n1, &n2, &unite)) {
        erreur_dialogue(ERR_D_PARAMETRE);
        return;
    }
    remplacer = configuration_session.profil == PROFIL_MITRA15_V8 && n1 == 0;
    if (!remplacer && bibliotheque_programme_secret()) {
        erreur_fichier(ERR_F_PROGRAMME_PROTEGE);
        return;
    }
    if ((e = codage_ouvrir(nom, unite)) != ERR_AUCUNE) {
        erreur_fichier(e);
        return;
    }
    modifie = 0;
    if (remplacer) {                                          /* comme APPELER */
        contexte_vider();
        programme_vider();
        bibliotheque_programme_libre();
        pile_ramener(&pile, 0);
        interprete_reinitialiser();
        etat_programme = PROG_AUCUN;
        modifie = 1;
    }
    while (codage_ligne(entree, sizeof entree - 1, &k, &e)) {
        if (k == 0) continue;
        if (entree[0] < '0' || entree[0] > '9') {             /* ligne sans numéro */
            erreur_dialogue(ERR_D_COMMANDE_ERRONEE);
            break;
        }
        for (i = 0, num = 0; i < k && entree[i] >= '0' && entree[i] <= '9' && num < 1000; i++)
            num = num * 10 + (unsigned int) (entree[i] - '0');
        if (num < (n1 == 0 ? 1 : n1) || (n2 != 0 && num > n2)) continue;
        if (!compilateur_compiler(entree, k, &r, &d)) {
            viser(entree, k);
            entree[k] = '\0';
            message(entree);                                  /* la ligne fautive */
            erreurs_formater(d.nature, configuration_session.libelles, m, sizeof m);
            message(m);
            break;
        }
        if (!programme_inserer(&r.resultat.ligne_programme, &e)) {
            objets_liberer(r.resultat.ligne_programme.bloc);
            entree[k] = '\0';
            message(entree);                                  /* première ligne non entrée (LSE15) */
            if (e != ERR_AUCUNE) {
                erreurs_formater(e, configuration_session.libelles, m, sizeof m);
                message(m);
            }
            e = ERR_AUCUNE;
            break;
        }
        if (!modifie) {                                       /* première ligne entrée */
            pile_ramener(&pile, 0);
            interprete_reinitialiser();
            etat_programme = PROG_AUCUN;
            modifie = 1;
        }
    }
    codage_fermer();
    if (e != ERR_AUCUNE) erreur_fichier(e);
}

/* --- commandes (IMP-0230, IMP-0236) ------------------------------------------ */

static void commande(Commande c)
{
    const char *nom = analyseur_nom(c);
    unsigned char p = analyseur_parametres(c);
    unsigned int n = 0;
    unsigned int v = 0;
    unsigned int i = 0;
    char t[256];

    if (c == CMD_BONJOUR) {          /* en session : ni complétion ni effet (IMP-0235) */
        erreur_dialogue(ERR_D_COMMANDE_ERRONEE);
        return;
    }
    /* complétion : la suite du nom après X-OFF ; le nom entier sur la ligne
       neuve qu'a ouverte l'écho de RETURN */
    if (!abrege) ecrire(fin_entree == SAISIE_RETURN ? nom : nom + 2);
    else if (p != PARAM_AUCUN && fin_entree == SAISIE_XOFF) ecrire(" ");
    if (p != PARAM_AUCUN) {
        if (c == CMD_IDENTIFICATION) console_echo(0);       /* clé secrète (INRDP75, LSE15) */
        n = lire_entree();
        console_echo(!silence);
        if (fin_entree == SAISIE_ESC) {
            message("PRET");
            return;
        }
        memcpy(t, entree, n);
    }
    if (fin_entree == SAISIE_XOFF) a_la_ligne();
    if (!plateforme_bibliotheque_disponible()                /* IMP-0250 : F 102 au lancement */
        && (c == CMD_RANGER || c == CMD_MODIFIER || c == CMD_APPELER || c == CMD_SUPPRIMER
            || c == CMD_CATALOGUER || c == CMD_IDENTIFICATION || c == CMD_TABLE
            || c == CMD_UTILISATION || c == CMD_DUPLIQUER || c == CMD_DECODER || c == CMD_ENCODER
            || c == CMD_CREATION_ID || c == CMD_SUPPRESSION_ID || c == CMD_LISTE_ID)) {
        erreur_fichier(ERR_F_BIBLIOTHEQUE);
        return;
    }

    switch (c) {
    case CMD_LISTER:      lister(t, n, 0); break;
    case CMD_NUMERO:      lister(t, n, 1); break;
    case CMD_EFFACER:     effacer(t, n); break;
    case CMD_EXECUTER:
    case CMD_REPRENDRE:   lancer(c, t, n); break;
    case CMD_CONTINUER:   continuer(); break;
    case CMD_POURSUIVRE:
        if (!numero(t, n, &i, &v) || i != n) {
            erreur_dialogue(ERR_D_PARAMETRE);
            break;
        }
        if (etat_programme == PROG_SUSPENDU && programme_lignes_entrees() == compteur_arret)
            contexte_fixer_arret((unsigned char) v);
        continuer();
        break;
    case CMD_PAS_A_PAS:   interprete_pas_a_pas(1); break;
    case CMD_NORMAL:      interprete_pas_a_pas(0); break;
    case CMD_ABREGER:     abrege = 1; break;
    case CMD_IN_EXTENSO:  abrege = 0; break;
    case CMD_SILENCE:                                   /* IMP-0265 */
        silence = 1;
        console_echo(0);
        break;
    case CMD_RUBAN:                                     /* SILENCE implicite (INRDP75 annexe II §3.3) */
        silence = 1;
        console_echo(0);
        ruban_lecteur(1);
        break;
    case CMD_PERFORER:    lister(t, n, 2); break;
    case CMD_RANGER:      ranger(t, n, 0); break;
    case CMD_MODIFIER:    ranger(t, n, 1); break;
    case CMD_APPELER:     appeler(t, n); break;
    case CMD_SUPPRIMER:   supprimer(t, n); break;
    case CMD_CATALOGUER:  cataloguer(t, n); break;
    case CMD_IDENTIFICATION: identification(t, n); break;
    case CMD_TABLE:       table(0, t, n); break;
    case CMD_UTILISATION: table(1, t, n); break;
    case CMD_DUPLIQUER:   dupliquer(t, n); break;
    case CMD_DECODER:     decoder(t, n); break;
    case CMD_ENCODER:     encoder(t, n); break;
    case CMD_CREATION_ID: creation_id(t, n); break;
    case CMD_SUPPRESSION_ID: suppression_id(t, n); break;
    case CMD_LISTE_ID:    liste_id(); break;
    case CMD_PRENDRE:                                   /* une seule console (IMP-0236) */
        if (!numero(t, n, &i, &v) || i != n) erreur_dialogue(ERR_D_PARAMETRE);
        else if (v != 1) erreur_dialogue(ERR_D_CONSOLE_INEXISTANTE);
        break;
    default:                                            /* étape « fichiers » (IMP-0236) */
        erreur_execution(ERR_E_INTERNE, 0);
        break;
    }
    liste_rendre();                                     /* noms d'un espace (AP-06) */
}

/* --- lignes ----------------------------------------------------------------- */

static void ligne_programme(unsigned int n)
{
    ResultatCompilation r;
    DiagnosticCompilation d;
    NatureErreur e;
    char t[100];

    if (bibliotheque_programme_secret()) {                    /* IMP-0255 */
        erreur_fichier(ERR_F_PROGRAMME_PROTEGE);
        return;
    }
    if (!compilateur_compiler(entree, n, &r, &d)) {
        viser(entree, n);
        erreurs_formater(d.nature, configuration_session.libelles, t, sizeof t);
        message(t);
        return;
    }
    if (!programme_inserer(&r.resultat.ligne_programme, &e)) {
        objets_liberer(r.resultat.ligne_programme.bloc);
        if (e != ERR_AUCUNE) {
            erreurs_formater(e, configuration_session.libelles, t, sizeof t);
            message(t);
        }
    }
}

static void ligne_bureau(unsigned int n)
{
    ResultatCompilation r;
    DiagnosticCompilation d;
    PositionExecution b;
    FinExecution f;
    unsigned int k;
    unsigned int hauteur;
    char t[100];

    if (!compilateur_compiler(entree, n, &r, &d)) {
        viser(entree, n);
        erreurs_formater(d.nature, configuration_session.libelles, t, sizeof t);
        message(t);
        return;
    }
    hauteur = pile.sommet;                      /* pile du programme suspendu (IMP-0233) */
    interprete_lecture_mettre_de_cote();        /* sa lecture aussi (IMP-0222) */
    b.ligne = 0;
    b.decalage = 0;
    for (;;) {
        f = interprete_bureau(&pile, r.resultat.bloc_bureau, r.longueur_bureau, &b, &diag);
        if (f != FIN_LECTURE) break;
        k = lire_entree();
        if (fin_entree == SAISIE_ESC) {
            message("PRET");
            break;
        }
        interprete_fournir_saisie(entree, k, fin_entree);
    }
    ruban_perforateur_arreter();                /* IMP-0274 */
    if (f == FIN_ERREUR) erreur_execution(diag.nature, 0);
    pile_ramener(&pile, hauteur);               /* après une erreur ou ESC, avant le bloc */
    interprete_lecture_retablir();
    objets_liberer(r.resultat.bloc_bureau);
}

/* --- éditeur de ligne (IMP-0267) ------------------------------------------------- */

/* Entier de 1 à 999 à partir de *i ; 0 s'il est absent ou nul. */
static unsigned int facteur(unsigned int n, unsigned int *i)
{
    unsigned int v = 0;
    while (*i < n && entree[*i] >= '0' && entree[*i] <= '9') {
        if (v < 1000) v = v * 10 + (unsigned int) (entree[*i] - '0');
        *i = *i + 1;
    }
    return v;
}

/* [n]X[p] puis /groupe/remplacement ou %groupe%remplacement (LAFOND83
   p. 133-134 ; MEMENTO82 §4) : à partir de l'occurrence p, au plus n
   occurrences du groupe sont remplacées dans la ligne visée ; la ligne
   corrigée est affichée puis traitée comme tapée. 1 si l'entrée est une
   correction. */
static int correction(unsigned int n)
{
    char res[256];
    unsigned int i = 0;
    unsigned int nombre = 1;
    unsigned int rang = 1;
    unsigned int debut;
    unsigned int lg;
    unsigned int q = 0;
    unsigned int m = 0;
    unsigned int occ = 0;
    unsigned int faits = 0;
    unsigned int j;
    char d;
    Commande c;
    CategorieEntree k;

    if (i < n && entree[i] >= '0' && entree[i] <= '9') {
        nombre = facteur(n, &i);
        if (i >= n || entree[i] != 'X') return 0;         /* ligne de programme */
    }
    if (i < n && entree[i] == 'X') {
        i++;
        if (i < n && entree[i] >= '0' && entree[i] <= '9') rang = facteur(n, &i);
    }
    if (i >= n || (entree[i] != '/' && entree[i] != '%')) return 0;
    d = entree[i];
    i++;
    debut = i;
    while (i < n && entree[i] != d) i++;
    lg = i - debut;
    if (i >= n || lg == 0 || nombre == 0 || rang == 0 || !a_visee) {
        erreur_dialogue(ERR_D_PARAMETRE);
        return 1;
    }
    i++;                                                  /* remplacement : entree[i..n) */
    if (bibliotheque_programme_secret()) {
        erreur_fichier(ERR_F_PROGRAMME_PROTEGE);
        return 1;
    }
    while (q < lg_visee) {
        if (q + lg <= lg_visee && memcmp(visee + q, entree + debut, lg) == 0) {
            occ++;
            if (occ >= rang && faits < nombre) {
                for (j = i; j < n && m < sizeof res - 1; j++) res[m++] = entree[j];
                faits++;
            } else {
                for (j = 0; j < lg && m < sizeof res - 1; j++) res[m++] = visee[q + j];
            }
            q = q + lg;
        } else {
            if (m < sizeof res - 1) res[m++] = visee[q];
            q++;
        }
    }
    if (faits == 0) return 1;                             /* aucun effet (MEMENTO82) */
    res[m] = '\0';
    message(res);                                         /* LAFOND83 : la ligne corrigée */
    memcpy(entree, res, m);
    k = analyseur_classer(entree, m, 0, &c);
    if (k == ENTREE_PROGRAMME) ligne_programme(m);
    else if (k == ENTREE_BUREAU) ligne_bureau(m);
    else erreur_dialogue(ERR_D_COMMANDE_ERRONEE);
    return 1;
}

/* --- session (IMP-0228) ------------------------------------------------------- */

/* Deux chiffres en *i ; 1 si c'en sont. */
static int deux_lus(const char *t, unsigned int n, unsigned int *i, unsigned int *v)
{
    if (*i + 2 > n || t[*i] < '0' || t[*i] > '9' || t[*i + 1] < '0' || t[*i + 1] > '9') return 0;
    *v = (unsigned int) (t[*i] - '0') * 10 + (unsigned int) (t[*i + 1] - '0');
    *i = *i + 2;
    return 1;
}

/* jj/mm/aa hh:mm{:ss}, 00/00/00 pour une date non réglée ; 1 si correcte. */
static int date_saisie(const char *t, unsigned int n, InstantHorloge *h)
{
    unsigned int i = 0;
    unsigned int j;
    unsigned int m;
    unsigned int a;
    unsigned int hh;
    unsigned int mi;
    unsigned int s = 0;

    if (!deux_lus(t, n, &i, &j) || i >= n || t[i++] != '/') return 0;
    if (!deux_lus(t, n, &i, &m) || i >= n || t[i++] != '/') return 0;
    if (!deux_lus(t, n, &i, &a) || i >= n || t[i++] != ' ') return 0;
    if (!deux_lus(t, n, &i, &hh) || i >= n || t[i++] != ':') return 0;
    if (!deux_lus(t, n, &i, &mi)) return 0;
    if (i < n && (t[i++] != ':' || !deux_lus(t, n, &i, &s))) return 0;
    if (i != n || hh > 23 || mi > 59 || s > 59) return 0;
    if (!(j == 0 && m == 0 && a == 0) && (j < 1 || j > 31 || m < 1 || m > 12)) return 0;
    h->annee = j == 0 ? 0 : 1900u + a;
    h->mois = (unsigned char) m;
    h->jour = (unsigned char) j;
    h->heure = hh;
    h->minute = (unsigned char) mi;
    h->seconde = (unsigned char) s;
    return 1;
}

static int demarrage = 0;

void moniteur_fixer_demarrage(int actif)
{
    demarrage = actif;
}

/* Démarrage de LSE F4 (IMP-0279) : RETURN seul garde l'horloge de l'Apple IIGS ;
   une réponse incorrecte fait reposer la question. */
static void demarrer(void)
{
    InstantHorloge h;
    unsigned int n;

    for (;;) {
        message("LSE F4");
        a_la_ligne();
        ecrire("DATE HEURES MINUTES : ");
        n = lire_entree();
        if (fin_entree == SAISIE_XOFF) a_la_ligne();
        if (console_fin_entree()) return;
        if (fin_entree == SAISIE_ESC) continue;
        if (n == 0) break;
        if (date_saisie(entree, n, &h)) {
            horloge_fixer(&h);
            break;
        }
    }
    message("EN LIGNE....");
}

/* Bannière de LSE F4, après BONJOUR comme après AU REVOIR (IMP-0266) : son
   texte, puis cinq lignes blanches. La dernière ligne de texte annonce le
   compte de l'identification initiale de la console (IMP-0268) : la clé reste
   secrète. */
static void banniere_f4(void)
{
    Identification id;
    char m[40];
    unsigned int lg = 0;
    unsigned int i;

    if (configuration_session.profil != PROFIL_T1600_F4) return;
    message("CONSOLE NO. 01.");
    message("LSE F4");
    message("RUB-OUT REMPLACE CTRL X-OFF");
    if (identification_initiale != 0 && bibliotheque_identification_chercher(identification_initiale, &id)) {
        texte(m, &lg, "VOUS ETES SOUS ID ");
        champ(m, &lg, (unsigned long) (id.numero % 100), 0);
        ligne_finie(m, lg);
    }
    for (i = 0; i < 5; i++) a_la_ligne();
}

/* AU REVOIR : message, programme et identification oubliés, temporaires vidés. */
static void fin_de_session(const char *texte)
{
    ecrire(texte);
    ecrire_date(configuration_session.profil == PROFIL_MITRA15_V8 ? HEURE_BLANCS : DATE_BLANCS);
    a_la_ligne();
    banniere_f4();
    contexte_vider();
    programme_vider();
    pile_liberer(&pile);
    plateforme_espace_vider(ESPACE_TEMPORAIRES);           /* IMP-0245 */
    identification_nulle(0);
    taches_detruire();
    interprete_pas_a_pas(0);
}

void moniteur_session(void)
{
    int dormant = 1;
    unsigned int n;
    CategorieEntree k;
    Commande c;

    pile_initialiser(&pile);
    etat_programme = PROG_AUCUN;
    abrege = 0;
    silence = 0;
    console_echo(1);
    a_visee = 0;
    arret_pas = 0;
    lecture_interrompue = 0;
    interprete_pas_a_pas(0);
    interprete_reinitialiser();
    interprete_debut_de_ligne(1);
    if (demarrage && configuration_session.profil == PROFIL_T1600_F4) demarrer();
    for (;;) {
        n = lire_entree();
        if (console_fin_entree()) {                          /* AU REVOIR implicite (IMP-0240) */
            if (!dormant) fin_de_session("AU REVOIR");
            return;
        }
        if (fin_entree == SAISIE_ESC) {                      /* IMP-0239 */
            if (!dormant) message("PRET");
            continue;
        }
        k = analyseur_classer(entree, n, dormant, &c);
        if (dormant) {                                       /* points K6, M6 */
            if (k == ENTREE_COMMANDE && c != CMD_BONJOUR) {   /* initialisateur (IMP-0257) */
                commande(c);
                if (!interprete_en_debut_de_ligne()) a_la_ligne();
            } else if (k == ENTREE_COMMANDE) {
                ecrire(fin_entree == SAISIE_RETURN ? "BONJOUR" : "NJOUR");
                if (configuration_session.profil == PROFIL_MITRA15_V8) {   /* INRDP75 p. 189 (IMP-0266) */
                    a_la_ligne();
                    ecrire("LSE-MI15  CONSOLE NO.01");
                }
                ecrire_date(DATE_BLANCS);
                a_la_ligne();
                plateforme_espace_vider(ESPACE_TEMPORAIRES);   /* IMP-0245 */
                taches_creer();                                 /* IMP-0130, IMP-0269 */
                identification_nulle(1);
                banniere_f4();
                dormant = 0;
            } else if (k != ENTREE_VIDE || fin_entree == SAISIE_XOFF) {
                if (fin_entree == SAISIE_XOFF) a_la_ligne();
                erreur_dialogue(ERR_D_COMMANDE_ERRONEE);
            }
            continue;
        }
        if (correction(n)) {                                 /* éditeur de ligne (IMP-0267) */
            if (!interprete_en_debut_de_ligne()) a_la_ligne();
            continue;
        }
        if (k == ENTREE_COMMANDE) {
            if (c == CMD_AU_REVOIR) {
                fin_de_session(fin_entree == SAISIE_RETURN ? "AU REVOIR" : " REVOIR");
                return;
            }
            commande(c);
            continue;
        }
        switch (k) {
        case ENTREE_VIDE:                                    /* IMP-0238 */
            if (fin_entree != SAISIE_XOFF) break;
            if (arret_pas && etat_programme == PROG_SUSPENDU) continuer();
            else erreur_dialogue(ERR_D_COMMANDE_ERRONEE);
            break;
        case ENTREE_PROGRAMME:
            ligne_programme(n);
            break;
        case ENTREE_BUREAU:
            ligne_bureau(n);
            break;
        default:
            erreur_dialogue(ERR_D_COMMANDE_ERRONEE);
            break;
        }
        /* la ligne suivante est tapée en début de ligne, même après X-OFF
           (INRDP75 : chaque ligne de programme et chaque « ? » sur sa ligne) */
        if (!interprete_en_debut_de_ligne()) a_la_ligne();
    }
}
