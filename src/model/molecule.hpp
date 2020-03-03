#ifndef __VTX_MODEL_MOLECULE__
#define __VTX_MODEL_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "atom.hpp"
#include "base_model.hpp"
#include "chain.hpp"
#include "define.hpp"
#include "generic/base_printable.hpp"
#include "generic/base_renderable.hpp"
#include "generic/base_transformable.hpp"
#include "math/aabb.hpp"
#include "residue.hpp"
#include "util/logger.hpp"
#include <GL/gl3w.h>
#include <glm/gtx/string_cast.hpp>
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
			public Generic::BasePrintable,
			public Generic::BaseTransformable,
			public Generic::BaseRenderable
		{
		  public:
			static uint Molecule::COUNTER;
			Molecule() { _id = COUNTER++; };
			~Molecule();

			// Models.
			inline const std::string & getName() const { return _name; };
			inline void				   setName( const std::string & p_name ) { _name = p_name; };

			inline void			   addChain() { _chains.emplace_back( Chain() ); }
			inline Chain &		   getChain( uint p_idx ) { return _chains[ p_idx ]; }
			inline const Chain &   getChain( uint p_idx ) const { return _chains[ p_idx ]; }
			inline void			   addResidue() { _residues.emplace_back( Residue() ); }
			inline Residue &	   getResidue( uint p_idx ) { return _residues[ p_idx ]; }
			inline const Residue & getResidue( uint p_idx ) const { return _residues[ p_idx ]; }
			inline void			   addAtom() { _atoms.emplace_back( Atom() ); }
			inline Atom &		   getAtom( uint p_idx ) { return _atoms[ p_idx ]; }
			inline const Atom &	   getAtom( uint p_idx ) const { return _atoms[ p_idx ]; }

			inline std::vector<Chain> &			getChains() { return _chains; }
			inline const std::vector<Chain> &	getChains() const { return _chains; }
			inline std::vector<Residue> &		getResidues() { return _residues; }
			inline const std::vector<Residue> & getResidues() const { return _residues; }
			inline std::vector<Atom> &			getAtoms() { return _atoms; }
			inline const std::vector<Atom> &	getAtoms() const { return _atoms; }

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

			inline const Vec3f & getAtomPosition( const uint p_id ) const { return _atomPositions[ p_id ]; }
			inline void addAtomPosition( const Vec3f p_position ) { _atomPositions.emplace_back( p_position ); }
			inline const std::vector<Vec3f> & getAtomPositions() const { return _atomPositions; };
			inline void		   addAtomRadius( const float p_radius ) { _atomRadius.emplace_back( p_radius ); }
			inline const float getAtomRadius( const uint p_idx ) const { return _atomRadius[ p_idx ]; }
			inline const std::vector<float> & getAtomRadius() const { return _atomRadius; };
			inline void			 addAtomColor( const Vec3f p_color ) { _atomColors.emplace_back( p_color ); }
			inline const Vec3f & getAtomColor( const uint p_idx ) const { return _atomColors[ p_idx ]; }
			inline const std::vector<Vec3f> & getAtomColors() const { return _atomColors; };
			inline void						  addBond( const uint p_bond ) { _bonds.emplace_back( p_bond ); }
			inline void						  addBonds( const std::vector<int> & p_bonds )
			{
				_bonds.insert( _bonds.end(), p_bonds.begin(), p_bonds.end() );
			}
			inline const std::vector<uint> & getBonds() const { return _bonds; };
			inline const uint				 getChainCount() const { return (uint)_chains.size(); }
			inline const uint				 getResidueCount() const { return (uint)_residues.size(); }
			inline const uint				 getAtomCount() const { return (uint)_atoms.size(); }
			inline const uint				 getBondCount() const { return (uint)_bonds.size(); }

			inline void resizeChainsVec( const uint s ) { _chains.resize( s ); }
			inline void resizeResiduesVec( const uint s ) { _residues.resize( s ); }
			inline void resizeAtomsVec( const uint s ) { _atoms.resize( s ); }

			inline const Math::AABB & AABB() const { return _aabb; }
			inline void				  extendAABB( const Vec3f & p_position, const float p_radius )
			{
				_aabb.extend( p_position, p_radius );
			}

			virtual void init() override;
			void		 setRepresentation();
			void		 setColorMode();

			virtual void print() const override;

			void bindBuffers();
			void unbindBuffers();

			virtual void render() override;

			virtual void setSelected( const bool ) override;

		  protected:
			virtual void _addItems() override final;

		  private:
		  private:
			// Models.
			std::string			 _name	   = "";
			std::vector<Chain>	 _chains   = std::vector<Chain>();
			std::vector<Residue> _residues = std::vector<Residue>();
			std::vector<Atom>	 _atoms	   = std::vector<Atom>();

			std::unordered_set<std::string> _unknownResidueSymbol = std::unordered_set<std::string>();
			std::unordered_set<std::string> _unknownAtomSymbol	  = std::unordered_set<std::string>();

			// Buffers.
			std::vector<Vec3f> _atomPositions = std::vector<Vec3f>();
			std::vector<float> _atomRadius	  = std::vector<float>();
			std::vector<Vec3f> _atomColors	  = std::vector<Vec3f>();
			std::vector<uint>  _bonds		  = std::vector<uint>();

			Math::AABB _aabb;

			// Selection.
			Chain *	  _selectedChain   = nullptr;
			Residue * _selectedResidue = nullptr;
			Atom *	  _selectedAtom	   = nullptr;

			// OpenGL buffers.
			enum ATTRIBUTE_LOCATION
			{
				ATOM_POSITION = 0,
				ATOM_COLOR	  = 1,
				ATOM_RADIUS	  = 2
			};

			GLuint _vao				 = GL_INVALID_VALUE; // Vao.
			GLuint _atomPositionsVBO = GL_INVALID_VALUE; // Atom positions vbo.
			GLuint _atomRadiusVBO	 = GL_INVALID_VALUE; // Radii vbo.
														 // TODO ? use SSBO ? ok for atom colors (CPK/residue/chain)
														 // but for energy based coloration, useless...
			GLuint _atomColorsVBO = GL_INVALID_VALUE;	 // Color vbo.
			GLuint _bondsIBO	  = GL_INVALID_VALUE;	 // Bonds ibo.

			void _createBuffers();

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
