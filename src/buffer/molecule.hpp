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

			void generate() override;
			void free() override;

			void bind() override;
			void unbind() override;

			void setAtomPositions( const std::vector<Vec3f> & );
			void setAtomRadius( const std::vector<float> & );
			void setAtomColors( const std::vector<Color::Rgb> & );
			void setAtomVisibilities( const std::vector<ushort> & );
			void setAtomSelections( const std::vector<ushort> & );
			void setBonds( const std::vector<uint> & );

			void refreshAtomBufferCacheSize( const uint p_atomCount )
			{
				_bufferAtomRadius.resize( p_atomCount );
				_bufferAtomColors.resize( p_atomCount );
				_bufferAtomVisibilities.resize( p_atomCount, 0 );
				_bufferAtomSelection.resize( p_atomCount );
			}
			void refreshBondBufferCacheSize( const uint p_bondCount ) { _bufferBonds.resize( p_bondCount * 2 ); }

			std::vector<Color::Rgb> & getColorBufferCache() const { return _bufferAtomColors; };

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

			// Buffer caches
			mutable std::vector<float>		_bufferAtomRadius		= std::vector<float>();
			mutable std::vector<Color::Rgb> _bufferAtomColors		= std::vector<Color::Rgb>();
			mutable std::vector<ushort>		_bufferAtomVisibilities = std::vector<ushort>();
			mutable std::vector<ushort>		_bufferAtomSelection	= std::vector<ushort>();
			mutable std::vector<uint>		_bufferBonds			= std::vector<uint>();
		};
	} // namespace Buffer
} // namespace VTX
#endif
