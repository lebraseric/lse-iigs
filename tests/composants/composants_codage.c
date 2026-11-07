/* composants_codage.c — DECODER et ENCODER : format des enregistrements
   EXI: EXI-0057  VM: VM-3404, VM-2203
   IMP: IMP-0259, IMP-0260, IMP-0261 ; points G1 à G3 (étape 7e1)
   Spécification des tests : chapitre Bibliothécaire et fichiers §7 quinquies ;
   PG66 et PG66 bis d'INRDP75 (5e partie, §4-1) reproduits à l'octet près.
   Niveau : composants, sur fichiers réels (bibliothèque tests:BIBCOD). */

#include "compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/fichiers.h"
#include "../../src/noyau/codage.h"
#include "../../src/interfaces/interface_fichiers.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

/* PG66 (FLIT), « ← » écrit « _ » */
static const char *const flit[] = {
    "1 AFFICHER 'NOM ';CHAINE NOM;LIRE NOM",
    "2 AFFICHER 'NO ';LIRE I;LIBERER Z;CHARGER Z,I,NOM,X;AFFICHER 'LONG. ',LGR(Z)",
    "3 SI X=-1 ALORS DEBUT AFFICHER 'ENR. VIDE';ALLER EN 2 FIN",
    "4 P_POS(Z,1,EQC(10));SI P=0 OU P>72 ALORS P_72",
    "5 AFFICHER SCH(Z,1,P,J);SI J>LGR(Z) ALORS ALLER EN 2",
    "6 Z_SCH(Z,P+1,'');ALLER EN 4",
    NULL };

static unsigned char attendu[600];
static unsigned int  attendu_lg;

static void entrer(const char *const *lignes)
{
    unsigned int i;
    NatureErreur n;
    programme_vider();
    for (i = 0; lignes[i] != NULL; i++)
        if (!compiler(lignes[i]) || !programme_inserer(&r.resultat.ligne_programme, &n))
            printf("  [%s] refusee\n", lignes[i]);
}

/* Flot attendu : chaque ligne suivie de X-OFF, puis NUL. */
static void flot(const char *const *lignes)
{
    unsigned int i;
    attendu_lg = 0;
    for (i = 0; lignes[i] != NULL; i++) {
        memcpy(attendu + attendu_lg, lignes[i], strlen(lignes[i]));
        attendu_lg += (unsigned int) strlen(lignes[i]);
        attendu[attendu_lg++] = 0x13;
    }
    attendu[attendu_lg++] = 0x00;
}

/* L'enregistrement k du fichier temporaire vaut-il o[0..n-1] ? */
static int enregistrement(const char *nom, unsigned int k, const unsigned char *o, unsigned int n)
{
    unsigned char type;
    unsigned char *c;
    unsigned long lg;
    int bon;
    if (fichiers_charger(nom, 0, k, &type, &c, &lg) != ERR_AUCUNE) {
        printf("  enregistrement %u absent\n", k);
        return 0;
    }
    bon = type == ENREG_CHAINE && lg == n && memcmp(c, o, n) == 0;
    if (!bon) printf("  enregistrement %u : type %u, longueur %lu au lieu de %u\n", k, type, lg, n);
    free(c);
    return bon;
}

static int absent(const char *nom, unsigned int k)
{
    unsigned char type;
    unsigned char *c;
    unsigned long lg;
    if (fichiers_charger(nom, 0, k, &type, &c, &lg) == ERR_E_ENREGISTREMENT_INEXISTANT) return 1;
    free(c);
    return 0;
}

/* ENCODER lit-il exactement les lignes données ? */
static int relu(const char *nom, const char *const *lignes)
{
    char t[300];
    unsigned int n;
    unsigned int i = 0;
    NatureErreur e;
    if (codage_ouvrir(nom, 0) != ERR_AUCUNE) return 0;
    while (codage_ligne(t, sizeof t, &n, &e)) {
        if (lignes[i] == NULL || n != strlen(lignes[i]) || memcmp(t, lignes[i], n) != 0) {
            printf("  ligne %u : [%.*s]\n", i, (int) n, t);
            codage_fermer();
            return 0;
        }
        i++;
    }
    codage_fermer();
    return e == ERR_AUCUNE && lignes[i] == NULL;
}

