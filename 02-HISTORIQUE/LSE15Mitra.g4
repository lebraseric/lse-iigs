/*
 * ============================================================================
 * GRAMMAIRE ANTLR4 — LSE 15 (Mitra 15, École d'Application des Transmissions,
 *                              manuel 1978)
 * ============================================================================
 *
 * Source : LSE15-Manuel-Mitra15-EAT-1978.md (transcription Eric, chapitres 1 à 9)
 *
 * PÉRIMÈTRE
 * ---------
 * Cette grammaire couvre le LANGAGE LSE proprement dit tel que défini aux
 * chapitres 2 à 8 du manuel, ainsi que le MODE « MACHINE DE BUREAU » du
 * chapitre 9 (§9-5) :
 *   - instructions arithmétiques et sur chaînes (ch. 3, 4)
 *   - instructions de contrôle : SI/ALORS/SINON, DEBUT/FIN, FAIRE, ALLER EN,
 *     PAUSE, TERMINER, EXECUTER (ch. 5)
 *   - instructions d'Entrées/Sorties : LIRE, AFFICHER, GARER, CHARGER (ch. 6)
 *   - procédures : PROCEDURE, RETOUR, RESULTAT, LOCAL (ch. 7)
 *   - commentaires (ch. 8)
 *   - mode machine de bureau (§9-5) : une ligne sans numéro, compilée par
 *     le même compilateur qu'une ligne de programme, avec quelques
 *     instructions en moins et `?` en synonyme d'AFFICHER — voir [J] et
 *     la règle `bureauLine`. Ce mode a été intégré à CETTE grammaire
 *     plutôt que traité à part : il ne définit aucune syntaxe propre
 *     (expressions, conditions, formats identiques à la ligne de
 *     programme), donc une grammaire séparée aurait dupliqué l'essentiel
 *     de celle-ci pour ne faire varier que quelques alternatives de
 *     `statement` — voir NOTES.md pour la discussion complète.
 *
 * HORS PÉRIMÈTRE (volontairement, cf. notes ci-dessous)
 *   - le reste du LANGAGE DE COMMANDE du moniteur (ch. 9 : BONJOUR,
 *     LISTER, EXECUTER A PARTIR DE, EFFACER LIGNES, etc.) : c'est un
 *     protocole de dialogue avec le système d'exploitation, distinct du
 *     langage LSE lui-même, avec ses propres règles de troncature
 *     ("commandes repérées par leurs deux premières lettres"). Il
 *     mériterait une grammaire séparée — contrairement au mode machine
 *     de bureau (voir ci-dessus), qui EST une variante du langage lui-même.
 *   - les caractères de contrôle de saisie clavier (`\` annule le dernier
 *     caractère, `ESC` annule la ligne en cours) : ce sont des artefacts de
 *     frappe, jamais présents dans un listage stocké (produit par LISTER).
 *
 * MODÈLE DE COMPILATION : PAS DE NOTION DE « PROGRAMME »
 * --------------------------------------------------------------------------
 * Le LSE est compilé LIGNE À LIGNE : chaque ligne est compilée séparément
 * et indépendamment de toute autre ligne (propriété documentée dans le
 * manuel INRDP 1975, qui couvre le même langage, bien qu'absente du
 * manuel Mitra 15 lui-même). Il n'existe donc, linguistiquement, aucune
 * unité « programme » : la règle de départ de cette grammaire est `line`
 * (une ligne isolée) ou `bureauLine` (une ligne sans numéro, mode machine
 * de bureau), pas une règle englobant plusieurs lignes. Voir [G] plus bas
 * et NOTES.md pour la règle utilitaire `listing`, qui n'a pas ce statut
 * linguistique et ne sert qu'à l'analyse pratique d'un fichier
 * multi-lignes avec cet outil.
 *
 * AMBIGUÏTÉS ET CHOIX DOCUMENTÉS (dans l'esprit "ne rien inventer, marquer
 * les incertitudes" — voir notes détaillées à la fin du fichier)
 *   [A] Terminaison de ligne : X-OFF vs saut de ligne physique
 *   [B] Identificateurs "lettre+chiffres" vs spécifications de format (E,F,U)
 *   [C] Unification expr arithmétique / expr chaîne (typage dynamique)
 *   [D] Précédence et associativité de l'exponentiation ↑, et restriction
 *       du changement de signe (ex. "A*-B" illégal)
 *   [E] Établissement du SINON en cas d'instructions SI imbriquées
 *   [F] Numéro de ligne vs constante numérique entière (fusion sur NUMBER)
 *   [G] Absence de notion de « programme » (compilation ligne à ligne)
 *   [H] Position de la déclaration de procédure (début de ligne uniquement)
 *   [I] Un paramètre LOCAL ne peut jamais être un nom de procédure
 *   [J] Mode machine de bureau : `?`=AFFICHER, instructions restreintes,
 *       restriction récursive sous SI/DEBUT...FIN
 *   [K] Appel de procédure-fonction interdit en mode machine de bureau
 *       (conséquence de l'interdiction de RESULTAT)
 *   [L] booleanExpr réécrite sans récursion gauche automatique (ANTLR4 ne
 *       supporte pas les paramètres de règle sur une règle gauche-récursive)
 *   [M] Instruction SUPPRIMER (Mitra 15) — omission du manuel Mitra 15,
 *       confirmée par le manuel INRDP 1975
 *   [N] Symbole @ (sens Mitra 15 : marqueur de numéro de ligne) — idem,
 *       restreint à @ suivi d'un entier littéral
 *   [O] Facteur de répétition variable interdit devant une description
 *       (E/F/U) — idem
 *   [P] Règle générale "pas deux opérateurs adjacents" (INRDP 1975,
 *       §4-3) retenue dans toute sa portée, au-delà du seul cas "*"
 * Chaque point est détaillé en commentaire à l'endroit concerné, et repris
 * dans NOTES.md.
 *
 * ============================================================================
 */

grammar LSE15Mitra;

/*
 * Drapeau pilotant le prédicat sémantique de la règle COMMENT (voir sa
 * documentation détaillée plus bas, section lexeur, marqueur [B]).
 * Un commentaire (§8) n'est légal qu'à deux positions :
 *   (a) immédiatement après le numéro de ligne (premier contenu de la
 *       ligne — "en début de ligne, espace non obligatoire") ;
 *   (b) immédiatement après un ';'.
 * On modélise (a) via un second drapeau `atLineStart`, vrai uniquement en
 * tout début de source ou juste après un terminateur de ligne (NEWLINE /
 * X-OFF) : le NUMBER qui suit alors est nécessairement le numéro de
 * ligne, et *lui seul* rend `commentAllowed` vrai pour le jeton suivant.
 * Tout autre NUMBER (opérande d'une expression) laisse `commentAllowed`
 * à faux, ce qui préserve la lecture normale de '*' comme MUL dans des
 * expressions telles que "2*X" même juste après une constante numérique.
 */
