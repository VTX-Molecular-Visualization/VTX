#ifndef __VTX_BUFFER_SECONDARY_STRUCTURE__
#define __VTX_BUFFER_SECONDARY_STRUCTURE__

#include "base_buffer_opengl.hpp"
#include "color/rgb.hpp"
#include "define.hpp"
#include "renderer/gl/vertex_array.hpp"
#include <vector>

namespace VTX
{
	namespace Buffer
	{
		class SecondaryStructure : public BaseBufferOpenGL
		{
		  public:
			SecondaryStructure()  = default;
			~SecondaryStructure() = default;

			const Renderer::GL::VertexArray & getVao() const override { return _vao; }

			void generate() override;
			void bind() override;
			void unbind() override;

			void setControlPointPositions( const std::vector<Vec4f> & );
			void setControlPointDirections( const std::vector<Vec3f> & );
			void setControlPointSecondaryStructure( const std::vector<uint> & );
			void setControlPointColors( const std::vector<Color::Rgb> & );
			void setControlPointVisibilities( const std::vector<uint> & );
			void setControlPointSelections( const std::vector<uint> & );
			void setIndices( const std::vector<uint> & );

		  private:
			enum ATTRIBUTE_LOCATION
			{
				CONTROL_POINT_POSITION			  = 0,
				CONTROL_POINT_DIRECTION			  = 1,
				CONTROL_POINT_SECONDARY_STRUCTURE = 2,
				CONTROL_POINT_COLOR				  = 3,
				CONTROL_POINT_VISIBILITY		  = 4,
				CONTROL_POINT_SELECTION			  = 5,
			};

			Renderer::GL::Buffer	  _vboPositions			  = Renderer::GL::Buffer();
			Renderer::GL::Buffer	  _vboDirections		  = Renderer::GL::Buffer();
			Renderer::GL::Buffer	  _vboSecondaryStructures = Renderer::GL::Buffer();
			Renderer::GL::Buffer	  _vboColors			  = Renderer::GL::Buffer();
			Renderer::GL::Buffer	  _vboVisibilities		  = Renderer::GL::Buffer();
			Renderer::GL::Buffer	  _vboSelections		  = Renderer::GL::Buffer();
			Renderer::GL::Buffer	  _ibo					  = Renderer::GL::Buffer();
			Renderer::GL::VertexArray _vao					  = Renderer::GL::VertexArray();
		};
	} // namespace Buffer
} // namespace VTX
#endif
