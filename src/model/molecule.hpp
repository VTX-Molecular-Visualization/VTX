#ifndef __VTX_MODEL_MOLECULE__
#define __VTX_MODEL_MOLECULE__

#include "base_model_3d.hpp"
#include "buffer/molecule.hpp"
#include "color/rgb.hpp"
#include "define.hpp"
#include "generic/base_representable.hpp"
#include "generic/base_scene_item.hpp"
#include "io/reader/prm.hpp"
#include "io/reader/psf.hpp"
#include "model/configuration/molecule.hpp"
#include "model/secondary_structure.hpp"
#include "model/selection.hpp"
#include "model/solvent_excluded_surface.hpp"
#include "object3d/helper/aabb.hpp"
#include "representation/instantiated_representation.hpp"
#include "representation/representation_target.hpp"
#include "trajectory/trajectory_enum.hpp"
#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace VTX
{
	namespace Model
	{
		struct UnknownResidueData
		{
			std::string symbolStr;
			std::string symbolName;
		};

		class Chain;
		class Residue;
		class Atom;
		class Bond;
		class Molecule :
			public BaseModel3D<Buffer::Molecule>,
			public Generic::BaseColorable,
			public Generic::BaseRepresentable,
			public Generic::BaseSceneItem
		{
			VTX_MODEL

		  public:
			using AtomPositionsFrame  = std::vector<Vec3f>;
			using RepresentationState = std::map<const Model::Representation::InstantiatedRepresentation *,
												 VTX::Representation::RepresentationTarget>;

			// Configuration.
			inline const Configuration::Molecule & getConfiguration() const { return _configuration; }
			inline Configuration::Molecule &	   getConfiguration() { return _configuration; }
			inline void setConfiguration( const Configuration::Molecule & p_config ) { _configuration = p_config; }

			// BaseSceneItem
			const Model::ID & getModelID() const override { return getId(); }

			// Representation.
			inline const RepresentationState & getRepresentationState() const { return _representationState; }
			inline RepresentationState &	   getRepresentationState() { return _representationState; }

			// Models.
			inline const std::string & getName() const { return _name; }
			inline void				   setName( const std::string & p_name ) { _name = p_name; }
			inline const std::string & getPdbIdCode() const { return _pdbIdCode; }
			void					   setPdbIdCode( const std::string & p_pdbId );

			inline const IO::FilePath & getPath() const { return _path; }
			inline void					setPath( const IO::FilePath & p_path ) { _path = p_path; }

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
			inline const bool hasSolventExcludedSurface() const { return _solventExcludedSurface != nullptr; }
			inline const SolventExcludedSurface & getSolventExcludedSurface() const { return *_solventExcludedSurface; }
			inline SolventExcludedSurface &		  getSolventExcludedSurface() { return *_solventExcludedSurface; }

			bool isEmpty();

			inline const bool isAtomVisible( const uint p_idx ) const
			{
				return bool( _bufferAtomVisibilities[ p_idx ] );
			}

			inline const float &	  getAtomRadius( const uint p_idx ) const { return _bufferAtomRadius[ p_idx ]; }
			inline const Color::Rgb & getAtomColor( const uint p_idx ) const { return _bufferAtomColors[ p_idx ]; }

			inline const std::vector<UnknownResidueData> & getUnknownResidueSymbols() const
			{
				return _unknownResidueSymbol;
			}
			inline const std::unordered_set<std::string> & getUnknownAtomSymbols() const { return _unknownAtomSymbol; }

			int			addUnknownResidueSymbol( const UnknownResidueData & p_data );
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
			inline const std::vector<AtomPositionsFrame> & getAtomPositionFrames() const
			{
				return _atomPositionsFrames;
			}
			inline std::vector<AtomPositionsFrame> & getAtomPositionFrames() { return _atomPositionsFrames; }
			inline std::vector<float> &				 getBufferAtomRadius() { return _bufferAtomRadius; }
			inline const std::vector<float> &		 getBufferAtomRadius() const { return _bufferAtomRadius; }
			inline std::vector<Color::Rgb> &		 getBufferAtomColors() { return _bufferAtomColors; }
			inline const std::vector<Color::Rgb> &	 getBufferAtomColors() const { return _bufferAtomColors; }
			inline std::vector<uint> &				 getBufferAtomVisibilities() { return _bufferAtomVisibilities; }
			inline const std::vector<uint> & getBufferAtomVisibilities() const { return _bufferAtomVisibilities; }
			inline std::vector<uint> &		 getBufferAtomSelections() { return _bufferAtomSelections; }
			inline const std::vector<uint> & getBufferAtomSelections() const { return _bufferAtomSelections; }
			inline std::vector<uint> &		 getBufferAtomIds() { return _bufferAtomIds; }
			inline const std::vector<uint> & getBufferAtomIds() const { return _bufferAtomIds; }
			inline std::vector<uint> &		 getBufferBonds() { return _bufferBonds; }
			inline const std::vector<uint> & getBufferBonds() const { return _bufferBonds; }

			inline const uint getChainCount() const { return uint( _chains.size() ); }
			inline const uint getResidueCount() const { return uint( _residues.size() ); }
			inline const uint getAtomCount() const { return uint( _atoms.size() ); }
			inline const uint getBondCount() const { return uint( _bonds.size() ); }

			void removeChildrenRepresentations() const;

			inline void refreshColors()
			{
				_fillBufferAtomColors();
				if ( _secondaryStructure != nullptr )
					_secondaryStructure->refreshColors();
				if ( _solventExcludedSurface != nullptr )
					_solventExcludedSurface->refreshColors();
			}
			inline void refreshVisibilities( const bool p_applyBuffer = true )
			{
				_fillBufferAtomVisibilities( p_applyBuffer );
				if ( _secondaryStructure != nullptr )
					_secondaryStructure->refreshVisibilities();
				if ( _solventExcludedSurface != nullptr )
					_solventExcludedSurface->refreshVisibilities();
			}
			inline void refreshStructure()
			{
				_buffer->setAtomVisibilities( _bufferAtomVisibilities );
				refreshBondsBuffer();
				refreshSecondaryStructure();
				refreshSolventExcludedSurface();
				computeAllRepresentationData();
			}

			void refreshSelection( const Selection::MapChainIds * const );
			void refreshBondsBuffer();

			inline bool									   hasTrajectory() { return _atomPositionsFrames.size() >= 2; }
			inline std::vector<AtomPositionsFrame> &	   getFrames() { return _atomPositionsFrames; }
			inline const std::vector<AtomPositionsFrame> & getFrames() const { return _atomPositionsFrames; }
			inline uint									   getFrame() const { return _currentFrame; }
			void										   setFrame( const uint );
			void										   applyNextFrame( const uint p_frameCount = 1 );
			inline const uint			getFrameCount() const { return uint( _atomPositionsFrames.size() ); }
			inline uint					getFPS() const { return _fps; }
			void						setFPS( const uint p_fps );
			inline bool					isPlaying() const { return _isPlaying; }
			void						setIsPlaying( const bool p_isPlaying );
			inline Trajectory::PlayMode getPlayMode() const { return _playMode; }
			void						setPlayMode( const Trajectory::PlayMode & p_playMode );

			void updateTrajectory( const float & p_deltaTime );
			void forceNotifyTrajectoryChanged();
			bool isAtEndOfTrajectoryPlay();
			void resetTrajectoryPlay();

			inline bool showWater() const { return _showWater; }
			void		setShowWater( const bool p_showWater );
			inline bool showHydrogen() const { return _showHydrogen; }
			void		setShowHydrogen( const bool p_showHydrogen );
			inline bool showSolvent() const { return _showSolvent; }
			void		setShowSolvent( const bool p_showSolvent );
			inline bool showIon() const { return _showIon; }
			void		setShowIon( const bool p_showIon );

			// At least one residue
			inline bool hasTopology() const { return getResidueCount() > 1; }
			inline bool hasDynamic() const { return getFrameCount() > 1; }

			void print() const;

			void setVisible( const bool );
			void setVisible( const bool p_visible, const bool p_notify );
			void render( const Object3D::Camera & ) const override;

			bool mergeTopology( const Molecule & );

			// Secondary structure.
			void createSecondaryStructure();
			void refreshSecondaryStructure();

			// Solvent excluded surface.
			void createSolventExcludedSurface();
			void refreshSolventExcludedSurface();

			void propagateEventToViews( const Event::VTXEvent * const p_event ) { _notifyViews( p_event ); }

			const std::string & getDisplayName() const { return _displayName; };
			void				setDisplayName( const std::string & p_name );

			// Hide BaseColorable::setColor
			void setColor( const Color::Rgb & p_color );
			void notifyStructureChange();
			void notifyVisibilityChange();

		  protected:
			void _init() override;
			void _computeAABB() const override;
			void _instantiate3DViews() override;

			void _addChain( Model::Chain * const p_chain );

			void _onRepresentationChange() override;

			Molecule();
			Molecule( const VTX::ID::VTX_ID & p_typeId );
			virtual ~Molecule();

		  private:
			std::string _displayName;
			uint		_realChainCount = 0;

			// Configuration.
			Configuration::Molecule _configuration = Configuration::Molecule();

			// Representation.
			RepresentationState _representationState = RepresentationState();

			// Models.
			IO::FilePath					_path;
			std::string						_name						= "unknown";
			std::string						_pdbIdCode					= "unknown";
			std::vector<Chain *>			_chains						= std::vector<Chain *>();
			std::vector<Residue *>			_residues					= std::vector<Residue *>();
			std::vector<Atom *>				_atoms						= std::vector<Atom *>();
			std::vector<Bond *>				_bonds						= std::vector<Bond *>();
			std::vector<AtomPositionsFrame> _atomPositionsFrames		= std::vector<AtomPositionsFrame>();
			uint							_indexFirstBondExtraResidue = 0;

			// Options.
			Generic::COLOR_MODE _colorMode = Generic::COLOR_MODE::INHERITED;

			// Missing symbols.
			std::vector<UnknownResidueData> _unknownResidueSymbol = std::vector<UnknownResidueData>();
			std::unordered_set<std::string> _unknownAtomSymbol	  = std::unordered_set<std::string>();

			std::vector<float>		_bufferAtomRadius		= std::vector<float>();
			std::vector<Color::Rgb> _bufferAtomColors		= std::vector<Color::Rgb>();
			std::vector<uint>		_bufferAtomVisibilities = std::vector<uint>();
			std::vector<uint>		_bufferAtomSelections	= std::vector<uint>();
			std::vector<uint>		_bufferAtomIds			= std::vector<uint>();
			std::vector<uint>		_bufferBonds			= std::vector<uint>();

			// Secondary structure.
			SecondaryStructure * _secondaryStructure = nullptr;
			// Solvent excluded surface.
			SolventExcludedSurface * _solventExcludedSurface = nullptr;

			// Trajectory
			uint				 _currentFrame	   = 0u;
			bool				 _isPlaying		   = true;
			uint				 _fps			   = 1u;
			Trajectory::PlayMode _playMode		   = Trajectory::PlayMode::Loop;
			uint				 _dynamicLoopCount = 0;
			float				 _trajectoryTimer  = 0;

			// Element visibility
			bool _showWater	   = true;
			bool _showSolvent  = true;
			bool _showHydrogen = true;
			bool _showIon	   = true;

			void _fillBufferAtomColors();
			void _fillBufferAtomVisibilities( const bool p_applyToBuffer = true );
			void _fillBufferAtomSelections( const Selection::MapChainIds * const = nullptr );

#ifdef _DEBUG
		  public:
			// Validation.
			uint chainCount	  = 0;
			uint residueCount = 0;
			uint atomCount	  = 0;
			uint bondCount	  = 0;
#endif
		};
	} // namespace Model
} // namespace VTX

#endif
