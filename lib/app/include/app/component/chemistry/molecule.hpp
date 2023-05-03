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
#include "app/internal/chemdb/category.hpp"
#include "app/internal/chemdb/unknown_residue_data.hpp"
#include "app/internal/io/reader/prm.hpp"
#include "app/internal/io/reader/psf.hpp"
#include "app/internal/math/range.hpp"
#include "app/render/buffer/molecule.hpp"
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
	namespace ChemDB = App::Internal::ChemDB;

	class Molecule :
		public Core::Model::BaseModel3D<App::Render::Buffer::Molecule>,
		public Component::Generic::BaseColorable,
		public Application::Representation::BaseRepresentable,
		public Core::Scene::BaseSceneItem
	{
		VTX_MODEL

	  public:
		using AtomPositionsFrame  = std::vector<Vec3f>;
		using RepresentationState = std::map<const Application::Representation::InstantiatedRepresentation *,
											 Application::Representation::RepresentationTarget>;

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
		inline const std::string & getName() const { return _name; }
		inline void				   setName( const std::string & p_name ) { _name = p_name; }
		inline const std::string & getPdbIdCode() const { return _pdbIdCode; }
		void					   setPdbIdCode( const std::string & p_pdbId );

		inline const FilePath & getPath() const { return _path; }
		inline void				setPath( const FilePath & p_path ) { _path = p_path; }

		Chain &								addChain();
		inline Chain * const				getChain( const uint p_idx ) { return _chains[ p_idx ]; }
		inline const Chain * const			getChain( const uint p_idx ) const { return _chains[ p_idx ]; }
		Chain *								getFirstChain();
		const Chain * const					getFirstChain() const;
		const Chain * const					getPreviousChain( const uint p_idBaseChain ) const;
		Chain * const						getPreviousChain( const uint p_idBaseChain );
		const Chain * const					getNextChain( const uint p_idBaseChain ) const;
		Chain * const						getNextChain( const uint p_idBaseChain );
		inline std::vector<Chain *> &		getChains() { return _chains; }
		inline const std::vector<Chain *> & getChains() const { return _chains; }
		uint								getRealChainCount() const { return _realChainCount; };
		void								removeChain( const uint p_id,
														 const bool p_delete	  = true,
														 const bool p_recursive	  = true,
														 const bool p_notifyViews = true );

		Residue &							  addResidue();
		inline Residue * const				  getResidue( const uint p_idx ) { return _residues[ p_idx ]; }
		inline const Residue * const		  getResidue( const uint p_idx ) const { return _residues[ p_idx ]; }
		const Residue * const				  getPreviousResidue( const uint p_idBaseResidue ) const;
		Residue * const						  getPreviousResidue( const uint p_idBaseResidue );
		const Residue * const				  getNextResidue( const uint p_idBaseResidue ) const;
		Residue * const						  getNextResidue( const uint p_idBaseResidue );
		inline std::vector<Residue *> &		  getResidues() { return _residues; }
		inline const std::vector<Residue *> & getResidues() const { return _residues; }
		int									  getRealResidueCount() const;
		void								  removeResidue( const uint p_id,
															 const bool p_delete			= true,
															 const bool p_recursive			= true,
															 const bool p_checkParentUpdate = true,
															 const bool p_notifyViews		= true );

		Atom &							   addAtom();
		inline Atom * const				   getAtom( const uint p_idx ) { return _atoms[ p_idx ]; }
		inline const Atom * const		   getAtom( const uint p_idx ) const { return _atoms[ p_idx ]; }
		inline std::vector<Atom *> &	   getAtoms() { return _atoms; }
		inline const std::vector<Atom *> & getAtoms() const { return _atoms; }
		int								   getRealAtomCount() const;
		void							   removeAtom( const uint p_id,
													   const bool p_delete			  = true,
													   const bool p_checkBonds		  = true,
													   const bool p_checkParentUpdate = true,
													   const bool p_notifyViews		  = true );

		Bond &							   addBond();
		inline Bond * const				   getBond( const uint p_idx ) { return _bonds[ p_idx ]; }
		inline const Bond * const		   getBond( const uint p_idx ) const { return _bonds[ p_idx ]; }
		inline std::vector<Bond *> &	   getBonds() { return _bonds; }
		inline const std::vector<Bond *> & getBonds() const { return _bonds; }
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

		bool isEmpty();

		inline const bool isAtomVisible( const uint p_idx ) const { return bool( _bufferAtomVisibilities[ p_idx ] ); }

		inline const float &			 getAtomRadius( const uint p_idx ) const { return _bufferAtomRadius[ p_idx ]; }
		inline const Util::Color::Rgba & getAtomColor( const uint p_idx ) const { return _bufferAtomColors[ p_idx ]; }
		inline const uint getAtomVisibility( const uint p_idx ) const { return _bufferAtomVisibilities[ p_idx ]; }

		inline const std::vector<Internal::ChemDB::UnknownResidueData *> & getUnknownResidueSymbols() const
		{
			return _unknownResidueSymbol;
		}
		inline const std::unordered_set<std::string> & getUnknownAtomSymbols() const { return _unknownAtomSymbol; }

		int											 getUnknownResidueSymbolIndex( const std::string & p_symbol ) const;
		Internal::ChemDB::UnknownResidueData * const getUnknownResidueSymbol( const uint p_symbolIndex ) const;
		Internal::ChemDB::UnknownResidueData * const getUnknownResidueSymbol( const std::string & p_symbol ) const;
		int			addUnknownResidueSymbol( Internal::ChemDB::UnknownResidueData * const p_data );
		inline void addUnknownAtomSymbol( const std::string & p_symbol ) { _unknownAtomSymbol.emplace( p_symbol ); }

		inline AtomPositionsFrame & addAtomPositionFrame()
		{
			_atomPositionsFrames.emplace_back();
			return _atomPositionsFrames.back();
		}
		inline void addAtomPositionFrame( const AtomPositionsFrame & p_frame )
		{
			_atomPositionsFrames.emplace_back( p_frame );
		}

		inline void setAtomPositionFrames( const std::vector<AtomPositionsFrame> & p_frame )
		{
			_atomPositionsFrames.clear();
			_atomPositionsFrames = p_frame;
		}
		inline const AtomPositionsFrame & getCurrentAtomPositionFrame() const
		{
			return _atomPositionsFrames[ _currentFrame ];
		}
		inline AtomPositionsFrame & getCurrentAtomPositionFrame() { return _atomPositionsFrames[ _currentFrame ]; }
		inline const AtomPositionsFrame & getAtomPositionFrame( const uint p_frame ) const
		{
			return _atomPositionsFrames[ p_frame ];
		}
		inline AtomPositionsFrame & getAtomPositionFrame( const uint p_frame )
		{
			return _atomPositionsFrames[ p_frame ];
		}
		inline const std::vector<AtomPositionsFrame> & getAtomPositionFrames() const { return _atomPositionsFrames; }
		inline std::vector<AtomPositionsFrame> &	   getAtomPositionFrames() { return _atomPositionsFrames; }
		inline std::vector<float> &					   getBufferAtomRadius() { return _bufferAtomRadius; }
		inline const std::vector<float> &			   getBufferAtomRadius() const { return _bufferAtomRadius; }
		inline std::vector<Util::Color::Rgba> &		   getBufferAtomColors() { return _bufferAtomColors; }
		inline const std::vector<Util::Color::Rgba> &  getBufferAtomColors() const { return _bufferAtomColors; }
		inline std::vector<uint> &					   getBufferAtomVisibilities() { return _bufferAtomVisibilities; }
		inline const std::vector<uint> & getBufferAtomVisibilities() const { return _bufferAtomVisibilities; }
		inline std::vector<uint> &		 getBufferAtomSelections() { return _bufferAtomSelections; }
		inline const std::vector<uint> & getBufferAtomSelections() const { return _bufferAtomSelections; }
		inline std::vector<uint> &		 getBufferAtomIds() { return _bufferAtomIds; }
		inline const std::vector<uint> & getBufferAtomIds() const { return _bufferAtomIds; }
		inline std::vector<uint> &		 getBufferBonds() { return _bufferBonds; }
		inline const std::vector<uint> & getBufferBonds() const { return _bufferBonds; }

		void resizeBuffers();

		inline const uint getChainCount() const { return uint( _chains.size() ); }
		inline const uint getResidueCount() const { return uint( _residues.size() ); }
		inline const uint getAtomCount() const { return uint( _atoms.size() ); }
		inline const uint getBondCount() const { return uint( _bonds.size() ); }

		void clearDefaultRepresentations();
		bool isDefaultRepresentation(
			const App::Application::Representation::InstantiatedRepresentation & p_representation ) const;
		void removeChildrenRepresentations();

		void refreshStructure();
		void refreshColors();
		void refreshVisibilities();
		void refreshSelection( const App::Application::Selection::SelectionModel::MapChainIds * const );
		void refreshBondsBuffer();

		inline bool									   hasTrajectory() { return _atomPositionsFrames.size() >= 2; }
		inline std::vector<AtomPositionsFrame> &	   getFrames() { return _atomPositionsFrames; }
		inline const std::vector<AtomPositionsFrame> & getFrames() const { return _atomPositionsFrames; }
		inline uint									   getFrame() const { return _currentFrame; }
		void										   setFrame( const uint );
		void										   applyNextFrame( const uint p_frameCount = 1 );
		inline const uint		   getFrameCount() const { return uint( _atomPositionsFrames.size() ); }
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

		bool mergeTopology( const Molecule & );

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

		void _addChain( Chemistry::Chain * const p_chain );

		void _markRepresentationAsDefault(
			const App::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation );

		void _onRepresentationChange() override;

		Molecule();
		Molecule( const VTX::App::VTX_ID & p_typeId );
		virtual ~Molecule();

	  private:
		std::string _displayName;
		uint		_realChainCount = 0;

		// Configuration.
		Component::IO::MoleculeConfiguration _configuration = Component::IO::MoleculeConfiguration();

		// Representation.
		RepresentationState _representationState = RepresentationState();

		// Models.
		FilePath						_path;
		std::string						_name						= "unknown";
		std::string						_pdbIdCode					= "unknown";
		std::vector<Chain *>			_chains						= std::vector<Chain *>();
		std::vector<Residue *>			_residues					= std::vector<Residue *>();
		std::vector<Atom *>				_atoms						= std::vector<Atom *>();
		std::vector<Bond *>				_bonds						= std::vector<Bond *>();
		std::vector<AtomPositionsFrame> _atomPositionsFrames		= std::vector<AtomPositionsFrame>();
		uint							_indexFirstBondExtraResidue = 0;

		// Options.
		App::Internal::ChemDB::Color::COLOR_MODE _colorMode = App::Internal::ChemDB::Color::COLOR_MODE::INHERITED;

		// Missing symbols.
		std::vector<Internal::ChemDB::UnknownResidueData *> _unknownResidueSymbol
			= std::vector<Internal::ChemDB::UnknownResidueData *>();
		std::unordered_set<std::string> _unknownAtomSymbol = std::unordered_set<std::string>();

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

#ifdef _DEBUG
	  public:
		// Validation.
		uint chainCount	  = 0;
		uint residueCount = 0;
		uint atomCount	  = 0;
		uint bondCount	  = 0;
#endif
	};
} // namespace VTX::App::Component::Chemistry

#endif
