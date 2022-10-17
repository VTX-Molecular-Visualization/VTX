#ifndef __VTX_BUFFER_SECONDARY_STRUCTURE__
#define __VTX_BUFFER_SECONDARY_STRUCTURE__

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
		class SecondaryStructure : public BaseBufferOpenGL
		{
		  public:
			SecondaryStructure()  = default;
			~SecondaryStructure() = default;

			void setControlPointPositions( const std::vector<Vec4f> & );
			void setControlPointDirections( const std::vector<Vec3f> & );
			void setSecondaryStructures( const std::vector<uint> & );
			void setColors( const std::vector<Color::Rgb> & );
			void setVisibilities( const std::vector<uint> & );
			void setSelections( const std::vector<uint> & );
			void setIds( const std::vector<Model::ID> & );
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

			Renderer::GL::BufferData _vboPositions			 = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboDirections			 = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboSecondaryStructures = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboColors				 = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboVisibilities		 = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboSelections			 = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboIds				 = Renderer::GL::BufferData();
			Renderer::GL::BufferData _ibo					 = Renderer::GL::BufferData();
		};
	} // namespace Buffer
} // namespace VTX
#endif
