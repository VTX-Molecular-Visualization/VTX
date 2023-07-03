#ifndef __VTX_UI_QT_ACTION_LABEL__
#define __VTX_UI_QT_ACTION_LABEL__

#include <app/old/core/action/base_action.hpp>
#include <app/old/component/object3d/label.hpp>
#include <unordered_set>
#include <vector>

namespace VTX::UI::QT::Action::Label
{
	class Orient : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Orient( App::Old::Component::Object3D::Label * const p_label ) : _labels( { p_label } ) {}
		explicit Orient( const std::unordered_set<App::Old::Component::Object3D::Label *> & p_labels ) : _labels( p_labels ) {}

		virtual void execute() override;

	  private:
		std::unordered_set<App::Old::Component::Object3D::Label *> _labels;
	};
} // namespace VTX::UI::QT::Action::Label
#endif
