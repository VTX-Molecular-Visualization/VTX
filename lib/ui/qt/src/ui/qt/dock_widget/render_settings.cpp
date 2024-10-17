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
		_createGroupBoxOutline( component );
		_createGroupBoxFog( component );
		_createGroupBoxSelection( component );
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
		sliderSpecularFactor->setMinimum( Renderer::SPECULAR_FACTOR_MIN * 100 );
		sliderSpecularFactor->setMaximum( Renderer::SPECULAR_FACTOR_MAX * 100 );
		sliderSpecularFactor->setValue( p_component->getSettings().specularFactor * 100 );
		connect(
			sliderSpecularFactor,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::SPECULAR_FACTOR, float>>(
						static_cast<float>( p_value ) / 100.f
					);
			}
		);

		// Shininess.
		auto * sliderShininess = new QSlider( Qt::Horizontal, groupBoxShading );
		layout->addWidget( sliderShininess );
		sliderShininess->setMinimum( Renderer::SHININESS_MIN * 100 );
		sliderShininess->setMaximum( Renderer::SHININESS_MAX * 100 );
		sliderShininess->setValue( p_component->getSettings().shininess * 100 );
		connect(
			sliderShininess,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::SHININESS, float>>(
						static_cast<float>( p_value ) / 100.f
					);
			}
		);

		// Toon steps.
		auto * sliderToonSteps = new QSlider( Qt::Horizontal, groupBoxShading );
		layout->addWidget( sliderToonSteps );
		sliderToonSteps->setMinimum( Renderer::TOON_STEPS_MIN );
		sliderToonSteps->setMaximum( Renderer::TOON_STEPS_MAX );
		sliderToonSteps->setValue( p_component->getSettings().toonSteps );
		connect(
			sliderToonSteps,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::TOON_STEPS, uint>>( p_value );
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
			[ sliderSpecularFactor ]( const float p_value ) { sliderSpecularFactor->setValue( p_value * 100 ); };
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::SHININESS, float>() +=
			[ sliderShininess ]( const float p_value ) { sliderShininess->setValue( p_value * 100 ); };
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::TOON_STEPS, uint>() +=
			[ sliderToonSteps ]( const uint p_value ) { sliderToonSteps->setValue( p_value ); };
	}

	void RenderSettings::_createGroupBoxSSAO( App::Component::Representation::RenderSettings * const p_component )
	{
		auto * groupBoxSSAO		  = new QGroupBox( QString::fromStdString( "Shadows" ) );
		auto * layout			  = new QVBoxLayout( groupBoxSSAO );
		auto * checkboxSSAOActive = new QCheckBox( "Active", groupBoxSSAO );
		layout->addWidget( checkboxSSAOActive );
		_layout->addWidget( groupBoxSSAO );

		// Intensity.
		auto * sliderSSAOIntensity = new QSlider( Qt::Horizontal, groupBoxSSAO );
		layout->addWidget( sliderSSAOIntensity );
		sliderSSAOIntensity->setMinimum( Renderer::SSAO_INTENSITY_MIN * 100 );
		sliderSSAOIntensity->setMaximum( Renderer::SSAO_INTENSITY_MAX * 100 );
		sliderSSAOIntensity->setValue( p_component->getSettings().ssaoIntensity * 100 );
		connect(
			sliderSSAOIntensity,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::SSAO_INTENSITY, float>>(
						static_cast<float>( p_value ) / 100.f
					);
			}
		);

		// Blur.
		auto * sliderBlurSize = new QSlider( Qt::Horizontal, groupBoxSSAO );
		layout->addWidget( sliderBlurSize );

		// Callbacks.
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::SSAO_INTENSITY, float>() +=
			[ sliderSSAOIntensity ]( const float p_value ) { sliderSSAOIntensity->setValue( p_value * 100 ); };
	}

	void RenderSettings::_createGroupBoxOutline( App::Component::Representation::RenderSettings * const p_component )
	{
		auto * groupBoxOutline		 = new QGroupBox( QString::fromStdString( "Outline" ) );
		auto * layout				 = new QVBoxLayout( groupBoxOutline );
		auto * checkboxOutlineActive = new QCheckBox( "Active", groupBoxOutline );
		layout->addWidget( checkboxOutlineActive );
		_layout->addWidget( groupBoxOutline );

		// Color.
		Widget::ColorPicker * colorPickerOutline = new Widget::ColorPicker( groupBoxOutline );
		layout->addWidget( colorPickerOutline );
		colorPickerOutline->setColor( Helper::toQColor( p_component->getSettings().colorOutline ) );
		colorPickerOutline->onColorChanged += [ p_component ]( const QColor & p_color )
		{
			App::ACTION_SYSTEM()
				.execute<App::Action::RenderSettings::
							 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::COLOR_OUTLINE, Util::Color::Rgba>>(
					Helper::fromQColor( p_color )
				);
		};

		// Sensitivity.
		auto * sliderOutlineSensitivity = new QSlider( Qt::Horizontal, groupBoxOutline );
		layout->addWidget( sliderOutlineSensitivity );
		sliderOutlineSensitivity->setMinimum( Renderer::OUTLINE_SENSITIVITY_MIN * 100 );
		sliderOutlineSensitivity->setMaximum( Renderer::OUTLINE_SENSITIVITY_MAX * 100 );
		sliderOutlineSensitivity->setValue( p_component->getSettings().outlineSensitivity * 100 );
		connect(
			sliderOutlineSensitivity,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::OUTLINE_SENSITIVITY, float>>(
						static_cast<float>( p_value ) / 100.f
					);
			}
		);

		// Thickness.
		auto * sliderOutlineThickness = new QSlider( Qt::Horizontal, groupBoxOutline );
		layout->addWidget( sliderOutlineThickness );
		sliderOutlineThickness->setMinimum( Renderer::OUTLINE_THICKNESS_MIN );
		sliderOutlineThickness->setMaximum( Renderer::OUTLINE_THICKNESS_MAX );
		sliderOutlineThickness->setValue( p_component->getSettings().outlineThickness );
		connect(
			sliderOutlineThickness,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::OUTLINE_THICKNESS, uint>>( p_value );
			}
		);

		// Callbacks.
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::COLOR_OUTLINE, Util::Color::Rgba>() +=
			[ colorPickerOutline ]( const Util::Color::Rgba & p_color )
		{ colorPickerOutline->setColor( Helper::toQColor( p_color ) ); };
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::OUTLINE_SENSITIVITY, float>() +=
			[ sliderOutlineSensitivity ]( const float p_value )
		{ sliderOutlineSensitivity->setValue( p_value * 100 ); };
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::OUTLINE_THICKNESS, uint>() +=
			[ sliderOutlineThickness ]( const uint p_value ) { sliderOutlineThickness->setValue( p_value ); };
	}

	void RenderSettings::_createGroupBoxFog( App::Component::Representation::RenderSettings * const p_component )
	{
		auto * groupBoxFog		 = new QGroupBox( QString::fromStdString( "Fog" ) );
		auto * layout			 = new QVBoxLayout( groupBoxFog );
		auto * checkboxFogActive = new QCheckBox( "Active", groupBoxFog );
		layout->addWidget( checkboxFogActive );
		_layout->addWidget( groupBoxFog );

		// Color.
		Widget::ColorPicker * colorPickerFog = new Widget::ColorPicker( groupBoxFog );
		layout->addWidget( colorPickerFog );
		colorPickerFog->setColor( Helper::toQColor( p_component->getSettings().colorFog ) );
		colorPickerFog->onColorChanged += [ p_component ]( const QColor & p_color )
		{
			App::ACTION_SYSTEM()
				.execute<App::Action::RenderSettings::
							 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::COLOR_FOG, Util::Color::Rgba>>(
					Helper::fromQColor( p_color )
				);
		};

		// Near.
		auto * sliderFogNear = new QSlider( Qt::Horizontal, groupBoxFog );
		layout->addWidget( sliderFogNear );
		sliderFogNear->setMinimum( Renderer::FOG_NEAR_MIN * 100 );
		sliderFogNear->setMaximum( Renderer::FOG_NEAR_MAX * 100 );
		sliderFogNear->setValue( p_component->getSettings().fogNear * 100 );
		connect(
			sliderFogNear,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<
						App::Action::RenderSettings::ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::FOG_NEAR, float>>(
						static_cast<float>( p_value ) / 100.f
					);
			}
		);

		// Far.
		auto * sliderFogFar = new QSlider( Qt::Horizontal, groupBoxFog );
		layout->addWidget( sliderFogFar );
		sliderFogFar->setMinimum( Renderer::FOG_FAR_MIN * 100 );
		sliderFogFar->setMaximum( Renderer::FOG_FAR_MAX * 100 );
		sliderFogFar->setValue( p_component->getSettings().fogFar * 100 );
		connect(
			sliderFogFar,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<
						App::Action::RenderSettings::ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::FOG_FAR, float>>(
						static_cast<float>( p_value ) / 100.f
					);
			}
		);

		// Density.
		auto * sliderFogDensity = new QSlider( Qt::Horizontal, groupBoxFog );
		layout->addWidget( sliderFogDensity );
		sliderFogDensity->setMinimum( Renderer::FOG_DENSITY_MIN * 100 );
		sliderFogDensity->setMaximum( Renderer::FOG_DENSITY_MAX * 100 );
		sliderFogDensity->setValue( p_component->getSettings().fogDensity * 100 );
		connect(
			sliderFogDensity,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::FOG_DENSITY, float>>(
						static_cast<float>( p_value ) / 100.f
					);
			}
		);

		// Callbacks.
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::COLOR_FOG, Util::Color::Rgba>() +=
			[ colorPickerFog ]( const Util::Color::Rgba & p_color )
		{ colorPickerFog->setColor( Helper::toQColor( p_color ) ); };
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::FOG_NEAR, float>() +=
			[ sliderFogNear ]( const float p_value ) { sliderFogNear->setValue( p_value * 100 ); };
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::FOG_FAR, float>() +=
			[ sliderFogFar ]( const float p_value ) { sliderFogFar->setValue( p_value * 100 ); };
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::FOG_DENSITY, float>() +=
			[ sliderFogDensity ]( const float p_value ) { sliderFogDensity->setValue( p_value * 100 ); };
	}

	void RenderSettings::_createGroupBoxSelection( App::Component::Representation::RenderSettings * const p_component )
	{
		auto * groupBoxSelection = new QGroupBox( QString::fromStdString( "Selection" ) );
		auto * layout			 = new QVBoxLayout( groupBoxSelection );
		auto * checkboxActive	 = new QCheckBox( "Active", groupBoxSelection );
		layout->addWidget( checkboxActive );
		_layout->addWidget( groupBoxSelection );

		// Color.
		Widget::ColorPicker * colorPickerSelection = new Widget::ColorPicker( groupBoxSelection );
		layout->addWidget( colorPickerSelection );
		colorPickerSelection->setColor( Helper::toQColor( p_component->getSettings().colorSelection ) );
		colorPickerSelection->onColorChanged += [ p_component ]( const QColor & p_color )
		{
			App::ACTION_SYSTEM()
				.execute<App::Action::RenderSettings::
							 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::COLOR_SELECTION, Util::Color::Rgba>>(
					Helper::fromQColor( p_color )
				);
		};

		// Callbacks.
		p_component->getCallback<Renderer::E_RENDER_SETTINGS::COLOR_SELECTION, Util::Color::Rgba>() +=
			[ colorPickerSelection ]( const Util::Color::Rgba & p_color )
		{ colorPickerSelection->setColor( Helper::toQColor( p_color ) ); };
	}

} // namespace VTX::UI::QT::DockWidget
