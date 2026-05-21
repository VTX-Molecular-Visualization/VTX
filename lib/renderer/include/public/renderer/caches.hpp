#ifndef __VTX_RENDERER_CACHES__
#define __VTX_RENDERER_CACHES__

#include "renderer/camera.hpp"
#include "renderer/types.hpp"
#include <core/struct/topology.hpp>
#include <util/math/bitset.hpp>
#include <util/types.hpp>
#include <vector>

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
		SystemUID				   uid;
		Mat4f					   transform;
		const Core::Struct::Topology * data = nullptr;
		std::vector<Vec3f>		   trajectory;
		std::vector<PickingUID>	   atomUids;
		std::vector<PickingUID>	   residueUids;
		std::vector<ColorIndex>	   atomColors;
		std::vector<RepresentationIndex> atomRepresentations;
		std::vector<Flag>		   atomFlags;
		Util::Math::BitSet		   visibility;
		MapRepresentationRanges	   representations;
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