@lexer::members {
    private boolean commentAllowed = false;
    private boolean atLineStart = true;

    @Override
    public org.antlr.v4.runtime.Token nextToken() {
        org.antlr.v4.runtime.Token t = super.nextToken();
        if (t.getChannel() != org.antlr.v4.runtime.Token.HIDDEN_CHANNEL) {
            int type = t.getType();
            if (type == SEMI) {
                commentAllowed = true;
                atLineStart = false;
            } else if (type == NEWLINE || type == XOFF) {
                commentAllowed = false;
                atLineStart = true;
            } else if (atLineStart && type == NUMBER) {
                // NUMBER consommé juste après un terminateur : c'est le
                // numéro de la nouvelle ligne -> un commentaire peut suivre.
                commentAllowed = true;
                atLineStart = false;
            } else {
                commentAllowed = false;
                atLineStart = false;
            }
        }
        return t;
    }
}

// ============================================================================
// RÈGLES DU PARSEUR
// ============================================================================

/*
 * RÈGLE DE DÉPART : `line`, pas `program`.
 *
 * [G] ABSENCE DE NOTION DE « PROGRAMME » — voir NOTES.md.
 * Le LSE est compilé LIGNE À LIGNE : chaque ligne est compilée
 * séparément et indépendamment de toute autre ligne. Cet aspect n'est pas
 * explicité dans le manuel Mitra 15 (EAT, 1978) lui-même, mais l'est dans
 * le manuel INRDP de 1975, qui documente le même langage. Une règle
 * `program : line+ EOF` n'a donc aucune justification linguistique dans
 * cette grammaire : le LSE ne connaît pas d'unité syntaxique
 * « programme », seulement des lignes indépendantes. La règle de départ
 * de cette grammaire est par conséquent `line`.
 *
 * Une règle utilitaire `listing` est fournie plus bas, hors de cette
 * hiérarchie conceptuelle, pour la seule commodité pratique d'analyser
 * d'un bloc un fichier de listage (plusieurs lignes concaténées, comme un
 * fichier `.md` de transcription ou une sortie de la commande LISTER) :
 * elle n'a pas de statut linguistique, voir sa documentation propre.
 */

/*
 * Une ligne LSE (§1) : un numéro (1 à 250 — contrainte SÉMANTIQUE, non
 * imposée ici par la grammaire, cf. NOTES.md [F]), suivi soit d'un
 * commentaire de ligne entière (précédé de '*', espace non obligatoire),
 * soit d'une liste d'instructions séparées par ';', soit de rien du tout
 * (« instruction vide », utilisée notamment comme dernière ligne d'une
 * boucle FAIRE — §5-5.3, remarque 2), le tout terminé par X-OFF.
 *
 * [A] AMBIGUÏTÉ DE TERMINATION — voir NOTES.md.
 * Dans du texte plat (comme cette transcription), le caractère X-OFF n'est
 * en pratique jamais représenté littéralement (sauf dans les exemples qui
 * l'illustrent explicitement). On accepte donc XOFF explicite OU un saut de
 * ligne comme terminateur, en supposant qu'une ligne imprimée = une ligne
 * LSE. C'est vrai pour la quasi-totalité des exemples du manuel, à
 * l'exception des rares cas de retour à la ligne pour la seule mise en page
 * du livre (ex. §5-5.2, second exemple) : dans un tel cas, il faut
 * recoller manuellement les lignes avant de soumettre le texte à cette
 * grammaire, faute de quoi le NEWLINE intermédiaire sera interprété à tort
 * comme la fin de la ligne LSE.
 */
line
    : lineNumber (fullLineComment | statementList)? lineTerminator
    ;

lineNumber
    : NUMBER   // contrainte sémantique : entier 1 <= n <= 250 (§1). Un
               // numéro de ligne est lexicalement une suite de chiffres,
               // indiscernable d'une constante numérique entière : voir
               // NOTES.md [F] pour la justification de la fusion avec le
               // jeton NUMBER (plutôt qu'un jeton INT séparé, qui créerait
               // une ambiguïté lexicale non résolue par ANTLR4 pour une
               // suite de chiffres pure).
    ;

lineTerminator
    : XOFF
    | NEWLINE
    ;

fullLineComment
    : COMMENT
    ;

/*
 * Règle UTILITAIRE, sans statut linguistique (cf. [G] ci-dessus) : simple
 * commodité pour analyser d'un bloc un fichier contenant plusieurs lignes
 * LSE concaténées (un fichier de transcription, une sortie de LISTER...).
 * Elle ne prétend PAS représenter une unité du langage — le LSE, compilé
 * ligne à ligne, n'en connaît aucune au-delà de `line` — et son seul
 * intérêt est de donner un point d'entrée pratique à l'outil ANTLR pour
 * ce cas d'usage précis (par ex. tester ce fichier lui-même d'un coup).
 * Pour analyser le langage à proprement parler, utiliser `line` comme
 * règle de départ, une ligne à la fois et indépendamment des autres.
 */
listing
    : line+ EOF
    ;

/*
 * [J] MODE « MACHINE DE BUREAU » (§9-5) — voir NOTES.md.
 * Une ligne entrée en mode machine de bureau est transmise au même
 * compilateur qu'une ligne de programme, avec la même syntaxe
 * d'expressions, de conditions, et d'instructions, à ceci près :
 *   - elle n'est PAS précédée d'un numéro de ligne ;
 *   - `?` y est un synonyme strict d'`AFFICHER` (même syntaxe, y compris
 *     les formats) — mais UNIQUEMENT dans ce mode : rien dans le manuel
 *     n'atteste son emploi en ligne de programme numérotée, et un test
 *     empirique sur un environnement LSE réel (PC, années 1980) confirme
 *     que `?` y est rejeté à la compilation. `afficherStmt` (ligne de
 *     programme) n'utilise donc PAS `QMARK` ;
 *   - certaines instructions y sont interdites : les instructions de
 *     contrôle de séquence (ALLER EN, FAIRE, RETOUR, RESULTAT, appel de
 *     procédure comme instruction), les déclarations de procédure
 *     (déjà, structurellement, jamais possibles hors tête de ligne —
 *     cf. [H] — donc a fortiori absentes ici), et, spécifiquement sur
 *     Mitra 15, les déclarations de variables (TABLEAU, CHAINE, LIBERER) ;
 *   - conséquence de l'interdiction de RESULTAT : l'appel de procédure-
 *     fonction comme OPÉRANDE d'expression est également interdit,
 *     puisqu'une procédure-fonction ne produit sa valeur que via
 *     RESULTAT (§7-4), inexécutable ici — voir [K], règle `atom`.
 *   - cette interdiction est RÉCURSIVE : un ALLER EN, un FAIRE, un appel
 *     de procédure-fonction, etc. restent interdits même nichés sous un
 *     SI...ALORS ou à l'intérieur d'un DEBUT...FIN, d'où les règles
 *     bureauConditionalStmt et bureauCompoundStmt ci-dessous, qui
 *     rebouclent sur bureauStatement et non sur statement, et
 *     transmettent `allowProc=false` à travers toute la chaîne
 *     d'expression (cf. [K]).
 * Les expressions, elles, sont IDENTIQUES pour le reste (`expr`,
 * `booleanExpr` sont réutilisées telles quelles, seul le paramètre
 * `allowProc` varie selon le contexte d'appel).
 */
