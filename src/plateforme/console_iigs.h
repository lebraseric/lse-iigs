/* console_iigs.h — ouverture et fermeture de la console de l'Apple IIGS
   IMP: IMP-0240 (pilote .CONSOLE de GS/OS, repli sur stdio), IMP-0283 (port série)
   Spécification : chapitre Moniteur et console §9 bis, §9 ter. */

#ifndef LSE_CONSOLE_IIGS_H
#define LSE_CONSOLE_IIGS_H

/* Met le pilote .CONSOLE en lecture brute, CR sans interligne ; à défaut de
   pilote (Golden Gate), la console passe par stdio. */
void console_ouvrir(void);
/* Option -SERIE (IMP-0283) : la console passe sur le port modem, par le pilote
   que GS/OS engendre pour le slot 2 ; le pilote .CONSOLE n'est pas touché.
   Rend 0 si le port est introuvable ou ne s'ouvre pas (Golden Gate compris). */
int  console_ouvrir_serie(void);
/* Rend au pilote l'état trouvé à l'ouverture ; ferme le port série. */
void console_fermer(void);
/* Vrai si la console passe par un pilote de GS/OS (.CONSOLE ou port série,
   Apple IIGS), faux sous Golden Gate. */
int  console_pilote(void);
/* Vrai : RUB-OUT (DELETE) termine la saisie comme X-OFF, sous LSE F4 (IMP-0238). */
void console_rubout(int vrai);

#endif
