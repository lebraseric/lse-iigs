/* rcim.c — écriture et parcours des éléments RCI-M
   EXI: EXI-0001, EXI-0011, EXI-0016, EXI-0020
   VM: VM-2401, VM-1402
   IV: ANN-K-0004 à ANN-K-0032, ANN-K-0008
   IMP: IMP-0103 (valeurs), IMP-0104 (références), IMP-0105 (positions),
        IMP-0112, IMP-0277 (FAIRE), IMP-0113 (commentaire), IMP-0115, IMP-0116
        (AFFICHER), IMP-0120, IMP-0121, IMP-0122, IMP-0123, IMP-0128,
        IMP-0165 (contrôle de place), IMP-0166 (correction différée),
        IMP-0174 (déclaration), IMP-0177 (APPEL), IMP-0184 (ordre des
        octets), IMP-0193 (segment), IMP-0280 (réels de 23 bits)
   Tests: tests/representation/representation_rcim.c
   Spécification : RCI-M §4, §9 ; fiche de codage §2 à §7 bis. */

#pragma noroot

#include <string.h>
#include "rcim.h"
#include "../interfaces/profil.h"

segment "rcim";

/* --- outils internes --------------------------------------------------- */

static int place(const TamponRCIM *t, unsigned int n)
{
    return t->curseur + n <= t->capacite;
}

static void poser_u8(TamponRCIM *t, unsigned char u)
{
    t->octets[t->curseur++] = u;
}

static void poser_u16(TamponRCIM *t, unsigned int v)
{
    t->octets[t->curseur++] = (unsigned char) (v & 0xFFu);
    t->octets[t->curseur++] = (unsigned char) (v >> 8);
}

static void poser_texte(TamponRCIM *t, const unsigned char *texte, unsigned int n)
{
    memcpy(t->octets + t->curseur, texte, n);
    t->curseur += n;
}

/* --- écriture ---------------------------------------------------------- */

void rcim_tampon_initialiser(TamponRCIM *t, unsigned char *octets, unsigned int capacite)
{
    t->octets = octets;
    t->capacite = capacite;
    t->curseur = 0;
}

int rcim_ecrire_tag(TamponRCIM *t, TagRCIM tag)
{
    if (!place(t, 1)) return 0;
    poser_u8(t, tag);
    return 1;
}

int rcim_ecrire_tag_u8(TamponRCIM *t, TagRCIM tag, unsigned char u)
{
    if (!place(t, 2)) return 0;
    poser_u8(t, tag);
    poser_u8(t, u);
    return 1;
}

int rcim_ecrire_tag_id(TamponRCIM *t, TagRCIM tag, unsigned int id)
{
    if (!place(t, 3)) return 0;
    poser_u8(t, tag);
    poser_u16(t, id);
    return 1;
}

int rcim_ecrire_entier(TamponRCIM *t, long valeur)
{
    if (valeur >= RCIM_VAL_IMMEDIAT_MIN && valeur <= RCIM_VAL_IMMEDIAT_MAX)
        return rcim_ecrire_tag(t, (TagRCIM) (RCIM_VAL_IMMEDIAT_BASE + (int) valeur));
    if (valeur >= -128L && valeur <= 127L)
        return rcim_ecrire_tag_u8(t, RCIM_VAL_ENTIER_COURT, (unsigned char) (int) valeur);
    if (valeur >= -32768L && valeur <= 32767L)
        return rcim_ecrire_tag_id(t, RCIM_VAL_ENTIER, (unsigned int) (int) valeur);
    return rcim_ecrire_reel(t, (float) valeur);
}

/* Format étendu de SANE : mantisse de 64 bits à bit entier explicite (mots 0
   à 3, poids faible d'abord), signe et exposant (mot 4). Les 23 bits gardés
   sont le mot 3 et les 7 bits de poids fort du mot 2 ; la demie unité du
   dernier rang gardé est le bit 8 du mot 2 : on l'ajoute, puis on tronque. */
typedef union {
    long double  x;
    unsigned int w[5];
} EtenduRCIM;

void rcim_arrondir_23(long double *z)
{
    EtenduRCIM *e = (EtenduRCIM *) z;
    unsigned int m;

    if ((e->w[4] & 0x7FFFu) == 0x7FFFu) return;          /* infini, NaN */
    m = e->w[2] + 0x0100u;
    if (m < e->w[2]) {                                   /* retenue */
        e->w[3]++;
        if (e->w[3] == 0) {                              /* 2^k : exposant suivant */
            e->w[3] = 0x8000u;
            e->w[4]++;
        }
    }
    e->w[2] = m & 0xFE00u;
    e->w[1] = 0;
    e->w[0] = 0;
}

