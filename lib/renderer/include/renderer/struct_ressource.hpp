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
		E_HANDLE_TYPE type;
		size_t		  id;
	};

	using Resource	 = StructResource;
	using Ressources = std::vector<Resource>;
} // namespace VTX::Renderer

#endif
