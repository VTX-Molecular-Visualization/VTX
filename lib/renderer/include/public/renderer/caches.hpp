#ifndef __VTX_RENDERER_CACHES__
#define __VTX_RENDERER_CACHES__

#include "renderer/camera.hpp"
#include "renderer/color.hpp"
#include "renderer/representation.hpp"
#include <core/struct/topology.hpp>
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

	enum struct E_SYSTEM_DIRTY : uint16_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE		   = 0,
		STRUCTURE	   = 1 << 0,
		TRANSFORM	   = 1 << 1,
		TRAJECTORY	   = 1 << 2,
		COLOR		   = 1 << 3,
		REPRESENTATION = 1 << 4,
		VISIBILITY	   = 1 << 5,
		SELECTION	   = 1 << 6,
		ALL			   = 0xFFFF
	};

	enum struct E_REPRESENTATION_DIRTY : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE	   = 0,
		PARAMETERS = 1 << 0,
		VISIBILITY = 1 << 1,
		ALL		   = 0xFF
	};

	enum struct E_RENDERER_DIRTY : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE			= 0,
		DRAW_RANGES		= 1 << 0,
		GEOMETRY_CHUNKS = 1 << 1,
		EXTERNAL_PASSES = 1 << 2,
		COMMAND_BUFFER	= 1 << 3,
		NEED_UPDATE		= 1 << 4,
		GRAPH			= 1 << 5,
		ALL				= 0xFF
	};

	struct System
	{
		// Owned.
		Mat4f transform;

		// Views.
		const Core::Struct::Topology &									 data;
		std::span<const Vec3f>											 trajectory;
		const Util::Math::Range<UID32> &								 atomUids; // TODO: delete and use gpu buffer?
		const Util::Math::Range<UID32> &								 residueUids;
		const std::unordered_map<E_COLOR_SCHEME, IndexRangeList> &		 colorSchemeAtoms;
		const std::unordered_map<ColorIndex, IndexRangeList> &			 customColorAtoms;
		const std::unordered_map<Entity, Core::Struct::IndexRangeList> & presetAtoms;
		const Util::Math::BitSet &										 visibility;
		const Util::Math::BitSet &										 selection;
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
