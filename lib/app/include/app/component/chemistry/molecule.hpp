#ifndef __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__
#define __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__

#include "_fwd.hpp"
#include "app/application/representation/base_representable.hpp"
#include "app/application/representation/instantiated_representation.hpp"
#include "app/application/representation/representation_target.hpp"
#include "app/application/selection/selection.hpp"
#include "app/component/chemistry/enum_trajectory.hpp"
#include "app/component/chemistry/secondary_structure.hpp"
#include "app/component/chemistry/solvent_excluded_surface.hpp"
#include "app/component/io/molecule_configuration.hpp"
#include "app/component/object3d/helper/aabb.hpp"
#include "app/core/model/base_model_3d.hpp"
#include "app/core/scene/base_scene_item.hpp"
#include "app/internal/io/reader/prm.hpp"
#include "app/internal/io/reader/psf.hpp"
#include "app/render/buffer/molecule.hpp"
#include <core/chemdb/category.hpp>
#include <core/chemdb/unknown_residue_data.hpp>
#include <core/struct/molecule.hpp>
#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Molecule :
		public Core::Model::BaseModel3D<App::Render::Buffer::Molecule>,
		public Component::Generic::BaseColorable,
		public Application::Representation::BaseRepresentable,
		public Core::Scene::BaseSceneItem
	{
		VTX_MODEL

	  public:
		using AtomPositionsFrame  = VTX::Core::Struct::Molecule::AtomPositionsFrame;
		using RepresentationState = std::map<const Application::Representation::InstantiatedRepresentation *,
											 Application::Representation::RepresentationTarget>;

	  private:
		// Using these tmp vector while new archi of molecule is not ready for getChains functions
		inline static std::vector<Chain *> TMP_CHAIN_VECTOR = std::vector<Chain *>();
		// Using these tmp vector while new archi of molecule is not ready for getChains functions
		inline static std::vector<Residue *> TMP_RESIDUE_VECTOR = std::vector<Residue *>();
		// Using these tmp vector while new archi of molecule is not ready for getChains functions
		inline static std::vector<Atom *> TMP_ATOM_VECTOR = std::vector<Atom *>();
		// Using these tmp vector while new archi of molecule is not ready for getChains functions
		inline static std::vector<Bond *> TMP_BOND_VECTOR = std::vector<Bond *>();

	  public:
		inline VTX::Core::Struct::Molecule &	   getMoleculeStruct() { return *_moleculeStruct; };
		inline const VTX::Core::Struct::Molecule & getMoleculeStruct() const { return *_moleculeStruct; };

		// Configuration.
		inline const Component::IO::MoleculeConfiguration & getConfiguration() const { return _configuration; }
		inline Component::IO::MoleculeConfiguration &		getConfiguration() { return _configuration; }
		inline void setConfiguration( const Component::IO::MoleculeConfiguration & p_config )
		{
			_configuration = p_config;
		}

		// BaseSceneItem
		const App::Core::Model::ID & getModelID() const override { return getId(); }

		// Representation.
		inline const RepresentationState & getRepresentationState() const { return _representationState; }
		inline RepresentationState &	   getRepresentationState() { return _representationState; }

		// Models.
		inline const std::string & getName() const { return _moleculeStruct->getName(); }
		inline void				   setName( const std::string & p_name ) { _moleculeStruct->setName( p_name ); }
		inline const std::string & getPdbIdCode() const { return _pdbIdCode; }
		void					   setPdbIdCode( const std::string & p_pdbId );
		inline const FilePath &	   getPath() const { return _path; }
		inline void				   setPath( const FilePath & p_path ) { _path = p_path; }

		Chain &				 addChain();
		inline Chain * const getChain( const uint p_idx )
		{
			return nullptr;
			// return _moleculeStruct->getChain( p_idx );
		}
		inline const Chain * const getChain( const uint p_idx ) const
		{
			return nullptr;
			// return _moleculeStruct->getChain( p_idx );
		}
		Chain * getFirstChain()
		{
			return nullptr;
			// return _moleculeStruct->getFirstChain();
		}
		const Chain * const getFirstChain() const
		{
			return nullptr;
			//	return _moleculeStruct->getFirstChain();
		}
		const Chain * const getPreviousChain( const uint p_idBaseChain ) const
		{
			return nullptr;
			// return _moleculeStruct->getPreviousChain();
		}
		Chain * const getPreviousChain( const uint p_idBaseChain )
		{
			return nullptr;
			//	return _moleculeStruct->getPreviousChain();
		}
		const Chain * const getNextChain( const uint p_idBaseChain ) const
		{
			return nullptr;
			// return _moleculeStruct->getNextChain( p_idBaseChain );
		}
		Chain * const getNextChain( const uint p_idBaseChain )
		{
			return nullptr;
			// return _moleculeStruct->getNextChain( p_idBaseChain );
		}
		inline std::vector<Chain *> & getChains()
		{
			return TMP_CHAIN_VECTOR;
			// return _moleculeStruct->getChains();
		}

		inline const std::vector<Chain *> & getChains() const
		{
			return TMP_CHAIN_VECTOR;
			// return _moleculeStruct->getChains();
		}
		uint getRealChainCount() const { return _moleculeStruct->getRealChainCount(); };
		void removeChain( const uint p_id,
						  const bool p_delete	   = true,
						  const bool p_recursive   = true,
						  const bool p_notifyViews = true );

		Residue &			   addResidue();
		inline Residue * const getResidue( const uint p_idx )
		{
			return nullptr;
			// return _moleculeStruct->getResidue( p_idx );
		}
		inline const Residue * const getResidue( const uint p_idx ) const
		{
			return nullptr;
			// return _moleculeStruct->getResidue( p_idx );
		}
		const Residue * const getPreviousResidue( const uint p_idBaseResidue ) const
		{
			return nullptr;
			// return _moleculeStruct->getPreviousResidue( p_idBaseResidue );
		}
		Residue * const getPreviousResidue( const uint p_idBaseResidue )
		{
			return nullptr;
			// return _moleculeStruct->getPreviousResidue( p_idBaseResidue );
		}
		const Residue * const getNextResidue( const uint p_idBaseResidue ) const
		{
			return nullptr;
			// return _moleculeStruct->getNextResidue( p_idBaseResidue );
		}
		Residue * const getNextResidue( const uint p_idBaseResidue )
		{
			return nullptr;
			// return _moleculeStruct->getNextResidue( p_idBaseResidue );
		}
		inline std::vector<Residue *> & getResidues()
		{
			return TMP_RESIDUE_VECTOR;
			// return _moleculeStruct->getResidues();
		}
		inline const std::vector<Residue *> & getResidues() const
		{
			return TMP_RESIDUE_VECTOR;
			// return _moleculeStruct->getResidues();
		}
		int	 getRealResidueCount() const { return _moleculeStruct->getRealResidueCount(); }
		void removeResidue( const uint p_id,
							const bool p_delete			   = true,
							const bool p_recursive		   = true,
							const bool p_checkParentUpdate = true,
							const bool p_notifyViews	   = true );

		Atom &				addAtom();
		inline Atom * const getAtom( const uint p_idx )
		{
			return nullptr;
			// return _moleculeStruct->getAtom( p_idx );
		}
		inline const Atom * const getAtom( const uint p_idx ) const
		{
			return nullptr;
			// return _moleculeStruct->getAtom( p_idx );
		}
		inline std::vector<Atom *> & getAtoms()
		{
			return TMP_ATOM_VECTOR;
			// return _moleculeStruct->getAtoms();
		}
		inline const std::vector<Atom *> & getAtoms() const
		{
			return TMP_ATOM_VECTOR;
			// return _moleculeStruct->getAtoms();
		}
		int	 getRealAtomCount() const { return _moleculeStruct->getRealAtomCount(); }
		void removeAtom( const uint p_id,
						 const bool p_delete			= true,
						 const bool p_checkBonds		= true,
						 const bool p_checkParentUpdate = true,
						 const bool p_notifyViews		= true );

		Bond &				addBond();
		inline Bond * const getBond( const uint p_idx )
		{
			return nullptr;
			// return _moleculeStruct->getBond( p_idx );
		}
		inline const Bond * const getBond( const uint p_idx ) const
		{
			return nullptr;
			// return _moleculeStruct->getBond( p_idx );
		}
		inline std::vector<Bond *> & getBonds()
		{
			return TMP_BOND_VECTOR;
			// return _moleculeStruct->getBonds();
		}
		inline const std::vector<Bond *> & getBonds() const
		{
			return TMP_BOND_VECTOR;
			// return _moleculeStruct->getBonds();
		}
		void removeBond( const uint p_id, const bool p_delete = true, const bool p_notifyViews = true );

		inline const bool				  hasSecondaryStructure() const { return _secondaryStructure != nullptr; }
		inline const SecondaryStructure & getSecondaryStructure() const { return *_secondaryStructure; }
		inline SecondaryStructure &		  getSecondaryStructure() { return *_secondaryStructure; }
		inline const bool hasSolventExcludedSurface() const { return _solventExcludedSurfaces.empty() == false; }

		bool					 hasSolventExcludedSurface( const ChemDB::Category::TYPE & p_categoryEnum ) const;
		SolventExcludedSurface & getSolventExcludedSurface( const ChemDB::Category::TYPE & p_categoryEnum );

		inline const Chemistry::Category & getCategory( const ChemDB::Category::TYPE & p_categoryEnum ) const
		{
			return *( _categories[ int( p_categoryEnum ) ] );
		}
		inline Chemistry::Category & getCategory( const ChemDB::Category::TYPE & p_categoryEnum )
		{
			return *( _categories[ int( p_categoryEnum ) ] );
		}
		inline const std::vector<Chemistry::Category *> & getCategories() const { return _categories; }
		Chemistry::Category *							  getCategoryFromChain( const Chemistry::Chain & p_chain );
		const Chemistry::Category * getCategoryFromChain( const Chemistry::Chain & p_chain ) const;

		bool isEmpty() { return _moleculeStruct->isEmpty(); }

		inline const bool isAtomVisible( const uint p_idx ) const { return bool( _bufferAtomVisibilities[ p_idx ] ); }

		inline const float &			 getAtomRadius( const uint p_idx ) const { return _bufferAtomRadius[ p_idx ]; }
		inline const Util::Color::Rgba & getAtomColor( const uint p_idx ) const { return _bufferAtomColors[ p_idx ]; }
		inline const uint getAtomVisibility( const uint p_idx ) const { return _bufferAtomVisibilities[ p_idx ]; }

		inline const std::vector<VTX::Core::ChemDB::UnknownResidueData *> & getUnknownResidueSymbols() const
		{
			return _moleculeStruct->getUnknownResidueSymbols();
		}
		inline const std::unordered_set<std::string> & getUnknownAtomSymbols() const
		{
			return _moleculeStruct->getUnknownAtomSymbols();
		}

		int getUnknownResidueSymbolIndex( const std::string & p_symbol ) const
		{
			return _moleculeStruct->getUnknownResidueSymbolIndex( p_symbol );
		}
		VTX::Core::ChemDB::UnknownResidueData * const getUnknownResidueSymbol( const uint p_symbolIndex ) const
		{
			return _moleculeStruct->getUnknownResidueSymbol( p_symbolIndex );
		}
		VTX::Core::ChemDB::UnknownResidueData * const getUnknownResidueSymbol( const std::string & p_symbol ) const
		{
			return _moleculeStruct->getUnknownResidueSymbol( p_symbol );
		}
		int addUnknownResidueSymbol( VTX::Core::ChemDB::UnknownResidueData * const p_data )
		{
			return _moleculeStruct->addUnknownResidueSymbol( p_data );
		}
		inline void addUnknownAtomSymbol( const std::string & p_symbol )
		{
			_moleculeStruct->addUnknownAtomSymbol( p_symbol );
		}

		inline AtomPositionsFrame & addAtomPositionFrame() { return _moleculeStruct->addAtomPositionFrame(); }
		inline void					addAtomPositionFrame( const AtomPositionsFrame & p_frame )
		{
			return _moleculeStruct->addAtomPositionFrame( p_frame );
		}

		inline void setAtomPositionFrames( const std::vector<AtomPositionsFrame> & p_frames )
		{
			return _moleculeStruct->setAtomPositionFrames( p_frames );
		}
		inline const AtomPositionsFrame & getCurrentAtomPositionFrame() const
		{
			return _moleculeStruct->getAtomPositionFrame( _currentFrame );
		}
		inline AtomPositionsFrame & getCurrentAtomPositionFrame()
		{
			return _moleculeStruct->getAtomPositionFrame( _currentFrame );
		}
		inline const AtomPositionsFrame & getAtomPositionFrame( const uint p_frame ) const
		{
			return _moleculeStruct->getAtomPositionFrame( p_frame );
		}
		inline AtomPositionsFrame & getAtomPositionFrame( const uint p_frame )
		{
			return _moleculeStruct->getAtomPositionFrame( p_frame );
		}
		inline const std::vector<AtomPositionsFrame> & getAtomPositionFrames() const
		{
			return _moleculeStruct->getAtomPositionFrames();
		}
		inline std::vector<AtomPositionsFrame> & getAtomPositionFrames()
		{
			return _moleculeStruct->getAtomPositionFrames();
		}

		inline std::vector<float> &					  getBufferAtomRadius() { return _bufferAtomRadius; }
		inline const std::vector<float> &			  getBufferAtomRadius() const { return _bufferAtomRadius; }
		inline std::vector<Util::Color::Rgba> &		  getBufferAtomColors() { return _bufferAtomColors; }
		inline const std::vector<Util::Color::Rgba> & getBufferAtomColors() const { return _bufferAtomColors; }
		inline std::vector<uint> &					  getBufferAtomVisibilities() { return _bufferAtomVisibilities; }
		inline const std::vector<uint> & getBufferAtomVisibilities() const { return _bufferAtomVisibilities; }
		inline std::vector<uint> &		 getBufferAtomSelections() { return _bufferAtomSelections; }
		inline const std::vector<uint> & getBufferAtomSelections() const { return _bufferAtomSelections; }
		inline std::vector<uint> &		 getBufferAtomIds() { return _bufferAtomIds; }
		inline const std::vector<uint> & getBufferAtomIds() const { return _bufferAtomIds; }
		inline std::vector<uint> &		 getBufferBonds() { return _bufferBonds; }
		inline const std::vector<uint> & getBufferBonds() const { return _bufferBonds; }

		void resizeBuffers();

		inline const uint getChainCount() const { return _moleculeStruct->getChainCount(); }
		inline const uint getResidueCount() const { return _moleculeStruct->getResidueCount(); }
		inline const uint getAtomCount() const { return _moleculeStruct->getAtomCount(); }
		inline const uint getBondCount() const { return _moleculeStruct->getBondCount(); }

		void clearDefaultRepresentations();
		bool isDefaultRepresentation(
			const App::Application::Representation::InstantiatedRepresentation & p_representation ) const;
		void removeChildrenRepresentations();

		void refreshStructure();
		void refreshColors();
		void refreshVisibilities();
		void refreshSelection( const App::Application::Selection::SelectionModel::MapChainIds * const );
		void refreshBondsBuffer();

		inline bool									   hasTrajectory() { return _moleculeStruct->hasTrajectory(); }
		inline std::vector<AtomPositionsFrame> &	   getFrames() { return _moleculeStruct->getFrames(); }
		inline const std::vector<AtomPositionsFrame> & getFrames() const { return _moleculeStruct->getFrames(); }
		inline uint									   getFrame() const { return _currentFrame; }
		void										   setFrame( const uint );
		void										   applyNextFrame( const uint p_frameCount = 1 );
		inline const uint		   getFrameCount() const { return _moleculeStruct->getFrameCount(); }
		inline uint				   getFPS() const { return _fps; }
		void					   setFPS( const uint p_fps );
		inline bool				   isPlaying() const { return _isPlaying; }
		void					   setIsPlaying( const bool p_isPlaying );
		inline Chemistry::PlayMode getPlayMode() const { return _playMode; }
		void					   setPlayMode( const Chemistry::PlayMode & p_playMode );

		void updateTrajectory( const float & p_deltaTime );
		void forceNotifyTrajectoryChanged();
		bool isAtEndOfTrajectoryPlay();
		void resetTrajectoryPlay();

		bool showWater() const;
		void setShowWater( const bool p_showWater );
		bool showHydrogen() const;
		void setShowHydrogen( const bool p_showHydrogen );
		bool showSolvent() const;
		void setShowSolvent( const bool p_showSolvent );
		bool showIon() const;
		void setShowIon( const bool p_showIon );

		// At least one residue
		inline bool hasTopology() const { return getResidueCount() > 1; }
		inline bool hasDynamic() const { return getFrameCount() > 1; }

		void print() const;

		void setVisible( const bool );
		void setVisible( const bool p_visible, const bool p_notify );
		void render( const App::Component::Render::Camera & ) const override;

		bool mergeTopology( const Molecule & p_other )
		{
			_moleculeStruct->mergeTopology( p_other.getMoleculeStruct() );
		}

		// Secondary structure.
		void createSecondaryStructure();
		void refreshSecondaryStructure();

		// Solvent excluded surface.
		void createSolventExcludedSurface( const ChemDB::Category::TYPE & p_categoryEnum );
		void refreshSolventExcludedSurfaces();

		// Categorization
		std::vector<Chemistry::Category *> getFilledCategories() const;

		void propagateEventToViews( const App::Core::Event::VTXEvent * const p_event ) { _notifyViews( p_event ); }

		template<typename... Args>
		inline void propagateEventToViews( const App::Core::Event::VTX_EVENT & p_eventID, Args... p_args )
		{
			_notifyViews( p_eventID, p_args... );
		}
		void propagateEventToViews( const App::Core::Event::VTX_EVENT & p_eventID ) { _notifyViews( p_eventID ); }

		const std::string & getDisplayName() const { return _displayName; };
		void				setDisplayName( const std::string & p_name );

		// Hide BaseColorable::setColor
		void setColor( const Util::Color::Rgba & p_color );
		void notifyStructureChange();
		void notifyVisibilityChange();

	  protected:
		void _init() override;
		void _computeAABB() const override;
		void _instantiate3DViews() override;

		void _markRepresentationAsDefault(
			const App::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation );

		void _onRepresentationChange() override;

		Molecule();
		Molecule( const VTX::App::VTX_ID & p_typeId );
		virtual ~Molecule();

	  private:
		VTX::Core::Struct::Molecule * _moleculeStruct = nullptr;
		std::string					  _displayName;

		// Configuration.
		Component::IO::MoleculeConfiguration _configuration = Component::IO::MoleculeConfiguration();

		// Representation.
		RepresentationState _representationState = RepresentationState();

		// Models.
		FilePath	_path;
		std::string _pdbIdCode = "unknown";

		// Options.
		VTX::Core::ChemDB::Color::COLOR_MODE _colorMode = VTX::Core::ChemDB::Color::COLOR_MODE::INHERITED;

		// Buffers.
		std::vector<float>			   _bufferAtomRadius	   = std::vector<float>();
		std::vector<Util::Color::Rgba> _bufferAtomColors	   = std::vector<Util::Color::Rgba>();
		std::vector<uint>			   _bufferAtomVisibilities = std::vector<uint>();
		std::vector<uint>			   _bufferAtomSelections   = std::vector<uint>();
		std::vector<uint>			   _bufferAtomIds		   = std::vector<uint>();
		std::vector<uint>			   _bufferBonds			   = std::vector<uint>();

		// Secondary structure.
		SecondaryStructure * _secondaryStructure = nullptr;
		// Solvent excluded surface.
		std::map<ChemDB::Category::TYPE, SolventExcludedSurface *> _solventExcludedSurfaces
			= std::map<ChemDB::Category::TYPE, SolventExcludedSurface *>();

		// Categories
		std::vector<Chemistry::Category *> _categories;

		// Trajectory
		uint				_currentFrame	  = 0u;
		bool				_isPlaying		  = true;
		uint				_fps			  = 1u;
		Chemistry::PlayMode _playMode		  = Chemistry::PlayMode::Loop;
		uint				_dynamicLoopCount = 0;
		float				_trajectoryTimer  = 0;

		// Element visibility
		bool _showWater	   = true;
		bool _showSolvent  = true;
		bool _showHydrogen = true;
		bool _showIon	   = true;

		std::vector<App::Core::Model::ID> _defaultRepresentationIDs = std::vector<App::Core::Model::ID>();

		// Fill Buffers Functions
		void _fillBufferAtomColors();
		void _fillBufferAtomVisibilities();
		void _fillBufferAtomSelections( const App::Application::Selection::SelectionModel::MapChainIds * const
										= nullptr );
	};
} // namespace VTX::App::Component::Chemistry

#endif
