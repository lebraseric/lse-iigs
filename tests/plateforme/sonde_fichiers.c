/* sonde_fichiers.c — sonde des opérations de fichier (étape 7a, A5)
   Niveau : sur cible et Golden Gate. Crée un répertoire, y écrit un fichier,
   le modifie sur place (r+b, fseek), le renomme, liste le répertoire, puis
   efface tout. Chaque étape affiche son résultat.
   « SONDEF V » interroge en outre le volume de la bibliothèque (@:LSE) :
   Volume par nom de volume, GetFileInfo du répertoire de volume, puis DInfo
   et Volume pour chaque périphérique (IMP-0257). Sur cible seulement :
   Golden Gate s'arrête à l'appel Volume.
   « SONDEF G [chemin] » ne fait que l'essai des périphériques (2026-11-06) :
   le parcours de plateforme_secteurs_libres fait tourner les lecteurs 5,25"
   au premier UTILISATION DISQUE d'une session. La sonde dresse la liste des
   périphériques par DInfo seul, puis essaie GetDevNumber ($2020) sur le nom
   du volume de la bibliothèque (défaut @:LSE), avec et sans « : », et sur le
   chemin complet ; elle appelle ensuite DInfo et Volume sur le numéro obtenu,
   enfin GetDevNumber sur un volume absent. Chaque étape est annoncée avant
   l'appel et chronométrée (clock()), pour rapprocher l'activité des lecteurs
   de l'étape qui la cause. Sur cible seulement (appel Volume). */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <gsos.h>
#include <orca.h>

static GSString255 chemin;

static GSString255 *gs(const char *c)
{
    chemin.length = (Word) strlen(c);
    memcpy(chemin.text, c, chemin.length);
    return &chemin;
}

static void sonde_volume(void)
{
    ExpandPathRecGS ex;
    DInfoRecGS di;
    VolumeRecGS vo;
    FileInfoRecGS fi;
    static ResultBuf255 complet;
    static ResultBuf255 vn;
    static ResultBuf32 dn;
    static GSString255 vol;
    Word e;
    Word d;
    unsigned int i;

    printf("SONDE VOLUME\n");
    ex.pCount = 2;
    ex.inputPath = gs("@:LSE");
    complet.bufSize = sizeof complet;
    complet.bufString.length = 0;
    ex.outputPath = &complet;
    ExpandPathGS(&ex);
    e = toolerror();
    printf("  ExpandPath @:LSE : erreur $%04X, %.*s\n", e, (int) complet.bufString.length,
           complet.bufString.text);
    for (i = 1; i < complet.bufString.length && complet.bufString.text[i] != ':'; i++) ;
    vol.length = (Word) i;
    memcpy(vol.text, complet.bufString.text, i);
    printf("  volume : %.*s\n", (int) vol.length, vol.text);

    memset(&vo, 0, sizeof vo);
    vn.bufSize = sizeof vn;
    vo.pCount = 6;
    vo.devName = (GSString32Ptr) &vol;
    vo.volName = &vn;
    VolumeGS(&vo);
    e = toolerror();
    printf("  Volume(volume) : erreur $%04X, total %lu, libres %lu, bloc %u\n", e,
           vo.totalBlocks, vo.freeBlocks, vo.blockSize);

    memset(&fi, 0, sizeof fi);
    fi.pCount = 10;
    fi.pathname = &vol;
    fi.optionList = NULL;
    GetFileInfoGS(&fi);
    e = toolerror();
    printf("  GetFileInfo(volume) : erreur $%04X, auxType %lu, blocksUsed %lu\n", e,
           fi.auxType, fi.blocksUsed);

    for (d = 1; d <= 32; d++) {
        di.pCount = 2;
        di.devNum = d;
        dn.bufSize = sizeof dn;
        dn.bufString.length = 0;
        di.devName = &dn;
        DInfoGS(&di);
        if ((e = toolerror()) != 0) {
            printf("  DInfo %u : erreur $%04X, fin\n", d, e);
            break;
        }
        memset(&vo, 0, sizeof vo);
        vn.bufSize = sizeof vn;
        vn.bufString.length = 0;
        vo.pCount = 6;
        vo.devName = &dn.bufString;
        vo.volName = &vn;
        VolumeGS(&vo);
        e = toolerror();
        printf("  %u %.*s : erreur $%04X, %.*s, total %lu, libres %lu, bloc %u\n", d,
               (int) dn.bufString.length, dn.bufString.text, e,
               (int) vn.bufString.length, vn.bufString.text,
               vo.totalBlocks, vo.freeBlocks, vo.blockSize);
    }
    printf("FIN SONDE VOLUME\n");
}

