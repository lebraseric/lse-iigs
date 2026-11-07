/* interface_console.h — interface entre le noyau et la console
   EXI: EXI-0045, EXI-0046 (dialogue)  VM: services d'entrée-sortie (Tome III)
   IMP: IMP-0136 (séparation noyau / plateforme), IMP-0220 ; Moniteur
        IMP-0238 (touches de fin), IMP-0239 (interruption) ; Bibliothécaire
        et fichiers IMP-0263 (X-ON)
   Tests: tests/bouchons/console_bouchon.c (réalisation de test)
   Spécification : chapitre Interpréteur §4 bis (IMP-0220).
   Les commandes sont celles d'une console de type télétype (LSE15 §6-2.2) ;
   la réalisation de la plateforme les rend sur son écran. */

#ifndef LSE_INTERFACE_CONSOLE_H
#define LSE_INTERFACE_CONSOLE_H

void         console_ecrire(const char *texte, unsigned int n);
void         console_retour_chariot(void);   /* CR : début de la ligne courante */
void         console_interligne(void);       /* LF : ligne suivante */
void         console_bip(void);              /* BEL */
/* Touche qui termine une saisie (Moniteur IMP-0238, IMP-0239). */
#define SAISIE_XOFF   0
#define SAISIE_RETURN 1
#define SAISIE_ESC    2
#define SAISIE_XON    3   /* X-ON (Ctrl-Q) sur une ligne vide : lecteur de ruban (IMP-0263) */
/* Lit une ligne tapée, avec écho, terminée par X-OFF, RETURN ou ESC, qui
   n'est pas rangée, ou X-ON frappé avant tout autre caractère ; renvoie sa longueur, au plus max, et la touche de fin
   dans *fin. L'écho de RETURN passe à la ligne ; celui de X-OFF et d'ESC ne
   déplace pas le curseur. Appelée par le moniteur seul. */
unsigned int console_lire_ligne(char *tampon, unsigned int max, unsigned char *fin);
/* 1 si ESC a été frappée depuis la dernière consultation. */
int          console_interruption(void);
/* 1 quand l'entrée est épuisée (repli stdio, Golden Gate) : le moniteur clôt la
   session par un AU REVOIR implicite (IMP-0240). Toujours 0 avec le pilote. */
int          console_fin_entree(void);
/* Écho de la saisie : 0 le supprime (IDENTIFICATION, SILENCE ; Bibliothécaire
   et fichiers IMP-0254), 1 le rétablit. */
void         console_echo(int vrai);

#endif
