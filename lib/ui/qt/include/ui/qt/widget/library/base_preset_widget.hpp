#ifndef __VTX_UI_QT_WIDGET_LIBRARY_BASE_PRESET_WIDGET__
#define __VTX_UI_QT_WIDGET_LIBRARY_BASE_PRESET_WIDGET__

#include "preset_selector.hpp"
#include <QGroupBox>
#include <QVBoxLayout>
#include <app/library/preset/representation.hpp>

namespace VTX::UI::QT::Widget::Library
{

	template<App::Library::ConceptPreset P>
	class BasePresetWidget : public QWidget
	{
	  public:
		BasePresetWidget( QWidget * p_parent ) : QWidget( p_parent ), _library( App::LIBRARY().getLibrary<P>() )
		{
			_presetSelector = new PresetSelector<P>( this );
			_groupboxPreset = new QGroupBox( this );
			_groupboxPreset->setLayout( new QVBoxLayout() );

			_layout = new QVBoxLayout( this );
			setLayout( _layout );
			_layout->setContentsMargins( 0, 0, 0, 0 );

			_layout->addWidget( _presetSelector );
			_layout->addWidget( _groupboxPreset );
		}

		virtual void init()
		{
			connect( _presetSelector, &PresetSelector<P>::presetChanged, this, &BasePresetWidget::_presetChanged );

			_library->onPresetAdded += [ this ]( const std::string_view p_name ) { _onPresetAdded( p_name ); };
			for ( const auto & [ name, _ ] : _library->getPresets() )
			{
				_onPresetAdded( name );
			}

			_preset = _library->getPreset( _presetSelector->getCurrentPreset().toStdString() );
			_onPresetChanged();
		}

		inline std::string getCurrentPreset() const { return _presetSelector->getCurrentPreset().toStdString(); }

		inline void addWidget( QWidget * const p_widget ) { _groupboxPreset->layout()->addWidget( p_widget ); }
		inline void setTitle( const QString & p_title ) { _groupboxPreset->setTitle( p_title ); }

	  protected:
		QPointer<QVBoxLayout>						 _layout;
		QPointer<Widget::Library::PresetSelector<P>> _presetSelector;
		QPointer<QGroupBox>							 _groupboxPreset;

		App::Library::BaseLibrary<P> * const _library;
		// TODO: const!
		P * _preset;

		/**
		 * @brief Called when a new preset is added to the library.
		 * @param name of the added preset.
		 */
		virtual void _onPresetAdded( const std::string_view ) {}

		/**
		 * @brief Called when the preset has changed.
		 */
		virtual void _onPresetChanged() {}

	  private:
		void _presetChanged( const QString & p_name )
		{
			std::string name = p_name.toStdString();
			_preset			 = App::LIBRARY().getLibrary<P>()->getPreset( name );
			_onPresetChanged();
		}
	};

} // namespace VTX::UI::QT::Widget::Library
#endif
