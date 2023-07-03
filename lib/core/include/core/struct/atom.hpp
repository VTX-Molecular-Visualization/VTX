#ifndef __VTX_CORE_NEW_STRUCT_ATOM__
#define __VTX_CORE_NEW_STRUCT_ATOM__

#include "_fwd.hpp"
#include "core/chemdb/atom.hpp"
#include <string>
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	template<ConceptMolecule M, ConceptResidue R>
	class Atom
	{
	  public:
		Atom() = default;
		Atom( R * const p_residue ) : _moleculePtr( _residuePtr->getMoleculePtr() ), _residuePtr( p_residue ) {};
		Atom( M * const p_molecule ) : _moleculePtr( p_molecule ) {};
		Atom( M * const p_molecule, const size_t p_internalIndex ) :
			_moleculePtr( p_molecule ), _internalIndex( p_internalIndex ) {};

		size_t						 getIndex() const { return _internalIndex; }
		void						 setIndex( const size_t p_index ) { _internalIndex = p_index; }
		R *							 getResiduePtr() const { return _residuePtr; }
		const R *					 getConstResiduePtr() const { return _residuePtr; }
		void						 setResiduePtr( R * p_residue ) { _residuePtr = p_residue; }
		const std::string &			 getName() const { return _name; }
		void						 setName( const std::string & p_name ) { _name = p_name; }
		const ChemDB::Atom::SYMBOL & getSymbol() const { return _symbol; }
		void						 setSymbol( const ChemDB::Atom::SYMBOL & p_symbol ) { _symbol = p_symbol; }

		const float getVdwRadius() const;

		inline const Vec3f & getLocalPosition() const
		{
			return getLocalPosition( _moleculePtr->trajectory.currentFrameIndex );
		}
		inline const Vec3f & getLocalPosition( const size_t & p_frameIndex ) const
		{
			return _moleculePtr->trajectory.frames[ p_frameIndex ][ _internalIndex ];
		}
		inline const Vec3f getWorldPosition() const
		{
			return Vec3f( Vec4f( getLocalPosition(), 0 ) * _moleculePtr->gpuStruct.transform );
		}
		inline const Vec3f getWorldPosition( const size_t & p_frameIndex ) const
		{
			return Vec3f( Vec4f( getLocalPosition( p_frameIndex ), 0 ) * _moleculePtr->gpuStruct.transform );
		}

		inline const bool isVisible() { return _moleculePtr->gpuStruct.atomVisibilities[ _internalIndex ]; }
		inline const void setVisible( const bool p_visible )
		{
			_moleculePtr->gpuStruct.atomVisibilities[ _internalIndex ] = p_visible;
		}

	  protected:
		// Store moleculePtr or use residuePtr->getChainPtr()->getMoleculePtr() ?
		size_t _internalIndex = INVALID_INDEX;
		M *	   _moleculePtr	  = nullptr;
		R *	   _residuePtr	  = nullptr;

		// /!\ Names PDB != MMTF (CA and C1 for alpha carbon).
		std::string			 _name	 = "";
		ChemDB::Atom::SYMBOL _symbol = ChemDB::Atom::SYMBOL::UNKNOWN; // Move this in GPU dataset ?

																	  // public:
		//  Move this
		//  ChemDB::Atom::TYPE _type = ChemDB::Atom::TYPE::NORMAL;

		// inline const uint	   getIndex() const { return internalIndex; };
		// inline void			   setIndex( const size_t p_index ) { internalIndex = p_index; };
		// Molecule * const	   getMoleculePtr() const { return moleculePtr; }
		// Chain * const		   getChainPtr() const;
		// inline Residue * const getResiduePtr() const { return residuePtr; }
		// inline void			   setResiduePtr( Residue * const p_residue ) { residuePtr = p_residue; }

		// inline const ChemDB::Atom::SYMBOL getSymbol() const { return symbol; };
		// inline void						  setSymbol( const ChemDB::Atom::SYMBOL p_symbol ) { symbol = p_symbol; };

		// inline const Util::Color::Rgba & getColor() const { return moleculePtr->atomColors[ internalIndex ]; };
		// inline const std::string &		  getSymbolStr() const { return ChemDB::Atom::SYMBOL_STR[ int( symbol ) ];
		// };
		// inline const std::string &		  getSymbolName() const { return ChemDB::Atom::SYMBOL_NAME[ int( symbol ) ];
		// }
		// inline const uint				  getAtomicNumber() const { return uint( symbol ); }

		// inline ChemDB::Atom::TYPE		  getType() const { return _type; }
		// inline void						  setType( const ChemDB::Atom::TYPE p_type ) { _type = p_type; }
		// inline const std::string & getName() const { return name; };
		// inline void				   setName( const std::string & p_name ) { name = p_name; };
	};

} // namespace VTX::Core::Struct
#endif
