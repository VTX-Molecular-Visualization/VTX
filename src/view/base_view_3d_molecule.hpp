#ifndef __VTX_BASE_VIEW_3D_MOLECULE__
#define __VTX_BASE_VIEW_3D_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_molecule.hpp"
#include "base_view_3d.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GL/gl3w.h>
#include <vector>

namespace VTX
{
	namespace View
	{
		enum class MOLECULE_COLOR_MODE : int
		{
			ATOM,
			RESIDUE,
			CHAIN
		};

		enum class MOLECULE_REPRESENTATION : int
		{
			BALL_AND_STICK,
			VAN_DER_WAALS,
			STICK
		};

		class BaseView3DMolecule : public BaseView3D<Model::ModelMolecule>
		{
		  public:
			explicit BaseView3DMolecule( Model::ModelMolecule * const p_model ) : BaseView3D( p_model ) {}

			virtual void notify( Event::EVENT_MODEL p_event ) override;
		};
	} // namespace View
} // namespace VTX
#endif