bureauLine
    : (bureauStatementList)? lineTerminator
    ;

/*
 * [H] POSITION DE LA DÉCLARATION DE PROCÉDURE — voir NOTES.md.
 * Le §7-6 est explicite : « Une déclaration de procédure doit se trouver
 * au début d'une ligne ». `procedureDecl` n'est donc PAS une alternative
 * de `statement` comme les autres (qui pourrait apparaître n'importe où
 * dans la liste, après un ';' quelconque) : elle ne peut être que la
 * toute première instruction de la ligne, unique, optionnellement suivie
 * d'autres instructions ordinaires après ';' (cf. §7-6, exemples où la
 * ligne de déclaration enchaîne directement sur `RESULTAT ...` ou une
 * autre instruction).
 */
statementList
    : procedureDecl (SEMI statement)*
    | statement (SEMI statement)*
    ;

/*
 * Une "instruction" peut elle-même être vide entre deux ';' consécutifs
 * (rare mais non explicitement interdit) : on l'autorise via l'alternative
 * emptyStatement pour rester permissif, comme l'instruction vide de fin de
 * boucle FAIRE. `procedureDecl` est volontairement ABSENTE de cette liste
 * — voir [H] ci-dessus : elle ne peut apparaître qu'en tête de
 * `statementList`, jamais ici.
 */
statement
    : emptyStatement
    | trailingComment
    | tableauDecl
    | libererDecl
    | chaineDecl
    | assignmentStmt[true]
    | conditionalStmt
    | compoundStmt
    | gotoStmt
    | faireStmt
    | pauseStmt
    | terminerStmt
    | executerStmt[true]
    | lireStmt[true]
    | afficherStmt[true]
    | garerStmt[true]
    | chargerStmt[true]
    | suppressionStmt[true]
    | retourStmt
    | resultatStmt
    | procedureCallStmt
    ;

/*
 * ---- Variantes « mode machine de bureau » (§9-5) — voir [J] plus haut ----
 *
 * `bureauStatementList` / `bureauStatement` reproduisent `statementList` /
 * `statement`, MOINS les instructions interdites dans ce mode :
 *   - contrôle de séquence : gotoStmt, faireStmt, retourStmt, resultatStmt,
 *     procedureCallStmt (l'appel de procédure comme instruction) ;
 *   - déclarations de procédure : déjà absentes de `statement` lui-même
 *     (cf. [H]) — rien à retirer de plus ici ;
 *   - déclarations de variables, spécifiquement sur Mitra 15 :
 *     tableauDecl, chaineDecl, libererDecl. (`LIBERER` est rattachée par
 *     le manuel à la même sous-section que les déclarations de tableaux
 *     — §3-1.2 — et traitée ici comme telle ; à confirmer si une lecture
 *     plus fine du manuel distinguait "déclarer" de "libérer".)
 *
 * Chaque règle partagée avec `statement` (assignmentStmt, executerStmt,
 * lireStmt, afficherStmt, garerStmt, chargerStmt, suppressionStmt) est
 * appelée ici avec `[false]` explicite, afin de propager l'interdiction
 * de l'appel de procédure-fonction comme opérande d'expression jusque
 * dans ses moindres recoins (indices de tableau, arguments de fonction,
 * etc.) — voir [K]. Dans `statement`, ces mêmes règles sont appelées
 * avec `[true]`.
 *
 * `afficherStmt` reste par ailleurs utilisable telle quelle (mais avec
 * `[false]`) : `AFFICHER` seul reste valide en mode machine de bureau,
 * `bureauAfficherStmt` n'ajoute QUE la possibilité d'utiliser `?` à la
 * place.
 */
bureauStatementList
    : bureauStatement (SEMI bureauStatement)*
    ;

bureauStatement
    : emptyStatement
    | trailingComment
    | assignmentStmt[false]
    | bureauConditionalStmt
    | bureauCompoundStmt
    | pauseStmt
    | terminerStmt
    | executerStmt[false]
    | lireStmt[false]
    | afficherStmt[false]
    | bureauAfficherStmt
    | garerStmt[false]
    | chargerStmt[false]
    | suppressionStmt[false]
    ;

/* SI condition ALORS i1 [SINON i2] — restriction RÉCURSIVE au mode
 * machine de bureau (cf. [J]) : i1/i2 sont des bureauStatement, pas des
 * statement, afin qu'un ALLER EN, un FAIRE, un appel de procédure-
 * fonction, etc. restent interdits même niché sous un SI ; `booleanExpr`
 * reçoit `[false]` pour la même raison (cf. [K]). */
bureauConditionalStmt
    : SI booleanExpr[false] ALORS bureauStatement (SINON bureauStatement)?
    ;

/* DEBUT i1;...;in FIN — même restriction récursive. */
bureauCompoundStmt
    : DEBUT bureauStatementList FIN
    ;

/* '?' ou AFFICHER, syntaxe identique par ailleurs (cf. [J]) ; `[false]`
 * explicite pour la même raison que dans `bureauStatement` ci-dessus. */
bureauAfficherStmt
    : QMARK afficherBody[false]
    ;

emptyStatement
    :   // ex. l'instruction "vide" de fin de boucle FAIRE (§5-5.3 rem. 2)
    ;

/* Commentaire après un ';' (§8 : "ou derrière un point-virgule"). */
trailingComment
    : COMMENT
    ;

// ----------------------------------------------------------------------
// Déclarations (§3-1.2, §4-3)
// ----------------------------------------------------------------------

/* TABLEAU t1[i1{,i2}]{,t2[...]}...  (§3-1.2 "Définition des tableaux") */
tableauDecl
    : TABLEAU tableauItem (COMMA tableauItem)*
    ;

tableauItem
    : IDENT LBRACK expr[true] (COMMA expr[true])? RBRACK   // 1 ou 2 dimensions seulement
    ;

/* LIBERER id{,id}...  (§3-1.2 "Libération de tableaux ou de variables") */
libererDecl
    : LIBERER IDENT (COMMA IDENT)*
    ;

/* CHAINE id{,id}...  (§4-3) */
chaineDecl
    : CHAINE IDENT (COMMA IDENT)*
    ;

// ----------------------------------------------------------------------
// Affectation (§3-3, §4-6)
// ----------------------------------------------------------------------

/*
 * [C] UNIFICATION ARITHMÉTIQUE / CHAÎNE — voir NOTES.md.
 * Le manuel distingue "instruction d'affectation arithmétique" (§3-3) et
 * "instruction d'affectation sur chaîne" (§4-6), mais syntaxiquement les
 * deux formes sont identiques : <variable> ← <expression>. La distinction
 * est purement sémantique (type déclaré de la variable — implicitement
 * arithmétique, ou explicitement CHAINE). On unifie donc ici en une seule
 * règle `expr`, le typage étant du ressort d'une passe sémantique
 * postérieure à l'analyse syntaxique (table des symboles).
 */