int rcim_ecrire_reel(TamponRCIM *t, float valeur)
{
    /* ORCA/C transmet et conserve un paramètre float au format SANE extended
       (10 octets) : son adresse ne désigne pas un float IEEE de 4 octets. La
       valeur est donc d'abord convertie dans une variable locale float. */
    float f = valeur;

    if (!place(t, 5)) return 0;
    poser_u8(t, RCIM_VAL_REEL);
    poser_texte(t, (const unsigned char *) &f, 4);   /* ordre natif = IMP-0184 */
    return 1;
}

int rcim_ecrire_chaine_en_ligne(TamponRCIM *t, const unsigned char *texte, unsigned int longueur)
{
    if (longueur > RCIM_CHAINE_LONGUEUR_MAX || !place(t, 2 + longueur)) return 0;
    poser_u8(t, RCIM_VAL_CHAINE);
    poser_u8(t, (unsigned char) longueur);
    poser_texte(t, texte, longueur);
    return 1;
}

int rcim_ecrire_chaine_index(TamponRCIM *t, unsigned int index)
{
    if (!place(t, 4)) return 0;
    poser_u8(t, RCIM_VAL_CHAINE);
    poser_u8(t, RCIM_CHAINE_INDEX);
    poser_u16(t, index);
    return 1;
}

int rcim_ecrire_reference(TamponRCIM *t, unsigned int id)
{
    return rcim_ecrire_tag_id(t, RCIM_REF_SIMPLE, id);
}

int rcim_ecrire_reference_indexee(TamponRCIM *t, unsigned int id, unsigned char nb_indices)
{
    if (!place(t, 4)) return 0;
    poser_u8(t, RCIM_REF_INDEXEE);
    poser_u16(t, id);
    poser_u8(t, nb_indices);
    return 1;
}

int rcim_ecrire_appel(TamponRCIM *t, unsigned char position, unsigned char nb_arguments)
{
    if (!place(t, 3)) return 0;
    poser_u8(t, RCIM_OP_APPEL);
    poser_u8(t, position);
    poser_u8(t, nb_arguments);
    return 1;
}

int rcim_ecrire_tableau(TamponRCIM *t, unsigned int id, unsigned char nb_dimensions)
{
    if (!place(t, 4)) return 0;
    poser_u8(t, RCIM_OP_TABLEAU);
    poser_u16(t, id);
    poser_u8(t, nb_dimensions);
    return 1;
}

int rcim_ecrire_branchement(TamponRCIM *t, TagRCIM tag, unsigned int *a_corriger)
{
    if (!place(t, 2)) return 0;
    poser_u8(t, tag);
    *a_corriger = t->curseur;
    poser_u8(t, RCIM_POSITION_PROVISOIRE);
    return 1;
}

void rcim_corriger(TamponRCIM *t, unsigned int a_corriger)
{
    t->octets[a_corriger] = (unsigned char) t->curseur;
}

int rcim_ecrire_commentaire(TamponRCIM *t, const unsigned char *texte, unsigned int longueur)
{
    if (longueur > 255u || !place(t, 2 + longueur)) return 0;
    poser_u8(t, RCIM_CTL_COMMENTAIRE);
    poser_u8(t, (unsigned char) longueur);
    poser_texte(t, texte, longueur);
    return 1;
}

int rcim_ecrire_declaration_procedure(TamponRCIM *t, unsigned int id_procedure,
                                      unsigned char n, const unsigned int *parametres,
                                      unsigned char m, const unsigned int *locaux)
{
    unsigned char i;

    if (!place(t, 1 + 2 + 1 + 2u * n + 1 + 2u * m)) return 0;
    poser_u8(t, RCIM_CTL_PROCEDURE);
    poser_u16(t, id_procedure);
    poser_u8(t, n);
    for (i = 0; i < n; i++) poser_u16(t, parametres[i]);
    poser_u8(t, m);
    for (i = 0; i < m; i++) poser_u16(t, locaux[i]);
    return 1;
}

int rcim_ecrire_libelle(TamponRCIM *t, unsigned char facteur,
                        const unsigned char *texte, unsigned int longueur)
{
    if (longueur > 255u || !place(t, 3 + longueur)) return 0;
    poser_u8(t, RCIM_OP_AFF_LIBELLE);
    poser_u8(t, facteur);
    poser_u8(t, (unsigned char) longueur);
    poser_texte(t, texte, longueur);
    return 1;
}

