#ifndef __VTX_APP_CORE_COLLECTIONABLE__
#define __VTX_APP_CORE_COLLECTIONABLE__

#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace VTX::App::Core
{
	class Collectionable
	{
	  public:
		Collectionable()		  = default;
		virtual ~Collectionable() = default;
	};

	class DisplayableCollectionable : public Collectionable
	{
	  public:
		virtual const std::string & getDisplayName() const = 0;
	};
} // namespace VTX::App::Core
#endif
