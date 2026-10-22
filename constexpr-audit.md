# Audit constexpr — 21 octobre 2026

## État final

Les points de code listés dans cet audit sont traités. Les sections suivantes
conservent le relevé initial et les étapes intermédiaires ; cet état final prévaut.

- trim, trimStart, trimEnd, split, replaceFirst et replaceAll sont définis constexpr
  dans string.hpp. Les traitements dépendant de la locale, des flux ou de
  from_chars restent à l'exécution en C++20.
- replaceAll progresse après le texte inséré et laisse la chaîne inchangée si le
  motif est vide. Cela évite les boucles et les occurrences sautées.
- mdprep::string est défini dans son header, avec son commentaire existant déplacé
  intact. Le noexcept de la fabrique get(E_MD_ENGINE, MdEngine&) est retiré car
  la construction peut allouer et lever.
- Les méthodes génériques de Player, son interface et ses wrappers propagent les
  exceptions. Les noexcept des lecteurs concrets, fondés sur des calculs entiers,
  sont conservés.
- TypeCount est limité aux types intégraux non signés dont le nombre de valeurs
  est représentable par size_t. Les types trop larges et les types signés sont rejetés.
- Les deux nextPowerOfTwoValue (util et BCS) lèvent overflow_error lorsque le
  résultat est irreprésentable ; leur noexcept est retiré. Les valeurs représentables,
  notamment l'entrée zéro donnant un, gardent leur résultat.
- Aucun consteval requis : les fonctions restent utiles à l'exécution.

Validation terminée avec MSVC en C++20, compilation isolée à partir des inclusions
et définitions déjà configurées pour vtx_util_test : huit unités compilées, dont
les six fichiers de tests BitSet, RangeList, Range, String, Math et Players.
Les assertions statiques passent. Exécution ciblée : 68 cas, 919 assertions réussies.
Vérification isolée supplémentaire réussie pour le header mdprep et le helper BCS,
y compris l'exception de dépassement. Pas de compilation complète Qt/CUDA, de
rebuild global ni de résolution Conan. Journaux dans
dev/build/Debug/constexpr-audit-check/.

clang-format appliqué depuis la racine ; git diff --check réussi sur les fichiers
du patch. La correction de WHITESPACES déjà présente à la reprise a été conservée.

Les remarques documentaires restent signalées conformément à AGENTS.md. Dans
les derniers headers touchés : replaceFirst manque de Doxygen ; les constantes
de constants.hpp et les helpers mathématiques concernés n'ont pas de blocs
individuels ; Player et les classes Players ont plusieurs méthodes, constructeurs
et membres internes non documentés ; md_engine_factory.hpp et les fonctions BCS
n'ont pas de Doxygen. Le commentaire de strToNumber reste trop restrictif
(entiers non signés alors que les entiers signés sont acceptés). Aucune correction
documentaire supplémentaire n'a été appliquée.

## Périmètre et limites

Inventaire textuel du dépôt : 1 000 occurrences dans 138 fichiers avant modification,
y compris les `if constexpr` et quelques blocs commentés. Analyse statique des
fonctions explicitement constexpr et des principales possibilités dans lib/util.
Ce relevé ne constitue pas une validation de toutes les instanciations de templates
ni de toutes les configurations de compilation. Aucune compilation effectuée.
Base de compatibilité : C++20 et GLM 1.0.3.

## Corrections et ajouts

- `collection.hpp` : retrait de constexpr de `Collection::hash<T>()`, qui appelle
  `Util::hash<T>()`, puis `type_info::hash_code()`, exclusivement à l'exécution.
- `chrono.hpp` : `Chrono::elapsedTime(start, end)` devient constexpr. Il faut fournir
  les deux dates pour une évaluation constante ; la valeur par défaut appelle now().
- `math.hpp` : les deux fonctions de densité d'échantillonnage deviennent constexpr.
- `math/range.hpp` : `Range::toStdVector()` devient constexpr.
- `color/rgba.hpp` : constructeur depuis vector et conversion vers vector constexpr.
- `math/grid.hpp` : constructeur et calculs simples de DenseGrid constexpr ; la
  conversion depuis une position réelle reste inchangée car elle utilise floor.
- `math/transform.hpp` : accesseurs, reset, setters de position/échelle et scale constexpr.
- `math/aabb.hpp` : construction par défaut, validité, bornes, translation, diagonale
  et centroïde constexpr.

Les allocations de vector doivent être libérées pendant la même évaluation
constante. Cela permet des calculs transitoires, pas un vector constexpr persistant
contenant une allocation dynamique en C++20.

## Autres constats

- Constantes scalaires, tableaux, couleurs, traits, conversions d'enums, mappings
  OpenGL, calculs de layout et fonctions Range : pas de retrait identifié.
- Les lambdas sans capture stockées dans des variables constexpr de presets sont
  légitimes : c'est l'objet lambda qui est constant, pas nécessairement son résultat.
- Les assertions et branches d'erreur ne rendent pas une fonction constexpr
  illégitime lorsque le chemin évalué à la compilation ne les déclenche pas.
- `tool/mdprep/.../md_engine_factory.hpp` déclare `string(E_MD_ENGINE)` constexpr,
  mais son corps est uniquement dans le .cpp. Son usage local dans la table est
  légitime ; l'interface n'est pas utilisable comme fonction constexpr depuis un
  autre fichier. Définition à rendre visible dans le header si cette API est exposée.
  Aucun appel extérieur correspondant trouvé dans les sources UI examinées.
- Les fonctions bcs/core/math.hpp ont bien leur définition dans math.inl inclus.
- `TypeCount<T>` décale de numeric_limits<T>::digits : valide pour l'usage actuel
  avec ColorIndex, mais pas générique pour les types aussi larges que size_t.