int rcim_ecrire_mise_en_page(TamponRCIM *t, TagRCIM tag, unsigned char facteur)
{
    return rcim_ecrire_tag_u8(t, tag, facteur);
}

int rcim_ecrire_description(TamponRCIM *t, unsigned char type, unsigned char repetition,
                            unsigned char e, unsigned char d)
{
    unsigned int n;
    int etendu = repetition > 63;
    int fe = type != RCIM_DESC_U;

    n = 2 + (etendu ? 1 : 0) + (fe ? 2 : 0);
    if (!place(t, n)) return 0;
    poser_u8(t, RCIM_OP_AFF_DESCRIPTION);
    if (etendu) {
        poser_u8(t, (unsigned char) ((type << 6) | 0x3F));
        poser_u8(t, repetition);
    } else {
        poser_u8(t, (unsigned char) ((type << 6) | (repetition - 1)));
    }
    if (fe) {
        poser_u8(t, e);
        poser_u8(t, d);
    }
    return 1;
}

/* --- parcours ---------------------------------------------------------- */

unsigned int rcim_lire_u16(const unsigned char *p)
{
    return (unsigned int) p[0] | ((unsigned int) p[1] << 8);
}

int rcim_lire_s16(const unsigned char *p)
{
    return (int) rcim_lire_u16(p);
}

float rcim_lire_reel(const unsigned char *p)
{
    float f;
    memcpy(&f, p, 4);
    return f;
}

unsigned int rcim_longueur_element(const unsigned char *base, unsigned int longueur,
                                   unsigned int position)
{
    const unsigned char *p;
    unsigned int reste;
    unsigned int n;
    TagRCIM tag;

    if (position >= longueur) return 0;
    p = base + position;
    reste = longueur - position;
    tag = p[0];

    /* une charge utile de longueur variable n'est lue que si elle est présente */
#define LIRE(i) ((i) < reste ? p[(i)] : 0)

    switch (tag & RCIM_CAT_MASQUE) {
    case RCIM_CAT_VALEUR:
        switch (tag) {
        case RCIM_VAL_ENTIER_COURT: n = 2; break;
        case RCIM_VAL_ENTIER:       n = 3; break;
        case RCIM_VAL_REEL:         n = 5; break;
        case RCIM_VAL_CHAINE:
            n = (LIRE(1) & RCIM_CHAINE_INDEX) ? 4 : 2 + (LIRE(1) & 0x7Fu);
            break;
        case RCIM_VAL_LIGNE:        n = 2; break;
        default:                    n = 1; break;   /* 0x04, 0x05, 0x07..0x3F */
        }
        break;
    case RCIM_CAT_REFERENCE:
        switch (tag) {
        case RCIM_REF_SIMPLE:         n = 3; break;
        case RCIM_REF_INDEXEE:        n = 4; break;
        case RCIM_REF_LIGNE_COURANTE:
        case RCIM_REF_LIGNE_RETOUR:   n = 1; break;
        default:                      return 0;     /* 0x44..0x7F réservés */
        }
        break;
    case RCIM_CAT_OPERATION:
        switch (tag) {
        case RCIM_OP_SKP: case RCIM_OP_PTR: case RCIM_OP_CNB: case RCIM_OP_EQN:
        case RCIM_OP_SCH: case RCIM_OP_GRL:
        case RCIM_OP_CHARGER: case RCIM_OP_SUPPRIMER: case RCIM_OP_EXECUTER:
        case RCIM_OP_AFF_LIGNE: case RCIM_OP_AFF_ESPACE:
        case RCIM_OP_AFF_RETOUR: case RCIM_OP_AFF_SAUT:
            n = 2; break;
        case RCIM_OP_APPEL:             n = 3; break;
        case RCIM_OP_TABLEAU:           n = 4; break;
        case RCIM_OP_CHAINE:
        case RCIM_OP_LIBERER:           n = 3; break;
        case RCIM_OP_AFF_LIBELLE:       n = 3 + LIRE(2); break;
        case RCIM_OP_LIRE_LIBELLE:      n = 3 + LIRE(2); break;
        case RCIM_OP_LIRE_MISE_EN_PAGE: n = 3; break;
        case RCIM_OP_AFF_DESCRIPTION:
            n = 2;
            if ((LIRE(1) & 0x3Fu) == 0x3Fu) n++;
            if ((LIRE(1) >> 6) != RCIM_DESC_U) n += 2;
            break;
        default:                        n = 1; break;
        }
        break;
    default: /* RCIM_CAT_CONTROLE */
        switch (tag) {
        case RCIM_CTL_SI_FAUX: case RCIM_CTL_SAUT:
        case RCIM_CTL_ALLER_EN: case RCIM_CTL_RETOUR_EN:
            n = 2; break;
        case RCIM_CTL_ALLER_EN_CALCULE: case RCIM_CTL_RETOUR:
        case RCIM_CTL_RETOUR_EN_CALCULE: case RCIM_CTL_TERMINER:
        case RCIM_CTL_PAUSE: case RCIM_CTL_FAIRE_PAS: case RCIM_CTL_RESULTAT:
        case RCIM_CTL_FAIRE_PAS_IMPLICITE: case RCIM_CTL_FAIRE_JUSQUA:
        case RCIM_CTL_FAIRE_TANT_QUE: case RCIM_CTL_LIRE_TETE:
        case RCIM_CTL_FAIRE_OUVRIR_SEUL:
            n = 1; break;
        case RCIM_CTL_FAIRE_OUVRIR:  n = 3; break;
        case RCIM_CTL_COMMENTAIRE:   n = 2 + LIRE(1); break;
        case RCIM_CTL_PROCEDURE:
            /* CD id(2) n(1) n×id m(1) m×id */
            n = 4 + 2u * LIRE(3);
            n = n + 1 + 2u * LIRE(n);
            break;
        default:                     return 0;      /* 0xD2..0xFF réservés */
        }
        break;
    }
#undef LIRE

    return n <= reste ? n : 0;
}