static clock_t t0;

/* Annonce l'étape avant l'appel, l'écran étant à jour (lecteurs observés). */
static void annoncer(const char *etape, const char *nom)
{
    printf("  %s %s ...\n", etape, nom);
    fflush(stdout);
    t0 = clock();
}

static void duree(void)
{
    clock_t t = clock() - t0;

    printf("    duree : %lu tops, %lu ms\n", (unsigned long) t,
           (unsigned long) t * 1000ul / (unsigned long) CLOCKS_PER_SEC);
}

/* GetDevNumber sur un nom C ; rend le numéro, 0 en cas d'erreur. */
static Word numero_par_nom(const char *nom)
{
    DevNumRecGS dn;
    Word e;

    annoncer("GetDevNumber", nom);
    dn.pCount = 2;
    dn.devName = (GSString32Ptr) gs(nom);
    dn.devNum = 0;
    GetDevNumberGS(&dn);
    e = toolerror();
    duree();
    printf("    erreur $%04X, numero %u\n", e, dn.devNum);
    return e == 0 ? dn.devNum : 0;
}

static void sonde_peripherique(const char *bib)
{
    ExpandPathRecGS ex;
    DInfoRecGS di;
    VolumeRecGS vo;
    static ResultBuf255 complet;
    static ResultBuf255 vn;
    static ResultBuf32 dn;
    static char complet_c[256];
    static char avec[32];
    static char sans[32];
    Word e;
    Word d;
    Word n;
    unsigned int i;

    printf("SONDE PERIPHERIQUES\n");
    ex.pCount = 2;
    ex.inputPath = gs(bib);
    complet.bufSize = sizeof complet;
    complet.bufString.length = 0;
    ex.outputPath = &complet;
    ExpandPathGS(&ex);
    e = toolerror();
    printf("  ExpandPath %s : erreur $%04X, %.*s\n", bib, e, (int) complet.bufString.length,
           complet.bufString.text);
    if (e != 0) return;
    memcpy(complet_c, complet.bufString.text, complet.bufString.length);
    complet_c[complet.bufString.length] = '\0';
    for (i = 1; i < complet.bufString.length && i < 31 && complet_c[i] != ':'; i++) ;
    memcpy(avec, complet_c, i);
    avec[i] = '\0';                          /* « :VOL » */
    strcpy(sans, avec + 1);                  /* « VOL » */

    /* 1. Liste des périphériques par DInfo seul (aucun accès au support
       attendu) : nom, caractéristiques, blocs, emplacement, unité, type. */
    annoncer("DInfo", "1 a 32");
    for (d = 1; d <= 32; d++) {
        memset(&di, 0, sizeof di);
        di.pCount = 8;
        di.devNum = d;
        dn.bufSize = sizeof dn;
        dn.bufString.length = 0;
        di.devName = &dn;
        DInfoGS(&di);
        if ((e = toolerror()) != 0) {
            printf("    %u : erreur $%04X, fin\n", d, e);
            break;
        }
        printf("    %u %.*s car $%04X blocs %lu empl %u unite %u type $%04X\n", d,
               (int) dn.bufString.length, dn.bufString.text, di.characteristics,
               di.totalBlocks, di.slotNum, di.unitNum, di.deviceID);
    }
    duree();

    /* 2. GetDevNumber sur le volume de la bibliothèque. */
    n = numero_par_nom(avec);
    d = numero_par_nom(sans);
    if (n == 0) n = d;
    numero_par_nom(complet_c);

    /* 3. DInfo et Volume sur le seul périphérique trouvé : ce que ferait
       plateforme_secteurs_libres. */
    if (n != 0) {
        memset(&vo, 0, sizeof vo);
        vn.bufSize = sizeof vn;
        vn.bufString.length = 0;
        annoncer("DInfo et Volume", "sur le numero obtenu");
        di.pCount = 2;
        di.devNum = n;
        dn.bufSize = sizeof dn;
        dn.bufString.length = 0;
        di.devName = &dn;
        DInfoGS(&di);
        e = toolerror();
        if (e == 0) {
            vo.pCount = 6;
            vo.devName = &dn.bufString;
            vo.volName = &vn;
            VolumeGS(&vo);
            e = toolerror();
        }
        duree();
        printf("    %u %.*s : erreur $%04X, %.*s, total %lu, libres %lu, bloc %u\n", n,
               (int) dn.bufString.length, dn.bufString.text, e,
               (int) vn.bufString.length, vn.bufString.text,
               vo.totalBlocks, vo.freeBlocks, vo.blockSize);
        if (e == 0)
            printf("    secteurs libres : %lu\n",
                   vo.freeBlocks * (unsigned long) vo.blockSize / 256ul);
    }

    /* 4. Volume absent : les lecteurs peuvent tourner. */
    numero_par_nom(":ZZSONDEABSENT");
    printf("FIN SONDE PERIPHERIQUES\n");
}

