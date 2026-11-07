/* historiques_erreurs.c — erreurs de compilation attestées par le corpus
   EXI: EXI-0019, EXI-0051  VM: VM-2210, VM-3201  IV: IV-IF-0001
   IMP: IMP-0185, IMP-0186, IMP-0187, IMP-0191, IMP-0195, IMP-0196
   Spécification des tests : chapitre Gestionnaire des erreurs §2.2 (messages
   attestés). Chaque ligne est celle de la source, la flèche écrite « _ » ;
   le message attendu est celui qu'imprimait la machine. La position n'est
   pas vérifiée : aucune source ne l'imprime.
   Hors de portée de cette suite : C 36 puis C 63 pour une ligne de plus de
   80 caractères coupée à la lecture (Relevé T1600, SPHIN) : la coupure est
   faite par le moniteur, avant le compilateur.
   Niveau : historique. */

#include "../composants/compilateur_outils.h"
#include "../../src/noyau/erreurs.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

/* Rejet de texte sous le profil donné, avec le message imprimé attendu. */
static int atteste(ProfilImplantation profil, const char *texte, const char *message)
{
    char obtenu_msg[100];

    configuration_session.profil = profil;
    symboles_vider();
    if (compiler(texte)) {
        recuperer();
        printf("  [%s] accepte, attendu %s\n", texte, message);
        return 0;
    }
    erreurs_formater(d.nature, 0, obtenu_msg, sizeof obtenu_msg);
    if (strcmp(obtenu_msg, message) != 0) {
        printf("  [%s] %s, attendu %s\n", texte, obtenu_msg, message);
        return 0;
    }
    return 1;
}

int main(void)
{
    VERIF_DEBUT("historiques/erreurs de compilation");

    /* Relevé T1600, 6EME - SPHIN, numerisation0022 : AFFICHEP, 8 lettres */
    VERIFIER(atteste(PROFIL_T1600_V4,
        "59 AFFICHEP 'LA BONNE REPONSE ETAIT VADUZ.';ALLER EN 61", "ERREUR  C 03"));
    /* Relevé T1600, 5EME - ANGLO V3, numerisation0004 : ALHER sans flèche */
    VERIFIER(atteste(PROFIL_T1600_V4,
        "33 SI R1='\"S' ALORS DEBUT ALHER EN 37;I_I+1 FIN", "ERREUR  C 38"));
    /* Relevé T1600, ANGLO V2 : PRE frappé en machine de bureau */
    VERIFIER(atteste(PROFIL_T1600_V4, "PRE", "ERREUR  C 38"));

    /* INRDP75, l. 995 : chaîne non terminée en machine de bureau */
    VERIFIER(atteste(PROFIL_T1600_V4, "?''!'PIERRE", "ERREUR  C 08"));
    VERIFIER(atteste(PROFIL_MITRA15_V8, "?''!'PIERRE", "ERREUR  C 08"));

    /* LAFOND83, p. 19 : commande mal frappée, puis instruction mal frappée */
    VERIFIER(atteste(PROFIL_MITRA15_V8, "LIS", "ERREUR  C 38"));
    VERIFIER(atteste(PROFIL_MITRA15_V8, "AFICHER 'IL FAIT BEAU.'", "ERREUR  C 03"));

    /* Les mêmes lignes corrigées sont acceptées */
    configuration_session.profil = PROFIL_T1600_V4;
    VERIFIER(accepte("59 AFFICHER 'LA BONNE REPONSE ETAIT VADUZ.';ALLER EN 61"));
    VERIFIER(accepte("33 SI R1='\"S' ALORS DEBUT ALLER EN 37;I_I+1 FIN"));
    VERIFIER(accepte("?''!'PIERRE'"));
    configuration_session.profil = PROFIL_MITRA15_V8;
    VERIFIER(accepte("AFFICHER 'IL FAIT BEAU.'"));

    VERIF_FIN();
}
