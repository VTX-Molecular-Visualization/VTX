#ifndef __VTX_APP_OLD_RENDER_BUFFER_SECONDARY_STRUCTURE__
#define __VTX_APP_OLD_RENDER_BUFFER_SECONDARY_STRUCTURE__

#include "app/old/core/model/base_model.hpp"
#include "app/old/render/renderer/gl/vertex_array.hpp"
#include "base_buffer_opengl.hpp"
#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Render::Buffer
{
	class SecondaryStructure : public BaseBufferOpenGL
	{
	  public:
		SecondaryStructure()  = default;
		~SecondaryStructure() = default;

		void setControlPointPositions( const std::vector<Vec4f> & );
		void setControlPointDirections( const std::vector<Vec3f> & );
		void setSecondaryStructures( const std::vector<uint> & );
		void setColors( const std::vector<Util::Color::Rgba> & );
		void setVisibilities( const std::vector<uint> & );
		void setSelections( const std::vector<uint> & );
		void setIds( const std::vector<App::Old::Core::Model::ID> & );
		void setIndices( const std::vector<uint> & );

	  protected:
		void _generate() override;

	  private:
		enum ATTRIBUTE_LOCATION
		{
			CONTROL_POINT_POSITION		= 0,
			CONTROL_POINT_DIRECTION		= 1,
			RESIDUE_SECONDARY_STRUCTURE = 2,
			RESIDUE_COLOR				= 3,
			RESIDUE_VISIBILITY			= 4,
			RESIDUE_SELECTION			= 5,
			RESIDUE_ID					= 6,
		};

		Renderer::GL::Buffer _vboPositions			 = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboDirections			 = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboSecondaryStructures = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboColors				 = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboVisibilities		 = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboSelections			 = Renderer::GL::Buffer();
		Renderer::GL::Buffer _vboIds				 = Renderer::GL::Buffer();
		Renderer::GL::Buffer _ibo					 = Renderer::GL::Buffer();
	};
} // namespace VTX::App::Old::Render::Buffer
#endif
