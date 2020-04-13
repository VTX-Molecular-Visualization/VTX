#ifndef __VTX_BASE_VIEW_3D_MOLECULE__
#define __VTX_BASE_VIEW_3D_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view_3d.hpp"
#include "model/molecule.hpp"
#include <vector>

namespace VTX
{
	namespace View
	{
		enum class MOLECULE_COLOR_MODE : int
		{
			ATOM,
			RESIDUE,
			CHAIN,
			PROTEIN
		};

		enum class MOLECULE_REPRESENTATION : int
		{
			BALL_AND_STICK,
			VAN_DER_WAALS,
			STICK,
			SAS
		};

		class BaseView3DMolecule : public BaseView3D<Model::Molecule>
		{
		  public:
			explicit BaseView3DMolecule( Model::Molecule * const p_model ) : BaseView3D( p_model ) {}
			virtual ~BaseView3DMolecule() {}
		};
	} // namespace View
} // namespace VTX
#endif
