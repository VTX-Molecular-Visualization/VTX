#ifndef __VTX_RENDERER_CONTEXT_OPENGL_45__
#define __VTX_RENDERER_CONTEXT_OPENGL_45__

#include "concept.hpp"
#include "gl/buffer.hpp"
#include "gl/chrono.hpp"
#include "gl/framebuffer.hpp"
#include "gl/include_opengl.hpp"
#include "gl/program_manager.hpp"
#include "gl/struct_opengl_infos.hpp"
#include "gl/texture_2d.hpp"
#include "gl/vertex_array.hpp"
#include <set>
#include <util/enum.hpp>
#include <util/exceptions.hpp>
#include <util/string.hpp>

namespace VTX::Renderer::Context
{

	class OpenGL45 : public BaseContext
	{
	  public:
		OpenGL45() = delete;
		OpenGL45( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_proc = nullptr );

		/**
		 * @brief Generates render instructions from render _graph inputs.
		 * @param p_renderQueue the list of passes to render, ordered by the scheduler.
		 * @param p_links the connections between passes.
		 * @param p_output the output framebuffer.
		 * @param p_globalData the global GPU variables.
		 * @param p_outInstructions the generated instruction list.
		 * @param p_outInstructionsDurationRanges the generated instruction list grouped by category.
		 */
		void build(
			const RenderQueue &				p_renderQueue,
			const Links &					p_links,
			const std::vector<BufferData> & p_globalData,
			Instructions &					p_outInstructions,
			InstructionsDurationRanges &	p_outInstructionsDurationRanges
		);

		void resize( const RenderQueue & p_renderQueue, const size_t p_width, const size_t p_height );

		inline void setOutput( const Handle p_output ) { _output = p_output; }

		/**
		 * @brief Send data to GPU.
		 * @param p_key the buffer name to send on.
		 * @param p_index is the index of the data to set if we need to update only one value in an array.
		 */
		inline void setValue( const Key & p_key, const void * const p_value, const size_t p_index = 0 )
		{
			const Hash hash = Util::hash( p_key );
			assert( _bufferValueEntries.contains( hash ) );

			auto & entry = _bufferValueEntries[ hash ];
			entry->buffer->setSub( p_value, entry->size, entry->offset + p_index * entry->totalSize );
		}

		/**
		 * @brief Creates a GPU buffer
		 * @param p_key the buffer name.
		 * @param p_size the number of items to store.
		 */
		inline void reserveData( const Key & p_key, const size_t p_size )
		{
			const Hash hash = Util::hash( p_key );
			assert( _buffers.contains( hash ) );

			// Set dummy size (size 0 prohibited).
			const size_t size = p_size > 0 ? p_size : 1;

			// Scale if needed.
			if ( _buffers[ hash ]->size() != size )
			{
				VTX_TRACE(
					"Resizing buffer {} : {} -> {}",
					p_key,
					Util::String::memSizeToStr( _buffers[ hash ]->size() ),
					Util::String::memSizeToStr( size )
				);
				_buffers[ hash ]->set( nullptr, GLsizei( size ), false, GL_STATIC_DRAW );
			}
		}

		/**
		 * @brief Send data to GPU (buffer).
		 */
		inline void set( const Key & p_key, const void * const p_data, const size_t p_size )
		{
			const Hash hash = Util::hash( p_key );
			assert( _buffers.contains( hash ) );

			// Set dummy.
			if ( p_size == 0 )
			{
				reserveData( p_key, 0 );
			}
			// Auto scale.
			else if ( _buffers[ hash ]->size() != p_size )
			{
				VTX_TRACE(
					"Resizing buffer {} : {} -> {}",
					p_key,
					Util::String::memSizeToStr( _buffers[ hash ]->size() ),
					Util::String::memSizeToStr( p_size )
				);
				_buffers[ hash ]->set( p_data, GLsizei( p_size ), false, GL_STATIC_DRAW );
			}
			else
			{
				_buffers[ hash ]->setSub( p_data, p_size );
			}
		}

		/**
		 * @brief Send data to an existing GPU buffer.
		 */
		inline void setSub(
			const Key &		   p_key,
			const void * const p_data,
			const size_t	   p_size,
			const size_t	   p_offset = 0
		)
		{
			const Hash hash = Util::hash( p_key );
			assert( _buffers.contains( hash ) );

			VTX_DEBUG( "Set sub buffer {} : {} -> {}", p_key, p_offset, p_size );
			_buffers[ hash ]->setSub( p_data, p_size, p_offset );
		}

		/**
		 * @brief Get data from GPU buffer.
		 */
		inline void get( const Key & p_key, void * const p_data, const size_t p_size )
		{
			const Hash hash = Util::hash( p_key );
			assert( _buffers.contains( hash ) );

			_buffers[ hash ]->get( p_data, p_size );
		}

		// TODDO: send data to buffer by map()?

		void fillInfos( StructInfos & p_infos ) const;

		inline float measureTaskDuration( const Util::Chrono::Task & p_task ) const { return GL::CHRONO_GPU( p_task ); }

		inline void compileShaders() const { _programManager->compileShaders(); }

		void snapshot(
			std::vector<uchar> & p_image,
			const RenderQueue &	 p_renderQueue,
			const Instructions & p_instructions,
			const size_t		 p_width,
			const size_t		 p_height
		);

