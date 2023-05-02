#ifndef __VTX_APP_RENDER_BUFFER_MOLECULE__
#define __VTX_APP_RENDER_BUFFER_MOLECULE__

#include "app/core/model/base_model.hpp"
#include "app/render/renderer/gl/vertex_array.hpp"
#include "base_buffer_opengl.hpp"
#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Render::Buffer
{
	class Molecule : public BaseBufferOpenGL
	{
	  public:
		Molecule()	= default;
		~Molecule() = default;

		void setAtomPositions( const std::vector<Vec3f> &, const bool );
		void setAtomRadius( const std::vector<float> & );
		void setAtomColors( const std::vector<Util::Color::Rgba> & );
		void setAtomVisibilities( const std::vector<uint> & );
		void setAtomSelections( const std::vector<uint> & );
		void setAtomIds( const std::vector<App::Core::Model::ID> & );
		void setBonds( const std::vector<uint> & );

		inline Renderer::GL::Buffer & getBufferPositions() { return _vboAtomPositions; }
		inline Renderer::GL::Buffer & getBufferRadius() { return _vboAtomRadius; }
		inline Renderer::GL::Buffer & getBufferColors() { return _vboAtomColors; }
		inline Renderer::GL::Buffer & getBufferVisibilities() { return _vboAtomVisibilities; }
		inline Renderer::GL::Buffer & getBufferSelections() { return _vboAtomSelections; }
		inline Renderer::GL::Buffer & getBufferIds() { return _vboAtomIds; }

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

		Renderer::GL::Buffer _vboAtomPositions	  = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboAtomRadius		  = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboAtomColors		  = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboAtomVisibilities = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboAtomSelections	  = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboAtomIds		  = Renderer::GL::Buffer();
		Renderer::GL::Buffer _iboBonds			  = Renderer::GL::Buffer();
	};
} // namespace VTX::App::Render::Buffer
#endif
