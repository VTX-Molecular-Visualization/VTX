# Architecture VTX

# Intro

Le but de la nouvelle architecture est de décomposer l'architecture actuelle en plusieurs partie pour assurer plus de modularité (Interface / Rendu), tout en conservant une exécution optimale du rendu.
Pour ce faire, nous comptons diviser le projet actuel en plusieurs parties afin d'assurer cette modularité. La découpe en plusieurs projets devrait aussi améliorer le temps de compilation du logiciel.

Pour coller à ces principes (modularité et optimisation), nous allons diviser le code en 7 sous-projets.

- **VTX_CORE** (ou VTX_DATASTRUCT) qui contiendra les structures de données pour la description d'une molécule.

- **VTX_RENDER** qui s'occupera de la partie rendue.

- **VTX_APP** (ou VTX_API) qui reliera **VTX_CORE** et **VTX_RENDER**, assurera la cohérence des données dans les structures de **VTX_CORE** et fournira toutes les autres classes et actions permettant l’exécution de fonctionnalités dans VTX

- **VTX_UI** qui fera le lien entre les actions de l'utilisateur et les fonctionnalités de **VTX_APP**

- **VTX_UTIL** (ou VTX_SHARED ?) contiendra un set de structure et de fonctionnalités commun à tous les modules dans le but d'optimiser la communication entre eux (notamment entre **VTX_CORE** et **VTX_RENDER**).

- **VTX_TOOLS** contiendra les différents outils d'analyse, de vidéo et de manipulation de molécule dans VTX qui pourront être intégrer (ou non) dans les différentes versions de VTX.

- **VTX_BUILDER** (?) permettra de builder une version de VTX à partir d'un fichier de config définissant le moteur de rendu, le type d'interface et les outils présent dans la build. (Projet séparé ou intégré dans **VTX_UI** ? Ça ne me semble pas terrible car cela implique une inter-dépendance **VTX_UI** et **VTX_TOOLS**)


