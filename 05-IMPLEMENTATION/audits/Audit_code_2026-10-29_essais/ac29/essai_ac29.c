/* essai_ac29.c — AC-29 : errno posé par fopen, fwrite et fflush d'ORCA/C
   Sans argument : ouvertures (absent, répertoire absent, fichier protégé).
   Avec un argument (chemin d'un répertoire sur un petit volume) : écriture
   jusqu'à remplir le volume. */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <gsos.h>
#include <orca.h>

static GSString255 g;

static GSString255 *gs(const char *c)
{
    g.length = (Word) strlen(c);
    memcpy(g.text, c, g.length);
    return &g;
}

static void info(const char *c)
{
    FileInfoRecGS fi;
    fi.pCount = 2;
    fi.pathname = gs(c);
    GetFileInfoGS(&fi);
    printf("  GetFileInfo %s : toolerror $%04X, access $%04X\n", c, toolerror(), fi.access);
}

static int acces(const char *c, Word a)
{
    FileInfoRecGS fi;
    fi.pCount = 2;
    fi.pathname = gs(c);
    fi.access = a;
    SetFileInfoGS(&fi);
    return toolerror();
}

static void ouvrir(const char *c, const char *mode)
{
    FILE *f;
    errno = 0;
    f = fopen(c, mode);
    printf("fopen(%s, %s) : %s, errno %d, toolerror $%04X\n", c, mode, f == NULL ? "NULL" : "ouvert", errno, toolerror());
    info(c);
    if (f != NULL) fclose(f);
}

static void contenu(const char *c)
{
    FILE *f;
    char t[8];
    size_t n;
    f = fopen(c, "rb");
    if (f == NULL) { printf("  contenu : illisible, errno %d\n", errno); return; }
    n = fread(t, 1, sizeof t, f);
    fclose(f);
    printf("  contenu : %u octet(s)\n", (unsigned) n);
}

static void remplir(const char *rep)
{
    static char bloc[4096];
    char c[200];
    FILE *f;
    unsigned long total = 0;
    size_t n;
    int r;

    strcpy(c, rep);
    strcat(c, ":PLEIN");
    memset(bloc, 0x55, sizeof bloc);
    errno = 0;
    f = fopen(c, "w+b");
    if (f == NULL) { printf("fopen PLEIN : errno %d\n", errno); return; }
    for (;;) {
        errno = 0;
        n = fwrite(bloc, 1, sizeof bloc, f);
        total += n;
        if (n != sizeof bloc) {
            printf("fwrite : %u sur %u, errno %d, toolerror $%04X, ferror %d, total %lu\n",
                   (unsigned) n, (unsigned) sizeof bloc, errno, toolerror(), ferror(f), total);
            break;
        }
        errno = 0;
        r = fflush(f);
        if (r != 0) {
            printf("fflush : %d, errno %d, toolerror $%04X, ferror %d, total %lu\n",
                   r, errno, toolerror(), ferror(f), total);
            break;
        }
        if (total > 8000000ul) { printf("aucun échec avant 8 Mo\n"); break; }
    }
    fclose(f);
    remove(c);
}

int main(int argc, char *argv[])
{
    FILE *f;

    if (argc > 1 && strcmp(argv[1], "H") == 0) {
        info("ESSAI29:HOTE");
        ouvrir("ESSAI29:HOTE", "r+b");
        contenu("ESSAI29:HOTE");
        ouvrir("ESSAI29:HOTE", "w+b");
        contenu("ESSAI29:HOTE");
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "D") == 0) {
        ouvrir("ESSAI29:REP", "rb");
        ouvrir("ESSAI29:REP", "r+b");
        errno = 0;
        printf("remove(ESSAI29:REP) non vide : %d, errno %d\n", remove("ESSAI29:REP"), errno);
        info("ESSAI29:REP");
        errno = 0;
        printf("remove(ESSAI29:ABSENT) : %d, errno %d\n", remove("ESSAI29:ABSENT"), errno);
        info("ESSAI29:ABSENT");
        return 0;
    }
    if (argc > 1) { remplir(argv[1]); return 0; }

    printf("ENOENT %d EIO %d EACCES %d ENOSPC %d EEXIST %d\n", ENOENT, EIO, EACCES, ENOSPC, EEXIST);
    ouvrir("ESSAI29:ABSENT", "rb");
    ouvrir("ESSAI29:ABSENT", "r+b");
    ouvrir("NOREP29:ABSENT", "r+b");
    ouvrir("NOREP29:ABSENT", "w+b");

    f = fopen("ESSAI29:PROTEGE", "w+b");
    if (f == NULL) { printf("création impossible, errno %d\n", errno); return 1; }
    fwrite("ABCDEF", 1, 6, f);
    fclose(f);
    printf("SetFileInfo lecture seule : $%04X\n", acces("ESSAI29:PROTEGE", 0x0001));
    info("ESSAI29:PROTEGE");
    ouvrir("ESSAI29:PROTEGE", "rb");
    ouvrir("ESSAI29:PROTEGE", "r+b");
    contenu("ESSAI29:PROTEGE");
    ouvrir("ESSAI29:PROTEGE", "w+b");
    contenu("ESSAI29:PROTEGE");
    acces("ESSAI29:PROTEGE", 0x00C3);
    remove("ESSAI29:PROTEGE");
    return 0;
}
