#ifndef __VTX_BUFFER_MOLECULE__
#define __VTX_BUFFER_MOLECULE__

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
		class Molecule : public BaseBufferOpenGL
		{
		  public:
			Molecule( OpenGLFunctions * const p_gl ) :
				BaseBufferOpenGL( p_gl ), _vboAtomPositions( p_gl ), _vboAtomRadii( p_gl ), _vboAtomColors( p_gl ),
				_vboAtomVisibilities( p_gl ), _vboAtomSelections( p_gl ), _iboBonds( p_gl ), _vao( p_gl ) {};
			~Molecule() = default;

			const Renderer::GL::VertexArray & getVao() const override { return _vao; }

			void bind() override;
			void unbind() override;

			void setAtomPositions( const std::vector<Vec3f> & );
			void setAtomRadius( const std::vector<float> & );
			void setAtomColors( const std::vector<Color::Rgb> & );
			void setAtomVisibilities( const std::vector<ushort> & );
			void setAtomSelections( const std::vector<ushort> & );
			void setBonds( const std::vector<uint> & );

		  protected:
			void _generate() override;

		  private:
			enum ATTRIBUTE_LOCATION
			{
				ATOM_POSITION	= 0,
				ATOM_COLOR		= 1,
				ATOM_RADIUS		= 2,
				ATOM_VISIBILITY = 3,
				ATOM_SELECTION	= 4,
			};

			Renderer::GL::Buffer	  _vboAtomPositions;
			Renderer::GL::Buffer	  _vboAtomRadii;
			Renderer::GL::Buffer	  _vboAtomColors;
			Renderer::GL::Buffer	  _vboAtomVisibilities;
			Renderer::GL::Buffer	  _vboAtomSelections;
			Renderer::GL::Buffer	  _iboBonds;
			Renderer::GL::VertexArray _vao;
		};
	} // namespace Buffer
} // namespace VTX
#endif
