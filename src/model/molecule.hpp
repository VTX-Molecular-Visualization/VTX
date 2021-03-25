#ifndef __VTX_MODEL_MOLECULE__
#define __VTX_MODEL_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model_3d.hpp"
#include "buffer/molecule.hpp"
#include "color/rgb.hpp"
#include "define.hpp"
#include "generic/base_representable.hpp"
#include "io/reader/prm.hpp"
#include "io/reader/psf.hpp"
#include "math/aabb.hpp"
#include "model/configuration/molecule.hpp"
#include "model/selection.hpp"
#include "representation/instantiated_representation.hpp"
#include "representation/representation_target.hpp"
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
		class Chain;
		class Residue;
		class Atom;
		class Bond;
		class SecondaryStructure;
		class Molecule :
			public BaseModel3D<Buffer::Molecule>,
			public Generic::BaseColorable,
			public Generic::BaseRepresentable
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

			// Representation.
			inline const RepresentationState & getRepresentationState() const { return _representationState; }
			inline RepresentationState &	   getRepresentationState() { return _representationState; }

			// Models.
			inline const std::string & getName() const { return _name; }
			inline void				   setName( const std::string & p_name ) { _name = p_name; }
			inline const std::string & getPdbIdCode() const { return _pdbIdCode; }
			void					   setPdbIdCode( const std::string & p_pdbId );

			inline const VTX::FilePath & getPath() const { return _path; }
			inline void					 setPath( const VTX::FilePath & p_path ) { _path = p_path; }

			Chain &								addChain();
			inline Chain * const				getChain( const uint p_idx ) { return _chains[ p_idx ]; }
			inline const Chain * const			getChain( const uint p_idx ) const { return _chains[ p_idx ]; }
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

			inline const SecondaryStructure & getSecondaryStructure() const { return *_secondaryStructure; }
			inline SecondaryStructure &		  getSecondaryStructure() { return *_secondaryStructure; }

			bool isEmpty();

			inline const bool isAtomVisible( const uint p_idx ) const
			{
				return bool( _bufferAtomVisibilities[ p_idx ] );
			}

			inline const float &	  getAtomRadius( const uint p_idx ) const { return _bufferAtomRadius[ p_idx ]; }
			inline const Color::Rgb & getAtomColor( const uint p_idx ) const { return _bufferAtomColors[ p_idx ]; }

			inline const std::unordered_set<std::string> & getUnknownResidueSymbols() const
			{
				return _unknownResidueSymbol;
			}
			inline const std::unordered_set<std::string> & getUnknownAtomSymbols() const { return _unknownAtomSymbol; }

			inline void addUnknownResidueSymbol( const std::string & p_symbol )
			{
				_unknownResidueSymbol.emplace( p_symbol );
			}
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
			inline std::vector<ushort> &			 getBufferAtomVisibilities() { return _bufferAtomVisibilities; }
			inline const std::vector<ushort> & getBufferAtomVisibilities() const { return _bufferAtomVisibilities; }
			inline std::vector<ushort> &	   getBufferAtomSelection() { return _bufferAtomSelection; }
			inline const std::vector<ushort> & getBufferAtomSelection() const { return _bufferAtomSelection; }
			inline std::vector<uint> &		   getBufferBonds() { return _bufferBonds; }
			inline const std::vector<uint> &   getBufferBonds() const { return _bufferBonds; }

			inline const uint getChainCount() const { return uint( _chains.size() ); }
			inline const uint getResidueCount() const { return uint( _residues.size() ); }
			inline const uint getAtomCount() const { return uint( _atoms.size() ); }
			inline const uint getBondCount() const { return uint( _bonds.size() ); }

			void applyRepresentation( Generic::BaseRepresentable::InstantiatedRepresentation * const p_representation );
			void removeRepresentation();
			void removeChildrenRepresentations() const;

			inline void refreshColors() { _fillBufferAtomColors(); }
			void		refreshSelection( const Selection::MapChainIds * const );
			void		refreshBondsBuffer();

			inline std::vector<AtomPositionsFrame> &	   getFrames() { return _atomPositionsFrames; }
			inline const std::vector<AtomPositionsFrame> & getFrames() const { return _atomPositionsFrames; }
			inline uint									   getFrame() const { return _currentFrame; }
			void										   setFrame( const uint );
			inline const uint getFrameCount() const { return uint( _atomPositionsFrames.size() ); }
			inline uint		  getFPS() const { return _fps; }
			void			  setFPS( const uint p_fps ) { _fps = p_fps; }
			inline bool		  isPlaying() const { return _isPlaying; }
			inline void		  setIsPlaying( const bool p_isPlaying ) { _isPlaying = p_isPlaying; }
			inline bool		  showSolvent() const { return _showSolvent; }
			inline void		  setShowSolvent( const bool p_showSolvent )
			{
				_showSolvent = p_showSolvent;
				_fillBufferAtomVisibilities();
			}
			inline bool showIon() const { return _showIon; }
			inline void setShowIon( const bool p_showIon )
			{
				_showIon = p_showIon;
				_fillBufferAtomVisibilities();
			}

			// At least one residue
			inline bool hasTopology() const { return getResidueCount() > 1; }
			inline bool hasDynamic() const { return getFrameCount() > 1; }

			void print() const;

			void setVisible( const bool );
			void render( const Object3D::Camera & ) override;

			bool mergeTopology( const Molecule & );

			void createSecondaryStructure();
			void propagateEventToViews( const Event::VTXEvent * const p_event ) { _notifyViews( p_event ); }

			const std::string & getDisplayName() const { return _displayName; };
			void				setDisplayName( const std::string & p_name );

		  protected:
			void _init() override;
			void _fillBuffer() override;
			void _computeAABB() const override;
			void _fillBufferAABB() override;
			void _instantiate3DViews() override;

			Molecule();
			Molecule( const ID::VTX_ID & p_typeId );
			~Molecule();

		  private:
			std::string _displayName;
			uint		_realChainCount = 0;

			// Configuration.
			Configuration::Molecule _configuration = Configuration::Molecule();

			// Representation.
			RepresentationState _representationState = RepresentationState();

			// Models.
			VTX::FilePath					_path;
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
			std::unordered_set<std::string> _unknownResidueSymbol = std::unordered_set<std::string>();
			std::unordered_set<std::string> _unknownAtomSymbol	  = std::unordered_set<std::string>();

			std::vector<float>		_bufferAtomRadius		= std::vector<float>();
			std::vector<Color::Rgb> _bufferAtomColors		= std::vector<Color::Rgb>();
			std::vector<ushort>		_bufferAtomVisibilities = std::vector<ushort>();
			std::vector<ushort>		_bufferAtomSelection	= std::vector<ushort>();
			std::vector<uint>		_bufferBonds			= std::vector<uint>();

			// Secondary structure.
			SecondaryStructure * _secondaryStructure = nullptr;

			uint _currentFrame = 0u;
			bool _isPlaying	   = true;
			uint _fps		   = 1u;

			bool _showSolvent = true;
			bool _showIon	  = true;

			void _fillBufferAtomColors();
			void _fillBufferAtomVisibilities();
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
