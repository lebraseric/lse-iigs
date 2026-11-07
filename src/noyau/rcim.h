/* rcim.h — représentation compilée interne en mémoire (RCI-M) : étiquettes,
   écriture et parcours des éléments
   EXI: EXI-0001, EXI-0011, EXI-0016, EXI-0020
   VM: VM-2401, VM-1402
   IV: ANN-K-0004 à ANN-K-0032 (Tome IV), ANN-K-0008 (parcours)
   IMP: IMP-0101 à IMP-0128, IMP-0165, IMP-0166, IMP-0174, IMP-0177,
        IMP-0183, IMP-0184, IMP-0193
   Tests: tests/representation/representation_rcim.c
   Spécification : RCI-M (tome-v/RCI-M_Specification_physique.md) ; fiche
   dérivée references/RCI-M_Reference_codage.md (§1 à §7 bis, §11). */

#ifndef LSE_RCIM_H
#define LSE_RCIM_H

#include "../../src/noyau/objets.h"

typedef unsigned char TagRCIM;   /* bits 7-6 : catégorie ; bits 5-0 : sous-code */

#define RCIM_CAT_VALEUR      0x00
#define RCIM_CAT_REFERENCE   0x40
#define RCIM_CAT_OPERATION   0x80
#define RCIM_CAT_CONTROLE    0xC0
#define RCIM_CAT_MASQUE      0xC0
#define RCIM_SOUSCODE_MASQUE 0x3F

/* --- valeurs immédiates (fiche §2) --- */
#define RCIM_VAL_ENTIER_COURT   0x00   /* s8 */
#define RCIM_VAL_ENTIER         0x01   /* s16 */
#define RCIM_VAL_REEL           0x02   /* float, 4 octets */
#define RCIM_VAL_CHAINE         0x03   /* u8 mode, puis txt ou u16 index */
#define RCIM_VAL_FAUX           0x04
#define RCIM_VAL_VRAI           0x05
#define RCIM_VAL_LIGNE          0x06   /* lig (MITRA 15) */
#define RCIM_VAL_IMMEDIAT_BASE  0x23   /* 0x07..0x3F : valeur = octet - 0x23 */
#define RCIM_VAL_IMMEDIAT_MIN   (-28)
#define RCIM_VAL_IMMEDIAT_MAX   28
#define RCIM_CHAINE_INDEX       0x80   /* bit 7 du mode : chaîne par la table des constantes */
#define RCIM_CHAINE_LONGUEUR_MAX 127

/* --- références (fiche §3) --- */
#define RCIM_REF_SIMPLE           0x40   /* id */
#define RCIM_REF_INDEXEE          0x41   /* id, u8 k */
#define RCIM_REF_LIGNE_COURANTE   0x42   /* @ (T1600) */
#define RCIM_REF_LIGNE_RETOUR     0x43   /* $ (T1600) */

/* --- opérations codées (fiche §4) --- */
#define RCIM_OP_ADD          0x80
#define RCIM_OP_SUB          0x81
#define RCIM_OP_NEG          0x82
#define RCIM_OP_MUL          0x83
#define RCIM_OP_DIV          0x84
#define RCIM_OP_PUIS         0x85
#define RCIM_OP_RAC          0x86
#define RCIM_OP_ABS          0x87
#define RCIM_OP_EXP          0x88
#define RCIM_OP_LGN          0x89
#define RCIM_OP_SIN          0x8A
#define RCIM_OP_COS          0x8B
#define RCIM_OP_ATG          0x8C
#define RCIM_OP_ENT          0x8D
#define RCIM_OP_ALE          0x8E
#define RCIM_OP_TEM          0x8F
#define RCIM_OP_EGAL         0x90
#define RCIM_OP_INF          0x91
#define RCIM_OP_INF_EGAL     0x92
#define RCIM_OP_DIFF         0x93
#define RCIM_OP_SUP          0x94
#define RCIM_OP_SUP_EGAL     0x95
#define RCIM_OP_ET           0x96
#define RCIM_OP_OU           0x97
#define RCIM_OP_NON          0x98
#define RCIM_OP_AFFECTER     0x99
#define RCIM_OP_UN           0x9A
#define RCIM_OP_ZER          0x9B
#define RCIM_OP_CONCAT       0x9C
#define RCIM_OP_LGR          0x9D
#define RCIM_OP_POS          0x9E
#define RCIM_OP_SKP          0x9F   /* u8 n */
#define RCIM_OP_PTR          0xA0   /* u8 n */
#define RCIM_OP_CNB          0xA1   /* u8 n */
#define RCIM_OP_EQN          0xA2   /* u8 n */
#define RCIM_OP_SCH          0xA3   /* u8 n */
#define RCIM_OP_GRL          0xA4   /* u8 n */
#define RCIM_OP_CCA          0xA5
#define RCIM_OP_EQC          0xA6
#define RCIM_OP_DAT          0xA7
#define RCIM_OP_APPEL        0xA8   /* u8 position, u8 k */
#define RCIM_OP_TABLEAU      0xA9   /* id, u8 dim */
#define RCIM_OP_CHAINE       0xAA   /* id */
#define RCIM_OP_LIBERER      0xAB   /* id */
#define RCIM_OP_GARER        0xAC
#define RCIM_OP_CHARGER      0xAD   /* u8 n */
#define RCIM_OP_SUPPRIMER    0xAE   /* u8 n */
#define RCIM_OP_EXECUTER     0xAF   /* u8 n */
#define RCIM_OP_AFF_LIBELLE      0xB0   /* u8 facteur, u8 n, txt[n] */
#define RCIM_OP_AFF_LIGNE        0xB1   /* u8 facteur */
#define RCIM_OP_AFF_ESPACE       0xB2   /* u8 facteur */
#define RCIM_OP_AFF_RETOUR       0xB3   /* u8 facteur */
#define RCIM_OP_AFF_SAUT         0xB4   /* u8 facteur */
#define RCIM_OP_AFF_DESCRIPTION  0xB5   /* forme compacte, fiche §6 */
#define RCIM_OP_AFF_VALEUR       0xB6
#define RCIM_OP_AFF_TETE_FORMAT       0xB7
#define RCIM_OP_AFF_TETE_SANS_FORMAT  0xB8
#define RCIM_OP_LIRE_LIBELLE     0xB9   /* réservé, non attesté : u8 r, u8 n, txt[n] */
#define RCIM_OP_LIRE_MISE_EN_PAGE 0xBA  /* réservé, non attesté : u8 car, u8 r */
#define RCIM_OP_LIRE_VALEUR      0xBB
#define RCIM_OP_SH           0xBC
#define RCIM_OP_CH           0xBD
#define RCIM_OP_TH           0xBE
#define RCIM_OP_SGN          0xBF

