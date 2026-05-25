#ifndef __VTX_UI_QT_WIDGET_LIBRARY_BASE_PRESET_WIDGET__
#define __VTX_UI_QT_WIDGET_LIBRARY_BASE_PRESET_WIDGET__

#include "preset_selector.hpp"
#include <QGroupBox>
#include <QVBoxLayout>

namespace VTX::UI::QT::Widget::Library
{

	template<typename P>
	class BasePresetWidget : public QWidget
	{
	  public:
		BasePresetWidget( QWidget * p_parent ) : QWidget( p_parent )
		{
			_presetSelector = new PresetSelector<P>( this );
			_groupboxPreset = new QGroupBox( this );
			_groupboxPreset->setLayout( new QVBoxLayout() );

			_layout = new QVBoxLayout( this );
			setLayout( _layout );
			_layout->setContentsMargins( 0, 0, 0, 0 );

			_layout->addWidget( _presetSelector );
			_layout->addWidget( _groupboxPreset );

			// Connect preset selector to update function.
			connect( _presetSelector, &PresetSelector<P>::currentPresetUpdated, this, &BasePresetWidget::_update );
			connect( _presetSelector, &PresetSelector<P>::presetChanged, this, &BasePresetWidget::_update );
		}

		inline App::Entity	 currentPreset() const { return _presetSelector->getCurrentPreset(); }
		inline QGroupBox * const presetGroupBox() const { return _groupboxPreset; }
		inline void addWidget( QWidget * const p_widget ) { _groupboxPreset->layout()->addWidget( p_widget ); }
		inline void setTitle( const QString & p_title ) { _groupboxPreset->setTitle( p_title ); }
		inline void refresh() { _presetSelector->refresh(); }
		inline void setCurrentPreset( const App::Entity p_preset ) { _presetSelector->setCurrentPreset( p_preset ); }

		/**
		 * @brief Update the widget when the preset is updated.
		 */
		virtual void _update( App::Entity ) = 0;

	  private:
		QPointer<QVBoxLayout>						 _layout;
		QPointer<Widget::Library::PresetSelector<P>> _presetSelector;
		QPointer<QGroupBox>							 _groupboxPreset;
	};

} // namespace VTX::UI::QT::Widget::Library
#endif
