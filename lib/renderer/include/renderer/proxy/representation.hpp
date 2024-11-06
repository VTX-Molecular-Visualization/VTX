#ifndef __VTX_RENDERER_PROXY_REPRESENTATION__
#define __VTX_RENDERER_PROXY_REPRESENTATION__

#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX::Renderer::Proxy
{
	/**
	 * @brief Representation data provider.
	 */
	struct Representation
	{
		bool  hasSphere;
		float radiusSphereFixed;
		float radiusSphereAdd;
		bool  radiusFixed;

		bool  hasCylinder;
		float radiusCylinder;
		bool  cylinderColorBlending;

		bool hasRibbon;
		bool ribbonColorBlending;

		bool hasSes = false;

		// TODO: id layout color.

		template<uint S, typename T>
		Util::Callback<const T> & onChange()
		{
			static Util::Callback<const T> callback;
			return callback;
		}
	};

} // namespace VTX::Renderer::Proxy

#endif