int main(void)
{
    const char *deux_trois[3];
    static const char *const cr[] = { "1 A_1", "2 B_2", NULL };
    unsigned char rec2[252];
    float v = 1.0f;
    NatureErreur e;

    deux_trois[0] = flit[1];
    deux_trois[1] = flit[2];
    deux_trois[2] = NULL;
    VERIF_DEBUT("composants/codage");
    programme_initialiser();
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBCOD"));
    plateforme_espace_vider(ESPACE_TEMPORAIRES);
    plateforme_espace_vider(ESPACE_DONNEES);
    entrer(flit);
    flot(flit);
    VERIFIER_EGAL(303u, attendu_lg);                   /* 220 + 53 + 29 + NUL */

    /* --- MITRA 15 (PG66 bis) : 252 caractères, fin du tampon précédent ------------ */
    VERIFIER_EGAL(ERR_AUCUNE, codage_decoder("ESSAI", 1, 0, 0));
    VERIFIER(enregistrement("ESSAI", 1, attendu, 252));
    memcpy(rec2, attendu + 252, 51);                   /* « (Z) ALORS ALLER EN 2 » ... X-OFF NUL */
    memcpy(rec2 + 51, attendu + 51, 252 - 51);         /* « O ';LIRE I;... » comme dans INRDP75 */
    VERIFIER(enregistrement("ESSAI", 2, rec2, 252));
    VERIFIER(absent("ESSAI", 3));
    VERIFIER(relu("ESSAI", flit));

    /* --- T1600 (PG66) : lignes entières, 220 puis 82 caractères -------------------- */
    configuration_session.profil = PROFIL_T1600_V4;
    VERIFIER_EGAL(ERR_AUCUNE, codage_decoder("ESSAI", 1, 0, 0));
    VERIFIER(enregistrement("ESSAI", 1, attendu, 220));
    memcpy(rec2, attendu + 220, 81);
    rec2[81] = 0x00;                                   /* NUL au lieu du dernier X-OFF (G1) */
    VERIFIER(enregistrement("ESSAI", 2, rec2, 82));
    VERIFIER(absent("ESSAI", 3));
    VERIFIER(relu("ESSAI", flit));
    VERIFIER_EGAL(ERR_AUCUNE, codage_decoder("PART", 2, 3, 0));   /* n1, n2 */
    VERIFIER(relu("PART", deux_trois));
    configuration_session.profil = PROFIL_T1600_F4;            /* IMP-0276 */
    VERIFIER_EGAL(ERR_AUCUNE, codage_decoder("ESSAI", 1, 0, 0));
    VERIFIER(enregistrement("ESSAI", 1, attendu, 220));
    VERIFIER(enregistrement("ESSAI", 2, rec2, 82));
    configuration_session.profil = PROFIL_MITRA15_V8;
    VERIFIER_EGAL(ERR_AUCUNE, codage_decoder("PART", 2, 3, 0));
    VERIFIER(relu("PART", deux_trois));

    /* --- programme vide : le seul NUL ------------------------------------------------ */
    programme_vider();
    VERIFIER_EGAL(ERR_AUCUNE, codage_decoder("VIDE", 1, 0, 0));
    VERIFIER(relu("VIDE", cr + 2));

    /* --- ENCODER : CR et LF admis, fin sans NUL ; erreurs ----------------------------- */
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_garer("CR", 0, 1, ENREG_CHAINE, (const unsigned char *) "1 A_1\r\n2 B_2\r", 13));
    VERIFIER(relu("CR", cr));
    VERIFIER_EGAL(ERR_F_FICHIER_INEXISTANT, codage_ouvrir("ZZZ", 0));
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_garer("NB", 0, 1, ENREG_NOMBRE, (const unsigned char *) &v, 4));
    VERIFIER_EGAL(ERR_F_FICHIER_ENDOMMAGE, codage_ouvrir("NB", 0));
    e = codage_ouvrir("ESSAI", 0);
    VERIFIER_EGAL(ERR_AUCUNE, e);
    codage_fermer();

    VERIF_FIN();
}