**VTX_RENDER** et **VTX_UI** pourront être étendus pour intégrer de nouvelles bibliothèques graphiques (Vulkan, Metal (? MoltenVK suffisant ?) et différentes interfaces (Bash, Light, Heavy, Custom)


# Modules

## VTX_CORE (ou VTX_DATASTRUCT)

### Description

**VTX_CORE** est composé des structures de données minimales nécessaires pour l'affichage d'une molécule.
Elle contient la structure logique des molécules, chaînes, résidus, atomes et liaisons.
Ces structures doivent être simplifiées au maximum et optimisées pour le render (concaténation en mémoire, ...) .

Liste et détail des classes :
- Molecule
	- std::vector\<Chain> _chains

	- std::vector\<Residue> _residues

	- std::vector\<Atom> _atoms

	- std::vector\<std::vector\<Vec3f>> _atomPositionsFrames ¤ *(Pour le moment ici. Concaténation des positions en mémoire pour optimisation GPU.)*
	
	- std::vector\<Bond> _bonds

	- Mat4f _transform (?) *(Ici ou dans VTX_APP ?)*

- Chain
	- Molecule* _moleculePtr (?) *(Ici ou dans VTX_APP ?)*

	- uint _firstResidueIndex

	- uint _residueCount
	
- Residue
	- Chain * _chainIndex (?) *(Ici ou dans VTX_APP ?)*

	- uint _firstAtomIndex

	- uint _atomCount

	- uint _firstBondIndex

	- uint _bondCount

- Atom
	- Residue * _residuePtr (?) *(Ici ou dans VTX_APP ?)*

	- uint _index (?)

	- SYMBOL (enum int) _symbol
	
	- Vec4f _positionAndRadius (?) ¤ *(Position+radius présent en tant qu'attribut d'atome ou directement dans Molecule ?)*
	
- Bond
	- Molecule* _moleculePtr (?) *(Ici ou dans VTX_APP ?)*

	- uint _firstAtom

	- uint _secondAtom

	- BOND_ORDER (enum int) _bondOrder *(BOND_ORDER pourrait être enum char si opti car contient peu de valeurs différentes (12))*


### ¤ Notes

- Comment stocker efficacement les frames d'animation (opti mémoire & opti render) ?

	- *Molecule::_atomPositionsFrames* pourrait être simplement un vecteur de Vec3f et les différentes frames pourraient définies avec un vecteur d'index (ou index, count).

	- *Molecule::_atomPositionsFrames* pourrait contenir des Vec4f au lieu de Vec3f pour intégrer le radius de l'atome (opti GPU ?)

	- Autre ?

- Stocker un identifiant unique pour chaque objet pour le picking ou gérer ça dans VTX_APP ?

- Enregistrement des couleurs ici ?

### Dépendances

**VTX_CORE** ne dépend que de **VTX_UTIL** pour des soucis d'optimisations (partage de structures avec **VTX_RENDER** pour éviter les conversions Vec3f, Vec4f, Mat4f, uint).

## VTX_RENDER

### Description

**VTX_RENDER** fourni une structure de donnée appropriée pour produire un rendu de molécule et fait l'affichage de celle-ci via la bibliothèque de rendu souhaitée.

**VTX_RENDER** devra exposer une liste de fonctionnalité relativement abstraite (définir à quel point) permettant de gérer les besoins au niveau du rendu.

Liste des fonctionnalités de **VTX_RENDER** :
- Produire un rendu

- Gérer les paramètres de la caméra (near clip, far clip, FOV, background color) *(Ici ?)*

- Gestion des render effects (SSAO, Outline, Fog) *(Ici ?)*

- Gestion des Lights (Global, point lights) *(Ici ?)*

- Générer un Snapshot

- Récupération de donnée dans un buffer (ie Picking)


Structure de rendu :

- Structure molécule :
	- Geometry (AtomRadius + AtomPositions dans un Vec4f (?) ?, Bonds)

	- Buffers (ModelID (?), Symbol, Color, Selection, Visibility, ...(?)) ¤

- Structure Secondary Structure (?)

- Structure Secondary SES (?)

- Structure Render Effects (?)

### ¤ Idées

- Possibilité d'optimiser les données du buffer dans un Vec4f avec la couleur en Vec3f, et le dernier float lu comme un BitArray (Selection, Visibility, autre ?).

- Associer 2 tableaux pour une molécule :

	- Un premier tableau de Vec4f avec position & info complémentaire pour chaque atomes

	- Un second tableau d'int contenant le symbol de l'atome pour pouvoir récupérer les informations relative au type d'atome stocké dans une table à part (radius, default_Color, (...) ).

### ¤ Notes

- Gestion des "Render Effects" dépendant du moteur ?

- Comment gérer les tools ayant besoin de fonctionnalités custom sur le moteur de rendu ?

### Dépendances

**VTX_RENDER** ne dépend que de **VTX_UTIL** pour des soucis d'optimisations (partage de structures avec **VTX_CORE** pour éviter des conversions).

## VTX_APP (ou VTX_API)

### Description

**VTX_APP** va s'occuper de faire le lien entre Core et Render, et fournir aux modules supérieurs différentes classes et actions permettant d’interagir avec l'environnement VTX.

Repenser l'archi en Entity Component System (via EnTT ?).

En plus du lien entre Core et Render, **VTX_APP** devrait contenir tout ce qui a attrait à :
- SES

- SS

- Selection

- Gestion des représentations (preset, etc)

- Gestion des render effects (preset, etc) (?)

- Gestion d'une scène

- Interface de manipulation de la camera

- Actions (+ Undo/redo)

- Events

- Settings

- Sauvegarde / Chargement de scène

- Sauvegarde(?) / Chargement / Téléchargement (?) de molécules

- Gestion des labels (?) (Ici ou dans VTX_TOOLS ?)

- Gestion des viewpoints (?) (Ici ou dans VTX_TOOLS ?)

- Gestion des paths (?) (Ici ou dans VTX_TOOLS ?)


**VTX_APP** pourrait fournir aussi toute une liste d'outils pour le développement de fonctionnalités (reader/writer json, worker/threading, network)

### Dépendances

**VTX_APP** dépend de **VTX_CORE** et de **VTX_RENDER** pour assurer la liaison entre ces deux modules.

## VTX_UI

**VTX_UI** fait l'interface entre l'utilisateur et **VTX_APP**.

La communication entre ces deux modules se fera par le biais d'actions et d'events. **VTX_UI** pourra lancer des actions sur l'ActionManager de **VTX_APP** qui les exécutera. Lorsqu'une structure de **VTX_APP** sera modifiée, elle enverra un event qui pourra être récupéré par les objets de **VTX_UI** qui se mettront à jour en conséquence.

**VTX_UI** propose plusieurs interfaces utilisateur différentes :

- Une interface "Bash" qui ne sera pas graphique et qui prendra que des lignes de commandes en entrée.

- Une interface graphique type Qt (+ Controllers).

L'interface graphique devra définir une partie abstraite pour gérer le layout, et une partie implémentation avec la librairie UI de notre choix (Par exemple Qt).

**VTX_UI** doit exposer tout un set de fonctions pour composer son interface (qui seront appelées selon les valeurs décrites par un fichier JSON depuis **VTX_BUILDER**) afin de générer des interfaces customisées selon les cas d'utilisation.

Dans cette optique, chaque élément d'interface intégré dans ce module doit être implémenté comme le seront les tools qui pourront venir se greffer à l'interface.

Les éléments intégrés d'office à **VTX_UI** sont :
- Le Controller (Camera et Picking)

- La fenêtre de Render

- La fenêtre de Scene

- La fenêtre d'Inspector

- La Console

- La Fenêtre de Sequence (?)

- La fenêtre de Settings

- La fenêtre d'About

- La gestion de Scene (Nouvelle Scene / Sauvegarde / Chargement)

- La gestion de Molécule (Loading, Download, Export) - (Duplicate, Extract, Delete) - (Show, Hide, Solo) - (Show/Hide Water, Solvent, Ion, Hydrogen)

- La gestion des Représentations

- La gestion des Render Effects

- La gestion des Labels (?)

- La gestion des Viewpoints (?)

- Le Snapshot (?)

**VTX_UI** sera composé d'au moins 3 namespaces. Dans un premier namespace (CORE) sera codé l'abstraction de l'interface (Ajouter un bouton, créer une fenêtre, changer le mode du pointeur, ...), ainsi que la génération de l'interface à partir d'un fichier JSON.

Un second namespace contiendra l'implémentation du CORE (dans un premier temps avec QT, puis avec potentiellement d'autres implémentations dans le futur.)

