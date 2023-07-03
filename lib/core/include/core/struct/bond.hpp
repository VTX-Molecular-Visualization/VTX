#ifndef __VTX_CORE_NEW_STRUCT_BOND__
#define __VTX_CORE_NEW_STRUCT_BOND__

#include "_fwd.hpp"
#include "core/chemdb/bond.hpp"
#include <util/exceptions.hpp>

namespace VTX::Core::Struct
{
	template<ConceptMolecule M>
	class Bond
	{
	  public:
		Bond() {}
		Bond( M * const p_moleculePtr ) : _moleculePtr( p_moleculePtr ) {}
		Bond( M * const p_moleculePtr, const size_t p_internalIndex ) :
			_moleculePtr( p_moleculePtr ), _internalIndex( p_internalIndex )
		{
		}

		size_t getIndex() const { return _internalIndex; };
		void   setIndex( const size_t p_index ) { _internalIndex = p_index; };

		M *		  getMoleculePtr() const { return _moleculePtr; };
		const M * getConstMoleculePtr() const { return _moleculePtr; };
		void	  setMoleculePtr( M * const p_moleculePtr ) { _moleculePtr = p_moleculePtr; };

		ChemDB::Bond::ORDER getOrder() const { return _order; };
		void				setOrder( const ChemDB::Bond::ORDER p_order ) { _order = p_order; };

		size_t getIndexFirstAtom() const
		{ /*throw NotImplementedException();*/
		}
		void setIndexFirstAtom( const size_t p_atomIndex )
		{ /*throw NotImplementedException();*/
		}
		size_t getIndexSecondAtom() const
		{ /*throw NotImplementedException();*/
		}
		void setIndexSecondAtom( const size_t p_atomIndex )
		{ /*throw NotImplementedException();*/
		}

	  protected:
		size_t _internalIndex = INVALID_INDEX;

		M *					_moleculePtr = nullptr;
		ChemDB::Bond::ORDER _order		 = ChemDB::Bond::ORDER::UNKNOWN;
	};

} // namespace VTX::Core::Struct
#endif
