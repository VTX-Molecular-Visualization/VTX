#ifndef __VTX_RENDER_VIEW_BASE_VIEW_3D_MOLECULE__
#define __VTX_RENDER_VIEW_BASE_VIEW_3D_MOLECULE__

#include "app/application/representation/representation_preset.hpp"
#include "app/application/representation/representation_target.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/core/view/base_view_3d.hpp"
#include <map>

namespace VTX::App::Render::View
{
	class BaseView3DMolecule : public App::Core::View::BaseView3D<App::Component::Chemistry::Molecule>
	{
		VTX_VIEW

	  protected:
		explicit BaseView3DMolecule( App::Component::Chemistry::Molecule * const p_model ) : BaseView3D( p_model ) {}
		virtual ~BaseView3DMolecule() {}
	};
} // namespace VTX::App::Render::View
#endif
