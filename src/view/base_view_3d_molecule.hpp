#ifndef __VTX_BASE_VIEW_3D_MOLECULE__
#define __VTX_BASE_VIEW_3D_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_molecule.hpp"
#include "base_view_3d.hpp"

namespace VTX
{
	namespace View
	{
		class BaseView3DMolecule : public BaseView3D<Model::ModelMolecule>
		{
		  public:
			enum class REPRESENTATION
			{
				BALL_AND_STICK
			};

			BaseView3DMolecule() {};
			~BaseView3DMolecule() = default;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
