#ifndef __VTX_UI_QT_WIDGET_LIBRARY_BASE_PRESET_WIDGET__
#define __VTX_UI_QT_WIDGET_LIBRARY_BASE_PRESET_WIDGET__

#include "preset_selector.hpp"
#include <QGroupBox>
#include <QVBoxLayout>
#include <app/core/library/library.hpp>

namespace VTX::UI::QT::Widget::Library
{
	template<App::Core::Library::ConceptPreset P>
	class BasePresetWidget : public QWidget
	{
	  public:
		BasePresetWidget( QWidget * p_parent ) : QWidget( p_parent )
		{
			_presetSelector = new PresetSelector<App::Library::Preset::Representation>( this );
			_groupboxPreset = new QGroupBox( this );
			_layout			= new QVBoxLayout( this );
			setLayout( _layout );
			// _layout->setContentsMargins( 0, 0, 0, 0 );

			_layout->addWidget( _presetSelector );

			connect( _presetSelector, &PresetSelector<P>::presetChanged, this, &BasePresetWidget::_presetChanged );
		}

		inline std::string getCurrentPreset() const { return _presetSelector->getCurrentPreset(); }

	  protected:
		QPointer<QVBoxLayout>						 _layout;
		QPointer<Widget::Library::PresetSelector<P>> _presetSelector;
		QPointer<QGroupBox>							 _groupboxPreset;

		virtual void _onPresetChanged() = 0;

	  private:
		const P * _preset;

		void _presetChanged( const QString & p_name )
		{
			_preset = App::LIBRARY_SYSTEM().getLibrary<P>()->getPreset( p_name.toStdString() );
			_onPresetChanged();
		}
	};

} // namespace VTX::UI::QT::Widget::Library
#endif
