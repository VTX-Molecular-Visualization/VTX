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
#include <glad/glad.h>
#include <util/enum.hpp>
#include <util/exceptions.hpp>

namespace VTX::Renderer::Context
{

	class OpenGL45 : public BaseContext
	{
	  public:
		OpenGL45() = delete;
		OpenGL45( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_proc = nullptr );

		void build(
			const RenderQueue &			  p_renderQueue,
			const Links &				  p_links,
			const Handle				  p_output,
			const std::vector<Uniforms> & p_uniforms,
			Instructions &				  p_outInstructions,
			InstructionsDurationRanges &  p_outInstructionsDurationRanges
		);

		void resize( const RenderQueue & p_renderQueue, const size_t p_width, const size_t p_height );

		template<typename T>
		inline void setUniform( const std::vector<T> & p_value, const std::string & p_key, const size_t p_index = 0 )
		{
			assert( _uniforms.find( p_key ) != _uniforms.end() );

			std::unique_ptr<_StructUniformEntry> & entry = _uniforms[ p_key ];
			T * const							   dest	 = reinterpret_cast<T * const>( entry->value );
			auto * const						   src	 = p_value.data();

			assert( src != nullptr && dest != nullptr && entry->size );

			memcpy( dest, src, entry->size );
			entry->buffer->setSubData( p_value, entry->offset + p_index * entry->totalSize );
		}

		template<typename T>
		inline void setUniform( const T & p_value, const std::string & p_key, const size_t p_index = 0 )
		{
			assert( _uniforms.find( p_key ) != _uniforms.end() );

			std::unique_ptr<_StructUniformEntry> & entry = _uniforms[ p_key ];
			T * const							   dest	 = reinterpret_cast<T * const>( entry->value );
			auto * const						   src	 = &p_value;

			assert( src != nullptr && dest != nullptr && entry->size );

			memcpy( dest, src, entry->size );
			entry->buffer->setSubData(
				p_value, entry->offset + p_index * entry->totalSize, GLsizei( entry->size /* + entry->padding*/ )
			);
		}

		template<typename T>
		inline void getUniform( T & p_value, const std::string & p_key, const size_t p_index = 0 )
		{
			assert( _uniforms.find( p_key ) != _uniforms.end() );

			std::unique_ptr<_StructUniformEntry> & entry = _uniforms[ p_key ];
			auto * const						   dest	 = &p_value;
			T * const							   src	 = reinterpret_cast<T * const>( entry->value );

			assert( src != nullptr && dest != nullptr && entry->size );

			memcpy( dest, src, entry->size );
		}

		template<typename T>
		inline void getUniform( std::vector<T> & p_value, const std::string & p_key, const size_t p_index = 0 )
		{
			// Cannot be done for the moment.
		}

		template<typename T>
		inline void setData( const std::vector<T> & p_data, const std::string & p_key )
		{
			assert( _bos.find( p_key ) != _bos.end() );

			_bos[ p_key ]->setData( p_data, GL_STATIC_DRAW );
		}

		inline void setData( const size_t p_size, const std::string & p_key )
		{
			assert( _bos.find( p_key ) != _bos.end() );

			_bos[ p_key ]->setData( GLsizei( p_size ), GL_STATIC_DRAW );
		}

		template<typename T>
		inline void setSubData( const std::vector<T> & p_data, const std::string & p_key, const size_t p_offset = 0 )

		{
			assert( _bos.find( p_key ) != _bos.end() );

			_bos[ p_key ]->setSubData( p_data, GLintptr( p_offset ) );
		}

		inline void setOutput( const Handle p_output ) { _output = p_output; }