Le 3ème namespace contiendra la génération de l'interface de base de VTX (partie abstraite et partie implémentation (QT)).

### Notes

- La création de ces différentes interfaces pourrait être faite via un fichier de configuration (en JSON) qui décrirait les différentes fonctionnalités qu'on souhaiterait intégrer dans notre build de VTX. Ce fichier de config est géré dans **VTX_BUILDER** pour pouvoir aussi gérer les tools.

- Bien penser à rendre configurable les fenêtre par défaut (Scene, Inspector, Contextual Menu) pour les tools externes.

### Dépendances

**VTX_UI** dépend de **VTX_APP** pour pouvoir appeler les différentes fonctionnalités exposées par ce dernier et pourrait dépendre de **VTX_RENDER** pour la fonction Snapshot (si l'action Snapshot n'est pas intégrée dans **VTX_APP**)..

## VTX_UTIL (ou VTX_SHARED)

**VTX_UTIL** va contenir diverses structures, classes et fonctions communes à tous les modules. Principalement les structures et algo mathématiques, ainsi que les structures de nécessaire au debug.

Liste des classes de VTX_UTIL :
- Exception : Définition des exceptions spécifiques à VTX (? Ici ou dans VTX_APP ?)

- Math
 	- Types
		- uint

		- Vec3f

		- Vec4f

		- Mat4f

		- Color

		- (...)

	- Constants *: Contient diverses constantes mathématiques utiles (Pi, VEC3F_ZERO, etc)*

- Util
	- Math *: Contient diverses fonctions mathématiques avec les structures dans Math*
	
	- String *: Contient diverses fonctions de manipulation de std::string*

- FilePath

- Time

- Tool
	- Logger (?) *(Ici ou dans VTX_APP ?)*

	- Chrono
	

### Dépendances

Par nature, ce module ne doit donc dépendre d'aucun autre module de VTX.
	
## VTX_TOOLS

**VTX_TOOLS** va contenir l'ensemble des outils annexes qui pourront être greffés à VTX via **VTX_UI**.

