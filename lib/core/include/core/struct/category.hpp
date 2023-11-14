#ifndef __VTX_CORE_STRUCT_CATEGORY__
#define __VTX_CORE_STRUCT_CATEGORY__

#include "core/chemdb/category.hpp"
#include <algorithm>
#include <vector>

namespace VTX::Core::Struct
{
	class Category
	{
	  public:
		Category() {}
		Category( const ChemDB::Category::TYPE & p_type ) : _categoryEnum( p_type ) {}

		void referenceChain( const size_t p_chainIndex ) { _linkedChains.emplace_back( p_chainIndex ); }
		void removeChain( const size_t p_chainIndex )
		{
			_linkedChains.erase( std::find( _linkedChains.begin(), _linkedChains.end(), p_chainIndex ) );
		}

		const ChemDB::Category::TYPE & getCategory() { return _categoryEnum; }
		const std::vector<size_t> &	   getLinkedChains() { return _linkedChains; }

	  private:
		ChemDB::Category::TYPE _categoryEnum = ChemDB::Category::TYPE::UNKNOWN;
		std::vector<size_t>	   _linkedChains = std::vector<size_t>();
	};

} // namespace VTX::Core::Struct
#endif
