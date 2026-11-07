/* sonde_pile_compilateur.c — essai d'audit G2 (2026-10-29), hors dépôt :
   profondeur de pile C consommée par compilateur_compiler selon
   l'imbrication syntaxique de la ligne. Méthode : la zone située sous le
   pointeur de pile est peinte d'un motif avant l'appel, puis l'on cherche
   l'octet le plus bas qui a été modifié (fonctions de bibliothèque et
   outils compris). Pile agrandie à 16 Kio pour que l'essai lui-même ne
   déborde pas. */

#ifndef SONDE_PILE_4K
#pragma stacksize 16384
#endif

#include "compilateur_outils.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

#define ZONE  12000u
#define MARGE 96u

static char texte[100];
static unsigned int profondeur_max = 0;

/* Profondeur (octets) consommée sous le pointeur de pile de l'appelant. */
static int garder = 0;   /* 1 : tables conservées (symboles déjà créés) */

static unsigned int mesurer(const char *t, int *accepte_)
{
    unsigned int s;
    unsigned int i;
    unsigned int n;
    unsigned char *b;
    unsigned char motif;
    unsigned int pire = 0;
    int passe;

    for (passe = 0; passe < 2; passe++) {
        motif = passe == 0 ? 0xA5 : 0x5A;
        if (!garder) symboles_vider();
        asm {
            tsc
            sta s
        }
        b = (unsigned char *) (unsigned long) (s - ZONE);
        for (i = 0; i < ZONE - MARGE; i++) b[i] = motif;
        *accepte_ = compiler(t);
        for (i = 0; i < ZONE - MARGE && b[i] == motif; i++)
            ;
        n = ZONE - i;
        if (n > pire) pire = n;
        if (*accepte_) recuperer();
    }
    return pire;
}

static void cas(const char *libelle)
{
    int ok;
    unsigned int p = mesurer(texte, &ok);
    if (p > profondeur_max) profondeur_max = p;
    printf("%-28s lg %2u %s C%3u pile %5u  [%s]\n", libelle, (unsigned int) strlen(texte),
           ok ? "OK " : "REJ", ok ? 0 : erreurs_numero(d.nature), p, texte);
}

static void parentheses(const char *tete, unsigned int n, const char *coeur)
{
    unsigned int i;
    strcpy(texte, tete);
    for (i = 0; i < n; i++) strcat(texte, "(");
    strcat(texte, coeur);
    for (i = 0; i < n; i++) strcat(texte, ")");
}

int main(void)
{
    unsigned int n;
    unsigned int i;
    char lib[40];

    VERIF_DEBUT("sonde pile compilateur (audit G2)");

    strcpy(texte, "A_1"); cas("bureau A_1");
    strcpy(texte, "A_B"); cas("bureau A_B (tables videes)");
    garder = 1;
    strcpy(texte, "A_B"); cas("bureau A_B (symboles connus)");
    strcpy(texte, "A_1"); cas("bureau A_1 (symboles connus)");
    strcpy(texte, "A_(B)"); cas("bureau A_(B) (connus)");
    strcpy(texte, "A_((B))"); cas("bureau A_((B)) (connus)");
    strcpy(texte, "?B"); cas("bureau ?B (connus)");
    strcpy(texte, "10 PROCEDURE &P(A,B) LOCAL C"); cas("declaration (connus)");
    strcpy(texte, "1 AFFICHER 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"); cas("chaine partagee");
    garder = 0;
    strcpy(texte, "10 A_1"); cas("programme 10 A_1");
    strcpy(texte, "?1"); cas("bureau ?1");
    strcpy(texte, "10 AFFICHER 1"); cas("programme AFFICHER 1");

    /* parenthèses d'expression : 7 niveaux d'appel par paire */
    for (n = 1; n <= 38; n = n < 4 ? n + 1 : n + 4) {
        parentheses("A_", n, "1");
        sprintf(lib, "A_ + %u parentheses", n);
        cas(lib);
    }
    parentheses("A_", 38, "1"); cas("A_ + 38 parentheses");
    /* même imbrication sous ? (AFFICHER : grand cadre corps_afficher) */
    for (n = 1; n <= 38; n = n < 4 ? n + 1 : n + 4) {
        parentheses("?", n, "1");
        sprintf(lib, "? + %u parentheses", n);
        cas(lib);
    }
    parentheses("?", 38, "1"); cas("? + 38 parentheses");
    /* condition entre parenthèses (facteur_logique, retour en arrière) */
    for (n = 4; n <= 30; n += 6) {
        strcpy(texte, "SI ");
        for (i = 0; i < n; i++) strcat(texte, "(");
        strcat(texte, "A=1");
        for (i = 0; i < n; i++) strcat(texte, ")");
        strcat(texte, " ALORS B_1");
        sprintf(lib, "SI + %u parentheses", n);
        cas(lib);
    }
    /* indices imbriqués A[A[A[...]]] */
    for (n = 4; n <= 24; n += 5) {
        strcpy(texte, "B_");
        for (i = 0; i < n; i++) strcat(texte, "A[");
        strcat(texte, "1");
        for (i = 0; i < n; i++) strcat(texte, "]");
        sprintf(lib, "indices %u", n);
        cas(lib);
    }
    /* fonctions imbriquées */
    for (n = 3; n <= 15; n += 4) {
        strcpy(texte, "B_");
        for (i = 0; i < n; i++) strcat(texte, "ABS(");
        strcat(texte, "1");
        for (i = 0; i < n; i++) strcat(texte, ")");
        sprintf(lib, "ABS imbriques %u", n);
        cas(lib);
    }
    /* DEBUT imbriqués */
    strcpy(texte, "1 ");
    for (i = 0; i < 7; i++) strcat(texte, "DEBUT ");
    strcat(texte, "A_1");
    for (i = 0; i < 7; i++) strcat(texte, " FIN");
    cas("DEBUT x7");
    /* SI ALORS imbriqués */
    strcpy(texte, "1 ");
    for (i = 0; i < 6; i++) strcat(texte, "SI A=1 ALORS ");
    strcat(texte, "B_1");
    cas("SI ALORS x6");

    printf("profondeur maximale observee : %u octets\n", profondeur_max);
    VERIFIER(profondeur_max > 0);
    VERIF_FIN();
}
