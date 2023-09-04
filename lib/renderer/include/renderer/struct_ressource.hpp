#ifndef __VTX_RENDERER_RESSOURCE__
#define __VTX_RENDERER_RESSOURCE__

// #include "context/concept_context.hpp"
#include <vector>

namespace VTX::Renderer
{

	struct Resource
	{
		Context::Handle id;
		Context::Desc	desc;
	};

	// using Attachment = Resource<Context::DescTexture>;

	// 	struct Attachement : public Resource
	// 	{
	// 		Context::DescAttachment * desc;
	// 	};
	//
	// 	struct Storage : public Resource
	// 	{
	// 	};

	// 	struct Uniform : public Resource
	// 	{
	// 	};

	// 	struct Program : public Resource
	// 	{
	// 	};

	using Ressources = std::vector<Resource>;
} // namespace VTX::Renderer

#endif
