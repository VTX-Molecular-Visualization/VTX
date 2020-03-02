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
#include <set>
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

			inline Chain &	 addChain() { return _chains.emplace_back( Chain() ); }
			inline Chain &	 getChain( uint p_idx ) { return _chains[ p_idx ]; }
			inline Residue & addResidue() { return _residues.emplace_back( Residue() ); }
			inline Residue & getResidue( uint p_idx ) { return _residues[ p_idx ]; }
			inline Atom &	 addAtom() { return _atoms.emplace_back( Atom() ); }
			inline Atom &	 getAtom( uint p_idx ) { return _atoms[ p_idx ]; }

			inline std::vector<Chain> &	  getChains() { return _chains; }
			inline std::vector<Residue> & getResidues() { return _residues; }
			inline std::vector<Atom> &	  getAtoms() { return _atoms; }

			inline std::set<std::string> & getUnknownResidueSymbols() { return _unknownResidueSymbol; }
			inline std::set<std::string> & getUnknownAtomSymbols() { return _unknownAtomSymbol; }

			inline void addUnknownResidueSymbol( const std::string & p_symbol )
			{
				_unknownResidueSymbol.emplace( p_symbol );
			}
			inline void addUnknownAtomSymbol( const std::string & p_symbol ) { _unknownAtomSymbol.emplace( p_symbol ); }

			inline Vec3f & addAtomPosition( const Vec3f p_position )
			{
				return _atomPositions.emplace_back( p_position );
			}
			inline std::vector<Vec3f> & getAtomPositions() { return _atomPositions; };
			inline void					addAtomRadius( const float p_radius ) { _atomRadius.emplace_back( p_radius ); }
			inline float				getAtomRadius( const uint p_idx ) { return _atomRadius[ p_idx ]; }
			inline std::vector<float> & getAtomRadius() { return _atomRadius; };
			inline void					addAtomColor( const Vec3f p_color ) { _atomColors.emplace_back( p_color ); }
			inline Vec3f &				getAtomColor( const uint p_idx ) { return _atomColors[ p_idx ]; }
			inline std::vector<Vec3f> & getAtomColors() { return _atomColors; };
			inline void					addBond( const uint p_bond ) { _bonds.emplace_back( p_bond ); }
			inline void					addBonds( const std::vector<int> & p_bonds )
			{
				_bonds.insert( _bonds.end(), p_bonds.begin(), p_bonds.end() );
			}
			inline std::vector<uint> & getBonds() { return _bonds; };
			inline uint				   getChainCount() const { return (uint)_chains.size(); }
			inline uint				   getResidueCount() const { return (uint)_residues.size(); }
			inline uint				   getAtomCount() const { return (uint)_atoms.size(); }
			inline uint				   getBondCount() const { return (uint)_bonds.size(); }

			inline Math::AABB & AABB() { return _aabb; }

			virtual void init() override;
			void		 setRepresentation();
			void		 setColorMode();

			virtual void print() const override;

			Chain * const	getSelectedChain() const { return _selectedChain; }
			Residue * const getSelectedResidue() const { return _selectedResidue; }
			Atom * const	getSelectedAtom() const { return _selectedAtom; }
			void			setSelectedChain( Chain * const );
			void			setSelectedResidue( Residue * const );
			void			setSelectedAtom( Atom * const );
			void			resetSelectedChain();
			void			resetSelectedResidue();
			void			resetSelectedAtom();

			void bindBuffers();
			void unbindBuffers();

			virtual void render() override;

			virtual void setSelected( const bool ) override;

		  protected:
			virtual void _addItems() override final;

		  private:
			// Models.
			std::string			 _name	   = "";
			std::vector<Chain>	 _chains   = std::vector<Chain>();
			std::vector<Residue> _residues = std::vector<Residue>();
			std::vector<Atom>	 _atoms	   = std::vector<Atom>();

			std::set<std::string> _unknownResidueSymbol = std::set<std::string>();
			std::set<std::string> _unknownAtomSymbol	= std::set<std::string>();

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
