#ifndef __VTX_MODEL_MOLECULE__
#define __VTX_MODEL_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "IO/reader/prm.hpp"
#include "atom.hpp"
#include "base_model.hpp"
#include "bond.hpp"
#include "chain.hpp"
#include "define.hpp"
#include "generic/base_printable.hpp"
#include "generic/base_renderable.hpp"
#include "generic/base_transformable.hpp"
#include "generic/base_visible.hpp"
#include "math/aabb.hpp"
#include "residue.hpp"
#include "util/logger.hpp"
#include <GL/gl3w.h>
#include <iostream>
#include <unordered_set>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class BaseView3DMolecule;
		class Molecule :
			public BaseModel,
			public Generic::BaseTransformable,
			public Generic::BaseRenderable,
			public Generic::BaseColorable,
			public Generic::BaseVisible
		{
		  public:
			using AtomPositionsFrame = std::vector<Vec3f>;

			Molecule() {};
			~Molecule();

			// Models.
			inline const std::string & getName() const { return _name; };
			inline void				   setName( const std::string & p_name ) { _name = p_name; };
			inline const std::string & getFileName() const { return _fileName; };
			inline void				   setFileName( const std::string & p_fileName ) { _fileName = p_fileName; };
			inline const IO::Reader::PRMFile & getPRM() const { return _prm; };
			inline void						   setPRM( const IO::Reader::PRMFile & p_prm ) { _prm = p_prm; };

			inline void							  addChain() { _chains.emplace_back( new Chain() ); }
			inline Chain &						  getChain( uint p_idx ) { return *_chains[ p_idx ]; }
			inline const Chain &				  getChain( uint p_idx ) const { return *_chains[ p_idx ]; }
			inline std::vector<Chain *> &		  getChains() { return _chains; }
			inline const std::vector<Chain *> &	  getChains() const { return _chains; }
			inline void							  addResidue() { _residues.emplace_back( new Residue() ); }
			inline Residue &					  getResidue( uint p_idx ) { return *_residues[ p_idx ]; }
			inline const Residue &				  getResidue( uint p_idx ) const { return *_residues[ p_idx ]; }
			inline std::vector<Residue *> &		  getResidues() { return _residues; }
			inline const std::vector<Residue *> & getResidues() const { return _residues; }
			inline void							  addAtom() { _atoms.emplace_back( new Atom() ); }
			inline Atom &						  getAtom( uint p_idx ) { return *_atoms[ p_idx ]; }
			inline const Atom &					  getAtom( uint p_idx ) const { return *_atoms[ p_idx ]; }
			inline std::vector<Atom *> &		  getAtoms() { return _atoms; }
			inline const std::vector<Atom *> &	  getAtoms() const { return _atoms; }
			inline void							  addBond() { _bonds.emplace_back( new Bond() ); }
			inline Bond &						  getBond( uint p_idx ) { return *_bonds[ p_idx ]; }
			inline const Bond &					  getBond( uint p_idx ) const { return *_bonds[ p_idx ]; }
			inline std::vector<Bond *> &		  getBonds() { return _bonds; }
			inline const std::vector<Bond *> &	  getBonds() const { return _bonds; }

			inline const float	 getAtomRadius( const uint p_idx ) const { return _bufferAtomRadius[ p_idx ]; }
			inline const Vec3f & getAtomColor( const uint p_idx ) const { return _bufferAtomColors[ p_idx ]; }

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

			inline void addAtomPositionFrame() { _atomPositionsFrames.emplace_back( AtomPositionsFrame() ); }
			inline void addAtomPositionFrame( const AtomPositionsFrame & p_frame )
			{
				_atomPositionsFrames.emplace_back( p_frame );
			}

			inline void seAtomPositionFrames( std::vector<AtomPositionsFrame> & p_frame )
			{
				_atomPositionsFrames.clear();
				_atomPositionsFrames = p_frame;
			}
			inline const AtomPositionsFrame & getAtomPositionFrame( const uint p_frame ) const
			{
				return _atomPositionsFrames[ p_frame ];
			};
			inline AtomPositionsFrame & getAtomPositionFrame( const uint p_frame )
			{
				return _atomPositionsFrames[ p_frame ];
			};
			inline const std::vector<AtomPositionsFrame> & getAtomPositionFrames() const
			{
				return _atomPositionsFrames;
			};
			inline std::vector<AtomPositionsFrame> & getAtomPositionFrames() { return _atomPositionsFrames; };

			inline const uint getChainCount() const { return (uint)_chains.size(); }
			inline const uint getResidueCount() const { return (uint)_residues.size(); }
			inline const uint getAtomCount() const { return (uint)_atoms.size(); }
			inline const uint getBondCount() const { return (uint)_bonds.size(); }

			inline const Math::AABB & getGlobalPositionsAABB() const { return _globalPositionsAABB; }

			virtual void	  init() override;
			void			  setRepresentation();
			void			  setColorMode();
			inline uint		  getFrame() const { return _currentFrame; }
			void			  setFrame( uint );
			inline const uint getFrameCount() const { return (uint)_atomPositionsFrames.size(); }
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

			inline bool hasTopology() const { return getResidueCount() > 1; }
			inline bool hasDynamic() const { return getFrameCount() > 1; }

			virtual void print() const override;

			void bindBuffers();
			void unbindBuffers();

			bool		 mergeTopology( const Molecule & );
			inline void	 refreshVisibility() { _fillBufferAtomVisibilities(); }
			virtual void render() override;
			virtual void setSelected( const bool ) override;
			virtual void setVisible( const bool ) override;

		  protected:
			virtual void _addItems() override final;

			void _computeGlobalPositionsAABB();

		  private:
			// Models.
			std::string						_fileName = "";
			IO::Reader::PRMFile				_prm;
			std::string						_name				 = "unknown";
			std::vector<Chain *>			_chains				 = std::vector<Chain *>();
			std::vector<Residue *>			_residues			 = std::vector<Residue *>();
			std::vector<Atom *>				_atoms				 = std::vector<Atom *>();
			std::vector<Bond *>				_bonds				 = std::vector<Bond *>();
			std::vector<AtomPositionsFrame> _atomPositionsFrames = std::vector<AtomPositionsFrame>();

			std::unordered_set<std::string> _unknownResidueSymbol = std::unordered_set<std::string>();
			std::unordered_set<std::string> _unknownAtomSymbol	  = std::unordered_set<std::string>();

			// Buffers.
			std::vector<float> _bufferAtomRadius	   = std::vector<float>();
			std::vector<Vec3f> _bufferAtomColors	   = std::vector<Vec3f>();
			std::vector<uint>  _bufferAtomVisibilities = std::vector<uint>();
			std::vector<uint>  _bufferBonds			   = std::vector<uint>();

			// Global AABB of atom positions (taking into account each frame).
			// TODO: find better name
			Math::AABB _globalPositionsAABB;

			// OpenGL buffers.
			enum ATTRIBUTE_LOCATION
			{
				ATOM_POSITION	= 0,
				ATOM_COLOR		= 1,
				ATOM_RADIUS		= 2,
				ATOM_VISIBILITY = 3,
			};

			GLuint _vao					= GL_INVALID_VALUE;
			GLuint _atomPositionsVBO	= GL_INVALID_VALUE;
			GLuint _atomRadiusVBO		= GL_INVALID_VALUE;
			GLuint _atomColorsVBO		= GL_INVALID_VALUE;
			GLuint _atomVisibilitiesVBO = GL_INVALID_VALUE;
			GLuint _bondsIBO			= GL_INVALID_VALUE;

			uint _currentFrame = 0u;
			bool _isPlaying	   = true;
			uint _fps		   = 1u;

			bool _showSolvent = true;
			bool _showIon	  = true;

			void _createBuffers();
			void _initBufferAtomPositions() const;
			void _updateBufferAtomPositions() const;
			void _fillBufferAtomRadius();
			void _fillBufferAtomColors();
			void _fillBufferAtomVisibilities();
			void _fillBufferBonds();

			void _computeSecondaryStructure();

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
