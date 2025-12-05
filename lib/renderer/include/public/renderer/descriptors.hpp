#ifndef __VTX_RENDERER_DESCRIPTORS__
#define __VTX_RENDERER_DESCRIPTORS__

#include "enums.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

/**
 * @brief Describes all the meta-data used by the render graph.
 */
namespace VTX::Renderer
{

	/**
	 * @brief Aliases.
	 */
	using Key	= std::string;
	using Keys	= std::vector<Key>;
	using Files = std::variant<FilePath, std::vector<FilePath>>;

	/**
	 * @brief Texture descriptor.
	 */
	struct TextureDesc
	{
		E_FORMAT format;
	};

	/**
	 * @brief Vertex attribute descriptor.
	 */
	struct VertexAttribute
	{
		Key			 name;
		E_TYPE		 type;
		std::uint8_t components;
	};

	/**
	 * @brief Vertex layout descriptor.
	 */
	struct VertexLayout
	{
		std::vector<VertexAttribute> attributes;
	};

	/**
	 * @brief Uniform value descriptor.
	 */
	struct UniformValue
	{
		Key										 name;
		E_TYPE									 type;
		std::array<std::uint8_t, 64>			 data {}; // Raw data storage (up to 64 bytes).
		std::optional<std::pair<double, double>> range;
	};

	/**
	 * @brief Uniform buffer descriptor.
	 */
	struct UniformBufferDesc
	{
		Key						  name;
		std::uint32_t			  binding = 0;
		std::vector<UniformValue> values;
	};

	/**
	 * @brief All resources.
	 */
	struct Resources
	{
		std::unordered_map<Key, TextureDesc>	   textures;
		std::unordered_map<Key, VertexLayout>	   vertexStreams;
		std::unordered_map<Key, UniformBufferDesc> uniformBuffers;
	};

	/**
	 * @brief Draw call descriptor.
	 */
	struct DrawCallDesc
	{
		Key			vertexStream;
		E_PRIMITIVE primitive  = E_PRIMITIVE::TRIANGLES;
		bool		useIndices = false;
	};

	/**
	 * @brief Program descriptor.
	 */
	struct ProgramDesc
	{
		Key							name;
		Files						shaders;
		std::vector<UniformValue>	uniforms;
		std::optional<DrawCallDesc> drawCall;
	};

	/**
	 * @brief Render function descriptor.
	 */
	using RenderFunc = std::function<void()>;

	/**
	 * @brief Pass descriptor.
	 */
	struct PassDesc
	{
		Key						  name;
		Keys					  inputs;
		Keys					  outputs;
		std::vector<ProgramDesc>  programs;
		std::optional<RenderFunc> customCallback;
	};

	/**
	 * @brief Link descriptor.
	 */
	struct LinkDesc
	{
		Key				 resourceName;
		const PassDesc * producer = nullptr;
		const PassDesc * consumer = nullptr;
	};

	/**
	 * @brief Aliases.
	 */
	using PassList = std::vector<std::unique_ptr<PassDesc>>;
	using LinkList = std::vector<LinkDesc>;

} // namespace VTX::Renderer

#endif
