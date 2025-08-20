#ifndef __VTX_UI_QT_WIDGET_LIBRARY_BASE_PRESET_WIDGET__
#define __VTX_UI_QT_WIDGET_LIBRARY_BASE_PRESET_WIDGET__

#include "preset_selector.hpp"
#include <QGroupBox>
#include <QVBoxLayout>
#include <app/library/preset/representation.hpp>

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
			_groupboxPreset->setLayout( new QVBoxLayout() );

			_layout = new QVBoxLayout( this );
			setLayout( _layout );
			_layout->setContentsMargins( 0, 0, 0, 0 );

			_layout->addWidget( _presetSelector );
			_layout->addWidget( _groupboxPreset );

			_preset
				= App::LIBRARY_SYSTEM().getLibrary<P>()->getPreset( _presetSelector->getCurrentPreset().toStdString() );
			connect( _presetSelector, &PresetSelector<P>::presetChanged, this, &BasePresetWidget::_presetChanged );
		}

		inline std::string getCurrentPreset() const { return _presetSelector->getCurrentPreset().toStdString(); }

	  protected:
		QPointer<QVBoxLayout>						 _layout;
		QPointer<Widget::Library::PresetSelector<P>> _presetSelector;
		QPointer<QGroupBox>							 _groupboxPreset;

		// TODO: const!
		// P * const preset() { return _preset; }
		P * _preset;
		/**
		 * @brief Called when the preset has changed, signals blocked.
		 */
		virtual void _onPresetChanged() = 0;

	  private:
		void _presetChanged( const QString & p_name )
		{
			std::string name = p_name.toStdString();
			VTX_DEBUG( "Preset changed: {}", name );
			_preset = App::LIBRARY_SYSTEM().getLibrary<P>()->getPreset( name );
			_onPresetChanged();
		}
	};

} // namespace VTX::UI::QT::Widget::Library
#endif
