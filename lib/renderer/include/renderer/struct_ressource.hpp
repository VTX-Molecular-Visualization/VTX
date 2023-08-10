#ifndef __VTX_RENDERER_RESSOURCE__
#define __VTX_RENDERER_RESSOURCE__

#include "context/concept_context.hpp"
#include <vector>

namespace VTX::Renderer
{
	enum struct E_HANDLE_TYPE
	{
		ATTACHMENT,
		STORAGE,
		// UNIFORM,
		PROGRAM,
		// ...?
	};

	// template<typename D>
	struct Resource
	{
		E_HANDLE_TYPE type;
		size_t		  id;
		// D			  desc;
	};

	// using Attachment = Resource<Context::DescTexture>;

	struct Attachement : public Resource
	{
		Context::DescAttachment desc;
	};

	struct Storage : public Resource
	{
	};

	// 	struct Uniform : public Resource
	// 	{
	// 	};

	struct Program : public Resource
	{
	};

	using Ressources = std::vector<Resource>;
} // namespace VTX::Renderer

#endif
