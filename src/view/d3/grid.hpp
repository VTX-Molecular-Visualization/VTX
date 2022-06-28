#ifndef __VTX_VIEW_3D_GRID__
#define __VTX_VIEW_3D_GRID__

#include "math/transform.hpp"
#include "view/base_view_3d.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
	}

	namespace View
	{
		namespace D3
		{
			struct Voxel
			{
				VTX::Vec3f min;
				VTX::Vec3f max;
				uint	   pad[ 2 ];
			};

			enum class GRID_ATTRIBUTE_LOCATION : uint8_t
			{
				VOXEL_MIN = 0,
				VOXEL_MAX = 1
			};

			class Grid : public BaseView3D
			{
			  public:
				void render( const Object3D::Camera & p_camera ) const override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;
				void						  _init() override;
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif // __VTX_VIEW_3D_MOLECULE_GRID__
