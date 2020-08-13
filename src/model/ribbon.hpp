#ifndef __VTX_MODEL_RIBBON__
#define __VTX_MODEL_RIBBON__

#ifdef _MSC_VER
#pragma once
#endif

#include "math/bspline.hpp"
#include "mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "util/logger.hpp"
#include "util/math.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Ribbon : public MeshTriangle
		{
		  public:
			static constexpr float HELIX_DIAM  = 1.8f;
			static constexpr float THICKNESS   = 0.3f;
			static constexpr float ARROW_WIDTH = 1.8f;
			static constexpr bool  ARROW	   = false;

			// Move to config.
			static constexpr float RIBBON_WIDTH[ 3 ] = { 0.3f, 1.5f, 1.7f };
			static constexpr float HELIX_WIDTH		 = 1.5f;
			static constexpr float STRAND_WIDTH		 = 1.7f;
			static constexpr float COIL_WIDTH		 = 0.3f;

			static constexpr uint DETAIL_LEVEL = 10;

			Ribbon( Molecule * const );

			inline Molecule * const getMolecule() const { return _molecule; }
			void					refreshVisibility();

		  private:
			static constexpr int LHANDED = -1;
			static constexpr int RHANDED = 1;

			Molecule * const _molecule;

			void _addControlPoints( const Vec3f &,
									const Vec3f &,
									const Vec3f &,
									const Residue::SECONDARY_STRUCTURE,
									const Residue::HANDEDNESS,
									Vec3f &,
									Math::BSpline &,
									Math::BSpline &,
									Math::BSpline & );

			void _computeTriangleMesh( Math::BSpline &,
									   Math::BSpline &,
									   Math::BSpline &,
									   const Color::Rgb &,
									   const bool,
									   uint &,
									   uint & );
		};

	} // namespace Model
} // namespace VTX
#endif
