# NOTES REFONTE ARCHI

// TODO

Création d'un module Logic qui contiendra uniquement les structures logique de VTX sous forme de modèle (?) :
Chemical :
	Molecule
	Category
	Chain
	Residue
	Atom

Math :
	Vec3f
	
Nécessité de redéfinir les structures de données tel que la Vec3f, Mat4f, Quat4f. Séparé de GLM	

Création d'un module de rendu adapté à chaque librairies
	Render_OpenGL
	
Création d'un module qui fait le lien entre les données de la logique, et le module de rendu voulu
	LogicToRender_OpenGL
	
	
	
Création d'un module intermédiaire entre Les structures logiques et l'interaction avec elle qui va gérer le système de MVC et d'event.
Intégration du système de MVC au sein du module "Logic" qui offrira une interface de manipulation des modèles qu'il inclut

Retrait des fonctions getPath/setPath dans molecule => extension à faire dans les modules qui gèrent le loading de molécule.


Quid du logger ???
Dépendance à GLM dans Logic ???


Retirer la map MoleculePtr,Float dans Scene et ne garder que MoleculePtr.
Transformer Scene et Camera en modèles

checker modern cmake + add folder instead of create vs filters

--------------------------------------------------------------------------------------
LOGIC folders and classes

	- Event
		- EventManager
		- Event
		
	- MVC
		- MVCManager
	
	- Enum
		- Tout les putains d'enums
		
	- Component
		- 
		
	- Generic
		- 
		
	- Model
		- Chemistry
			- Configuration
				- Molecule
			- Molecule
			- Category
			- Chain
			- Residue
			- Bond
			- Atom
			- SecondaryStructure
			- SolventExcludedSurface
		
		- Representation
			- RepresentationLibrary
			- RepresentationPreset
			- RepresentationPrimitive
			- RepresentationData
			- InstantiatedRepresentation (rename into RepresentationInstance)
			- RepresentationTarget
			
			
		- RenderEffect
			- RenderEffectLibrary
			- RenderEffectPreset
			
		- Color
			- Rgb
			- Rgba

		- Selection
			- Selection
		
		- Object3D
			- Scene
			- Camera
			- MeshTriangle
			- AABB
			- Grid
		
		- Math (glm using/typedef)
			- Vec3f
			...
		
	- Algo
		- Util
			- Math
		
		- GenerateSecondaryStructure
		- GenerateSES
		- GenerateBond
		- GenerateBondOrder
		
----------------------------------------------------------------------

Models that need to be in separated tool projects
		- Label
			- BaseLabel (?)
			- TextLabel
			- Measurement
				- Distance
				- DistanceToCycle
				- Angle
				- DihedralAngle
				- MeasureInProgress
		- Video
			- Viewpoint
			- Path
		