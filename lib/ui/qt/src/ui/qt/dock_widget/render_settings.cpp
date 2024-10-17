#include "ui/qt/dock_widget/render_settings.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/widget/color_picker.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <app/action/render_settings.hpp>
#include <app/application/scene.hpp>
#include <app/component/representation/render_settings.hpp>
#include <util/color/rgba.hpp>

namespace VTX::UI::QT::DockWidget
{

	RenderSettings::RenderSettings( QWidget * p_parent ) : BaseDockWidget<RenderSettings>( "Render settings", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		const auto & scene	   = App::SCENE();
		auto *		 component = &App::ECS_REGISTRY().getComponent<App::Component::Representation::RenderSettings>(
			  App::ECS_REGISTRY().getEntity( scene )
		  );

		_createGroupBoxShading( component );
		_createGroupBoxSSAO( component );
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	}

	void RenderSettings::_createGroupBoxShading( App::Component::Representation::RenderSettings * const p_component )
	{ // Shading group.
		auto * groupBoxShading = new QGroupBox( QString::fromStdString( "Lighting" ) );
		auto * layout		   = new QVBoxLayout( groupBoxShading );

		// Shading mode.
		auto * comboBoxShading = new QComboBox( groupBoxShading );
		layout->addWidget( comboBoxShading );
		_layout->addWidget( groupBoxShading );
		// TODO: not hardcoded.
		comboBoxShading->addItem( "Diffuse" );
		comboBoxShading->addItem( "Glossy " );
		comboBoxShading->addItem( "Toon" );
		comboBoxShading->addItem( "Flat" );
		comboBoxShading->setCurrentIndex( p_component->getSettings().shadingMode );
		connect(
			comboBoxShading,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ p_component ]( const int p_index )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::SHADING_MODE, uint>>( p_index );
			}
		);

		// Background color.
		Widget::ColorPicker * colorPickerBackground = new Widget::ColorPicker( groupBoxShading );
		layout->addWidget( colorPickerBackground );
		colorPickerBackground->setColor( Helper::toQColor( p_component->getSettings().colorBackground ) );
		colorPickerBackground->setText( "Background color" );
		colorPickerBackground->onColorChanged += [ p_component ]( const QColor & p_color )
		{
			App::ACTION_SYSTEM()
				.execute<App::Action::RenderSettings::
							 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::COLOR_BACKGROUND, Util::Color::Rgba>>(
					Helper::fromQColor( p_color )
				);
		};

		// Light color.
		Widget::ColorPicker * colorPickerLight = new Widget::ColorPicker( groupBoxShading );
		layout->addWidget( colorPickerLight );
		colorPickerLight->setColor( Helper::toQColor( p_component->getSettings().colorLight ) );
		colorPickerLight->setText( "Light color" );
		colorPickerLight->onColorChanged += [ p_component ]( const QColor & p_color )
		{
			App::ACTION_SYSTEM()
				.execute<App::Action::RenderSettings::
							 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::COLOR_LIGHT, Util::Color::Rgba>>(
					Helper::fromQColor( p_color )
				);
		};

		// Specular factor.
		auto * sliderSpecularFactor = new QSlider( Qt::Horizontal, groupBoxShading );
		layout->addWidget( sliderSpecularFactor );
		sliderSpecularFactor->setMinimum( 0 );
		sliderSpecularFactor->setMaximum( 100 );
		sliderSpecularFactor->setValue( p_component->getSettings().specularFactor );
		connect(
			sliderSpecularFactor,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::SPECULAR_FACTOR, uint>>( p_value );
			}
		);

		// Callbacks.
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::SHADING_MODE, uint>() +=
			[ comboBoxShading ]( const uint p_value )
		{
			// TODO: check with this is triggering another QComboBox::currentIndexChanged.
			comboBoxShading->setCurrentIndex( p_value );
		};
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::COLOR_BACKGROUND, Util::Color::Rgba>() +=
			[ colorPickerBackground ]( const Util::Color::Rgba & p_color )
		{ colorPickerBackground->setColor( Helper::toQColor( p_color ) ); };
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::COLOR_LIGHT, Util::Color::Rgba>() +=
			[ colorPickerLight ]( const Util::Color::Rgba & p_color )
		{ colorPickerLight->setColor( Helper::toQColor( p_color ) ); };
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::SPECULAR_FACTOR, float>() +=
			[ sliderSpecularFactor ]( const uint p_value ) { sliderSpecularFactor->setValue( p_value ); };
	}

	void RenderSettings::_createGroupBoxSSAO( App::Component::Representation::RenderSettings * const p_component )
	{
		auto * groupBoxSSAO		  = new QGroupBox( QString::fromStdString( "SSAO" ) );
		auto * layout			  = new QVBoxLayout( groupBoxSSAO );
		auto * checkboxSSAOActive = new QCheckBox( "Active", groupBoxSSAO );
		layout->addWidget( checkboxSSAOActive );
		_layout->addWidget( groupBoxSSAO );

		// auto * sliderSSAOIntensity = new QSlider( Qt::Horizontal, groupBoxSSAO );
	}

} // namespace VTX::UI::QT::DockWidget
