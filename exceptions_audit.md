# Étude des exceptions — VTX (app, lib, tool)

Branche `dev-sge` @ `12501064a`. Étude en lecture seule : rien n'a été modifié ni compilé.
Les points marqués ✔ ont été revérifiés directement dans le code.

> **Bilan après correctifs : voir §10** (état à `b1a187f68`, relecture finale du 2026-11-06).

Volume : ~200 `throw`, ~95 `catch` hors tests, ~740 `noexcept`.

| Module | throw | catch | noexcept |
|---|---|---|---|
| lib/renderer | 78 | 19 | 179 |
| lib/util | 25 | 15 | 270 |
| lib/app | 57 | 42 | 48 |
| lib/io | 12 | 0 | 45 |
| lib/core | 0 | 0 | 3 |
| lib/python_binding | 14 | 28 (dont tests) | 17 |
| lib/ui | 5 | 19 | 10 |
| app/vtx, app/bench | 6 | 7 | 17 |
| tool/mdprep | 3 | 5 | 148 |
| tool/example, measurement, structalign, topology_editor | 0 | 0 | 0 |

---

## 0. Synthèse

- **Le hot path du renderer est bien sans exception.** La chaîne `Renderer::render` → `_render` → `ContextWrapper::execute` → `Executor::OpenGL::execute` ne contient rien qui puisse lever, et aucun `noexcept` n'est dangereux dans lib/renderer. Les exceptions possibles par frame viennent de `_flushDirty()` (exécuté à chaque frame pendant la lecture d'une trajectoire) et du pass externe SES. Toutes sont capturées.
- **L'usage de `noexcept` est globalement sain.** Il y a un vrai cas A (mdprep `_openSettings*`). Les autres cas sont des moves qui devraient être `noexcept` conditionnels, ou des destructeurs implicitement `noexcept` qui appellent du code pouvant lever.
- **Les vrais problèmes concernent la récupération après exception, pas `noexcept` :**
  - le renderer détruit le backend GL à la première erreur ;
  - le pipeline saute le rendu ;
  - l'ActionManager avale certaines erreurs et en rejoue d'autres ;
  - le thread Python peut bloquer le démarrage ou mourir silencieusement ;
  - deux callbacks C n'ont aucune protection.
- **Checkpoints existants :** `main`, `Application::notify`, `VTXApp::update`, `ActionManager::execute`, `BaseThread`, `ThreadManager::update`, le thread Python et `Renderer::render`. La structure est déjà là, mais plusieurs sont incomplets (voir §1 et §6).

---

## 1. Carte actuelle des points de capture

```
main()                                   app/vtx/src/main.cpp:163-172      std + ...  → stderr, EXIT_FAILURE
└─ Application::start → exec()
   ├─ Application::notify  (filet Qt)    lib/ui/qt/src/ui/qt/application.cpp:128-144  std + ... → log, return true
   │   ├─ _postQtStartup (singleShot)    application.cpp:146-228           std + ... → quit()   ⚠ code de sortie 0
   │   │   └─ fallback setOpenGL         application.cpp:201               std → setDefault + ApplicationError
   │   ├─ QTimer → VTXApp::update noexcept   lib/app/include/app/vtx_app.hpp:40-52   std + ...  ← checkpoint frame
   │   │   └─ Pipeline::process : THREAD → HUB → PASS → ACTION → Update → PostUpdate → RENDER → PostRender
   │   │       ├─ ThreadManager::update  lib/util/src/util/thread/thread_manager.cpp:39-50, 67-78  std + ...
   │   │       ├─ ActionManager::update → QueuedAction → ActionManager::execute
   │   │       │                         lib/app/include/app/action/action_manager.hpp:107-118   ⚠ std seulement
   │   │       └─ Renderer::render noexcept   lib/renderer/src/renderer/renderer.cpp:89-133
   │   │           ├─ _flushDirty        renderer.cpp:98-118               std + ... → setDefault() ⚠
   │   │           ├─ loadEnvironment    renderer.cpp:573-580              std (local, bonne récupération)
   │   │           └─ SES external pass  renderer.cpp:618-625 + executor/opengl.hpp:285-292 (catch ...)
   │   └─ slots Qt → ACTION().execute   → action_manager.hpp:107-118
   ├─ aboutToQuit lambda                 application.cpp:50-70             std + ...
   └─ ~Application                       application.cpp:82-109            ⚠ SETTINGS().save : std seulement

Threads
├─ BaseThread::_start (actions threadables, loaders)   lib/util/src/util/thread/base_thread.cpp:30-45   std + ...
├─ Thread Python runPythonThread         lib/app/src/app/python_binding/interpretor.cpp:88-104   std + ... ⚠ fin de boucle
│   ├─ _actuallyRunCommand               interpretor.cpp:158-181           ⚠ CommandException& / ScriptException& seulement
│   └─ _actuallyRunScript                interpretor.cpp:195-212           ⚠ ScriptException& seulement
└─ worker spdlog                         catch interne spdlog (par sink)

Frontières C / bibliothèques
├─ GL debug callback                     lib/renderer/include/private/renderer/context/backend/gl/debug.hpp:19/73   noexcept + catch(...) ✔ OK
├─ Logger::_log                          lib/util/include/util/logger.hpp:118-130   catch(...) ✔ OK (logging no-throw)
├─ curl ProgressCallback                 lib/util/src/util/network.cpp:22-34        ⚠ AUCUNE protection ✔
├─ Velopack OnBeforeUninstall            lib/app/src/app/session.cpp:110-111        ⚠ AUCUNE protection (filesystem_error)
└─ pybind11 (fonctions bindées)          traduction automatique en exception Python  OK
```

Absents du dépôt : `std::set_terminate`, `SetUnhandledExceptionFilter`, `qInstallMessageHandler`.

---

## 2. Problèmes réels, par priorité

### P0 — blocage, crash ou perte définitive du rendu

1. **Le démarrage bloque si l'init Python échoue.** ✔
   - Si `_interpretor.emplace()` lève dans `runPythonThread` (`lib/app/src/app/python_binding/interpretor.cpp:91`), le catch se contente de logger. Causes possibles : Python home invalide, script d'init absent (`lib/python_binding/src/python_binding/interpretor.cpp:98, :126`).
   - La promesse de `getRuntimePythonVersion()` (`:244-251`) reste dans la file sans jamais être remplie, donc `future.get()` bloque `finishStartup` indéfiniment.
   - Ligne `:250` : `success` n'est pas testé, et `substr(1, size-2)` lève `out_of_range` sur un résultat vide.