#define RCIM_APPEL_INSTRUCTION  0x00
#define RCIM_APPEL_EXPRESSION   0x01
#define RCIM_FACTEUR_VARIABLE   0x80   /* bit 7 du facteur des éléments B0 à B4 */
#define RCIM_DESC_F  0
#define RCIM_DESC_E  1
#define RCIM_DESC_U  2

/* --- informations de contrôle (fiche §5) --- */
#define RCIM_CTL_SI_FAUX              0xC0   /* pos */
#define RCIM_CTL_SAUT                 0xC1   /* pos */
#define RCIM_CTL_ALLER_EN             0xC2   /* lig */
#define RCIM_CTL_ALLER_EN_CALCULE     0xC3
#define RCIM_CTL_RETOUR               0xC4
#define RCIM_CTL_RETOUR_EN            0xC5   /* lig */
#define RCIM_CTL_RETOUR_EN_CALCULE    0xC6
#define RCIM_CTL_TERMINER             0xC7
#define RCIM_CTL_PAUSE                0xC8
#define RCIM_CTL_FAIRE_OUVRIR         0xC9   /* id */
#define RCIM_CTL_FAIRE_PAS            0xCA
#define RCIM_CTL_RESULTAT             0xCB
#define RCIM_CTL_COMMENTAIRE          0xCC   /* u8 n, txt[n] */
#define RCIM_CTL_PROCEDURE            0xCD   /* id, u8 n, n × id, u8 m, m × id */
#define RCIM_CTL_FAIRE_PAS_IMPLICITE  0xCE
#define RCIM_CTL_FAIRE_JUSQUA         0xCF
#define RCIM_CTL_FAIRE_TANT_QUE       0xD0
#define RCIM_CTL_LIRE_TETE            0xD1
#define RCIM_CTL_FAIRE_OUVRIR_SEUL    0xD2   /* FAIRE n1 TANT QUE c, LSE F4 (IMP-0277) */

#define RCIM_LONGUEUR_MAX_SEQUENCE    255u   /* IMP-0105, IMP-0165 */
#define RCIM_POSITION_PROVISOIRE      0xFFu  /* IMP-0166 : jamais testée */

/* --- objet compilé d'une ligne (RCI-M §5) --- */
typedef struct {
    unsigned char numero_ligne;      /* 1-250 (MITRA 15) ou 1-255 (T1600) */
    unsigned int  longueur_octets;   /* <= 255 */
    HandleObjet   bloc;              /* jamais un pointeur persistant */
} EntreeObjetCompile;

/* --- écriture (Compilateur §5, §6) ---------------------------------------
   Une séquence en construction est un tampon de capacite octets et un
   curseur d'écriture. Chaque fonction écrit un élément complet, étiquette et
   charge utile, et renvoie 1 ; si l'élément ne tient pas dans la place
   restante, elle n'écrit rien et renvoie 0 (IMP-0165). */
typedef struct {
    unsigned char *octets;
    unsigned int   capacite;   /* RCIM_LONGUEUR_MAX_SEQUENCE pour le compilateur */
    unsigned int   curseur;    /* longueur écrite */
} TamponRCIM;

