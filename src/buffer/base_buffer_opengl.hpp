#ifndef __VTX_BASE_BUFFER_OPENGL__
#define __VTX_BASE_BUFFER_OPENGL__

#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "renderer/gl/vertex_array.hpp"

namespace VTX
{
	namespace Buffer
	{
		class BaseBufferOpenGL : public Generic::BaseOpenGL
		{
		  public:
			BaseBufferOpenGL( OpenGLFunctions * const p_gl ) :
				BaseOpenGL( p_gl ), _vboAABB( p_gl ), _iboAABB( p_gl ), _vaoAABB( p_gl )
			{
			}
			virtual ~BaseBufferOpenGL() {};

			virtual const Renderer::GL::VertexArray & getVao() const = 0;
			const Renderer::GL::VertexArray &		  getAABBVao() const { return _vaoAABB; }

			virtual void generate()
			{
				_vboAABB.create();
				_iboAABB.create();

				_vaoAABB.create();

				_vaoAABB.bindElementBuffer( _iboAABB );

				_vaoAABB.enableAttribute( ATTRIBUTE_AABB_LOCATION::AABB_CENTER );
				_vaoAABB.setVertexBuffer( ATTRIBUTE_AABB_LOCATION::AABB_CENTER, _vboAABB, 0, sizeof( Vec3f ) );
				_vaoAABB.setAttributeFormat(
					ATTRIBUTE_AABB_LOCATION::AABB_CENTER, 3, Renderer::GL::VertexArray::Type::FLOAT );
				_vaoAABB.setAttributeBinding( ATTRIBUTE_AABB_LOCATION::AABB_CENTER,
											  ATTRIBUTE_AABB_LOCATION::AABB_CENTER );

				_generate();
			}

			virtual void bindAABB() { _vaoAABB.bind(); }

			virtual void unbindAABB() { _vaoAABB.unbind(); }

			void setAABBCorners( const std::vector<Vec3f> & p_positions )
			{
				_vboAABB.set<Vec3f>( p_positions, Renderer::GL::Buffer::Usage::STATIC_DRAW );
			}

			void setAABBIndices( const std::vector<uint> & p_indices )
			{
				_iboAABB.set<uint>( p_indices, Renderer::GL::Buffer::Usage::STATIC_DRAW );
			}

			virtual void bind()	  = 0;
			virtual void unbind() = 0;

		  protected:
			virtual void _generate() = 0;

		  private:
			enum ATTRIBUTE_AABB_LOCATION
			{
				AABB_CENTER = 0
			};

			Renderer::GL::Buffer	  _vboAABB;
			Renderer::GL::Buffer	  _iboAABB;
			Renderer::GL::VertexArray _vaoAABB;
		};
	} // namespace Buffer
} // namespace VTX
#endif