		inline void fillInfos( StructInfos & p_infos ) const
		{
			p_infos.gpuMemoryInfoDedicated		  = _openglInfos.gpuMemoryInfoDedicatedVidmemNVX;
			p_infos.gpuMemoryInfoTotalAvailable	  = _openglInfos.gpuMemoryInfoTotalAvailableMemoryNVX;
			p_infos.gpuMemoryInfoCurrentAvailable = _openglInfos.gpuMemoryInfoCurrentAvailableVidMemNVX;
		}

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
			std::any &			   p_textureData,
			const size_t		   p_x,
			const size_t		   p_y,
			const std::string &	   p_pass,
			const E_CHANNEL_OUTPUT p_channel

		) const;

	  private:
		// TODO: find a better solution (magic enum explodes compile time).
		static std::map<const E_CHANNEL_OUTPUT, const GLenum> _mapAttachments;
		static std::map<const E_PRIMITIVE, const GLenum>	  _mapPrimitives;
		static std::map<const E_FORMAT, const GLenum>		  _mapFormats;
		static std::map<const E_WRAPPING, const GLint>		  _mapWrappings;
		static std::map<const E_FILTERING, const GLint>		  _mapFilterings;
		static std::map<const E_TYPE, const GLenum>			  _mapTypes;
		static std::map<const E_TYPE, const size_t>			  _mapTypeSizes;
		static std::map<const E_FORMAT, const E_TYPE>		  _mapFormatTypes;
		static std::map<const E_FORMAT, const GLenum>		  _mapFormatInternalTypes;

		std::unique_ptr<GL::ProgramManager>								  _programManager;
		std::unordered_map<std::string, std::unique_ptr<GL::VertexArray>> _vaos;
		std::unordered_map<std::string, std::unique_ptr<GL::Buffer>>	  _bos;
		std::vector<std::unique_ptr<GL::Buffer>>						  _ubosShared;

		// TODO: check if mapping is useful.
		std::unordered_map<const IO *, std::unique_ptr<GL::Texture2D>>	   _textures;
		std::unordered_map<const Program *, const GL::Program * const>	   _programs;
		std::unordered_map<const Pass *, std::unique_ptr<GL::Framebuffer>> _fbos;
		std::unordered_map<const Program *, std::unique_ptr<GL::Buffer>>   _ubos;

		struct _StructUniformEntry
		{
			GL::Buffer * buffer;
			size_t		 offset;
			size_t		 size;
			size_t		 padding;
			size_t		 totalSize;
			void *		 value;
			_StructUniformEntry(
				GL::Buffer * p_buffer,
				const size_t p_offset,
				const size_t p_size,
				const size_t p_padding,
				const size_t p_arraySize = 1
			) :
				buffer( p_buffer ),
				offset( p_offset ), size( p_size ), padding( p_padding ),
				// Store only first value.
				value( malloc( p_size ) )
			{
			}
			~_StructUniformEntry() { free( value ); }
		};
		std::unordered_map<std::string, std::unique_ptr<_StructUniformEntry>> _uniforms;

		// Output.
		Handle _output;

		// Specs.
		GL::StructOpenglInfos _openglInfos;

		void _createInputs( const Pass * const p_descPassPtr );

		void _createOuputs(
			const Pass * const	  p_descPassPtr,
			std::vector<GLenum> & p_drawBuffers,
			bool &				  p_hasDepthComponent
		);

		void _createAttachment( const IO & p_descIO );

		void _createUniforms(
			GL::Buffer * const	  p_ubo,
			const Uniforms &	  p_uniforms,
			const Program * const p_descProgram = nullptr,
			const Pass * const	  p_descPassPtr = nullptr
		);

		template<typename T>
		void _setUniformDefaultValue(
			const Uniform &		  p_descUniform,
			const Program * const p_descProgram = nullptr,
			const Pass * const	  p_descPassPtr = nullptr
		)
		{
			assert( std::holds_alternative<StructUniformValue<T>>( p_descUniform.value ) );

			std::string key = ( p_descPassPtr ? p_descPassPtr->name : "" )
							  + ( p_descProgram ? p_descProgram->name : "" ) + p_descUniform.name;

			setUniform<T>( std::get<StructUniformValue<T>>( p_descUniform.value ).value, key );
		}

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
