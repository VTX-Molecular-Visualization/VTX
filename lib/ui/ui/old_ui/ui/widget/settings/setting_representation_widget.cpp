#include "setting_representation_widget.hpp"
#include "old_ui/style.hpp"
#include "old_ui/ui/widget_factory.hpp"
#include "old_ui/vtx_app.hpp"
#include <QVBoxLayout>
#include <app/old_app/model/representation/representation_library.hpp>

namespace VTX::UI::Widget::Settings
{
	SettingRepresentationWidget::SettingRepresentationWidget( QWidget * const p_parent ) : BaseManualWidget( p_parent )
	{
	}

	void SettingRepresentationWidget::receiveEvent( const VTX::Event::VTXEvent & p_event ) {}

	void SettingRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_presetLibrary = WidgetFactory::get().instantiateViewWidget<RepresentationLibraryView>(
			&Model::Representation::RepresentationLibrary::get(),
			ID::View::UI_REPRESENTATION_PRESET_LIBRARY,
			this,
			"PresetLibrary" );

		QVBoxLayout * const layout = new QVBoxLayout( this );
		layout->addWidget( _presetLibrary );
	}

	void SettingRepresentationWidget::_setupSlots() {};
	void SettingRepresentationWidget::localize() {};

} // namespace VTX::UI::Widget::Settings
