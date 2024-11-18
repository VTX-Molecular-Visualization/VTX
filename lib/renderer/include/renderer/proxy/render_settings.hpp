#ifndef __VTX_RENDERER_PROXY_RENDER_SETTINGS__
#define __VTX_RENDERER_PROXY_RENDER_SETTINGS__

#include "proxy_pointer_collection.hpp"

namespace VTX::Renderer::Proxy
{
	enum E_RENDER_SETTINGS
	{
		ACTIVE_SSAO,
		SSAO_INTENSITY,
		BLUR_SIZE,

		SHADING_MODE,
		COLOR_LIGHT,
		COLOR_BACKGROUND,
		SPECULAR_FACTOR,
		SHININESS,
		TOON_STEPS,

		ACTIVE_FOG,
		COLOR_FOG,
		FOG_NEAR,
		FOG_FAR,
		FOG_DENSITY,

		ACTIVE_OUTLINE,
		COLOR_OUTLINE,
		OUTLINE_SENSITIVITY,
		OUTLINE_THICKNESS,

		ACTIVE_SELECTION,
		COLOR_SELECTION
	};

	/**
	 * @brief Render settings data provider.
	 */
	class RenderSettings : public ProxyPointerCollection
	{
	  public:
		template<E_RENDER_SETTINGS, typename... Args>
		Util::Callback<Args...> & onChange()
		{
			static Util::Callback<Args...> callback;
			return callback;
		}
	};

} // namespace VTX::Renderer::Proxy

#endif
