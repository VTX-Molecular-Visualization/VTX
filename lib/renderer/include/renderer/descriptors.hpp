#ifndef __VTX_RENDERER_DESCRIPTORS__
#define __VTX_RENDERER_DESCRIPTORS__

#include "enums.hpp"
#include <array>
#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <variant>
#include <vector>

/**
 * @brief Describes all the meta-data used by the render graph.
 */
namespace VTX::Renderer
{
	using Size = std::optional<std::variant<size_t, float>>;
	struct Attachment
	{
		E_FORMAT	format		 = E_FORMAT::RGBA16F;
		E_WRAPPING	wrappingS	 = E_WRAPPING::CLAMP_TO_EDGE;
		E_WRAPPING	wrappingT	 = E_WRAPPING::CLAMP_TO_EDGE;
		E_FILTERING filteringMin = E_FILTERING::NEAREST;
		E_FILTERING filteringMag = E_FILTERING::NEAREST;
		Size		width		 = std::nullopt;
		Size		height		 = std::nullopt;
		void *		data		 = nullptr;
	};

	struct BufferDraw
	{
		struct Entry
		{
			std::string name;
			E_TYPE		nativeType;
			size_t		components;
		};
		std::vector<Entry> entries;
	};

	template<typename T>
	struct BufferValue
	{
		T value;

		// TODO: test anonymous struct.
		struct MinMax
		{
			T min;
			T max;
		};
		std::optional<MinMax> minMax;
	};

	using BufferDataValueVariant = std::variant<
		BufferValue<bool>,
		BufferValue<char>,
		BufferValue<uchar>,
		BufferValue<short>,
		BufferValue<ushort>,
		BufferValue<int>,
		BufferValue<uint>,
		BufferValue<float>,
		BufferValue<Vec2i>,
		BufferValue<Vec2f>,
		BufferValue<Vec3f>,
		BufferValue<Vec4f>,
		BufferValue<Mat3f>,
		BufferValue<Mat4f>,
		BufferValue<Util::Color::Rgba>>;

	struct BufferDataValue
	{
		std::string			   name;
		E_TYPE				   type;
		BufferDataValueVariant value;
	};

	using BufferDataValues = std::vector<BufferDataValue>;

	struct BufferData
	{
		std::string		 name;
		char			 binding;
		BufferDataValues values;
		bool			 isLarge	 = false; // If max size >64 Ko
		bool			 isImmutable = false; // TODO: use variant of flags enums.

		/*
		struct BufferDraw
		{
			size_t size;
			void * data;
			uint   binding;
			bool   overwrite = false; // TODO: force data?
		};
		*/
	};

	using Handle = uint;
	using IO	 = std::variant<Attachment, BufferData, BufferDraw>;

	struct Input
	{
		std::string name;
		IO			desc;
	};

	struct Output : public Input
	{
	};

	using NeedRenderFunc = std::function<bool()>;
	struct Draw
	{
		std::string name;
		E_PRIMITIVE primitive;
		bool		useIndices = false;

		struct Range
		{
			std::vector<void *> offsets;
			std::vector<uint>	counts;
		};
		Range * ranges = nullptr;

		NeedRenderFunc needRenderFunc;
	};

	using Files = std::variant<FilePath, std::vector<FilePath>>;

	using BufferDataList = std::vector<BufferData>;

	struct Program
	{
		std::string			name;
		Files				shaders;
		BufferDataValues	data;
		std::optional<Draw> draw;
		std::string			toInject;
		std::string			suffix;
	};

	using Inputs   = std::unordered_map<E_CHAN_IN, Input>;
	using Outputs  = std::unordered_map<E_CHAN_OUT, Output>;
	using Programs = std::vector<Program>;

	struct Pass
	{
		std::string			   name;
		Inputs				   inputs;
		Outputs				   outputs;
		Programs			   programs;
		std::vector<E_SETTING> settings;
	};

	constexpr int LOCAL_SIZE_X = 256;
	constexpr int LOCAL_SIZE_Y = 1;
	constexpr int LOCAL_SIZE_Z = 1;

	struct ComputePass
	{
		Program program;

		// TODO: merge with uniform to use same buffer descriptor?
		struct BufferDraw
		{
			size_t size;
			void * data;
			uint   binding;
			bool   overwrite = false; // TODO: force data?
		};

		std::vector<ComputePass::BufferDraw *> data;
		std::variant<Vec3i, size_t>			   size;
	};

	struct Link
	{
		Pass *	   src;
		Pass *	   dest;
		E_CHAN_OUT channelSrc  = E_CHAN_OUT::COLOR_0;
		E_CHAN_IN  channelDest = E_CHAN_IN::_0;
	};

	using Passes = std::vector<std::unique_ptr<Pass>>;
	using Links	 = std::vector<std::unique_ptr<Link>>;

	using RenderQueue = std::vector<Pass *>;

	using Instruction  = std::function<void()>;
	using Instructions = std::vector<Instruction>;

	struct InstructionsDurationRange
	{
		std::string name;
		size_t		first;
		size_t		last;
		float		duration;
	};

	using InstructionsDurationRanges = std::vector<InstructionsDurationRange>;

	struct StructCompareVisitorDesc
	{
		bool operator()( const Attachment & p_left, const Attachment & p_right ) const
		{
			// return p_left.format == p_right.format;
			// TODO: check compatibility.
			return true;
		}

		bool operator()( const BufferDraw & p_left, const BufferDraw & p_right ) const { return false; }

		bool operator()( const BufferData & p_left, const BufferData & p_right ) const { return false; }

		template<typename T, typename U>
		bool operator()( const T & p_left, const U & p_right ) const
		{
			return false;
		}
	};

} // namespace VTX::Renderer

#endif