assignmentStmt[boolean allowProc]
    : lvalue[allowProc] ASSIGN expr[allowProc]
    ;

/* Variable simple ou élément de tableau (§3-1.2). */
lvalue[boolean allowProc]
    : IDENT (LBRACK expr[allowProc] (COMMA expr[allowProc])? RBRACK)?
    ;

// ----------------------------------------------------------------------
// Instructions de contrôle (§5)
// ----------------------------------------------------------------------

/* ALLER EN <expr arithmétique> (§5-1) */
gotoStmt
    : ALLER EN expr[true]
    ;

/*
 * SI condition ALORS i1 [SINON i2]   (§5-2)
 *
 * [E] RATTACHEMENT DU SINON — voir NOTES.md.
 * Le manuel précise explicitement (§5-2, Remarque) : "chaque SINON est
 * associé au SI immédiatement précédent qui n'avait pas encore de SINON
 * associé" — c'est la règle classique du "dangling else", et elle
 * correspond exactement au comportement par défaut d'un parseur LL généré
 * par ANTLR4 pour cette règle récursive (l'alternative avec SINON
 * "consomme" le SINON le plus proche). Aucune action corrective n'est donc
 * nécessaire ici.
 */
conditionalStmt
    : SI booleanExpr[true] ALORS statement (SINON statement)?
    ;

/* DEBUT i1;i2;...;in FIN  (§5-3) */
compoundStmt
    : DEBUT statementList FIN
    ;

/*
 * FAIRE ε1 POUR V ← ε2 [PAS ε3] (JUSQUA ε4 | TANT QUE booleanExpr)  (§5-5)
 * ε1..ε4 sont des expressions arithmétiques (§5-5.1) ; V doit être une
 * variable simple (contrainte sémantique, non vérifiable ici : `lvalue`
 * sans indice).
 */
faireStmt
    : FAIRE expr[true] POUR IDENT ASSIGN expr[true] (PAS expr[true])? faireLimitClause
    ;

faireLimitClause
    : JUSQUA expr[true]           # jusquaClause
    | TANT QUE booleanExpr[true]  # tantQueClause
    ;

/* PAUSE (§5-6.1) / TERMINER (§5-6.2) */
pauseStmt    : PAUSE ;
terminerStmt : TERMINER ;

/* EXECUTER α[,β]  (§5-7.2) — α : expression-chaîne, β : expr arithmétique */
executerStmt[boolean allowProc]
    : EXECUTER expr[allowProc] (COMMA expr[allowProc])?
    ;

// ----------------------------------------------------------------------
// Entrées/Sorties (§6)
// ----------------------------------------------------------------------

/* LIRE v1{,v2}...  (§6-1) — variables ou tableaux entiers */
lireStmt[boolean allowProc]
    : LIRE lireItem[allowProc] (COMMA lireItem[allowProc])*
    ;

lireItem[boolean allowProc]
    : IDENT (LBRACK expr[allowProc] (COMMA expr[allowProc])? RBRACK)?
    ;

/*
 * AFFICHER[format] liste  (§6-2)
 * Le format et la liste peuvent chacun être absents (§6-2 : "suivi
 * éventuellement d'un format... et d'une liste (qui peut être vide)").
 *
 * Le corps (format + liste) est factorisé dans `afficherBody`, partagé
 * avec `bureauAfficherStmt` (mode machine de bureau, où `?` est
 * synonyme de `AFFICHER`, cf. [J] et la règle `bureauLine` plus bas) :
 * la syntaxe du format et de la liste est rigoureusement identique dans
 * les deux cas, seul le mot introducteur diffère.
 */
afficherStmt[boolean allowProc]
    : AFFICHER afficherBody[allowProc]
    ;

afficherBody[boolean allowProc]
    : (LBRACK formatSpecList RBRACK)? displayList[allowProc]?
    ;

formatSpecList
    : formatSpec (COMMA formatSpec)*
    ;

/*
 * [O] FACTEUR DE RÉPÉTITION VARIABLE INTERDIT DEVANT UNE DESCRIPTION —
 * voir NOTES.md. Absent du manuel Mitra 15 (EAT, 1978) transcrit — cette
 * restriction est confirmée par TROIS passages concordants et explicites
 * du manuel INRDP 1975 :
 *   - §7-3 : « Les spécifications de description ne peuvent être
 *     précédées que d'un facteur de répétition FIXE. »
 *   - §7-5 : « À la place d'un facteur de répétition fixe, précédant une
 *     spécification de LIBELLÉ OU DE MISE EN PAGE, on peut mettre * »
 *     (la description n'est pas mentionnée).
 * Une première version de cette grammaire autorisait `repeatFactor`
 * (NUMBER ou MUL indifféremment) devant les trois natures de
 * spécification. `formatSpec` distingue donc désormais explicitement le
 * cas de la description (facteur FIXE uniquement, `fixedRepeatFactor`)
 * du cas libellé/mise en page (facteur fixe OU variable, `repeatFactor`
 * inchangée).
 */
formatSpec
    : repeatFactor? labelSpec          # labelFormatSpec
    | repeatFactor? layoutSpec         # layoutFormatSpec
    | fixedRepeatFactor? descSpec      # descFormatSpec
    ;

/* Facteur fixe ou variable (§6-2.6 / §7-5) — libellé et mise en page. */
repeatFactor
    : NUMBER  // 1 à 99, entier (contrainte sémantique, §6-2.1) — même
              // remarque que pour lineNumber, cf. NOTES.md [F]
    | MUL     // '*' : facteur de répétition variable (§6-2.6) — même jeton
              // que la multiplication, sans ambiguïté réelle puisque cette
              // règle n'est atteignable que depuis formatSpecList (§6-2)
    ;

/* Facteur FIXE uniquement (§7-3) — description, jamais '*' — cf. [O]. */
fixedRepeatFactor
    : NUMBER
    ;

/* Spécification de libellé : chaîne entre apostrophes (§6-2.1). */
labelSpec
    : STRING
    ;

/* Spécification de mise en page : / X C L (§6-2.2). */
layoutSpec
    : DIV     // '/' : passage au début de la ligne suivante (§6-2.2)
    | IDENT   // doit valoir 'X', 'C' ou 'L' — cf. NOTES.md [B]
    ;

/*
 * Spécification de description : Ee.d | Fe.d | U (§6-2.3).
 * DESC_NUM est un jeton dédié du lexeur pour le motif "lettre+digits.digits"
 * (voir règle lexicale plus bas) ; U reste un IDENT ordinaire — cf.
 * NOTES.md [B].
 */
descSpec
    : DESC_NUM   // ex. E3.4, F2.1, F0.0
    | IDENT      // doit valoir 'U'
    ;

displayList[boolean allowProc]
    : displayItem[allowProc] (COMMA displayItem[allowProc])*
    ;

displayItem[boolean allowProc]
    : expr[allowProc]   // expression arithmétique, expression-chaîne, ou nom de tableau
    ;

/* GARER α,β,γ  (§6-3.2) : α ident (tableau/var/chaîne), β expr arith, γ expr-chaîne */
garerStmt[boolean allowProc]
    : GARER IDENT COMMA expr[allowProc] COMMA expr[allowProc]
    ;

