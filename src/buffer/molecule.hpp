#ifndef __VTX_BUFFER_MOLECULE__
#define __VTX_BUFFER_MOLECULE__

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
		class Molecule : public BaseBufferOpenGL
		{
		  public:
			Molecule( QOpenGLFunctions_4_5_Core * const p_gl ) : BaseBufferOpenGL( p_gl ) {};
			~Molecule() = default;

			void generate() override;
			void free() override;

			void bind() override;
			void unbind() override;

			void setAtomPositions( const std::vector<Vec3f> & );
			void setAtomRadius( const std::vector<float> & );
			void setAtomColors( const std::vector<Color::Rgb> & );
			void setAtomVisibilities( const std::vector<uint> & );
			void setBonds( const std::vector<uint> & );

		  private:
			enum ATTRIBUTE_LOCATION
			{
				ATOM_POSITION	= 0,
				ATOM_COLOR		= 1,
				ATOM_RADIUS		= 2,
				ATOM_VISIBILITY = 3,
			};

			GLuint _vao					= GL_INVALID_VALUE;
			GLuint _atomPositionsVBO	= GL_INVALID_VALUE;
			GLuint _atomRadiusVBO		= GL_INVALID_VALUE;
			GLuint _atomColorsVBO		= GL_INVALID_VALUE;
			GLuint _atomVisibilitiesVBO = GL_INVALID_VALUE;
			GLuint _bondsIBO			= GL_INVALID_VALUE;
		};
	} // namespace Buffer
} // namespace VTX
#endif
