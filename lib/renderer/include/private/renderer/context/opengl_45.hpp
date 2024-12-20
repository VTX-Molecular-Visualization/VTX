#ifndef __VTX_RENDERER_CONTEXT_OPENGL_45__
#define __VTX_RENDERER_CONTEXT_OPENGL_45__

#include "concept_context.hpp"
#include "gl/buffer.hpp"
#include "gl/chrono.hpp"
#include "gl/framebuffer.hpp"
#include "gl/program_manager.hpp"
#include "gl/struct_opengl_infos.hpp"
#include "gl/texture_2d.hpp"
#include "gl/vertex_array.hpp"
#include "include_opengl.hpp"
#include <set>
#include <util/enum.hpp>
#include <util/exceptions.hpp>
#include <util/string.hpp>

namespace VTX::Renderer::Context
{

	class OpenGL45 : public BaseContext
	{
	  public:
		using Key  = std::string;
		using Keys = std::vector<Key>;

		OpenGL45() = delete;
		OpenGL45( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_proc = nullptr );

		/**
		 * @brief Generates render instructions from render graph inputs.
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
			const Handle					p_output,
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
		template<typename T>
		inline void setValue( const T & p_value, const Key & p_key, const size_t p_index = 0 )
		{
			assert( _bufferValueEntries.contains( p_key ) );

			std::unique_ptr<_StructBufferDataValueEntry> & entry = _bufferValueEntries[ p_key ];
			entry->buffer->setSub(
				p_value, GLint( entry->offset + p_index * entry->totalSize ), GLsizei( entry->size )
			);
		}

		/**
		 * @brief Creates a GPU buffer
		 * @tparam T the data type to store.
		 * @param p_size the number of items to store.
		 * @param p_key the buffer name.
		 */
		template<typename T>
		inline void reserveData( const size_t p_size, const Key & p_key )
		{
			assert( _buffers.contains( p_key ) );

			// Set dummy size (size 0 prohibited).
			size_t size = sizeof( T ) * p_size;
			size		= size > 0 ? size : 1;

			// Scale if needed.
			if ( _buffers[ p_key ]->size() != size )
			{
				VTX_TRACE(
					"Resizing buffer {} : {} -> {}",
					p_key,
					Util::String::memSizeToStr( _buffers[ p_key ]->size() ),
					Util::String::memSizeToStr( size )
				);
				_buffers[ p_key ]->set( GLsizei( size ), nullptr, 0, GL_STATIC_DRAW );
			}
		}

		/**
		 * @brief Send data to GPU (buffer).
		 */
		template<typename T>
		inline void set( const std::vector<T> & p_data, const Key & p_key )
		{
			assert( _buffers.contains( p_key ) );

			// Set dummy.
			if ( p_data.size() == 0 )
			{
				reserveData<char>( 0, p_key );
			}
			// Auto scale.
			else if ( _buffers[ p_key ]->size() != sizeof( T ) * p_data.size() )
			{
				VTX_TRACE(
					"Resizing buffer {} : {} -> {}",
					p_key,
					Util::String::memSizeToStr( _buffers[ p_key ]->size() ),
					Util::String::memSizeToStr( sizeof( T ) * p_data.size() )
				);
				_buffers[ p_key ]->set( p_data, 0, GL_STATIC_DRAW );
			}
			else
			{
				_buffers[ p_key ]->setSub( p_data );
			}
		}

		/**
		 * @brief Send data to an existing GPU buffer.
		 */
		template<typename T>
		inline void setSub(
			const std::vector<T> & p_data,
			const Key &			   p_key,
			const size_t		   p_offset		  = 0,
			const bool			   p_offsetSource = false,
			const size_t		   p_size		  = 0
		)
		{
			VTX_DEBUG( "Set sub buffer {} : {} -> {}", p_key, p_offset, p_size );
			assert( _buffers.contains( p_key ) );

			_buffers[ p_key ]->setSub(
				p_data, GLint( p_offset * sizeof( T ) ), p_offsetSource, GLsizei( p_size * sizeof( T ) )
			);
		}

