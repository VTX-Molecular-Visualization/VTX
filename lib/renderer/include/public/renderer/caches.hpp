#ifndef __VTX_RENDERER_CACHES__
#define __VTX_RENDERER_CACHES__

#include "renderer/camera.hpp"
#include "renderer/types.hpp"
#include <core/struct/system.hpp>
#include <unordered_map>
#include <util/types.hpp>

namespace VTX::Renderer
{
	using MapRepresentationRanges = std::unordered_map<RepresentationIndex, Core::Struct::IndexRangeList>;
}

namespace VTX::Renderer::Cache
{
	struct Camera
	{
		VTX::Renderer::Camera camera;
		Vec3f				  position;
		Mat4f				  matView;
		Mat4f				  matProj;
	};

	struct System
	{
		Mat4f					transform;
		ModelIndex				modelIndex;
		MapRepresentationRanges representationAtomsRanges;
		MapRepresentationRanges representationBondsRanges;
		MapRepresentationRanges representationResiduesRanges;
	};

	struct Representation
	{
		bool showSphere;
		bool showCylinder;
		bool showRibbon = false;
		bool showSes	= false;
	};
} // namespace VTX::Renderer::Cache

#endif
