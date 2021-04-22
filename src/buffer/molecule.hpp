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
			Molecule( OpenGLFunctions * const p_gl ) : BaseBufferOpenGL( p_gl ) {};
			~Molecule() = default;

			void bind() override;
			void unbind() override;

			void setAtomPositions( const std::vector<Vec3f> & );
			void setAtomRadius( const std::vector<float> & );
			void setAtomColors( const std::vector<Color::Rgb> & );
			void setAtomVisibilities( const std::vector<uint> & );
			void setAtomSelections( const std::vector<uint> & );
			void setBonds( const std::vector<uint> & );

		  protected:
			void _generate() override;
			void _free() override;

		  private:
			enum ATTRIBUTE_LOCATION
			{
				ATOM_POSITION	= 0,
				ATOM_COLOR		= 1,
				ATOM_RADIUS		= 2,
				ATOM_VISIBILITY = 3,
				ATOM_SELECTION	= 4,
			};

			GLuint _vboAtomPositions	= GL_INVALID_VALUE;
			GLuint _vboAtomRadius		= GL_INVALID_VALUE;
			GLuint _vboAtomColors		= GL_INVALID_VALUE;
			GLuint _vboAtomVisibilities = GL_INVALID_VALUE;
			GLuint _vboAtomSelections	= GL_INVALID_VALUE;
			GLuint _iboBonds			= GL_INVALID_VALUE;
			GLuint _vao					= GL_INVALID_VALUE;
		};
	} // namespace Buffer
} // namespace VTX
#endif
