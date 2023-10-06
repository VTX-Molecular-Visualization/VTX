#ifndef __VTX_RENDERER_DESCRIPTORS__
#define __VTX_RENDERER_DESCRIPTORS__

#include "enums.hpp"
#include <functional>
#include <map>
#include <unordered_map>
#include <util/color/rgba.hpp>
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

	using Handle = uint;
	using IO	 = std::variant<Attachment, Storage, Data>;

	struct Input
	{
		std::string name;
		IO			desc;
	};

	struct Output : public Input
	{
	};

	struct Uniform
	{
		std::string name;
		E_TYPE		type;
	};

	using Files	   = std::variant<FilePath, std::vector<FilePath>>;
	using Uniforms = std::vector<Uniform>;

	struct Program
	{
		std::string name;
		Files		shaders;
		Uniforms	uniforms;
		std::string toInject;
		std::string suffix;
	};

	using Inputs   = std::unordered_map<E_CHANNEL_INPUT, Input>;
	using Outputs  = std::unordered_map<E_CHANNEL_OUTPUT, Output>;
	using Programs = std::vector<Program>;

	struct Pass
	{
		std::string name;

		Inputs	 inputs;
		Outputs	 outputs;
		Programs programs;
	};

	struct Link
	{
		Pass *			 src;
		Pass *			 dest;
		E_CHANNEL_OUTPUT channelSrc	 = E_CHANNEL_OUTPUT::COLOR_0;
		E_CHANNEL_INPUT	 channelDest = E_CHANNEL_INPUT::_0;
	};

	using Passes = std::vector<std::unique_ptr<Pass>>;
	using Links	 = std::vector<std::unique_ptr<Link>>;

	using RenderQueue = std::vector<Pass *>;

	using Instruction  = std::function<void()>;
	using Instructions = std::vector<Instruction>;

	struct StructCompareVisitorDesc
	{
		bool operator()( const Attachment & p_left, const Attachment & p_right ) const
		{
			return p_left.format == p_right.format;
		}
		bool operator()( const Storage & p_left, const Storage & p_right ) const { return false; }

		bool operator()( const Data & p_left, const Data & p_right ) const { return false; }

		template<typename T, typename U>
		bool operator()( const T & p_left, const U & p_right ) const
		{
			return false;
		}
	};

} // namespace VTX::Renderer

#endif