		/**
		 * @brief Get data from GPU buffer.
		 */
		template<typename T>
		inline void get( std::vector<T> & p_data, const Key & p_key )
		{
			assert( _buffers.contains( p_key ) );

			_buffers[ p_key ]->get( p_data );
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

		template<typename T>
		void getTextureData(
			T &				 p_textureData,
			const size_t	 p_x,
			const size_t	 p_y,
			const Key &		 p_key,
			const E_CHAN_OUT p_channel
		)
		{
			assert( _framebuffers.contains( p_key ) );

			auto &			   fbo	  = _framebuffers[ p_key ];
			const Pass * const pass	  = _descPasses[ p_key ];
			const IO &		   descIO = pass->outputs.at( p_channel ).desc;

			assert( std::holds_alternative<Attachment>( descIO ) );

			const Attachment & attachment = std::get<Attachment>( descIO );
			const E_FORMAT	   format	  = attachment.format;

			fbo->bind( GL_READ_FRAMEBUFFER );
			fbo->setReadBuffer( _mapAttachments[ p_channel ] );
			glReadPixels(
				GLint( p_x ),
				GLint( p_y ),
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
		/////////////////// TODO: use collection util class
		//
		// TODO: find a better solution (magic enum explodes compile time).
		static std::map<const E_CHAN_OUT, const GLenum>	 _mapAttachments;
		static std::map<const E_PRIMITIVE, const GLenum> _mapPrimitives;
		static std::map<const E_FORMAT, const GLenum>	 _mapFormats;
		static std::map<const GLenum, const size_t>		 _mapFormatSizes;
		static std::map<const E_WRAPPING, const GLint>	 _mapWrappings;
		static std::map<const E_FILTERING, const GLint>	 _mapFilterings;
		static std::map<const E_TYPE, const GLenum>		 _mapTypes;
		static std::map<const E_TYPE, const size_t>		 _mapTypeSizes;
		static std::map<const E_FORMAT, const E_TYPE>	 _mapFormatTypes;
		static std::map<const E_FORMAT, const GLenum>	 _mapFormatInternalTypes;

		const Key _KEY_QUAD_VAO	   = "VAO_QUAD";
		const Key _KEY_QUAD_BUFFER = "BUFFER_QUAD";
		const Key _KEY_IN		   = "In";
		const Key _KEY_OUT		   = "Out";
		const Key _KEY_EBO		   = "Idx";

		template<typename T>
		using Collection = std::unordered_map<Key, std::unique_ptr<T>>;
		template<typename T>
		using CollectionPtr = std::unordered_map<Key, const T * const>;

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

		/////////////////////////////////////
		// TODO: rework.
		/*
		inline Key _getKey( const BufferData & p_buffer ) const { return p_buffer.name; }
		inline Key _getKey( const Pass & p_pass ) const { return p_pass.name; }
		inline Key _getKey( const Pass * const p_pass, const Program & p_program ) const
		{
			return ( p_pass ? p_pass->name : "" ) + p_program.name;
		}
		inline Key _getKey( const Draw & p_draw, const bool p_isIndice = false ) const
		{
			return p_draw.name + ( p_isIndice ? _KEY_EBO_SUFFIX : "" );
		}
		inline Key _getKey( const Pass & p_pass, const bool p_isInput, const uint p_chan ) const
		{
			return p_pass.name + ( p_isInput ? "In" : "Out" ) + std::to_string( p_chan );
		}
		inline Key _getKey( const Input & p_input, const bool p_isIndice = false ) const
		{
			return p_input.name + ( p_isIndice ? _KEY_EBO_SUFFIX : "" );
		}
		inline Key _getKey( const Input & p_input, const BufferDraw::Entry & p_entry ) const
		{
			return p_input.name + p_entry.name;
		}
		inline Key _getKey(
			const Pass * const		p_pass,
			const Program * const	p_program,
			const BufferDataValue & p_bufferDataValue
		) const
		{
			return ( p_pass ? p_pass->name : "" ) + ( p_program ? p_program->name : "" ) + p_bufferDataValue.name;
		}
		*/
		//////////////////////////////////////////

		void _createInputs( const Links &, const Pass * const, const Key, Keys & );

		void _createOuputs( const Pass * const, std::set<GLenum> & p_drawBuffers, const Key, Keys & );

		const GL::Program * const _createProgram( const Program &, const Key, Keys & );

		std::optional<std::pair<const Output * const, const Key>> _getInputTextureKey(
			const Links &,
			const Pass * const,
			const E_CHAN_IN
		);

		bool _hasDepthComponent( const Pass * const ) const;

		GL::Texture2D * const _createTexture( const IO &, const Key &, Keys & );

		Vec2i _getTextureSize( const Attachment & ) const;

		GL::Buffer * const _createBufferData( const BufferData &, const Key &, Keys & );

		template<typename T>
		void _setBufferDataDefaultValue( const BufferDataValue & p_value, const Key & p_key )
		{
			assert( std::holds_alternative<BufferValue<T>>( p_value.value ) );
			assert( _bufferValueEntries.contains( p_key ) );

			setValue<T>( std::get<BufferValue<T>>( p_value.value ).value, p_key );
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
