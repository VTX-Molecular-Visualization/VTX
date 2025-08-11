#ifndef __VTX_RENDERER_PROXY_REPRESENTATION__
#define __VTX_RENDERER_PROXY_REPRESENTATION__

#include "proxy_pointer_collection.hpp"
#include <util/callback.hpp>

namespace VTX::Renderer::Proxy
{

	/**
	 * @brief Representation data provider.
	 */
	class Representation : public ProxyPointerCollection
	{
	  public:
		template<int, typename... Args>
		Util::Callback<Args...> & onChange()
		{
			static Util::Callback<Args...> callback;
			return callback;
		}
	};

} // namespace VTX::Renderer::Proxy

#endif
