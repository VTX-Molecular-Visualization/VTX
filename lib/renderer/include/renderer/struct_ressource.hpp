#ifndef __VTX_RENDERER_RESSOURCE__
#define __VTX_RENDERER_RESSOURCE__

#include <vector>

namespace VTX::Renderer
{
	enum struct E_HANDLE_TYPE
	{
		ATTACHMENT,
		STORAGE,
		UNIFORM,
		// ...?
	};

	struct StructResource
	{
		size_t		  id;
		E_HANDLE_TYPE type;
	};

	using ListRessources = std::vector<StructResource>;
} // namespace VTX::Renderer

#endif