void rcim_tampon_initialiser(TamponRCIM *t, unsigned char *octets, unsigned int capacite);

int rcim_ecrire_tag(TamponRCIM *t, TagRCIM tag);                      /* élément sans charge utile */
int rcim_ecrire_tag_u8(TamponRCIM *t, TagRCIM tag, unsigned char u);  /* charge utile d'un octet */
int rcim_ecrire_tag_id(TamponRCIM *t, TagRCIM tag, unsigned int id);  /* charge utile id (u16) */

/* Constante entière, dans le plus petit sous-format (fiche §2) ; au-delà de
   s16, réel. */
int rcim_ecrire_entier(TamponRCIM *t, long valeur);
int rcim_ecrire_reel(TamponRCIM *t, float valeur);
/* Réels du profil T1600 (IMP-0280) : *z arrondi à 23 bits significatifs,
   la demie en s'éloignant de zéro ; le résultat tient dans un float sans
   nouvel arrondi. Infini et NaN inchangés. */
void rcim_arrondir_23(long double *z);
int rcim_ecrire_chaine_en_ligne(TamponRCIM *t, const unsigned char *texte, unsigned int longueur);
int rcim_ecrire_chaine_index(TamponRCIM *t, unsigned int index);
int rcim_ecrire_reference(TamponRCIM *t, unsigned int id);
int rcim_ecrire_reference_indexee(TamponRCIM *t, unsigned int id, unsigned char nb_indices);
int rcim_ecrire_appel(TamponRCIM *t, unsigned char position, unsigned char nb_arguments);
int rcim_ecrire_tableau(TamponRCIM *t, unsigned int id, unsigned char nb_dimensions);

/* Branchement C0 ou C1 à cible provisoire (IMP-0166) : *a_corriger reçoit la
   position de l'octet de charge utile, à passer ensuite à rcim_corriger. */
int  rcim_ecrire_branchement(TamponRCIM *t, TagRCIM tag, unsigned int *a_corriger);
/* Écrit la position courante du curseur comme cible du branchement. */
void rcim_corriger(TamponRCIM *t, unsigned int a_corriger);

int rcim_ecrire_commentaire(TamponRCIM *t, const unsigned char *texte, unsigned int longueur);
int rcim_ecrire_declaration_procedure(TamponRCIM *t, unsigned int id_procedure,
                                      unsigned char n, const unsigned int *parametres,
                                      unsigned char m, const unsigned int *locaux);

/* AFFICHER (fiche §6) : facteur fixe 1 à 99, ou RCIM_FACTEUR_VARIABLE. */
int rcim_ecrire_libelle(TamponRCIM *t, unsigned char facteur,
                        const unsigned char *texte, unsigned int longueur);
int rcim_ecrire_mise_en_page(TamponRCIM *t, TagRCIM tag, unsigned char facteur);
/* type RCIM_DESC_F, _E ou _U ; repetition 1 à 99 ; e et d ignorés pour U. */
int rcim_ecrire_description(TamponRCIM *t, unsigned char type, unsigned char repetition,
                            unsigned char e, unsigned char d);

/* --- parcours (RCI-M §9, ANN-K-0008 ; fiche §7 bis) ----------------------- */
typedef struct {
    const unsigned char *base;       /* résolution temporaire du HandleObjet */
    unsigned int         longueur;
    unsigned int         position;   /* 0 <= position <= longueur */
} CurseurSequence;

void    premier_element(CurseurSequence *c, const unsigned char *base, unsigned int longueur);
int     fin_sequence(const CurseurSequence *c);
TagRCIM lire_tag(const CurseurSequence *c);
void    element_suivant(CurseurSequence *c);

/* Longueur, étiquette comprise, de l'élément qui commence à position ;
   0 si l'étiquette est réservée ou si l'élément dépasse la séquence. */
unsigned int rcim_longueur_element(const unsigned char *base, unsigned int longueur,
                                   unsigned int position);

/* Vrai si la séquence se découpe exactement en éléments valides ; *nb
   reçoit alors leur nombre (nb peut être NULL). */
int rcim_sequence_valide(const unsigned char *base, unsigned int longueur, unsigned int *nb);

/* Vrai si la séquence se découpe exactement en éléments valides, et si chaque
   identifiant de symbole y est inférieur à ns et chaque index de constante
   inférieur à nc (restauration d'un fichier-programme, IMP-0246). */
int rcim_sequence_conforme(const unsigned char *base, unsigned int longueur,
                           unsigned int ns, unsigned int nc);

/* Lecture d'un champ de plusieurs octets, octet de poids faible en tête (IMP-0184). */
unsigned int rcim_lire_u16(const unsigned char *p);
int          rcim_lire_s16(const unsigned char *p);
float        rcim_lire_reel(const unsigned char *p);

#endif