2. **Le renderer détruit le backend GL à la première erreur de `_flushDirty`.** ✔
   - `renderer.cpp:102/111` appelle `setDefault()`, qui appelle `ContextWrapper::setNull()`. Le `Backend::OpenGL` est détruit avec son contexte.
   - Le rendu reste mort jusqu'au redémarrage, car `setOpenGL` n'est appelé qu'au démarrage.
   - Déclencheurs réalistes :
     - texture matériau illisible (`renderer.cpp:586`, aucun catch local, contrairement à l'environnement) ;
     - erreur CUDA transitoire dans `releaseBufferChunk` → `unregisterBuffer`, appelé à chaque frame de trajectoire ;
     - `geometry/ses.cpp:523-528`.

3. **`refreshGraph` peut produire un use-after-free.** ✔ (`lib/renderer/src/renderer/builder/render_graph_build.cpp:585-587`)
   - `p_currentConfig = config` est assigné avant le build, donc un build qui a échoué n'est jamais retenté.
   - `p_graph.set()` détruit les passes (`PassList = vector<unique_ptr<Pass>>`) avant `p_graph.build()`. Si `build()` lève, `p_queue` (`RenderQueue = vector<const Pass*>`) pointe sur des passes détruites. Le prochain `rebuildCommandBuffer` sur un backend réel les déréférence.

4. **mdprep : des `noexcept` qui peuvent réellement lever (catégorie A).** ✔
   - Fonctions : `tool/mdprep/src/ui/form_basic/event_manager.cpp:162, 174, 186, 198, 224`, `_openSettings{Minimization,Nvt,Npt,Prod,Manager}() noexcept`.
   - Elles construisent un `SettingsDialog`, qui passe par `makeQObject`. Celui-ci lève `std::logic_error` si le parent est nul (`lib/ui/qt/include/ui/qt/helper.hpp:67`).
   - Elles appellent aussi un `std::function` qui peut être vide, et allouent de nombreux widgets.
   - Comme elles sont appelées depuis des slots `clicked`, le résultat est `std::terminate` au lieu d'une récupération par `notify`.

### P1 — états incohérents ou erreurs silencieuses

5. **Pipeline : une exception saute tout le reste de la frame.** ✔ (`lib/app/src/app/pipeline.cpp:25-45`)
   - Une exception dans THREAD, HUB, PASS, ACTION ou Update annule toutes les étapes suivantes, y compris le rendu. Une erreur persistante fige l'application et produit une ligne d'erreur par frame.
   - `SystemUpdater::update` (`lib/app/src/app/pass/system_updater.cpp:87-158`) et `MeshUpdater::update` ne vident leurs listes qu'à la fin. Si l'item k lève, les items 0..k-1 sont ré-ajoutés au renderer à la frame suivante. ✔
   - `EventHub::update` (`lib/util/include/util/event_hub.hpp:99-108`) : si un listener lève, le reste du dispatcher local `pending` est perdu. ✔
   - `TrajectoryUpdater` : une entité en échec bloque toutes les autres et réessaie à chaque frame.
   - `NetworkManager::_onUpdate` : un callback qui lève fait perdre les téléchargements terminés restants.

6. **ActionManager.** ✔ (`action_manager.hpp`, `action_manager.cpp`)
   - Seul `const std::exception &` est capturé. `A a;` (`:68`, `:81`) et `THREAD().createThread` (`:86`) sont hors du try : une exception de construction remonte au slot Qt ou au démarrage.
   - **File d'attente** (`action_manager.cpp:51-56`) :
     - Si `execute()` lève une exception non standard, `pop()` est sauté. L'action est rejouée à la frame suivante avec des arguments déjà déplacés (`std::apply(..., std::move(_args))`) et bloque le reste de la file.
     - Le mutex reste tenu pendant l'exécution des actions, ce qui bloque les `subscribe` des workers.
     - `_skipTime` n'est jamais remis à zéro : le commentaire « not every frame » est faux.
   - **Échec masqué côté Python :** quand une QueuedAction échoue avec une exception standard, elle est loggée puis marquée `executed`. Python croit que l'appel a réussi. Exemples : `py representation.cpp:27, 36`, `py scene.cpp:21`.
   - **Échec non standard :** le waiter n'est jamais notifié. Le thread Python reste bloqué jusqu'au `shutdown()`, et le latch est décrémenté deux fois (`lib/app/src/app/extractor/system.cpp:107` puis `:246`), ce qui est un comportement indéfini.
   - **Action threadable en échec :**
     - `BaseThread` logge l'erreur sans le nom de l'action.
     - `ThreadTerminated(manuallyStopped=false)` est indiscernable d'un succès, et l'UI n'affiche rien.
     - L'entité reste à moitié construite (`extractor/system.cpp:273`, UID enregistrés, pas de rollback).

7. **La boucle Python meurt définitivement sur la plupart des erreurs.**
   - `_actuallyRunCommand` et `_actuallyRunScript` ne capturent que `CommandException&` et `ScriptException&` (et en référence non-const).
   - Deux exceptions courantes sortent de la boucle et la terminent : `pybind11::cast_error` (`lib/python_binding/src/python_binding/interpretor.cpp:180`) et `error_already_set` levée depuis `Interpretor::add` (`lib/python_binding/src/python_binding/binder.cpp:9, 15`).
   - Conséquences :
     - les commandes en attente ne sont jamais exécutées ;
     - les promesses sont soit jamais tenues, soit cassées (`broken_promise`) ;
     - `_interpretor` est ensuite détruit sur le main thread dans `~Impl`, c'est-à-dire `Py_Finalize` et des DECREF sans le GIL, sur le mauvais thread.
   - `interpretor.cpp:183` : après un `error_already_set` en mode eval, la ligne est relancée en mode exec, donc ses effets de bord s'exécutent deux fois.

8. **mdprep : un état reste bloqué si `SystemTester` lève.**
   - `createRootDir()` (`tool/mdprep/src/backends/gromacs/gromacs.cpp:274`) et `fs::remove_all` (`:312`) sont hors du try.
   - Si l'un d'eux lève, ni `CheckReport` ni `waiter.count_down()` n'ont lieu : `checkInProgress` reste vrai et `wait()` bloque.
   - Même problème dans `StartPreparation` (`tool/mdprep/src/tool/mdprep/actions/jobs.cpp:36-104`). Correctif : un scope guard.

9. **Deux frontières C sans protection.**
   - La lambda `ProgressCallback` de curl/cpr (`lib/util/src/util/network.cpp:22-34`) appelle `p_progress` sans try. ✔
   - Velopack `OnBeforeUninstall` (`lib/app/src/app/session.cpp:110-111`) peut lever `filesystem_error`.
   - Dans les deux cas, une exception qui traverse des frames C est un comportement indéfini ou un `terminate`.

### P2 — robustesse et diagnostic

10. **Le processus masque les échecs de démarrage.**
    - Code de sortie 0 : `_postQtStartup` échoue → `quit()` → `exec()` renvoie 0, et `main` renvoie `EXIT_SUCCESS`. ✔
    - Sous Windows (`/SUBSYSTEM:WINDOWS`), les erreurs levées avant l'attache de stderr (`main.cpp:77-88`) sont perdues.
    - Il n'y a pas de `set_terminate` pour logger, flusher et afficher une boîte de dialogue.

11. **Des destructeurs implicitement `noexcept` appellent du code qui peut lever.**
    - `~ThreadManager` : lock de mutex, `stop()` virtuel, `join`.
    - `~BaseThread` : un `join` sur soi-même lève `resource_deadlock_would_occur` (cas limite).
    - `~VTXApp` : `ACTION().shutdown()`, `removeCtx<…>` et `LOGGER::stop()` sont hors try.
    - `~Application` : `SETTINGS().save()` n'a pas de `catch(...)`.
    - `Interpretor::Impl` : busy-wait puis `stop()` et `wait()`.
    - Conséquence : les try/catch de `lib/app/src/app/vtx_app.cpp:73-87` et `:94-108` sont du **code mort**. Le destructeur appelé dans `removeCtx` est `noexcept`, donc le programme passe par `terminate` avant d'atteindre le handler.

12. **Exception-safety interne au renderer.**
    - Le constructeur de `Program` fuit les shaders et l'id du programme s'il lève (`gl/program.hpp:56-69, 140-141`).
    - `Slot::construct` détruit avant de construire (`resource_handler.hpp:46-47`).
    - Les handlers de `SES::compute` appellent du code qui peut lever (`ses.cpp:984-1001`).
    - La restauration dans le catch de `snapshot` peut masquer l'erreur d'origine (`renderer.cpp:216`). Un scope guard serait préférable.

13. **Divers.**
    - `Logger::init` ne capture que `spdlog_ex` : un `filesystem_error` (`logger.cpp:75`) remonte sans être traduit.
    - Dans `Util::Callback` (`callback.hpp:77-80`), un abonné qui lève coupe tous les abonnés suivants.

---

## 3. `noexcept` à retirer ou à rendre conditionnels

| Lieu | Cat. | Action proposée |
|---|---|---|
| `tool/mdprep/src/ui/form_basic/event_manager.cpp:162,174,186,198,224` (+ `.hpp:84-88`) `_openSettings*` | **A** | retirer |
| `tool/mdprep/include/tool/mdprep/ui/md_engine_field_placer.hpp:29,38,75-76,88,90` `activate/deactivate` | B | retirer : délègue à `GromacsFieldPlacer::activate` (`gromacs_field_placer.cpp:60,79`), qui n'est pas `noexcept` (`QFormLayout::addRow`). Les `static_assert` (`:55-63`) prétendent vérifier `noexcept` mais ne le font pas ; il faudrait `{ t.activate() } noexcept;` |
| `tool/mdprep/src/ui/form_basic/event_manager.cpp:23,36,61-87,125-160` (move, `set*Settings`, `_connect*`) | B | `QObject::connect` alloue : retirer `noexcept`, ou ajouter `catch(...)` comme le fait déjà `form_advanced/event_manager.cpp:47-54, 96-103` |
| `lib/util/include/util/datalocker.hpp:50, 54` move ctor / move assign | B/C ✔ | `noexcept(std::is_nothrow_move_constructible_v<DataType>)` / `..._assignable_v`. Utilisé sur `std::queue` (deque, dont le move alloue sous MSVC et libstdc++) |
| `lib/app/include/app/pass/pass_manager.hpp:46-47` `noexcept = default` | B | retirer le `noexcept` explicite et laisser le compilateur le déduire (le move d'`unordered_map` alloue sous MSVC) |
| `lib/app/src/app/python_binding/interpretor.cpp:260` `Del` → `~Impl` | B | garder `noexcept` mais protéger `stop()`/`wait()` par `catch(...)` |
| `lib/io/src/io/writer/chemfiles.cpp:591` `System::fetch` | A théorique | `.at()` gardé par `contains()` : remplacer par `find` |
| `tool/mdprep/src/ui/form_basic/settings_dialog.cpp:34` `~SettingsDialog() noexcept` | C | redondant (déjà implicite) ; `close()` émet des signaux |
| `lib/app/vendor/velopack/include/Velopack.hpp:843,853,865` | A (vendor) | à signaler seulement, ne pas modifier |

Renderer : aucun à retirer. Deux durcissements sont proposés :
- `static_assert(noexcept(p_executor.execute(...)))` dans `_executePayload`, `_executeCommand` et `_executeRange` (`context_wrapper.cpp:80-154`), pour qu'un futur executor ne puisse pas déclencher `terminate` en silence ;
- `_executeSESExternalPass` en `noexcept` avec `catch(...)`, et un pointeur typé `void(*)(uintptr_t) noexcept` au lieu de `uintptr_t`.

Usages vérifiés corrects : les `noexcept` conditionnels de `hashing.hpp:32` et `math/bitset.hpp:709` ; `VTXApp::update` et `Renderer::render` (function-try-block avec `catch(...)`) ; les helpers de log `VTX_*`.

---

## 4. Candidats `noexcept` sûrs

Par ordre de valeur : moves (impact sur `std::vector`) > hot path > accesseurs.

### lib/renderer
- **Moves :**
  - `Sesdf` (`bcs/sesdf/sesdf.hpp:29-30`, `sesdf.cu:192,235`) et `Ssesdf` (`bcs/ssesdf/ssesdf.hpp:26-27`, `ssesdf.cu:67,95`). Vérifier d'abord les membres d'`AccelerationGrid`.
  - `CudaConstructionDeleter::operator()` (`ses_cuda.hpp:23`).
- **Hot path :**
  - `executor/opengl.hpp:23` (constructeur).
  - `command_buffer.hpp:271, 277, 283, 373, 386, 391`.
  - `resource_handler.hpp:147, 149`, itérateurs et `Entries` `220-381`, `_validateStorage` `391`, `contains/handle/descriptor/get` `528, 576, 587, 598, 600`.
  - `context_wrapper.hpp:184` ; `render_graph.hpp:20, 22`.
- **API publique :** `renderer.hpp:42, 44, 46, 143, 148`. **Pas** la `:153` (`setVSync` peut lever).
- **Backend :**
  - `opengl.cpp:112` `_toGLStorageFlags` et `:197` `_toSettingFlags` (leurs fonctions sœurs sont déjà `noexcept`).
  - `setRenderTarget` (`:1633`), `interopAvailability` (`:1541`).
  - `OpenGLCudaInterop::availability/clear` (`interop.cpp:94, 96`).
- **Wrappers GL :**
  - `buffer.hpp:17, 94, 115` ; `vertex_array.hpp:13` ; `program.hpp:120` ; `chrono.hpp:11, 23, 29, 31`.
  - `context.hpp` WGL `162, 173, 185, 198, 200, 220` ; macOS `334-346` ; EGL `567, 570, 584, 594, 602`.
- **Géométrie :** `grid.hpp:25, 37` ; `base_geometry.hpp:53, 192, 194` ; `ses.hpp:40, 58, 124` ; `binary_buffer.hpp:308, 335, 369-377` ; `graph_builder.cpp:251, 263, 269`.

### lib/util
- **Thread :** `base_thread.hpp:59, 64, 69, 74` (`isFinished`, `isManuallyStopped`, `getId`, `getProgress`) ; `base_thread.cpp:69` `setProgress`.
- **Enum :** `enum.hpp:41, 66, 73` ; `logger.hpp:136` `_toSpdlogHint`.
- **Math :**
  - `math.hpp:81, 88, 104, 116, 134, 166, 176, 194, 264, 272, 275, 283` ; `math.cpp:6`.
  - **Exclus** : `randomFloat` (`random_device` peut lever), `randomVec3f`, `toStdVector`, `randomUniVectors`.
- **Rgba :** tout `color/rgba.hpp:19-220` et `rgba.cpp:11-156`, **sauf** `toStdVector`, `toHexaString*`, `setSingleChannelHexaInStream`, `operator<<`, `random`, `randomPastel`.
- **AABB :** tout `math/aabb.hpp:21-104` et `aabb.cpp:7-20, 46-95, 145`, **sauf** `getSummits` et `transformed`. Les constructeurs, `invalidate`, `largestAxis` et `area` pourraient aussi être `constexpr`.
- **Grid :** `DenseGrid` en entier (`math/grid.hpp:26-91`) ; les accesseurs de `Grid<T>` (`134-234`, pas le constructeur ni `add`).
- **RangeList** (`range_list.hpp`, déjà `constexpr`) : `160-223, 228, 243, 248, 253, 258, 271, 276, 281, 302, 307, 465, 467, 480, 527-533`.
- **Autres :**
  - `BitSet::Iterator` (constructeur, `bitset.hpp:795`) ; constructeurs de `Players` (`players.cpp:7, 31, 56, 81, 113, 185`).
  - `collection.hpp:25, 28, 150, 157, 159, 162, 184` ; `datalocker.hpp:110-116`.
  - `monitoring/frame_info.hpp:39, 44`, `stats.hpp:31`, `stats.cpp:20` ; `chrono.hpp:87`.
  - `CHRONO_CPU` (`chrono.hpp:73`) pourrait avoir un `noexcept` conditionnel : `noexcept(std::is_nothrow_invocable_v<F, Args...>)`.

### lib/core, lib/app, lib/io
- **core :**
  - Accesseurs de `Topology` (`core/struct/topology.hpp:130-208, 224-255`) : tous les `get*Count/Range/Name/Index/Symbol`, `getBondFirstAtom/SecondAtom`, `findFirstAtomByName`.
  - **Exclu** : `getCategoryAtomRangeList`, qui alloue.
- **app :**
  - `IPass::setPaused/setDeleteNextFrame` (`pass_manager.hpp:28, 30`) ; `InputManager::rotate/rotateAlt/pan/zoom` (`input/input_manager.hpp:63-69`).
  - `UIDManager::getPickingPool`, `VTXApp::getTools`, `ArgumentParser::needHelp`, `Session::hasManager`.
  - `QueuedAction::getWaiter` et le constructeur privé de `Waiter`.
- **io :**
  - Constructeurs des handles writer (`writer/chemfiles.cpp:367, 390, 443, 475, 502`).
  - `isAtomOfResidue` (`writer/system.cpp:142`) ; `SystemIndexManager::getAtomIdx` (remplacer `.value()` par `*`), `nextSystem`, `multiSystem` ; `Reader::_toVec3f` (`reader.cpp:781`).

### lib/ui, lib/python_binding, app/bench, tool/mdprep
- **ui :**
  - `Application::notify` : tout y est capturé et les handlers sont `noexcept`. La lambda `aboutToQuit` peut l'être aussi.
  - `widget/renderer.cpp:109` `getNativeDisplay`, `:125` `getNativePlatform`.
- **python_binding :** `module.hpp:81` `getModulePath` ; `interpretor.cpp:138, 146, 251, 253, 260` (getters de module).
- **bench :** `user_interface.hpp:23, 27` ; `input_manager.hpp:41, 43` ; `scene.hpp:28` ; `camera_controller.hpp:53`.
- **mdprep :** `backends/gromacs/gromacs.cpp:331` `SystemTester::Del::operator()` (ses deleters frères sont déjà `noexcept`).

---

## 5. Les `catch` : à garder, corriger, supprimer

### Checkpoints à garder, avec corrections
| Lieu | État | Correction |
|---|---|---|
| `app/vtx/src/main.cpp:163-172` | std + `...` | ajouter `\n` aux messages ; renvoyer le code de `exec()` ; ajouter `set_terminate` |
| `application.cpp:128-144` `notify` | std + `...`, log seulement | notifier l'utilisateur (`ApplicationError`) ; peut devenir `noexcept` |
| `application.cpp:218-226` `_postQtStartup` | std + `...` → `quit()` | `exit(EXIT_FAILURE)` au lieu de `quit()` |
| `application.cpp:105` (`~Application`) | std seulement | **ajouter `catch(...)`** |
| `vtx_app.hpp:40-52` `VTXApp::update` | std + `...` | descendre au niveau de chaque étape du pipeline et limiter la répétition des logs (§6) |
| `action_manager.hpp:107-118` | **std seulement** | voir §6 |
| `base_thread.cpp:30-45` | std + `...` | exposer l'échec (flag ou `exception_ptr`) et le nom de l'action |
| `thread_manager.cpp:39-50, 67-78, 115-123` ; `thread_manager.hpp:57-65` | corrects | — |
| `app/python_binding/interpretor.cpp:88-104` | std + `...`, la boucle meurt | voir §6 (thread Python) |
| `renderer.cpp:89-133` `render` | std + `...` | récupération moins brutale que `setDefault()` (§6) |
| `executor/opengl.hpp:285-292`, `debug.hpp:19/73` | `...` | corrects |

### Catches locaux légitimes (à garder)
- `action/io.cpp:30-37` (métadonnées optionnelles).
- `renderer.cpp:573-580` (environnement).
- `extractor/system.cpp:101-109, 151-248` (rollback + rethrow). Corriger le double `count_down`, idéalement avec un scope guard.
- `session.cpp` (101-118, 123-132, 159-171, 237-291, 359-373) : gestion d'état. Le bloc `359-373` est un bon modèle de notification utilisateur.
- `network_manager.cpp:31-55` (bookkeeping) ; `trajectory_updater.cpp:24-41` (garde de destructeur).
- `filesystem.cpp:44-51, 146-155`, `logger.cpp:72/119` (traduction) ; `pdb2gmx.cpp:203`.
- `form_advanced/event_manager.cpp:51, 100` ; `base_widget.hpp:36`.
- `baker.cpp:148-158, 304-309`.
- `ses.cpp:409-516, 963-1001` : à durcir, car leurs handlers peuvent eux-mêmes lever.
- `renderer.cpp:186-227` `snapshot` : remplacer le catch par un scope guard.
- `context.hpp:128-132` (rethrow) : passer à `catch(...)` par symétrie.
- `opengl.cpp:1468-1482` : la première boucle est morte et le nettoyage peut masquer l'erreur.
- Côté Python : `wrapper/function.cpp:28, 45` ; `module.cpp:31`.

### Catches à compléter
- `style_manager.cpp:105` : pas de `catch(...)`, et le fallback rappelle `setTheme`, qui peut relancer.
- `app/bench/src/main.cpp:186` : pas de `catch(...)`.
- `vtx_app.cpp:175-188` (init Python) : pas de `catch(...)`, et l'échec asynchrone n'est pas couvert (P0-1).
- `network_manager.cpp:31-55` : une exception non standard laisse `_activeDownload` dans un état périmé.
- `lib/python_binding/.../interpretor.cpp:218/222` : pas de `catch(...)`. Le premier handler est redondant.
- `interpretor.cpp:241` : remplacer `throw e;` (copie tronquée) par `throw;`. Le log puis rethrow produit deux logs.
- `function.hpp:31` : le message dit « not found » pour toute erreur Python, et le `cast_error` d'argument n'est pas encapsulé.
- `object.hpp:57, 72` : `pybind11::str()` dans le handler peut lever à nouveau.
- `module.cpp:61` : `cast_error` s'échappe.
- `gromacs.cpp:304/308` : le code avant et après le try n'est pas protégé (P1-8).

### Redondants, à supprimer une fois le checkpoint ActionManager corrigé
- `lib/app/src/app/action/io.cpp:245-258` (Snapshot) : tous les appels passent par `ACTION().execute`. ✔
- `lib/app/src/app/python_binding/trajectory.cpp:31-59, 87-98` : pybind11 traduit déjà `invalid_argument` et `out_of_range` en ValueError et IndexError.
- `renderer.cpp:19-31` (`setDefault`) : le build sur le backend null ne peut pas lever.
- `renderer.cpp:618-625` : redondant avec le `catch(...)` de l'executor. À transformer en `noexcept` + `catch(...)`.
- `logger` (`debug.hpp:82-88`) : `VTX_ERROR` est `noexcept`, le try est inutile.

### Code mort
- `lib/ui/qt/src/ui/qt/validator.cpp:27-37` : try vide (le `stoull` est commenté). ✔
- `app/bench/src/user_interface.cpp:287-294` et `:585-592` : try vide. ✔
- `lib/app/src/app/vtx_app.cpp:73-87, 94-108` : les destructeurs sont `noexcept`, donc `terminate` est appelé avant le handler.
- `lib/python_binding/src/python_binding/interpretor.cpp:193, 198` : rien ne lève `CommandException` à cet endroit.
- `function.hpp:70` : `cast<T>()` lève `cast_error`, pas `error_already_set`.

Style : les catch par référence non-const sont à passer en `const &` :
- `ui/qt/util.cpp:366, 370` ; `validator.cpp` ;
- `python_binding/interpretor.cpp:95` ; `app/python_binding/interpretor.cpp:172, 177, 204` ;
- `gromacs.cpp:304` ; tests.

---

## 6. Architecture cible des checkpoints

Principe : on lève librement dans le code métier, sans try/catch local sauf pour récupérer réellement (fallback, rollback, traduction). La capture se fait à **sept niveaux**, chacun avec `std::exception` + `...`.

1. **Processus.**
   - `main` (déjà en place).
   - `std::set_terminate` : logger, flusher spdlog, et sous Windows afficher une boîte de dialogue.
   - Renvoyer le code de `exec()`.
2. **Boucle Qt.**
   - `Application::notify` (déjà en place) : la rendre `noexcept` et ajouter `HUB().enqueue<Events::ApplicationError>` pour notifier l'utilisateur.
   - Garder `aboutToQuit` et `_postQtStartup`.
   - Les slots qui font de l'I/O passent par `ACTION()`.
3. **Frame.** Remplacer le seul checkpoint de `VTXApp::update` par une garde par étape dans `Pipeline::process`, pour que RENDER et THREAD tournent toujours. Ajouter un throttle des erreurs identiques. Rendre les updaters transactionnels : vider la liste dans une locale avant de la traiter, ou retirer chaque item avant de le traiter.
4. **ActionManager** (point central) :
   ```cpp
   // esquisse
   template<typename F>
   bool _guard( const std::string_view p_action, F && p_f ) const noexcept
   {
       try { std::forward<F>( p_f )(); return true; }
       catch ( const std::exception & p_e ) { VTX_ERROR( "Action {} failed: {}", p_action, p_e.what() ); }
       catch ( ... ) { VTX_ERROR( "Action {} failed: unknown exception", p_action ); }
       // + notification UI (enqueue ApplicationError, lui-même dans un try)
       return false;
   }
   ```
   - Construire `A` **dans** `_guard`, aussi bien dans `execute<A>` que dans la lambda threadée (qui passe par la même garde, avec le nom de l'action).
   - Ajouter `std::exception_ptr error` dans `QueuedAction::State`. `_wrapper::execute` remplit toujours `executed` ou `error` et notifie, et `Waiter::wait` relance l'exception côté appelant. Ainsi pybind11 la remonte à Python, ce qui corrige les échecs silencieux.
   - Dans `update` : `swap` de la file dans une locale sous lock, puis exécution hors lock. Faire `pop` avant `execute`, et remettre `_skipTime` à zéro.
5. **Threads.**
   - Garder `BaseThread`.
   - Stocker un `exception_ptr` ou un flag `failed` consultable dans `TerminatedCallback` et dans l'événement `ThreadTerminated`.
   - Protéger `~ThreadManager` et `~BaseThread` (`catch(...)` autour de `join`/`stop`).
6. **Thread Python.**
   - Une garde `catch(...)` **par instruction et par commande** dans `_listenQueue`, qui tient toujours la promesse (résultat en échec, ou `set_exception`).
   - En cas d'échec d'init, passer en état « failed » : vider la file et faire échouer toutes les promesses en attente et futures. Cela corrige le blocage P0-1.
   - Détruire `_interpretor` sur le thread Python dans tous les cas.
7. **Renderer.**
   - Garder `render() noexcept`.
   - Remplacer « `setDefault()` sur toute erreur » par une récupération par sous-système, chacun avec son try et son fallback « désactiver la fonctionnalité » : textures, environnement, CUDA/SES, upload des systèmes. `setDefault()` est réservé à la perte réelle du contexte.
   - Garantie forte dans `refreshGraph` : construire dans un temporaire puis faire un swap, et n'assigner `p_currentConfig` qu'après le succès.

Frontières C : envelopper le callback de progression curl (`catch(...)` → `return false`) et les callbacks Velopack.

---

## 7. Proposition de lots de patch

| Lot | Contenu | Risque |
|---|---|---|
| 1 | §3 : retirer ou conditionner les `noexcept` dangereux (mdprep, `DataLocker`, `PassManager`) ; ajouter les `catch(...)` manquants (§5) ; supprimer les try vides | faible |
| 2 | Checkpoint ActionManager (§6.4) + suppression des catches redondants associés | moyen |
| 3 | Thread Python : blocage au démarrage, garde par commande, état « failed » | moyen |
| 4 | Renderer : récupération granulaire + garantie forte de `refreshGraph` + `static_assert` noexcept sur les executors | moyen/élevé (renderer) |
| 5 | Pipeline : gardes par étape + updaters transactionnels + throttle des logs | moyen |
| 6 | §4 : candidats `noexcept`, module par module (moves et hot path d'abord) | faible |
| 7 | Processus : `set_terminate`, code de sortie, callbacks C | faible |

---

## 8. Rapports AGENTS.md (commentaires, Doxygen, constexpr)

### Commentaires inexacts ou obsolètes (non modifiés)
- `lib/app/src/app/action/action_manager.cpp:41` : « We do not want to do that every frame » est faux, car `_skipTime` n'est jamais remis à zéro.
- `lib/app/include/app/action/action_manager.hpp:56` : « executed later on the main thread » est faux pour les chemins inline. La ligne `:57` a un `@param` vide.
- `lib/app/include/app/action/preset.hpp:143-154` : un TODO contient un `throw` commenté. La vérification « preset en cours d'utilisation » est désactivée, ce qui laisse une référence pendante si l'on supprime le preset actif.
- `lib/renderer/src/renderer/context/backend/opengl.cpp:323-324` « No OpenGL objects… » : faux, `build` crée des objets GL. `:638` « done once at startup » : faux, ce code s'exécute à chaque build.
- `lib/renderer/include/public/renderer/renderer.hpp:150` : « Force update each frame » sur `setVSync`.
- `context_wrapper.hpp:28` : dit « OpenGL 4.5 » et contient la coquille « backkend ». `opengl.hpp:172` dit « EGL context » alors que le membre couvre aussi WGL. `render_graph.hpp:30-31` a un `@return` vide.
- `lib/util/include/util/fmt/rgba.hpp:10` : Rgba n'hérite pas de `glm::vec4`, il contient un membre `Vec4f`.
- `lib/util/include/util/thread/base_thread.hpp:40` : ne mentionne pas le `request_stop()` du destructeur. `thread_manager.hpp:42` commence par `/*` au lieu de `/**`, donc Doxygen l'ignore. `factories.hpp:15` : coquille « Expections ».
- `lib/python_binding/src/python_binding/interpretor.cpp:165-167` : décrit une logique de fallback que les catches morts contredisent.
- `app/vtx/src/main.cpp:40-48` : bloc Doxygen mal formé.
- `md_engine_field_placer.hpp:55-63` : les textes des `static_assert` affirment que `noexcept` est imposé, ce qui n'est pas le cas.

### Doxygen manquant (headers du périmètre, non exhaustif)
- **lib/app :**
  - `action_manager.hpp` : constructeur, destructeur et `execute` de `QueuedAction`, `Del`, `_noThread`.
  - `pass_manager.hpp` : `IPass::setPaused/setDeleteNextFrame`.
  - `arguments.hpp` : champs d'`Arguments` et méthodes d'`ArgumentParser`.
  - `interpretor.hpp` : `Del`.
- **lib/renderer :** la plupart des méthodes de `context/backend/gl/*.hpp` et d'`executor/opengl.hpp`.
- **lib/util :** `players.hpp`, `sentry.hpp`, `callback.hpp`, `collection.hpp`, `datalocker.hpp`.
- **lib/python_binding :** `wrapper/function.hpp`, `object.hpp`, `module.hpp`.
- **tool/mdprep :** `form_basic/event_manager.hpp`, `settings_dialog.hpp`, `md_engine_field_placer.hpp` (uniquement des commentaires `//`).
- **app/bench :** accesseurs de `camera_controller.hpp`.

### `constexpr` / `consteval`
- Tous les usages existants sont valides en C++20.
- `enumCast`, `enumToAnother` et `nextPowerOfTwoValue` sont `constexpr` avec un chemin `throw`. C'est légal : une mauvaise utilisation à la compilation devient une erreur de compilation.
- `chrono.hpp:40` est `constexpr` avec un argument par défaut non-constexpr (`now()`). C'est légal, mais l'évaluation constante n'est possible qu'avec des arguments explicites.
- Aucun `consteval` injustifié.
- Candidats :
  - les constructeurs d'`AABB`, `invalidate`, `largestAxis` et `area` ;
  - `string(E_FILE_FORMATS)` et `convert` dans `io/writer/chemfiles.cpp` ;
  - `tool/mdprep/src/gateway/form_data.cpp:5, 18` ; `pdb2gmx.cpp:53, 69, 270` ; `shared.hpp:115`.

---

## 9. Trouvé en passant (hors exceptions)
- **`tool/mdprep/src/gateway/form_data.cpp:22`** : le multiplicateur des femtosecondes vaut `1e3`. Il devrait sans doute valoir `1e-3` (les nanosecondes valent aussi `1e3`). ✔
- `lib/renderer/include/public/renderer/context/command_buffer.hpp:575` : `operator<<` ignore son flux et écrit sur `std::cout`.
- `lib/util/src/util/players.cpp:31, 81` : avec `stepNum == 0`, `stepNum-1` donne UINT_MAX, ce qui conduit à une division par zéro dans du code `noexcept` (`:39-46, 90-102`).
- `lib/util/src/util/sentry.cpp:35` : déréférencement nul pour un `Sentry` obtenu d'un `SentryTarget` déplacé.
- `lib/util/src/util/image.cpp:131` : `assert(1)` ne se déclenche jamais. `:144-160` : Exiv2 `ImageFactory::open` lève au lieu de renvoyer null, donc le test de null est mort.
- `lib/util/include/util/string.hpp:42` : `replaceFirst` lève `out_of_range` si le motif est absent.
- `lib/renderer/src/renderer/context/context_wrapper.cpp:376` et `opengl.cpp:1434` : `front()` appelé sur un vecteur potentiellement vide.
- `tool/mdprep/src/backends/gromacs/gromacs.cpp:290-293` : le job tourne dans `parent/"1"`, mais le nettoyage porte sur `rootDir`.
- mdprep, risques de durée de vie :
  - `report.cpp:225` utilise `singleShot(0, nullptr, [&]…)` ;
  - `mdprep.cpp:103` combine une lambda sans contexte et un pointeur `win` statique ;
  - `_disconnectAll` (`form_basic/event_manager.cpp:101`) oublie `_buttonInputCheck`.

---

## 10. Bilan final (`b1a187f68`, 2026-11-06)

Méthode :
- pointage de chaque élément des §2 à §9 dans le code de HEAD ;
- trois relectures indépendantes du diff `12501064a..HEAD` (renderer ; app + python ; util, io, ui, tool, bench), dont les bugs ont été revérifiés à la main.

Rien n'a été compilé localement. La CI a compilé et fait tourner les tests Python après `1be235516`.

### 10.1 État des points de l'audit

| Point | État |
|---|---|
| P0-1 blocage au démarrage si Python échoue | ✅ corrigé (état `failed`, promesses remplies, `getRuntimePythonVersion` testé) |
| P0-2 backend GL détruit à la première erreur | ✅ corrigé : nouvelle tentative avec rafraîchissement complet, puis `setDefault()` ; textures matériau récupérées localement. ⚠ voir 10.2 B1, R3, R4 |
| P0-3 `refreshGraph` | ✅ corrigé (construction dans un temporaire, garantie forte) |
| P0-4 `noexcept` mdprep catégorie A | ✅ retirés |
| P1-5 pipeline | ✅ garde par étape, rendu toujours lancé, déduplication des logs ; updaters transactionnels. ⚠ voir R1 |
| P1-5 `EventHub::update` | ⏸ non fait, par choix : une garde par listener masquerait les erreurs au pipeline |
| P1-6 ActionManager | ✅ `_guard` + `_run`, `exception_ptr` relancée par `Waiter`, file traitée hors verrou ; double `count_down` de l'extracteur corrigé indirectement. ⏳ `_skipTime` jamais remis à zéro ; rollback d'entité à moitié construite non fait |
| P1-7 boucle Python | ✅ garde par instruction et par commande, `runCommand` n'exécute plus deux fois. ⚠ voir B2, B3 |
| P1-8 mdprep `SystemTester` / `StartPreparation` / `CheckSystem` | ✅ corrigé |
| P1-9 frontières C (curl, Velopack) | ✅ corrigé |
| P2-10 processus | ✅ code de sortie, console attachée dès le début, `set_terminate`. ⚠ voir R2. Pas de boîte de dialogue |
| P2-11 destructeurs | ✅ protégés ; code mort de `~VTXApp` retiré. ⚠ voir B4 |
| P2-12 renderer interne | ✅ `Program`, pools de ressources, `snapshot`, interop. ⏸ handlers SES non touchés |
| P2-13 divers | ✅ `Logger::init` ; `Util::Callback` supprimé entre-temps (`4f1b1c860`) |
| §3 `noexcept` à retirer / conditionner | ✅ sauf `~SettingsDialog() noexcept` (redondant, inoffensif) et le vendor Velopack |
| §3 durcissements renderer | ✅ `static_assert` sur les executors. ⏸ `_executeSESExternalPass` non typé `noexcept` |
| §4 candidats `noexcept` | ✅ appliqués. ⏸ moves de `Sesdf`/`Ssesdf` et `getNative*` écartés |
| §5 catches | ✅ complétés, `const &`, code mort retiré, `Snapshot` passe par `_guard`. ⏸ restent redondants : `python_binding/trajectory.cpp`, `setDefault`, `debug.hpp` |
| §5 `notify` → `ApplicationError` | ⏸ non fait (risque d'avalanche de dialogues) |
| §9 trouvés en passant | ✅ tous corrigés sauf `report.cpp:225` (`singleShot` sans contexte) et le test Exiv2 mort ; `mdprep.cpp` passe en plus par un `QPointer` |
| Hors périmètre | `exceptions.hpp` vérifie les formats à la compilation (`fmt::format_string`) ; `Logger::logException` / `isInitialized` ajoutés ; `replaceFirst` ne lève plus |

### 10.2 Défauts trouvés à la relecture finale

#### Bugs (vérifiés dans le code)
- ✅ **B1 — renderer : suppression d'un système neuf** (`renderer.cpp:318-327` + nouvelle tentative `:104-111`).
  - Après un échec postérieur à la boucle de suppression, `{h, DELETING}` reste dans `_dirtySystems` alors que `h` est revenu dans `_availables`.
  - Un `addSystem` réutilise `h`, et la nouvelle tentative supprime le système neuf : l'app garde un handle mort.
  - Introduit par la nouvelle tentative (avant, `setDefault()` vidait tout).
  - Correctif : retirer de `_dirtySystems` les entrées des systèmes supprimés juste après la boucle.
- ✅ **B2 — Python : `_thread` pendant** (`app/python_binding/interpretor.cpp:43, 53, 87`).
  - Si le thread Python s'arrête tôt (échec d'init), `ThreadManager::update` détruit le `BaseThread`. `~Impl` (`stop`, `wait`) et `runScript` (`getId`) l'utilisent ensuite.
  - Préexistant, mais masqué avant par le blocage du démarrage.
  - Correctif : garder l'id du thread et un drapeau « terminé », ne plus toucher au `BaseThread` ensuite ; protéger aussi `wait()`.
- ✅ **B3 — Python : promesses de scripts jamais remplies** (`interpretor.cpp:_failPendingJobs`).
  - Vider la file d'instructions détruit les lambdas de `runScript`, mais l'appelant garde sa copie de la promesse : `future.get()` bloque indéfiniment.
  - Correctif : un garde capturé par le lambda qui remplit la promesse avec « not available » si le script n'a pas tourné.
- ✅ **B4 — `~BaseThread` : `detach` après un `join` raté** (`base_thread.cpp:20-31`).
  - Le seul cas réaliste est un thread qui détruit son propre `BaseThread`. Après le `detach`, le lambda du worker écrit encore `_finished` dans un objet détruit.
  - Introduit par le groupe 2 : un `terminate` est devenu un use-after-free silencieux.
  - Correctif : supprimer le `detach` (log puis `terminate`, comme avant mais avec une trace).

#### Risques
- ✅ **R1 — pointeurs périmés avec « toujours rendre »** (`system_updater.cpp`, `mesh_updater.cpp`). Un élément qui échoue à chaque frame reste en tête de liste, le patch des pointeurs n'est jamais atteint, et le rendu tourne avec des pointeurs vers l'ancien stockage entt. Correctif : garde par élément, retirer l'élément en échec, faire le patch, puis relancer la première erreur.
- ✅ **R2 — `set_terminate` par thread sous MSVC** (`main.cpp:130`). Le gestionnaire ne couvre que le thread principal. Correctif : le propager dans `BaseThread::_start` avec `std::get_terminate()` (couvre aussi le thread Python).
- **R3 — backend à moitié construit pendant une frame** (`renderer.cpp:104-111`). Entre l'échec et la nouvelle tentative, un `resize` ou un `snapshot` peut chercher un framebuffer absent. Avant, le backend était déjà `Null`.
- ✅ **R4 — `_flushRetried` périmé**. Non remis à zéro par `clear()` ni par le `_flushDirty()` de `snapshot()`. Correctif : le remettre à zéro dans `clear()` et à la fin d'un `_flushDirty()` réussi.
- ✅ **R5 — ordre d'initialisation** (préexistant). Le thread Python écrit `_stopToken` alors que ce membre est déclaré après `_thread` et peut être construit après le démarrage du thread. Correctif : déclarer `_stopToken` avant `_thread`.
- **R6 — `onTerminate` peut bloquer** dans `LOGGER::stop()` si `terminate` survient sur le worker spdlog ou sous un verrou dont dépend un sink. Faible probabilité.

#### Détails
- Nouvelle tentative du renderer : le rafraîchissement complet saute `ses.invalidateForRecompute` des trajectoires ; une erreur déterministe coûte un rafraîchissement complet en plus.
- `ResourcePool::_release` fait un `push_back` dans un `catch` : un `bad_alloc` y remplacerait l'exception d'origine.
- Déduplication des logs du pipeline limitée à une seule erreur ; les erreurs secondaires de `PassManager` et `TrajectoryUpdater` ne sont pas dédupliquées.
- Une `QueuedAction` annulée (fermeture) est vue comme réussie par Python ; un échec d'action appelée depuis Python est loggé deux fois.
- `Image::write` lève maintenant : le snapshot du bench (`app/bench/src/user_interface.cpp:263`) n'a pas de garde et quitte le bench.
- mdprep `jobs.cpp` avale toutes les exceptions : `setFailed()` n'est jamais appelé ; le `dispatch` de `theEnd` est hors du `try`.
- Déplacement d'`EventManager` : si un `_connect*` lève, les suivants sont sautés (boutons inactifs, une ligne de log).
- Style AGENTS.md : `const` de premier niveau manquant sur les paramètres par valeur des constructeurs de `players.cpp` et des constructeurs inline de `Rgba`.

### 10.2 bis Correctifs appliqués après la relecture (non commités)
- **B1** : `renderer.cpp`, après la boucle de suppression, `std::erase_if` retire de `_dirtySystems` les entrées des systèmes supprimés.
- **R4** : `_flushRetried = false` dans `Renderer::clear()` et à la fin d'un `_flushDirty()` réussi (couvre aussi `snapshot()`).
- **B4** : `~BaseThread` n'appelle plus `detach()` ; l'échec est loggé puis le destructeur de `jthread` termine le processus.
- **R2** : `BaseThread::_start` capture `std::get_terminate()` du thread créateur et le réinstalle dans le worker (couvre le thread Python).
- **B2 + R5** : interpréteur côté App.
  - `_threadId` (atomique, écrit par le thread Python) remplace `_thread->getId()` ; le thread n'écrit plus `_thread`.
  - `_threadFinished` est posé par un garde RAII à la sortie de `runPythonThread` ; `~Impl` ne touche plus au `BaseThread` s'il est posé, et protège aussi `wait()`.
  - `_threadFinished`, `_threadId` et `_stopToken` sont déclarés avant `_thread`, donc construits avant le démarrage du thread.
- **B3** : les scripts mis en file capturent un `ScriptPromiseGuard` partagé ; si le lambda est détruit sans avoir tourné, le garde remplit la promesse avec « Python interpretor is not available. ».
- **R1** : `SystemUpdater` et `MeshUpdater`.
  - Chaque ajout, suppression et le patch sont protégés ; un élément en échec est retiré de la liste, le patch est toujours fait, puis la première exception est relancée (les suivantes sont loggées).
  - `_onDestroySystem`, `_onDestroyMesh` et `_onDestroyRepresentationPreset` ne font plus `map[ e ]` sur une entité absente : une entité encore en attente est retirée de la liste d'ajout, une entité jamais ajoutée au renderer est ignorée. Avant, `operator[]` insérait le handle 0 et supprimait un autre système.

- **Factorisation** : la règle « garder la première exception, logger les suivantes, relancer à la fin » est regroupée dans `Util::ExceptionCollector` (déclarée dans `util/exceptions.hpp` avec `run` et `rethrow` ; `collect`, qui logge, est défini dans le nouveau `util/exceptions.cpp` pour éviter le cycle `logger.hpp` → `enum.hpp` → `exceptions.hpp` ; relancer la configuration CMake, les sources étant listées par `GLOB`), utilisée par `PassManager`, `TrajectoryUpdater`, `SystemUpdater` et `MeshUpdater`.

Restent ouverts : R3, R6 et les détails de 10.2.

### 10.3 Rapports AGENTS.md
- **Commentaires devenus inexacts (non modifiés) :**
  - `python_binding/interpretor.cpp` : le commentaire de tête de `runCommand` décrit encore l'ancien « eval puis exec ».
  - `base_thread.hpp:40` « Wait for the thread to finish and destroy it. » ; `_failed` est placé sous le commentaire « Flag as stopped ».
  - `action_manager.cpp:41` « We do not want to do that every frame. » reste faux.
  - `Application::start()` « Run the main loop. » ne mentionne pas le code de retour.
- **Doxygen manquant (nouvelles déclarations) :** `Impl::FinishedGuard`, `Impl::ScriptPromiseGuard` et les membres `_threadFinished`/`_threadId` (dans un `.cpp`, donc hors vérification), `Logger::isInitialized`, `Logger::logException`, `BaseThread::hasFailed/setFailed/_failed`, `ActionManager::_run/_guard/_logFailure`, `Pipeline::_lastError/_lastErrorRepeatCount/_step/_logError/_flushRepeatedError`, `ResourcePool::_release`, `Renderer::_flushRetried`, `Object::_toString`, `ThreadTerminated::failed`.
- **`noexcept` :** tous les ajouts et retraits ont été revérifiés ; seul `~BaseThread` (B4) pose problème. Les `static_assert` des executors sont satisfaits.
- **`constexpr` :** aucun ajout ni retrait ; candidats inchangés (§8).

### 10.4 Verdict
L'architecture cible du §6 est en place et les P0/P1 de l'audit sont corrigés. La relecture finale a trouvé quatre bugs (B1 à B4), dont deux introduits par les correctifs eux-mêmes (B1, B4) et deux rendus atteignables par eux (B2, B3) ; ils sont corrigés, ainsi que R1, R2, R4 et R5 (§10.2 bis). Restent à décider : R3 et R6. **À faire avant de clore : compiler (MSVC et clang) et lancer les tests**, en particulier `python_binding`, `python_api` et les tests de l'app.
