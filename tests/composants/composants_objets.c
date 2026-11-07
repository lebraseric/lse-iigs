/* composants_objets.c — gestionnaire d'objets, en boîte noire par l'interface §11
   EXI: EXI-0050  VM: VM-1601, VM-1602  IV: ANN-K, RCI-M §3
   IMP: IMP-0157 à IMP-0163, IMP-0194, IMP-0243 (copie et écriture sans
        verrouillage), IMP-0270, IMP-0271
   Spécification des tests : Gestionnaire d'objets §17.
   Niveau : composants. Liée avec src/noyau/objets.c et
   src/plateforme/memoire_iigs.c (aucun bouchon).

   Non couverts ici :
   - objets_capacite_disponible : FreeMem n'existe pas sous Golden Gate
     (IMP-0192) ; à exécuter sur cible ;
   - violation volontaire de la discipline de verrouillage (§17, dernier
     test) : documentaire, à ne pas exécuter hors d'un banc contrôlé. */

#include <string.h>
#include "../commun/verif.h"
#include "../../src/interfaces/profil.h"
#include "../../src/noyau/objets.h"

#define NB_MAX 256

static HandleObjet h[NB_MAX];

/* Remplit un objet d'un motif propre à son numéro. */
static void remplir(HandleObjet x, unsigned int longueur, unsigned char motif)
{
    unsigned char *p = objets_resoudre(x);
    memset(p, motif, longueur);
    objets_relacher(x);
}

/* Vrai si tout l'objet porte le motif attendu. */
static int intact(HandleObjet x, unsigned int longueur, unsigned char motif)
{
    unsigned char *p = objets_resoudre(x);
    unsigned int i;
    int ok = 1;
    for (i = 0; i < longueur; i++)
        if (p[i] != motif) { ok = 0; break; }
    objets_relacher(x);
    return ok;
}

static unsigned long adresse(HandleObjet x)
{
    unsigned long a = (unsigned long) objets_resoudre(x);
    objets_relacher(x);
    return a;
}

static void tout_liberer(unsigned int n)
{
    unsigned int i;
    for (i = 0; i < n; i++)
        objets_liberer(h[i]);
}

