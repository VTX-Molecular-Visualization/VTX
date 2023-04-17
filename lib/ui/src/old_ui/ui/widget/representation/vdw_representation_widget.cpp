#include "ui/old_ui/ui/widget/representation/vdw_representation_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/action/representable.hpp>
#include <app/old_app/setting.hpp>

namespace VTX::UI::Widget::Representation
{

	void VdwRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addSphereWidgetInLayout();
		_sphereWidget->setRadiusWidgetVisible( false );
		_addColorModeWidgetInLayout();
	}

	void VdwRepresentationWidget::localize() {}

} // namespace VTX::UI::Widget::Representation