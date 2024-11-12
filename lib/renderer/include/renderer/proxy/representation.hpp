#ifndef __VTX_RENDERER_PROXY_REPRESENTATION__
#define __VTX_RENDERER_PROXY_REPRESENTATION__

#include "proxy_pointer_collection.hpp"

namespace VTX::Renderer::Proxy
{
	enum E_REPRESENTATION_SETTINGS
	{
		HAS_SPHERE,
		RADIUS_SPHERE_FIXED,
		RADIUS_SPHERE_ADD,
		RADIUS_FIXED,

		HAS_CYLINDER,
		RADIUS_CYLINDER,
		CYLINDER_COLOR_BLENDING,

		HAS_RIBBON,
		RIBBON_COLOR_BLENDING,

		HAS_SES
	};

	/**
	 * @brief Representation data provider.
	 */
	class Representation : public ProxyPointerCollection
	{
	  public:
		template<E_REPRESENTATION_SETTINGS, typename... Args>
		Util::Callback<Args...> & onChange()
		{
			static Util::Callback<Args...> callback;
			return callback;
		}
	};

} // namespace VTX::Renderer::Proxy

#endif
