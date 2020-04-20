#ifndef __VTX_MODEL_RIBBONS__
#define __VTX_MODEL_RIBBONS__

#ifdef _MSC_VER
#pragma once
#endif

#include "math/bspline.hpp"
#include "mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "util/logger.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Model
	{
		class Ribbons : public MeshTriangle
		{
		  public:
			// TODO: move in settings, or modifiable member.
			static constexpr uint RIBBON_DETAIL = 4u;
			static constexpr uint RENDER_MODE	= 1u;

			static constexpr float HELIX_DIAM  = 1.8f;
			static constexpr float THICKNESS   = 0.3f;
			static constexpr float ARROW_WIDTH = 1.8f;

			static constexpr float RIBBON_WIDTH[ 3 ] = { 0.3f, 1.5f, 1.7f };
			static constexpr float HELIX_WIDTH		 = 1.5f;
			static constexpr float STRAND_WIDTH		 = 1.7f;
			static constexpr float COIL_WIDTH		 = 0.3f;

			/*
			static constexpr Vec3f HELIX_COLOR	= Vec3f( 1.f, 0.f, 0.f );
			static constexpr Vec3f STRAND_COLOR = Vec3f( 0.f, 0.f, 1.f );
			static constexpr Vec3f COIL_COLOR	= Vec3f( 1.f, 1.f, 1.f );

			static constexpr Vec3f CHAIN_COLOR_A = Vec3f( 10.f / 256.f, 3.f / 256.f, 200.f / 256.f );
			static constexpr Vec3f CHAIN_COLOR_B = Vec3f( 10.f / 256.f, 170.f / 256.f, 34.f / 256.f );
			static constexpr Vec3f CHAIN_COLOR_C = Vec3f( 0.f, 150.f / 256.f, 150.f / 256.f );
			static constexpr Vec3f CHAIN_COLOR_D = Vec3f( 0.f, 180.f / 256.f, 0.f );
			static constexpr Vec3f CHAIN_COLOR_E = Vec3f( 0.f, 170.f / 256.f, 200.f / 256.f );
			*/

			Ribbons( const Molecule & );

		  private:
			static constexpr int LHANDED = -1;
			static constexpr int RHANDED = 1;

			void _addControlPoints( const Vec3f &,
									const Vec3f &,
									const Vec3f &,
									const Residue::SECONDARY_STRUCTURE,
									Vec3f &,
									Math::BSpline &,
									Math::BSpline &,
									Math::BSpline & );
		};

	} // namespace Model
} // namespace VTX
#endif
