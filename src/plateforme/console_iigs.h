/* console_iigs.h — ouverture et fermeture de la console de l'Apple IIGS
   IMP: IMP-0240 (pilote .CONSOLE de GS/OS, repli sur stdio)
   Spécification : chapitre Moniteur et console §9 bis. */

#ifndef LSE_CONSOLE_IIGS_H
#define LSE_CONSOLE_IIGS_H

/* Met le pilote .CONSOLE en lecture brute, CR sans interligne ; à défaut de
   pilote (Golden Gate), la console passe par stdio. */
void console_ouvrir(void);
/* Rend au pilote l'état trouvé à l'ouverture. */
void console_fermer(void);
/* Vrai si le pilote .CONSOLE est ouvert (Apple IIGS), faux sous Golden Gate. */
int  console_pilote(void);
/* Vrai : RUB-OUT (DELETE) termine la saisie comme X-OFF, sous LSE F4 (IMP-0238). */
void console_rubout(int vrai);

#endif