		void getTextureData(
			const Key &		 p_key,
			std::any &		 p_textureData,
			const size_t	 p_x,
			const size_t	 p_y,
			const E_CHAN_OUT p_channel
		)
		{
			const Hash hash = Util::hash( p_key );
			assert( _framebuffers.contains( hash ) );

			auto &			   fbo	  = _framebuffers[ hash ];
			const Pass * const pass	  = _descPasses[ hash ];
			const IO &		   descIO = pass->outputs.at( p_channel ).desc;

			assert( std::holds_alternative<Attachment>( descIO ) );

			const Attachment & attachment = std::get<Attachment>( descIO );
			const E_FORMAT	   format	  = attachment.format;

			fbo->bind( GL_READ_FRAMEBUFFER );
			fbo->setReadBuffer( _mapAttachments[ p_channel ] );
			glReadPixels(
				int32_t( p_x ),
				int32_t( p_y ),
				1,
				1,
				_mapFormatInternalTypes[ format ],
				_mapTypes[ _mapFormatTypes[ format ] ],
				&p_textureData
			);
			fbo->unbind();
		}

		void compute( const ComputePass & p_pass );

	  private:
		FilePath _shaderPath;

		/////////////////// TODO: use collection util class
		//
		// TODO: find a better solution (magic enum explodes compile time).
		static std::map<const E_CHAN_OUT, const GLenum>	 _mapAttachments;
		static std::map<const E_PRIMITIVE, const GLenum> _mapPrimitives;
		static std::map<const E_FORMAT, const GLenum>	 _mapFormats;
		static std::map<const GLenum, const GLsizei>	 _mapFormatSizes;
		static std::map<const E_WRAPPING, const GLint>	 _mapWrappings;
		static std::map<const E_FILTERING, const GLint>	 _mapFilterings;
		static std::map<const E_TYPE, const GLenum>		 _mapTypes;
		static std::map<const E_TYPE, const GLsizeiptr>	 _mapTypeSizes;
		static std::map<const E_TYPE, const GLuint>		 _mapTypeAlignments;
		static std::map<const E_FORMAT, const E_TYPE>	 _mapFormatTypes;
		static std::map<const E_FORMAT, const GLenum>	 _mapFormatInternalTypes;

		const Key _KEY_QUAD_VAO	   = "VAO_QUAD";
		const Key _KEY_QUAD_BUFFER = "BUFFER_QUAD";
		const Key _KEY_IN		   = "In";
		const Key _KEY_OUT		   = "Out";
		const Key _KEY_EBO		   = "Idx";

		// TODO: use Util::Collection instead?
		template<typename T>
		using Collection = std::unordered_map<Hash, std::unique_ptr<T>>;
		template<typename T>
		using CollectionPtr = std::unordered_map<Hash, const T * const>;

		CollectionPtr<Pass>			_descPasses;
		Collection<GL::VertexArray> _vertexArrays;
		Collection<GL::Buffer>		_buffers;
		Collection<GL::Framebuffer> _framebuffers;
		Collection<GL::Texture2D>	_textures;
		///////////////////

		struct _StructBufferDataValueEntry
		{
			GL::Buffer * buffer;
			size_t		 offset;
			size_t		 size;
			size_t		 padding;
			size_t		 totalSize;
			_StructBufferDataValueEntry(
				GL::Buffer * p_buffer,
				const size_t p_offset,
				const size_t p_size,
				const size_t p_padding
			) : buffer( p_buffer ), offset( p_offset ), size( p_size ), padding( p_padding )
			{
			}
		};
		std::unique_ptr<GL::ProgramManager>		_programManager;
		CollectionPtr<GL::Program>				_programs;
		Collection<_StructBufferDataValueEntry> _bufferValueEntries;

		// Output.
		Handle _output;

		// Specs.
		GL::StructOpenglInfos _openglInfos;

		void _createInputs( const Links &, const Pass * const, const Key &, Keys & );

		void _createOuputs( const Pass * const, std::set<uint32_t> & p_drawBuffers, const Key &, Keys & );

		const GL::Program * const _createProgram( const Program &, const Key &, Keys & );

		std::optional<std::pair<const Output * const, const Key>> _getInputTextureKey(
			const Links &,
			const Pass * const,
			const E_CHAN_IN
		);

		bool _hasDepthComponent( const Pass * const ) const;

		GL::Texture2D * const _createTexture( const Attachment & p_attachment, const Key & p_key, Keys & );

		Vec2i _getTextureSize( const Attachment & ) const;

		GL::Buffer * const _createBufferData( const BufferData &, const Key &, Keys & );

		template<typename T>
		void _setBufferDataDefaultValue( const BufferDataValue & p_value, const Key & p_key )
		{
			assert( std::holds_alternative<BufferValue<T>>( p_value.value ) );

			const Hash hashValue = Util::hash( p_key );

			assert( _bufferValueEntries.contains( hashValue ) );

			setValue( p_key, &( std::get<BufferValue<T>>( p_value.value ).value ) );
		}

		/**
		 * @brief After a build, purge resources that are not needed anymore.
		 * @param the list of keys to keep, others will be deleted.
		 */
		void _purgeResources( const Keys & );

		void				 _getOpenglInfos();
		static void APIENTRY _debugMessageCallback(
			const GLenum   p_source,
			const GLenum   p_type,
			const GLuint   p_id,
			const GLenum   p_severity,
			const GLsizei  p_length,
			const GLchar * p_msg,
			const void *   p_data
		);
	};
} // namespace VTX::Renderer::Context

#endif
