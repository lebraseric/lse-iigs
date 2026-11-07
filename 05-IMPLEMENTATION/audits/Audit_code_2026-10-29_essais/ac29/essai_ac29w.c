/* essai_ac29w.c — AC-29 : code GS/OS rendu par WriteGS sur un volume plein */
#include <stdio.h>
#include <string.h>
#include <gsos.h>
#include <orca.h>

static GSString255 g;
static char bloc[4096];

int main(int argc, char *argv[])
{
    CreateRecGS cr;
    OpenRecGS op;
    IORecGS io;
    RefNumRecGS rf;
    NameRecGS ds;
    unsigned long total = 0;

    if (argc < 2) return 1;
    g.length = (Word) strlen(argv[1]);
    memcpy(g.text, argv[1], g.length);
    cr.pCount = 5; cr.pathname = &g; cr.access = 0xC3; cr.fileType = 6; cr.auxType = 0; cr.storageType = 1;
    CreateGS(&cr);
    printf("Create $%04X\n", toolerror());
    op.pCount = 3; op.pathname = &g; op.requestAccess = 3;
    OpenGS(&op);
    printf("Open $%04X\n", toolerror());
    if (toolerror() != 0) return 1;
    io.pCount = 4; io.refNum = op.refNum; io.dataBuffer = bloc; io.requestCount = sizeof bloc;
    for (;;) {
        WriteGS(&io);
        total += io.transferCount;
        if (toolerror() != 0 || io.transferCount != sizeof bloc) {
            printf("Write $%04X, transfert %lu, total %lu\n", toolerror(), io.transferCount, total);
            break;
        }
        if (total > 8000000ul) break;
    }
    rf.pCount = 1; rf.refNum = op.refNum;
    CloseGS(&rf);
    ds.pCount = 1; ds.pathname = &g;
    DestroyGS(&ds);
    return 0;
}
