/* main.c — point d'entrée du LSE-IIGS, lancé depuis le shell ORCA
   IMP: IMP-0142 (seul .root), IMP-0180 (profil au lancement),
        IMP-0276 (F4 par défaut ; -F4, -T1600, -MITRA15),
        IMP-0186 (-LIBELLES), IMP-0196 (configuration de session), IMP-0228,
        IMP-0245 (-BIB), IMP-0253 (-TEMP), IMP-0258 (-UNITE), IMP-0262 (-RUBAN), IMP-0268 (-ID),
        IMP-0270 (blocs rendus en fin de session), IMP-0272 (pile de 16 Kio)
   Spécification : chapitre Moniteur et console §2, §9 bis. Segment blanc. */

#pragma stacksize 16384                         /* IMP-0272 */

#include <string.h>
#include <stdlib.h>
#include "../interfaces/types_communs.h"
#include "../noyau/programme.h"
#include "../noyau/moniteur.h"
#include "../noyau/fichiers.h"
#include "../noyau/objets.h"
#include "../noyau/erreurs.h"
#include "../noyau/bibliotheque.h"
#include "console_iigs.h"
#include "fichiers_iigs.h"
#include "../interfaces/interface_fichiers.h"
#include "../interfaces/interface_console.h"
#include "../interfaces/profil.h"

ConfigurationSession configuration_session = { PROFIL_T1600_F4, 0 };

/* Nombre décimal de min à max ; 0 si l'argument n'en est pas un (IMP-0253, IMP-0268). */
static int nombre_option(const char *s, unsigned long min, unsigned long max, unsigned long *v)
{
    unsigned int i;
    unsigned long x = 0;
    unsigned long d;

    if (s[0] == '\0') return 0;
    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') return 0;
        d = (unsigned long) (s[i] - '0');
        if (x > (max - d) / 10ul) return 0;
        x = x * 10ul + d;
    }
    if (x < min) return 0;
    *v = x;
    return 1;
}

/* Option de lancement invalide : message, puis pas de session (audit du code, AC-27). */
static int option_incorrecte(const char *option, const char *valeur)
{
    console_ouvrir();
    console_ecrire("OPTION INCORRECTE : ", 20);
    console_ecrire(option, (unsigned int) strlen(option));
    if (valeur != NULL) {
        console_ecrire(" ", 1);
        console_ecrire(valeur, (unsigned int) strlen(valeur));
    }
    console_retour_chariot();
    console_interligne();
    console_fermer();
    return 1;
}

int main(int argc, char *argv[])
{
    int i;
    const char *bib = NULL;                      /* @:LSE (IMP-0245) */
    unsigned long v;
    char t[100];
    int id_choisie = 0;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-F4") == 0) configuration_session.profil = PROFIL_T1600_F4;
        else if (strcmp(argv[i], "-T1600") == 0) configuration_session.profil = PROFIL_T1600_V4;
        else if (strcmp(argv[i], "-MITRA15") == 0) configuration_session.profil = PROFIL_MITRA15_V8;
        else if (strcmp(argv[i], "-LIBELLES") == 0) configuration_session.libelles = 1;
        else if (strcmp(argv[i], "-BIB") != 0 && strcmp(argv[i], "-UNITE") != 0
                 && strcmp(argv[i], "-RUBAN") != 0 && strcmp(argv[i], "-ID") != 0
                 && strcmp(argv[i], "-TEMP") != 0) return option_incorrecte(argv[i], NULL);
        else if (i + 1 >= argc || argv[i + 1][0] == '\0') return option_incorrecte(argv[i], NULL);
        else if (strcmp(argv[i], "-BIB") == 0) bib = argv[++i];
        else if (strcmp(argv[i], "-UNITE") == 0) plateforme_unite_designer(argv[++i]);
        else if (strcmp(argv[i], "-RUBAN") == 0) plateforme_ruban_designer(argv[++i]);
        else if (strcmp(argv[i], "-ID") == 0) {                                /* IMP-0268 */
            if (!nombre_option(argv[i + 1], 0ul, (unsigned long) IDENTIFICATION_MAX, &v))
                return option_incorrecte(argv[i], argv[i + 1]);
            moniteur_fixer_identification((unsigned int) v);
            id_choisie = 1;
            i++;
        } else {                                                               /* -TEMP, point C1 */
            if (!nombre_option(argv[i + 1], 1ul, 0xFFFFFFFFul, &v))
                return option_incorrecte(argv[i], argv[i + 1]);
            fichiers_fixer_quota(v);
            i++;
        }
    }
    if (!id_choisie && configuration_session.profil == PROFIL_T1600_F4)
        moniteur_fixer_identification(1);               /* Victor Duruy (IMP-0276) */
    if (!programme_initialiser()) {
        objets_terminer();                          /* IMP-0270 */
        return 1;
    }
    console_ouvrir();
    console_rubout(configuration_session.profil == PROFIL_T1600_F4);   /* IMP-0238 */
    plateforme_volume_permettre(console_pilote());     /* IMP-0257 */
    if (plateforme_bibliotheque_ouvrir(bib) != PLAT_OK) {   /* bibliothèque inaccessible (IMP-0250) */
        erreurs_formater_fichier(ERR_F_BIBLIOTHEQUE, 0, 0, configuration_session.libelles, t, sizeof t);
        console_ecrire(t, (unsigned int) strlen(t));
        console_retour_chariot();
        console_interligne();
    }
    moniteur_fixer_demarrage(1);                    /* IMP-0279 */
    moniteur_session();
    console_fermer();
    objets_terminer();                              /* IMP-0270 */
    return 0;
}