- `nextPowerOfTwoValue` (util et bcs) peut boucler après débordement pour une entrée
  supérieure à la plus grande puissance représentable. Problème de domaine des
  entrées, indépendant de la légitimité de constexpr ; non modifié.

## Première extension appliquée : BitSet

Les constructeurs, accès à un bit, requêtes globales, remplissage, inversion,
comparaisons, accès au stockage et itérateurs de BitSet sont désormais constexpr.
Les opérations sur Range/RangeList et les opérations binaires entre bitsets restent
inchangées pour garder cette étape ciblée. Une assertion statique dans biteset.cpp
couvre une taille de 130 bits, le masquage du dernier mot et l'itération entre mots.
Elle n'a pas été compilée ; elle sera évaluée au prochain build de cette cible.

Dans ce périmètre, les noexcept existants sont justifiés par les opérations
entières et les accès sans allocation. Les opérations allouantes restent sans
noexcept. Aucun consteval nécessaire : les usages à l'exécution restent utiles.
Le commentaire de Word reste inexact (signalé ci-dessous). Doxygen manque pour
les alias, constructeur, opérateurs, helper et membres de Iterator, ainsi que
les membres privés _size et _data. Aucun commentaire de code modifié.

## Possibilités supplémentaires non appliquées

- BitSet et RangeList : conversion constexpr terminée pour les opérations
  compatibles C++20. Les représentations textuelles restent à l'exécution.
- Les traitements de chaînes fondés uniquement sur string/vector peuvent être
  candidats, mais nécessitent de rendre visibles les corps aujourd'hui en .cpp.
  Les conversions utilisant les flux, la locale ou from_chars ne sont pas à
  convertir systématiquement en C++20.
- Hachage standard/RTTI, mutex, horloges, hasard, I/O, réseau et journalisation
  restent à l'exécution. Les appels GLM de normalisation, trigonométrie et matrices
  doivent être évalués individuellement ; aucun ajout automatique.

## Commentaires et Doxygen

Aucun commentaire de code modifié intentionnellement.

Commentaires incohérents relevés :
- Range() est décrit comme invalide, alors que isValid() accepte first == last.
- BitSet::Word est décrit comme une taille en octets, mais représente un mot 64 bits.
- fmt/rgba.hpp indique un héritage de glm::vec4f ; Rgba utilise la composition.
- String::strToNumber est décrit comme limité aux entiers non signés, mais accepte
  les types intégraux signés également.

Doxygen manquante dans les headers modifiés :
- collection.hpp : méthodes et alias iterator, y compris hash, et helpers privés.
- color/rgba.hpp : constructeurs, méthodes, opérateurs et stockage privé ; seul le
  type dispose d'une description générale.
- math.hpp : fonctions utilitaires sans blocs Doxygen ; plusieurs ont seulement
  un commentaire simple, notamment Morton et les densités d'échantillonnage.
- chrono.hpp : alias de Chrono, constructeur/destructeur et membres de ScopedChrono.
- math/range.hpp : Count, getLast, wrappers expand/shrink après expandFirst,
  toString, membres de l'itérateur et begin/end n'ont pas de bloc individuel.
- math/grid.hpp : surcharge denseIndex(x,y,z), alias et plusieurs méthodes de Grid
  n'ont pas de bloc individuel.
- math/transform.hpp : les groupes d'accesseurs, rotations et setters ont des
  commentaires collectifs, mais plusieurs surcharges n'ont pas de bloc individuel.
- math/aabb.hpp : plusieurs constructeurs, getMax, diameter/radius/centroid,
  largestAxis/area/offset et _max n'ont pas de bloc individuel.

## noexcept et validation

Les noexcept des huit headers modifiés ont été examinés : pas de nouveau cas
injustifié identifié pour les types concrets concernés. Aucun noexcept ajouté.
Les conversions vector restent potentiellement levantes.

Hors patch : les wrappers génériques de players.hpp imposent noexcept aux appels
d'un T non contraint ; une exception de ce T terminerait le processus. Le get()
de md_engine_factory mérite aussi vérification car il construit Gromacs/MdEngine
sous noexcept. Ces points sont signalés, sans modifier les contrats.

clang-format appliqué depuis la racine aux huit headers ; git diff --check réussi.
Pas de build, pas d'exécution des tests, pas de résolution Conan.

## Extension terminée : BitSet, RangeList et Range

Cette étape complète la première extension de BitSet : opérations binaires,
surcharges Range/RangeList, callback et conversion toRangeList sont constexpr.
RangeList devient constexpr pour ses constructeurs, mutations, opérations
ensemblistes, requêtes, conversions vector et itérateurs. Range ne nécessite pas
d'ajout : ses calculs sont déjà constexpr. toString reste à l'exécution dans
Range et RangeList, car il dépend de std::to_string en C++20.

Aucun consteval nécessaire. Le noexcept conditionnel de forEachSetBit teste
désormais l'appel avec const size_t &, comme dans le corps, plutôt qu'un temporaire.
Les autres noexcept examinés dans ce périmètre sont conservés ; les opérations
allouantes restent potentiellement levantes.

Assertions statiques ajoutées aux tests existants : opérations ensemblistes,
séparation/fusion de plages, itération et conversions croisées. Non compilées.
Validation limitée à clang-format depuis la racine et git diff --check.

Doxygen manquante dans RangeList : RangeType, surcharges de constructeurs sans
bloc individuel, addValues(span), sizeRange, size, surcharges rangeBegin/rangeEnd,
éléments internes de iterator et begin/end. Les lacunes précédemment relevées
dans Range et BitSet demeurent. Aucun commentaire de code modifié.
Commentaire supplémentaire inexact : currentSize() utilise size() plutôt que
capacity(), donc ne mesure pas toute la mémoire réservée par le vector.
