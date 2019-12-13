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
		enum class VIEW_3D_MOLECULE : int
		{
			SPHERE,
			CYLINDER
		};

		enum class MOLECULE_REPRESENTATION : int
		{
			BALL_AND_STICK,
			VAN_DER_WAALS
		};

		class BaseView3DMolecule : public BaseView3D<Model::ModelMolecule>
		{
		  public:
			BaseView3DMolecule()  = default;
			~BaseView3DMolecule() = default;

			virtual void notify( Event::EVENT_MODEL p_event ) override;

		  protected:
			bool _isActive = false;
		};
	} // namespace View
} // namespace VTX
#endif