Afin de limiter le nombre de projet dans la solution VTX, nous allons intégrer l'ensemble des tools dans un même module **VTX_TOOLS**. Il faudra bien partitionner les tools dans des namespaces séparés. Peut-être ajouter une séparation intermédiaire par métier (Illustration, Video, Analyse, Simulation, etc)

La dépendance entre tools pourra être envisagée mais il faudra trouver un bon moyen de gérer ça. (par exemple le tool Measurement qui récupère des données du tool Label, etc).

Pour bien faire, un tool devra être scinder en 2 parties, la partie APP qui contient les données et les fonctions nécessaires au tool, et la partie UI qui implémentera son interface (CommandLine, Qt, ImGUI (?)).

Liste des tools existants:
- Analysis
	- Fenêtre de Séquence (Ici ou dans **VTX_APP** ?)

	- Measurement
		- Distance

		- Angle

		- Dihedral angle

	- RMSD

	- Structural Alignment

- Illustration
	- Label (Ici ou dans **VTX_APP** ?)

	- Viewpoints (Ici ou dans **VTX_APP** ?)

	- Snapshot (Ici ou dans **VTX_APP** ?)

Liste des tools à venir :
- Analysis
	- Measurement
		- Distance to cycle

	- Hydrophobic area

- Simulation
	- UDock

	- Tinker

- Video
	- Path

- (...)

### Note 



### Dépendances

**VTX_TOOLS** dépend de **VTX_UI** pour implémenter son interface, et potentiellement de **VTX_APP** et de **VTX_RENDER** selon les outils.

## VTX_BUILDER (?)

(Module à part entière ?) **VTX_BUILDER** est un module qui doit pouvoir builder une version de VTX à partir d'un fichier de config.
Il dépend de tout. C'est la pièce la plus haut niveau du logiciel.

Le fichier de config doit spécifier la / les bibliothèque de rendu à utiliser (choisi automatiquement selon plateforme cible ?), l'interface (CommandLine, Qt, ImGUI) et les différents tools externes à greffer à l'interface.
Ce doit être le point d'entrée de l’exécutable et le projet courant par défaut dans Visual Studio.

Prévoir plusieurs fichier de config par défaut :
- VTX : Interface de release avec tout les outils voulus.

- VTX Lite : Interface minimale pour les tests de perfs graphiques, et l'utilisation dans les TP CG.

- Bash : Pas d'interface graphique, ni de rendu. Utilisé pour faire des calculs ou des exports programmés uniquement. (Build séparée ?)

Autres fichiers de config probable :
- VTX Dev : Interface avec un (ou plusieurs) outils en cours de développement.

- VTX Qubit : Interface avec seulement les outils intéressant Qubit.

### Note 

- Selon la plateforme cible de la build, VTX_BUILDER pourrait activer (ou non) les moteurs de rendus adaptés à cette plateforme (Windows => OpenGL & Vulkan ; Mac => Vulkan & Metal ; Linux => OpenGL & Vulkan)

- A priori, au niveau de l'UI, il serait possible de ne pas passer par un projet à part et d'utiliser le pattern Abstract Factory pour intégrer automatiquement les widgets de VTX_TOOLS dans l'interface. Ce pattern devrait être applicable aux bibliothèques de rendu aussi. (https://stackoverflow.com/questions/582331/is-there-a-way-to-instantiate-objects-from-a-string-holding-their-class-name)

### Dépendances

**VTX_BUILDER** dépend de **VTX_UI** et **VTX_TOOLS** pour builder une version de VTX.


## Schéma des dépendances

```plantuml
@startuml
[VTX_UTIL]
[VTX_CORE]
[VTX_RENDER]
[VTX_APP]
[VTX_UI]
[VTX_TOOLS]

[VTX_BUILDER]

[VTX_CORE]-->[VTX_UTIL]
[VTX_RENDER]-->[VTX_UTIL]

[VTX_APP]-->[VTX_CORE]
[VTX_APP]-down->[VTX_RENDER]

[VTX_UI]-->[VTX_APP]

[VTX_TOOLS]-->[VTX_UI]
[VTX_TOOLS]-->[VTX_APP]
[VTX_TOOLS]..>[VTX_RENDER]

[VTX_BUILDER]-->[VTX_TOOLS]
[VTX_BUILDER]-->[VTX_UI]
@enduml
```

