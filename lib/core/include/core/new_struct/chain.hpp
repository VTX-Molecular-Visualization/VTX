#ifndef __VTX_CORE_NEW_STRUCT_CHAIN__
#define __VTX_CORE_NEW_STRUCT_CHAIN__

#include "concept.hpp"
#include "core/define.hpp"
#include "define.hpp"
#include <string>

namespace VTX::Core::NewStruct
{
	template<ConceptMolecule M>
	class Chain
	{
	  public:
		Chain() = default;
		Chain( M * const p_molecule, const size_t p_internalIndex ) :
			_molecule( p_molecule ), _internalIndex( p_internalIndex )
		{
		}

		const std::string & getName() const { return _name; }
		void				setName( const std::string & p_name ) { _name = p_name; }

		const size_t getIndex() const { return _internalIndex; }
		void		 setIndex( const size_t p_index ) const { _internalIndex = p_index; }

		M * const		getMoleculePtr() const { return _molecule; };
		const M * const getConstMoleculePtr() const { return _molecule; };
		void			setMoleculePtr( M * const p_molecule ) { _molecule = p_molecule; };

		const size_t getIndexFirstResidue() const { return _indexFirstResidue; };
		void		 setIndexFirstResidue( const size_t p_residueIndex ) { _indexFirstResidue = p_residueIndex; };

		const size_t getResidueCount() const { return _residueCount; };
		void		 setResidueCount( const size_t p_residueCount ) { _residueCount = p_residueCount; };

	  protected:
		size_t _internalIndex = INVALID_INDEX;
		M *	   _molecule;

		std::string _name			   = "unknown";
		size_t		_indexFirstResidue = 0;
		size_t		_residueCount	   = 0;

		// std::string _originalChainID = "";
		// uint		_realResidueCount  = 0;
	};

} // namespace VTX::Core::NewStruct
#endif