/* CHARGER α,β,γ[,δ]  (§6-3.2) : δ optionnel, variable arithmétique */
chargerStmt[boolean allowProc]
    : CHARGER IDENT COMMA expr[allowProc] COMMA expr[allowProc] (COMMA IDENT)?
    ;

/*
 * [M] INSTRUCTION SUPPRIMER (Mitra 15 uniquement) — voir NOTES.md.
 * Absente du manuel Mitra 15 (EAT, 1978) transcrit — confirmée comme une
 * omission involontaire de ce manuel par recoupement avec le manuel
 * INRDP 1975 (§2-3-3 : "cette instruction n'existe que sur MITRA 15" ;
 * confirmée aussi par l'Annexe VI des mots réservés de ce même manuel).
 * `SUPPRIMER γ` supprime tout le fichier γ ; `SUPPRIMER γ,β` supprime le
 * seul enregistrement β du fichier γ. γ et β ont exactement la même
 * nature que pour GARER (γ : expression-chaîne = nom de fichier ; β :
 * expression arithmétique = numéro d'enregistrement, ici optionnel).
 * Placée logiquement aux côtés de GARER/CHARGER : ni contrôle de
 * séquence, ni déclaration, donc autorisée en mode machine de bureau
 * comme ses deux voisines (cf. [J], bureauStatement).
 */
suppressionStmt[boolean allowProc]
    : SUPPRIMER expr[allowProc] (COMMA expr[allowProc])?
    ;

// ----------------------------------------------------------------------
// Procédures (§7)
// ----------------------------------------------------------------------

/*
 * PROCEDURE &nom(p1,...,pn) [LOCAL l1,...,lm]   (§7-2)
 * Une déclaration de procédure n'est jamais exécutée directement ; le
 * reste de la ligne (après ';') peut contenir n'importe quelle instruction
 * LSE ordinaire (§7-6, ex. `PROCEDURE &FORM();RESULTAT RAC(...)`).
 *
 * [H] POSITION EN DÉBUT DE LIGNE — voir NOTES.md.
 * Le §7-6 impose : « Une déclaration de procédure doit se trouver au
 * début d'une ligne ». `procedureDecl` n'apparaît donc PAS parmi les
 * alternatives de `statement` (qui pourraient survenir n'importe où dans
 * la liste) : elle n'est autorisée qu'en toute première position de
 * `statementList`, voir la règle `statementList` elle-même.
 */
procedureDecl
    : PROCEDURE PROC_IDENT LPAREN formalParamList? RPAREN localClause?
    ;

formalParamList
    : formalParam (COMMA formalParam)*
    ;

/* Un paramètre formel est un identificateur simple ou un nom de procédure
 * (pour recevoir une procédure-fonction en paramètre, §7-8.2). */
formalParam
    : IDENT
    | PROC_IDENT
    ;

localClause
    : LOCAL localItem (COMMA localItem)*
    ;

/*
 * [I] UN PARAMÈTRE LOCAL NE PEUT JAMAIS ÊTRE UN NOM DE PROCÉDURE — voir
 * NOTES.md. Le §7-8.2 est explicite : on utilise un paramètre formel
 * *référence* (c'est-à-dire absent de la liste LOCAL) précisément
 * "quand on veut utiliser un nom de procédure comme paramètre". Un nom
 * de procédure ne peut donc, par construction, jamais figurer dans la
 * liste qui suit LOCAL — contrairement à `formalParam`, qui accepte les
 * deux catégories (paramètres locaux ET référence) puisqu'elle énumère
 * l'ensemble des paramètres formels de la procédure, quelle que soit
 * leur catégorie.
 */
localItem
    : IDENT
    ;

/* RETOUR | RETOUR EN <expr arithmétique>  (§7-4, §7-5) */
retourStmt
    : RETOUR (EN expr[true])?
    ;

/* RESULTAT <expr>  (§7-4) */
resultatStmt
    : RESULTAT expr[true]
    ;

/* Appel de procédure-sous-programme utilisé comme instruction (§7-6.2). */
procedureCallStmt
    : procedureCall
    ;

/* &nom(p1,...,pn)  (§7-6) — utilisé comme instruction OU comme opérande
 * d'expression (procédure-fonction, §7-6.1) ; voir règle `expr`. */
procedureCall
    : PROC_IDENT LPAREN effectiveParamList? RPAREN
    ;

effectiveParamList
    : effectiveParam (COMMA effectiveParam)*
    ;

/*
 * Un paramètre effectif est soit une expression (cas "local", §7-8.1),
 * soit un identificateur simple passé par référence : nom de tableau, nom
 * de variable/variable-chaîne à modifier, ou nom de procédure (§7-8.2).
 * `expr` couvre déjà le cas d'un simple IDENT (règle varExpr), donc une
 * seule alternative suffit pour les cas "valeur" ; PROC_IDENT est ajouté
 * séparément car un nom de procédure n'est pas une `expr` valide en soi
 * (il ne peut apparaître seul, sans être appelé, que comme paramètre).
 */
effectiveParam
    : expr[true]
    | PROC_IDENT
    ;

// ----------------------------------------------------------------------
// Expressions (§3-2, §3-3, §4-4, §4-5, §5-4)
// ----------------------------------------------------------------------

/*
 * Expression booléenne (§5-4) : combinaison de "conditions" via ET / OU.
 * Priorité : ET > OU (comme * > + en arithmétique) ; évaluation gauche à
 * droite au sein d'un même niveau (§5-4).
 *
 * [L] RÉÉCRITURE EN PALIERS EXPLICITES (PAS DE RÉCURSION GAUCHE) — voir
 * NOTES.md. Une première version de cette règle utilisait la récursion
 * gauche automatique d'ANTLR4 (quatre alternatives dont `andExpr` et
 * `orExpr` gauche-récursives). Lorsque le paramètre `allowProc` a été
 * introduit (cf. [K]) pour propager l'interdiction de l'appel de
 * procédure-fonction jusque dans les conditions, la compilation a échoué
 * : ANTLR4 **ne supporte pas les paramètres de règle sur une règle
 * gauche-récursive**, même sans valeur par défaut et même si le
 * paramètre n'est pas utilisé dans l'alternative récursive elle-même —
 * limitation réelle de l'outil, vérifiée empiriquement y compris sur
 * l'exemple canonique du manuel ANTLR4 lui-même (`expr[int x] : expr[x]
 * '+' expr[x] | INT ;`, qui échoue aussi à la compilation dans cette
 * version d'ANTLR).
 *
 * `booleanExpr` est donc réécrite en paliers de précédence EXPLICITES
 * (`booleanExpr` → `booleanTerm` → `booleanFactor`), exactement la même
 * technique déjà appliquée à l'expression arithmétique après le bug [D]
 * — chaque palier utilise le motif itératif `(OP palier-inférieur)*`,
 * qui n'est PAS une règle gauche-récursive au sens d'ANTLR4 et accepte
 * donc les paramètres sans restriction. Le comportement observable est
 * rigoureusement identique (même précédence ET > OU, même évaluation
 * gauche à droite) — vérifié empiriquement par les mêmes tests que la
 * version précédente.
 */
