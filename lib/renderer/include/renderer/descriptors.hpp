#ifndef __VTX_RENDERER_DESCRIPTORS__
#define __VTX_RENDERER_DESCRIPTORS__

#include "enums.hpp"
#include <functional>
#include <unordered_map>
#include <util/types.hpp>
#include <variant>
#include <vector>

namespace VTX::Renderer
{
	struct Attachment
	{
		E_FORMAT	format		 = E_FORMAT::RGBA16F;
		E_WRAPPING	wrappingS	 = E_WRAPPING::CLAMP_TO_EDGE;
		E_WRAPPING	wrappingT	 = E_WRAPPING::CLAMP_TO_EDGE;
		E_FILTERING filteringMin = E_FILTERING::NEAREST;
		E_FILTERING filteringMag = E_FILTERING::NEAREST;
	};

	struct Storage
	{
	};

	struct Data
	{
	};

	// 	struct Uniform
	// 	{
	// 	};

	struct Program
	{
		std::string									  name;
		std::variant<FilePath, std::vector<FilePath>> shaders;
		std::string									  toInject;
		std::string									  suffix;
	};

	using Handle = uint;
	using IO	 = std::variant<Attachment, Storage, Data>;

	struct Pass
	{
		std::string name;
		struct Input
		{
			std::string name;
			IO			desc;
		};

		struct Output : public Input
		{
			// std::vector<Input *> dest; //?
		};

		using Inputs   = std::unordered_map<E_CHANNEL, Input>;
		using Outputs  = std::unordered_map<E_CHANNEL, Output>;
		using Programs = std::vector<Program>;

		Inputs	 inputs;
		Outputs	 outputs;
		Programs programs;
	};

	struct Link
	{
		Pass *	  src;
		Pass *	  dest;
		E_CHANNEL channelSrc  = E_CHANNEL::COLOR_0;
		E_CHANNEL channelDest = E_CHANNEL::COLOR_0;
	};

	using Passes = std::vector<std::unique_ptr<Pass>>;
	using Links	 = std::vector<std::unique_ptr<Link>>;

	using RenderQueue = std::vector<Pass *>;

	using Instruction  = std::function<void()>;
	using Instructions = std::vector<Instruction>;

} // namespace VTX::Renderer

#endif
