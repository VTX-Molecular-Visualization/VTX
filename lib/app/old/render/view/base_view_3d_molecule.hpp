#ifndef __VTX_RENDER_VIEW_BASE_VIEW_3D_MOLECULE__
#define __VTX_RENDER_VIEW_BASE_VIEW_3D_MOLECULE__

#include "app/old/application/representation/representation_preset.hpp"
#include "app/old/application/representation/representation_target.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/core/view/base_view_3d.hpp"
#include <map>

namespace VTX::App::Old::Render::View
{
	class BaseView3DMolecule : public App::Old::Core::View::BaseView3D<App::Old::Component::Chemistry::Molecule>
	{
		VTX_VIEW

	  protected:
		explicit BaseView3DMolecule( App::Old::Component::Chemistry::Molecule * const p_model ) : BaseView3D( p_model ) {}
		virtual ~BaseView3DMolecule() {}
	};
} // namespace VTX::App::Old::Render::View
#endif