int main(void)
{
    unsigned int i;
    int tous_meme_bloc;
    int tous_valides;
    unsigned char b0;
    unsigned long avant_c;
    HandleObjet a, bb, c, d, w;

    VERIF_DEBUT("composants/objets");

    /* 1. 64 objets dans un bloc ; le 65e ouvre un second bloc (IMP-0158, IMP-0159) */
    tous_meme_bloc = 1;
    for (i = 0; i < 65; i++)
        h[i] = objets_creer(32);
    b0 = GESTOBJ_BLOC(h[0]);
    for (i = 1; i < 64; i++)
        if (GESTOBJ_BLOC(h[i]) != b0) tous_meme_bloc = 0;
    VERIFIER(tous_meme_bloc);
    VERIFIER(GESTOBJ_BLOC(h[64]) != b0);
    VERIFIER(GESTOBJ_BLOC(h[64]) != GESTOBJ_INDICE_HORS_NORME);
    tout_liberer(65);

    /* 2. Blocs vides rendus, indice 0 réutilisé (IMP-0162, IMP-0163) */
    a = objets_creer(10);
    VERIFIER_EGAL(0, GESTOBJ_BLOC(a));
    objets_liberer(a);

    /* 3. Libération en ordre inverse : place récupérée au bord (§7) */
    a = objets_creer(1000); remplir(a, 1000, 0xA1);
    bb = objets_creer(1000); remplir(bb, 1000, 0xB2);
    c = objets_creer(1000); remplir(c, 1000, 0xC3);
    objets_liberer(c);
    objets_liberer(bb);
    w = objets_creer(GESTOBJ_SEUIL_HORS_NORME - 1000);  /* 2833 : tout le reste du bloc */
    VERIFIER_EGAL(GESTOBJ_BLOC(a), GESTOBJ_BLOC(w));
    VERIFIER(intact(a, 1000, 0xA1));
    objets_liberer(w);
    objets_liberer(a);

    /* 4. Compaction : A, B, C ; libérer B ; D plus grand que le trou (IMP-0160, RCI-M §3) */
    a = objets_creer(1000); remplir(a, 1000, 0xA1);
    bb = objets_creer(1000); remplir(bb, 1000, 0xB2);
    c = objets_creer(1000); remplir(c, 1000, 0xC3);
    avant_c = adresse(c);
    objets_liberer(bb);
    d = objets_creer(1500);   /* 833 contigus + 1000 de trou */
    VERIFIER_EGAL(GESTOBJ_BLOC(a), GESTOBJ_BLOC(d));
    VERIFIER(objets_existe(a));
    VERIFIER(objets_existe(c));
    VERIFIER(intact(a, 1000, 0xA1));
    VERIFIER(intact(c, 1000, 0xC3));
    VERIFIER(adresse(c) != avant_c);
    VERIFIER_EGAL(adresse(a), adresse(c) + 1000);   /* A et C contigus */
    objets_liberer(a);
    objets_liberer(c);
    objets_liberer(d);

    /* 5. Cohérence comptable après créations et libérations intercalées (§7, §8) */
    h[0] = objets_creer(300); remplir(h[0], 300, 0x11);
    h[1] = objets_creer(500);
    h[2] = objets_creer(200); remplir(h[2], 200, 0x33);
    h[3] = objets_creer(400);
    h[4] = objets_creer(600); remplir(h[4], 600, 0x55);
    objets_liberer(h[1]);
    objets_liberer(h[3]);
    /* place totale : 3833 - 2000 + 900 = 2733 */
    w = objets_creer(2734);
    VERIFIER(GESTOBJ_BLOC(w) != GESTOBJ_BLOC(h[0]));   /* un octet de trop : autre bloc */
    objets_liberer(w);
    w = objets_creer(2733);
    VERIFIER_EGAL(GESTOBJ_BLOC(h[0]), GESTOBJ_BLOC(w));
    VERIFIER(intact(h[0], 300, 0x11));
    VERIFIER(intact(h[2], 200, 0x33));
    VERIFIER(intact(h[4], 600, 0x55));
    d = objets_creer(1);                                /* bloc plein, sans trou */
    VERIFIER(GESTOBJ_BLOC(d) != GESTOBJ_BLOC(h[0]));
    objets_liberer(d);
    objets_liberer(w);
    objets_liberer(h[0]);
    objets_liberer(h[2]);
    objets_liberer(h[4]);

    /* 6. Seuil des objets hors-norme : 3833 en bloc, 3834 hors-norme (IMP-0161) */
    a = objets_creer(GESTOBJ_SEUIL_HORS_NORME);
    VERIFIER(GESTOBJ_BLOC(a) != GESTOBJ_INDICE_HORS_NORME);
    c = objets_creer(GESTOBJ_SEUIL_HORS_NORME + 1);
    VERIFIER_EGAL(GESTOBJ_INDICE_HORS_NORME, GESTOBJ_BLOC(c));
    remplir(c, GESTOBJ_SEUIL_HORS_NORME + 1, 0x7E);
    VERIFIER(intact(c, GESTOBJ_SEUIL_HORS_NORME + 1, 0x7E));
    objets_liberer(a);
    objets_liberer(c);
    VERIFIER(!objets_existe(c));

    /* 7. 255 objets hors-norme, échec propre du 256e (§9, §11) */
    tous_valides = 1;
    for (i = 0; i < 255; i++) {
        h[i] = objets_creer(4000);
        if (h[i] == GESTOBJ_HANDLE_NUL) tous_valides = 0;
    }
    VERIFIER(tous_valides);
    remplir(h[0], 4000, 0x5A);
    remplir(h[254], 4000, 0xA5);
    VERIFIER(objets_creer(4000) == GESTOBJ_HANDLE_NUL);
    VERIFIER(intact(h[0], 4000, 0x5A));
    VERIFIER(intact(h[254], 4000, 0xA5));
    tout_liberer(255);

    /* 8. 255 blocs-maîtres pleins, échec propre au-delà (§10, §11) */
    tous_valides = 1;
    for (i = 0; i < 255; i++) {
        h[i] = objets_creer(GESTOBJ_SEUIL_HORS_NORME);
        if (h[i] == GESTOBJ_HANDLE_NUL || GESTOBJ_BLOC(h[i]) != i) tous_valides = 0;
    }
    VERIFIER(tous_valides);
    VERIFIER(objets_creer(GESTOBJ_SEUIL_HORS_NORME) == GESTOBJ_HANDLE_NUL);
    VERIFIER(objets_creer(1) == GESTOBJ_HANDLE_NUL);
    tout_liberer(255);
    a = objets_creer(1);                               /* tout est de nouveau libre */
    VERIFIER_EGAL(0, GESTOBJ_BLOC(a));
    objets_liberer(a);

    /* 9. Objet de longueur nulle (IMP-0194) */
    a = objets_creer(0);
    VERIFIER(a != GESTOBJ_HANDLE_NUL);
    VERIFIER(objets_existe(a));
    VERIFIER(objets_resoudre(a) != NULL);
    objets_relacher(a);
    objets_liberer(a);

    /* 10 bis. objets_copier (IMP-0243) : copie sans verrouillage */
    {
        unsigned char t[16];
        unsigned char *p;
        a = objets_creer(10);
        p = objets_resoudre(a);
        for (i = 0; i < 10; i++) p[i] = (unsigned char) (i + 1);
        objets_relacher(a);
        memset(t, 0, sizeof t);
        VERIFIER(objets_copier(a, 0, t, 10) && t[0] == 1 && t[9] == 10);
        VERIFIER(objets_copier(a, 4, t, 3) && t[0] == 5 && t[2] == 7);
        VERIFIER(!objets_copier(a, 8, t, 3));                /* au-delà de l'objet */
        VERIFIER(objets_copier(a, 10, t, 0));                /* plage vide en fin */
        c = objets_creer(GESTOBJ_SEUIL_HORS_NORME + 1);
        remplir(c, GESTOBJ_SEUIL_HORS_NORME + 1, 0x3C);
        VERIFIER(objets_copier(c, GESTOBJ_SEUIL_HORS_NORME - 15, t, 16) && t[0] == 0x3C && t[15] == 0x3C);
        VERIFIER(!objets_copier(c, GESTOBJ_SEUIL_HORS_NORME, t, 2));
        objets_liberer(c);
        VERIFIER(!objets_copier(c, 0, t, 1));                /* libéré */
        objets_liberer(a);
        VERIFIER(!objets_copier(a, 0, t, 1));
        VERIFIER(!objets_copier(GESTOBJ_HANDLE_NUL, 0, t, 1));
    }

    /* 10 ter. objets_ecrire (IMP-0243) : écriture sans verrouillage ; décalage
       au-delà de 64 Kio dans un objet hors-norme */
    {
        static const unsigned char motif[5] = { 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
        unsigned char t[8];
        a = objets_creer(10);
        remplir(a, 10, 0x11);
        VERIFIER(objets_ecrire(a, 3, motif, 5));
        VERIFIER(objets_copier(a, 2, t, 7) && t[0] == 0x11 && t[1] == 0xA1 && t[5] == 0xA5 && t[6] == 0x11);
        VERIFIER(!objets_ecrire(a, 6, motif, 5));            /* au-delà de l'objet */
        VERIFIER(!objets_ecrire(a, 65536ul, motif, 1));
        VERIFIER(objets_copier(a, 9, t, 1) && t[0] == 0x11); /* rien d'écrit */
        c = objets_creer(70000ul);
        VERIFIER(c != GESTOBJ_HANDLE_NUL);
        memset(t, 0, sizeof t);
        VERIFIER(objets_ecrire(c, 0, t, 8) && objets_ecrire(c, 4459, t, 8));   /* 65 536 et 69 995 modulo 65 536 */
        VERIFIER(objets_ecrire(c, 65533ul, motif, 5));       /* à cheval sur 65 536 */
        VERIFIER(objets_ecrire(c, 69995ul, motif, 5));       /* fin de l'objet */
        VERIFIER(!objets_ecrire(c, 69996ul, motif, 5));
        VERIFIER(objets_copier(c, 65533ul, t, 5) && memcmp(t, motif, 5) == 0);
        VERIFIER(objets_copier(c, 69995ul, t, 5) && memcmp(t, motif, 5) == 0);
        /* adresses calculées sur 32 bits : rien n'est retombé au début de l'objet */
        VERIFIER(objets_copier(c, 0, t, 8) && t[0] == 0 && t[1] == 0 && t[7] == 0);
        VERIFIER(objets_copier(c, 4459, t, 8) && t[0] == 0 && t[4] == 0 && t[7] == 0);
        objets_liberer(c);
        VERIFIER(!objets_ecrire(c, 0, motif, 1));            /* libéré */
        objets_liberer(a);
        VERIFIER(!objets_ecrire(a, 0, motif, 1));
        VERIFIER(!objets_ecrire(GESTOBJ_HANDLE_NUL, 0, motif, 1));
    }

    /* 10. objets_existe : handle nul, handle libéré */
    VERIFIER(!objets_existe(GESTOBJ_HANDLE_NUL));
    a = objets_creer(8);
    c = objets_creer(8);
    objets_liberer(a);
    VERIFIER(!objets_existe(a));
    VERIFIER(objets_existe(c));
    objets_liberer(c);

    /* 11. objets_terminer (IMP-0270) : tous les blocs rendus, puis reprise à vide */
    a = objets_creer(8);
    c = objets_creer(GESTOBJ_SEUIL_HORS_NORME + 1);
    for (i = 0; i < 70; i++)                        /* deux blocs-maîtres */
        h[i] = objets_creer(32);
    VERIFIER(GESTOBJ_BLOC(h[69]) != GESTOBJ_BLOC(a));
    objets_terminer();
    VERIFIER(!objets_existe(a));
    VERIFIER(!objets_existe(c));
    VERIFIER(!objets_existe(h[0]) && !objets_existe(h[69]));
    a = objets_creer(8);
    VERIFIER_EGAL(0, GESTOBJ_BLOC(a));              /* table vide : premier indice */
    VERIFIER(objets_existe(a));
    objets_terminer();

    VERIF_FIN();
}
