#ifndef __VTX_RENDERER_CONTEXT_BACKEND_OPENGL__
#define __VTX_RENDERER_CONTEXT_BACKEND_OPENGL__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/backend/gl/buffer.hpp"
#include "renderer/context/backend/gl/chrono.hpp"
#include "renderer/context/backend/gl/context.hpp"
#include "renderer/context/backend/gl/framebuffer.hpp"
#include "renderer/context/backend/gl/include_opengl.hpp"
#include "renderer/context/backend/gl/program.hpp"
#include "renderer/context/backend/gl/sampler.hpp"
#include "renderer/context/backend/gl/struct_opengl_infos.hpp"
#include "renderer/context/backend/gl/texture.hpp"
#include "renderer/context/backend/gl/vertex_array.hpp"
#include "renderer/context/backend/interop/opengl_cuda_interop.hpp"
#include "renderer/context/command_buffer.hpp"
#include "renderer/descriptors.hpp"
#include "renderer/resource_handler.hpp"
#include "renderer/struct_infos.hpp"
#include <memory>
#include <span>
#include <vector>

namespace VTX::Renderer::Context::Backend
{
	/**
	 * @brief OpenGL backend.
	 */
	class OpenGL
	{
	  public:
		/**
		 * @brief Texture binding info.
		 */
		struct TextureBinding
		{
			Desc::Handle  texture;
			Desc::Handle  sampler;
			Desc::Binding unit;
		};

		/**
		 * @brief Buffer binding info.
		 */
		struct BufferBinding
		{
			Desc::Handle		 buffer;
			Desc::E_BUFFER_USAGE usage;
			Desc::Binding		 binding;
			uint32_t			 offsetBytes = 0;
			uint32_t			 sizeBytes	 = 0;
		};

		/**
		 * @brief Resource table per pass.
		 */
		struct ResourceTable
		{
			std::vector<TextureBinding> textures;
			std::vector<BufferBinding>	buffers;
		};

		/**
		 * @brief Default constructor.
		 */
		OpenGL(
			const size_t p_width,
			const size_t p_height,
			const Desc::NativeContextInfo &,
			const FilePath & p_shaderDir
		);
		~OpenGL();

		/**
		 * @brief Build the command buffer from the render queue and resources.
		 */
		void build( const Desc::RenderQueue &, const Desc::Resources &, CommandBuffer & );

		/**
		 * @brief Resize textures.
		 */
		void resize( const uint32_t, const uint32_t, const Desc::PassList &, const Desc::ResourceMap<Desc::Texture> & );

		/**
		 * @brief Swap buffers.
		 */
		inline void swap() const { _glContext.swapBuffers(); }

		/**
		 * @brief Set data to a buffer.
		 */
		void setBufferData( const Desc::BufferRef &, SpanBytes, const size_t );
		bool ensureBufferChunk( const Desc::BufferRef & );
		bool releaseBufferChunk( const Desc::BufferRef & );

		/**
		 * @brief Map graphics buffers to an external compute backend pointer.
		 */
		Desc::InteropBufferMapping				mapInteropBuffer( Desc::E_INTEROP_API, const Desc::BufferRef & );
		std::vector<Desc::InteropBufferMapping> mapInteropBuffers(
			Desc::E_INTEROP_API,
			std::span<const Desc::BufferRef>
		);
		void unmapInteropBuffer( Desc::E_INTEROP_API, const Desc::InteropBufferMapping & );
		void unmapInteropBuffers( Desc::E_INTEROP_API, std::span<const Desc::InteropBufferMapping> );
		void unregisterInteropBuffer( Desc::E_INTEROP_API, const Desc::BufferRef & );

		/**
		 * @brief Query external API availability for graphics interop.
		 */
		Desc::InteropAvailability interopAvailability( Desc::E_INTEROP_API ) const;

		/**
		 * @brief Get texture data at a given pixel, or full texture.
		 */
		std::vector<std::byte> getTextureData(
			const Desc::Key &,
			std::optional<Desc::E_FORMAT>,
			std::optional<size_t>,
			std::optional<size_t>
		) const;

		/**
		 * @brief Set texture data.
		 */
		void setTextureData(
			const Desc::Key &,
			SpanBytes,
			std::optional<size_t> p_width  = {},
			std::optional<size_t> p_height = {}
		);

		/**
		 * @brief Set render target.
		 */
		void setRenderTarget( const Desc::E_RENDER_TARGET );

		/**
		 * @brief Set options.
		 */
		void setOption( const Desc::E_OPTION, const bool );

		/**
		 * @brief Fill backend infos.
		 */
		void fillInfos( StructInfos & p_infos ) const;

		/**
		 * @brief Resources accessors.
		 */
		inline const ResourceTable & resourceTable( const Desc::Handle p_handle ) const noexcept
		{ return _resourceTables.get( p_handle ); }

		inline const GL::Framebuffer & framebuffer( const Desc::Handle p_handle ) const noexcept
		{ return _framebuffers.get( p_handle ); }

		inline const GL::Texture & texture( const Desc::Handle p_handle ) const noexcept
		{ return _textures.get( p_handle ); }

