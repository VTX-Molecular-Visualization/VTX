#ifndef __VTX_RENDERER_CACHES__
#define __VTX_RENDERER_CACHES__

#include "renderer/camera.hpp"
#include "renderer/color.hpp"
#include "renderer/descriptors.hpp"
#include "renderer/graphics_config.hpp"
#include "renderer/representation.hpp"
#include <core/struct/topology.hpp>
#include <cstdint>
#include <span>
#include <unordered_map>
#include <util/ecs.hpp>
#include <util/math/bitset.hpp>
#include <util/types.hpp>
#include <util/uid.hpp>
#include <vector>

namespace VTX::Renderer
{
	// TODO: move to approriotate namespace.
	using Symbol		 = Core::ChemDB::Atom::SYMBOL;
	using Flag			 = uint8_t;
	using Indice		 = uint32_t;
	using IndexRange	 = Core::Struct::IndexRange;
	using IndexRangeList = Core::Struct::IndexRangeList;
	// TODO: move to approriotate namespace.
	enum struct E_ELEMENT_FLAGS : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		// Needed ?
		VISIBILITY = 1 << 0,
		SELECTION  = 1 << 1
	};

} // namespace VTX::Renderer

namespace VTX::Renderer::Cache
{
	struct Camera
	{
		VTX::Renderer::Camera camera;
		Vec3f				  position;
		Mat4f				  matView;
		Mat4f				  matProj;
	};

	struct ColorLayout
	{
		Color::Layout data;
	};

	struct GraphicsConfig
	{
		VTX::Renderer::GraphicsConfig data;
	};

	enum struct E_SYSTEM_DIRTY : uint16_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE		   = 0,
		STRUCTURE	   = 1 << 0,
		DELETING	   = 1 << 1,
		TRANSFORM	   = 1 << 2,
		TRAJECTORY	   = 1 << 3,
		COLOR		   = 1 << 4,
		REPRESENTATION = 1 << 5,
		VISIBILITY	   = 1 << 6,
		SELECTION	   = 1 << 7,
		ALL			   = STRUCTURE | TRANSFORM | TRAJECTORY | COLOR | REPRESENTATION | VISIBILITY | SELECTION
	};

	enum struct E_REPRESENTATION_DIRTY : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE	   = 0,
		PARAMETERS = 1 << 0,
		VISIBILITY = 1 << 1,
		ALL		   = PARAMETERS | VISIBILITY
	};

	enum struct E_RENDERER_DIRTY : uint16_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE			= 0,
		DRAW_RANGES		= 1 << 0,
		GEOMETRY_CHUNKS = 1 << 1,
		EXTERNAL_PASSES = 1 << 2,
		COMMAND_BUFFER	= 1 << 3,
		NEED_UPDATE		= 1 << 4,
		GRAPH			= 1 << 5,
		REPRESENTATIONS = 1 << 6,
		CAMERA			= 1 << 7,
		MODELS			= 1 << 8,
		COLOR_LAYOUT	= 1 << 9,
		GRAPHICS_CONFIG = 1 << 10,
		ALL = DRAW_RANGES | GEOMETRY_CHUNKS | EXTERNAL_PASSES | COMMAND_BUFFER | NEED_UPDATE | GRAPH | REPRESENTATIONS
			  | CAMERA | MODELS | COLOR_LAYOUT | GRAPHICS_CONFIG
	};

	struct System
	{
		// Owned.
		Mat4f transform;

		// Views.
		struct Data
		{
			const Core::Struct::Topology * topology;
			std::span<const Vec3f>		   trajectory;
			// TODO: delete and use gpu buffer?
			const Util::Math::Range<UID32> *						   atomUids;
			const Util::Math::Range<UID32> *						   residueUids;
			const std::unordered_map<E_COLOR_SCHEME, IndexRangeList> * colorSchemeAtoms;
			const std::unordered_map<ColorIndex, IndexRangeList> *	   customColorAtoms;
			// Ugly.
			const std::unordered_map<Entity, Desc::Handle> *				 representationHandles;
			const std::unordered_map<Entity, Core::Struct::IndexRangeList> * presetAtoms;
			const Util::Math::BitSet *										 visibility;
			const Util::Math::BitSet *										 selection;
		};

		Data data;

		// Computed.
		uint32_t modelIndex = 0;
	};

	struct Representation
	{
		// View.
		struct Data
		{
			const VTX::Renderer::Representation * rep;
		};

		Data data;

		// Computed.
		bool showSphere;
		bool showCylinder;
		bool showRibbon;
		bool showSes;
	};
} // namespace VTX::Renderer::Cache

#endif