int main(int argc, char *argv[])
{
    CreateRecGS cr;
    OpenRecGS op;
    DirEntryRecGS de;
    RefNumRecGS rf;
    NameRecGS nr;
    ResultBuf255 nom;
    FILE *f;
    unsigned char bloc[512];
    unsigned char lu[16];
    long taille;
    int i;

    if (argc > 1 && (argv[1][0] == 'G' || argv[1][0] == 'g')) {
        sonde_peripherique(argc > 2 ? argv[2] : "@:LSE");
        return 0;
    }
    printf("SONDE FICHIERS\n");
    cr.pCount = 5;
    cr.pathname = gs("SONDE.REP");
    cr.access = 0xC3;
    cr.fileType = 0x0F;                       /* répertoire */
    cr.auxType = 0;
    cr.storageType = 0x0D;
    CreateGS(&cr);
    printf("  CreateGS repertoire : erreur $%04X\n", toolerror());

    for (i = 0; i < 512; i++) bloc[i] = (unsigned char) i;
    f = fopen("SONDE.REP:F1", "wb");
    printf("  fopen wb : %s\n", f != NULL ? "ok" : "ECHEC");
    if (f != NULL) {
        printf("  fwrite 512 : %u\n", (unsigned int) fwrite(bloc, 1, 512, f));
        fclose(f);
    }
    f = fopen("SONDE.REP:F1", "r+b");
    printf("  fopen r+b : %s\n", f != NULL ? "ok" : "ECHEC");
    if (f != NULL) {
        memset(lu, 0xAA, 10);
        printf("  fseek 256 : %d\n", fseek(f, 256L, SEEK_SET));
        printf("  fwrite 10 : %u\n", (unsigned int) fwrite(lu, 1, 10, f));
        fseek(f, 0L, SEEK_END);
        taille = ftell(f);
        printf("  taille : %ld\n", taille);
        fseek(f, 250L, SEEK_SET);
        fread(lu, 1, 16, f);
        printf("  octets 250..265 : %02X %02X ... %02X %02X\n", lu[0], lu[5], lu[6], lu[15]);
        fseek(f, 512L, SEEK_SET);
        printf("  fwrite en fin 4 : %u\n", (unsigned int) fwrite(lu, 1, 4, f));
        fseek(f, 0L, SEEK_END);
        printf("  taille apres ajout : %ld\n", ftell(f));
        fclose(f);
    }
    printf("  rename : %d\n", rename("SONDE.REP:F1", "SONDE.REP:F2"));

    op.pCount = 3;
    op.pathname = gs("SONDE.REP");
    op.requestAccess = 1;
    OpenGS(&op);
    printf("  OpenGS repertoire : erreur $%04X\n", toolerror());
    nom.bufSize = sizeof nom;
    f = fopen("SONDE.REP:F3", "wb");
    if (f != NULL) fclose(f);
    for (i = 1; i < 5; i++) {
        de.pCount = 7;
        de.refNum = op.refNum;
        de.flags = 0;
        de.base = 1;                          /* déplacement relatif */
        de.displacement = 1;
        de.name = &nom;
        GetDirEntryGS(&de);
        if (toolerror() != 0) {
            printf("  GetDirEntryGS : fin, erreur $%04X\n", toolerror());
            break;
        }
        printf("  entree : %.*s, type $%02X\n", (int) nom.bufString.length, nom.bufString.text,
               de.fileType);
    }
    rf.pCount = 1;
    rf.refNum = op.refNum;
    CloseGS(&rf);

    printf("  remove : %d\n", remove("SONDE.REP:F2"));
    remove("SONDE.REP:F3");
    nr.pCount = 1;
    nr.pathname = gs("SONDE.REP");
    DestroyGS(&nr);
    printf("  DestroyGS repertoire : erreur $%04X\n", toolerror());
    f = fopen("SONDE.REP:F2", "rb");
    printf("  fichier absent apres remove : %s\n", f == NULL ? "oui" : "NON");
    printf("FIN SONDE\n");
    if (argc > 1 && (argv[1][0] == 'V' || argv[1][0] == 'v')) sonde_volume();
    return 0;
}
