#include "setting_representation_widget.hpp"
#include "model/representation/representation_library.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"
#include <QVBoxLayout>

namespace VTX::UI::Widget::Settings
{
	SettingRepresentationWidget::SettingRepresentationWidget( QWidget * const p_parent ) : BaseManualWidget( p_parent )
	{
	}

	void SettingRepresentationWidget::receiveEvent( const Event::VTXEvent & p_event ) {}

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
