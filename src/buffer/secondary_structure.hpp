#ifndef __VTX_BUFFER_SECONDARY_STRUCTURE__
#define __VTX_BUFFER_SECONDARY_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_buffer_opengl.hpp"
#include "color/rgb.hpp"
#include "define.hpp"
#include <vector>

namespace VTX
{
	namespace Buffer
	{
		class SecondaryStructure : public BaseBufferOpenGL
		{
		  public:
			SecondaryStructure( OpenGLFunctions * const p_gl ) : BaseBufferOpenGL( p_gl ) {};
			~SecondaryStructure() = default;

			void bind() override;
			void unbind() override;

			void setControlPointPositions( const std::vector<Vec3f> & );
			void setControlPointDirections( const std::vector<Vec3f> & );
			void setControlPointNormals( const std::vector<Vec3f> & );
			void setControlPointSecondaryStructure( const std::vector<ushort> & );
			void setControlPointColors( const std::vector<Color::Rgb> & );
			void setControlPointSelections( const std::vector<ushort> & );
			void setIndices( const std::vector<uint> & );

		  protected:
			void _generate() override;
			void _free() override;

		  private:
			enum ATTRIBUTE_LOCATION
			{
				CONTROL_POINT_POSITION			  = 0,
				CONTROL_POINT_DIRECTION			  = 1,
				CONTROL_POINT_NORMAL			  = 2,
				CONTROL_POINT_SECONDARY_STRUCTURE = 3,
				CONTROL_POINT_COLOR				  = 4,
				CONTROL_POINT_SELECTION			  = 5,
			};

			GLuint _vboPositions		   = GL_INVALID_VALUE;
			GLuint _vboDirections		   = GL_INVALID_VALUE;
			GLuint _vboNormals			   = GL_INVALID_VALUE;
			GLuint _vboSecondaryStructures = GL_INVALID_VALUE;
			GLuint _vboColors			   = GL_INVALID_VALUE;
			GLuint _vboSelections		   = GL_INVALID_VALUE;
			GLuint _ibo					   = GL_INVALID_VALUE;
			GLuint _vao					   = GL_INVALID_VALUE;
		};
	} // namespace Buffer
} // namespace VTX
#endif
