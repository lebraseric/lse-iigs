# Tome I — Architecture historique du système L.S.E.

# Préface

Le présent tome appartient à la collection *Reconstruction historique du Langage Symbolique d'Enseignement (L.S.E.)*. Il constitue une édition documentaire consacrée à l'architecture historique du système L.S.E. et a pour objet de rassembler, d'organiser et de présenter les éléments qui peuvent être établis à partir des sources historiques retenues pour la collection. Il ne constitue ni une spécification du langage, ni une spécification d'implémentation, ni une proposition d'architecture, ni une reconstruction technique des mécanismes internes du système. [CONV-ED1 §1] [CONV-ED1 §2] [CONV-ED1 §11]

La documentation historique du L.S.E. se trouve répartie dans des documents de nature différente. Les manuels d'utilisation décrivent le langage et son environnement tels qu'ils sont présentés à leurs utilisateurs, avec des objectifs pédagogiques et pratiques propres à leur fonction documentaire. Le récit historique publié par Yves Noyelle en 1989 revient, pour sa part, sur l'histoire des langages de la famille LSx et sur les conditions de leur conception, de leur réalisation et de leurs évolutions. La Grammaire LSE est retenue comme source de syntaxe. Ces documents n'ont donc ni le même objet documentaire, ni le même niveau de description, ni la même relation temporelle avec les systèmes qu'ils décrivent. [INRDP75 §« Avant-propos »] [INRDP75 §« Remarque sur la présentation du fascicule »] [LSE15 §1] [NOYELLE89 p.216] [CONV-ED1 §11]

Le Manuel d'utilisation L.S.E. publié par l'Institut national de recherche et de documentation pédagogiques en janvier 1975 se présente comme un document destiné à la fois à l'apprentissage et à l'utilisation du langage. Son *Avant-propos* expose le double objectif assigné au manuel : constituer un document pratiquement autodidactique pour des personnes possédant seulement des notions générales d'informatique et permettre à un programmeur de retrouver facilement le libellé exact d'une instruction ou d'une fonction. Le document précise que son public visé comprend les élèves et les enseignants de toutes disciplines des lycées. [INRDP75 §« Avant-propos »]

Ce même manuel indique que sa deuxième édition a été élaborée à partir de l'expérience de la première édition et que des modifications importantes ont été introduites, notamment pour mieux présenter la gestion des fichiers, le fonctionnement du système en temps partagé, les traitements sur les chaînes de caractères et les procédures. Il précise également que sa rédaction a été achevée en décembre 1974 et qu'il présente les caractéristiques alors connues des deux systèmes L.S.E. existants mentionnés dans le fascicule : la version 4 du système L.S.E. sur T1600 et la version 8 du système L.S.E. sur Mitra 15. [INRDP75 §« Avant-propos »] [INRDP75 §« Remarque sur la présentation du fascicule »]

Le Manuel d'utilisation LSE 15 de 1978 présente un autre état documentaire du langage. Son organisation comporte notamment des parties consacrées aux généralités, aux instructions arithmétiques, aux instructions sur les chaînes de caractères, aux instructions de contrôle, aux entrées et sorties, aux procédures, aux commentaires et au langage de commande. Dans ses généralités, il décrit le Langage Symbolique d'Enseignement comme destiné à programmer des problèmes numériques ou portant sur des chaînes de caractères et conçu pour une utilisation conversationnelle. Il associe cette utilisation conversationnelle à une structure de lignes dans laquelle chaque ligne est indépendante syntaxiquement des lignes suivantes ou précédentes du programme. [LSE15 §Sommaire] [LSE15 §1]

Le manuel de 1978 décrit également la ligne comme l'unité prise en compte lors de l'entrée du programme. La ligne de programme comprend un numéro, une ou plusieurs instructions et un caractère signalant la fin de la ligne ; le document indique que chaque ligne est analysée dès l'envoi de ce caractère et qu'une erreur de syntaxe est alors signalée immédiatement. Lorsqu'une ligne est syntaxiquement correcte, elle est mémorisée et replacée selon l'ordre croissant de son numéro ; une ligne nouvelle portant le même numéro remplace la ligne précédente. [LSE15 §1]

Le Manuel d'utilisation L.S.E. de 1975 présente de manière comparable une organisation en lignes et précise que chaque ligne est compilée par le système dès l'envoi du caractère de fin de ligne. Il distingue l'analyse syntaxique de la ligne des erreurs qui ne peuvent être détectées qu'à l'exécution. Il décrit en outre les mécanismes du système sous la forme de processeurs distincts, notamment le compilateur, le décompilateur, l'interpréteur, le bibliothécaire et l'analyseur de commandes. [INRDP75 §3] [INRDP75 §3.3] [INRDP75 §« Présentation d'un programme L.S.E. »]

Le récit de Yves Noyelle, publié en 1989 dans le *Bulletin de l'EPI* sous le titre « La saga du LSE et de sa famille (LSD/LSG/LST) », apporte une source de nature différente. L'auteur y retrace le développement des langages LSD, LSE, LSG et LST et situe leur développement dans la période 1968-1976 à l'École Supérieure d'Électricité. Le texte expose successivement l'environnement historique, la conception de LSD, l'histoire de LSE et les évolutions ultérieures de cette famille de langages. [NOYELLE89 p.216] [NOYELLE89 p.223-232]

Ce récit permet notamment de situer la genèse du LSE dans la continuité du LSD. Il indique que l'origine du projet LSE se rattache au colloque CERI/OCDE de Sèvres de mars 1970, au cours duquel fut discuté l'apport possible de l'informatique à l'enseignement. Il rapporte que le LSD fut alors proposé comme langage de support à l'introduction de l'informatique dans l'enseignement, avant que son nom et son domaine d'emploi ne soient modifiés pour donner naissance au LSE. [NOYELLE89 p.223-224]

Le même récit distingue la définition du langage et la réalisation des systèmes destinés à l'exploiter. Il mentionne le rapport de définition du LSE remis au Ministère de l'Éducation Nationale en octobre 1971, puis la réalisation du système de programmation et du système d'exploitation en temps partagé sur les mini-ordinateurs T1600 de Télémécanique et MITRA-15 de SEMS. Il indique également que les réalisations associées à ces deux matériels ont nécessité des choix et des développements spécifiques. [NOYELLE89 p.225-227]

L'étude de 1989 décrit ainsi un ensemble historique plus large que celui des seuls manuels d'utilisation. Elle permet d'identifier des étapes de développement, des acteurs, des matériels, des projets successifs et des évolutions de la famille LSx. Elle constitue à ce titre une source historique pour situer les descriptions fonctionnelles des manuels dans leur contexte de développement. Elle demeure toutefois un récit rétrospectif publié plusieurs années après les premières réalisations, et ses indications sont conservées dans les limites de ce que l'auteur rapporte effectivement. [NOYELLE89 p.216] [NOYELLE89 p.223-232]

Les documents retenus ne décrivent donc pas tous le même objet sous le même angle. Le manuel de 1975 décrit les caractéristiques connues des systèmes L.S.E. exploités sur T1600 et Mitra 15 à la date de sa rédaction ; le manuel LSE 15 de 1978 présente le langage et son utilisation dans le cadre documentaire de l'École d'application des transmissions ; le récit de 1989 fournit un témoignage historique sur la succession des projets LSx et sur leur développement ; la Grammaire LSE est retenue pour les questions de syntaxe conformément au périmètre documentaire de la collection. Ces différences de nature documentaire doivent être conservées lors de la lecture croisée des sources et ne doivent pas être effacées par une formulation prétendant établir une unité plus précise que celle attestée par les documents. [INRDP75 §« Remarque sur la présentation du fascicule »] [LSE15 §1] [NOYELLE89 p.216] [CONV-ED1 §2] [CONV-ED1 §11]

Le présent tome adopte en conséquence une méthode de lecture fondée sur la traçabilité. Une information présente dans plusieurs sources est présentée avec les références correspondantes. Lorsqu'une source fournit une information qui n'apparaît pas dans les autres documents consultés, cette particularité est conservée. Lorsqu'une divergence est rencontrée, les formulations ou descriptions distinctes sont maintenues comme telles ; aucune réconciliation n'est opérée lorsque les sources ne permettent pas d'établir qu'elles décrivent exactement le même état du système ou la même réalisation. [CONV-ED1 §2] [CONV-ED1 §6] [CONV-ED1 §11]

Cette méthode impose également de distinguer le niveau de description fourni par les sources. Un manuel peut indiquer qu'une ligne est compilée, qu'une chaîne codée est exécutée par un interpréteur ou qu'un décompilateur permet de reconstituer une présentation textuelle. Une telle description documente le comportement ou l'organisation générale exposée par le document. Elle ne suffit pas, à elle seule, à déterminer un mécanisme interne qui n'est pas explicitement décrit. De même, l'existence d'une fonction, d'une instruction, d'un processeur ou d'une zone mémoire ne permet pas d'en déduire une organisation interne qui n'aurait pas été documentée. [INRDP75 §3.3] [NOYELLE89 p.218-220] [CONV-ED1 §11]

La distinction est particulièrement importante pour la notion d'architecture historique. Les sources emploient des termes tels que « compilateur », « décompilateur », « interpréteur », « analyseur de commandes », « bibliothécaire », « système résident », « zone données-utilisateur », « zone de swap » et « chaîne codée ». Ces termes peuvent être employés dans le présent tome lorsqu'ils sont attestés par les documents historiques. Leur présence ne signifie toutefois pas que l'organisation interne précise de chaque composant est connue au-delà de ce que les sources exposent explicitement. [INRDP75 §3.3] [INRDP75 §3] [CONV-ED1 §3] [CONV-ED1 §13]

La terminologie historique est conservée autant que possible. Le langage est désigné sous la forme « L.S.E. » lorsqu'il est cité comme nom officiel, conformément aux conventions éditoriales de la collection. La forme « LSE » est réservée aux identifiants, aux noms de fichiers et aux exemples de code. Les termes historiques français sont privilégiés lorsqu'ils existent, notamment « interpréteur », « chaîne codée », « chaîne post-fixée », « pile d'évaluation », « cadre d'activation », « processeur », « analyseur de commandes », « bibliothécaire », « zone données-utilisateur » et « zone de swap ». [CONV-ED1 §3] [CONV-ED1 §4] [CONV-ED1 §13]

Le Tome I ne cherche donc pas à produire une image unifiée du système par-delà les différences documentaires. Il établit seulement ce que les sources permettent d'établir. Lorsque les documents décrivent un mécanisme, le mécanisme est exposé dans les termes des documents. Lorsque les documents ne permettent pas d'en déterminer le fonctionnement interne, cette absence est explicitement conservée comme une limite documentaire. [CONV-ED1 §2] [CONV-ED1 §10] [CONV-ED1 §11]

## Limites des sources

Les documents historiques retenus ne permettent pas, à eux seuls, d'établir une description exhaustive et uniforme de toutes les versions, toutes les implantations et toutes les périodes de fonctionnement du L.S.E. Le manuel de 1975 précise lui-même qu'il ne prétend pas être exhaustif et renvoie, pour les utilisateurs, à la documentation fournie par les constructeurs. Il indique également qu'il décrit les caractéristiques connues à la date de rédaction des versions 4 du système L.S.E. sur T1600 et 8 du système L.S.E. sur Mitra 15, et prévoit la publication d'additifs en cas de modifications importantes ou de nouvelles implantations. [INRDP75 §« Remarque sur la présentation du fascicule »]

Le corpus ne permet pas non plus d'établir qu'une description donnée dans un manuel d'utilisation s'applique sans modification à toutes les implantations historiques du L.S.E. Le manuel de 1975 signale explicitement l'existence de différences entre T1600 et Mitra 15 et précise que ces différences sont signalées dans le fascicule lorsque cela est nécessaire. Le manuel LSE 15 de 1978 constitue pour sa part une documentation associée à une réalisation particulière et à son contexte d'utilisation. [INRDP75 §« Remarque sur la présentation du fascicule »] [LSE15 §1]

La documentation disponible dans le corpus ne permet pas, dans le cadre du présent tome, d'établir une bibliographie exhaustive de l'ensemble des documents historiques ayant contribué à la définition, à la réalisation ou à la diffusion du L.S.E. Le manuel de 1975 mentionne lui-même comme documents de base le « Manuel de référence » établi à l'E.S.E. et la première édition du manuel d'utilisation L.S.E. d'avril 1973, mais ces documents ne constituent pas, dans le présent corpus documentaire autorisé, des sources historiques directement exploitées au même titre que les quatre sources retenues. [INRDP75 §« Remarque sur la présentation du fascicule »] [CONV-ED1 §11]

La Grammaire LSE est retenue dans le corpus comme source de syntaxe. Les documents utilisés pour la présente rédaction ne permettent pas d'en établir ici, sans document bibliographique complémentaire, les informations éditoriales détaillées qui ne figurent pas dans le corpus effectivement consulté. Son emploi dans le Tome I est donc limité à son domaine documentaire défini par le cadre de la collection : la syntaxe ; elle ne peut être utilisée pour établir une architecture interne du système. [CONDITIONNEMENT-TOME-I §« Sources historiques autorisées »] [CONDITIONNEMENT-TOME-I §« Objet du Tome I »]

Les sources ne permettent enfin pas d'établir, dans tous les cas, si deux descriptions appartenant à des documents différents correspondent exactement à une même version logicielle, à une même implantation matérielle ou à une même période d'exploitation. Lorsqu'une telle correspondance n'est pas explicitement documentée, le présent tome ne la déduit pas. [INRDP75 §« Remarque sur la présentation du fascicule »] [LSE15 §1] [NOYELLE89 p.216-232] [CONV-ED1 §11]

# 1. Les sources historiques

## 1.1. Constitution du corpus documentaire

Le corpus historique du Tome I est constitué de quatre sources autorisées : le *Manuel d'utilisation L.S.E. INRDP* de 1975, le *Manuel d'utilisation LSE 15* de 1978, l'article de Yves Noyelle « La saga du LSE et de sa famille (LSD-LSG-LST) » publié en 1989, et la *Grammaire LSE*. Les trois premières sources sont des documents historiques identifiables par leur institution, leur date et leur contenu ; la quatrième est retenue comme source de syntaxe conformément au cadre documentaire du Tome I. [CONDITIONNEMENT-TOME-I §« Sources historiques autorisées »]

Ces quatre sources ne sont pas de même nature. Le manuel de 1975 est un manuel d'utilisation destiné à l'enseignement et à l'utilisation du L.S.E. ; le manuel LSE 15 de 1978 est une documentation d'utilisation associée à l'École d'application des transmissions et à la Direction de l'instruction ; l'article de 1989 est un récit historique rétrospectif consacré à la famille des langages LSx ; la Grammaire LSE relève de la description syntaxique. [INRDP75 §« Avant-propos »] [LSE15 §« Généralités »] [NOYELLE89 p.216] [CONDITIONNEMENT-TOME-I §« Sources historiques autorisées »]

La diversité de ces documents interdit de leur attribuer par défaut une même valeur descriptive pour toutes les questions relatives au système. Un manuel d'utilisation documente prioritairement ce qui est nécessaire à l'emploi du langage et du système tels qu'ils sont présentés à l'utilisateur. Un récit historique expose une succession de projets et d'événements selon le point de vue rétrospectif de son auteur. Une grammaire décrit la syntaxe dans le domaine qui lui est attribué. Le Tome I respecte ces différences de fonction documentaire. [INRDP75 §« Avant-propos »] [NOYELLE89 p.216-232] [CONV-ED1 §2] [CONV-ED1 §11]

Le principe de traçabilité impose que les affirmations historiques et techniques soient rattachées à la source qui les documente. Les références utilisées dans le Tome I reprennent les identifiants bibliographiques abrégés définis par les conventions éditoriales : [INRDP75] pour le manuel publié par l'I.N.R.D.P. en 1975, [LSE15] pour le manuel LSE 15 de 1978, [NOYELLE89] pour l'article de Yves Noyelle publié dans le *Bulletin de l'EPI* en 1989, et une référence propre à la Grammaire LSE pour les éléments qui relèvent de son domaine syntaxique. [CONV-ED1 §6]

Les identifiants documentaires permanents suivants sont employés dans la présente collection :

- **SRC-0001** — *Manuel d'utilisation L.S.E.*, Institut national de recherche et de documentation pédagogiques, janvier 1975. Référence abrégée : [INRDP75].
- **SRC-0002** — *Langage Symbolique d'Enseignement — LSE 15 — Manuel d'utilisation*, École d'application des transmissions, Direction de l'instruction, 1978. Référence abrégée : [LSE15].
- **SRC-0003** — Yves Noyelle, « La saga du LSE et de sa famille (LSD/LSG/LST) », *Bulletin de l'EPI*, no 54, 1989, p. 216-234. Référence abrégée : [NOYELLE89].
- **SRC-0004** — *Grammaire LSE*. Référence abrégée : [GRAMMAIRE-LSE].

Les trois premières identifications bibliographiques sont établies par les documents eux-mêmes ou par les éléments bibliographiques qu'ils portent. La quatrième identification est conservée sous la forme désignée par le corpus documentaire du projet ; aucune information bibliographique supplémentaire n'est ajoutée lorsqu'elle n'est pas établie par les documents disponibles. [INRDP75 §« Remarque sur la présentation du fascicule »] [LSE15 §« Généralités »] [NOYELLE89 p.216] [CONDITIONNEMENT-TOME-I §« Sources historiques autorisées »]

## 1.2. Le Manuel d'utilisation L.S.E. de l'I.N.R.D.P. — 1975

Le *Manuel d'utilisation L.S.E.* publié par l'Institut national de recherche et de documentation pédagogiques appartient à la documentation institutionnelle consacrée à l'usage du L.S.E. dans l'enseignement. Il est présenté comme un numéro spécial du bulletin de liaison « l'informatique et l'enseignement secondaire », publié en janvier 1975. [INRDP75 §Titre]

Son *Avant-propos* indique que le manuel constitue une deuxième édition et revient sur les objectifs de la première édition. Les auteurs déclarent avoir voulu réaliser à la fois un document pratiquement autodidactique, accessible à des personnes ne disposant que de notions très générales d'informatique, et un manuel d'utilisation permettant de retrouver facilement le libellé exact d'une instruction ou d'une fonction. Le public visé est décrit comme hétérogène et comprenant notamment des élèves et des enseignants de toutes disciplines des lycées. [INRDP75 §« Avant-propos »]

Le manuel précise que sa deuxième édition résulte notamment de la constatation de lacunes dans la première. Les auteurs indiquent que la partie relative aux fichiers a été revue, notamment parce que certaines instructions ou commandes n'étaient pas opérationnelles en mars 1973. Ils expliquent également avoir ajouté des éléments d'information sur le fonctionnement du système en temps partagé afin de mieux rendre compte de la manière dont certaines commandes mobilisent les programmes du système. [INRDP75 §« Avant-propos »]

Le document expose également une réorganisation de la présentation du langage. L'importance du caractère conversationnel du L.S.E. est explicitement mise en avant, de même que la possibilité d'effectuer des traitements aussi bien sur des chaînes de caractères que sur des données numériques. Le manuel précise que cette orientation a conduit à organiser une deuxième partie consacrée aux expressions L.S.E. et aux opérateurs et fonctions arithmétiques et chaînes, puis une troisième partie consacrée aux instructions, et une quatrième partie entièrement consacrée aux procédures. [INRDP75 §« Avant-propos »]

La *Remarque sur la présentation du fascicule* indique que le manuel a été rédigé par M. F. Chedeville, Mme F. Faure et M. C. Lafond. Elle précise également que les documents de base furent le « Manuel de référence » établi à l'E.S.E. en exécution du contrat no 718009 passé par la Délégation à l'informatique et la première édition du manuel d'utilisation L.S.E. publiée en avril 1973. Le fascicule indique en outre que des programmes transmis à la Section Informatique et Enseignement de l'I.N.R.D.P. ont été insérés dans le manuel. [INRDP75 §« Remarque sur la présentation du fascicule »]

La même présentation précise que le fascicule cherche à suivre une progression logique dans l'apprentissage du L.S.E. et que certaines notions jugées plus difficiles ou certaines instructions destinées à des utilisateurs possédant déjà une bonne pratique du langage sont placées dans les chapitres correspondants. Elle précise également une disposition pratique selon laquelle, lorsque cela est possible, la définition et le format de l'instruction figurent sur la page de gauche et un exemple d'utilisation sur la page de droite. [INRDP75 §« Remarque sur la présentation du fascicule »]

Le manuel fait état d'une mise au point de programmes sur ordinateurs T1600 de la Télémécanique et Mitra 15 de la CII. Il indique que les différences entre les deux matériels sont signalées lorsqu'elles existent et que des listages obtenus sur Mitra 15 ou T1600 sont publiés lorsque cela est nécessaire. [INRDP75 §« Remarque sur la présentation du fascicule »]

Le document précise que la rédaction du fascicule a été terminée en décembre 1974 et qu'il contient les explications concernant toutes les caractéristiques connues à cette date des deux systèmes L.S.E. existants mentionnés dans le manuel : la version 4 du système L.S.E. sur T1600 et la version 8 du système L.S.E. sur Mitra 15. Il précise en même temps que le fascicule ne prétend pas être exhaustif et que les utilisateurs sont invités à consulter la documentation fournie par les constructeurs. [INRDP75 §« Remarque sur la présentation du fascicule »]

Le manuel de 1975 présente ensuite le L.S.E. comme un langage évolué permettant de programmer des manipulations sur des chaînes de caractères ou des problèmes numériques et créé en vue d'une utilisation conversationnelle. Il introduit à cette occasion la notion de système L.S.E. comme l'ensemble des moyens matériels et logiciels permettant la gestion des tâches soumises à l'ordinateur. [INRDP75 §3]

Dans sa description du système, le manuel distingue les fonctions du moniteur de temps partagé, des processeurs et des différents états d'une console. Il décrit notamment le partage du temps entre les tâches, l'existence d'une zone données-utilisateur et d'une zone de swap, ainsi que l'utilisation de processeurs spécialisés pour certaines opérations. [INRDP75 §3.1] [INRDP75 §3.2] [INRDP75 §3.3]

La section consacrée aux processeurs fournit une description particulièrement importante pour l'étude historique de l'architecture documentaire du système. Elle distingue le compilateur L.S.E., le décompilateur, l'interpréteur, le bibliothécaire et l'analyseur de commandes. Le compilateur analyse la ligne L.S.E., détecte les erreurs de syntaxe et codifie la ligne ; le résultat de la compilation ligne à ligne est décrit sous la forme d'une chaîne codée exécutable par l'interpréteur. Le décompilateur est décrit comme transformant la chaîne codée en une chaîne de caractères. L'interpréteur est chargé de l'exécution et de la détection des erreurs d'exécution. Le bibliothécaire assure la gestion des fichiers utilisateurs et l'analyseur de commandes traite les commandes introduites par l'utilisateur. [INRDP75 §3.3]

Le manuel décrit également deux contextes d'exécution. En mode machine de bureau, le système appelle l'interpréteur après la compilation afin que les instructions de la ligne soient immédiatement exécutées. En mode programmé, l'utilisateur doit donner la commande EXECUTER afin que le système charge l'interpréteur et exécute le programme. [INRDP75 §3.3]

La structure de ligne occupe dans ce manuel une place centrale. La composition d'une ligne est décrite dans un ordre déterminé : un numéro, un espace sauf dans le cas d'un commentaire, une ou plusieurs instructions séparées par des points-virgules, puis un caractère signalant la fin de la ligne. Le manuel indique que l'ensemble comporte au plus 80 caractères, caractère de fin de ligne compris. [INRDP75 §« Présentation d'un programme L.S.E. »]

Lors de l'entrée d'un programme, chaque ligne est compilée par le système dès la frappe du caractère de fin de ligne. Le manuel précise que cette opération comprend l'analyse de la ligne, sa traduction en langage machine et la détection des erreurs de syntaxe. Une ligne syntaxiquement incorrecte n'est pas prise en compte ; une ligne syntaxiquement correcte est mémorisée et replacée selon l'ordre croissant des numéros de ligne. Une ligne nouvelle portant le même numéro remplace la précédente lorsque la nouvelle ligne est syntaxiquement correcte. [INRDP75 §« Présentation d'un programme L.S.E. »]

Le manuel distingue explicitement la vérification syntaxique de la ligne des erreurs qui ne peuvent être détectées qu'au moment de l'exécution du programme. Il fournit notamment l'exemple d'une déclaration de variable-chaîne omise : l'erreur correspondante n'est pas détectée lors de l'entrée du programme si la ligne considérée demeure syntaxiquement correcte. [INRDP75 §« Présentation d'un programme L.S.E. »]

Le Manuel d'utilisation L.S.E. de 1975 est donc une source majeure pour la description documentaire du système tel qu'il est présenté dans un environnement d'utilisation concret. Il fournit simultanément des éléments de langage, de structure de programme, de compilation ligne à ligne, d'exécution, de gestion des tâches, de temps partagé, de stockage et de répartition des fonctions entre différents processeurs. Ces éléments sont toutefois liés au périmètre temporel et matériel explicitement indiqué dans le fascicule. [INRDP75 §« Remarque sur la présentation du fascicule »] [INRDP75 §3] [INRDP75 §3.3]

## 1.3. Le Manuel d'utilisation LSE 15 de 1978

Le *Manuel d'utilisation LSE 15* est publié sous l'en-tête de l'École d'application des transmissions, Direction de l'instruction. Le document se présente comme une diffusion interne à des fins pédagogiques seulement. Son sommaire comprend des parties consacrées aux généralités, aux instructions arithmétiques, aux chaînes de caractères, au contrôle, aux entrées et sorties, aux procédures, aux commentaires et au langage de commande. [LSE15 §Titre] [LSE15 §Sommaire]

La section « Généralités » indique que le Langage Symbolique d'Enseignement permet de programmer des problèmes numériques ou portant sur des chaînes de caractères et qu'il a été étudié pour une utilisation conversationnelle. Le manuel relie cette utilisation conversationnelle à la facilité de modification des programmes et à une structure de lignes dans laquelle chaque ligne est indépendante syntaxiquement des lignes suivantes ou précédentes du programme. [LSE15 §1]

Le manuel définit la ligne de programme comme une construction comprenant un numéro, un espace dans les cas ordinaires, une ou plusieurs instructions séparées par des points-virgules et un caractère de fin de ligne. Il précise que chaque ligne est analysée par l'ordinateur dès que le caractère de fin de ligne est envoyé et qu'une erreur de syntaxe détectée à ce moment est immédiatement signalée. [LSE15 §1]

Le traitement des lignes est présenté comme une propriété essentielle de l'utilisation conversationnelle du langage. Lorsqu'une ligne est incorrecte, l'utilisateur doit la corriger et la refrapper en totalité, tandis que les lignes précédemment introduites n'ont pas à être réentrées. Lorsque la ligne est correcte, elle est mémorisée à son rang selon l'ordre croissant des numéros. [LSE15 §1]

Le manuel précise que les numéros de lignes ne sont pas nécessairement consécutifs et recommande de ménager des intervalles entre eux afin de permettre l'insertion ultérieure d'instructions, notamment d'affichage ou de pause, pendant la mise au point. Il indique également que deux lignes portant le même numéro ne coexistent pas dans le programme : la dernière ligne introduite prend la place de la précédente. [LSE15 §1]

Le document distingue de manière explicite l'indépendance syntaxique des lignes et les erreurs qui ne peuvent apparaître qu'à l'exécution. La vérification de la syntaxe porte sur chaque ligne indépendamment des autres lignes du programme ; certaines erreurs, telles que l'emploi incohérent d'un identificateur avec une déclaration attendue, ne sont donc signalées qu'au moment où le programme est exécuté. [LSE15 §1]

La structure du manuel de 1978 montre également que la documentation du LSE 15 ne se limite pas à la syntaxe du langage. Elle couvre les traitements arithmétiques, les chaînes de caractères, les instructions de contrôle, les échanges d'entrée et de sortie, les procédures et le langage de commande. Les procédures y font l'objet d'une partie spécifique comprenant notamment les paramètres, le corps d'une procédure, les instructions RETOUR et RESULTAT, les possibilités de sortie extraordinaire, les déclarations et appels de procédures et les variables locales. [LSE15 §Sommaire]

Le manuel de 1978 constitue ainsi une source importante pour la description d'un environnement documentaire où le langage est présenté comme un langage conversationnel organisé autour de lignes syntaxiquement indépendantes. Son organisation met en évidence le fait que la documentation historique associe étroitement le langage, les mécanismes d'exécution et le langage de commande, sans pour autant que chacune des fonctions décrites soit nécessairement présentée comme un composant interne distinct de l'implantation. [LSE15 §1] [LSE15 §Sommaire]

## 1.4. « La saga du LSE et de sa famille » — Yves Noyelle, 1989

L'article de Yves Noyelle, « La saga du LSE et de sa famille (LSD/LSG/LST) », est publié dans le *Bulletin de l'EPI*, no 54, en 1989, aux pages 216 à 234. Il s'agit d'un récit consacré à la famille de langages LSx, comprenant LSD, LSE, LSG et LST. L'auteur y présente le développement de ces langages comme un ensemble de projets conduits à l'École Supérieure d'Électricité entre 1968 et 1976. [NOYELLE89 p.216]

Le récit commence par l'environnement historique de l'École Supérieure d'Électricité à la fin de 1967. Il expose la nécessité de donner aux élèves de l'école un accès à l'informatique et rappelle l'existence d'une SEA CAB 500 dont l'usage était alors réservé à une partie des élèves. Il décrit ensuite l'acquisition d'un ordinateur IBM 1130 et la décision de mettre en place un système en temps partagé faisant intervenir un Hewlett-Packard 2116A. [NOYELLE89 p.216-217]

Dans ce contexte, l'auteur explique que les contraintes associées au temps partagé ont conduit à privilégier une compilation ligne à ligne et un système interprété. Il associe cette orientation à la nécessité de réduire le coût des modifications apportées aux programmes et de faciliter la détection des erreurs sémantiques et la protection du système. [NOYELLE89 p.217]

Le récit expose ensuite les objectifs assignés à LSD et sa réalisation sur le couple 1130-2116. Il décrit notamment la compilation ligne à ligne, les identificateurs multilettres, les procédures, fonctions et sous-programmes, les objets locaux et les modes de passage des paramètres. Il précise également qu'un seul type de données, le type réel, était prévu initialement, avant l'introduction rapide du type caractère, et décrit les tableaux comme le seul genre d'objets composés prévu dans cette première conception. [NOYELLE89 p.218-220]

Le texte de 1989 décrit aussi la représentation d'un programme sous la forme d'une notation polonaise inverse, désignée dans l'article comme une « chaîne post-fixée ». Il en expose plusieurs propriétés : facilité d'interprétation, compacité et possibilité de décompilation. Il indique que cette représentation était utilisée dans le contexte de ressources matérielles limitées et que le texte source n'avait pas à être conservé pour permettre un listage, puisqu'il pouvait être recréé à partir de la chaîne post-fixée. [NOYELLE89 p.219-220]

L'auteur décrit ensuite l'origine du LSE comme successeur du LSD dans le contexte de l'introduction expérimentale de l'informatique dans l'enseignement. Le colloque CERI/OCDE de Sèvres de mars 1970 est cité comme point de départ de cette étape. Après une démonstration, le LSD fut retenu comme langage support, sous réserve d'un changement de nom et d'une adaptation permettant son emploi dans un contexte d'enseignement plus large que celui des seules disciplines scientifiques. Le récit présente alors le LSE comme le successeur du LSD. [NOYELLE89 p.223-224]

Le récit précise que l'élargissement du domaine d'usage du langage a notamment conduit à l'introduction du type chaîne de caractères. Il expose les travaux consacrés aux opérations portant sur ce type de données entre juin et septembre 1971. Il mentionne à cette occasion plusieurs influences ou points de départ, parmi lesquels la connaissance de LISP, des fonctionnalités similaires de PL/1 et l'observation des opérations courantes effectuées par un programme manipulant du texte. [NOYELLE89 p.223-224]

Le rapport de définition du LSE est situé par l'auteur en octobre 1971. Le texte indique qu'il restait alors à réaliser le système de programmation et le système d'exploitation en temps partagé prévu pour au moins neuf terminaux, sur les deux machines retenues : les T1600 de Télémécanique et les MITRA-15 de SEMS. [NOYELLE89 p.225]

Le récit rapporte ensuite les conditions de réalisation des deux systèmes. Il décrit les contraintes de mémoire, de disque et de temps de réponse propres aux machines concernées, ainsi que les différences de réalisation entre les systèmes destinés aux deux constructeurs. Il mentionne notamment, pour le système associé au MITRA-15, des techniques de compactage et de recouvrement dynamique, la configuration en mémoire du sous-ensemble de l'interpréteur nécessaire à l'exécution du programme courant, ainsi que le recours à une technique d'échange. [NOYELLE89 p.225-227]

Pour la réalisation du LSE, l'auteur attribue des responsabilités distinctes à plusieurs participants et mentionne notamment l'interpréteur, le décompilateur, le compilateur, la définition du langage, l'analyseur de commandes, le système d'exploitation et le système de gestion de fichiers. Il décrit également une répartition des tâches entre les deux machines du couple 1130-2116 dans le développement antérieur du LSD. [NOYELLE89 p.222] [NOYELLE89 p.227]

Le récit fournit ainsi des éléments qui ne figurent pas sous cette forme dans les manuels d'utilisation. Il documente la filiation historique entre LSD et LSE, les conditions ayant conduit au changement de langage, la remise du rapport de définition de LSE, les matériels retenus pour l'implantation, les participants aux travaux et certains choix de réalisation. Il constitue donc une source historique complémentaire aux manuels, et non un simple manuel d'utilisation rétrospectif. [NOYELLE89 p.223-227]

La suite de l'article retrace également les prolongements de la famille LSE, notamment LSG et LST, puis la démarche de normalisation ultérieure. Elle distingue ainsi l'histoire du LSE initial des évolutions et extensions ultérieures de la famille LSx. Les caractéristiques ajoutées dans le cadre de la normalisation ne doivent donc pas être attribuées sans distinction à toutes les implantations historiques antérieures du L.S.E. [NOYELLE89 p.227-232]

