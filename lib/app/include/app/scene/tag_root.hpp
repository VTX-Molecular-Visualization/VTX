#ifndef __VTX_APP_SCENE_TAG_ROOT__
#define __VTX_APP_SCENE_TAG_ROOT__

#include <util/types.hpp>

namespace VTX::App::Scene
{
	/**
	 * @brief Scene items.
	 */
	enum struct E_ITEM : uint8_t
	{
		SYSTEM,
		CATEGORY,
		CHAIN,
		RESIDUE,
		ATOM,
		PATH,
		VIEWPOINT,

		COUNT
	};

	/**
	 * @brief Empty component to use as tag.
	 */
	class TagRoot
	{
	};
} // namespace VTX::App::Scene

#endif
