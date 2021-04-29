#ifndef __VTX_BUFFER_SECONDARY_STRUCTURE__
#define __VTX_BUFFER_SECONDARY_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

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
			SecondaryStructure( OpenGLFunctions * const p_gl ) :
				BaseBufferOpenGL( p_gl ), _vboPositions( p_gl ), _vboDirections( p_gl ),
				_vboSecondaryStructures( p_gl ), _vboColors( p_gl ), _vboSelections( p_gl ), _ibo( p_gl ),
				_vao( p_gl ) {};
			~SecondaryStructure() = default;

			const Renderer::GL::VertexArray & getVao() const override { return _vao; }

			void bind() override;
			void unbind() override;

			void setControlPointPositions( const std::vector<Vec4f> & );
			void setControlPointDirections( const std::vector<Vec3f> & );
			void setControlPointSecondaryStructure( const std::vector<uint> & );
			void setControlPointColors( const std::vector<Color::Rgb> & );
			void setControlPointSelections( const std::vector<uint> & );
			void setIndices( const std::vector<uint> & );

		  protected:
			void _generate() override;

		  private:
			enum ATTRIBUTE_LOCATION
			{
				CONTROL_POINT_POSITION			  = 0,
				CONTROL_POINT_DIRECTION			  = 1,
				CONTROL_POINT_SECONDARY_STRUCTURE = 2,
				CONTROL_POINT_COLOR				  = 3,
				CONTROL_POINT_SELECTION			  = 4,
			};

			Renderer::GL::Buffer	  _vboPositions;
			Renderer::GL::Buffer	  _vboDirections;
			Renderer::GL::Buffer	  _vboSecondaryStructures;
			Renderer::GL::Buffer	  _vboColors;
			Renderer::GL::Buffer	  _vboSelections;
			Renderer::GL::Buffer	  _ibo;
			Renderer::GL::VertexArray _vao;
		};
	} // namespace Buffer
} // namespace VTX
#endif
