#ifndef __VTX_RENDERER_GEOMETRIES__
#define __VTX_RENDERER_GEOMETRIES__

#include "renderer/geometry/cylinder.hpp"
#include "renderer/geometry/ribbon.hpp"
#include "renderer/geometry/ses.hpp"
#include "renderer/geometry/sphere.hpp"
#include "renderer/geometry/voxel.hpp"

namespace VTX::Renderer
{
	struct Geometries
	{
		Geometry::Sphere   spheres;
		Geometry::Cylinder cylinders;
		Geometry::Ribbon   ribbons;
		Geometry::Voxel	   voxels;
		//  Geometry::SES	   ses;

		void construct( const SystemData & p_data )
		{
			spheres.construct( p_data );
			cylinders.construct( p_data );
			ribbons.construct( p_data );
		}

		void buildDrawRanges()
		{
			spheres.buildDrawRanges();
			cylinders.buildDrawRanges();
			ribbons.buildDrawRanges();
		}
	};
} // namespace VTX::Renderer

#endif