## 1.5. La Grammaire LSE

La *Grammaire LSE* est retenue dans le corpus historique autorisé comme source de syntaxe. Son domaine documentaire est donc défini de manière spécifique : elle peut être utilisée pour établir les constructions syntaxiques du langage lorsque les éléments correspondants y sont effectivement documentés. [CONDITIONNEMENT-TOME-I §« Sources historiques autorisées »]

Cette fonction documentaire est distincte de celle qui est attribuée aux manuels d'utilisation et au récit historique de 1989. La grammaire peut établir la forme syntaxique d'une construction ; elle ne constitue pas, dans le cadre méthodologique du Tome I, une source permettant de décrire ou de reconstruire une architecture interne du système. [CONDITIONNEMENT-TOME-I §« Sources historiques autorisées »]

Une règle syntaxique, une forme d'instruction ou une organisation grammaticale documentée par la Grammaire LSE peut donc être rapportée comme telle, avec la référence correspondante. En revanche, le fait qu'une construction soit définie syntaxiquement ne permet pas d'en déduire l'organisation du compilateur, de l'interpréteur, de la chaîne codée, du stockage des objets ou des autres composants internes lorsque ces éléments ne sont pas explicitement décrits par les sources historiques. [CONDITIONNEMENT-TOME-I §« Sources historiques autorisées »] [CONDITIONNEMENT-TOME-I §« Objet du Tome I »] [CONV-ED1 §11]

Cette distinction est fondamentale pour l'édition documentaire. Le Tome I peut rapprocher une description syntaxique de descriptions fonctionnelles ou historiques fournies par les autres sources, mais il ne transforme pas ce rapprochement en connaissance d'un mécanisme interne non documenté. [CONV-ED1 §2] [CONV-ED1 §11]

## 1.6. Complémentarité des sources