# Rendu

## Rendre une molécule

Nous allons détailler ici comment se déroule le rendu d'une molécule.

### Détail des classes en jeu

#### VTX_CORE

```plantuml
@startuml
class MoleculeCore
{
	Chain[] _chains
	Residue[] _residues
	Atom[] _atoms
	AtomPositions[] _atomPositionsFrames
	Bond[] _bonds
	Mat4f _transform
}
@enduml
```

#### VTX_RENDER

```plantuml
@startuml

abstract class AbstractRenderer
{
}
class MoleculeRenderer
{
	geometry
	--
	buffers
}

class RenderingEngine{
	void render(AbstractRenderer p_renderer)
}

AbstractRenderer <|-- MoleculeRenderer

@enduml
```

#### VTX_APP

```plantuml
@startuml
	package "VTX_CORE"
	{
		class MoleculeCore
	}
	package "VTX_RENDER"
	{
		class MoleculeRenderer
		class RenderingEngine
	}

	package "VTX_APP"
	{
		class VTXApp
		class VTXEngine

		class Entity
		class Component

		class Scene
		class SceneItem
		class Camera

		class Renderer
		class MoleculeRendererComponent

		class MoleculeApp
		{
			string _name
			...
		}

		abstract class RepresentationArchetype
		class AtomSphere
		class BondCylinder

		abstract class Representation
		{
			IndexRange[] _targets
		}
		class Stick
		{
			SphereData _sphereData
			CylinderData _cylinderData
		}
		class VanDerWaals
		{
			SphereData _sphereData
		}
		class BallAndSticks
		{
			SphereData _sphereData
			CylinderData _cylinderData
		}
	}

	VTXApp *-- VTXEngine
	VTXApp *-- Scene

	VTXEngine *-- RenderingEngine

	Entity *-- Component

	Component <|-- Scene
	Component <|-- Camera
	Component <|-- MoleculeApp
	Component <|-- Representation
	
	Component <|-- Renderer
	Renderer <|-- MoleculeRendererComponent

	Scene o-- SceneItem
	Scene o-right- Camera

	MoleculeApp *-- MoleculeCore

	MoleculeRendererComponent *-- RepresentationArchetype
	MoleculeRendererComponent *-- MoleculeRenderer

	RepresentationArchetype <|-- BondCylinder
	RepresentationArchetype <|-- AtomSphere
	
	Representation <|-- Stick
	Representation <|-- VanDerWaals
	Representation <|-- BallAndSticks

	Stick ... AtomSphere
	VanDerWaals ... AtomSphere
	BallAndSticks ... AtomSphere

	Stick ... BondCylinder
	BallAndSticks ... BondCylinder
	

	MoleculeCore -[hidden]- MoleculeRenderer
@enduml
```

Représentation des entités Molecule, Scene et Camera
```plantuml
@startuml
entity MoleculeEntity
entity SceneEntity
entity CameraEntity


SceneEntity -- Scene
Scene o-- SceneItem
Scene o-- Camera

CameraEntity -- Camera

MoleculeEntity -- MoleculeRendererComponent : Mandatory
MoleculeEntity -- MoleculeApp : Mandatory
MoleculeEntity -- SceneItem : Mandatory
MoleculeEntity .. Stick : Optional
@enduml
```

### Séquence détaillée

Pour générer un rendu dans VTX nous allons avoir besoin de plusieurs éléments :
- VTXEngine, une classe de **VTX_APP** qui va permettre de lancer un rendu sur **VTX_RENDER**.

- SceneEntity, une entité possedant un composant (Scene) qui va stocker les différents éléments qui peuvent être rendus.

- MoleculeEntity, une entité qui va contenir les données de la molécule et ses représentations

- Un composant de représentation (par exemple Stick)

- Des RepresentationArchetype qui vont permettre de factoriser l'affectation dans les buffers.

Nous allons suivre pas à pas l'intéraction entre ces différents éléments pour afficher une molécule en représentation Stick dans VTX

#### Création d'une molécule

Avant toute chose, il faut ajouter une molécule dans la scène via la fonction addMolecule dans Scene. Avec le modèle ECS, une molécule est une entité possédant certains composants obligatoires, et d'autres optionnels. Les composants obligatoires sont ajoutés dans la fonction d'instantiation de la molécule.

