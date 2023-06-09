#ifndef __VTX_CORE_STRUCT_ATOM__
#define __VTX_CORE_STRUCT_ATOM__

#include "_fwd.hpp"
#include "core/chemdb/atom.hpp"
#include "core/define.hpp"
#include <string>
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	class Atom
	{
	  public:
		// Store moleculePtr or use residuePtr->getChainPtr()->getMoleculePtr() ?
		size_t	   internalIndex = INVALID_INDEX;
		Molecule * moleculePtr	 = nullptr;
		Residue *  residuePtr	 = nullptr;

		// /!\ Names PDB != MMTF (CA and C1 for alpha carbon).
		std::string			 name	= "";
		ChemDB::Atom::SYMBOL symbol = ChemDB::Atom::SYMBOL::UNKNOWN; // Move this in GPU dataset ?

		const float getVdwRadius() const;

		const Vec3f & getLocalPosition() const;
		const Vec3f & getLocalPosition( const size_t & p_frameIndex ) const;
		const Vec3f	  getWorldPosition() const;
		const Vec3f	  getWorldPosition( const size_t & p_frameIndex ) const;

		const bool isVisible();
		const void setVisible( const bool p_visible );

		// Move this
		// ChemDB::Atom::TYPE _type = ChemDB::Atom::TYPE::NORMAL;

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