booleanExpr[boolean allowProc]
    : booleanTerm[allowProc] (OU booleanTerm[allowProc])*
    ;

/* Rang le plus fort : ET. */
booleanTerm[boolean allowProc]
    : booleanFactor[allowProc] (ET booleanFactor[allowProc])*
    ;

/* Rang le plus haut : une condition, ou une sous-expression booléenne
 * parenthésée (les parenthèses réinitialisent la précédence, comme pour
 * `atom` côté arithmétique). */
booleanFactor[boolean allowProc]
    : LPAREN booleanExpr[allowProc] RPAREN
    | condition[allowProc]
    ;

/*
 * Une "condition" (§5-2) compare deux expressions de même nature
 * (arithmétique/arithmétique ou chaîne/chaîne) via un opérateur de
 * relation. Cette contrainte de même nature est sémantique (cf. [C]).
 */
condition[boolean allowProc]
    : expr[allowProc] relOp expr[allowProc]
    ;

relOp
    : LT | LE | EQ | NE | GE | GT
    ;

/*
 * Expression unifiée arithmétique / chaîne — voir [C] plus haut.
 *
 * [D] PRÉCÉDENCE ET RESTRICTION DU CHANGEMENT DE SIGNE — voir NOTES.md.
 * Le §3-3.4 place le "changement de signe" (moins unaire) au MÊME rang
 * hiérarchique que + et - binaires (rang 4, le plus bas des opérateurs
 * arithmétiques), rang strictement plus bas que * / (rang 3) et ↑
 * (rang 2). Conséquence explicitement énoncée par le manuel : une
 * expression comme "A*-B" est ILLÉGALE (il faut écrire "A*(-B)"), car le
 * signe moins unaire ne peut apparaître directement comme opérande d'un
 * opérateur de rang supérieur sans parenthèses.
 *
 * Une première version de cette grammaire utilisait le mécanisme standard
 * de précédence automatique d'ANTLR4 pour les règles gauche-récursives
 * (une seule règle `expr` avec alternatives ordonnées). Un test réel
 * (voir NOTES.md, section "Validation empirique") a montré que ce
 * mécanisme, bien qu'il fonctionne pour les opérateurs INFIXES, NE BRIDE
 * PAS un opérateur PRÉFIXE (comme le moins unaire) empêchant son emploi
 * comme opérande direct d'un opérateur infixe plus prioritaire : "A*-B"
 * était accepté à tort. On modélise donc ici les paliers de précédence
 * EXPLICITEMENT, règle par règle (technique classique en dehors du
 * mécanisme automatique d'ANTLR4), ce qui reproduit fidèlement la
 * contrainte du manuel : `mulDivTerm` ne peut prendre pour second
 * opérande qu'un `powTerm` (jamais un `signedFactor`), donc "A*-B" est
 * rejeté sans parenthèses, exactement comme spécifié — vérifié
 * empiriquement (cf. NOTES.md).
 *
 * Toutes les opérations de même niveau s'évaluent de gauche à droite
 * (§3-3.4), d'où l'associativité gauche de chaque palier (répétition
 * `(OP atome-du-palier-inférieur)*`), y compris pour ↑ : le manuel ne
 * prévoit aucune exception d'associativité droite pour l'exponentiation.
 */
expr[boolean allowProc]
    : concatTerm[allowProc]                              # baseExpr
    | SI booleanExpr[allowProc] ALORS expr[allowProc] SINON expr[allowProc]    # condExpr
    ;

/* Concaténation de chaînes (§4-4.1) — hors classement du §3-3.4 (qui ne
 * concerne que l'arithmétique) ; placée ici au niveau juste sous le SI
 * conditionnel, par choix arbitraire documenté (cf. NOTES.md [D]) puisque
 * concaténation et opérateurs arithmétiques ne se mélangent jamais dans
 * un programme valide. */
concatTerm[boolean allowProc]
    : addSubTerm[allowProc] (CONCAT addSubTerm[allowProc])*
    ;

/*
 * Rang 4 (le plus bas) : addition, soustraction, changement de signe.
 *
 * [P] RÈGLE GÉNÉRALE « PAS DEUX OPÉRATEURS ADJACENTS » — voir NOTES.md.
 * Absente du manuel Mitra 15 (EAT, 1978) transcrit, qui ne travaille
 * explicitement que le cas "A*-B illégal". Le manuel INRDP 1975 (§4-3,
 * p. 33) énonce la règle de portée GÉNÉRALE dont ce cas n'est qu'une
 * instance : « Une expression arithmétique ne peut pas contenir deux
 * opérateurs adjacents. » Cette règle est retenue ici dans toute sa
 * généralité (et non restreinte au seul "*"), donc appliquée également
 * à + et - : seul le TOUT PREMIER opérande d'une chaîne addition/
 * soustraction peut porter un signe unaire optionnel (`signedFactor`) ;
 * chaque opérande SUIVANT, après un + ou un -, est un `mulDivTerm` SANS
 * signe optionnel — sans quoi "A+-B" ou "A- -B" présenteraient deux
 * jetons opérateurs consécutifs, exactement comme "A*-B". Il faut donc
 * désormais écrire "A+(-B)". Un signe unaire en tête d'une expression
 * fraîchement ouverte (début de ligne, après '(', après SI/ALORS/SINON)
 * reste licite : ce n'est jamais précédé d'un autre opérateur, donc pas
 * "adjacent" au sens du manuel — d'où l'usage de `signedFactor`
 * uniquement pour le PREMIER opérande de la chaîne.
 */
addSubTerm[boolean allowProc]
    : signedFactor[allowProc] ((PLUS | MINUS) mulDivTerm[allowProc])*
    ;

/* Un signe unaire optionnel ne peut porter que sur un `mulDivTerm`
 * complet (ex. "-A*B" = -(A*B)), jamais apparaître comme second opérande
 * direct d'un * ou d'un / (d'où l'absence de `signedFactor` dans
 * `mulDivTerm` ci-dessous) — ni, depuis [P], comme opérande d'un + ou
 * d'un - autre que le tout premier de la chaîne. */
signedFactor[boolean allowProc]
    : (PLUS | MINUS)? mulDivTerm[allowProc]
    ;

/* Rang 3 : multiplication, division. */
mulDivTerm[boolean allowProc]
    : powTerm[allowProc] ((MUL | DIV) powTerm[allowProc])*
    ;

/* Rang 2 : exponentiation. */
powTerm[boolean allowProc]
    : atom[allowProc] (POW atom[allowProc])*
    ;