Le Manuel d'utilisation L.S.E. de 1975 et le Manuel d'utilisation LSE 15 de 1978 présentent plusieurs éléments communs. Tous deux décrivent un langage conçu pour une utilisation conversationnelle et organisé autour d'une structure de lignes. Tous deux décrivent une prise en compte de la ligne lors de son entrée et une vérification syntaxique associée à cette unité. Tous deux décrivent également la modification d'une ligne par réintroduction de son numéro et l'exécution du programme selon l'ordre des numéros de ligne. [INRDP75 §« Présentation d'un programme L.S.E. »] [LSE15 §1]

Ces convergences documentent des propriétés communes de la présentation historique du L.S.E. dans les deux manuels. Elles ne permettent cependant pas, à elles seules, d'affirmer que toutes les versions du L.S.E. ont toujours possédé exactement les mêmes limites numériques, les mêmes caractères de contrôle, les mêmes dispositifs matériels ou les mêmes détails d'implantation. Le manuel de 1975 signale explicitement des différences entre T1600 et Mitra 15, tandis que le manuel de 1978 constitue une documentation portant sur LSE 15 dans son propre contexte. [INRDP75 §« Remarque sur la présentation du fascicule »] [LSE15 §1]

Le manuel de 1975 décrit explicitement la compilation ligne à ligne et distingue le compilateur, le décompilateur et l'interpréteur comme des processeurs du système. Le récit de 1989 décrit, pour sa part, une organisation historique dans laquelle la compilation, la décompilation, l'interprétation et d'autres fonctions du système sont réalisées par des composants distincts. Ces descriptions concordent sur l'existence historique de fonctions séparées de compilation, d'interprétation et de décompilation, mais elles ne fournissent pas nécessairement une description identique de leur organisation interne pour toutes les implantations. [INRDP75 §3.3] [NOYELLE89 p.222] [NOYELLE89 p.225-227]

La notion de chaîne post-fixée apparaît explicitement dans le récit de 1989 pour décrire la représentation issue de la structure du LSD. Le manuel de 1975, quant à lui, emploie la notion de chaîne codée pour décrire le résultat de la compilation ligne à ligne et précise que cette chaîne est exécutable par l'interpréteur. Ces deux formulations sont conservées comme des formulations historiquement attestées dans leurs sources respectives. Le corpus ne permet pas, à lui seul, d'établir que les deux expressions désignent nécessairement une représentation interne identique dans toutes les versions et implantations du L.S.E. [NOYELLE89 p.219-220] [INRDP75 §3.3]

Le manuel de 1975 décrit le décompilateur comme un composant permettant de transformer la chaîne codée en une chaîne de caractères et de permettre notamment le listage du programme. Le récit de 1989 rapporte de son côté, à propos du LSD et de son évolution historique, qu'une représentation sous forme de chaîne post-fixée pouvait être décompilée afin de recréer le texte nécessaire au listage. Ces descriptions documentent, à des niveaux différents, une relation historique entre une représentation compilée et une opération de reconstruction textuelle. Elles ne permettent pas de conclure au fonctionnement précis des algorithmes ou des formats internes employés. [INRDP75 §3.3] [NOYELLE89 p.219-220]

Le manuel de 1975 fournit enfin une description explicite de l'environnement de temps partagé, avec un moniteur de temps partagé, des tâches, une zone données-utilisateur, une zone de swap et des processeurs spécialisés. Le récit de 1989 apporte des informations historiques sur les conditions matérielles qui ont conduit à certaines réalisations, notamment les contraintes de mémoire et de disque et le recours à des techniques de compactage, de recouvrement et d'échange. Ces deux catégories d'informations se complètent pour documenter l'environnement historique, tout en demeurant distinctes quant à leur niveau de détail et à leur objet. [INRDP75 §3.1] [INRDP75 §3.3] [NOYELLE89 p.225-227]

La complémentarité documentaire ne doit toutefois pas être confondue avec une homogénéité parfaite. Le manuel de 1975 documente un état des systèmes L.S.E. correspondant aux versions qu'il indique explicitement ; le manuel de 1978 documente LSE 15 dans un autre contexte ; le récit de 1989 embrasse une période plus longue et une famille de langages plus large. Une propriété attestée dans une seule de ces sources doit donc être attribuée à cette source tant qu'aucune autre source autorisée ne permet de l'étendre. [INRDP75 §« Remarque sur la présentation du fascicule »] [LSE15 §1] [NOYELLE89 p.216-232]

## 1.7. Règles d'exploitation documentaire du corpus

Toute affirmation historique ou technique retenue dans le Tome I doit pouvoir être rapportée à une source autorisée. Les références sont placées à la fin du paragraphe correspondant, selon le format défini par les conventions éditoriales. Lorsqu'un même mécanisme est décrit par plusieurs sources, les références correspondantes sont toutes indiquées. [CONV-ED1 §6]

Les identifiants permanents du corpus permettent de distinguer les documents eux-mêmes des références bibliographiques abrégées utilisées dans le corps du texte. Cette distinction assure la stabilité des références lors des évolutions ultérieures de la collection. [CONV-ED1 §9] [CONV-ED1 §18]

Le statut documentaire retenu dans le Tome I est limité à deux catégories : « Documenté » et « Non documenté ». Une information explicitement présente dans une source peut être retenue comme documentée. Lorsqu'aucune des sources autorisées ne permet d'établir une information, celle-ci est considérée comme non documentée et cette absence doit être signalée sans tentative de reconstitution. [CONV-ED1 §10] [CONV-ED1 §11]

Le Tome I n'introduit donc ni hypothèse, ni déduction, ni choix de conception. Une interprétation qui permettrait de rendre cohérentes plusieurs descriptions historiques ne peut être transformée en fait documentaire simplement parce qu'elle paraît techniquement plausible. Lorsque les sources ne permettent pas de trancher entre plusieurs possibilités, le texte conserve cette indétermination. [CONV-ED1 §2] [CONV-ED1 §10] [CONV-ED1 §11]

La distinction entre description externe et organisation interne est également maintenue. Les sources peuvent documenter les opérations visibles ou décrites par les documents — entrée d'une ligne, analyse syntaxique, compilation, exécution, listage, gestion de fichiers, traitement des commandes — sans fournir nécessairement les détails de l'organisation interne des programmes qui réalisent ces opérations. Dans ce cas, le Tome I décrit les fonctions documentées et ne complète pas la description par une architecture non attestée. [INRDP75 §3.3] [NOYELLE89 p.222-227] [CONV-ED1 §11]

De même, la présence d'un terme historique dans plusieurs sources ne suffit pas à établir que ce terme possède exactement la même portée dans chacune d'elles. Les termes « compilateur », « interpréteur », « décompilateur », « processeur », « système », « zone de swap » ou « chaîne codée » sont employés dans leur contexte documentaire respectif. Lorsqu'une source donne une définition explicite, cette définition est conservée ; lorsqu'elle ne donne pas de définition plus précise, le Tome I ne lui en attribue pas une. [INRDP75 §3.3] [NOYELLE89 p.218-220] [CONV-ED1 §3] [CONV-ED1 §13]

La méthode documentaire adoptée pour ce tome impose enfin de distinguer clairement l'histoire du L.S.E. de sa reconstruction ultérieure. Le corpus historique peut fournir des éléments qui seront utiles aux tomes consacrés à la reconstruction, mais ces éléments demeurent dans le Tome I à leur niveau documentaire. Ils ne sont pas transformés en décisions de conception, en modèles internes ou en spécifications. [CONV-ED1 §1] [CONV-ED1 §11] [CONV-ED1 §18]

## Limites des sources

Les sources autorisées ne permettent pas d'établir une correspondance complète et systématique entre toutes les versions historiques du L.S.E., toutes les versions de ses composants et toutes les implantations matérielles mentionnées dans les documents. Le manuel de 1975 distingue explicitement deux systèmes et signale l'existence de différences entre T1600 et Mitra 15 ; le manuel de 1978 porte sur LSE 15 ; l'article de 1989 retrace plusieurs générations et plusieurs descendants du LSE. [INRDP75 §« Remarque sur la présentation du fascicule »] [LSE15 §Titre] [NOYELLE89 p.216-232]

Le corpus ne permet pas d'établir une chronologie exhaustive de toutes les versions du L.S.E. ni de toutes les modifications intervenues entre les états documentés par les sources retenues. Les dates et versions mentionnées sont conservées lorsqu'elles sont explicitement indiquées, mais aucune chronologie intermédiaire n'est reconstituée lorsqu'elle n'est pas attestée. [INRDP75 §« Remarque sur la présentation du fascicule »] [NOYELLE89 p.223-232]

Le corpus ne permet pas non plus de déterminer, pour chaque fonction décrite dans les manuels, la structure exacte du logiciel qui la réalisait, la répartition exacte des données et du code en mémoire, le format complet des représentations compilées ou la totalité des échanges entre les différents processeurs, dès lors que ces éléments ne sont pas explicitement documentés. Les descriptions de fonctions telles que compilateur, interpréteur, décompilateur, bibliothécaire ou analyseur de commandes ne sont donc pas étendues au-delà de leur contenu historique explicite. [INRDP75 §3.3] [CONV-ED1 §11]

Le corpus ne permet pas davantage d'établir qu'une propriété décrite pour LSD, pour une version particulière du LSE ou pour une réalisation particulière du système est nécessairement commune à toutes les versions historiques ultérieures. Les éléments rapportés par Yves Noyelle concernant le LSD, le LSE, LSG, LST et la normalisation sont conservés dans le périmètre temporel et historique que leur attribue la source. [NOYELLE89 p.218-232]

La documentation disponible ne permet enfin pas d'utiliser la syntaxe comme preuve suffisante d'une architecture interne. La Grammaire LSE est une source autorisée pour la syntaxe, mais le cadre documentaire du Tome I exclut son utilisation pour reconstruire une architecture non attestée. Toute relation entre une construction syntaxique et un mécanisme interne demeure donc non documentée lorsqu'aucune source historique ne l'établit explicitement. [CONDITIONNEMENT-TOME-I §« Sources historiques autorisées »] [CONDITIONNEMENT-TOME-I §« Objet du Tome I »] [CONV-ED1 §11]

# Chapitre 2 — Philosophie générale

## 2.1. Finalité générale (HIS-2.1)

Les langages de la famille LSx, parmi lesquels figure le Langage Symbolique d'Enseignement (L.S.E.), sont présentés par Yves Noyelle comme ayant été développés dans le but de « mettre l'ordinateur au service du programmeur ». La philosophie générale exposée à propos de leur conception repose sur l'idée qu'un outil doit faciliter autant que possible l'activité de celui qui l'emploie et ne pas lui imposer de contraintes lorsque celles-ci peuvent être évitées. Noyelle rattache explicitement cette orientation à une réaction contre les complexités qu'il jugeait inutilement imposées par une partie des langages et des systèmes de l'époque. Il mentionne également l'influence du langage PAF de la CAB 500, qu'il présente comme ayant poursuivi antérieurement une orientation comparable. [NOYELLE89 p.216]

Dans le cas du L.S.E., cette orientation est associée à un objectif d'utilisation conversationnelle. Le manuel de l'INRDP de 1975 définit le L.S.E. comme un langage permettant de programmer aussi bien des manipulations sur des chaînes de caractères que des problèmes numériques et précise qu'il a été créé en vue d'une utilisation conversationnelle, l'utilisateur pouvant dialoguer avec le système au moyen du clavier d'une console de visualisation ou d'une télétype. Le même manuel indique que les programmes écrits en L.S.E. sont facilement modifiables. [INRDP75 §3]

Le manuel LSE 15 de 1978 reprend cette présentation en indiquant que le Langage Symbolique d'Enseignement permet de programmer facilement des problèmes numériques ou portant sur des chaînes de caractères et qu'il a été étudié en vue d'une utilisation conversationnelle. Le lien établi dans ce manuel entre le caractère conversationnel et la facilité de modification des programmes est explicite : c'est précisément la structure de lignes, chaque ligne étant indépendante syntaxiquement des lignes suivantes ou précédentes du programme, qui est présentée comme permettant cette modification. [LSE15 §1]

L'orientation générale est également visible dans la présentation pédagogique du manuel de 1975. Celui-ci assigne au manuel deux objectifs : constituer un document pratiquement autodidactique, compréhensible par des personnes n'ayant que des notions générales d'informatique, et permettre à un programmeur de retrouver facilement le libellé exact d'une instruction ou d'une fonction. Le public visé est décrit comme comprenant des élèves et des enseignants de toutes disciplines. Le manuel indique en outre que la progression retenue doit permettre d'écrire assez rapidement des programmes simples, tandis que les notions plus difficiles sont introduites dans les chapitres auxquels elles se rattachent. [INRDP75 Avant-propos ; Remarque sur la présentation du fascicule]

Cette finalité ne se limite donc pas, dans les sources, à la définition de constructions du langage. Elle concerne également les modalités concrètes de dialogue avec le système, la modification des programmes, la détection des erreurs et la mise au point. Les différents aspects sont toutefois décrits avec des degrés de précision différents selon les documents : Noyelle expose les intentions qui ont présidé à la conception des premiers langages LSx et du LSD, tandis que les manuels de 1975 et de 1978 décrivent les propriétés du L.S.E. et de sa mise en œuvre dans des environnements déterminés. Ces niveaux de description sont conservés séparément dans le présent ouvrage. [NOYELLE89 p.216-221 ; INRDP75 §3-4 ; LSE15 §1 et §9] 
### Limites des sources

Les documents historiques disponibles ne permettent pas d'établir une liste exhaustive et formalisée des principes ayant présidé à la définition du L.S.E. Ils exposent certains objectifs et certaines propriétés, mais ne fournissent pas de document unique énumérant l'ensemble des principes directeurs du système. Ils ne permettent pas non plus de déterminer précisément, pour chaque propriété du L.S.E., si celle-ci procède directement d'une orientation générale ou d'une contrainte particulière de mise en œuvre. [NOYELLE89 p.216-221 ; INRDP75 Avant-propos et §3-4 ; LSE15 §1 et §9]

## 2.2. Facilité d'emploi, naturalité et apprentissage (HIS-2.2)

La volonté de faciliter l'emploi de l'ordinateur est formulée par Noyelle comme un principe général. Dans son récit, le produit recherché devait être facile à employer et heurter le moins possible le « bon sens de l'utilisateur ». L'erreur humaine est explicitement prise en compte : le système devait résister aux fausses manipulations. Cette formulation appartient au récit historique de la conception des langages LSx et doit être comprise dans ce cadre. [NOYELLE89 p.217-218]

Un autre objectif rapporté par Noyelle consistait à éviter le recours à un cours de programmation. Le langage devait être suffisamment « naturel » pour permettre l'auto-apprentissage, sans pour autant conduire à de mauvaises habitudes. L'exigence d'une certaine facilité d'apprentissage est ainsi associée à une exigence pédagogique : la simplicité recherchée ne devait pas être obtenue au prix d'un enseignement considéré comme incorrect ou de pratiques que les auteurs estimaient mauvaises. [NOYELLE89 p.217-218]

Le manuel de 1975 présente une orientation comparable à travers son propre objectif pédagogique. Il indique que le fascicule doit pouvoir être lu et compris par des personnes ne possédant que des notions très générales d'informatique. Il précise également qu'il doit rester un manuel d'utilisation permettant de retrouver les formulations exactes du langage. Le caractère autodidactique est donc associé à une fonction de référence, les deux objectifs étant poursuivis conjointement. [INRDP75 Avant-propos]

La même orientation apparaît dans la décision de ne pas réserver le manuel à un public spécialisé. Le public visé comprend des élèves et des enseignants de toutes disciplines des lycées. Le texte précise que cette hétérogénéité rend difficile la rédaction d'un manuel qui soit à la fois pédagogique et utilisable comme documentation de référence. La progression retenue est explicitement présentée comme devant permettre l'écriture rapide de programmes simples, les notions plus difficiles étant repoussées aux chapitres correspondants. [INRDP75 Avant-propos ; Remarque sur la présentation du fascicule]

La recherche d'une utilisation directe se retrouve également dans le langage de commande décrit par Noyelle. Celui-ci est présenté comme intégré, à la fois concis et explicite : l'utilisateur n'a à saisir que les deux premières lettres d'une commande, puis le système complète le libellé avant de demander éventuellement les arguments. Un mode « abrégé » est prévu pour éviter cette complétion lorsque l'utilisateur averti ne souhaite pas la subir. Le manuel de l'INRDP décrit également cette possibilité de n'entrer que les deux premières lettres des commandes, le système complétant ensuite leur affichage. [NOYELLE89 p.220 ; INRDP75 §4] 
La préférence pour des mots-clés français est également mentionnée par Noyelle parmi les éléments destinés à faciliter la tâche du programmeur. La source ne présente pas cette caractéristique comme une simple propriété linguistique : elle l'inscrit explicitement dans l'ensemble des dispositions destinées à rendre le langage plus accessible. Les manuels de 1975 et de 1978 emploient eux aussi des mots-clés français tels que LIRE, AFFICHER, EXECUTER, PAUSE ou TERMINER. [NOYELLE89 p.220-221 ; INRDP75 §4 ; LSE15 §2 et suivants] 
### Limites des sources

Les documents ne permettent pas d'établir une définition générale et formelle de ce que les concepteurs entendaient exactement par un langage « naturel », ni de définir précisément ce qui constituait, selon eux, une « mauvaise habitude » de programmation. Ils indiquent ces objectifs, mais ne fournissent pas de critères exhaustifs permettant de les mesurer. Les sources ne permettent pas non plus d'attribuer chaque choix de syntaxe ou de commande à l'un de ces objectifs avec certitude. [NOYELLE89 p.217-220 ; INRDP75 Avant-propos et §4 ; LSE15 §1-9]

## 2.3. La modification progressive des programmes (HIS-2.3)

La facilité de modification des programmes constitue un élément récurrent des descriptions historiques. Dans le récit de Noyelle, le choix d'une compilation ligne à ligne est directement relié à la situation d'utilisation en temps partagé et à la nécessité d'éviter de refaire la traduction de l'ensemble d'un programme après chaque modification. Cette considération est d'autant plus importante, selon le texte, que les modifications sont nombreuses chez un débutant. Noyelle indique que, dans LSD, n'importe quelle ligne, y compris une ligne de déclaration, pouvait être ajoutée, modifiée ou supprimée sans recompilation du reste du programme. [NOYELLE89 p.217-218] 
Les manuels de 1975 et de 1978 décrivent concrètement cette propriété au moyen de la structure de lignes. Le programme est constitué de lignes numérotées, introduites indépendamment les unes des autres. Une ligne correctement analysée est mémorisée immédiatement à la place correspondant à son numéro. Si une nouvelle ligne porte le même numéro qu'une ligne existante, la nouvelle ligne, si elle est syntaxiquement correcte, remplace l'ancienne. Le remplacement d'une seule ligne permet ainsi de modifier le programme sans réintroduire les autres lignes. [INRDP75 §3 ; LSE15 §1] 
La structure de lignes est également liée à l'indépendance syntaxique. Le manuel de 1975 précise que la vérification de la syntaxe porte sur une ligne considérée indépendamment des autres lignes du programme. Le manuel signale toutefois que cette propriété ne permet pas de détecter toutes les erreurs au moment de l'entrée : certaines erreurs ne peuvent être relevées qu'au moment de l'exécution du programme. Le manuel donne notamment l'exemple d'une variable chaîne dont la déclaration aurait été omise. [INRDP75 §3 ; Annexe II] 
La modification progressive concerne également l'organisation des numéros de ligne. Les manuels indiquent que les numéros n'ont pas à être consécutifs et recommandent de laisser des intervalles afin de permettre l'insertion ultérieure de nouvelles lignes, notamment pendant la mise au point. Le numéro de ligne est donc simultanément un élément d'identification de la ligne et un moyen de permettre l'évolution du programme par insertion. [INRDP75 §3 ; LSE15 §1]

Cette organisation est explicitement associée, dans les sources, au caractère conversationnel du langage. Le manuel de 1975 indique que le L.S.E. est conçu pour un usage conversationnel à la compilation comme à l'exécution et précise que les programmes écrits sont ainsi facilement modifiables. Le manuel de 1978 reprend la même articulation entre usage conversationnel, structure de lignes et indépendance syntaxique des lignes. [INRDP75 §3 ; LSE15 §1] 
### Limites des sources

Les documents établissent que la ligne constitue l'unité de modification du programme et que la compilation ligne à ligne évite la recompilation des autres lignes. Ils ne permettent pas d'établir de manière exhaustive quelles opérations internes étaient effectuées lors du remplacement d'une ligne ni quelles structures exactes permettaient de conserver les autres lignes déjà introduites. Ils ne décrivent pas non plus, dans les passages considérés, la totalité des conditions dans lesquelles la modification d'une ligne pouvait avoir des conséquences sur l'exécution ultérieure du programme. [INRDP75 §3 ; LSE15 §1 ; NOYELLE89 p.218]

## 2.4. Détection des erreurs et recherche de fiabilité (HIS-2.4)

La détection des erreurs constitue un élément important de la philosophie historique exposée par Noyelle. Dans son récit de la conception de LSD, il estime que FORTRAN ne facilitait pas suffisamment la détection des erreurs à l'exécution et indique que les erreurs sur la machine utilisée pouvaient entraîner la « mort du système ». L'objectif était donc de disposer d'un système fiable et de réduire la nécessité de faire appel à des assistants pour la mise au point. Selon Noyelle, cette orientation conduisait à privilégier un système interprété dans lequel la détection des erreurs sémantiques était plus facile et où des protections pouvaient être mises en œuvre pour assurer la fiabilité. [NOYELLE89 p.217-218]

Les manuels de L.S.E. décrivent plusieurs niveaux de détection. Lors de l'entrée d'une ligne, le compilateur analyse la ligne et détecte les erreurs de syntaxe. Si une erreur syntaxique est rencontrée, la ligne est refusée et l'utilisateur doit la corriger puis la refrapper. Le manuel de 1975 précise que l'utilisateur reçoit alors un message d'erreur codé et doit consulter le code des erreurs fourni par le constructeur pour obtenir le libellé correspondant. [INRDP75 §3.3 ; LSE15 §1] 
La détection des erreurs ne s'arrête toutefois pas à l'analyse syntaxique. Le manuel de 1975 distingue les erreurs qui peuvent être détectées lors de l'entrée d'une ligne et celles qui ne peuvent être détectées qu'au moment de l'exécution. Dans son récapitulatif des opérations de mise au point, il mentionne notamment les déclarations omises, les variables non définies, certaines erreurs concernant les fonctions sur chaînes et des erreurs de gestion des variables dans les boucles. [INRDP75 Annexe II]

Cette séparation entre entrée du programme et exécution est également reflétée dans la description des processeurs. Le compilateur détecte les erreurs de syntaxe et produit la forme codée de la ligne ; l'interpréteur exécute cette forme et détecte les erreurs d'exécution. Le fonctionnement décrit par le manuel associe donc la compilation à la première phase de contrôle et l'interprétation à une phase ultérieure de contrôle au moment de l'exécution. [INRDP75 §3.3]

La recherche de fiabilité ne concerne pas seulement les erreurs du programmeur. Noyelle indique qu'il était également recherché qu'un programme ne soit jamais perdu du fait du système, y compris en cas de « plantage ». Il décrit ainsi une volonté de protéger le travail de l'utilisateur et de réduire les conséquences des défaillances du système. Ce point relève du récit de l'implémentation historique du LSD et ne doit pas être généralisé sans précision à toutes les versions ultérieures du L.S.E. [NOYELLE89 p.221-222]

### Limites des sources

Les sources décrivent les catégories d'erreurs détectées à différents moments et exposent des objectifs de fiabilité, mais elles ne permettent pas d'établir une liste exhaustive des mécanismes de protection effectivement disponibles dans toutes les versions historiques du L.S.E. Elles ne permettent pas non plus de conclure que les garanties décrites pour le LSD de l'École Supérieure d'Électricité étaient identiques dans toutes les implantations ultérieures du L.S.E. [NOYELLE89 p.217-222 ; INRDP75 §3.3 et Annexe II ; LSE15 §1]

## 2.5. Temps de réponse, ressources disponibles et temps partagé (HIS-2.5)

Le choix de la compilation ligne à ligne est présenté par Noyelle dans le contexte historique du temps partagé et de machines dont la puissance restait limitée. La possibilité de compiler une ligne sans reprendre la traduction de l'ensemble du programme après une modification est ainsi directement associée à la volonté de conserver un système utilisable dans ces conditions. [NOYELLE89 p.217]

Noyelle rattache également la conception du système à l'objectif d'obtenir des temps de réponse corrects et des tailles de programme permettant de traiter des problèmes réels. Le souci de l'utilisateur est ainsi présenté comme incluant la perception de la rapidité du système. Dans la description du système d'exploitation du LSD, Noyelle mentionne notamment la priorité accordée aux travaux courts et la simultanéité entre les traitements et l'impression. Il indique que le temps de compilation pouvait être recouvert par le temps nécessaire à l'impression des caractères transmis au terminal. [NOYELLE89 p.218 et p.221-222]

Le manuel de 1975 décrit le temps partagé en termes de partage du temps consacré aux tâches, et non de division permanente de la mémoire entre les utilisateurs. Chaque tâche est alternativement implantée dans une zone de mémoire appelée « zone-données-utilisateur » puis exécutée. À un instant donné, une seule tâche est active ; lorsqu'elle cesse de l'être, son état est conservé dans une zone de swap afin de pouvoir être restauré lors de sa réactivation. [INRDP75 §3.1]

Le moniteur de temps partagé est présenté comme le programme résident chargé d'assurer cette gestion. Le manuel lui attribue notamment les opérations de chargement et d'activation des tâches, le choix de la tâche à activer, le chargement du processeur compétent pour l'opération demandée et le chargement de la tâche elle-même. Lorsque la tâche cesse d'être active, son état est recopié dans la zone de swap. [INRDP75 §3.1]

Le manuel précise également les circonstances dans lesquelles une tâche cesse d'être active. Cette situation peut intervenir lorsque le temps limite accordé par le système est atteint ou lorsqu'une demande d'entrée-sortie se produit, par exemple un affichage ou une lecture d'une ligne ou d'une donnée saisie par l'utilisateur. Le manuel signale en outre l'existence de priorités, notamment une priorité accordée à la tâche de compilation lorsqu'un utilisateur est en train d'entrer un programme. [INRDP75 §3.1]

La description de Noyelle et celle du manuel de 1975 se rejoignent donc sur l'association entre utilisation conversationnelle, temps partagé et recherche de temps de réponse acceptables, mais elles ne décrivent pas le même système dans le même contexte matériel. Noyelle décrit notamment le système LSD réalisé sur un ensemble comprenant un IBM 1130 et un Hewlett-Packard 2116A, tandis que le manuel de 1975 décrit des systèmes L.S.E. sur T1600 et Mitra 15. [NOYELLE89 p.217-218 ; INRDP75 Remarque sur la présentation du fascicule] 
### Limites des sources

Les sources ne permettent pas d'établir une mesure homogène des temps de réponse du L.S.E. applicable à toutes les implantations historiques. Elles ne permettent pas davantage de déterminer que les mécanismes de gestion du temps partagé décrits pour le LSD ou pour les systèmes T1600 et Mitra 15 étaient identiques dans toutes les versions du langage. Les informations relatives aux priorités, aux temps limites et à l'organisation mémoire doivent donc rester rattachées aux systèmes pour lesquels elles sont documentées. [NOYELLE89 p.217-222 ; INRDP75 §3.1]

---

# Chapitre 3 — L'environnement conversationnel

## 3.1. Définition de l'environnement conversationnel (HIS-3.1)

Le manuel de l'INRDP définit le L.S.E. comme un langage créé pour une utilisation conversationnelle. L'utilisateur dialogue avec le système au moyen du clavier d'une console de visualisation ou d'une télétype. Le caractère conversationnel est ainsi décrit comme une relation directe entre l'utilisateur et le système au cours de laquelle les entrées sont prises en compte au fur et à mesure de leur transmission. [INRDP75 §3]

Dans cette présentation, le langage possède une structure de ligne. Chaque ligne est prise individuellement en compte dès l'envoi du caractère de fin de ligne. Le contenu de cette ligne peut être une commande, une ou plusieurs instructions traitées en « mode machine de bureau », ou une ligne d'un programme L.S.E., identifiée dans ce dernier cas par la présence d'un numéro. [INRDP75 §3]

Le manuel LSE 15 reprend ce principe en définissant le caractère conversationnel par la facilité de modification des programmes et en reliant cette propriété à une structure de lignes dans laquelle chaque ligne est indépendante syntaxiquement des lignes précédentes ou suivantes du programme. La ligne constitue ainsi le point d'articulation entre l'entrée interactive, le programme en mémoire et le traitement ultérieur. [LSE15 §1]

Noyelle décrit pour le LSD une organisation analogue : dans le mode concerné, l'unité d'information est la ligne, et le système distingue une ligne de commande d'une ligne de programme grâce au numéro qui commence les lignes de programme. Il indique également que ce numéro sert à déterminer leur ordre d'exécution. Cette description est celle du LSD dans le récit historique de Noyelle ; elle est présentée ici en parallèle des descriptions du L.S.E. fournies par les manuels. [NOYELLE89 p.220-221]

### Limites des sources

Les sources établissent le principe d'un échange fondé sur l'entrée de lignes, mais elles ne permettent pas d'établir que l'ensemble des versions historiques du L.S.E. possédait exactement les mêmes modalités matérielles de dialogue. Elles décrivent notamment des consoles de visualisation et des télétypes, sans fournir une description exhaustive de tous les périphériques utilisés par les différentes implantations historiques. [INRDP75 §2-3 ; LSE15 §1 ; NOYELLE89 p.218-221]

## 3.2. La ligne comme unité de traitement (HIS-3.2)

Le manuel de 1975 définit une ligne de programme comme une séquence comprenant, dans l'ordre, un numéro, au moins un espace sauf dans le cas d'un commentaire, une ou plusieurs instructions séparées par des points-virgules et un caractère signalant la fin de la ligne. Pour les systèmes décrits par ce manuel, le numéro est compris entre 1 et 250 sur Mitra 15 et entre 1 et 255 sur T1600. L'ensemble de la ligne, caractère de fin de ligne compris, ne doit pas dépasser 80 caractères. [INRDP75 §3]

Le manuel LSE 15 de 1978 définit également une ligne de programme comme une ligne commençant par un numéro compris entre 1 et 250, suivie, sauf pour un commentaire, d'un espace au moins, puis d'une ou plusieurs instructions séparées par des points-virgules et terminée par le caractère de fin de ligne X-OFF. [LSE15 §1]

Le caractère de fin de ligne joue un rôle essentiel dans le fonctionnement conversationnel décrit par les manuels. C'est sa transmission qui déclenche l'analyse de la ligne. Le système ne traite donc pas le programme comme un texte continu dont la totalité devrait être saisie avant d'être analysée : chaque ligne peut être soumise séparément au système. [INRDP75 §3 ; LSE15 §1]

Les lignes sont ensuite rangées selon leur numéro. Le manuel de 1975 précise que, dès qu'une ligne syntaxiquement correcte est acceptée, elle est mémorisée et replacée immédiatement à son rang défini par l'ordre croissant des numéros. Le listage et l'exécution du programme suivent cet ordre, indépendamment de l'ordre chronologique dans lequel les lignes ont été introduites. [INRDP75 §3]

Le manuel LSE 15 décrit le même comportement. Il donne l'exemple d'un programme dont les lignes sont entrées dans un ordre différent de celui de leurs numéros, puis précise que l'exécution s'effectue dans l'ordre croissant des numéros. Il indique également que, dans une ligne donnée, les instructions sont exécutées de gauche à droite. [LSE15 §1]

Le numéro de ligne remplit ainsi plusieurs fonctions documentées : il distingue une ligne de programme d'une commande lorsqu'elles sont saisies à la console, il détermine la position de la ligne dans le programme et il permet de remplacer directement une ligne existante en saisissant une nouvelle ligne portant le même numéro. Le manuel LSE 15 précise que l'existence du numéro permet de distinguer les lignes de programme des commandes et que l'utilisateur peut insérer des commandes entre les lignes de son programme. [LSE15 §9-1 ; §1]

### Limites des sources

Les deux manuels ne décrivent pas exactement les mêmes limites numériques pour toutes les implantations : le manuel de 1975 distingue Mitra 15 et T1600, tandis que le manuel LSE 15 de 1978 indique la plage 1 à 250. Les sources ne permettent pas d'établir une règle unique applicable à toutes les versions historiques du L.S.E. Elles ne permettent pas non plus de décrire, au niveau interne, la représentation exacte d'une ligne après son entrée. [INRDP75 §3 ; LSE15 §1]

## 3.3. Entrée d'une ligne et compilation immédiate (HIS-3.3)

L'entrée d'un programme s'effectue ligne par ligne à la console de visualisation ou à la télétype. Selon le manuel de 1975, chaque ligne est « compilée » par le système dès que le caractère de fin de ligne est frappé. Le manuel explique cette opération en indiquant que la ligne est analysée, traduite en langage machine et que les erreurs de syntaxe sont signalées à l'utilisateur. [INRDP75 §3]

Le manuel LSE 15 décrit le même traitement. Chaque ligne est analysée dès que le caractère X-OFF qui en signale la fin est envoyé. Si une erreur de syntaxe est détectée, elle est signalée immédiatement. L'utilisateur doit alors corriger la ligne et la refrapper en totalité, tandis que les lignes précédemment introduites n'ont pas besoin d'être réintroduites. [LSE15 §1]

Dans la description des processeurs du système L.S.E., le compilateur est chargé d'analyser la ligne lue, de détecter les erreurs de syntaxe et de codifier la ligne. Le manuel indique qu'après cette compilation ligne à ligne, le programme se présente sous la forme d'une chaîne codée exécutable par l'interpréteur. Il précise aussi que l'utilisateur n'a pas à demander explicitement la compilation : celle-ci est effectuée automatiquement dès l'envoi du caractère de fin de ligne. [INRDP75 §3.3]

La notion de compilation ligne à ligne est également présente dans le récit historique de Noyelle consacré au LSD. Il indique qu'il était possible d'ajouter, de modifier ou de supprimer une ligne sans recompilation du reste du programme. Ce fonctionnement est présenté comme une conséquence directe des conditions d'utilisation en temps partagé et de la volonté d'éviter une nouvelle traduction complète à chaque modification. [NOYELLE89 p.217-218]

La compilation est ainsi intégrée au dialogue avec le système. Dans le cadre décrit par les manuels, l'envoi de la fin de ligne ne constitue pas seulement une opération de transmission : il déclenche le traitement de la ligne et fournit immédiatement à l'utilisateur le résultat de l'analyse syntaxique. Une ligne incorrecte est rejetée à ce moment, tandis qu'une ligne correcte est incorporée au programme courant. [INRDP75 §3 ; LSE15 §1]

### Limites des sources

Les sources établissent qu'une ligne correcte est analysée et codifiée dès sa fin de saisie et qu'une ligne incorrecte est rejetée, mais elles ne permettent pas de préciser l'ensemble des opérations internes effectuées entre la lecture de la ligne et sa mémorisation. Le manuel de 1975 emploie l'expression « traduite en langage machine », tandis que la description des processeurs parle d'une « chaîne codée » ; les sources ne permettent pas, dans le cadre du présent tome, d'établir une identité détaillée entre ces formulations au-delà de ce qui est explicitement décrit. [INRDP75 §3 et §3.3 ; NOYELLE89 p.218-220]

## 3.4. Indépendance syntaxique et remplacement des lignes (HIS-3.4)

La vérification syntaxique est explicitement limitée à la ligne en cours d'entrée. Le manuel de 1975 précise que cette vérification porte sur la ligne indépendamment des autres lignes du programme. Une ligne peut donc être acceptée même si le programme complet comporte des éléments qui ne pourront être validés qu'au moment de l'exécution. [INRDP75 §3]

Lorsqu'une ligne contient une erreur syntaxique, elle n'est pas prise en compte. Le système affiche un message d'erreur codé et l'utilisateur doit consulter le code des erreurs fourni par le constructeur, puis refrapper la ligne corrigée en totalité. Les autres lignes déjà introduites demeurent présentes et n'ont pas à être réintroduites. [INRDP75 §3 ; LSE15 §1] 
Lorsqu'une ligne syntaxiquement correcte porte un numéro déjà présent dans le programme, elle remplace la ligne précédente. Le manuel de 1975 présente explicitement cette règle comme un moyen de modifier facilement un programme. Le manuel LSE 15 de 1978 reprend la même règle. [INRDP75 §3 ; LSE15 §1]

Cette règle de remplacement s'inscrit dans le fonctionnement conversationnel global : l'utilisateur peut conserver un programme en mémoire, modifier certaines lignes, introduire de nouvelles lignes et laisser les numéros déterminer l'ordre final du programme. Le système réorganise alors les lignes selon leur ordre numérique, et non selon leur ordre d'entrée. [INRDP75 §3 ; LSE15 §1] 
### Limites des sources

Les sources indiquent que les lignes sont syntaxiquement indépendantes lors de leur entrée, mais elles ne permettent pas d'établir l'ensemble des dépendances susceptibles d'être vérifiées ultérieurement au moment de l'exécution. Elles ne permettent pas non plus de déterminer précisément quelles opérations sont effectuées lorsqu'une ligne est remplacée dans la représentation interne du programme. [INRDP75 §3 ; LSE15 §1]

## 3.5. Commandes et lignes de programme dans le même dialogue (HIS-3.5)

Le manuel LSE 15 distingue les commandes et les lignes de programme par la présence d'un numéro. Une ligne de programme est précédée d'un numéro, suivi d'un espace ou d'un astérisque selon le cas décrit ; une commande n'est pas précédée de ce numéro. L'utilisateur peut donc introduire des commandes entre les lignes de son programme. [LSE15 §9-1]

Le manuel de 1975 décrit également plusieurs catégories de commandes : commandes à action immédiate, commandes faisant appel au décompilateur, commandes faisant appel à l'interpréteur et commandes portant sur les fichiers. Les commandes sont ainsi intégrées au même environnement d'utilisation que le programme courant. [INRDP75 §4]

Les commandes à action immédiate comprennent notamment les commandes de connexion et de déconnexion, les commandes de mise au point et certaines commandes portant directement sur le programme courant. Les commandes de listage font appel au décompilateur, tandis que les commandes d'exécution provoquent le passage à l'état d'exécution et le chargement de l'interpréteur. Les commandes relatives aux fichiers font intervenir le bibliothécaire. [INRDP75 §3.3-4]

Le langage de commande est décrit par Noyelle comme intégré au système. Le caractère intégré ne signifie pas seulement que des commandes existent : le système lui-même reconnaît la ligne comme commande ou comme ligne de programme en fonction de sa forme. Les commandes peuvent en outre être abrégées et le système peut compléter leur libellé avant de demander leurs paramètres. [NOYELLE89 p.220-221 ; INRDP75 §4] 
### Limites des sources

Les documents décrivent les règles permettant de distinguer une commande d'une ligne de programme et énumèrent les principales catégories de commandes, mais ils ne permettent pas d'établir une description unifiée et exhaustive de l'analyse effectuée par le système pour toutes les formes de lignes saisissables. Ils ne permettent pas non plus de déterminer si les règles exactes de reconnaissance étaient identiques dans toutes les implantations historiques. [INRDP75 §3-4 ; LSE15 §9-1 ; NOYELLE89 p.220-221]

## 3.6. Les états de la console et le déroulement d'une session (HIS-3.6)

Le manuel de 1975 distingue trois états d'une console. Dans l'état « dormant », la console n'est pas en communication avec le système. Dans l'état « moniteur », l'utilisateur dialogue avec le système au moyen de commandes. Dans l'état « exécution », le système peut fonctionner soit en exécution immédiate, correspondant au « mode machine de bureau », soit en exécution différée, correspondant au « mode programmé », dans lequel l'utilisateur demande l'exécution d'un programme constitué de plusieurs lignes L.S.E. préalablement entrées en machine. [INRDP75 §3.2]

La commande BONJOUR fait passer la console de l'état dormant à l'état moniteur et annonce au système l'arrivée d'un nouvel utilisateur. La commande AU REVOIR annonce la fin de la session, permet au système d'effacer ce qui appartient à l'utilisateur selon les règles du système de fichiers et fait passer la console de l'état moniteur à l'état dormant. [INRDP75 §3.2 ; LSE15 §9-2] 
Le manuel LSE 15 présente lui aussi les états « dormant », « moniteur » et « exécution ». L'état moniteur correspond au dialogue de l'utilisateur avec le système d'exploitation, tandis que l'état exécution correspond au dialogue avec le programme en cours d'exécution. Le passage d'un état à l'autre est obtenu selon les commandes et les mécanismes décrits dans le manuel. [LSE15 §9-1]

Une différence de présentation apparaît dans le récit de Noyelle consacré au LSD. Celui-ci décrit, pour le système historique qu'il présente, deux modes principaux, « Exécution » et « Moniteur ». Cette formulation ne doit pas être artificiellement fusionnée avec la description en trois états donnée par les manuels de L.S.E. : les sources ne décrivent pas ici le même niveau de présentation ni nécessairement le même système dans la même implantation. [NOYELLE89 p.220-221]

### Limites des sources

Les documents historiques ne permettent pas d'établir une terminologie unique des états et modes applicable à toutes les versions du système. Le manuel de 1975 et le manuel LSE 15 décrivent trois états de la console, tandis que Noyelle présente le LSD selon deux modes principaux. Les sources ne permettent pas de conclure que ces différences sont uniquement terminologiques ou qu'elles correspondent à des architectures différentes. [INRDP75 §3.2 ; LSE15 §9-1 ; NOYELLE89 p.220-221]

## 3.7. Le mode machine de bureau (HIS-3.7)

Le mode machine de bureau est un élément particulier de l'environnement conversationnel. Le manuel de 1975 l'associe à l'exécution immédiate : dès l'envoi de la fin de ligne, la ligne est exécutée si elle a été acceptée par le système. Le mode permet ainsi d'effectuer directement des traitements sans constituer au préalable un programme composé de plusieurs lignes. [INRDP75 §3.2]

Le manuel LSE 15 précise que ce mode est utilisable lorsque la console se trouve dans l'état moniteur. Il permet d'effectuer des calculs simples à la console et d'obtenir la valeur des variables utilisées par le programme courant, sous réserve que celui-ci ait déjà été exécuté au moins une fois. Le manuel indique également qu'il est possible de modifier la valeur d'une variable ou d'en affecter une nouvelle. [LSE15 §9-5]

L'interrogation d'une expression s'effectue, dans ce mode, au moyen d'un point d'interrogation suivi de l'expression puis du caractère de fin de ligne. Le système évalue l'expression et affiche sa valeur. La même possibilité est utilisée pour interroger les variables et les éléments de tableaux du programme courant. [LSE15 §9-5]

Le mode machine de bureau est ainsi également utilisé comme outil de mise au point. Le manuel de 1975 indique qu'il peut être utilisé après une instruction PAUSE afin d'interroger l'état du programme avant de reprendre l'exécution. Le manuel LSE 15 présente de la même manière le mode comme un moyen d'observer ou de modifier des valeurs pendant la mise au point. [INRDP75 Annexe II ; LSE15 §9-5] 
Noyelle décrit dans le même esprit un outil de mise au point intégré au LSD : l'utilisateur pouvait interrompre un programme, savoir où il avait été arrêté, demander ou modifier la valeur d'un objet, puis faire continuer l'exécution sans perte d'information. Il présente cette possibilité comme un élément important de l'environnement, impliquant à la fois l'interpréteur et le système d'exploitation. [NOYELLE89 p.221]

### Limites des sources

Les sources décrivent précisément les opérations accessibles en mode machine de bureau, mais elles ne permettent pas d'établir une définition interne complète de ce mode ni de préciser comment les expressions saisies étaient transmises au processeur compétent. Elles ne permettent pas non plus de déterminer si toutes les possibilités de consultation et de modification décrites pour une implantation étaient disponibles sans différence dans toutes les versions du L.S.E. [INRDP75 §3.2 et Annexe II ; LSE15 §9-5 ; NOYELLE89 p.221]

## 3.8. Exécution immédiate et exécution différée (HIS-3.8)

Le manuel de 1975 distingue deux modalités d'exécution. En exécution immédiate, correspondant au mode machine de bureau, la ligne est exécutée dès que le système a reçu sa fin de ligne et l'a acceptée. En exécution différée, correspondant au mode programmé, l'utilisateur demande l'exécution d'un programme constitué de plusieurs lignes précédemment introduites. [INRDP75 §3.2]

Le manuel décrit les deux modalités au moyen des processeurs du système. En mode machine de bureau, l'interpréteur est appelé automatiquement par le système après la compilation de la ligne et les instructions de cette ligne sont exécutées immédiatement. En mode programmé, l'utilisateur doit donner la commande EXECUTER pour que le système charge l'interpréteur et exécute le programme. [INRDP75 §3.3]

La distinction entre les deux modalités est également visible dans la nature du travail effectué. Le mode machine de bureau est adapté aux calculs ou traitements immédiats, tandis que le programme à plusieurs lignes permet d'utiliser des branchements, des conditions, des boucles et des procédures, dont le manuel de 1975 présente l'introduction comme nécessaire dès lors que l'exécution strictement séquentielle ne suffit plus. [INRDP75 §3 et présentation des programmes]

Le système conversationnel ne sépare donc pas l'activité de programmation de l'activité de calcul immédiat. Une même console permet d'entrer des lignes de programme, de les compiler, d'exécuter directement une ligne, de demander l'exécution différée d'un programme, puis de revenir au mode moniteur pour effectuer des commandes ou utiliser le mode machine de bureau. [INRDP75 §3-4 ; LSE15 §9] 
### Limites des sources

Les sources établissent les deux modalités d'exécution et leurs principales commandes, mais elles ne permettent pas d'établir une description complète des transitions internes entre compilation, exécution immédiate, exécution différée et retour au moniteur. Elles ne permettent pas non plus de déterminer si les mécanismes de conservation de l'état d'exécution étaient identiques dans toutes les implantations. [INRDP75 §3.2-3.3 ; LSE15 §9]

## 3.9. Interruption, reprise et mise au point interactive (HIS-3.9)

La possibilité pour l'utilisateur de reprendre le contrôle de sa console constitue un élément essentiel de l'environnement conversationnel décrit par les sources. Le manuel de 1975 indique qu'une touche d'interruption peut être frappée à n'importe quel moment. Lorsque la console est en état d'exécution, elle repasse dans l'état moniteur et affiche le message PRET ainsi que le numéro de la ligne en cours d'exécution au moment de l'interruption. Si la console était déjà en état moniteur, elle y reste. [INRDP75 §3.2]

Le manuel LSE 15 formule cette possibilité de manière explicite : à tout moment, l'utilisateur doit pouvoir reprendre le contrôle de sa console et notamment la faire passer de l'état exécution à l'état moniteur, y compris lorsque le programme tourne dans une boucle sans fin ou lorsque la console est en cours d'affichage. La touche d'interruption provoque alors le passage à l'état moniteur et l'affichage du message PRET ; si l'exécution était en cours, le numéro de la ligne interrompue est également indiqué. [LSE15 §9-4]

La mise au point peut être conduite à l'aide de plusieurs mécanismes combinés. Le manuel de 1975 cite notamment la touche d'interruption, le mode machine de bureau, la commande PAS À PAS et la commande CONTINUER. Il indique également que des instructions AFFICHER peuvent être insérées pour observer les valeurs de variables et que des instructions PAUSE peuvent provoquer un arrêt permettant des interrogations avant la reprise. [INRDP75 Annexe II]

La commande PAS À PAS permet de faire arrêter l'exécution au début de chaque ligne. Le manuel précise que la console repasse alors dans l'état moniteur et affiche le numéro de la ligne atteinte. L'utilisateur peut effectuer des interrogations ou des commandes, puis revenir à l'exécution au moyen de CONTINUER. Le manuel LSE 15 décrit également ce fonctionnement et présente explicitement l'exécution pas à pas comme utile en cours de mise au point. [INRDP75 §4.1 ; LSE15 §9-3] 
Noyelle décrit pour le LSD une possibilité d'interruption analogue, mais avec une exigence plus forte de continuité : le programme pouvait être arrêté à tout moment, l'utilisateur pouvait examiner ou modifier les valeurs, puis faire continuer l'exécution sans perte d'information. Le texte mentionne même le cas d'une interruption pendant l'impression, sans perte d'un caractère. Cette caractéristique est présentée par Noyelle comme importante et comme nécessitant une conception adaptée de l'interpréteur et du système d'exploitation. [NOYELLE89 p.221]

La commande de reprise ne se limite pas nécessairement à la poursuite au point exact de l'arrêt. Le manuel LSE 15 décrit également la commande REPRENDRE, qui permet de reprendre l'exécution à partir d'une autre ligne tout en conservant l'affectation et les valeurs des identificateurs dans les conditions précisées par le manuel. La commande est distincte de CONTINUER et s'inscrit dans l'ensemble des possibilités offertes pour conduire la mise au point. [LSE15 §9-3]

### Limites des sources

Les sources permettent d'établir l'existence d'un mécanisme d'interruption et de plusieurs moyens de reprise ou de mise au point, mais elles ne permettent pas de définir une procédure générale unique applicable à toutes les versions du L.S.E. Les possibilités de reprise et les conditions dans lesquelles elles peuvent être utilisées diffèrent selon les commandes et les versions documentées. La conservation exacte de l'état d'exécution n'est décrite avec précision que pour certains cas. [INRDP75 §4.1 et Annexe II ; LSE15 §9-3-5 ; NOYELLE89 p.221]

## 3.10. Le temps partagé comme cadre d'exécution (HIS-3.10)

Le temps partagé constitue, dans le manuel de 1975, le cadre général dans lequel les tâches utilisateurs sont prises en charge. Le manuel précise que ce n'est pas la mémoire qui est partagée entre les utilisateurs, mais le temps que le système consacre aux différentes tâches. Chaque tâche est alternativement implantée dans la zone-données-utilisateur et exécutée. À un instant donné, une seule tâche est active. [INRDP75 §3.1]

Lorsque la tâche active est abandonnée, son état est conservé dans la zone de swap. Cette zone est présentée comme divisée en autant de parties qu'il existe de consoles connectées au système, chaque partie ayant une longueur correspondant à celle de la zone-données-utilisateur. Le manuel indique que les images de toutes les tâches utilisateurs sont ainsi présentes dans la zone de swap. [INRDP75 §3.1]

Le moniteur de temps partagé, programme résident en mémoire vive, prend en charge la gestion de ces changements d'activité. Le manuel lui attribue les opérations de chargement en mémoire et d'activation, le choix de la tâche à activer, le chargement du processeur compétent pour l'opération demandée et le chargement de la tâche elle-même. Lorsqu'une tâche cesse d'être active, son état est recopié dans la zone de swap. [INRDP75 §3.1]

Le mécanisme de changement de tâche est lié au fonctionnement conversationnel. Une tâche peut cesser d'être active lorsque le temps qui lui est attribué est écoulé ou lorsqu'une opération d'entrée-sortie doit intervenir, par exemple lorsqu'un affichage est demandé ou lorsqu'une ligne ou une donnée doit être saisie par l'utilisateur. Le manuel décrit donc explicitement le temps d'attente lié à l'interaction avec l'utilisateur comme un élément pris en compte dans la gestion des tâches. [INRDP75 §3.1]

Le manuel mentionne également des priorités. La compilation d'un programme en cours d'entrée bénéficie d'une priorité particulière, car la tâche de compilation comporte, selon le document, beaucoup de temps d'attente dû à la frappe des lignes. Il est donc prévu de pouvoir réactiver cette tâche dès qu'une ligne a été lue. [INRDP75 §3.1]

Dans le récit de Noyelle consacré au LSD, le temps partagé constitue également une donnée fondamentale du système. Le langage prévu initialement était FORTRAN, mais la réflexion sur le temps partagé a conduit à privilégier une compilation ligne à ligne puis un système interprété. La gestion de l'exécution, des terminaux, du temps partagé et des traitements entre les deux machines est décrite comme une partie intégrante du système historique. [NOYELLE89 p.217-222] 
### Limites des sources

La description détaillée du temps partagé fournie par le manuel de 1975 concerne les systèmes documentés dans ce manuel et ne peut être étendue sans preuve aux autres implantations. La description de Noyelle concerne le LSD réalisé sur le couple 1130-2116 et une organisation particulière des tâches. Les sources ne permettent pas d'établir un mécanisme de temps partagé unique et commun à l'ensemble des versions historiques du L.S.E. [INRDP75 §3.1 ; NOYELLE89 p.217-222]

## 3.11. Les processeurs du système (HIS-3.11)

Le manuel de 1975 présente les ressources communes des utilisateurs sous la forme de programmes appelés processeurs, chacun étant chargé d'une tâche déterminée. Cette présentation distingue le compilateur, le décompilateur, l'interpréteur, le bibliothécaire et l'analyseur de commande. [INRDP75 §3.3]

Le compilateur L.S.E. analyse la ligne L.S.E. lue, détecte les erreurs de syntaxe et codifie la ligne. La compilation ligne à ligne produit une chaîne codée qui peut ensuite être exécutée par l'interpréteur. La compilation est déclenchée automatiquement à la réception de la fin de ligne. [INRDP75 §3.3]

Le décompilateur effectue l'opération inverse de celle du compilateur selon la description du manuel : il transforme la chaîne codée en une chaîne de caractères. Il est notamment utilisé pour permettre le listage du programme courant au moyen de la commande LISTER. [INRDP75 §3.3]

L'interpréteur exécute le programme ou la ligne sous la forme fournie par le compilateur et détecte les erreurs d'exécution. En mode machine de bureau, il est appelé automatiquement après la compilation ; en mode programmé, la commande EXECUTER est nécessaire pour que le système charge l'interpréteur et exécute le programme. [INRDP75 §3.3]

Le bibliothécaire est chargé de la gestion des fichiers utilisateurs. Le manuel précise qu'il est appelé par les commandes ou les instructions portant sur les fichiers programmes ou les fichiers données. [INRDP75 §3.3]

L'analyseur de commande analyse les commandes saisies par les utilisateurs. Si elles sont correctement écrites, il les exécute lorsqu'il s'agit de commandes à action immédiate ou appelle le processeur compétent lorsqu'il s'agit de décompilation, d'exécution ou de gestion de fichiers. [INRDP75 §3.3]

La description des processeurs établit ainsi un enchaînement entre la saisie d'une ligne, son identification comme ligne de programme ou comme commande, puis l'appel du processeur correspondant. Elle montre également que l'environnement conversationnel ne repose pas sur un seul programme chargé d'accomplir toutes les opérations : le système historique est décrit comme un ensemble de processeurs spécialisés accessibles selon la nature de la demande. [INRDP75 §3.3-4]

Noyelle fournit une description plus détaillée de la répartition des fonctions dans l'implémentation historique du LSD. Il indique, à titre indicatif, que le 1130 assurait notamment l'interprétation, la gestion des fichiers et du swap, tandis que le 2116 assurait la compilation, la décompilation, l'analyse des commandes, la gestion des terminaux et du temps partagé. Cette répartition concerne le système historique décrit par Noyelle et ne doit pas être généralisée aux implantations L.S.E. décrites dans les manuels ultérieurs. [NOYELLE89 p.222]

### Limites des sources

Les sources identifient les principaux processeurs et leurs fonctions générales, mais elles ne permettent pas d'établir la totalité des processeurs ayant existé dans toutes les versions historiques. Elles ne permettent pas non plus de déterminer avec précision les frontières internes entre ces processeurs dans chaque implantation. La répartition entre les machines 1130 et 2116 décrite par Noyelle concerne le LSD et ne peut être transposée directement aux systèmes T1600 ou Mitra 15. [INRDP75 §3.3 ; NOYELLE89 p.222]

## 3.12. Le langage de commande intégré (HIS-3.12)

Le langage de commande fait partie intégrante de l'environnement conversationnel. Le manuel de 1975 distingue quatre catégories de commandes : les commandes à action immédiate, les commandes faisant appel au décompilateur, les commandes faisant appel à l'interpréteur et les commandes portant sur les fichiers. Cette classification correspond aux différentes opérations que l'utilisateur peut demander directement au système. [INRDP75 §4]

Les commandes sont entrées directement à la console. Le manuel de 1975 précise que, comme pour BONJOUR et AU REVOIR, l'utilisateur n'a généralement à taper que les deux premières lettres de la commande, suivies du caractère de fin de ligne ; le système complète alors l'affichage de la commande et demande éventuellement les paramètres. La commande ABREGER permet de supprimer cette complétion pour les utilisateurs qui ne souhaitent pas qu'elle soit affichée. [INRDP75 §4]

Le manuel LSE 15 présente la même logique dans le cadre de son langage de commande. À l'exception du mode machine de bureau, les commandes sont repérées par leurs deux premières lettres. Une fois ces lettres saisies avec le caractère de fin de ligne, le système complète la commande puis demande éventuellement les paramètres. [LSE15 §9-1]

Noyelle présente ce mécanisme parmi les dispositions destinées à faciliter la tâche du programmeur. Le langage de commande est décrit comme à la fois concis et explicite, avec une complétion automatique du nom de la commande. Le mode abrégé permet d'éviter cette complétion pour les utilisateurs avertis. [NOYELLE89 p.220]

La présence du langage de commande dans le même environnement que le programme courant contribue également à l'organisation interactive des opérations. Les commandes LISTER, EXECUTER, PAS À PAS, CONTINUER, REPRENDRE, APPELER ou RANGER, selon les systèmes et les versions documentés, permettent à l'utilisateur d'agir sur le programme courant, sur son exécution ou sur les fichiers, sans quitter l'environnement conversationnel. [INRDP75 §4 ; LSE15 §9-2-5] 
### Limites des sources

Les sources décrivent le fonctionnement général du langage de commande et certaines commandes importantes, mais elles ne permettent pas d'établir une description exhaustive de toutes les commandes de toutes les versions historiques. Certaines commandes ou certains comportements sont explicitement signalés comme spécifiques à une implantation, notamment dans le manuel de 1975. [INRDP75 §4 ; LSE15 §9]

## 3.13. Continuité du dialogue entre programmation, exécution et mise au point (HIS-3.13)

L'environnement conversationnel du L.S.E. ne se limite pas à l'introduction initiale d'un programme. Les sources décrivent une succession d'activités qui peuvent être conduites depuis la même console : entrée et compilation des lignes, exécution immédiate ou différée, interruption, interrogation de l'état du programme, modification de valeurs, reprise et nouvelle modification du programme. [INRDP75 §3-4 ; LSE15 §9] 
Cette continuité est particulièrement visible lors de la mise au point. Le manuel de 1975 propose de combiner les instructions AFFICHER, PAUSE, le mode machine de bureau et la commande PAS À PAS. L'utilisateur peut ainsi observer les valeurs produites pendant l'exécution, interrompre le programme, examiner certaines valeurs, puis poursuivre l'exécution. [INRDP75 Annexe II]

Le manuel LSE 15 expose la même continuité à travers la touche d'interruption, le mode machine de bureau, la commande PAS À PAS et les commandes de reprise. La console peut passer du moniteur à l'exécution puis revenir au moniteur sans que l'utilisateur ait à quitter la session. [LSE15 §9-3-5] 
Cette possibilité est présentée par Noyelle comme un élément essentiel de l'outil historique. Dans le LSD, l'utilisateur devait pouvoir arrêter un programme, examiner et modifier son état, puis poursuivre l'exécution. La continuité entre exécution et mise au point est donc explicitement considérée comme une caractéristique du système, et non comme une activité extérieure au fonctionnement normal de l'environnement. [NOYELLE89 p.221]

Le caractère conversationnel prend ainsi une dimension opérationnelle complète : le dialogue ne se situe pas uniquement avant l'exécution du programme, mais se poursuit pendant son exécution et pendant sa mise au point. Les sources historiques décrivent un environnement dans lequel l'utilisateur peut intervenir sur le programme et sur son état d'exécution à différents moments de la session. [INRDP75 §3-4 et Annexe II ; LSE15 §9 ; NOYELLE89 p.221]

### Limites des sources

Les documents établissent la continuité entre entrée, exécution et mise au point, mais ils ne permettent pas de déterminer une séquence unique et obligatoire d'utilisation. Les mécanismes de reprise, d'interruption et de modification sont décrits par commandes distinctes et peuvent différer selon les implantations et les versions. Les sources ne permettent pas d'en déduire un protocole général au-delà des comportements explicitement documentés. [INRDP75 §4 et Annexe II ; LSE15 §9 ; NOYELLE89 p.221]

## 3.14. Organisation matérielle et logicielle de l'environnement conversationnel (HIS-3.14)

Le manuel de 1975 présente le système L.S.E. comme l'ensemble des moyens matériels et logiciels permettant de gérer toutes les tâches soumises à l'ordinateur. Le schéma général présenté dans le manuel associe notamment une unité centrale, un disque magnétique, un lecteur de télétype, une ou plusieurs consoles de visualisation et des équipements de perforation. [INRDP75 §2-3]

Le fonctionnement de l'environnement conversationnel repose alors sur l'articulation entre la console utilisée par l'utilisateur et les différents processeurs du système. Une demande de l'utilisateur peut conduire à l'analyse d'une commande, à la compilation d'une ligne, à l'exécution par l'interpréteur, à la décompilation pour le listage ou à l'appel du bibliothécaire pour la gestion des fichiers. [INRDP75 §3.3-4]

La gestion du temps partagé ajoute à cette organisation une dimension de multiplexage des tâches utilisateurs. Le moniteur de temps partagé assure alternativement l'activation des tâches et leur conservation dans les zones de swap, pendant que les processeurs spécialisés sont chargés selon les opérations demandées. [INRDP75 §3.1 et §3.3]

Dans le système historique décrit par Noyelle pour LSD, l'environnement conversationnel repose lui aussi sur une organisation répartie entre plusieurs fonctions et plusieurs machines. La répartition indicative donnée pour le 1130 et le 2116 associe les fonctions d'interprétation, de gestion des fichiers et de swap à l'une des machines, et la compilation, la décompilation, l'analyse des commandes, la gestion des terminaux et du temps partagé à l'autre. [NOYELLE89 p.222]

Les sources présentent donc l'environnement conversationnel comme un ensemble comprenant à la fois le terminal de l'utilisateur, le langage et son organisation en lignes, le langage de commande, les processeurs spécialisés et les mécanismes de temps partagé. Ces éléments sont toutefois décrits à partir de systèmes et d'implantations historiques distincts ; leurs descriptions ne doivent pas être fusionnées en une architecture unique qui ne serait pas explicitement attestée. [INRDP75 §2-4 ; LSE15 §1 et §9 ; NOYELLE89 p.217-222] 
### Limites des sources

Les documents historiques décrivent plusieurs environnements matériels et logiciels, mais ils ne permettent pas d'établir une architecture générale et unique valable pour toutes les versions du L.S.E. Les schémas et répartitions fonctionnelles sont liés aux systèmes pour lesquels ils sont présentés. Les sources ne permettent pas de déterminer les mécanismes internes qui ne sont pas explicitement décrits par les manuels ou par le récit historique de Noyelle. [INRDP75 §2-3 ; LSE15 §1 et §9 ; NOYELLE89 p.217-222]

# Chapitre 4 — Le système en temps partagé

## 4.1. Principe général du temps partagé

**HIS-4.1 — Le système en temps partagé**

Le système L.S.E. est décrit, dans le manuel d'utilisation publié par l'Institut national de recherche et de documentation pédagogiques, comme un ensemble de moyens matériels et logiciels destinés à gérer les tâches soumises à l'ordinateur. Le système est conçu pour une utilisation conversationnelle : l'utilisateur dialogue avec le système au moyen du clavier d'une console de visualisation ou d'une télétype. Le langage lui-même possède une structure de ligne, chaque ligne étant prise individuellement en compte dès l'envoi du caractère de fin de ligne. Une ligne peut alors constituer une commande, une ou plusieurs instructions exécutées en mode machine de bureau, ou une ligne de programme L.S.E., reconnaissable à la présence d'un numéro de ligne. [INRDP75 §3]

Le principe du temps partagé est explicitement distingué d'un partage de la mémoire entre les utilisateurs. Le manuel indique que les travaux des différentes consoles ne sont pas exécutés en même temps par une division de la mémoire en autant de parties qu'il y aurait d'utilisateurs. C'est le temps consacré par le système aux différentes tâches qui est partagé. Chaque tâche est alternativement implantée dans une partie de la mémoire appelée « zone-données-utilisateur » et exécutée. À un instant donné, une seule tâche est active. [INRDP75 §3.1]

La tâche est définie de manière générale comme le travail fourni au système par l'utilisateur. Elle peut correspondre à un simple calcul exécuté en mode machine de bureau aussi bien qu'à l'exécution d'un programme L.S.E. [INRDP75 §3.1]

Cette organisation est également décrite, pour LSE 15, à travers les états successifs d'une console. Le manuel distingue un état « dormant », dans lequel la console n'est pas utilisée par une session active, un état « moniteur », dans lequel l'utilisateur dialogue avec le système, et un état « exécution », dans lequel l'utilisateur dialogue avec son programme en cours d'exécution. Le passage d'un état à l'autre est associé à des commandes ou à des mécanismes d'interruption et d'arrêt de l'exécution. [LSE15 §1 ; §9-1]

L'article de Yves Noyelle replace historiquement la réalisation du système en temps partagé dans le contexte du Service Informatique de l'École Supérieure d'Électricité. À la fin de l'année 1967, l'objectif était de permettre à un nombre beaucoup plus important d'élèves d'accéder à l'informatique et, en pratique, à un ordinateur. Un IBM 1130 avait été acquis dans cette perspective, mais son système d'exploitation nécessitait notamment du personnel qualifié et conduisait à des temps de service jugés trop longs. Il avait donc été décidé de réaliser un système en temps partagé ; un Hewlett-Packard 2116A avait été ajouté au 1130 pour cette réalisation. [NOYELLE89 pp.216-217]

Dans ce contexte historique, le projet initial devait utiliser FORTRAN. Yves Noyelle indique toutefois que la réflexion sur le temps partagé conduisit à reconsidérer le choix du langage. La faible puissance des ordinateurs disponibles rendait notamment souhaitable une compilation ligne à ligne, afin de ne pas avoir à reprendre toute la traduction d'un programme à chaque modification. Les préoccupations de fiabilité et de détection des erreurs conduisirent également vers un système interprété. [NOYELLE89 p.217]

Le même article associe donc directement la conception du langage aux contraintes du système en temps partagé. Le système devait notamment fournir des temps de réponse corrects et permettre le traitement de problèmes réels malgré les capacités limitées des machines disponibles. Cette contrainte est présentée conjointement avec les objectifs d'utilisation naturelle, d'auto-apprentissage et de résistance aux fausses manipulations. [NOYELLE89 pp.217-218]

La documentation historique ne présente toutefois pas une définition abstraite et générale du « temps partagé » indépendante d'une implantation. Les descriptions disponibles sont liées aux systèmes effectivement documentés, notamment aux configurations matérielles et logicielles auxquelles les manuels se rapportent. [INRDP75 §2 ; §3.1] [NOYELLE89 pp.216-218]

### Limites des sources

Les sources historiques consultées ne permettent pas d'établir une définition générale du temps partagé qui serait indépendante des systèmes L.S.E. effectivement décrits.

Elles ne permettent pas d'établir que toutes les implantations historiques du L.S.E. ont utilisé exactement le même mécanisme de partage du temps.

Elles ne permettent pas d'établir les caractéristiques détaillées de la politique d'ordonnancement au-delà des indications données pour les systèmes décrits.

Elles ne permettent pas d'établir la durée exacte des différentes phases de fonctionnement d'une tâche, ni une règle générale de calcul de ces durées.

Elles ne permettent pas d'établir la correspondance exacte entre la description générale donnée par le manuel de 1975 et toutes les versions ultérieures du système L.S.E.

---

## 4.2. La tâche et son activation

**HIS-4.2 — La tâche active**

Dans le système décrit par le manuel de 1975, une tâche est successivement rendue active puis abandonnée au profit d'une autre tâche. Lorsqu'une tâche est active, elle est implantée dans la zone-données-utilisateur et exécutée. Une seule tâche est active à un moment donné. [INRDP75 §3.1]

Lorsque le système abandonne une tâche, il doit conserver son état afin de pouvoir le restaurer lorsque cette tâche sera de nouveau activée. Le manuel associe cette nécessité à la présence, sur le disque, d'une zone de swap divisée en autant de parties qu'il existe de consoles connectées au système. Chaque partie possède une longueur égale à celle de la zone données-utilisateur correspondante. Les images des tâches utilisateurs sont présentes dans cette zone de swap. [INRDP75 §3.1]

Le cycle décrit par le manuel comporte ainsi une activation par chargement de la tâche en mémoire, une exécution de la tâche active, puis une recopie de son état dans la zone de swap lorsqu'elle cesse d'être active. Le document désigne respectivement ces opérations sous les termes « SWAP IN » et « SWAP OUT ». [INRDP75 §3.1]

La cessation de l'activité d'une tâche peut se produire lorsque le temps limite qui lui a été accordé par le système est atteint. Le manuel indique que ce temps limite constitue une fraction de seconde fixée par le système. Une tâche peut également cesser d'être active lorsqu'une demande d'entrée-sortie intervient auparavant, notamment à l'occasion d'un affichage ou d'une lecture d'une ligne ou d'une donnée introduite par l'utilisateur. [INRDP75 §3.1]

Le manuel explique cette interruption par le fait qu'il serait inutile qu'une tâche monopolise le système pendant une période correspondant au temps nécessaire à l'utilisateur pour effectuer une saisie. La description associe donc le partage du temps de calcul aux périodes d'attente liées aux échanges avec les consoles. [INRDP75 §3.1]

Le choix de la tâche à activer est généralement effectué de manière cyclique. Le manuel mentionne néanmoins l'existence de priorités pouvant conduire à réactiver une tâche avant son tour. Il donne comme exemple particulier la tâche associée à la compilation lorsqu'un utilisateur entre un programme. Cette tâche est prioritaire, en raison du temps d'attente important qui intervient pendant la frappe des lignes par l'utilisateur ; le système doit donc pouvoir forcer son activation dès qu'une ligne a été lue. [INRDP75 §3.1]

Le schéma de fonctionnement publié dans le manuel représente une tâche active passant par un cycle comprenant notamment les échanges entre mémoire et disque, l'attente d'une entrée-sortie, la fin d'une entrée-sortie et le retour parmi les tâches éligibles. La figure représente le passage par les états « active », « SWAP IN », « SWAP OUT » et « attente d'entrée/sortie ». [INRDP75 §3.1]

Le manuel de 1975 ne décrit cependant pas l'état interne d'une tâche sous la forme d'une structure détaillée. Il indique que son état est conservé dans la zone de swap, mais ne décompose pas cet état en une liste précise de valeurs ou de composants. [INRDP75 §3.1]

La description donnée par Yves Noyelle pour la réalisation historique du LSD et de sa famille apporte, sur un autre système, des indications complémentaires concernant le traitement des tâches et la recherche de temps de réponse courts. Pour une réalisation ultérieure, l'auteur mentionne notamment une gestion donnant la priorité aux travaux courts, ainsi que la simultanéité entre certains traitements et l'impression. Il précise toutefois que ces indications concernent le système décrit dans le cadre de la saga du LSE et ne constituent pas une description générale du moniteur de temps partagé de toutes les implantations. [NOYELLE89 p.222]

Dans cette même réalisation, la recherche d'un temps de réponse réduit est également associée à la compilation ligne à ligne. Yves Noyelle indique que le temps de compilation, comprenant l'analyse syntaxique et la génération de la chaîne post-fixée, était en général recouvert par le temps d'impression des caractères faisant passer le terminal à la ligne. [NOYELLE89 p.222]

### Limites des sources

Les sources historiques ne permettent pas d'établir la représentation interne exacte de l'état d'une tâche sauvegardé dans la zone de swap.

Elles ne permettent pas d'établir la structure précise des données utilisées par le moniteur pour identifier les tâches éligibles.

Elles ne permettent pas d'établir la totalité des règles de priorité utilisées par les différentes versions du système.

Elles ne permettent pas d'établir si les mécanismes de priorité décrits pour une implantation particulière ont été conservés sans modification dans les autres implantations.

Elles ne permettent pas d'établir un algorithme complet et formalisé de sélection de la tâche suivante.

---

## 4.3. Le moniteur de temps partagé

**HIS-4.3 — Le système résident et le moniteur de temps partagé**

La gestion du temps partagé est assurée par un programme spécial qui demeure en mémoire vive. Le manuel de 1975 le désigne comme le « moniteur temps partagé » et explique que sa présence permanente en mémoire justifie le qualificatif de « résident ». [INRDP75 §3.1]

Le moniteur de temps partagé effectue les opérations de chargement en mémoire et d'activation d'une tâche, désignées par le terme « SWAP IN ». Son fonctionnement comprend notamment le choix de la tâche à activer, le chargement du processeur compétent pour la tâche considérée et le chargement de la tâche elle-même. [INRDP75 §3.1]

Dans la terminologie du manuel, le « processeur » est un programme du système permettant d'effectuer une opération donnée, telle que la compilation ou l'exécution. Le moniteur charge ainsi le processeur correspondant à l'opération que doit effectuer la tâche, avant de charger la tâche. [INRDP75 §3.1]

Lorsqu'une tâche cesse d'être active, le moniteur de temps partagé recopie son état dans la zone de swap. Cette opération est désignée sous le terme « SWAP OUT ». [INRDP75 §3.1]

Le fonctionnement du moniteur repose donc, dans la description historique disponible, sur l'association de trois éléments : une zone mémoire dans laquelle une tâche active est exécutée, une zone de swap sur disque contenant les images des tâches qui ne sont pas actives, et un système résident chargé d'assurer les échanges et l'activation des tâches. [INRDP75 §3.1]

Le manuel présente le moniteur de temps partagé dans un cadre plus large comprenant également un « système résident ». Le schéma d'implantation mémoire-disque publié dans le manuel représente, dans la mémoire, le système résident et le moniteur de temps partagé, ainsi que le système processeur en cours d'exécution pour la tâche active. Le disque contient les images des tâches utilisateurs dans la zone de swap. [INRDP75 §3.1]

La documentation ne présente pas le moniteur comme un programme destiné directement à l'utilisateur. Les opérations réalisées par ce moniteur sont décrites comme des opérations internes de gestion du temps partagé, tandis que l'utilisateur interagit avec le système par l'intermédiaire de commandes et des mécanismes associés aux états de la console. [INRDP75 §3.1 ; §3.2] [LSE15 §9-1]

La séparation entre les fonctions de gestion du temps partagé et les autres fonctions du système est également visible dans l'article de Yves Noyelle. Pour le système réalisé sur le couple 1130-2116, l'auteur indique que la gestion du temps partagé faisait partie des fonctions prises en charge par le 2116, tandis que l'interprétation, la gestion des fichiers et la gestion du « swap » étaient prises en charge par le 1130. [NOYELLE89 p.222]

Cette répartition entre deux machines constitue une description d'une implantation historique particulière. Elle montre que, dans cette réalisation, le temps partagé et la gestion de l'exécution des programmes n'étaient pas nécessairement portés par une seule machine. Le texte de Noyelle précise également que le logiciel des deux machines avait dû être réécrit pour économiser de la place ou satisfaire certaines fonctionnalités du système. [NOYELLE89 p.222]

L'article mentionne, dans cette réalisation, la nécessité de pouvoir interrompre immédiatement un programme en cours d'exécution à tout moment, y compris pendant une impression. Cette contrainte avait conduit à la réalisation d'un pilote de terminal particulier, adapté aux caractéristiques du coupleur du 2116. [NOYELLE89 p.222]

Dans le système décrit par le manuel L.S.E. de 1975, la fonction d'interruption de l'exécution apparaît également comme une fonction accessible à l'utilisateur. La touche d'interruption permet de reprendre le contrôle de la console et, lorsque celle-ci est en état d'exécution, de la faire passer à l'état moniteur. Le système affiche alors le message « PRET » ainsi que, lorsque la console était en cours d'exécution, le numéro de ligne qui était en cours au moment de l'interruption. [INRDP75 §3.2]

Le manuel LSE 15 de 1978 reprend cette exigence de contrôle direct par l'utilisateur. Il indique qu'à tout moment l'utilisateur doit pouvoir reprendre le contrôle de sa console, notamment même lorsqu'un programme tourne dans une boucle sans fin ou que la console est en cours d'affichage. Une touche d'interruption permet alors de faire passer la console à l'état moniteur et d'afficher le message « PRET » ; lorsque la console était en état d'exécution, le numéro de la ligne en cours est également indiqué. [LSE15 §9-4]

Le moniteur de temps partagé est donc documenté comme une composante permanente du système, chargée de la gestion des tâches et de l'activation des processeurs appropriés. Les mécanismes permettant à l'utilisateur de reprendre le contrôle de sa console sont documentés parallèlement dans le langage de commande et dans les états de la console, mais les sources ne décrivent pas l'ensemble des échanges internes entre le moniteur et les autres programmes du système. [INRDP75 §3.1 ; §3.2 ; §3.3] [LSE15 §9-4]

### Limites des sources

Les sources ne permettent pas d'établir le contenu détaillé du programme appelé « moniteur temps partagé ».

Elles ne permettent pas d'établir son organisation interne, ses sous-programmes ou ses structures de données.

Elles ne permettent pas d'établir précisément la manière dont le moniteur communique avec les processeurs.

Elles ne permettent pas d'établir si le moniteur est identique d'une implantation matérielle à une autre.

Elles ne permettent pas d'établir si le moniteur décrit en 1975 correspond exactement, dans son organisation interne, au dispositif utilisé par LSE 15 en 1978.

---

## 4.4. Organisation de la mémoire et du disque

**HIS-4.4 — La mémoire et la zone données-utilisateur**

Le schéma de configuration présenté dans le manuel de 1975 associe une unité centrale, un disque magnétique, un lecteur de télétype, des consoles de visualisation et des télétypes. La documentation mentionne une configuration comportant huit consoles dans le schéma général présenté à cette occasion. [INRDP75 §2]

La mémoire utilisée par une tâche active est désignée comme la « zone-données-utilisateur ». La tâche qui est actuellement exécutée est implantée dans cette zone. Lorsque la tâche active est abandonnée, son état est recopié dans une partie de la zone de swap située sur le disque. [INRDP75 §3.1]

Le schéma d'implantation mémoire-disque du manuel montre en mémoire le système résident, le système processeur en cours d'exécution et la zone-données-utilisateur contenant la tâche active. Sur le disque sont représentées les images des tâches utilisateurs dans la zone de swap, ainsi que plusieurs catégories de fichiers. [INRDP75 §3.1]

Le manuel indique une taille de zone-données-utilisateur de 2 000 mots, soit 2 K mots, sur Mitra 15 et de 2 800 mots, soit 2,8 K mots, sur T 1600. Ces valeurs sont associées aux deux implantations matérielles auxquelles le manuel de 1975 se rapporte. [INRDP75 §3.1]

Dans cette même description, la zone-données-utilisateur contient notamment le programme courant de l'utilisateur lors de son chargement, ainsi que le programme courant et les données lors de l'exécution. La même zone est donc présentée comme l'espace dans lequel se trouvent les éléments nécessaires au fonctionnement de la tâche active. [INRDP75 §3.1]

Le manuel précise également que tous les utilisateurs ont recours aux mêmes ressources du système L.S.E. Ces ressources sont des programmes appelés « processeurs », chacun effectuant une tâche précise. La zone occupée par ces processeurs n'est pas décrite comme une zone utilisateur permanente indépendante de la tâche active ; le moniteur charge le processeur compétent pour la tâche considérée. [INRDP75 §3.3]

Le manuel donne par ailleurs une description plus générale de la mémoire en termes binaires. La plus petite unité d'information est le bit, et les informations manipulées par le système sont regroupées en octets et en mots. Le manuel précise que, sur les ordinateurs T 1600 et Mitra 15 visés par cette description, les mots sont de 16 bits, soit deux octets. [INRDP75 §3.1]

Selon le même document, un caractère est codé en binaire et occupe un octet. Un nombre est également rangé en mémoire sous forme binaire et occupe deux mots. Ces indications concernent la représentation des informations en mémoire dans les systèmes matériels considérés par le manuel. [INRDP75 §3.1]

**HIS-4.4.1 — La zone de swap**

La zone de swap est située sur le disque et est divisée en autant de parties qu'il y a de consoles connectées au système. Chaque partie est de longueur égale à la zone-données-utilisateur correspondante. Le manuel indique que les images de toutes les tâches utilisateurs sont présentes à chaque instant dans cette zone. [INRDP75 §3.1]

Le programme courant d'un utilisateur appartient à la tâche associée à sa console. Le manuel consacré aux fichiers précise que chaque console dispose d'une zone de swap contenant notamment son programme courant. Une autre console peut accéder à cette zone au moyen de la commande PRENDRE ETAT CONSOLE, qui recopie la zone de swap de la console appelée dans celle de la console appelante. L'état antérieur de cette dernière est alors perdu. [INRDP75 §5-1.2 ; §5-1.3]

Cette opération est décrite comme un transfert du programme courant d'une console à une autre. Le programme ainsi transféré conserve une durée de vie correspondant à la session de la console qui le reçoit, la zone de swap étant remise à zéro par la commande AU REVOIR. [INRDP75 §5-1.3]

La zone de swap joue ainsi plusieurs rôles documentés : elle constitue le lieu où sont conservées les images des tâches inactives, elle contient notamment le programme courant associé à une console, et elle peut être utilisée pour transférer un programme courant d'une console à une autre par l'intermédiaire d'une commande dédiée. [INRDP75 §3.1 ; §5-1.3]

**HIS-4.4.2 — La zone système**

La documentation sur les fichiers publiée en 1975 décrit une « zone système » sur le disque. Cette zone contient l'ensemble des programmes du système L.S.E. proprement dit, parmi lesquels sont explicitement cités le compilateur, l'interpréteur et l'analyseur de commandes. L'utilisateur n'a pas accès à cette zone. [INRDP75 §5-1.2]

La présence d'une zone système distincte de la zone de swap et des zones de stockage de fichiers utilisateurs est ainsi explicitement documentée. Les sources ne donnent toutefois pas, dans cette présentation, une description complète de tous les programmes contenus dans cette zone. [INRDP75 §5-1.2]

**HIS-4.4.3 — Les fichiers de données temporaires**

La documentation de 1975 décrit une zone particulière du disque destinée aux fichiers de données temporaires. Chaque console dispose, à l'ouverture de la session, d'une zone propre pour l'entreposage de ses fichiers temporaires. Une taille minimum est fixée pour toutes les consoles lors du lancement du système et constitue un minimum garanti. Un utilisateur ne peut pas empiéter sur la zone allouée à une autre console. Cette organisation est désignée comme l'« espace temporaire standard ». [INRDP75 §5-1.2]

Le manuel prévoit également la possibilité, sous certaines conditions, de demander une extension de l'espace temporaire pour les gros fichiers de données. Lorsqu'une telle extension est accordée, elle reste réservée à l'utilisateur qui l'a demandée jusqu'à sa libération. La zone temporaire standard, quant à elle, reste affectée à chaque console, qu'elle soit effectivement utilisée ou non. [INRDP75 §5-1.2]

La zone des fichiers temporaires peut également contenir un type particulier de fichier de données résultant de la commande DECODER. Cette commande transforme le programme courant en une chaîne de caractères stockée comme une donnée. Le programme ainsi transformé peut alors être traité par un autre programme, notamment pour renuméroter ses lignes. La commande ENCODER permet ensuite de transformer cette donnée en un nouveau programme courant. [INRDP75 §5-1.2]

Les fichiers temporaires sont liés à la durée de la session. Le manuel indique qu'ils sont détruits automatiquement à la fin de la session, c'est-à-dire lors de la commande AU REVOIR. Le programme courant est également détruit à ce moment-là, sauf s'il a auparavant été transformé en fichier-programme permanent. [INRDP75 §5-1.1]

Une conséquence documentaire explicitement signalée est que plusieurs utilisateurs peuvent utiliser le même nom pour des fichiers temporaires distincts. Comme chaque console n'accède qu'à ses propres fichiers temporaires, ces noms peuvent être identiques sans ambiguïté pour le système dans le cadre d'une utilisation séparée. [INRDP75 §5-1.3]

**HIS-4.4.4 — La zone des fichiers permanents**

La zone des fichiers permanents comprend la place restante sur le disque après déduction de la place réservée au système, des zones de swap et des espaces temporaires. Elle contient les deux catégories de fichiers permanents décrites par le manuel : les fichiers-programmes et les fichiers-données permanents. [INRDP75 §5-1.2]

Les fichiers-programmes sont conservés sur le disque afin de pouvoir être réutilisés. Les fichiers-données permanents sont également conservés au-delà de la session. Le manuel distingue ainsi, du point de vue de la durée de conservation, le programme courant et les fichiers-données temporaires, d'une part, et les fichiers-programmes et fichiers-données permanents, d'autre part. [INRDP75 §5-1.1]

Le système comprend donc, selon la description du manuel, plusieurs catégories d'espace disque ayant des fonctions différentes : la zone système, la zone de swap, la zone des fichiers de données temporaires et la zone des fichiers permanents. [INRDP75 §5-1.2]

### Limites des sources

Les sources ne permettent pas d'établir un plan mémoire complet indiquant les adresses ou les bornes précises de toutes les zones.

Elles ne permettent pas d'établir la disposition physique exacte des zones de la mémoire vive et du disque.

Elles ne permettent pas d'établir si les tailles de 2 000 et 2 800 mots correspondent à toutes les versions de Mitra 15 et T 1600 ou uniquement aux configurations documentées.

Elles ne permettent pas d'établir la structure interne exacte de la zone de swap ni le format des images de tâches.

Elles ne permettent pas d'établir la méthode exacte utilisée pour réserver et libérer les zones de fichiers temporaires.

Elles ne permettent pas d'établir les structures internes utilisées pour localiser les fichiers sur le disque.

Elles ne permettent pas d'établir le fonctionnement détaillé des protections appliquées aux différentes zones.

---

## 4.5. Les processeurs du système

**HIS-4.5 — Une organisation fondée sur des processeurs spécialisés**

Le manuel de 1975 indique que tous les utilisateurs ont recours aux mêmes ressources du système L.S.E. Ces ressources sont des programmes appelés « processeurs », chacun étant chargé d'effectuer une tâche précise. [INRDP75 §3.3]

Le manuel distingue plusieurs processeurs : le compilateur L.S.E., le décompilateur, l'interpréteur, le bibliothécaire et l'analyseur de commandes. Chacun intervient dans une partie particulière du traitement d'une ligne, d'un programme ou d'une commande. [INRDP75 §3.3]

Cette organisation est représentée dans un organigramme intitulé « ENCHAINEMENT DES PROCESSUS ». Le schéma associe notamment la lecture d'une ligne, l'identification d'une commande ou d'une demande de processeur, puis le recours au compilateur, à l'interpréteur, au bibliothécaire ou au décompilateur selon le cas. [INRDP75 §3.3]

**HIS-4.5.1 — Le compilateur**

Le compilateur L.S.E. analyse la ligne L.S.E. lue, détecte les erreurs de syntaxe et codifie la ligne. Après compilation ligne à ligne, le programme se présente sous la forme d'une chaîne codée qui peut être exécutée par l'interpréteur. [INRDP75 §3.3]

L'utilisateur n'a pas à donner de commande particulière pour provoquer cette compilation. Celle-ci est effectuée automatiquement dès l'envoi du caractère de fin de ligne. La ligne est donc traitée par le compilateur au moment où l'utilisateur termine son entrée. [INRDP75 §3.3]

Cette règle s'inscrit dans la structure de ligne du langage. Le manuel indique que chaque ligne est prise individuellement en compte par le système dès l'envoi du caractère de fin de ligne. Une ligne peut alors être une commande, une ou plusieurs instructions exécutées en mode machine de bureau, ou une ligne de programme L.S.E. [INRDP75 §3 ; §3.3]

La compilation ligne à ligne est également décrite par Yves Noyelle dans le cadre historique de la conception du LSD. Elle répondait, selon l'auteur, à la nécessité de ne pas reprendre la traduction de l'ensemble d'un programme lors de chaque modification, ces modifications étant particulièrement fréquentes chez un débutant. [NOYELLE89 p.217]

Dans cette perspective historique, la compilation ligne à ligne est également associée à une détection rapide des erreurs. L'article de Noyelle indique que la compilation ligne à ligne permettait une détection rapide et complète des erreurs, avec notamment la détection de l'emploi d'objets non initialisés. [NOYELLE89 p.222]

**HIS-4.5.2 — Le décompilateur**

Le décompilateur effectue l'opération inverse de celle du compilateur : il transforme la chaîne codée en une chaîne de caractères. Le manuel indique qu'il permet notamment de lister le programme au moyen de la commande LISTER. [INRDP75 §3.3]

L'intervention du décompilateur dans la commande LISTER montre que le programme courant n'est pas nécessairement conservé uniquement sous une représentation textuelle directement affichable. La commande de listage demande la transformation inverse de la représentation codée afin d'obtenir une forme de caractères pouvant être présentée à l'utilisateur. [INRDP75 §3.3 ; §4.2]

La documentation de 1975 précise également, dans la description des fichiers, qu'un programme courant peut être transformé par DECODER en une chaîne de caractères stockée sous la forme d'un fichier-données. Cette opération est distincte du fonctionnement du décompilateur utilisé par LISTER : DECODER produit explicitement une donnée constituée d'une chaîne de caractères, tandis que LISTER fait afficher le programme courant. [INRDP75 §5-1.2 ; §4.2]

Dans la réalisation historique décrite par Yves Noyelle, le décompilateur fait partie des composants du système et sa réalisation est attribuée à E. Fizzarotti pour l'une des versions du système, tandis qu'une autre organisation de l'équipe fait apparaître C. Lhermitte parmi les personnes ayant participé au décompilateur. Ces indications concernent les différentes réalisations historiques décrites dans l'article et ne permettent pas d'attribuer un même composant à toutes les versions du LSE. [NOYELLE89 pp.219, 222] 

**HIS-4.5.3 — L'interpréteur**

L'interpréteur exécute le programme ou la ligne sous la forme fournie par le compilateur. Il détecte les erreurs d'exécution. [INRDP75 §3.3]

En mode machine de bureau, l'interpréteur est appelé par le système dès que la compilation de la ligne est terminée. Les instructions de la ligne sont alors immédiatement exécutées. [INRDP75 §3.3]

En mode programmé, l'utilisateur doit donner la commande EXECUTER pour que le système charge l'interpréteur et exécute le programme courant. La distinction entre ces deux modes est donc décrite comme une différence dans le moment auquel l'interpréteur est appelé et dans la manière dont le programme est soumis à l'exécution. [INRDP75 §3.3 ; §4.3]

Le manuel distingue ainsi l'exécution immédiate, correspondant au mode machine de bureau, et l'exécution différée d'un programme constitué de plusieurs lignes L.S.E. préalablement entrées dans la machine. [INRDP75 §3.2]

Yves Noyelle rapporte, pour la réalisation du LSD sur le couple IBM 1130 et Hewlett-Packard 2116A, une répartition des tâches dans laquelle l'interprétation était assurée par le 1130. Pour cette même réalisation, la compilation, la décompilation et la gestion du temps partagé étaient attribuées au 2116. [NOYELLE89 p.222]

Dans une autre réalisation historique évoquée par Noyelle, l'interpréteur fait partie des composants développés spécifiquement pour le système, et l'auteur indique avoir participé à sa réalisation. L'article mentionne également des techniques de compactage et de recouvrement dynamique permettant de configurer en mémoire le sous-ensemble de l'interpréteur nécessaire au programme courant. [NOYELLE89 pp.219, 226]

**HIS-4.5.4 — Le bibliothécaire**

Le bibliothécaire assure la gestion des fichiers utilisateurs. Le manuel précise qu'il est appelé par les différentes commandes ou instructions portant sur les fichiers-programmes ou les fichiers-données. [INRDP75 §3.3]

Le bibliothécaire intervient donc notamment dans les opérations liées au stockage et à la récupération des programmes et des données. Les commandes et instructions correspondantes sont présentées dans la documentation relative aux fichiers. [INRDP75 §3.3 ; §5]

La gestion des fichiers est organisée autour de plusieurs catégories : fichiers-programmes, fichiers-données temporaires et fichiers-données permanents. Les fichiers-programmes sont conservés sur le disque ; les données temporaires sont liées à la session ; les fichiers-données permanents peuvent être conservés au-delà de la session. [INRDP75 §5-1.1 ; §5-1.2]

Dans l'article de Yves Noyelle consacré à l'histoire du LSE et de sa famille, la gestion des fichiers apparaît également parmi les fonctions essentielles du système. Dans la réalisation décrite pour le couple 1130-2116, la gestion des fichiers est attribuée au 1130. [NOYELLE89 p.222]

**HIS-4.5.5 — L'analyseur de commandes**

L'analyseur de commandes analyse les commandes saisies par les utilisateurs. Lorsqu'une commande est correctement écrite, il l'exécute directement lorsqu'il s'agit d'une commande à action immédiate, ou appelle le processeur compétent pour les opérations de décompilation, d'exécution ou de gestion des fichiers. [INRDP75 §3.3]

Le manuel décrit ainsi l'analyseur de commandes comme un élément situé à l'entrée du traitement des demandes de l'utilisateur. Il détermine notamment si la ligne saisie correspond à une commande et, dans ce cas, si cette commande doit être exécutée directement ou conduire au chargement d'un autre processeur. [INRDP75 §3.3]

L'article de Yves Noyelle attribue également la réalisation d'un analyseur de commandes à l'une des équipes de développement du système historique. Dans la répartition des fonctions entre les deux machines de la réalisation décrite à la page 222, l'analyse des commandes est prise en charge par le 2116. [NOYELLE89 p.222]

**HIS-4.5.6 — L'enchaînement des processeurs**

Le traitement d'une demande utilisateur ne repose donc pas sur un unique programme de traitement. La documentation de 1975 décrit un enchaînement dans lequel la ligne est d'abord lue, puis reconnue selon qu'elle correspond à une commande, à une demande de processeur ou à une utilisation du mode machine de bureau. Le processeur approprié est ensuite mobilisé : compilateur, interpréteur, décompilateur ou bibliothécaire selon le cas. [INRDP75 §3.3]

Le mode machine de bureau constitue dans cet enchaînement une utilisation particulière. Lorsqu'une expression est saisie précédée du symbole « ? », elle est compilée puis interprétée et son résultat est immédiatement affiché. [INRDP75 §3.3 ; §2]

Le traitement d'un programme en mode programmé est différent : les lignes sont d'abord entrées et compilées au fur et à mesure de leur saisie, puis l'exécution du programme est demandée explicitement par la commande EXECUTER. L'interpréteur est alors chargé par le système. [INRDP75 §3.3 ; §4.3]

La commande LISTER fait intervenir le décompilateur, tandis que les opérations portant sur les fichiers font intervenir le bibliothécaire. La commande APPELER permet, pour sa part, de faire du programme stocké sur le disque le programme courant de l'utilisateur. [INRDP75 §3.3 ; §4.2 ; §4.4]

La séparation entre ces processeurs est également visible dans le système historique décrit par Yves Noyelle. Pour la réalisation considérée, la compilation et la décompilation étaient réalisées sur le 2116, l'interprétation et la gestion des fichiers sur le 1130, et la gestion du temps partagé sur le 2116. [NOYELLE89 p.222]

### Limites des sources

Les sources ne permettent pas d'établir le découpage interne détaillé de chacun des processeurs.

Elles ne permettent pas d'établir les interfaces internes entre le compilateur, le décompilateur, l'interpréteur, le bibliothécaire et l'analyseur de commandes.

Elles ne permettent pas d'établir les structures de données internes utilisées pour passer d'un processeur à un autre.

Elles ne permettent pas d'établir la forme exacte des informations transmises au processeur suivant.

Elles ne permettent pas d'établir si l'architecture en processeurs décrite en 1975 est identique à celle de toutes les implantations du L.S.E.

Elles ne permettent pas d'établir les différences internes entre les processeurs présents sur T 1600, Mitra 15 et les autres matériels mentionnés dans l'histoire du LSE.

---

## 4.6. Les états d'une console

**HIS-4.6 — Les trois états de la console**

Le manuel de 1975 distingue trois états pour une console : l'état « dormant », l'état « moniteur » et l'état « exécution ». [INRDP75 §3.2]

Dans l'état dormant, la console n'est pas en communication avec le système. L'utilisateur n'a pas de session active à cette console. [INRDP75 §3.2]

Dans l'état moniteur, l'utilisateur dialogue avec le système au moyen de commandes. Cet état constitue le cadre des opérations de gestion de la session, du programme courant, des fichiers et des mécanismes de mise au point. [INRDP75 §3.2 ; §4]

Dans l'état exécution, le manuel distingue deux modes de fonctionnement : l'exécution immédiate, correspondant au mode machine de bureau, et l'exécution différée, correspondant à l'exécution d'un programme constitué de plusieurs lignes L.S.E. préalablement entrées. [INRDP75 §3.2]

Le manuel LSE 15 reprend cette distinction en termes très proches. L'état dormant est l'état dans lequel la console n'est pas utilisée dans le cadre d'une session. L'état moniteur est celui dans lequel l'utilisateur dialogue avec le système d'exploitation de l'ordinateur. L'état exécution est celui dans lequel l'utilisateur peut dialoguer avec son programme en cours d'exécution. [LSE15 §1 ; §9-1]

Le passage de l'état dormant à l'état moniteur et le passage inverse sont obtenus par les commandes BONJOUR et AU REVOIR. Le passage de l'état moniteur à l'état exécution intervient lors du lancement de l'exécution d'un programme. Le retour de l'état exécution vers l'état moniteur peut résulter de l'utilisation de la touche d'interruption, de l'exécution d'une instruction telle que PAUSE ou de certaines conditions prévues par une commande du moniteur. [LSE15 §9-1]

**HIS-4.6.1 — L'ouverture et la fermeture d'une session**

La commande BONJOUR est la première commande utilisée lorsqu'un utilisateur arrive à une console. Elle annonce au système qu'un nouvel utilisateur va utiliser la console et marque le début d'une session. La console passe alors de l'état dormant à l'état moniteur. [LSE15 §9-2]

Dans le système décrit par le manuel de 1975, la commande BONJOUR annonce également au système un nouvel utilisateur et fait passer la console de l'état dormant à l'état moniteur. [INRDP75 §3.2]

La commande AU REVOIR marque la fin de la session. Elle indique au système que la console peut être libérée et que les éléments appartenant à l'utilisateur peuvent être effacés, à l'exception éventuelle des fichiers permanents. La console passe alors de l'état moniteur à l'état dormant. [LSE15 §9-2]

Dans le manuel de 1975, la commande AU REVOIR fait également passer la console de l'état moniteur à l'état dormant et autorise le système à effacer ce qui appartient à l'utilisateur, dans le cadre de la gestion des fichiers. [INRDP75 §3.2]

La fin de session entraîne donc la disparition du programme courant et des données temporaires associés à la session. Les fichiers permanents, lorsqu'ils existent, ne sont pas détruits par cette opération. [INRDP75 §5-1.1]

**HIS-4.6.2 — L'interruption de l'exécution**

La touche d'interruption permet à l'utilisateur de reprendre le contrôle de sa console. Dans le système décrit en 1975, elle peut être frappée à n'importe quel moment. Si la console est en état d'exécution, elle passe à l'état moniteur et affiche le message PRET avec le numéro de la ligne qui était en cours d'exécution. Si elle est déjà dans l'état moniteur, elle y reste et affiche PRET. [INRDP75 §3.2]

La touche d'interruption peut être utilisée avec les commandes PAS A PAS, CONTINUER et avec le mode machine de bureau pour mettre au point un programme. [INRDP75 §3.2]

Le manuel LSE 15 renforce la formulation de cette possibilité en précisant que l'utilisateur doit pouvoir reprendre le contrôle de sa console à tout moment, y compris lorsqu'un programme boucle indéfiniment ou lorsque la console est en cours d'affichage. [LSE15 §9-4]

**HIS-4.6.3 — Le mode moniteur pendant une exécution**

L'état moniteur n'est pas limité aux opérations effectuées avant le lancement d'un programme. Il peut également être retrouvé pendant une exécution interrompue. Les commandes CONTINUER et REPRENDRE permettent alors de reprendre l'exécution du programme, selon les conditions précisées par le langage de commande. [INRDP75 §4.3] [LSE15 §9-2 ; §9-3]

La commande PAS À PAS permet de faire arrêter l'exécution au début de chaque ligne. La console repasse alors dans l'état moniteur et affiche le numéro de la ligne atteinte. L'utilisateur peut ensuite laisser poursuivre l'exécution ou exécuter une autre commande avant de demander sa continuation. [INRDP75 §4.1] [LSE15 §9-3]

La commande NORMAL annule l'effet de PAS À PAS et ramène l'exécution au mode normal. [LSE15 §9-3]

La commande REPRENDRE permet de reprendre l'exécution sur une ligne différente de celle où l'exécution avait été interrompue. Le manuel LSE 15 précise que cette commande ne modifie ni l'affectation des identificateurs, ni leur valeur ; l'exécution est toujours reprise au niveau principal, même si le programme avait été interrompu dans une procédure. [LSE15 §9-3]

Le manuel de 1975 décrit également la possibilité de reprendre l'exécution après une interruption, notamment au moyen de CONTINUER, REPRENDRE ou d'autres commandes de poursuite de l'exécution. [INRDP75 §4.3]

### Limites des sources

Les sources ne permettent pas d'établir une description interne des états de la console sous la forme d'un automate détaillé.

Elles ne permettent pas d'établir toutes les transitions possibles entre les états dans toutes les versions du système.

Elles ne permettent pas d'établir les mécanismes internes par lesquels la touche d'interruption provoque le passage de l'état exécution à l'état moniteur.

Elles ne permettent pas d'établir si les dispositifs matériels d'interruption étaient identiques pour les différentes consoles et implantations.

Elles ne permettent pas d'établir le traitement interne exact de l'état d'une tâche lors d'une interruption volontaire par l'utilisateur.

---

## 4.7. Le langage de commande et l'environnement du temps partagé

**HIS-4.7 — Le langage de commande**

Le LSE est destiné à être exploité dans un système en temps partagé avec lequel l'utilisateur doit pouvoir dialoguer. Le manuel LSE 15 indique qu'un langage de commande est donc nécessaire. Il précise également que ce langage dépend, dans une certaine mesure, du type de console utilisé pour communiquer avec le système. [LSE15 §9-1]

Pour le manuel LSE 15, les consoles envisagées sont des Télétypes ASR 33 ou des consoles de visualisation alphanumérique pouvant afficher un nombre suffisant de lignes d'une longueur suffisante. Le manuel donne comme exemple une capacité d'affichage de vingt lignes de 64 caractères ou, de préférence, de vingt lignes de 80 caractères. [LSE15 §9-1]

Le langage de commande comprend un mode « machine de bureau ». Celui-ci permet d'effectuer des calculs simples sans entrer un programme complet. Il permet également de demander la valeur de variables du programme courant et, dans certaines conditions, de modifier la valeur de ces variables. [LSE15 §9-1 ; §9-5]

Dans le système décrit par le manuel de 1975, les commandes sont classées en quatre catégories : commandes à action immédiate, commandes faisant appel au décompilateur, commandes faisant appel à l'interpréteur et commandes portant sur les fichiers. [INRDP75 §4]

Les commandes de listage font appel au décompilateur. La commande LISTER permet d'afficher le programme courant, éventuellement à partir d'une ligne donnée et jusqu'à une autre ligne donnée. L'utilisateur peut interrompre un listage au moyen de la touche d'interruption. [INRDP75 §4.2]

Les commandes d'exécution font appel à l'interpréteur. La commande EXECUTER fait passer la console de l'état moniteur à l'état exécution et lance le programme courant. Les commandes CONTINUER et REPRENDRE permettent de reprendre une exécution interrompue dans les conditions définies par le système. [INRDP75 §4.3]

Les commandes portant sur les fichiers font intervenir le bibliothécaire. Le manuel présente notamment APPELER et RANGER dans les commandes essentielles de ce domaine. APPELER permet de charger un programme déjà présent sur disque afin qu'il devienne le programme courant de l'utilisateur. [INRDP75 §3.3 ; §4.4]

L'utilisateur dispose ainsi d'un environnement dans lequel les commandes du système et les lignes du programme L.S.E. peuvent être saisies depuis la même console. Le manuel LSE 15 précise que la différence entre une ligne de programme et une commande repose sur la présence d'un numéro, précédant la ligne de programme, alors que les commandes ne sont pas précédées de ce numéro. [LSE15 §9-1]

**HIS-4.7.1 — Les commandes et les lignes de programme**

Le système L.S.E. traite les lignes saisies à la console selon leur nature. Une ligne de programme comporte un numéro et peut être enregistrée dans le programme courant. Une commande, à l'inverse, est interprétée par le langage de commande. [INRDP75 §3 ; LSE15 §9-1]

Le manuel LSE 15 indique que les commandes sont, à l'exception du mode machine de bureau, repérées par leurs deux premières lettres. Lorsque ces deux lettres sont suivies du caractère de fin de ligne, le système complète l'affichage de la commande et demande éventuellement les paramètres nécessaires. La commande ABREGER permet toutefois de supprimer cette complétion automatique. [LSE15 §9-1]

Le manuel de 1975 décrit également une saisie abrégée des commandes. L'utilisateur n'a normalement à taper que les deux premières lettres, suivies du caractère de fin de ligne ; le système complète ensuite le nom de la commande et attend éventuellement les paramètres nécessaires. [INRDP75 §4]

La coexistence, dans une même console, des commandes et des lignes de programme constitue un élément central de l'environnement conversationnel documenté. Le système reçoit des lignes individuelles et détermine, selon leur forme, s'il doit les traiter comme des commandes, des instructions immédiates ou des lignes numérotées du programme courant. [INRDP75 §3 ; §3.3]

### HIS-4.7.4 — Référentiel des commandes documentées dans INRDP75

Le manuel INRDP75 distingue les commandes à action immédiate, les commandes faisant appel au décompilateur, les commandes faisant appel à l'interpréteur et les commandes portant sur les fichiers. Le tableau ci-dessous recense les commandes documentées dans le manuel, sans prétendre imposer une organisation interne au système. [INRDP75 §4 ; « Enchaînement des processus »]

| Code | Commande | Domaine historique | Implantation |
|---|---|---|---|
| BO | BONJOUR | connexion | T1600 / MITRA 15 |
| AU | AU REVOIR | déconnexion | T1600 / MITRA 15 |
| PA | PAS A PAS | mise au point | T1600 / MITRA 15 |
| NO | NORMAL | mise au point | T1600 / MITRA 15 |
| PR | PRENDRE ETAT CONSOLE | état de console | T1600 / MITRA 15 |
| EF | EFFACER LIGNES | programme courant | T1600 / MITRA 15 |
| RU | RUBAN | lecture de ruban | MITRA 15 uniquement |
| AB | ABREGER | dialogue | T1600 / MITRA 15 |
| IN | IN EXTENSO | dialogue | T1600 / MITRA 15 |
| SI | SILENCE | lecture de ruban / dialogue | MITRA 15 uniquement |
| LI | LISTER A PARTIR DE | décompilation | T1600 / MITRA 15 |
| PE | PERFORER A PARTIR DE | décompilation | T1600 / MITRA 15 |
| NU | NUMERO A PARTIR DE | décompilation | T1600 / MITRA 15 |
| EX | EXECUTER A PARTIR DE | exécution | T1600 / MITRA 15 |
| CO | CONTINUER | reprise d'exécution | T1600 / MITRA 15 |
| RE | REPRENDRE A PARTIR DE | reprise d'exécution | T1600 / MITRA 15 |
| PO | POURSUIVRE JUSQU'EN | poursuite d'exécution | T1600 / MITRA 15 |
| AP | APPELER | fichier-programme | T1600 / MITRA 15 |
| RA | RANGER | fichier-programme | T1600 / MITRA 15 |
| MO | MODIFIER | fichier-programme | T1600 / MITRA 15 |
| SU | SUPPRIMER | fichiers | T1600 / MITRA 15 |
| CA | CATALOGUER | fichier-données | T1600 / MITRA 15 |
| UT | UTILISATION DISQUE | fichiers / stockage | T1600 / MITRA 15 |
| TA | TABLE DES FICHIERS | fichiers / stockage | T1600 / MITRA 15 |
| DE | DECODER | décompilation / fichier-données | T1600 / MITRA 15 |
| EN | ENCODER | fichier-données / compilation | T1600 / MITRA 15 |
| ID | IDENTIFICATION | fichiers / session | T1600 / MITRA 15 |

Les catégories indiquées ci-dessus décrivent le domaine fonctionnel de la commande. Le diagramme « Enchaînement des processus » constitue la référence historique pour le routage vers les processeurs ; lorsqu'une commande met en œuvre plusieurs fonctions successives, le tableau ne doit pas être interprété comme une assertion sur une interface interne précise. [INRDP75 §3.3 ; Annexe II]

Le manuel précise que l'utilisateur tape normalement les deux premières lettres de la commande suivies du caractère de fin de ligne ; le système complète alors l'affichage et attend éventuellement les paramètres. ABREGER permet de supprimer cette complétion. [INRDP75 §4]

**HIS-4.7.2 — Le mode machine de bureau**

Le mode machine de bureau est présenté comme une forme d'utilisation directe de la console. Le manuel de 1975 le décrit comme l'utilisation la plus simple d'une console dans l'état moniteur. [INRDP75 §2]

Ce mode permet notamment d'évaluer une expression L.S.E. et d'en afficher la valeur en la faisant précéder du symbole « ? ». Il permet également de demander la valeur de variables déjà définies et d'affecter une valeur à une variable. [INRDP75 §2]

L'exécution y est immédiate : dès l'envoi du caractère de fin de ligne, la ligne est compilée puis interprétée et exécutée. Le manuel précise que ce mode est différent du mode programmé et que les instructions faisant intervenir des numéros de ligne ne peuvent pas être utilisées dans ce contexte. [INRDP75 §2]

Le manuel LSE 15 reprend la fonction du mode machine de bureau en le réservant à l'état moniteur. Une expression précédée du symbole « ? » est évaluée et sa valeur affichée ; les valeurs des variables du programme courant peuvent également être consultées et modifiées. [LSE15 §9-5]

Le mode machine de bureau constitue ainsi un point de contact direct entre l'utilisateur et les mécanismes de calcul ou l'état du programme courant, sans passer par l'exécution complète d'un programme constitué de plusieurs lignes. [INRDP75 §2 ; LSE15 §9-5]

**HIS-4.7.3 — Les mécanismes de mise au point**

L'environnement de temps partagé comprend des mécanismes permettant de contrôler l'exécution d'un programme en cours de mise au point. La touche d'interruption, la commande PAS À PAS, les commandes CONTINUER et REPRENDRE et le mode machine de bureau peuvent être utilisés conjointement à cette fin. [INRDP75 §3.2 ; §4.1 ; §4.3]

La commande PAS À PAS permet de suspendre l'exécution au début de chaque ligne. La console revient alors à l'état moniteur et affiche le numéro de ligne atteint. L'utilisateur peut examiner ou modifier certaines informations au moyen du mode machine de bureau avant de reprendre l'exécution. [LSE15 §9-3 ; §9-5]

Le mécanisme de reprise permet également de poursuivre une exécution interrompue. La commande CONTINUER reprend l'exécution à l'endroit où elle a été arrêtée, tandis que REPRENDRE permet de reprendre sur une autre ligne sans modifier l'affectation ni la valeur des identificateurs dans les conditions précisées par le manuel LSE 15. [LSE15 §9-3]

Ces mécanismes sont documentés comme faisant partie intégrante de l'environnement conversationnel. Ils permettent à l'utilisateur de passer de l'exécution au dialogue avec le système, puis de revenir à l'exécution du programme. [INRDP75 §3.2 ; LSE15 §9-1 ; §9-4]

### Limites des sources

Les sources ne permettent pas d'établir une description complète du langage de commande de toutes les versions historiques du L.S.E.

Elles ne permettent pas d'établir les différences détaillées entre les langages de commande utilisés sur T 1600, Mitra 15 et les autres implantations.

Elles ne permettent pas d'établir la syntaxe complète de toutes les commandes historiques dans le cadre du présent chapitre.

Elles ne permettent pas d'établir les mécanismes internes utilisés pour reconnaître une commande ou une ligne de programme.

Elles ne permettent pas d'établir les mécanismes internes par lesquels le mode machine de bureau accède aux variables du programme courant.

---

## 4.8. Répartition historique des fonctions entre les machines

**HIS-4.8 — Une architecture répartie dans certaines réalisations**

L'article de Yves Noyelle décrit plusieurs réalisations historiques dans lesquelles les fonctions du système LSE sont réparties entre plusieurs machines. Cette organisation n'est pas présentée comme une propriété abstraite du langage, mais comme le résultat de réalisations matérielles particulières. [NOYELLE89 pp.218-222]

Pour la réalisation initiale du LSD, l'article décrit un ensemble composé d'un IBM 1130 et d'un Hewlett-Packard 2116A. Le 1130 disposait de 16 Ko de mémoire et d'un disque amovible de 1 Mo ; le 2116 disposait également de 16 Ko de mémoire et était relié à treize terminaux Télétypes ASR33. Les deux machines étaient reliées par leur accès direct mémoire. [NOYELLE89 p.218]

Dans une autre génération de systèmes décrite par l'auteur, les machines destinées aux établissements scolaires disposaient de capacités mémoire et disque différentes selon les constructeurs. Pour les systèmes livrés par Télémécanique, l'auteur indique des machines dotées de 16 Ko de mémoire centrale et d'un disque rapide de 256 Ko. Pour le système réalisé par la SEMS, il indique une machine dotée de 8 Ko de mémoire centrale et d'un disque rapide de 384 Ko. [NOYELLE89 p.226]

La contrainte de faible capacité mémoire est explicitement décrite pour la réalisation SEMS. L'objectif était de conserver suffisamment de place dans la zone utilisateur pour permettre l'exécution d'un module de fonctionnalités équivalentes à celles d'un programme d'environ 300 lignes de FORTRAN. [NOYELLE89 p.226]

Pour cette réalisation, plusieurs techniques sont mentionnées : compactage, recouvrement dynamique et organisation en mémoire d'un sous-ensemble de l'interpréteur correspondant au programme courant afin de limiter les accès au disque. Une partie seulement des tampons associés aux terminaux était résidente, le reste étant placé sur disque. [NOYELLE89 p.226]

La même description indique que le système avait été prévu pour seize terminaux et que l'organisation des tampons permettait de garantir un débit minimal de 600 bauds sur chacun d'eux. La moitié de la zone mémoire, soit 4 Ko, pouvait ainsi être attribuée à chaque utilisateur, avec recours à une technique d'échange. [NOYELLE89 p.226]

La recherche d'un temps de réponse court conduisit également à organiser les différentes branches des structures de recouvrement statiques, notamment celles du compilateur et du décompilateur, de manière à réduire le délai d'accès au disque et à limiter le nombre de chargements. L'auteur indique un temps de réponse généralement inférieur à une demi-seconde pour cette réalisation. [NOYELLE89 p.226]

Dans la réalisation décrite à la page 222, les fonctions étaient réparties entre les deux machines de la façon suivante : le 1130 assurait l'interprétation, la gestion des fichiers, la gestion du « swap » et la gestion de l'accès aux objets en mode « mise au point » ; le 2116 assurait la compilation, la décompilation, l'analyse des commandes, la gestion des terminaux, la gestion de l'arrêt du système et des coupures et retours secteurs, ainsi que la gestion du temps partagé. [NOYELLE89 p.222]

Cette répartition montre que, dans la réalisation décrite, les fonctions du système pouvaient être distribuées entre plusieurs unités de calcul. Elle ne permet cependant pas, à elle seule, d'établir que toutes les implantations du LSE ont adopté une organisation identique. [NOYELLE89 p.222]

L'article précise enfin que le logiciel des deux machines avait dû être réécrit, notamment pour économiser de la place ou pour satisfaire aux fonctionnalités attendues du système. La réalisation du logiciel du système est ainsi étroitement liée aux contraintes matérielles de l'implantation considérée. [NOYELLE89 p.222]

### Limites des sources

Les sources ne permettent pas d'établir une architecture matérielle unique du système L.S.E.

Elles ne permettent pas d'établir quelles fonctions étaient systématiquement réparties entre les machines dans toutes les implantations historiques.

Elles ne permettent pas d'établir les protocoles détaillés utilisés pour les échanges entre les machines.

Elles ne permettent pas d'établir la nature exacte des données échangées entre les processeurs répartis sur plusieurs machines.

Elles ne permettent pas d'établir si les répartitions de fonctions décrites pour les systèmes historiques ont été conservées dans les versions ultérieures.

---

## 4.9. Temps de réponse et contraintes de fonctionnement

**HIS-4.9 — Le temps de réponse comme caractéristique documentée du système**

La réalisation du système en temps partagé est historiquement associée à la recherche de temps de réponse suffisamment courts pour permettre une utilisation interactive. Yves Noyelle indique que le système devait être réellement utilisable, avec des temps de réponse corrects et des tailles de programme permettant de traiter des problèmes réels. [NOYELLE89 p.217]

Dans la réalisation initiale sur IBM 1130 et Hewlett-Packard 2116A, le système gérait treize terminaux. L'auteur rapporte que les utilisateurs s'étaient déclarés satisfaits des temps de réponse, qu'ils trouvaient normaux. Les difficultés signalées concernaient plutôt le bruit et la lenteur des télétypes ainsi que le temps limité pendant lequel le système était accessible. [NOYELLE89 p.222]

Le système ne fonctionnait alors qu'entre 12 heures et 14 heures, en raison de l'organisation des études à Sup-Élec et de l'utilisation du 1130 par les élèves de la section Informatique l'après-midi. [NOYELLE89 p.222]

La recherche de temps de réponse courts est également associée à l'ordonnancement des tâches. L'auteur indique que les travaux courts étaient placés en priorité selon un algorithme décrit comme « fort simple », et que certains traitements étaient réalisés simultanément avec l'impression. [NOYELLE89 p.222]

Dans cette organisation, le temps de compilation pouvait être masqué par le temps d'impression du terminal. L'analyse syntaxique et la génération de la chaîne post-fixée intervenaient pendant que le terminal imprimait les caractères nécessaires à son avancement, de sorte que le temps de compilation était généralement invisible pour l'utilisateur. [NOYELLE89 p.222]

La documentation de 1975 décrit également un mécanisme de priorité spécifique pour la compilation des lignes entrées par l'utilisateur. Lorsqu'un utilisateur saisit un programme, la tâche de compilation est prioritaire afin que la compilation puisse être effectuée rapidement dès qu'une ligne est disponible. [INRDP75 §3.1]

La recherche de temps de réponse ne se limite donc pas, dans les descriptions historiques, à la seule vitesse de calcul. Elle concerne également la gestion des attentes liées à la frappe, les échanges avec les terminaux, la compilation ligne à ligne, les accès au disque et la sélection des tâches. [INRDP75 §3.1 ; §3.3] [NOYELLE89 pp.222, 226]

Dans la réalisation décrite pour les machines à mémoire réduite, l'organisation des structures de recouvrement et des tampons de terminaux est directement associée à la recherche d'un temps de réponse court. L'auteur mentionne également un objectif de réduction du nombre de chargements des différentes branches du système. [NOYELLE89 p.226]

### Limites des sources

Les sources ne permettent pas d'établir une mesure générale du temps de réponse du système L.S.E.

Elles ne permettent pas d'établir un modèle quantitatif reliant le temps de réponse au nombre de consoles actives.

Elles ne permettent pas d'établir les conditions exactes dans lesquelles les travaux courts sont identifiés ou prioritaires.

Elles ne permettent pas d'établir la fréquence exacte des changements de tâche dans les différentes implantations.

Elles ne permettent pas d'établir une spécification complète des performances historiques du système.

---

## 4.10. Continuité entre le traitement conversationnel et l'exécution programmée

**HIS-4.10 — Une même console pour l'entrée, la compilation, l'exécution et la mise au point**

Le système L.S.E. décrit dans les sources historiques associe dans un même environnement conversationnel plusieurs activités : l'entrée des lignes du programme, leur compilation, l'utilisation de commandes du système, l'exécution du programme, l'interruption de cette exécution et sa reprise. [INRDP75 §3 ; §4] [LSE15 §9]

Lorsqu'une ligne de programme est saisie, elle est analysée et compilée dès la réception du caractère de fin de ligne. Les lignes précédemment introduites restent présentes lorsque la ligne nouvellement saisie doit être corrigée, ce qui permet de modifier un programme ligne par ligne. [LSE15 §1]

Le programme courant peut ensuite être exécuté dans l'ordre croissant des numéros de lignes. Le manuel LSE 15 indique que deux lignes portant le même numéro ne coexistent pas : la dernière ligne entrée remplace la précédente. Cette propriété permet de modifier facilement un programme. [LSE15 §1]

Le programme courant peut être affiché par la commande LISTER, exécuté par EXECUTER, interrompu par la touche d'interruption ou par une instruction telle que PAUSE, puis repris par CONTINUER ou REPRENDRE. [INRDP75 §4.2 ; §4.3] [LSE15 §9-3 ; §9-4]

Le mode machine de bureau permet, entre deux étapes d'exécution, d'effectuer directement des calculs, d'interroger des variables et, dans les conditions définies par le manuel, de modifier leur valeur. [INRDP75 §2] [LSE15 §9-5]

Le système permet également de charger un programme conservé sur disque afin qu'il devienne le programme courant de l'utilisateur. La commande APPELER joue ce rôle dans le langage de commande documenté. Le programme ainsi chargé peut ensuite être listé ou exécuté. [INRDP75 §4.4]

Cette continuité est l'un des traits documentés de l'environnement conversationnel : l'utilisateur n'est pas placé dans un environnement distinct pour écrire un programme, puis dans un autre pour le compiler et dans un autre encore pour le mettre au point. Les commandes, le programme courant et l'exécution sont accessibles depuis la même console, selon l'état dans lequel elle se trouve. [INRDP75 §3.2 ; §4] [LSE15 §9]

La documentation historique établit ainsi une relation étroite entre le temps partagé et la structure du langage. Le système traite les lignes au fur et à mesure de leur saisie, les utilisateurs disposent d'un programme courant propre à leur session, et le temps partagé permet de faire alterner le traitement des tâches issues des différentes consoles. [INRDP75 §3 ; §3.1]

### Limites des sources

Les sources ne permettent pas d'établir la totalité des mécanismes internes qui assurent la continuité entre la compilation, l'exécution et la mise au point.

Elles ne permettent pas d'établir la représentation interne exacte du programme courant pendant les changements d'état de la console.

Elles ne permettent pas d'établir comment les informations de contrôle sont conservées entre une interruption et une reprise d'exécution.

Elles ne permettent pas d'établir l'organisation exacte des données nécessaires au passage entre les différents processeurs.

---

## Limites des sources du chapitre 4

Les documents historiques consultés permettent d'établir que le système L.S.E. est conçu comme un environnement conversationnel fonctionnant en temps partagé, dans lequel les travaux des utilisateurs sont traités sous forme de tâches alternativement actives, avec conservation de leur état dans une zone de swap sur disque. Ils décrivent le moniteur de temps partagé, la zone-données-utilisateur, la zone de swap, les différents états d'une console, ainsi que plusieurs processeurs spécialisés du système : compilateur, décompilateur, interpréteur, bibliothécaire et analyseur de commandes. [INRDP75 §3.1 ; §3.2 ; §3.3]

Ils permettent également d'établir que certaines réalisations historiques ont réparti les fonctions du système entre plusieurs machines et que les contraintes de mémoire, de disque, de temps de réponse et de gestion des terminaux ont influencé l'organisation de ces réalisations. [NOYELLE89 pp.222, 226]

Les sources ne permettent toutefois pas d'établir une architecture interne complète et unique du système L.S.E. Elles ne décrivent pas de manière exhaustive les structures de données du moniteur, les mécanismes internes de changement de tâche, les formats des images sauvegardées dans la zone de swap, les interfaces entre processeurs, ni les détails d'implémentation de chacune des versions historiques.

Elles ne permettent pas non plus d'établir que les mécanismes documentés pour les implantations T 1600, Mitra 15, IBM 1130, Hewlett-Packard 2116A ou les autres matériels évoqués dans les sources sont identiques dans toutes les versions du système.

Enfin, les documents historiques ne permettent pas d'établir une représentation complète et formalisée de l'architecture interne du système L.S.E. sous la forme d'un modèle abstrait indépendant de ses implantations matérielles. Une telle représentation n'est pas fournie par les sources historiques consultées et ne peut donc pas être établie dans le cadre documentaire du présent tome.

# Chapitre 5 — Organisation générale du système

## 5.1 Analyseur de commandes

**Identifiant : HIS-5.1**

Le système L.S.E. distingue, dans son fonctionnement, les programmes appelés « processeurs ». Ces processeurs sont décrits comme des programmes du système auxquels sont confiées des tâches déterminées. Parmi eux figure l'analyseur de commandes, chargé d'examiner les commandes introduites par l'utilisateur. [INRDP75 §3.3]

L'analyseur de commandes intervient sur les lignes reconnues comme des commandes du langage de commande. Lorsque la commande est correctement écrite et correspond à une action immédiate, elle est exécutée directement. Dans les autres cas décrits dans le manuel, l'analyseur de commandes appelle le processeur correspondant à l'opération demandée, notamment pour la décompilation, l'exécution ou la gestion des fichiers. [INRDP75 §3.3]

Le manuel distingue quatre catégories de commandes : les commandes à action immédiate, les commandes faisant appel au décompilateur pour les opérations de listage, les commandes faisant appel à l'interpréteur pour l'exécution et les commandes portant sur les fichiers. Cette classification est présentée conjointement avec la description des processeurs et de leur enchaînement. [INRDP75 §3.3–§4]

Dans le fonctionnement décrit par le manuel, une même console peut donc mettre en jeu plusieurs processeurs selon la nature de l'opération demandée. Une commande de listage n'est pas traitée de la même manière qu'une commande d'exécution, et une opération portant sur un fichier fait intervenir le bibliothécaire. [INRDP75 §3.3–§4]

Le rôle de l'analyseur de commandes est également lié à la distinction entre les opérations effectuées immédiatement et celles qui nécessitent le chargement d'un autre processeur. Le schéma d'enchaînement fourni par le manuel présente cette sélection comme une étape préalable au recours au processeur concerné. [INRDP75 §3.3]

La documentation ne décrit pas en détail l'organisation interne de l'analyseur de commandes. Elle en définit le rôle fonctionnel et précise les catégories d'opérations qu'il peut reconnaître et orienter, sans fournir la description de ses structures de données ou de ses algorithmes internes. [INRDP75 §3.3]

## 5.2 Compilateur

**Identifiant : HIS-5.2**

Le compilateur L.S.E. est l'un des processeurs du système. Il analyse la ligne L.S.E. lue, détecte les erreurs de syntaxe et codifie la ligne. Le résultat de la compilation ligne à ligne est décrit comme une chaîne codée, qui pourra ensuite être exécutée par l'interpréteur. [INRDP75 §3.3]

La compilation d'une ligne ne constitue pas une opération explicitement demandée par une commande particulière. Le manuel indique que l'utilisateur n'a pas à donner de commande pour déclencher cette compilation : elle est réalisée automatiquement dès l'envoi du caractère marquant la fin de la ligne. [INRDP75 §3.3]

Cette compilation automatique concerne aussi bien les lignes introduites directement par l'utilisateur que les lignes traitées par l'intermédiaire de la commande ENCODER. Dans ce dernier cas, le manuel précise que le compilateur traite des lignes lues à partir d'un fichier données au lieu de lignes frappées au clavier ou lues sur un ruban. [INRDP75 Annexe II]

La structure du système est ainsi fondée sur une compilation effectuée au fur et à mesure de l'introduction des lignes. Une erreur syntaxique portant sur une ligne est signalée lors de cette opération et ne permet pas l'intégration de la ligne concernée dans le programme courant. [INRDP75 Annexe II]

Le manuel LSE15 décrit également l'analyse de chaque ligne dès que le caractère de fin de ligne est envoyé. Si une erreur de syntaxe est détectée, elle est signalée immédiatement et la ligne doit être corrigée puis refrappée en totalité ; les lignes précédemment introduites n'ont pas à être réintroduites. [LSE15 §1]

Le compilateur participe également au traitement de la commande ENCODER. Le manuel INRDP précise que cette commande fait intervenir deux processeurs, le bibliothécaire et le compilateur, les lignes à compiler étant fournies par un fichier données. [INRDP75 Annexe II]

La documentation historique ne fournit pas la description détaillée de la méthode utilisée par le compilateur pour transformer une ligne en chaîne codée. Le manuel définit son rôle et le résultat attendu de son fonctionnement, mais ne donne pas la structure interne du compilateur ni la description exhaustive des opérations réalisées pendant la codification. [INRDP75 §3.3]

## 5.3 Décompilateur

**Identifiant : HIS-5.3**

Le décompilateur est un processeur qui effectue l'opération inverse de celle décrite pour le compilateur. Le manuel INRDP le définit comme le processeur qui transforme la chaîne codée en une chaîne de caractères. [INRDP75 §3.3]

Cette transformation permet notamment de réaliser le listage du programme courant. La commande LISTER fait appel au décompilateur afin de restituer sous forme de caractères les lignes du programme qui doivent être affichées à l'utilisateur. [INRDP75 §3.3–§4-2]

Le décompilateur intervient également dans la commande PERFORER. Le résultat décompilé peut alors être transmis au support perforé, au lieu d'être simplement affiché par la console. Le manuel présente ainsi LISTER et PERFORER comme deux utilisations différentes du résultat de la décompilation. [INRDP75 §4-2]

La commande DECODER utilise également le décompilateur. Le manuel INRDP précise que cette commande fait appel à ce processeur, mais que le résultat de la décompilation n'est alors ni simplement affiché ni perforé : il est rangé dans un fichier données temporaire. Le bibliothécaire intervient donc également dans cette opération. [INRDP75 Annexe II]

La décompilation ne constitue donc pas uniquement une opération destinée à l'affichage. La chaîne de caractères produite peut être utilisée comme donnée, ce qui permet notamment de traiter le programme sous forme textuelle par un autre programme L.S.E. [INRDP75 §Fichiers ; Annexe II]

Le manuel LSE15 décrit de manière concordante la commande DECODER comme une commande qui range le programme courant sous forme de langage-source, c'est-à-dire sous forme d'une chaîne de caractères, dans un fichier de données temporaire. Le fichier est constitué d'enregistrements suffisamment petits pour pouvoir être chargés en mémoire centrale et la commande est destinée à permettre la modification du programme, notamment au moyen d'un programme écrit en L.S.E. [LSE15 §9-5]

La documentation historique ne précise pas le format exact de la chaîne codée ni le détail des opérations effectuées par le décompilateur pour produire la chaîne de caractères reconstruite. [INRDP75 §3.3]

## 5.4 Interpréteur

**Identifiant : HIS-5.4**

L'interpréteur est le processeur chargé d'exécuter le programme ou la ligne sous la forme fournie par le compilateur. Le manuel INRDP indique également qu'il détecte les erreurs d'exécution. [INRDP75 §3.3]

En mode machine de bureau, l'interpréteur est appelé automatiquement par le système dès que la compilation de la ligne est terminée. Les instructions contenues dans la ligne sont alors exécutées immédiatement. [INRDP75 §3.3]

En mode programmé, le fonctionnement est différent. Le programme a été constitué par plusieurs lignes précédemment introduites et compilées ; l'utilisateur demande ensuite son exécution au moyen de la commande EXECUTER. Le système charge alors l'interpréteur pour exécuter le programme. [INRDP75 §3.3–§4-3]

Le manuel décrit également l'exécution comme s'effectuant selon l'ordre croissant des numéros de lignes et, à l'intérieur d'une même ligne, dans l'ordre des instructions. Cette organisation est explicitement documentée dans le manuel LSE15. [LSE15 §1]

L'interpréteur est également impliqué dans les opérations de mise au point. Le fonctionnement décrit pour la touche d'interruption, PAUSE, PAS À PAS, CONTINUER et REPRENDRE suppose que l'exécution peut être suspendue puis reprise selon des modalités définies par le langage de commande et l'état d'exécution du programme. [INRDP75 §3-2, §4-1, §4-3 ; Annexe II] 
Le récit de conception de Yves Noyelle décrit également, pour la famille LSD/LSE, un outil de mise au point intégré permettant d'interrompre l'exécution, d'examiner ou de modifier les valeurs d'objets et de poursuivre ensuite l'exécution sans perte d'information dans les cas prévus. Il précise que cette possibilité impliquait une organisation adaptée à la fois de l'interpréteur et du système d'exploitation. [NOYELLE89 p.221]

Les sources ne décrivent toutefois pas l'organisation interne de l'interpréteur. Elles ne permettent pas d'établir les structures internes utilisées pour représenter l'état d'une exécution ni les mécanismes internes employés pour interpréter la chaîne codée. [INRDP75 §3.3 ; NOYELLE89 p.221] 
## 5.5 Bibliothécaire

**Identifiant : HIS-5.5**

Le bibliothécaire est le processeur chargé de la gestion des fichiers utilisateurs. Le manuel INRDP indique qu'il est appelé par les commandes ou instructions portant sur les fichiers programmes ou données. [INRDP75 §3.3]

Les opérations relevant du bibliothécaire comprennent notamment le rangement, l'appel, la modification et la suppression des fichiers, ainsi que les opérations portant sur les fichiers de données temporaires et permanents. [INRDP75 §Fichiers]

La commande RANGER permet de conserver sur le disque le programme courant. Dans le manuel INRDP, le programme est alors rangé sous forme codée et non sous forme d'une chaîne de caractères correspondant au texte source. [INRDP75 §4-4]

Le manuel LSE15 décrit de manière concordante la commande RANGER comme permettant de conserver dans la bibliothèque une image du programme sous le nom d'un fichier. La commande APPELER charge ensuite en mémoire le programme contenu dans un fichier-programme et en fait le programme courant de l'utilisateur. [LSE15 §9-5]

Le bibliothécaire intervient également pour les fichiers de données. La commande CHARGER permet d'utiliser les données d'un fichier avec le programme courant ; la commande CATALOGUER permet de rendre permanent un fichier données temporaire. [INRDP75 §Fichiers ; LSE15 §9-5] 
La commande DECODER illustre une utilisation conjointe du bibliothécaire et du décompilateur : le programme courant est décompilé puis le résultat textuel est rangé comme fichier données temporaire. La commande ENCODER utilise au contraire le bibliothécaire pour lire le fichier données et le compilateur pour en traiter les lignes. [INRDP75 Annexe II]

Le rôle du bibliothécaire est ainsi lié à la distinction entre programme courant, fichiers-programmes, fichiers-données temporaires et fichiers-données permanents. Cette organisation est décrite en détail dans le manuel INRDP dans la partie consacrée à la gestion des fichiers. [INRDP75 §1-1–§1-3]

## 5.6 Enchaînement des processeurs

**Identifiant : HIS-5.6**

L'architecture historique décrite par le manuel INRDP repose sur l'enchaînement de plusieurs processeurs spécialisés. Le fonctionnement général est présenté sous la forme d'un schéma simplifié qui associe les commandes et les instructions aux processeurs concernés. [INRDP75 §3.3]

Une ligne introduite par l'utilisateur peut conduire à une action immédiate, à une compilation, à une exécution ou à une opération de décompilation. Les commandes portant sur les fichiers font intervenir le bibliothécaire. [INRDP75 §3.3–§4]

Dans le mode machine de bureau, l'introduction d'une ligne conduit d'abord à sa compilation ; une fois cette compilation terminée, l'interpréteur est appelé automatiquement et la ligne est exécutée immédiatement. [INRDP75 §3.3 ; §2, mode machine de bureau] 
Dans le mode programmé, les lignes du programme sont compilées lors de leur introduction. L'exécution est différée jusqu'à ce que l'utilisateur demande l'exécution du programme au moyen de la commande EXECUTER, ce qui entraîne le recours à l'interpréteur. [INRDP75 §3.3–§4-3]

La commande LISTER fait intervenir le décompilateur afin de transformer la chaîne codée en caractères affichables. La commande PERFORER utilise également le résultat de la décompilation, mais pour produire un support perforé. [INRDP75 §3.3 ; §4-2]

La commande DECODER fait appel au décompilateur puis au bibliothécaire. Le programme courant est décompilé, mais le résultat n'est pas simplement affiché ou perforé : il est rangé dans un fichier données temporaire. [INRDP75 Annexe II]

La commande ENCODER fait intervenir le bibliothécaire et le compilateur. Les lignes à compiler sont lues à partir d'un fichier données au lieu d'être introduites directement au clavier ou lues sur un ruban. [INRDP75 Annexe II]

Le manuel INRDP précise ainsi que DECODER et ENCODER ne sont pas représentées avec toute leur précision dans le schéma général d'enchaînement des processeurs, en raison du souci de présenter séparément le rôle des différents processeurs et les commandes qui y font appel. Une remarque spécifique est consacrée à ces deux commandes pour préciser leur recours respectif aux processeurs. [INRDP75 Annexe II]

### HIS-5.6.1 — Représentation textuelle du diagramme « Enchaînement des processus »

Le diagramme historique peut être représenté, pour les besoins de la reconstruction, par le flux logique suivant. Cette représentation est une **reconstruction de l'auteur du contenu du diagramme** ; elle ne prétend pas reproduire une architecture interne non visible dans la source. [INRDP75 §3.3]

```text
ENTRÉE D'UNE LIGNE
        │
        ▼
  RECONNAISSANCE DE LA NATURE DE L'ENTRÉE
        │
        ├── commande à action immédiate ───────────────► FIN / MONITEUR
        │
        ├── commande de listage ou de restitution ─────► DÉCOMPILATEUR
        │                                                   │
        │                                                   ▼
        │                                                  FIN
        │
        ├── commande d'exécution / poursuite ───────────► INTERPRÉTEUR
        │                                                   │
        │                                                   ▼
        │                                             ÉTAT EXÉCUTION
        │
        ├── commande ou instruction de fichier ─────────► BIBLIOTHÉCAIRE
        │                                                   │
        │                                                   ▼
        │                                                  FIN
        │
        └── ligne de programme / mode machine de bureau ─► COMPILATEUR
                                                            │
                                         ┌──────────────────┴──────────────┐
                                         │                                 │
                                  mode machine de bureau             mode programmé
                                         │                                 │
                                         ▼                                 ▼
                                   INTERPRÉTEUR                    PROGRAMME COURANT
                                         │
                                         ▼                                 │
                                        FIN                         EXECUTER → INTERPRÉTEUR
```

Le diagramme associe explicitement les commandes aux processeurs ; DECODER et ENCODER font l'objet d'une précision complémentaire dans l'Annexe II. [INRDP75 §3.3 ; Annexe II]

Dans la description de la réalisation historique de LSD, Yves Noyelle fournit, pour une implantation utilisant un IBM 1130 et un Hewlett-Packard 2116A, une répartition indicative des tâches entre les deux machines : l'interprétation, la gestion des fichiers, la gestion du swap et l'accès aux objets en mode de mise au point relevaient du 1130 ; la compilation, la décompilation, l'analyse des commandes, la gestion des terminaux et du temps partagé relevaient du 2116. [NOYELLE89 p.222]

Cette répartition matérielle appartient à une implantation historique précise et ne peut pas être étendue aux différentes versions ultérieures du système sans documentation supplémentaire. Le manuel INRDP, notamment, décrit la présence des mêmes catégories fonctionnelles de processeurs sans fournir une répartition matérielle générale applicable à toutes les implantations. [INRDP75 §3.3]

### Limites des sources

Les documents historiques ne permettent pas d'établir :

- le protocole détaillé d'échange d'informations entre l'analyseur de commandes, le compilateur, le décompilateur, l'interpréteur et le bibliothécaire ;
- la structure interne des données transmises d'un processeur à l'autre ;
- le mécanisme interne utilisé pour charger ou décharger chaque processeur ;
- une répartition matérielle générale des processeurs valable pour toutes les versions historiques du système L.S.E. ;
- les différences internes éventuelles entre les implémentations du système sur T1600, MITRA 15 et d'autres matériels, au-delà des différences explicitement mentionnées par les manuels. [INRDP75 §3.3 ; NOYELLE89 p.222] 
# Chapitre 6 — Compilation

## 6.1 Structure en lignes

**Identifiant : HIS-6.1**

Le Langage Symbolique d'Enseignement (L.S.E.) est décrit comme possédant une structure de lignes. Le contenu d'une ligne est pris en compte individuellement par le système dès l'envoi du caractère signalant la fin de la ligne. Une ligne peut être une commande, une ou plusieurs instructions traitées en mode machine de bureau, ou une ligne de programme L.S.E. commençant par un numéro. [INRDP75 §3]

Le manuel LSE15 précise qu'une ligne de programme est constituée d'un numéro, suivi d'un espace dans le cas général, puis d'une ou plusieurs instructions séparées par des points-virgules et terminées par un caractère de fin de ligne. Cette organisation permet de traiter les lignes indépendamment les unes des autres. [LSE15 §1]

Le même manuel indique que chaque ligne est analysée par l'ordinateur lorsque le caractère de fin de ligne est envoyé. La compilation et l'analyse de la ligne constituent donc une opération déclenchée par la fin de l'entrée de cette ligne, et non par la constitution préalable d'un programme complet. [LSE15 §1]

La structure en lignes permet de modifier un programme sans réintroduire les autres lignes. Lorsqu'une ligne portant un numéro déjà utilisé est introduite, la dernière ligne introduite sous ce numéro prend la place de la précédente. [LSE15 §1]

Le programme est ensuite exécuté dans l'ordre croissant des numéros de lignes, indépendamment de l'ordre dans lequel les lignes ont été introduites. À l'intérieur d'une ligne, les instructions sont exécutées de la gauche vers la droite. [LSE15 §1]

Le manuel INRDP présente la même organisation générale en indiquant que la ligne constitue l'unité prise individuellement en compte par le système. Le numéro de ligne permet notamment de distinguer une ligne de programme des autres formes de lignes traitées par le système. [INRDP75 §3]

Cette organisation en lignes est directement liée au caractère conversationnel du système : l'utilisateur peut introduire, modifier ou supprimer des lignes sans avoir à soumettre de nouveau l'ensemble du programme au traitement du système. [INRDP75 §3 ; LSE15 §1] 
## 6.2 Compilation immédiate

**Identifiant : HIS-6.2**

La compilation est effectuée dès que le caractère de fin de ligne est envoyé. Le compilateur analyse alors la ligne, recherche les erreurs de syntaxe et codifie la ligne. [INRDP75 §3.3]

Dans le mode machine de bureau, cette compilation est suivie immédiatement d'une interprétation. Le manuel indique que l'interpréteur est appelé par le système dès que la compilation est terminée et que les instructions de la ligne sont alors exécutées immédiatement. [INRDP75 §3.3]

Le manuel INRDP décrit explicitement ce fonctionnement comme une exécution immédiate : toute expression introduite en mode machine de bureau est compilée puis interprétée et exécutée dès la fin de la ligne. [INRDP75 §Mode machine de bureau]

Ce mode est distinct du mode programmé. Le premier permet de soumettre directement une expression ou une opération à la console et d'en obtenir immédiatement le résultat ; le second repose sur l'exécution différée d'un programme composé de plusieurs lignes préalablement introduites. [INRDP75 §3-2 ; §Mode machine de bureau] 
Le caractère immédiat de la compilation ne signifie pas que toutes les erreurs susceptibles d'être rencontrées pendant l'exécution sont détectées à ce moment. Le manuel INRDP distingue explicitement les erreurs de syntaxe, détectées lors de l'analyse de la ligne, des erreurs qui ne pourront être rencontrées qu'au moment de l'exécution. [INRDP75 Annexe II]

## 6.3 Détection des erreurs de syntaxe

**Identifiant : HIS-6.3**

La détection des erreurs de syntaxe est l'une des fonctions explicitement attribuées au compilateur L.S.E. Le compilateur analyse la ligne lue, détecte les erreurs de syntaxe et ne produit la chaîne codée correspondante que lorsque la ligne peut être acceptée. [INRDP75 §3.3]

Le manuel LSE15 précise que l'erreur de syntaxe est signalée immédiatement après l'envoi du caractère de fin de ligne. La ligne doit alors être corrigée et refrappée dans son ensemble. Les lignes précédemment introduites ne sont pas à réintroduire. [LSE15 §1]

Le manuel INRDP indique également, dans son récapitulatif des manipulations courantes, qu'une ligne syntaxiquement incorrecte provoque l'affichage d'une erreur et n'est pas prise en compte. L'utilisateur doit alors se reporter au code d'erreur et entrer de nouveau la ligne corrigée. [INRDP75 Annexe II]

Cette organisation permet de limiter le traitement de la correction à la ligne concernée. Le mécanisme décrit ne nécessite pas que les lignes précédemment acceptées soient de nouveau analysées ou compilées lors de la correction d'une autre ligne. [LSE15 §1 ; INRDP75 Annexe II] 
Le récit de Yves Noyelle associe lui aussi la compilation ligne à ligne à une détection rapide des erreurs. Il indique que cette organisation permettait une détection rapide et complète, notamment en ce qui concerne l'emploi d'objets non initialisés ; cette dernière catégorie relève toutefois de la détection au cours de l'exécution et ne doit pas être confondue avec la détection des erreurs de syntaxe. [NOYELLE89 p.221]

## 6.4 Production d'une chaîne codée

**Identifiant : HIS-6.4**

Le résultat de la compilation d'une ligne est décrit dans le manuel INRDP comme une chaîne codée. Une fois les lignes compilées, le programme est ainsi conservé sous une forme que l'interpréteur peut exécuter. [INRDP75 §3.3]

La chaîne codée constitue donc la forme sous laquelle les lignes compilées sont conservées pour l'exécution. Le manuel n'emploie pas, dans cette description, une terminologie plus détaillée permettant de définir son organisation interne. [INRDP75 §3.3]

Le manuel INRDP distingue cette représentation codée de sa forme textuelle. La commande DECODER permet en effet de transformer le programme courant en une chaîne de caractères correspondant au langage source et de conserver cette chaîne dans un fichier données temporaire. [INRDP75 Annexe II ; §Fichiers] 
La commande RANGER, à l'inverse, conserve le programme sous une forme codée. Le manuel indique explicitement que le programme utilisateur est rangé sur le disque « sous forme codé et non sous forme d'une chaîne de caractères L.S.E. ». [INRDP75 §4-4]

Le manuel LSE15 décrit de son côté le contenu d'un fichier-programme comme une « image du programme ». L'existence d'une représentation stockée distincte de la chaîne de caractères source est ainsi également attestée par cette documentation. [LSE15 §9-5]

## 6.5 Compilation automatique

**Identifiant : HIS-6.5**

La compilation constitue une opération automatique du système. L'utilisateur n'a pas à demander explicitement la compilation d'une ligne de programme : elle intervient dès que la fin de la ligne est signalée. [INRDP75 §3.3]

Cette propriété s'applique à l'introduction interactive d'un programme, ligne par ligne. Elle s'applique également au traitement réalisé par ENCODER, dans lequel les lignes sont fournies au compilateur à partir d'un fichier données. [INRDP75 Annexe II]

Le manuel LSE15 décrit cette analyse automatique de chaque ligne comme une conséquence directe de la structure en lignes du langage. La ligne est traitée dès la réception du caractère de fin de ligne et une erreur de syntaxe est signalée sans attendre le traitement du reste du programme. [LSE15 §1]

Le caractère automatique de cette compilation participe ainsi au fonctionnement conversationnel du système : la modification d'une ligne entraîne le traitement de cette ligne seule, tandis que les autres lignes déjà acceptées restent disponibles dans le programme courant. [LSE15 §1]

Le récit de Yves Noyelle décrit également la compilation ligne à ligne comme un élément important du fonctionnement historique de LSD et de sa famille. Il indique que le temps de compilation pouvait, dans certaines conditions, être masqué par le temps nécessaire à l'affichage sur le terminal, la compilation comprenant notamment l'analyse syntaxique et la génération de la chaîne post-fixée dans la description de LSD. [NOYELLE89 p.222]

### Limites des sources

Les documents historiques ne permettent pas d'établir :

- l'algorithme exact employé pour analyser une ligne ;
- la structure interne du compilateur ;
- le format exact de la chaîne codée produite par le compilateur L.S.E. ;
- le détail des opérations de génération de cette chaîne ;
- les conditions internes précises dans lesquelles la compilation est répartie entre les différentes unités matérielles selon chaque implantation historique. [INRDP75 §3.3 ; NOYELLE89 p.222] 
# Chapitre 7 — La chaîne post-fixée

## 7.1 Existence

**Identifiant : CPF-7.1**

Les manuels INRDP et LSE15 décrivent explicitement, pour L.S.E., l'existence d'une chaîne codée résultant de la compilation ligne à ligne. Cette chaîne est exécutable par l'interpréteur et peut être transformée en chaîne de caractères par le décompilateur. [INRDP75 §3.3]

Le terme « chaîne post-fixée » apparaît explicitement dans le récit de Yves Noyelle consacré à l'histoire de LSD et de sa famille. La structure de LSD est décrite comme permettant une représentation des programmes en notation polonaise inverse, appelée dans le texte « chaîne post-fixée ». [NOYELLE89 p.220]

Le même récit indique que cette représentation pouvait être interprétée efficacement, qu'elle était peu encombrante par rapport au texte source et qu'elle était décompilable. Ces trois propriétés sont présentées comme des caractéristiques de la représentation utilisée dans LSD. [NOYELLE89 p.220]

Les manuels L.S.E. de 1975 et LSE15 de 1978 utilisent pour leur part les termes « chaîne codée », « programme codé » ou « image du programme », sans fournir, dans les passages consultés, une définition formelle établissant que la chaîne codée de chacune de ces versions est identique à la représentation post-fixée explicitement décrite par Yves Noyelle pour LSD. [INRDP75 §3.3 ; §4-4 ; LSE15 §9-5 ; NOYELLE89 p.220] 
La documentation historique permet donc d'établir l'existence d'une représentation compilée codée et, dans la description historique de LSD, d'une représentation post-fixée. En revanche, les sources ne permettent pas, à elles seules, de transformer cette description de LSD en une propriété explicitement documentée de toutes les versions ultérieures de L.S.E. [INRDP75 §3.3 ; NOYELLE89 p.220] 
## 7.2 Objectifs

**Identifiant : CPF-7.2**

Dans la description de LSD, la représentation post-fixée est présentée comme répondant à trois objectifs principaux : permettre une interprétation efficace, occuper peu de place et être décompilable. [NOYELLE89 p.220]

La première propriété rapportée est la facilité d'interprétation efficace. La source ne décrit pas l'algorithme exact de l'interpréteur, mais relie explicitement la forme post-fixée à la possibilité d'une interprétation efficace. [NOYELLE89 p.220]

La deuxième propriété est la compacité. Le texte de 1989 indique que la représentation post-fixée était environ deux fois moins encombrante que le texte source. La justification donnée est directement liée aux contraintes de mémoire centrale des machines considérées à l'époque. [NOYELLE89 p.220]

La troisième propriété est la décompilabilité. La source indique que la représentation pouvait être reconvertie en texte source pour le listage, ce qui évitait de conserver séparément le texte source dans le stockage. [NOYELLE89 p.220]

Dans le manuel INRDP, ces propriétés apparaissent sous une autre terminologie. Le programme compilé est conservé sous forme de chaîne codée ; le décompilateur permet de transformer cette chaîne codée en caractères ; le stockage du programme par RANGER est réalisé sous forme codée. [INRDP75 §3.3 ; §4-4] 
## 7.3 Informations connues

**Identifiant : CPF-7.3**

La chaîne codée résulte de la compilation des lignes du programme. Elle constitue la forme directement utilisée par l'interpréteur pour l'exécution du programme. [INRDP75 §3.3]

Elle peut également être traitée par le décompilateur. La décompilation transforme la chaîne codée en caractères, ce qui permet notamment de lister le programme courant. [INRDP75 §3.3–§4-2]

La représentation compilée peut être conservée comme fichier-programme. La commande RANGER enregistre le programme courant sous une forme codée, tandis que la commande APPELER recharge ensuite ce programme et en fait le programme courant. [INRDP75 §4-4 ; LSE15 §9-5] 
La représentation textuelle source peut également être obtenue à partir du programme courant au moyen de DECODER. Elle est alors rangée sous forme de données, et non sous la forme du fichier-programme codé. [INRDP75 Annexe II ; LSE15 §9-5] 
La distinction entre ces deux formes est donc documentée : le programme courant peut être conservé comme programme codé dans un fichier-programme, ou être transformé en une suite de caractères représentant le langage source dans un fichier données temporaire. [INRDP75 §4-4 ; Annexe II] 
## 7.4 Informations absentes

**Identifiant : CPF-7.4**

Les documents historiques ne donnent pas la définition détaillée des éléments constitutifs de la chaîne codée ou de la chaîne post-fixée. Ils ne permettent notamment pas d'établir la représentation exacte des opérations, des constantes, des variables, des références aux lignes, des appels de procédures ou des données de contrôle. [INRDP75 §3.3 ; NOYELLE89 p.220] 
Les sources disponibles ne fournissent pas non plus la liste des éventuels codes opératoires utilisés dans la chaîne codée, ni leur représentation numérique, ni la manière dont les différents éléments sont délimités dans la représentation compilée. [INRDP75 §3.3 ; NOYELLE89 p.220] 
La documentation ne permet pas davantage de déterminer la relation exacte entre la représentation post-fixée décrite explicitement pour LSD et la représentation compilée utilisée par les versions L.S.E. documentées par les manuels de 1975 et 1978. [INRDP75 §3.3 ; LSE15 §9-5 ; NOYELLE89 p.220] 
### Limites des sources

Les documents historiques ne permettent pas d'établir :

- le format complet de la chaîne codée ;
- le format complet de la chaîne post-fixée ;
- la liste des opérations codées ;
- la représentation binaire des opérations codées ;
- les mécanismes exacts de délimitation et de référencement internes ;
- l'identité exacte entre la représentation de LSD décrite par Yves Noyelle et les représentations compilées des versions L.S.E. de 1975 et 1978. [INRDP75 §3.3 ; LSE15 §9-5 ; NOYELLE89 p.220] 
# Chapitre 8 — L'interprétation

## 8.1 Exécution immédiate

**Identifiant : HIS-8.1**

L'exécution immédiate est associée au mode machine de bureau. Dans ce mode, la console étant dans l'état moniteur, l'utilisateur peut introduire une expression ou une opération à exécuter immédiatement. [INRDP75 §3-2 ; Mode machine de bureau] 
Le manuel INRDP indique que l'expression introduite est compilée, interprétée puis exécutée dès la réception du caractère de fin de ligne. La compilation et l'interprétation sont donc enchaînées sans passage par une phase d'exécution différée du programme complet. [INRDP75 §Mode machine de bureau]

Le mode machine de bureau permet notamment d'évaluer une expression et d'en faire afficher la valeur, d'obtenir la valeur de variables et d'affecter une valeur à une variable. [INRDP75 §Mode machine de bureau]

Le manuel LSE15 décrit également ce mode comme utilisable uniquement lorsque la console est dans l'état moniteur. Une expression précédée du symbole prévu à cet effet est évaluée après l'envoi du caractère de fin de ligne et sa valeur est affichée. [LSE15 §9-5]

## 8.2 Exécution différée

**Identifiant : HIS-8.2**

L'exécution différée correspond au mode programmé. Dans ce mode, l'utilisateur a préalablement introduit plusieurs lignes L.S.E. qui constituent son programme ; l'exécution du programme est ensuite demandée au moyen de la commande EXECUTER. [INRDP75 §3-2 ; §3.3] 
Le programme est exécuté selon l'ordre croissant des numéros de lignes, indépendamment de l'ordre d'entrée des lignes. À l'intérieur d'une ligne, les instructions sont exécutées de gauche à droite. [LSE15 §1]

La commande EXECUTER fait passer la console de l'état moniteur à l'état exécution. L'interpréteur est alors chargé pour effectuer l'exécution du programme courant. [INRDP75 §3.3–§4-3]

Le système distingue ainsi deux temporalités de fonctionnement : d'une part, la compilation de chacune des lignes au moment de leur entrée ; d'autre part, l'interprétation ultérieure du programme lorsque l'utilisateur demande son exécution. [INRDP75 §3.3 ; LSE15 §1] 
## 8.3 Détection des erreurs d'exécution

**Identifiant : HIS-8.3**

L'interpréteur est explicitement chargé de détecter les erreurs d'exécution. Cette détection est distincte de celle des erreurs syntaxiques effectuée au cours de la compilation de chaque ligne. [INRDP75 §3.3]

Le manuel INRDP précise, dans son récapitulatif des manipulations de mise au point, qu'un programme dont toutes les lignes ont été acceptées par l'analyse syntaxique peut néanmoins contenir des erreurs qui ne seront détectées qu'au cours de son exécution. [INRDP75 Annexe II]

Parmi les erreurs citées figurent notamment l'absence de déclarations de chaînes ou de tableaux, l'utilisation de variables non définies, certaines valeurs incorrectes de pointeurs de fonctions chaînes et des erreurs dans la gestion des variables au cours des boucles. [INRDP75 Annexe II]

Le récit de Yves Noyelle mentionne également une détection complète des erreurs, et cite expressément la détection de l'emploi d'objets non initialisés. [NOYELLE89 p.221]

Les sources établissent donc une distinction entre les erreurs de syntaxe, détectées lors de la compilation d'une ligne, et les erreurs d'exécution, détectées par l'interpréteur lors du fonctionnement du programme. [INRDP75 §3.3 ; Annexe II] 
## 8.4 Arrêt sur erreur

**Identifiant : HIS-8.4**

Une erreur d'exécution entraîne l'arrêt de l'exécution du programme. Le manuel décrit les erreurs d'exécution comme des erreurs signalées « en ligne », l'exécution ne pouvant alors se poursuivre normalement. [INRDP75 Annexe II]

La touche d'interruption constitue un autre moyen d'arrêter une exécution. Elle peut être actionnée à tout moment et, lorsque la console est en état d'exécution, provoque son retour à l'état moniteur. Le système affiche alors le message PRET et le numéro de la ligne qui était en cours d'exécution. [INRDP75 §3-2]

Le manuel LSE15 précise que cette touche peut être utilisée même lorsque le programme tourne dans une boucle sans fin ou lorsque la console est en cours d'affichage. Elle permet ainsi à l'utilisateur de reprendre le contrôle de la console. [LSE15 §9-4]

Dans la description historique de Yves Noyelle, le programme pouvait être interrompu à tout moment par la touche ESC, y compris pendant une impression. L'état de l'exécution pouvait ensuite être conservé pour permettre sa poursuite dans les situations où celle-ci était autorisée. [NOYELLE89 p.221]

La distinction entre un arrêt provoqué par la touche d'interruption, un arrêt provoqué par PAUSE et un arrêt résultant d'une erreur est maintenue par les documents historiques. L'arrêt sur erreur ne permet pas de reprendre l'exécution dans les conditions offertes par un arrêt volontaire. [NOYELLE89 p.221]

## 8.5 Reprise

**Identifiant : HIS-8.5**

La commande CONTINUER permet de reprendre l'exécution après un arrêt provoqué par PAUSE ou par la touche d'interruption. Le manuel indique que l'exécution reprend à l'endroit où elle a été arrêtée. [INRDP75 §4-3]

La commande REPRENDRE permet au contraire de relancer l'exécution à partir d'une autre ligne. Les valeurs affectées aux identificateurs avant l'interruption sont conservées. [INRDP75 §4-3]

Le manuel précise également que REPRENDRE reprend l'exécution au niveau principal, même lorsque le programme avait été interrompu dans une procédure. [INRDP75 §4-3]

Le récit de Yves Noyelle décrit le même principe général de conservation de l'état lors d'un arrêt volontaire : après interruption, l'exécution pouvait reprendre sans perte d'information, y compris sans perte de caractère lorsqu'un arrêt avait lieu pendant une impression. [NOYELLE89 p.221]

Cette possibilité de reprise n'est pas applicable de la même manière à un arrêt sur erreur. Yves Noyelle précise explicitement qu'après une erreur, l'exécution ne pouvait évidemment pas continuer. [NOYELLE89 p.221]

La documentation LSE15 confirme par ailleurs que la touche d'interruption, la commande CONTINUER et le mode machine de bureau peuvent être utilisés conjointement pour la mise au point d'un programme. [LSE15 §9-4–§9-5]

### Limites des sources

Les documents historiques ne permettent pas d'établir :

- la représentation interne complète de l'état d'exécution conservé lors d'un arrêt ;
- la manière exacte dont l'état d'une exécution est sauvegardé et restauré ;
- la structure interne des informations utilisées pour mémoriser la ligne d'arrêt ;
- les mécanismes internes permettant la conservation de l'état lors d'une reprise ;
- les modalités internes de traitement des erreurs par l'interpréteur, au-delà des effets observables décrits dans les manuels. [INRDP75 §3.3–§4-3 ; NOYELLE89 p.221] 
# Chapitre 9 — Décompilation

## 9.1 Principe

**Identifiant : HIS-9.1**

La décompilation consiste à transformer la chaîne codée du programme en une chaîne de caractères. Cette opération est assurée par le décompilateur. [INRDP75 §3.3]

Le résultat de la décompilation peut être utilisé pour reconstituer la représentation textuelle du programme. Cette propriété permet notamment d'afficher les lignes du programme courant au moyen de la commande LISTER. [INRDP75 §3.3–§4-2]

La décompilation ne modifie donc pas, dans la description fournie par les sources, la nature du programme courant en tant que représentation compilée. Elle produit une représentation sous forme de caractères destinée à l'affichage ou au stockage sous forme de données. [INRDP75 §3.3 ; Annexe II] 
Le manuel LSE15 décrit DECODER comme une opération qui range le programme courant sous forme de langage-source dans un fichier données temporaire. Le résultat est donc une représentation textuelle distincte du fichier-programme codé. [LSE15 §9-5]

## 9.2 Utilisation

**Identifiant : HIS-9.2**

La commande LISTER utilise le décompilateur pour afficher le programme courant. Elle peut porter sur une plage de numéros de lignes déterminée par l'utilisateur. [INRDP75 §4-2]

La commande PERFORER utilise également la décompilation. Elle permet d'obtenir un ruban perforé contenant le programme L.S.E. introduit à la console. [INRDP75 §4-2]

La commande DECODER utilise le même processeur mais affecte au résultat une autre destination. Le résultat décompilé est rangé dans un fichier données temporaire au lieu d'être directement affiché ou perforé. [INRDP75 Annexe II]

Le manuel LSE15 indique que le fichier produit par DECODER contient une suite d'enregistrements de taille suffisamment petite pour que chacun puisse être chargé en mémoire centrale. La commande est destinée à permettre la modification de la représentation textuelle du programme, notamment par un programme écrit en L.S.E. [LSE15 §9-5]

Cette possibilité permet notamment de traiter le programme comme une donnée. Le manuel INRDP donne l'exemple d'un programme pouvant travailler sur un fichier contenant la représentation textuelle d'un autre programme, notamment pour en modifier les numéros de lignes. [INRDP75 §Fichiers]

## 9.3 Conséquences sur le stockage

**Identifiant : HIS-9.3**

Le principe de décompilation permet de distinguer le stockage du programme courant sous forme codée et sa restitution sous forme de langage-source. Le manuel INRDP indique que le programme rangé au moyen de RANGER est conservé sous forme codée et non sous forme d'une chaîne de caractères L.S.E. [INRDP75 §4-4]

Cette organisation permet de conserver le programme sous une forme différente de celle nécessaire à son affichage. Lorsque le programme doit être listé, le décompilateur produit les caractères nécessaires à partir de la représentation codée. [INRDP75 §3.3–§4-2]

Dans le récit de Yves Noyelle, cette propriété est présentée comme un avantage direct de la représentation post-fixée de LSD : il n'était pas nécessaire de conserver le texte source, qui aurait occupé de la place sur le disque et augmenté le temps d'accès ; le texte était recréé à partir de la chaîne post-fixée au moment du listage. [NOYELLE89 p.220]

L'opération DECODER constitue toutefois un moyen de produire explicitement une représentation source sous forme de fichier données. Cette représentation n'est pas le fichier-programme lui-même : elle est générée à partir de celui-ci et peut ensuite être modifiée avant de faire l'objet d'une nouvelle opération ENCODER. [INRDP75 Annexe II ; LSE15 §9-5] 
La commande ENCODER réalise l'opération inverse sur le plan fonctionnel : elle lit dans un fichier données les lignes représentant un programme L.S.E. et les soumet au compilateur, de manière analogue à une entrée directe depuis la console ou le ruban. [INRDP75 Annexe II]

### Limites des sources

Les documents historiques ne permettent pas d'établir :

- le format détaillé des données produites par le décompilateur ;
- le format interne des enregistrements produits par DECODER au-delà de leur caractère suffisamment petit pour être chargé en mémoire centrale ;
- la méthode exacte de reconstruction du texte source ;
- la représentation complète des relations entre le texte source, la chaîne codée et la chaîne post-fixée dans toutes les versions historiques de L.S.E. [INRDP75 §3.3 ; Annexe II ; LSE15 §9-5 ; NOYELLE89 p.220] 
# Chapitre 10 — Gestion mémoire

## 10.1 Zone utilisateur

**Identifiant : HIS-10.1**

Dans le système en temps partagé décrit par le manuel INRDP, ce n'est pas la mémoire qui est partagée entre les utilisateurs, mais le temps consacré aux différentes tâches. Chaque tâche est alternativement chargée dans une partie de la mémoire appelée « zone données-utilisateur » puis exécutée. [INRDP75 §8-1]

À un instant donné, une seule tâche est active. Lorsqu'une tâche cesse temporairement d'être active, son état doit être conservé pour pouvoir être restauré lors d'une activation ultérieure. Le manuel attribue cette fonction à la zone de swap située sur le disque. [INRDP75 §8-1]

La zone de swap est divisée en autant de parties qu'il y a de consoles connectées au système. Chaque partie a une longueur égale à celle de la zone données-utilisateur et contient l'image de la tâche correspondant à une console. [INRDP75 §8-1]

Le moniteur de temps partagé effectue notamment les opérations de chargement en mémoire et d'activation, désignées par SWAP IN, sélectionne la tâche à activer, charge le processeur compétent, charge la tâche puis, lors de l'abandon de la tâche active, recopie son état dans la zone de swap, opération désignée par SWAP OUT. [INRDP75 §8-1]

La tâche active cesse d'occuper le processeur lorsque le temps limite qui lui est attribué est atteint ou lorsqu'une demande d'entrée-sortie conduit à suspendre son activité. Le manuel cite notamment l'affichage et la lecture d'une ligne ou d'une donnée tapée par l'utilisateur. [INRDP75 §8-1]

Le système prévoit également des priorités pouvant entraîner la réactivation d'une tâche avant son tour normal. Le manuel indique en particulier que la tâche de compilation associée à l'entrée d'un programme est prioritaire, en raison de l'attente importante liée à la frappe des lignes. [INRDP75 §8-1]

Dans la description de la réalisation historique de LSD, Yves Noyelle indique que la mémoire centrale du 1130 et du 2116 était de 16 Ko et qu'une technique de swap permettait à chaque utilisateur d'occuper jusqu'à environ 10 Ko dans la réalisation décrite. [NOYELLE89 p.220]

Le manuel INRDP indique de son côté une taille de zone données-utilisateur de 2000 mots sur MITRA 15 et de 2800 mots sur T1600 dans la configuration décrite. [INRDP75 §8-1]

Ces dimensions sont donc propres aux configurations matérielles décrites par les documents et ne constituent pas une valeur générale indépendante du matériel. [INRDP75 §8-1 ; Remarques de présentation]

## 10.2 Programme

**Identifiant : HIS-10.2**

Le programme courant de l'utilisateur fait partie des informations associées à sa tâche. Lorsqu'il est introduit en machine, ses lignes sont compilées au fur et à mesure et conservées sous une forme codée utilisable par l'interpréteur. [INRDP75 §3.3]

Le programme courant est associé à la zone de swap de la console lorsqu'il n'est pas actif en mémoire centrale. Le manuel INRDP indique que chaque console dispose d'une zone de swap contenant notamment son programme courant. [INRDP75 §Fichiers ; §8-1] 
Le programme courant peut être conservé au-delà de la session en le transformant en fichier-programme. Le manuel INRDP précise que la commande AU détruit le programme courant, comme les fichiers temporaires ; un programme destiné à être conservé doit donc être enregistré comme fichier-programme. [INRDP75 §1-1]

Le fichier-programme est une information permanente. Le manuel décrit le stockage du programme sous forme codée, par opposition à la représentation sous forme de chaîne de caractères obtenue par DECODER. [INRDP75 §4-4 ; Annexe II] 
Le manuel LSE15 décrit également RANGER comme une commande qui conserve une image du programme dans la bibliothèque, tandis qu'APPELER charge cette image en mémoire pour en faire le programme courant. [LSE15 §9-5]

## 10.3 Données

**Identifiant : HIS-10.3**

Les données utilisées par un programme peuvent être conservées dans des fichiers données. Le manuel INRDP distingue deux catégories de fichiers données selon leur durée de conservation : les fichiers données temporaires et les fichiers données permanents. [INRDP75 §1-1]

Les fichiers données temporaires sont conservés pendant la durée de la session, entre BONJOUR et AU REVOIR. Ils sont automatiquement détruits à la fin de la session. [INRDP75 §1-1]

Les fichiers données permanents sont conservés sans limitation de durée et ne sont détruits qu'à la demande de leur créateur. [INRDP75 §1-1]

Le système prévoit ainsi une séparation entre les données temporaires, directement associées à une session, et les données permanentes conservées sur le disque. [INRDP75 §1-1–§1-2]

Le manuel INRDP précise que chaque console dispose, dès l'ouverture de la session, d'une zone particulière destinée aux fichiers données temporaires. Une taille minimale est garantie pour chaque console et une extension de cette zone peut être demandée sous certaines conditions. [INRDP75 §1-2]

La zone temporaire standard attribuée à une console ne peut pas être récupérée par une autre console, même lorsqu'elle n'est pas utilisée. Une extension accordée à une console bénéficie également d'une protection jusqu'à sa libération. [INRDP75 §1-2]

La zone des fichiers permanents occupe l'espace restant sur le disque après déduction de la zone système, des zones de swap et des espaces temporaires. Elle contient les fichiers-programmes et les fichiers-données permanents. [INRDP75 §1-2]

## 10.4 Chaînes

**Identifiant : HIS-10.4**

Le manuel INRDP distingue trois catégories de variables : les variables arithmétiques simples, les variables indicées et les variables chaînes. [INRDP75 §3-1]

Les variables chaînes doivent être déclarées explicitement au moyen de l'instruction CHAINE. Le manuel précise que cette déclaration correspond à une information fournie au système et non à une instruction exécutable au sens des instructions de traitement. [INRDP75 §3-2]

Les chaînes sont également utilisées comme fichiers données. Dans le stockage sur disque décrit par le manuel INRDP, une donnée de type chaîne de caractères est stockée à raison d'un caractère par octet. Les capacités par secteur diffèrent selon le matériel : 254 caractères par secteur sur T1600 et 252 caractères dans le premier secteur puis 254 dans les suivants sur MITRA 15. [INRDP75 §Fichiers]

La représentation textuelle produite par DECODER est également décrite comme une suite de caractères. Le fichier données temporaire obtenu peut ensuite être traité par un programme L.S.E. [INRDP75 Annexe II ; LSE15 §9-5] 
La documentation historique n'indique pas, dans les passages consacrés à la gestion mémoire, une structure interne plus détaillée des variables chaînes ou une représentation complète des en-têtes associés à ces données en mémoire centrale. [INRDP75 §3-2 ; §Fichiers] 
## 10.5 Nombres

**Identifiant : HIS-10.5**

Le manuel INRDP indique qu'un nombre occupe en général deux mots mémoire, soit quatre octets, sous une forme comprenant une mantisse et un exposant. [INRDP75 §3-3]

La place mémoire étant limitée, le nombre de chiffres représentables est limité. Le manuel indique que les constructeurs garantissent six chiffres significatifs, correspondant à une précision de l'ordre de 10^-6. [INRDP75 §3-3]

Le nombre est également utilisé dans les fichiers données. Le manuel INRDP indique que les données numériques de type tableau sont stockées à raison d'un nombre pour quatre octets. [INRDP75 §Fichiers]

La documentation mentionne également le stockage d'une constante sur quatre octets. Elle précise qu'un stockage isolé d'une constante peut conduire à réserver un secteur entier, ce qui explique la recommandation historique de regrouper plusieurs constantes dans un tableau lorsque cela est nécessaire pour limiter la perte d'espace. [INRDP75 §Fichiers]

Ces indications concernent la représentation des nombres utilisée par les versions matérielles décrites dans le manuel. Elles ne permettent pas d'établir une représentation numérique universelle pour toutes les implémentations historiques de L.S.E. [INRDP75 §3-3 ; §Fichiers] 
## 10.6 Tableaux

**Identifiant : HIS-10.6**

Les variables indicées sont désignées par le manuel INRDP sous le terme de tableaux. Un tableau peut comporter un ou deux indices. Sa déclaration s'effectue au moyen de l'instruction TABLEAU. [INRDP75 §3-1 ; §4-1]

La déclaration d'un tableau permet de préciser l'espace à réserver pour ses éléments. Les tableaux sont donc traités comme des objets dont l'occupation mémoire dépend de leur déclaration et de leur taille. [INRDP75 §4-1]

Le manuel LSE15 indique qu'un tableau peut avoir une ou deux dimensions et que sa déclaration peut intervenir n'importe où dans le programme, sous réserve qu'elle soit exécutée avant l'utilisation du tableau. L'utilisation d'un élément d'un tableau non déclaré provoque une erreur d'exécution. [LSE15 §3-1]

Une déclaration TABLEAU portant sur un identificateur qui désignait déjà un tableau provoque la libération implicite de ce tableau. En revanche, une erreur est détectée si l'identificateur désignait une variable simple ou une variable chaîne. [LSE15 §3-1]

L'instruction LIBERER permet de libérer l'espace occupé par un tableau ou une variable simple. Le manuel présente cette possibilité comme utile notamment lorsqu'il faut utiliser successivement de grands tableaux qui ne pourraient pas tenir simultanément en mémoire. [INRDP75 §4-1 ; LSE15 §3-1]

Dans le récit de conception de LSD, Yves Noyelle indique qu'un seul genre d'objets composés avait été retenu : le tableau, à une ou deux dimensions. Il relie également la déclaration dynamique des tableaux à la possibilité de moduler l'encombrement mémoire d'une tâche dans un système utilisant le swap. [NOYELLE89 p.219]

Le stockage des données de tableau sur disque est décrit à raison d'un nombre pour quatre octets. La capacité effectivement disponible par secteur dépend du matériel et des informations réservées par le système : le manuel indique 63 valeurs pour un tableau à une dimension et 62 valeurs pour un tableau à deux dimensions dans les configurations décrites. [INRDP75 §Fichiers]

### Limites des sources

Les documents historiques ne permettent pas d'établir :

- la carte mémoire complète d'une tâche ;
- la localisation exacte de chacun des objets dans la zone données-utilisateur ;
- la représentation interne détaillée des variables, chaînes et tableaux en mémoire centrale ;
- le mécanisme exact de l'allocation et de la libération des objets ;
- la structure interne de la table permettant d'associer les identificateurs aux emplacements mémoire ;
- les différences internes entre les mécanismes mémoire des diverses implantations historiques lorsque celles-ci ne sont pas explicitement documentées. [INRDP75 §8-1 ; §3 ; §Fichiers ; LSE15 §3-1 ; NOYELLE89 p.219] 
# Chapitre 11 — Gestion des fichiers

## 11.1 Bibliothécaire

**Identifiant : HIS-11.1**

La gestion des fichiers utilisateurs relève du bibliothécaire, qui est l'un des processeurs du système. Le manuel INRDP indique que ce processeur est appelé par les commandes et instructions portant sur les fichiers programmes et les fichiers données. [INRDP75 §3.3]

Les fichiers sont destinés à conserver soit des programmes, soit des données. Le manuel distingue, pour les programmes, le programme courant et le fichier-programme permanent ; pour les données, les fichiers temporaires et les fichiers permanents. [INRDP75 §1-1]

Le fonctionnement de la bibliothèque ne se limite pas au rangement des programmes. Il comprend également leur appel, leur remplacement, leur suppression et la gestion des fichiers de données. [INRDP75 §Fichiers ; LSE15 §9-5] 
Le bibliothécaire intervient également lorsque le système doit mettre en relation un programme et un fichier de données. La commande CHARGER permet ainsi de charger des données qui seront exploitées par le programme courant. [INRDP75 §Fichiers]

## 11.2 Programmes

**Identifiant : HIS-11.2**

Le programme courant est l'objet de travail associé à une session. Il peut être modifié par l'introduction, la modification ou la suppression de lignes. [INRDP75 §3 ; LSE15 §1] 
Un programme destiné à être conservé au-delà de la session doit être transformé en fichier-programme. Le manuel INRDP précise que le programme courant est détruit par la commande AU et qu'un programme à conserver doit être rangé sous forme de fichier-programme permanent. [INRDP75 §1-1]

La commande RANGER permet de conserver le programme courant sous un nom de fichier dans la bibliothèque. Sur la documentation INRDP, le programme est rangé sous forme codée. [INRDP75 §4-4]

La commande APPELER permet de charger en mémoire un fichier-programme. Le programme ainsi chargé devient le programme courant de la console. [INRDP75 §4-4 ; LSE15 §9-5] 
La commande MODIFIER permet de remplacer dans la bibliothèque un programme précédemment rangé par l'image du programme courant. Le manuel LSE15 précise que cette commande réalise en fait l'enchaînement d'une suppression et d'un rangement portant sur le même nom de fichier. [LSE15 §9-5]

La commande SUPPRIMER permet d'éliminer de la bibliothèque un fichier-programme ou un fichier-donnée. [LSE15 §9-5]

## 11.3 Données

**Identifiant : HIS-11.3**

Les fichiers données temporaires sont attachés à la console qui les utilise. Une console ne peut pas utiliser directement les fichiers temporaires appartenant à une autre console. [INRDP75 §1-3]

Cette organisation permet à plusieurs utilisateurs de disposer de fichiers temporaires portant le même nom sans ambiguïté pour le système, puisque chaque utilisateur n'a accès qu'à son propre espace temporaire. [INRDP75 §1-3]

La zone des fichiers permanents est, en principe, accessible en lecture à tous les utilisateurs. Un fichier-programme peut être appelé et un fichier-donnée peut être chargé. [INRDP75 §1-3]

Cette accessibilité est toutefois soumise à des mécanismes de protection. Certains programmes peuvent être protégés en exécution seule, tandis qu'une protection absolue de la lecture des fichiers permanents est également mentionnée pour le T1600. [INRDP75 §1-3]

La zone de swap ne peut pas être modifiée par une autre console. Celle-ci peut lire le contenu de la zone de swap d'une autre console au moyen de PRENDRE ETAT CONSOLE, mais ne peut pas y écrire. [INRDP75 §1-3]

La commande CATALOGUER permet de rendre permanent un fichier données temporaire. Le fichier temporaire est alors détruit et l'espace correspondant devient réutilisable pour d'autres fichiers temporaires. [LSE15 §9-5]

## 11.3.1 Identification des utilisateurs et protections des fichiers (HIS-11.3.1)

La gestion des fichiers permanents est soumise à un mécanisme d'identification. La commande IDENTIFICATION doit précéder toute création, modification ou suppression d'un fichier permanent. L'utilisateur fournit une clé et un numéro de compte ; la clé n'est pas imprimée sur la console. [INRDP75 §Fichiers]

Un numéro d'identification associe un numéro de compte à une clé et possède des attributs déterminant les opérations autorisées. Les attributs documentés concernent : la création, modification et suppression de fichiers-programmes ; la création et suppression de fichiers-données permanents ; l'augmentation dynamique de l'espace temporaire sur MITRA 15 ; et l'attribution d'un espace temporaire fixe supérieur à l'espace standard lorsque cette possibilité est définie. [INRDP75 §Fichiers]

Un utilisateur dépourvu de numéro d'identification ne dispose que de l'espace temporaire standard et ne peut pas écrire dans les fichiers permanents. Plusieurs identifications peuvent être utilisées au cours d'une même session, mais une seule est valide à un instant donné : la dernière indiquée. Le changement d'identification détruit les fichiers temporaires créés par l'utilisateur. [INRDP75 §Fichiers]

Tout fichier permanent créé sous une identification donnée ne peut être modifié ou supprimé que sous cette même identification. La zone des fichiers permanents est en principe accessible en lecture, mais des protections peuvent limiter cet accès. Les programmes « à exécution seule » peuvent être exécutés sans pouvoir être listés ou modifiés ; sur T1600, une protection absolue peut également interdire la lecture d'un fichier permanent sans l'identification de création. Sur MITRA 15, les protections sont notamment déterminées par les numéros d'identification affectés aux utilisateurs. [INRDP75 §Fichiers]

Les commandes TA et UT sont elles-mêmes soumises à des règles dépendant de l'identification et de l'implantation. TA liste les fichiers associés à l'identification courante ; UT permet de connaître l'utilisation du disque, mais son accès est restreint sur MITRA 15. [INRDP75 §Fichiers]

### Limites des sources

Les sources ne permettent pas d'établir les structures internes utilisées pour conserver les identifications, les clés ou les droits, ni l'algorithme interne de vérification. Elles établissent en revanche les règles observables d'identification, de propriété et de protection des fichiers permanents. [INRDP75 §Fichiers]

## 11.4 Stockage des programmes

**Identifiant : HIS-11.4**

Le disque est décrit par le manuel INRDP comme composé principalement de quatre zones : la zone système, la zone de swap, la zone des fichiers données temporaires et la zone des fichiers permanents. [INRDP75 §1-2]

La zone système contient les programmes du système L.S.E., notamment le compilateur, l'interpréteur et l'analyseur de commandes. L'utilisateur n'a pas accès à cette zone. [INRDP75 §1-2]

La zone de swap contient l'état des tâches de tous les utilisateurs. Elle constitue donc une zone distincte des fichiers permanents et des espaces temporaires. [INRDP75 §1-2 ; §8-1] 
La zone des fichiers données temporaires est attachée aux consoles et comprend l'espace standard attribué à chaque console ainsi que, dans certaines conditions, une éventuelle extension. [INRDP75 §1-2]

La zone des fichiers permanents contient les fichiers-programmes et les fichiers-données permanents. [INRDP75 §1-2]

Les noms de fichiers obéissent à une règle de formation comparable à celle des identificateurs : une lettre suivie d'au plus quatre caractères alphanumériques. Le nom constitue l'identification utilisée par le système pour retrouver le fichier. [INRDP75 §Fichiers]

Les fichiers temporaires peuvent porter le même nom sur plusieurs consoles, alors que les fichiers permanents doivent être distingués les uns des autres dans la bibliothèque. [INRDP75 §1-2–§1-3 ; LSE15 §9-5] 
## 11.5 Chargement

**Identifiant : HIS-11.5**

La commande APPELER est utilisée pour charger en mémoire un programme contenu dans un fichier-programme. Le programme chargé devient le programme courant de l'utilisateur et peut ensuite être listé ou exécuté. [INRDP75 §4-4]

La commande CHARGER est utilisée pour accéder aux données d'un fichier données à partir du programme courant. Le fichier chargé n'est donc pas assimilé au programme courant lui-même. [INRDP75 §Fichiers]

Le manuel LSE15 décrit également APPELER comme une opération qui charge en mémoire le programme contenu dans un fichier-programme. Le programme devient alors le programme courant de l'utilisateur. [LSE15 §9-5]

La commande ENCODER constitue un autre mode de chargement de programme, mais elle fonctionne à partir d'un fichier données contenant une représentation textuelle du programme. Les lignes de ce fichier sont alors traitées par le compilateur comme si elles avaient été introduites par l'utilisateur. [INRDP75 Annexe II ; LSE15 §9-5] 
Le programme résultant de cette opération est donc un nouveau programme courant produit à partir de la représentation textuelle lue dans le fichier données. [INRDP75 Annexe II]

### Limites des sources

Les documents historiques ne permettent pas d'établir :

- la structure détaillée de la bibliothèque et de ses index ;
- la représentation interne des entrées de fichiers ;
- l'algorithme de recherche d'un fichier à partir de son nom ;
- les structures exactes utilisées pour gérer l'allocation et la libération des espaces temporaires ;
- les mécanismes internes précis des protections de fichiers ;
- le détail des opérations effectuées lors du chargement d'un programme ou de données, au-delà de leurs effets documentés. [INRDP75 §1-2–§1-3 ; §4-4 ; LSE15 §9-5] 
# Chapitre 12 — Robustesse du système

## 12.1 Détection des erreurs

**Identifiant : HIS-12.1**

La robustesse du système est étroitement liée à la détection des erreurs. La compilation ligne à ligne permet une détection immédiate des erreurs syntaxiques lorsqu'une ligne est introduite. [INRDP75 §3.3 ; LSE15 §1] 
Les erreurs qui ne peuvent être détectées à ce stade sont susceptibles d'être rencontrées lors de l'exécution. Le système signale alors les erreurs d'exécution et arrête l'exécution du programme. [INRDP75 §3.3 ; Annexe II] 
Yves Noyelle décrit cette détection comme rapide et complète et mentionne explicitement la détection de l'emploi d'objets non initialisés. [NOYELLE89 p.221]

La détection des erreurs constitue ainsi un élément du fonctionnement courant du système et non une opération séparée réservée à une procédure de vérification externe. [INRDP75 §3.3 ; NOYELLE89 p.221] 
## 12.2 Protection de l'utilisateur

**Identifiant : HIS-12.2**

La protection de l'utilisateur apparaît dans la conception historique du système sous plusieurs formes. Le système distingue notamment les espaces appartenant aux différentes consoles et interdit à une console d'écrire dans la zone de swap d'une autre console. [INRDP75 §1-3]

Les fichiers données temporaires sont également protégés contre les accès d'écriture depuis une autre console. Une console ne peut créer, modifier ou supprimer que les fichiers temporaires appartenant à son propre espace. [INRDP75 §1-3]

Cette protection permet à un même programme d'être exécuté simultanément sur plusieurs consoles en utilisant un nom identique pour un fichier temporaire, chaque console travaillant sur son propre fichier. [INRDP75 §1-3]

Le mécanisme de protection existe également pour les fichiers permanents. Des programmes peuvent être déclarés à exécution seule et certaines protections supplémentaires sont prévues sur T1600. [INRDP75 §1-3]

Le récit de Yves Noyelle associe cette philosophie de protection à la volonté de ne pas faire subir à un utilisateur les conséquences d'une erreur ou d'une fausse manipulation. Cette orientation est présentée comme l'un des principes directeurs du développement des langages de la famille. [NOYELLE89 p.216-218] 
## 12.3 Objet non initialisé

**Identifiant : HIS-12.3**

Le manuel INRDP indique qu'une variable utilisée dans une expression doit avoir reçu préalablement une valeur déterminée. Dans le cas contraire, un message d'erreur est produit. [INRDP75 §3-3]

Le manuel indique ainsi que le système ne laisse pas une variable non définie fournir silencieusement une valeur arbitraire au programme. L'utilisation d'une variable sans valeur déterminée est détectée comme une erreur. [INRDP75 §3-3]

Dans son récit historique, Yves Noyelle souligne également la détection de l'emploi d'objets non initialisés parmi les propriétés de détection complète des erreurs recherchées dans le système. [NOYELLE89 p.221]

Le manuel LSE15 fournit des exemples complémentaires du même principe pour les tableaux : toute tentative d'utilisation d'un élément d'un tableau non déclaré entraîne une erreur d'exécution. [LSE15 §3-1]

La documentation distingue ainsi l'absence de définition d'une variable, l'absence de déclaration d'un tableau ou d'une variable chaîne et les autres erreurs susceptibles de n'être détectées qu'au moment de l'exécution. [INRDP75 §3-2–§3-3 ; Annexe II] 
## 12.4 Résistance aux erreurs de manipulation

**Identifiant : HIS-12.4**

La résistance aux erreurs de manipulation constitue un objectif explicitement mentionné dans le récit historique de la conception de LSD. Yves Noyelle indique qu'il fallait que le produit résiste à n'importe quelle fausse manipulation, l'erreur étant considérée comme inévitable dans l'utilisation d'un système par des personnes. [NOYELLE89 p.217]

Cette orientation se retrouve dans la séparation des espaces appartenant aux différentes consoles. Une console ne peut pas écrire dans la zone de swap d'une autre console ni modifier ses fichiers temporaires. [INRDP75 §1-3]

La protection des fichiers permanents constitue un autre mécanisme de limitation des effets d'une utilisation non autorisée. Les programmes à exécution seule peuvent être exécutés sans pouvoir être listés ou modifiés dans les conditions ordinaires. [INRDP75 §1-3]

Le système permet également à l'utilisateur de reprendre le contrôle de sa console au moyen de la touche d'interruption, même lorsque l'exécution est engagée dans une boucle sans fin ou pendant un affichage. [LSE15 §9-4]

Le récit de Yves Noyelle ajoute que le système avait été conçu de façon à ce qu'un programme ne soit pas perdu du fait du système, y compris en cas de plantage. Cette exigence est présentée comme un objectif de fonctionnement du système d'exploitation. [NOYELLE89 p.222]

## 12.5 Interruption de l'exécution

**Identifiant : HIS-12.5**

La touche d'interruption permet à l'utilisateur de reprendre le contrôle de sa console à tout moment. Lorsqu'elle est actionnée pendant l'exécution, la console passe à l'état moniteur et affiche PRET ainsi que le numéro de la ligne en cours d'exécution. [INRDP75 §3-2]

Le manuel LSE15 précise que cette possibilité demeure disponible même en cas de boucle sans fin ou lorsque la console est en cours d'affichage. [LSE15 §9-4]

La touche d'interruption peut également être utilisée pendant une opération de listage lorsque la console est déjà en état moniteur. Dans ce cas, le listage est interrompu et le système affiche PRET. [INRDP75 §3-2]

L'arrêt sur interruption est associé aux commandes CONTINUER et REPRENDRE. CONTINUER reprend l'exécution à l'endroit de l'arrêt ; REPRENDRE permet de repartir à partir d'une autre ligne tout en conservant les valeurs antérieurement affectées aux identificateurs. [INRDP75 §4-3]

Yves Noyelle décrit cette possibilité comme un outil de mise au point intégré. Il précise qu'un programme pouvait être interrompu à n'importe quel moment, qu'il était alors possible d'examiner ou de modifier les valeurs d'objets et que l'exécution pouvait ensuite reprendre sans perte d'information dans les situations prévues. [NOYELLE89 p.221]

### Limites des sources

Les documents historiques ne permettent pas d'établir :

- la totalité des mécanismes internes de protection contre les erreurs de manipulation ;
- la représentation interne des erreurs et des diagnostics ;
- la méthode exacte utilisée pour détecter tous les cas d'objets non initialisés ;
- les mécanismes internes assurant la reprise sans perte d'information ;
- les mécanismes internes de protection contre une panne générale du système, au-delà des objectifs et effets décrits par les sources. [INRDP75 §3-3 ; §1-3 ; §4-3 ; NOYELLE89 p.221-222] 
# Chapitre 13 — Mise au point des programmes

## 13.1 PAS À PAS

**Identifiant : HIS-13.1**

La commande PAS À PAS permet de faire arrêter l'exécution au début de chaque ligne. Le système fait alors passer la console dans l'état moniteur et affiche le numéro de la ligne atteinte. [INRDP75 §4-1]

La commande peut être utilisée avant EXECUTER, CONTINUER ou REPRENDRE. Son fonctionnement est donc associé aussi bien au démarrage d'une exécution qu'à la reprise d'une exécution interrompue. [INRDP75 §4-1]

Pour poursuivre l'exécution après un arrêt obtenu par PAS À PAS, l'utilisateur peut frapper la touche d'interruption ou utiliser le mode machine de bureau. Il doit ensuite utiliser CONTINUER pour revenir à l'exécution du programme. [INRDP75 §4-1]

Le manuel INRDP décrit le PAS À PAS comme un moyen de mise au point permettant d'examiner la valeur des variables avant chaque exécution de ligne. L'annexe consacrée aux manipulations courantes recommande cette possibilité lorsque l'origine d'une erreur d'exécution doit être recherchée. [INRDP75 Annexe II]

Dans le mode PAS À PAS, le système affiche à chaque étape le numéro de la ligne qui va être exécutée. Avant l'exécution de cette ligne, l'utilisateur peut effectuer des interrogations en mode machine de bureau. [INRDP75 Annexe II]

Le mécanisme est également documenté dans LSE15 par la description générale de la mise au point au moyen de la touche d'interruption, de la commande PAS À PAS et de CONTINUER. [LSE15 §9-4–§9-5]

## 13.2 PAUSE

**Identifiant : HIS-13.2**

L'instruction PAUSE permet d'interrompre temporairement l'exécution d'un programme. Dans l'état résultant de cette interruption, l'utilisateur peut reprendre le contrôle de la console et accéder au mode machine de bureau pour examiner les valeurs utilisées par le programme. [INRDP75 §4-3 ; Annexe II] 
Le manuel INRDP recommande explicitement l'utilisation de PAUSE pour la mise au point. Une pause peut être introduite avant une ligne où une erreur a été détectée, afin de permettre l'examen de l'état du programme avant la poursuite de l'exécution. [INRDP75 Annexe II]

À partir du mode machine de bureau, l'utilisateur peut interroger les variables et, selon les possibilités décrites dans la documentation, modifier leur valeur avant de reprendre l'exécution. [INRDP75 §Mode machine de bureau ; Annexe II] 
La commande CONTINUER permet ensuite de reprendre l'exécution du programme à l'endroit où l'instruction PAUSE a provoqué l'arrêt. [INRDP75 §4-3]

Le fonctionnement de PAUSE est donc intégré au fonctionnement courant de l'environnement conversationnel : l'arrêt ne détruit pas l'état d'exécution, l'utilisateur peut interroger cet état, le modifier dans les limites prévues par le système, puis reprendre l'exécution. [INRDP75 §4-3 ; Annexe II ; NOYELLE89 p.221] 
## 13.3 CONTINUER

**Identifiant : HIS-13.3**

La commande CONTINUER permet de relancer l'exécution d'un programme momentanément interrompu par l'instruction PAUSE ou par la touche d'interruption. [INRDP75 §4-3]

L'exécution reprend à l'endroit où elle a été arrêtée. La commande ne demande donc pas de redémarrer le programme depuis sa première ligne ni de réinitialiser les valeurs accumulées avant l'interruption. [INRDP75 §4-3]

Le recours à CONTINUER est également prévu après une utilisation du PAS À PAS. Une fois les interrogations ou modifications nécessaires effectuées, la commande permet de poursuivre l'exécution selon le déroulement du programme. [INRDP75 §4-1 ; Annexe II] 
Le fonctionnement décrit pour CONTINUER fait partie de l'outil de mise au point intégré au système. Yves Noyelle indique que l'arrêt volontaire d'un programme pouvait être suivi d'une reprise sans perte d'information dans les cas prévus. [NOYELLE89 p.221]

## 13.4 REPRENDRE

La commande REPRENDRE permet de relancer l'exécution d'un programme qui a été momentanément interrompue par l'instruction PAUSE ou par l'utilisation de la touche d'interruption. À la différence de la commande EXECUTER, elle permet d'effectuer cette reprise sans réinitialiser les valeurs des identificateurs. Les valeurs antérieures à l'interruption sont conservées. [INRDP75 §4.3]

La commande CONTINUER provoque la poursuite de l'exécution à partir de l'endroit où celle-ci avait été interrompue. La commande REPRENDRE introduit une possibilité supplémentaire : elle permet de reprendre l'exécution à partir d'une ligne différente de celle où l'interruption s'est produite, tout en conservant les affectations existantes. [INRDP75 §4.3]

Le manuel d'utilisation de 1975 décrit ainsi trois situations distinctes. Après une interruption provoquée par PAUSE, CONTINUER permet d'exécuter l'instruction située après l'instruction PAUSE. REPRENDRE permet au contraire de reprendre l'exécution à partir d'une ligne choisie par l'utilisateur. La commande EXECUTER A PARTIR DE, quant à elle, constitue une nouvelle exécution et efface les valeurs antérieures des variables ; le manuel donne explicitement un exemple dans lequel la valeur d'une variable nécessaire à la poursuite du programme est ainsi perdue. [INRDP75 §4.3]

Le format décrit dans le manuel permet donc d'indiquer une ligne de départ pour la reprise. Les paramètres N1 et N2 de REPRENDRE ont la même signification que ceux de la commande EXECUTER. La commande permet ainsi de désigner la ligne à partir de laquelle l'exécution doit reprendre et, lorsque le second paramètre est utilisé, une borne supplémentaire conformément au mécanisme décrit pour les commandes d'exécution. [INRDP75 §4.3]

Le comportement de REPRENDRE comporte cependant une limitation explicitement indiquée par le manuel : l'exécution est toujours reprise au niveau principal, même lorsque l'interruption s'est produite à l'intérieur d'une procédure. La reprise à une ligne déterminée ne constitue donc pas la continuation du contexte d'exécution interne d'une procédure interrompue. Cette caractéristique est donnée par le manuel comme une règle de fonctionnement de la commande. [INRDP75 §4.3]

La conservation des affectations et la possibilité de choisir une nouvelle ligne de départ permettent notamment d'utiliser REPRENDRE dans une situation de mise au point ou de poursuite sélective d'un programme. Le manuel présente un exemple dans lequel une exécution est interrompue par PAUSE après le calcul d'une somme, puis reprise à une ligne ultérieure pour effectuer un autre traitement, sans perdre les valeurs déjà établies. [INRDP75 §4.3]

L'exemple proposé par le manuel illustre également la différence entre CONTINUER, REPRENDRE et EXECUTER A PARTIR DE. Après une pause sur une ligne donnée, CONTINUER poursuit au point d'arrêt ; REPRENDRE A PARTIR DE 10 relance l'exécution à la ligne 10 en conservant les valeurs déjà présentes ; une nouvelle commande EXECUTER A PARTIR DE 10 aurait, au contraire, effacé les valeurs antérieures à l'interruption. [INRDP75 §4.3]

REPRENDRE appartient ainsi à l'ensemble des mécanismes qui permettent de reprendre le contrôle d'un programme arrêté sans abandonner nécessairement l'état de ses données. Dans le système décrit par le manuel, cette possibilité est associée aux fonctions de mise au point qui comprennent également la touche d'interruption, PAS À PAS, CONTINUER et le mode machine de bureau. [INRDP75 §3.2] [INRDP75 §4.1] [INRDP75 §4.3]

Le manuel précise que la commande REPRENDRE peut être utilisée conjointement avec PAS À PAS. La commande PAS À PAS fait arrêter l'exécution au début de chaque ligne et fait repasser la console dans l'état « moniteur » en affichant le numéro de la ligne atteinte. Après une telle interruption, la reprise peut être commandée par CONTINUER ou par REPRENDRE. [INRDP75 §4.1]

Dans le fonctionnement décrit, l'utilisateur peut donc interrompre l'exécution, examiner ou modifier l'état du programme depuis le mode machine de bureau, puis reprendre l'exécution. Le mode machine de bureau permet notamment d'obtenir la valeur de variables utilisées par le programme courant et, si nécessaire, d'en modifier la valeur avant la reprise. [INRDP75 §3.2] [INRDP75 §4.1]

Le manuel de 1975 décrit explicitement cette possibilité de modification des valeurs pendant une interruption. Une fois la console revenue dans l'état « moniteur », le mode machine de bureau permet d'évaluer des expressions, d'obtenir la valeur de variables du programme courant et d'affecter une nouvelle valeur à une variable. La commande REPRENDRE permet ensuite de reprendre l'exécution sans réinitialiser les valeurs des identificateurs. [INRDP75 §4.1] [INRDP75 §4.3]

La touche d'interruption constitue le mécanisme qui permet de provoquer l'arrêt alors que le programme est en cours d'exécution. Le manuel indique que son utilisation arrête l'exécution et fait passer la console de l'état « exécution » à l'état « moniteur ». L'utilisateur dispose alors des commandes de mise au point, parmi lesquelles REPRENDRE. [INRDP75 §3.2] [INRDP75 §4.3]

Le manuel de 1975 indique également que l'utilisation de la touche d'interruption peut arrêter une exécution en cours, puis que REPRENDRE permet de la relancer sans réinitialiser les variables. Le mécanisme repose donc sur la conservation de l'état des valeurs associées au programme entre l'arrêt et la reprise, sans que le manuel fournisse, dans cette description, le détail interne de la conservation de cet état. [INRDP75 §4.3]

La commande REPRENDRE est ainsi distincte d'une nouvelle commande EXECUTER. La première agit sur une exécution interrompue et conserve les affectations antérieures ; la seconde lance une exécution du programme courant à partir d'une ligne donnée et entraîne la réinitialisation des valeurs antérieures des variables. Cette distinction est explicitement mise en évidence par le manuel dans son exemple de calcul des sommes. [INRDP75 §4.3]

Le manuel précise enfin que le comportement de REPRENDRE ne doit pas être confondu avec celui de l'instruction de liaison EXECUTER lorsqu'elle est utilisée pour charger un autre module. L'instruction EXECUTER de liaison provoque le chargement d'un autre programme et l'effacement des données antérieures du programme appelant, tandis que la commande REPRENDRE agit sur le programme courant interrompu et conserve ses affectations. [INRDP75 §4.3] [INRDP75 §H1-3.1]

### Limites des sources

Les documents historiques consultés ne décrivent pas la représentation interne de l'état conservé entre une interruption et une reprise par REPRENDRE. Ils n'établissent notamment pas la forme sous laquelle cet état est enregistré ni les structures internes utilisées pour le maintenir. [INRDP75 §4.3]

Les documents ne permettent pas d'établir le comportement détaillé de REPRENDRE pour chacune des situations particulières d'interruption autres que celles explicitement décrites, notamment au-delà des cas de PAUSE et de la touche d'interruption. [INRDP75 §4.3]

Les sources ne permettent pas d'établir le traitement interne de la position d'exécution lorsque REPRENDRE est utilisé après une interruption survenue à l'intérieur d'une procédure. Elles établissent seulement que la reprise s'effectue au niveau principal, même lorsque l'interruption s'est produite dans une procédure. [INRDP75 §4.3]

Les documents ne permettent pas d'établir les détails internes de la remise en état nécessaire à la reprise après modification d'une variable en mode machine de bureau. Ils indiquent seulement que les valeurs peuvent être consultées ou modifiées pendant l'arrêt et que REPRENDRE ne réinitialise pas les affectations. [INRDP75 §4.1] [INRDP75 §4.3]


# Chapitre 14 — Contraintes matérielles

## 14.1 Mémoire centrale

Les documents historiques décrivent le système L.S.E. dans un environnement où la mémoire centrale constitue une ressource limitée et partagée entre les besoins du système et ceux des utilisateurs. Le manuel d'utilisation de 1975 présente explicitement l'organisation de la mémoire autour d'un système résident, du moniteur de temps partagé, du processeur utilisé pour la tâche active et de la zone données-utilisateur. [INRDP75 §3.1]

Le principe du temps partagé décrit dans le manuel ne consiste pas à attribuer en permanence une partie de la mémoire centrale à chaque utilisateur. La mémoire est utilisée successivement pour les différentes tâches : une seule tâche est active à un instant donné, tandis que les autres sont conservées sous forme d'images dans la zone de swap sur disque. La mémoire centrale disponible pour chaque tâche est donc réutilisée successivement par les utilisateurs. [INRDP75 §3.1]

Le schéma d'implantation mémoire présenté dans le manuel distingue une partie résidente comprenant le système et le moniteur de temps partagé, une zone données-utilisateur contenant la tâche active et une zone de swap située sur disque. Les images des tâches des autres utilisateurs sont conservées dans cette dernière zone. [INRDP75 §3.1]

Le manuel indique que la zone données-utilisateur contient, suivant l'opération effectuée, le programme courant de l'utilisateur et les données nécessaires à son exécution. La même zone peut également recevoir le programme courant lors de son chargement. Le système et les processeurs nécessaires au traitement doivent coexister avec cette zone dans la mémoire centrale disponible. [INRDP75 §3.1]

Pour les systèmes décrits dans le manuel de 1975, la taille de la zone données-utilisateur est donnée comme étant de 2 000 mots sur MITRA 15 et de 2 800 mots sur T 1600. Ces valeurs concernent la zone attribuée à la tâche utilisateur dans l'organisation mémoire décrite par le manuel. [INRDP75 §3.1]

Le manuel précise également que les mots des ordinateurs T 1600 et MITRA 15 sont constitués de 16 bits, soit deux octets. Un caractère occupe un octet et un nombre occupe deux mots. Ces indications donnent le cadre matériel dans lequel sont représentées les informations manipulées par le système L.S.E. décrit. [INRDP75 §3.1]

La limitation de la mémoire centrale apparaît directement dans les possibilités d'exécution des programmes. Dans la description consacrée au chaînage des programmes, le manuel indique que, sur MITRA 15, l'utilisateur ne dispose que de 2 000 mots en mémoire centrale et que cet espace doit contenir à la fois le programme, les données et une zone de taille variable correspondant à la « pile d'exécution ». [INRDP75 §H1-3.1]

La « pile d'exécution » décrite par le manuel contient notamment les éléments nécessaires au déroulement de l'exécution, parmi lesquels une table des identificateurs utilisés dans le programme avec leurs adresses et le numéro de la ligne en cours. Le manuel indique que cette zone occupe une quantité variable de mémoire selon les programmes. [INRDP75 §H1-3.1]

La taille du programme ne peut donc pas être considérée indépendamment de la taille de ses données et des éléments nécessaires à son exécution. Le manuel constate que les programmes approchant 1 200 à 1 500 mots peuvent fréquemment se voir refuser l'exécution sur MITRA 15 faute de place en mémoire centrale. Cette indication est explicitement donnée comme valable pour MITRA 15 et comme dépendant notamment de l'importance des données. [INRDP75 §H1-3.1]

La zone des données est décrite comme particulièrement critique parce que sa taille peut varier au cours d'une même exécution. Lorsque cette zone est saturée et que le système ne dispose plus de place pour enregistrer de nouvelles données, l'exécution est interrompue et une erreur est signalée. Le manuel indique les codes d'erreur correspondants pour les systèmes T 1600 et MITRA 15. [INRDP75 §H1-3.1]

Les tableaux et les chaînes de caractères sont explicitement mentionnés comme pouvant contribuer à l'occupation importante de la zone de données. Le manuel recommande, dans ce contexte matériel, d'éviter les programmes de grande taille qui manipulent des quantités importantes de données ou des chaînes de caractères encombrantes. [INRDP75 §H1-3.1]

La limitation de la mémoire centrale conduit ainsi, dans la documentation historique, à l'utilisation de programmes découpés en modules. Le manuel indique que l'instruction EXECUTER permet l'enchaînement de modules et présente cette possibilité comme une solution aux programmes dont la taille dépasse les capacités de chargement en mémoire centrale avec leurs données. [INRDP75 §H1-3.1]

Cette relation entre taille des programmes et taille de la mémoire est également présente dans le fonctionnement des versions décrites historiquement par Yves Noyelle. Dans le récit consacré au développement des systèmes de la famille LSx, celui-ci indique qu'un système réalisé sur une machine disposant de 8 Ko de mémoire centrale devait néanmoins réserver une place suffisante en zone utilisateur pour permettre l'exécution d'un module présentant des fonctionnalités comparables à celles d'un programme d'environ 300 lignes de FORTRAN. [NOYELLE89 p.226]

Pour tenir compte de cette contrainte, le système décrit par Noyelle utilise des techniques de compactage et de recouvrement dynamique permettant de configurer en mémoire le sous-ensemble de l'interpréteur nécessaire à l'exécution du programme courant. L'objectif indiqué est de limiter les accès au disque tout en conservant une zone utilisateur suffisamment importante. [NOYELLE89 p.226]

Dans cette même réalisation, Noyelle indique que la moitié de la mémoire, soit 4 Ko, pouvait être attribuée à chaque utilisateur, avec une technique d'échange. La description concerne un système doté de 8 Ko de mémoire centrale et d'un disque de 384 Ko ; elle est présentée dans le récit historique comme une réalisation particulière du système LSE. [NOYELLE89 p.226]

Le même récit indique que les différents éléments du système, parmi lesquels le compilateur et le décompilateur, étaient organisés selon des structures de recouvrement statiques afin de réduire les accès au disque et de limiter les chargements répétés. Ces dispositions sont présentées comme des réponses directes aux contraintes de mémoire et de temps d'accès rencontrées dans cette réalisation. [NOYELLE89 p.226]

Les systèmes décrits dans les sources historiques présentent donc des capacités mémoire différentes selon les matériels et les versions. Le manuel de 1975 donne 2 000 mots pour la zone données-utilisateur sur MITRA 15 et 2 800 mots sur T 1600, tandis que le récit historique de Noyelle décrit également des réalisations utilisant 8 Ko ou 16 Ko de mémoire centrale et des techniques spécifiques de compactage, de recouvrement et d'échange. Ces valeurs ne sont pas présentées comme les caractéristiques d'un matériel unique ou d'une version unique du système. [INRDP75 §3.1] [NOYELLE89 p.218] [NOYELLE89 p.226]

La documentation de 1975 indique par ailleurs que les deux systèmes auxquels elle se rapporte sont la Version 4 du système LSE sur T 1600 et la Version 8 du système LSE sur MITRA 15. Les différences entre ces deux matériels sont signalées lorsqu'elles existent. [INRDP75 Avant-propos]

La contrainte de mémoire centrale apparaît ainsi à plusieurs niveaux de la documentation historique : organisation du temps partagé, coexistence du système résident et de la zone utilisateur, conservation des tâches non actives sur disque, limitation de la taille des programmes et des données, et recours au découpage en modules lorsque la capacité disponible est insuffisante. [INRDP75 §3.1] [INRDP75 §H1-3.1]

## 14.2 Disques

Le disque magnétique constitue un élément permanent de la configuration matérielle décrite par le manuel d'utilisation de 1975. Le schéma général de la configuration L.S.E. fait apparaître l'unité centrale associée à un disque magnétique, ainsi qu'à des équipements de console et de télétype. [INRDP75 §2]

Le disque intervient dans le fonctionnement du temps partagé comme espace de conservation des images des tâches non actives. La zone de swap est divisée en autant de parties qu'il y a de consoles connectées au système, chaque partie étant de longueur égale à celle de la zone données-utilisateur. Les images de toutes les tâches utilisateur sont ainsi conservées sur le disque afin de pouvoir être restaurées lorsque les tâches sont de nouveau activées. [INRDP75 §3.1]

Le moniteur de temps partagé effectue les opérations de chargement et d'activation, désignées par l'expression « SWAP IN », puis recopie sur disque l'état d'une tâche lorsqu'elle cesse d'être active, opération désignée par « SWAP OUT ». Le manuel relie directement ces opérations à l'alternance des tâches dans la mémoire centrale. [INRDP75 §3.1]

L'utilisation du disque n'est donc pas limitée au stockage permanent de fichiers. Le disque assure également une fonction liée au temps partagé par l'intermédiaire de la zone de swap. Le manuel distingue, dans son schéma d'implantation, la zone de swap destinée aux images des tâches et la zone de fichiers contenant notamment les programmes et les données. [INRDP75 §3.1]

Le schéma du manuel indique que les fichiers comprennent notamment les programmes, les données temporaires et les données permanentes. La mémoire centrale contient, pour sa part, le programme courant de l'utilisateur lors de son chargement, puis le programme courant et les données au cours de l'exécution. [INRDP75 §3.1]

La capacité du disque constitue également une limite pour la conservation des fichiers. Le manuel rappelle que les disques ont une taille finie et que le responsable du centre peut être amené à supprimer périodiquement des fichiers programmes ou données afin de libérer de l'espace pour d'autres utilisateurs. [INRDP75 Annexe 2]

Le stockage sur disque n'est pas présenté comme une garantie absolue de conservation. Le manuel indique que des incidents peuvent entraîner une reconfiguration du système et, par conséquent, la suppression de tous les fichiers. Il recommande donc de sauvegarder les programmes, et éventuellement les données, sur ruban perforé. [INRDP75 Annexe 2]

La fonction du disque dans le système L.S.E. est donc multiple dans les descriptions historiques : il sert de support à la zone de swap utilisée par le temps partagé ; il accueille les fichiers programmes et données ; il fournit l'espace nécessaire au chargement de modules et à la gestion des fichiers ; enfin, sa capacité détermine une limite pratique au nombre de fichiers pouvant être conservés simultanément. [INRDP75 §3.1] [INRDP75 Annexe 2]

Le manuel de 1975 décrit également les différences de matériel entre les systèmes T 1600 et MITRA 15. Les opérations de lecture et de perforation de ruban ne sont pas identiques sur les deux systèmes, ce qui constitue un exemple de dépendance de certaines fonctions de l'environnement L.S.E. au matériel périphérique disponible. [INRDP75 Annexe 2]

Dans le récit historique de Noyelle, les systèmes successifs sont également associés à des capacités de disque différentes. Le premier système décrit, LSD, est réalisé sur un IBM 1130 disposant de 16 Ko de mémoire et d'un disque amovible de 1 Mo, associé à un Hewlett-Packard 2116A disposant également de 16 Ko de mémoire et de treize terminaux Télétypes ASR33. [NOYELLE89 p.218]

Noyelle décrit ensuite une réalisation de système sur une machine dotée de 16 Ko de mémoire centrale et d'un disque rapide de 256 Ko, ainsi qu'une autre réalisation disposant de 8 Ko de mémoire centrale et d'un disque rapide de 384 Ko. Ces deux descriptions correspondent à des réalisations matérielles distinctes du développement historique des systèmes de la famille LSE. [NOYELLE89 p.225-226]

Dans le système disposant de 8 Ko de mémoire centrale et d'un disque de 384 Ko, le disque est sollicité pour une partie des tampons associés aux terminaux ainsi que pour les mécanismes d'échange et de recouvrement décrits par Noyelle. Les choix d'organisation sont explicitement liés à la nécessité de conserver une zone mémoire utilisateur importante malgré la faible capacité de mémoire centrale. [NOYELLE89 p.226]

Le récit indique également que les structures de recouvrement statiques comprenant notamment le compilateur et le décompilateur ont été disposées afin de réduire les temps d'accès au disque et que leur chargement a été limité autant que possible. Le temps d'accès au disque constitue ainsi, dans cette réalisation historique, une contrainte directement prise en compte dans l'organisation des composants du système. [NOYELLE89 p.226]

Les sources historiques montrent donc que le disque constitue simultanément un support de stockage, un espace utilisé par le temps partagé et un moyen de compenser, dans certaines réalisations, la faiblesse de la mémoire centrale. La documentation ne réduit pas son rôle à celui d'une simple mémoire auxiliaire destinée aux fichiers permanents. [INRDP75 §3.1] [NOYELLE89 p.226]

## 14.3 Télétypes

Le télétype constitue l'un des équipements matériels directement associés à l'utilisation conversationnelle du L.S.E. Le manuel de 1975 décrit le dialogue de l'utilisateur avec le système au moyen du clavier d'une console de visualisation ou d'une télétype. [INRDP75 §3]

Le schéma général de configuration présenté dans le manuel fait apparaître une télétype ainsi que des consoles de visualisation. Le même schéma mentionne également un lecteur et un perforateur associés au matériel de télétype. [INRDP75 §2]

Le rôle du télétype dépasse donc la seule saisie et l'affichage. Dans le système décrit, il peut être associé à la lecture de programmes enregistrés sur ruban perforé et à la perforation de programmes afin d'en assurer la sauvegarde. Les opérations correspondantes sont commandées par le langage de commande du système. [INRDP75 §4.1] [INRDP75 Annexe 2]

Sur MITRA 15, la commande RUBAN permet de déclencher le lecteur de ruban de la télétype. La documentation indique que cette commande est utilisée notamment pour entrer un programme à partir d'un ruban perforé ou pour lire des données sur ruban. [INRDP75 §4.1]

Sur T 1600, la commande RUBAN n'est pas nécessaire : le manuel indique qu'il suffit de mettre le lecteur en route. Cette différence est explicitement attribuée aux caractéristiques respectives des matériels. [INRDP75 §4.1]

La commande PERFORER permet, pour un utilisateur travaillant sur la télétype, d'obtenir un ruban perforé du programme L.S.E. présent dans le programme courant. La commande entraîne la mise en marche automatique du perforateur et son arrêt automatique à la fin de l'opération, le système laissant une bande vierge en tête et en fin de ruban. [INRDP75 §4.2] [INRDP75 Annexe 2]

Le ruban perforé constitue donc un support matériel complémentaire du disque. Le manuel le présente notamment comme une sauvegarde destinée à préserver les programmes et les données dans un contexte où l'espace disque est limité ou lorsqu'une reconfiguration du système risque d'entraîner la suppression des fichiers. [INRDP75 Annexe 2]

Le fonctionnement du lecteur et du perforateur est intégré aux possibilités de dialogue du système. La documentation fournit notamment des commandes et des caractères de contrôle permettant de mettre en route ou d'arrêter ces périphériques. Les détails de ces opérations sont toutefois présentés comme dépendants du matériel utilisé. [INRDP75 Annexe VI]

Dans le récit historique de Noyelle consacré aux premiers systèmes de la famille LSx, le système LSD est décrit comme ayant été réalisé sur un Hewlett-Packard 2116A équipé de treize terminaux Télétypes ASR33. Ces terminaux étaient reliés au système par l'accès direct mémoire. [NOYELLE89 p.218]

La description historique d'une autre réalisation mentionne un système prévu pour seize terminaux. Afin de réduire la consommation de mémoire centrale, seule une petite partie des tampons associés aux terminaux était résidente, tandis que le reste était placé sur disque. L'organisation retenue devait néanmoins permettre d'assurer un débit minimal de 600 bauds pour chacun des terminaux prévus. [NOYELLE89 p.226]

Cette organisation montre que les terminaux conversationnels participent directement aux contraintes matérielles du système : leur nombre, leur débit et les tampons nécessaires à leur gestion influencent l'utilisation de la mémoire centrale et du disque. Cette relation est explicitement décrite pour la réalisation historique disposant de 8 Ko de mémoire centrale et d'un disque de 384 Ko. [NOYELLE89 p.226]

Le manuel de 1975 décrit en outre la tâche utilisateur comme pouvant être interrompue lorsqu'une demande d'entrée-sortie se produit, notamment lorsqu'un affichage doit être effectué ou lorsqu'une ligne ou une donnée est attendue de l'utilisateur. Dans ce contexte, le temps passé par l'utilisateur à taper une ligne est pris en compte dans le fonctionnement du temps partagé. [INRDP75 §3.1]

La nature conversationnelle de l'environnement est donc liée à la présence des consoles et des télétypes autant qu'au langage lui-même. Le système alterne les tâches en fonction notamment des demandes d'entrée-sortie, et le matériel de communication avec l'utilisateur constitue ainsi un élément du fonctionnement global du temps partagé. [INRDP75 §3.1]

## 14.4 Consoles

Le manuel de 1975 distingue explicitement les consoles de visualisation et les télétypes dans la présentation de la configuration matérielle. L'utilisateur peut dialoguer avec le système au moyen du clavier d'une console de visualisation ou d'une télétype. [INRDP75 §2] [INRDP75 §3]

Une console est associée à un état du système et à un utilisateur. Le manuel décrit trois états : l'état « dormant », lorsque la console n'est pas en communication avec le système ; l'état « moniteur », qui permet le dialogue avec le système au moyen de commandes ; et l'état « exécution », dans lequel un traitement est en cours. [INRDP75 §3.2]

Le passage de l'état « dormant » à l'état « moniteur » est effectué par la commande BONJOUR. Le passage inverse, de l'état « moniteur » à l'état « dormant », est effectué par la commande AU REVOIR. Ces commandes permettent au système de prendre en compte le début et la fin de l'utilisation d'une console. [INRDP75 §3.2]

L'état « exécution » comporte deux modes de fonctionnement. Le mode machine de bureau correspond à l'exécution immédiate d'une ligne ou d'un traitement soumis à la console. Le mode programmé correspond à l'exécution différée d'un programme composé de plusieurs lignes L.S.E. préalablement entrées dans le système. [INRDP75 §3.2]

La console n'est donc pas seulement un périphérique d'entrée-sortie. Elle constitue, dans la description historique du système, le point d'accès d'un utilisateur à un environnement comprenant un moniteur, des commandes, un programme courant et un état d'exécution. [INRDP75 §3.2]

La touche d'interruption joue également un rôle particulier dans le fonctionnement de la console. Elle peut être utilisée à tout moment ; lorsqu'une console se trouve dans l'état « exécution », son utilisation la fait passer dans l'état « moniteur » et provoque l'affichage du message PRET accompagné du numéro de la ligne qui était en cours d'exécution. [INRDP75 §3.2]

Lorsque la console se trouve déjà dans l'état « moniteur », la touche d'interruption la laisse dans cet état et provoque l'affichage du message PRET. Le manuel donne comme exemple l'interruption d'un listage en cours. [INRDP75 §3.2]

La même fonction est décrite dans le manuel LSE 15 de 1978. La touche d'interruption permet à l'utilisateur de reprendre le contrôle de sa console et notamment de la faire passer de l'état « exécution » à l'état « moniteur », y compris lorsqu'un programme est engagé dans une boucle sans fin ou lorsqu'une opération d'affichage est en cours. Le système affiche alors PRET et, lorsque la console était en cours d'exécution, le numéro de la ligne atteinte. [LSE15 §9-4]

Le manuel LSE 15 associe explicitement cette touche à la mise au point des programmes, en liaison avec PAS À PAS, CONTINUER et le mode machine de bureau. La fonction de la console dans l'environnement d'exécution comprend ainsi non seulement la saisie et l'affichage, mais également le contrôle interactif d'une exécution en cours. [LSE15 §9-4]

Le mode machine de bureau est utilisable lorsque la console se trouve dans l'état « moniteur ». Le manuel de 1975 indique qu'il permet d'évaluer une expression et d'en afficher le résultat, d'obtenir la valeur de variables utilisées par le programme courant et d'affecter une valeur à une variable. [INRDP75 §2 de la 2e partie]

Le manuel LSE 15 de 1978 décrit également le mode machine de bureau comme un mode réservé à l'état « moniteur ». Il permet d'effectuer des calculs simples, d'obtenir la valeur de variables utilisées par le programme courant et de modifier les valeurs de variables. [LSE15 §9-5]

La console constitue ainsi l'interface au moyen de laquelle l'utilisateur agit sur le système à différents niveaux : il peut soumettre une ligne, entrer une ligne de programme, donner une commande, exécuter un programme, interrompre une exécution, consulter des valeurs et modifier des variables. Ces possibilités sont décrites dans les manuels comme des fonctions intégrées au fonctionnement conversationnel du système. [INRDP75 §3] [INRDP75 §3.2] [LSE15 §9-4] [LSE15 §9-5]

Le manuel de 1975 indique, dans le schéma de configuration générale, la présence de huit consoles. La documentation fournie ne permet toutefois pas d'établir, à partir de ce seul schéma, si cette valeur constitue une caractéristique générale de toutes les configurations L.S.E. ou celle d'une configuration matérielle particulière. [INRDP75 §2]

Les équipements associés aux consoles diffèrent également selon les systèmes. Le manuel de 1975 décrit des versions L.S.E. fonctionnant sur T 1600 et MITRA 15 et précise que les différences entre ces deux types de matériel sont signalées lorsqu'elles existent. [INRDP75 Avant-propos]

Le récit historique de Noyelle décrit, pour sa part, des configurations différentes selon les générations de systèmes. Le premier système LSD est associé à treize terminaux Télétypes ASR33, tandis qu'une autre réalisation historique est prévue pour seize terminaux. Ces données montrent la diversité des configurations matérielles rencontrées au cours de l'histoire du système. [NOYELLE89 p.218] [NOYELLE89 p.226]

La documentation historique ne présente donc pas une configuration matérielle unique et immuable du système L.S.E. Les versions et réalisations décrites sont associées à des matériels différents, à des capacités de mémoire différentes et à des nombres différents de terminaux ou de consoles. Le fonctionnement du langage et du système conversationnel est cependant décrit autour des mêmes éléments généraux : une console, un système en temps partagé, un programme courant, un état de la console et des mécanismes de contrôle interactif. [INRDP75 Avant-propos] [INRDP75 §3.1] [INRDP75 §3.2] [NOYELLE89 p.218] [NOYELLE89 p.226]

## 14.5 Influence sur l'architecture

Les contraintes matérielles décrites par les documents historiques sont directement associées à plusieurs éléments de l'organisation du système L.S.E. La mémoire centrale limitée conduit à partager le temps plutôt que la mémoire entre les utilisateurs, tandis que les tâches non actives sont conservées sur disque sous forme d'images permettant leur réactivation ultérieure. [INRDP75 §3.1]

Le fonctionnement du moniteur de temps partagé repose ainsi sur l'alternance entre la mémoire centrale et la zone de swap sur disque. Une tâche est chargée dans la zone données-utilisateur lorsqu'elle devient active, puis son état est recopié dans la zone de swap lorsque le système lui retire l'accès au processeur. [INRDP75 §3.1]

Cette organisation est liée aux caractéristiques de l'entrée-sortie conversationnelle. Une tâche peut cesser d'être active lorsque le temps qui lui est attribué est écoulé, mais également lorsqu'une demande d'entrée-sortie survient. Le manuel cite notamment les opérations d'affichage et la lecture d'une ligne ou d'une donnée saisie par l'utilisateur. [INRDP75 §3.1]

Le système doit donc conserver l'état d'une tâche pendant les périodes où l'utilisateur n'effectue aucune opération de saisie. La zone de swap constitue, dans la documentation historique, le support de cette conservation temporaire des images des tâches. [INRDP75 §3.1]

La mémoire centrale limitée agit également sur l'organisation des programmes. Le manuel indique qu'un programme doit partager l'espace disponible avec les données et la pile d'exécution. Lorsque cette capacité est insuffisante, l'exécution peut être refusée ou interrompue. [INRDP75 §H1-3.1]

Le recours à l'instruction EXECUTER pour enchaîner plusieurs modules est présenté comme une réponse à cette contrainte de taille. Les programmes peuvent être découpés en unités distinctes qui sont chargées et exécutées successivement, plutôt que de devoir être présents simultanément avec toutes leurs données dans la zone de mémoire disponible. [INRDP75 §H1-3.1]

La contrainte matérielle de mémoire influence également l'organisation interne des programmes de service du système dans les réalisations décrites par Noyelle. Le récit historique mentionne des techniques de compactage et de recouvrement dynamique permettant de ne conserver en mémoire que le sous-ensemble de l'interpréteur nécessaire à l'exécution du programme courant. [NOYELLE89 p.226]

Le même récit décrit le recours à des structures de recouvrement statiques pour le compilateur, le décompilateur et les autres composants, ainsi qu'une organisation visant à limiter le nombre de chargements depuis le disque. La disposition des différentes branches de ces structures tient compte du délai d'accès au disque. [NOYELLE89 p.226]

Dans cette réalisation, les tampons associés aux terminaux ne sont eux-mêmes que partiellement résidents en mémoire. Le reste est placé sur disque afin de dégager de l'espace pour les utilisateurs, tout en maintenant le débit de communication prévu pour les terminaux. [NOYELLE89 p.226]

La contrainte de mémoire est donc associée, dans les sources historiques, à une organisation répartie entre plusieurs espaces et plusieurs niveaux de stockage : système résident et processeurs en mémoire centrale, zone données-utilisateur pour la tâche active, zone de swap pour les tâches inactives, fichiers sur disque, et, dans certaines réalisations, parties de tampons de terminaux également conservées sur disque. [INRDP75 §3.1] [NOYELLE89 p.226]

La capacité et les performances du disque interviennent également dans cette organisation. Le disque est utilisé pour le stockage permanent des fichiers, pour la zone de swap et, dans certaines réalisations, pour des éléments qui ne peuvent rester en permanence en mémoire centrale. Les performances d'accès au disque influencent alors le temps nécessaire à la réactivation d'une tâche ou au chargement d'un composant du système. [INRDP75 §3.1] [NOYELLE89 p.226]

Les contraintes liées aux terminaux ont également des conséquences sur l'organisation du système. Noyelle indique que, dans une réalisation prévue pour seize terminaux, la gestion des tampons associés aux terminaux devait être organisée de manière à économiser la mémoire centrale et à conserver un débit minimal de 600 bauds pour chacun des terminaux. [NOYELLE89 p.226]

Les caractéristiques matérielles des consoles influencent donc le fonctionnement du temps partagé. Les tâches sont interrompues notamment en présence de demandes d'entrée-sortie, tandis que les terminaux constituent des équipements dont les tampons peuvent être déplacés vers le disque lorsque la mémoire centrale est insuffisante. [INRDP75 §3.1] [NOYELLE89 p.226]

La mémoire centrale et le disque interviennent également dans la taille des programmes qui peuvent être exécutés. Le manuel de 1975 indique que les programmes approchant 1 200 à 1 500 mots peuvent être refusés sur MITRA 15 selon l'importance des données, tandis que des programmes plus importants doivent être découpés en modules. [INRDP75 §H1-3.1]

Le matériel influence ainsi indirectement l'organisation des programmes utilisateur. Les possibilités de segmentation, de chargement de modules et de transmission des données par fichiers sont présentées dans le manuel comme des moyens de tenir compte des limites de la mémoire disponible. [INRDP75 §H1-3.1]

Le découpage en modules présente également un lien avec la possibilité de modifier les programmes. Le manuel recommande des modules courts en faisant notamment valoir qu'ils sont généralement plus rapides à exécuter et qu'ils peuvent être plus facilement modifiés. La nécessité de segmenter les programmes est ainsi associée simultanément à la limitation de la mémoire et à la facilité de mise au point. [INRDP75 §H1-3.1]

L'organisation matérielle intervient également dans la conservation des programmes. Le disque est le support habituel des fichiers permanents, mais les capacités de stockage sont finies et des reconfigurations peuvent provoquer la suppression des fichiers. Le ruban perforé est donc utilisé comme support supplémentaire de sauvegarde et de transfert. [INRDP75 Annexe 2]

Les sources historiques décrivent enfin des différences entre les réalisations et les matériels. Le manuel de 1975 couvre la Version 4 du système L.S.E. sur T 1600 et la Version 8 sur MITRA 15 ; le récit de Noyelle mentionne d'autres configurations matérielles, notamment celles des premiers systèmes et de réalisations disposant de 8 Ko ou 16 Ko de mémoire centrale. Il ne ressort pas de ces documents qu'une seule organisation matérielle soit applicable à toutes les versions du système. [INRDP75 Avant-propos] [NOYELLE89 p.218] [NOYELLE89 p.225-226]

Les contraintes matérielles ne sont donc pas décrites comme des caractéristiques externes sans conséquence sur le système. Elles apparaissent dans la documentation historique au cœur de l'organisation du temps partagé, de la gestion de la mémoire centrale, du recours à la zone de swap, de la place donnée au disque, de l'organisation des processeurs, de la gestion des terminaux et du découpage des programmes utilisateur. [INRDP75 §3.1] [INRDP75 §H1-3.1] [NOYELLE89 p.226]

Cette influence est toutefois décrite de manière différente selon les sources. Le manuel de 1975 expose le fonctionnement général du système L.S.E. tel qu'il se présente sur T 1600 et MITRA 15, tandis que Noyelle décrit rétrospectivement plusieurs réalisations et les contraintes rencontrées lors de leur développement. Les éléments communs peuvent être présentés conjointement, mais les valeurs matérielles et les techniques particulières restent attachées aux réalisations auxquelles les sources les attribuent. [INRDP75 Avant-propos] [NOYELLE89 p.218] [NOYELLE89 p.225-226]

### Limites des sources

Les documents historiques ne permettent pas d'établir une configuration matérielle unique correspondant à l'ensemble des systèmes L.S.E. décrits dans la période considérée. Les matériels, les capacités de mémoire, les capacités de disque et le nombre de terminaux varient selon les systèmes et les réalisations documentées. [INRDP75 Avant-propos] [NOYELLE89 p.218] [NOYELLE89 p.225-226]

Les sources ne permettent pas d'établir une cartographie complète et exhaustive des configurations matérielles utilisées pour toutes les versions du système L.S.E. Seules les configurations explicitement décrites dans les documents historiques peuvent être retenues. [INRDP75 Avant-propos]

Les documents ne permettent pas d'établir le détail de l'implantation physique en mémoire de chacun des composants du système pour les versions T 1600 et MITRA 15. Le manuel présente une organisation générale de la mémoire, du système résident, des processeurs et de la zone données-utilisateur, mais ne fournit pas une description exhaustive des adresses ou des implantations internes. [INRDP75 §3.1]

Les sources ne permettent pas d'établir la structure interne détaillée de la zone de swap au-delà de sa division en parties correspondant aux consoles connectées et de la conservation des images des tâches. [INRDP75 §3.1]

Les documents ne permettent pas d'établir le détail des mécanismes internes utilisés pour effectuer les opérations de « SWAP IN » et de « SWAP OUT ». Ils décrivent leurs fonctions dans le temps partagé, sans en fournir l'implémentation détaillée. [INRDP75 §3.1]

Les sources ne permettent pas d'établir la composition complète des structures de recouvrement utilisées dans les différentes réalisations historiques. Noyelle indique l'existence de techniques de compactage, de recouvrement dynamique et de structures de recouvrement statiques, mais ne décrit pas l'ensemble de leur organisation interne. [NOYELLE89 p.226]

Les documents ne permettent pas d'établir les caractéristiques détaillées des disques au-delà des capacités et des qualificatifs explicitement mentionnés pour certaines réalisations. Les temps d'accès précis, les modes d'organisation physique des fichiers et les mécanismes internes de gestion du stockage ne sont pas décrits de manière complète dans les sources retenues. [INRDP75 §3.1] [NOYELLE89 p.218] [NOYELLE89 p.225-226]

Les sources ne permettent pas d'établir la configuration complète de toutes les consoles et de tous les télétypes utilisés par les différentes versions du système. Les documents fournissent certaines configurations et certains nombres de terminaux, mais ne présentent pas un inventaire exhaustif des matériels associés à chaque version. [INRDP75 §2] [INRDP75 Avant-propos] [NOYELLE89 p.218] [NOYELLE89 p.226]

Les documents ne permettent pas d'établir le fonctionnement matériel détaillé des interfaces entre les terminaux, les consoles, l'unité centrale et le système de temps partagé. Ils décrivent les fonctions accessibles à l'utilisateur et certains principes d'organisation des tampons et des échanges, sans fournir une description complète des interfaces matérielles. [INRDP75 §3.1] [NOYELLE89 p.218] [NOYELLE89 p.226]

Les sources ne permettent pas d'établir la relation exacte entre chacune des contraintes matérielles décrites et chacun des choix particuliers d'implantation des composants du système. Les documents exposent certains liens explicitement, notamment entre faible capacité mémoire, recouvrement, échange et utilisation du disque, mais ne documentent pas de manière exhaustive l'ensemble des décisions prises pour toutes les versions. [INRDP75 §3.1] [NOYELLE89 p.226]

Les documents historiques ne permettent enfin pas d'établir, pour toutes les versions et toutes les réalisations, les performances exactes obtenues en fonctionnement courant. Certaines performances sont indiquées pour des réalisations particulières, mais ces valeurs ne peuvent être généralisées au-delà des configurations auxquelles les sources les attribuent. [NOYELLE89 p.226]
