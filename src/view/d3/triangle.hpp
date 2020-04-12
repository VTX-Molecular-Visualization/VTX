#ifndef __VTX_VIEW_3D_TRIANGLE_MESH__
#define __VTX_VIEW_3D_TRIANGLE_MESH__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/mesh_triangle.hpp"
#include "view/base_view_3d.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Triangle : public BaseView3D<Model::MeshTriangle>
			{
			  public:
				explicit Triangle( Model::MeshTriangle * const p_model ) : BaseView3D( p_model ) {}

				virtual const std::string & getName() const override { return ID::View::D3_TRIANGLE; };
				virtual void				init() override;
				virtual void				notify( const Event::VTX_EVENT_MODEL & ) override;

				virtual void render() override;

			  private:
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