/* Rang 1 (le plus haut) : atomes — appel de fonction, appel de procédure-
 * fonction, variable/élément de tableau, littéral, ou sous-expression
 * parenthésée (les parenthèses réinitialisent la précédence : à
 * l'intérieur, `expr` complet est de nouveau accessible, y compris SI, le
 * signe unaire en position quelconque, etc.).
 *
 * [K] APPEL DE PROCÉDURE-FONCTION INTERDIT EN MODE MACHINE DE BUREAU —
 * voir NOTES.md. Une procédure-fonction ne produit sa valeur que par une
 * instruction RESULTAT (§7-4) ; or RESULTAT est structurellement
 * inexécutable en mode machine de bureau (cf. [J], bureauStatement). Un
 * appel de procédure comme OPÉRANDE d'expression y est donc, par voie de
 * conséquence, tout aussi impossible — pas seulement non vérifiable, mais
 * incohérent avec le mode lui-même. Le paramètre `allowProc` (propagé
 * depuis `expr` à travers tous les paliers de précédence, valeur par
 * défaut `true` pour ne rien changer aux lignes de programme) gate donc
 * l'alternative `procCallAtom` par un prédicat sémantique
 * `{allowProc}?` ; `bureauStatement` et ses règles satellites
 * transmettent `false` à la place. Vérifié empiriquement.
 */
atom[boolean allowProc]
    : LPAREN expr[allowProc] RPAREN     # parenAtom
    | functionCall[allowProc]           # funcCallAtom
    | {$allowProc}? procedureCall       # procCallAtom
    | lvalue[allowProc]                 # varAtom
    | lineRefLiteral                    # lineRefAtom
    | NUMBER                            # numberAtom
    | STRING                            # stringLiteralAtom
    ;

/*
 * [N] SYMBOLE @ (Mitra 15 uniquement) — voir NOTES.md.
 * Absent du manuel Mitra 15 (EAT, 1978) transcrit — confirmé comme une
 * omission involontaire de ce manuel par recoupement avec le manuel
 * INRDP 1975, Annexe III point 3 : « sur MITRA 15, @ signale au système
 * que la valeur d'une variable représente un numéro de ligne », utile
 * "dans le cas d'une renumérotation" (ex. `10 I←@50` puis `20 ALLER EN
 * I`, qui branche en ligne 50 même si la ligne 50 a depuis été
 * renumérotée : c'est le contenu marqué "@", pas la position textuelle,
 * qui est suivi).
 *
 * Portée délibérément restreinte à un ENTIER LITTÉRAL, et non à une
 * expression arithmétique quelconque : le manuel ne fournit qu'un seul
 * exemple (`@50`, un simple entier), sans jamais illustrer `@` devant
 * une variable ou une expression composée — et l'intérêt même de la
 * construction (marquer un numéro de ligne FIXE en toutes lettres dans
 * le texte source pour qu'il survive à une renumérotation) suppose que
 * ce soit un littéral écrit en clair, pas une valeur calculée à
 * l'exécution. Restreindre `lineRefLiteral` à NUMBER plutôt qu'à `expr`
 * est donc un choix délibéré de fidélité au texte plutôt qu'une
 * généralisation non attestée.
 */
lineRefLiteral
    : AT NUMBER   // NUMBER : contrainte sémantique, doit être un entier
                  // 1-250 valide comme numéro de ligne (cf. [F])
    ;


/*
 * Fonctions élémentaires à résultat numérique (§3-2.2) et fonctions sur
 * chaînes (§4-4.2, 11 fonctions au total : 5 à résultat chaîne, 6 à
 * résultat numérique). Le paramètre `allowProc` est propagé à chaque
 * argument (cf. [K]) : un appel de procédure-fonction imbriqué dans un
 * argument, ex. `SIN(&FACT(3))`, doit rester tout aussi interdit en mode
 * machine de bureau qu'un appel non imbriqué.
 */
functionCall[boolean allowProc]
    : (SIN | COS | ATG | LGN | EXP | RAC | ABS | ENT | ALE) LPAREN expr[allowProc] RPAREN        # elemFuncCall
    | TEM LPAREN RPAREN                                                                           # temFuncCall
    | DAT LPAREN RPAREN                                                                           # datFuncCall
    | SCH LPAREN expr[allowProc] COMMA expr[allowProc] COMMA expr[allowProc] (COMMA IDENT)? RPAREN # schFuncCall
    | GRL LPAREN expr[allowProc] COMMA expr[allowProc] (COMMA IDENT)? RPAREN                       # grlFuncCall
    | CCA LPAREN expr[allowProc] RPAREN                                                            # ccaFuncCall
    | EQC LPAREN expr[allowProc] RPAREN                                                            # eqcFuncCall
    | LGR LPAREN expr[allowProc] RPAREN                                                            # lgrFuncCall
    | POS LPAREN expr[allowProc] COMMA expr[allowProc] COMMA expr[allowProc] RPAREN                # posFuncCall
    | CNB LPAREN expr[allowProc] COMMA expr[allowProc] (COMMA IDENT)? RPAREN                       # cnbFuncCall
    | EQN LPAREN expr[allowProc] (COMMA expr[allowProc])? RPAREN                                   # eqnFuncCall
    | PTR LPAREN expr[allowProc] COMMA expr[allowProc] (COMMA expr[allowProc])? RPAREN             # ptrFuncCall
    | SKP LPAREN expr[allowProc] COMMA expr[allowProc] (COMMA expr[allowProc])? RPAREN             # skpFuncCall
    ;

// ============================================================================
// RÈGLES DU LEXEUR
// ============================================================================

// ---- Mots-clés des instructions (§2 à §8) -----------------------------
// Déclarés AVANT la règle IDENT pour que le lexeur ANTLR4 leur donne
// priorité en cas d'égalité de longueur (règle du "premier arrivé" pour
// les jetons de même longueur de correspondance).
TABLEAU    : 'TABLEAU' ;
LIBERER    : 'LIBERER' ;
CHAINE     : 'CHAINE' ;
SI         : 'SI' ;
ALORS      : 'ALORS' ;
SINON      : 'SINON' ;
DEBUT      : 'DEBUT' ;
FIN        : 'FIN' ;
ALLER      : 'ALLER' ;
EN         : 'EN' ;
FAIRE      : 'FAIRE' ;
POUR       : 'POUR' ;
PAS        : 'PAS' ;
JUSQUA     : 'JUSQUA' ;
TANT       : 'TANT' ;
QUE        : 'QUE' ;
ET         : 'ET' ;
OU         : 'OU' ;
PAUSE      : 'PAUSE' ;
TERMINER   : 'TERMINER' ;
EXECUTER   : 'EXECUTER' ;
LIRE       : 'LIRE' ;
AFFICHER   : 'AFFICHER' ;
GARER      : 'GARER' ;
CHARGER    : 'CHARGER' ;
SUPPRIMER  : 'SUPPRIMER' ;
PROCEDURE  : 'PROCEDURE' ;
LOCAL      : 'LOCAL' ;
RETOUR     : 'RETOUR' ;
RESULTAT   : 'RESULTAT' ;

// ---- Fonctions élémentaires (§3-2.2) -----------------------------------
SIN : 'SIN' ;
COS : 'COS' ;
ATG : 'ATG' ;
LGN : 'LGN' ;
EXP : 'EXP' ;
RAC : 'RAC' ;
ABS : 'ABS' ;
ENT : 'ENT' ;
ALE : 'ALE' ;
TEM : 'TEM' ;

