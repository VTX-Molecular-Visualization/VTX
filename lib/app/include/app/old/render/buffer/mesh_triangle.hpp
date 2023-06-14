#ifndef __VTX_APP_OLD_RENDER_BUFFER_MESH_TRIANGLE__
#define __VTX_APP_OLD_RENDER_BUFFER_MESH_TRIANGLE__

#include "app/old/core/model/base_model.hpp"
#include "app/old/render/renderer/gl/vertex_array.hpp"
#include "base_buffer_opengl.hpp"
#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Render::Buffer
{
	class MeshTriangle : public BaseBufferOpenGL
	{
	  public:
		MeshTriangle()	= default;
		~MeshTriangle() = default;

		void setPositions( const std::vector<Vec4f> & );
		void setNormals( const std::vector<Vec4f> & );
		void setColors( const std::vector<Util::Color::Rgba> & );
		void setVisibilities( const std::vector<uint> & );
		void setSelections( const std::vector<uint> & );
		void setIds( const std::vector<App::Old::Core::Model::ID> & );
		void setIndices( const std::vector<uint> & );

		inline Renderer::GL::Buffer & getBufferPositions() { return _vboPositions; }
		inline Renderer::GL::Buffer & getBufferNormals() { return _vboNormals; }
		inline Renderer::GL::Buffer & getBufferColors() { return _vboColors; }
		inline Renderer::GL::Buffer & getBufferVisibilities() { return _vboVisibilities; }
		inline Renderer::GL::Buffer & getBufferSelections() { return _vboSelections; }
		inline Renderer::GL::Buffer & getBufferIds() { return _vboIds; }
		inline Renderer::GL::Buffer & getBufferIndices() { return _ibo; }
		inline Renderer::GL::Buffer & getBufferAtomsToTriangles() { return _bufferAtomsToTriangles; }

	  protected:
		void _generate() override;

	  private:
		enum ATTRIBUTE_LOCATION
		{
			VERTEX_POSITION	  = 0,
			VERTEX_NORMAL	  = 1,
			VERTEX_COLOR	  = 2,
			VERTEX_VISIBILITY = 3,
			VERTEX_SELECTION  = 4,
			VERTEX_ID		  = 5,
		};

		Renderer::GL::Buffer _vboPositions	  = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboNormals	  = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboColors		  = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboVisibilities = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboSelections	  = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboIds		  = Renderer::GL::Buffer();
		Renderer::GL::Buffer _ibo			  = Renderer::GL::Buffer();

		Renderer::GL::Buffer _bufferAtomsToTriangles = Renderer::GL::Buffer();
	};
} // namespace VTX::App::Old::Render::Buffer
#endif