		inline const GL::Sampler & sampler( const Desc::Handle p_handle ) const noexcept
		{ return _samplers.get( p_handle ); }

		inline const GL::Program & program( const Desc::Handle p_handle ) const noexcept
		{ return _programs.get( p_handle ); }

		inline const GL::Buffer & buffer( const Desc::Handle p_handle ) const noexcept
		{ return _buffers.get( p_handle ); }

		inline const GL::VertexArray & vertexArray( const Desc::Handle p_handle ) const noexcept
		{ return _vertexArrays.get( p_handle ); }

	  private:
		/**
		 * @brief Current width and height.
		 */
		uint32_t _width;
		uint32_t _height;

		/**
		 * @brief EGL context.
		 */
		GL::GLContextWrapper _glContext;

		/**
		 * @brief Render target (default framebuffer or offscreen).
		 */
		Desc::Handle _default	= Desc::NO_HANDLE;
		Desc::Handle _offscreen = Desc::NO_HANDLE;
		Desc::Handle _target	= Desc::NO_HANDLE;

		/**
		 * @brief Shader path.
		 */
		const FilePath _shaderPath;

		/**
		 * @brief Global shader buffers and resource tables.
		 */
		using GlobalShaderBuffers = std::vector<BufferBinding>;
		GlobalShaderBuffers _globalShaderBuffers;

		/**
		 * @brief Resource pools.
		 */
		ResourceHandler<GL::Framebuffer>								_framebuffers;
		ResourceHandler<ResourceTable>									_resourceTables;
		ResourceHandler<GL::VertexArray, Desc::Key, Desc::VertexLayout> _vertexArrays;
		ResourceHandler<GL::Buffer, Desc::Key, Desc::Buffer>			_buffers;
		ResourceHandler<GL::Texture, Desc::Key, Desc::Texture>			_textures;
		ResourceHandler<GL::Sampler, Desc::Key, Desc::Sampler>			_samplers;
		ResourceHandler<GL::Program, Desc::Key, Desc::Program>			_programs;

		/**
		 * @brief Get or create resources.
		 */
		Desc::Handle _getOrCreateQuad();
		Desc::Handle _getOrCreateFramebuffer( const Desc::Pass &, const Desc::Resources &, const bool = false );
		Desc::Handle _getOrCreateResourceTable( const Desc::Pass &, const Desc::Resources & );
		Desc::Handle _getOrCreateChunkResourceTable( const Desc::Pass &, Desc::BufferChunk );
		Desc::Handle _getOrCreateTexture( const Desc::Key &, const Desc::Texture & );
		Desc::Handle _getOrCreateSampler( const Desc::Key &, const Desc::Sampler & );
		Desc::Handle _getOrCreateVertexLayout( const Desc::Key &, const Desc::VertexLayout & );
		Desc::Handle _getOrCreateGeometryVertexArray(
			const Desc::Geometry &,
			const Desc::Resources &,
			const Desc::BufferChunk
		);
		Desc::Handle _getOrCreateBuffer( const Desc::Key &, const Desc::Buffer & );
		Desc::Handle _getOrCreateBufferChunk( const Desc::Key &, const Desc::Buffer &, const Desc::BufferChunk );
		Desc::Handle _getOrCreateProgram( const Desc::Program & );
		Desc::Handle _bufferHandle( const Desc::Key &, const Desc::Resources &, const Desc::BufferChunk );
		Desc::Key	 _physicalBufferKey( const Desc::BufferRef & );
		void		 _setBufferData( const Desc::Key &, SpanBytes, const size_t );
		std::vector<Desc::InteropBufferMapping> _mapPhysicalInteropBuffers(
			Desc::E_INTEROP_API,
			std::span<const Desc::Key>
		);
		static Desc::Key _bufferChunkKey( const Desc::Key &, Desc::BufferChunk );
		static Desc::Key _vertexArrayChunkKey( const Desc::Key &, Desc::BufferChunk );

		/**
		 * @brief Build resources.
		 */
		GlobalShaderBuffers _buildGlobalShaderBuffers( const Desc::Resources & );
		ResourceTable		_buildResourceTableForPass( const Desc::Pass &, const Desc::Resources & );
		ResourceTable		_buildChunkResourceTable( const Desc::Resources &, Desc::BufferChunk );

		/**
		 * @brief Bind resources.
		 */
		void _attachTexturesToFramebuffer( const Desc::Pass &, const Desc::ResourceMap<Desc::Texture> & );
		void _bindGeometryToVao(
			const Desc::Key &,
			const Desc::Geometry &,
			const Desc::Resources &,
			const Desc::BufferChunk
		);

		/**
		 * @brief Create the screen quad.
		 */
		inline static const Desc::Key _QUAD		   = "Quad";
		inline static const Desc::Key _QUAD_VBO	   = _QUAD + ".Position";
		inline static const Desc::Key _DEFAULT_FBO = "Default";

		/**
		 * @brief Specs.
		 */
		GL::StructOpenglInfos _openglInfos;
		void				  _getOpenglInfos();

		/**
		 * @brief Interop with CUDA.
		 */
		Interop::OpenGLCudaInterop _cudaInterop;
	};
} // namespace VTX::Renderer::Context::Backend

#endif