// ---- Fonctions sur chaînes (§4-4.2) ------------------------------------
SCH : 'SCH' ;
GRL : 'GRL' ;
CCA : 'CCA' ;
EQC : 'EQC' ;
DAT : 'DAT' ;
LGR : 'LGR' ;
POS : 'POS' ;
CNB : 'CNB' ;
EQN : 'EQN' ;
PTR : 'PTR' ;
SKP : 'SKP' ;

// ---- Opérateurs et ponctuation -----------------------------------------
ASSIGN  : '\u2190' ;  // ←  (§3-3 : signe d'affectation)
QMARK   : '?' ;        // synonyme de AFFICHER, mode machine de bureau
                        // UNIQUEMENT (cf. [J]) — jamais en ligne de
                        // programme numérotée.
POW     : '\u2191' ;  // ↑  (§3-2.1 : exponentiation)
CONCAT  : '!' ;        // §4-4.1 : opérateur de concaténation
AT      : '@' ;        // sens Mitra 15 uniquement (cf. [N]) — marqueur
                        // de numéro de ligne, ex. I←@50
PLUS    : '+' ;
MINUS   : '-' ;
MUL     : '*' ;        // réutilisé comme facteur de répétition, cf. formatSpec
DIV     : '/' ;
LT      : '<' ;
LE      : '<=' ;
EQ      : '=' ;
NE      : '#' ;
GE      : '>=' ;
GT      : '>' ;
LPAREN  : '(' ;
RPAREN  : ')' ;
LBRACK  : '[' ;
RBRACK  : ']' ;
COMMA   : ',' ;
SEMI    : ';' ;
DOT     : '.' ;
// NB : le '/' de mise en page (§6-2.2, "passage à la ligne suivante") est
// le MÊME caractère que l'opérateur de division ; on réutilise donc le
// jeton DIV dans la règle layoutSpec plutôt que de créer un second jeton
// pour le même caractère (ce qui rendrait le lexeur ambigu) — cf. NOTES.md [B].

// ---- Identificateurs (§3-1.2, §7-2) ------------------------------------
// Variable/tableau simple : 1 lettre + au plus 4 lettres/chiffres (5 max).
IDENT
    : LETTRE (LETTRE | CHIFFRE)? (LETTRE | CHIFFRE)? (LETTRE | CHIFFRE)? (LETTRE | CHIFFRE)?
    ;

// Nom de procédure : '&' + 1 lettre + au plus 4 lettres/chiffres (§7-2).
PROC_IDENT
    : '&' LETTRE (LETTRE | CHIFFRE)? (LETTRE | CHIFFRE)? (LETTRE | CHIFFRE)? (LETTRE | CHIFFRE)?
    ;

// ---- Spécification de description numérique (§6-2.3) -------------------
// Ee.d | Fe.d avec 0<=e<100, 0<=d<100 (1 ou 2 chiffres chacun). Jeton
// dédié : voir NOTES.md [B] pour la justification de ce choix.
DESC_NUM
    : [EF] CHIFFRE CHIFFRE? DOT CHIFFRE CHIFFRE?
    ;

// ---- Constantes numériques (§3-1.1) -------------------------------------
// NB : le signe (+/-) n'est PAS inclus dans ce jeton malgré le tableau du
// §3-1.1 qui présente "-5" comme "nombre LSE" — voir NOTES.md [D] pour la
// justification (le signe est traité comme opérateur unaire par `expr`,
// ce qui est cohérent avec la règle de priorité du §3-3.4 : "changement de
// signe" y est listé comme une OPÉRATION, au même rang que + et -
// binaires, non comme faisant partie du littéral numérique).
//
// Le point décimal peut ne pas être suivi de chiffres (mantisse du type
// "3." acceptée, pas seulement "3.0") : d'où `DOT CHIFFRE*` (étoile),
// et non `DOT CHIFFRE+` (plus), pour la partie fractionnaire.
NUMBER
    : CHIFFRE+ (DOT CHIFFRE*)? (E_LETTER (PLUS | MINUS)? CHIFFRE+)?
    ;
fragment E_LETTER : 'E' ;

// ---- Constantes-chaîne (§4-2) --------------------------------------------
// Encadrées d'apostrophes ; une apostrophe interne s'écrit '' (doublée).
STRING
    : '\'' ( '\'\'' | ~['] )* '\''
    ;

/*
 * ---- Commentaires (§8) -----------------------------------------------
 * "précédé d'un astérisque, en début de ligne (espace non obligatoire), ou
 * derrière un point-virgule" ; se termine par X-OFF (ici : fin de ligne).
 *
 * [B] AMBIGUÏTÉ LEXICALE RÉELLE '*' COMMENTAIRE / '*' MULTIPLICATION.
 * Contrairement à une première intuition, cette ambiguïté N'EST PAS
 * résolue automatiquement par le simple fait que `statement` et `expr`
 * soient des règles de PARSEUR distinctes : le LEXEUR ANTLR4 tokenise le
 * flux de caractères en une passe entièrement séparée et antérieure à
 * l'analyse syntaxique, par plus-longue-correspondance ; sans garde-fou,
 * TOUT '*' serait donc happé comme début de COMMENT jusqu'à la fin de la
 * ligne, y compris en pleine expression arithmétique (ex. "2*(3-1)").
 *
 * On lève cette ambiguïté avec la technique standard ANTLR4 du prédicat
 * sémantique piloté par un drapeau maintenu dans une redéfinition de
 * nextToken() (@lexer::members ci-dessous) : le drapeau `commentAllowed`
 * n'est vrai qu'immédiatement après un ';', un NEWLINE, un X-OFF, ou en
 * tout début de source — c'est-à-dire exactement aux positions où le §8
 * autorise un commentaire. Le prédicat {commentAllowed}? doit être
 * satisfait pour que la règle COMMENT s'applique ; sinon, '*' est reconnu
 * comme MUL.
 *
 * NB pour la relecture/le test : les outils d'interprétation pure (ex.
 * `antlr4-parse`, utilisé pour les essais manuels documentés dans
 * NOTES.md) n'exécutent PAS les actions/prédicats du langage cible
 * (ici Java) et ne peuvent donc pas valider ce mécanisme à l'exécution ;
 * seule une génération réelle (antlr4 -Dlanguage=Java, puis compilation)
 * l'exerce complètement. La grammaire a néanmoins été vérifiée exempte
 * d'erreur de génération avec ce mécanisme en place. Le bloc
 * @lexer::members correspondant est déclaré en tête de grammaire (juste
 * après `grammar LSE15Mitra;`), comme l'exige ANTLR4.
 */

COMMENT
    : {commentAllowed}? '*' ~[\r\n]*
    ;

// ---- Terminateurs, espaces (§1) --------------------------------------------
XOFF : 'X-OFF' ;

NEWLINE : '\r'? '\n' ;

WS : [ \t]+ -> skip ;

// ---- Fragments -------------------------------------------------------------
fragment LETTRE  : [A-Z] ;
fragment CHIFFRE : [0-9] ;
