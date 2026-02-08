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
		Geometry::SES	   ses;

		void buildDrawRanges()
		{
			spheres.buildDrawRanges();
			cylinders.buildDrawRanges();
			// ribbons.buildDrawRanges();
			// voxels.buildDrawRanges();
			// ses.buildDrawRanges();
		}
	};
} // namespace VTX::Renderer

#endif
