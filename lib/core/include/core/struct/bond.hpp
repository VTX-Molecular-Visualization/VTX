#ifndef __VTX_CORE_STRUCT_BOND__
#define __VTX_CORE_STRUCT_BOND__

#include "_fwd.hpp"
#include "core/chemdb/bond.hpp"
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	class Bond
	{
	  public:
		Bond() {};

		inline uint getIndexFirstAtom() const { return _indexFirstAtom; }
		inline void setIndexFirstAtom( const uint p_index ) { _indexFirstAtom = p_index; }
		inline uint getIndexSecondAtom() const { return _indexSecondAtom; }
		inline void setIndexSecondAtom( const uint p_index ) { _indexSecondAtom = p_index; }

		inline ChemDB::Bond::ORDER getOrder() const { return _order; }
		inline void				   setOrder( const ChemDB::Bond::ORDER p_order ) { _order = p_order; }

		inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
		inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }

	  private:
		uint				_indexFirstAtom	 = 0;
		uint				_indexSecondAtom = 0;
		Molecule *			_moleculePtr	 = nullptr;
		ChemDB::Bond::ORDER _order			 = ChemDB::Bond::ORDER::UNKNOWN;
	};

} // namespace VTX::Core::Struct
#endif
