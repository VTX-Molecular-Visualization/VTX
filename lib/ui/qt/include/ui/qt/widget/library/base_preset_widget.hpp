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

			connect( _presetSelector, &PresetSelector<P>::presetChanged, this, &BasePresetWidget::_onPresetChanged );
		}

		inline App::ECS::Entity getCurrentPreset() const { return _presetSelector->getCurrentPreset(); }
		inline void addWidget( QWidget * const p_widget ) { _groupboxPreset->layout()->addWidget( p_widget ); }
		inline void setTitle( const QString & p_title ) { _groupboxPreset->setTitle( p_title ); }

	  protected:
		QPointer<QVBoxLayout>						 _layout;
		QPointer<Widget::Library::PresetSelector<P>> _presetSelector;
		QPointer<QGroupBox>							 _groupboxPreset;

		// TODO: const!
		P * _preset = nullptr;

	  private:
		void _onPresetChanged( App::ECS::Entity p_ent )
		{
			VTX_INFO( "_onPresetChanged: {}", std::to_string( int( p_ent ) ) );
			/*
			std::string name = p_name.toStdString();
			_preset			 = &App::LIBRARY().getLibrary<P>().getPreset( name );
			_onPresetChanged();
			*/
		}
	};

} // namespace VTX::UI::QT::Widget::Library
#endif
