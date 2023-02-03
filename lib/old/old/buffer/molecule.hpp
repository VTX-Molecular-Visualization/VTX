#ifndef __VTX_BUFFER_MOLECULE__
#define __VTX_BUFFER_MOLECULE__

#include "base_buffer_opengl.hpp"
#include "color/rgb.hpp"
#include "define.hpp"
#include "model/base_model.hpp"
#include "renderer/gl/vertex_array.hpp"
#include <vector>

namespace VTX
{
	namespace Buffer
	{
		class Molecule : public BaseBufferOpenGL
		{
		  public:
			Molecule()	= default;
			~Molecule() = default;

			void setAtomPositions( const std::vector<Vec3f> & );
			void setAtomRadius( const std::vector<float> & );
			void setAtomColors( const std::vector<Color::Rgb> & );
			void setAtomVisibilities( const std::vector<uint> & );
			void setAtomSelections( const std::vector<uint> & );
			void setAtomIds( const std::vector<Model::ID> & );
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
				ATOM_ID			= 5,
			};

			Renderer::GL::BufferData _vboAtomPositions	  = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboAtomRadii		  = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboAtomColors		  = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboAtomVisibilities = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboAtomSelections	  = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboAtomIds		  = Renderer::GL::BufferData();
			Renderer::GL::BufferData _iboBonds			  = Renderer::GL::BufferData();
		};
	} // namespace Buffer
} // namespace VTX
#endif