```c++
MoleculeApp & Scene::addMolecule()
{
	Entity * molecule = instantiateMolecule();
	_sceneItemContainer.emplace_back( molecule );

	return *molecule->getComponent<MoleculeApp>();
}

Entity * Scene::instantiateMolecule()
{
	Entity * moleculeEntity = new Entity();
	
	moleculeEntity->addComponent<SceneItem>();
	moleculeEntity->addComponent<MoleculeApp>();
	moleculeEntity->addComponent<MoleculeRenderable>();
	
	return moleculeEntity;
}
```

Note : On peut imaginer passer par une factory qui s'occupe d'instancier et d'ajouter tous les composants obligatoires pour le type d'objet à instancier.

#### Ajouter une représentation à la molécule

L'ajout d'une représentation se fait en ajoutant un component héritant de Representation à une entité possedant un component héritant de Renderable. Par exemple on peut ajouter le component Stick à l'entité Molecule qui possède forcément un component MoleculeRenderable (héritant de Renderable) :

```c++
moleculeEntity.addComponent<Stick>();
```

Lorsqu'elle est créée ou modifiée, la représentation Stick va récupérer les RepresentationArchetype qui la concerne et mettre à jour ses données. Ces archetype écriront directement dans le buffer de rendu lors de la mise à jour des buffers.

```c++
void Stick::refresh()
{
	getComponent<MoleculeRenderable>()->getArchetype<AtomSphere>()->setData(_target, _sphereData);
	getComponent<MoleculeRenderable>()->getArchetype<BondCylinder>()->setData(_target, _cylinderData);
}
```

La representation Stick connait les archetypes sur lesquels elle doit agir et affecte les données qui la concerne dedans.

Note : La molecule peut instancier les archetype lorsqu'ils sont appelé la première fois, et garder une trace des représentations ayant besoin de ces archetypes afin de les supprimer si la dernière représentation accédant à un archetype de représentation est supprimée.

#### Mise à jour des buffers

La classe VTXEngine est appelée régulièrement pour générer un rendu. Lors de sa boucle de rendu, elle va dans un premier temps mettre à jour les buffers des différents objets à afficher puis envoyer les différents objets à rendre au moteur de rendu.

Mise à jour des buffers :
```c++
void VTXEngine::updateBuffers(const Scene & p_scene)
{
	foreach ( Renderable * renderable : p_scene.getAllComponentsOfType<Renderable>() )
		renderable->refreshBuffers();
}

void MoleculeRenderable::refreshBuffers()
{
	foreach (RepresentationArchetype * representationArchetype : _representationArchetypes )
	{
		representationArchetype->fillBuffers( _renderClass );
	}
}

void BondCylinder::fillBuffers(VTX_RENDER::MoleculeRenderer & p_renderClass)
{
	// Ici je reprend ce qu'il y a dans View::Cylinder. Ce sera sûrement à adapter. C'est pour donner une idée du contenu de la fonction
	_program->setFloat( "u_cylRad", _cylinderData.radius );
	_program->setUInt( "u_colorBlendingMode", uint( _cylinderData.colorBlendingMode ) );

	p_renderClass.getBuffer()->getVao().multiDrawElement( 
		Renderer::GL::VertexArray::DrawMode::LINES,
		(GLsizei *)( &_target.counts[ 0 ] ),
		Renderer::GL::VertexArray::Type::UNSIGNED_INT,
		(GLvoid **)( &t_arget.indices[ 0 ] ),
		GLsizei( _target.indices.size() ) );
}
```
Note : On peut imaginer que les RepresentationArchetype possèdent un flag pour ne rien faire si aucune modification n'a été faite dessus.

Les RepresentationArchetype vont directement accéder aux buffers d'AbstractRenderer liés au composant Renderable.

Rendu :
```c++
void VTXEngine::render(const Scene & p_scene)
{
	_graphicEngine.clear();

	foreach ( Renderable * renderable : p_scene.getAllComponentsOfType<Renderable>() )
		_graphicEngine->render(renderable->getRenderClass(), p_scene.getCamera() )
}
```

# Test

La modularité va nous permettre de faciliter les tests de non-regressions en testant certains inputs et en s'assurant que la sortie est conforme à notre attente.