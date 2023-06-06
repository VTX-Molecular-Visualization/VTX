#ifndef __VTX_CORE_STRUCT_MOLECULE__
#define __VTX_CORE_STRUCT_MOLECULE__

#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core
{
	struct StructMolecule
	{
		Mat4f						   tranform;
		std::vector<Vec3f>			   atomPositions;
		std::vector<Util::Color::Rgba> atomColors;
		std::vector<float>			   atomRadii;
		std::vector<uint>			   atomVisibilities;
		std::vector<uint>			   atomSelections;
		std::vector<uint>			   atomIds;
		std::vector<uint>			   bonds;
	};

	static const StructMolecule DEFAULT_MOLECULE
		= StructMolecule { MAT4F_ID,
						   { Vec3f( -2.f, 0.f, 0.f ), Vec3f( 2.f, 0.f, 0.f ), Vec3f( 0.f, 1.f, 0.f ) },
						   { Util::Color::Rgba::random(), Util::Color::Rgba::random(), Util::Color::Rgba::random() },
						   { 0.5f, 0.3f, 0.4 },
						   { 1, 1, 1 },
						   { 0, 0, 0 },
						   { 0, 0, 0 },
						   { 0, 1, 1, 2, 2, 0 }

		  };
} // namespace VTX::Core

#endif
