#ifndef __VTX_APP_CORE_DISPLAYABLE_COLLECTION__
#define __VTX_APP_CORE_DISPLAYABLE_COLLECTION__

#include "collection.hpp"
#include "collectionable.hpp"
#include <string>

namespace VTX::App::Core
{
	template<typename T>
	concept DisplayableCollectionableConcept
		= CollectionableConcept<T> && std::derived_from<T, DisplayableCollectionable>;

	template<DisplayableCollectionableConcept T>
	class DisplayableCollection : public Collection<T>
	{
	};
} // namespace VTX::App::Core
#endif
