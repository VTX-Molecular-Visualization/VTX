#ifndef __VTX_BASE_VIEW_3D_MOLECULE__
#define __VTX_BASE_VIEW_3D_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view_3d.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "model/molecule.hpp"
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
			CHAIN,
			PROTEIN
		};

		enum class MOLECULE_REPRESENTATION : int
		{
			BALL_AND_STICK,
			VAN_DER_WAALS,
			STICK
		};

		class BaseView3DMolecule : public BaseView3D<Model::Molecule>
		{
		  public:
			explicit BaseView3DMolecule( Model::Molecule * const p_model ) : BaseView3D( p_model ) {}
			virtual ~BaseView3DMolecule() {}

			virtual void notify( const Event::VTX_EVENT_MODEL & p_event ) override
			{
				if ( p_event == Event::VTX_EVENT_MODEL::RENDER && _isActive ) { render(); }
			}
		};
	} // namespace View
} // namespace VTX
#endif