void premier_element(CurseurSequence *c, const unsigned char *base, unsigned int longueur)
{
    c->base = base;
    c->longueur = longueur;
    c->position = 0;
}

int fin_sequence(const CurseurSequence *c)
{
    return c->position >= c->longueur;
}

TagRCIM lire_tag(const CurseurSequence *c)
{
    return c->base[c->position];
}

/* Un élément invalide (étiquette réservée, élément tronqué) termine le
   parcours : le curseur est placé en fin de séquence. */
void element_suivant(CurseurSequence *c)
{
    unsigned int n = rcim_longueur_element(c->base, c->longueur, c->position);
    if (n == 0)
        c->position = c->longueur;
    else
        c->position += n;
}

int rcim_sequence_valide(const unsigned char *base, unsigned int longueur, unsigned int *nb)
{
    unsigned int position = 0;
    unsigned int compte = 0;
    unsigned int n;

    while (position < longueur) {
        n = rcim_longueur_element(base, longueur, position);
        if (n == 0) return 0;
        position += n;
        compte++;
    }
    if (nb != NULL) *nb = compte;
    return 1;
}

int rcim_sequence_conforme(const unsigned char *base, unsigned int longueur,
                           unsigned int ns, unsigned int nc)
{
    const unsigned char *p;
    unsigned int position = 0;
    unsigned int n;
    unsigned int k;
    unsigned int i;
    unsigned int liste;
    unsigned int compte;

    while (position < longueur) {
        n = rcim_longueur_element(base, longueur, position);
        if (n == 0) return 0;
        p = base + position;
        switch (p[0]) {
        case RCIM_VAL_CHAINE:
            if ((p[1] & RCIM_CHAINE_INDEX) && rcim_lire_u16(p + 2) >= nc) return 0;
            break;
        case RCIM_REF_SIMPLE: case RCIM_REF_INDEXEE:
        case RCIM_OP_TABLEAU: case RCIM_OP_CHAINE: case RCIM_OP_LIBERER:
        case RCIM_CTL_FAIRE_OUVRIR:
            if (rcim_lire_u16(p + 1) >= ns) return 0;
            break;
        case RCIM_CTL_PROCEDURE:                         /* CD id(2) n(1) n×id m(1) m×id */
            if (rcim_lire_u16(p + 1) >= ns) return 0;
            k = 3;
            for (liste = 0; liste < 2; liste++) {
                compte = p[k];
                k++;
                for (i = 0; i < compte; i++) {
                    if (rcim_lire_u16(p + k) >= ns) return 0;
                    k = k + 2;
                }
            }
            break;
        default:
            break;
        }
        position += n;
    }
    return 1;
}
