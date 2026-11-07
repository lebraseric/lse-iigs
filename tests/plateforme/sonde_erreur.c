/* sonde_erreur.c — SystemError rend la console (IMP-0273 ; audit du code, AC-12)
   Ouvre la console comme LSE (lecture brute, CR sans LF), puis provoque un
   débordement de la pile. Sur l'Apple IIGS (image en PROFIL=cible), le retour
   au shell doit trouver une console normale : écho de la frappe et passage à
   la ligne. Sous Golden Gate, sans pilote : message « Stack overflow », code
   de sortie 255. Programme SONDEE de l'image ; hors de « make tests ».
   Jamais lié dans LSE. */

#include "../../src/plateforme/console_iigs.h"
#include "../../src/interfaces/interface_console.h"
#include "../../src/interfaces/profil.h"

static void descendre(unsigned int n)
{
    volatile unsigned char t[64];
    t[0] = (unsigned char) n;
    descendre(n + 1);
}

int main(void)
{
    console_ouvrir();
    console_ecrire("SONDEE : DEBORDEMENT VOLONTAIRE DE LA PILE", 42);
    console_retour_chariot();
    console_interligne();
    descendre(0);
    return 0;
}
