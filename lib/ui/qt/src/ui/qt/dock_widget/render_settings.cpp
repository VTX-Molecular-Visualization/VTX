#include "ui/qt/dock_widget/render_settings.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/widget/color_picker.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
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

		for ( int i = 0; i < int( Renderer::E_SHADING::COUNT ); ++i )
		{
			comboBoxShading->addItem( Renderer::SHADING_STR[ i ].data() );
		}
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
		auto * labelSpecularFactor	= new QLabel( "Specular factor", groupBoxShading );
		auto * sliderSpecularFactor = new QSlider( Qt::Horizontal, groupBoxShading );
		layout->addWidget( labelSpecularFactor );
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
		auto * labelShininess  = new QLabel( "Shininess", groupBoxShading );
		auto * sliderShininess = new QSlider( Qt::Horizontal, groupBoxShading );
		layout->addWidget( labelShininess );
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
		auto * labelToonSteps  = new QLabel( "Toon steps", groupBoxShading );
		auto * sliderToonSteps = new QSlider( Qt::Horizontal, groupBoxShading );
		layout->addWidget( labelToonSteps );
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
		p_component->callback<Renderer::E_RENDER_SETTINGS::SHADING_MODE, uint>() +=
			[ comboBoxShading,
			  labelSpecularFactor,
			  sliderSpecularFactor,
			  labelShininess,
			  sliderShininess,
			  labelToonSteps,
			  sliderToonSteps ]( const uint p_value )
		{
			comboBoxShading->setCurrentIndex( p_value );

			// TODO: better way, root widget?
			switch ( Renderer::E_SHADING( p_value ) )
			{
			case Renderer::E_SHADING::DIFFUSE:
				labelSpecularFactor->setVisible( false );
				sliderSpecularFactor->setVisible( false );
				labelShininess->setVisible( false );
				sliderShininess->setVisible( false );
				labelToonSteps->setVisible( false );
				sliderToonSteps->setVisible( false );
				break;
			case Renderer::E_SHADING::GLOSSY:
				labelSpecularFactor->setVisible( true );
				sliderSpecularFactor->setVisible( true );
				labelShininess->setVisible( true );
				sliderShininess->setVisible( true );
				labelToonSteps->setVisible( false );
				sliderToonSteps->setVisible( false );
				break;
			case Renderer::E_SHADING::TOON:
				labelSpecularFactor->setVisible( false );
				sliderSpecularFactor->setVisible( false );
				labelShininess->setVisible( false );
				sliderShininess->setVisible( false );
				labelToonSteps->setVisible( true );
				sliderToonSteps->setVisible( true );
				break;
			case Renderer::E_SHADING::FLAT:
				labelSpecularFactor->setVisible( false );
				sliderSpecularFactor->setVisible( false );
				labelShininess->setVisible( false );
				sliderShininess->setVisible( false );
				labelToonSteps->setVisible( false );
				sliderToonSteps->setVisible( false );
				break;
			default: break;
			}
		};
		p_component->callback<Renderer::E_RENDER_SETTINGS::COLOR_BACKGROUND, Util::Color::Rgba>() +=
			[ colorPickerBackground ]( const Util::Color::Rgba & p_color )
		{ colorPickerBackground->setColor( Helper::toQColor( p_color ) ); };
		p_component->callback<Renderer::E_RENDER_SETTINGS::COLOR_LIGHT, Util::Color::Rgba>() +=
			[ colorPickerLight ]( const Util::Color::Rgba & p_color )
		{ colorPickerLight->setColor( Helper::toQColor( p_color ) ); };
		p_component->callback<Renderer::E_RENDER_SETTINGS::SPECULAR_FACTOR, float>() +=
			[ sliderSpecularFactor ]( const float p_value ) { sliderSpecularFactor->setValue( p_value * 100 ); };
		p_component->callback<Renderer::E_RENDER_SETTINGS::SHININESS, float>() +=
			[ sliderShininess ]( const float p_value ) { sliderShininess->setValue( p_value * 100 ); };
		p_component->callback<Renderer::E_RENDER_SETTINGS::TOON_STEPS, uint>() +=
			[ sliderToonSteps ]( const uint p_value ) { sliderToonSteps->setValue( p_value ); };

		// Emit init.
		emit comboBoxShading->currentIndexChanged( comboBoxShading->currentIndex() );
	}

	void RenderSettings::_createGroupBoxSSAO( App::Component::Representation::RenderSettings * const p_component )
	{
		auto * groupBoxSSAO = new QGroupBox( QString::fromStdString( "Shadows" ) );
		auto * layout		= new QVBoxLayout( groupBoxSSAO );
		_layout->addWidget( groupBoxSSAO );

		// Active.
		auto * checkboxSSAOActive = new QCheckBox( "Active", groupBoxSSAO );
		layout->addWidget( checkboxSSAOActive );
		checkboxSSAOActive->setChecked( p_component->getSettings().activeSSAO );

		connect(
			checkboxSSAOActive,
			&QCheckBox::stateChanged,
			[ p_component ]( const int p_state )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::ACTIVE_SSAO, bool>>(
						p_state == Qt::Checked
					);
			}
		);

		// Intensity.
		auto * labelSSAOIntensity  = new QLabel( "Intensity", groupBoxSSAO );
		auto * sliderSSAOIntensity = new QSlider( Qt::Horizontal, groupBoxSSAO );
		layout->addWidget( labelSSAOIntensity );
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
		auto * labelBlurSize  = new QLabel( "Blur size", groupBoxSSAO );
		auto * sliderBlurSize = new QSlider( Qt::Horizontal, groupBoxSSAO );
		layout->addWidget( labelBlurSize );
		layout->addWidget( sliderBlurSize );
		sliderBlurSize->setMinimum( Renderer::BLUR_SIZE_MIN * 100 );
		sliderBlurSize->setMaximum( Renderer::BLUR_SIZE_MAX * 100 );
		sliderBlurSize->setValue( p_component->getSettings().blurSize * 100 );
		connect(
			sliderBlurSize,
			&QSlider::valueChanged,
			[ p_component ]( const int p_value )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::BLUR_SIZE, float>>(
						static_cast<float>( p_value ) / 100.f
					);
			}
		);

		// Callbacks.
		p_component->callback<Renderer::E_RENDER_SETTINGS::ACTIVE_SSAO, bool>() +=
			[ checkboxSSAOActive, labelBlurSize, sliderBlurSize, labelSSAOIntensity, sliderSSAOIntensity ](
				const bool p_value
			)
		{
			checkboxSSAOActive->setChecked( p_value ? Qt::Checked : Qt::Unchecked );
			labelBlurSize->setVisible( p_value );
			sliderBlurSize->setVisible( p_value );
			labelSSAOIntensity->setVisible( p_value );
			sliderSSAOIntensity->setVisible( p_value );
		};
		p_component->callback<Renderer::E_RENDER_SETTINGS::SSAO_INTENSITY, float>() +=
			[ sliderSSAOIntensity ]( const float p_value ) { sliderSSAOIntensity->setValue( p_value * 100 ); };
		p_component->callback<Renderer::E_RENDER_SETTINGS::BLUR_SIZE, float>() +=
			[ sliderBlurSize ]( const float p_value ) { sliderBlurSize->setValue( p_value * 100 ); };

		// Emit init.
		emit checkboxSSAOActive->stateChanged( checkboxSSAOActive->checkState() );
	}

	void RenderSettings::_createGroupBoxOutline( App::Component::Representation::RenderSettings * const p_component )
	{
		auto * groupBoxOutline = new QGroupBox( QString::fromStdString( "Outline" ) );
		auto * layout		   = new QVBoxLayout( groupBoxOutline );
		_layout->addWidget( groupBoxOutline );

		// Active.
		auto * checkboxOutlineActive = new QCheckBox( "Active", groupBoxOutline );
		layout->addWidget( checkboxOutlineActive );
		checkboxOutlineActive->setChecked( p_component->getSettings().activeOutline );
		connect(
			checkboxOutlineActive,
			&QCheckBox::stateChanged,
			[ p_component ]( const int p_state )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::ACTIVE_OUTLINE, bool>>(
						p_state == Qt::Checked
					);
			}
		);

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
		auto * labelOutlineSensitivity	= new QLabel( "Sensitivity", groupBoxOutline );
		auto * sliderOutlineSensitivity = new QSlider( Qt::Horizontal, groupBoxOutline );
		layout->addWidget( labelOutlineSensitivity );
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
		auto * labelOutlineThickness  = new QLabel( "Thickness", groupBoxOutline );
		auto * sliderOutlineThickness = new QSlider( Qt::Horizontal, groupBoxOutline );
		layout->addWidget( labelOutlineThickness );
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
		p_component->callback<Renderer::E_RENDER_SETTINGS::ACTIVE_OUTLINE, bool>() +=
			[ checkboxOutlineActive,
			  labelOutlineSensitivity,
			  sliderOutlineSensitivity,
			  labelOutlineThickness,
			  sliderOutlineThickness,
			  colorPickerOutline ]( const bool p_value )
		{
			checkboxOutlineActive->setChecked( p_value ? Qt::Checked : Qt::Unchecked );
			labelOutlineSensitivity->setVisible( p_value );
			sliderOutlineSensitivity->setVisible( p_value );
			labelOutlineThickness->setVisible( p_value );
			sliderOutlineThickness->setVisible( p_value );
			colorPickerOutline->setVisible( p_value );
		};
		p_component->callback<Renderer::E_RENDER_SETTINGS::COLOR_OUTLINE, Util::Color::Rgba>() +=
			[ colorPickerOutline ]( const Util::Color::Rgba & p_color )
		{ colorPickerOutline->setColor( Helper::toQColor( p_color ) ); };
		p_component->callback<Renderer::E_RENDER_SETTINGS::OUTLINE_SENSITIVITY, float>() +=
			[ sliderOutlineSensitivity ]( const float p_value )
		{ sliderOutlineSensitivity->setValue( p_value * 100 ); };
		p_component->callback<Renderer::E_RENDER_SETTINGS::OUTLINE_THICKNESS, uint>() +=
			[ sliderOutlineThickness ]( const uint p_value ) { sliderOutlineThickness->setValue( p_value ); };

		// Emit init.
		emit checkboxOutlineActive->stateChanged( checkboxOutlineActive->checkState() );
	}

	void RenderSettings::_createGroupBoxFog( App::Component::Representation::RenderSettings * const p_component )
	{
		auto * groupBoxFog = new QGroupBox( QString::fromStdString( "Fog" ) );
		auto * layout	   = new QVBoxLayout( groupBoxFog );
		_layout->addWidget( groupBoxFog );

		// Active.
		auto * checkboxFogActive = new QCheckBox( "Active", groupBoxFog );
		layout->addWidget( checkboxFogActive );
		checkboxFogActive->setChecked( p_component->getSettings().activeFog );
		connect(
			checkboxFogActive,
			&QCheckBox::stateChanged,
			[ p_component ]( const int p_state )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::ACTIVE_FOG, bool>>(
						p_state == Qt::Checked
					);
			}
		);

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
		auto * labelFogNear	 = new QLabel( "Near", groupBoxFog );
		auto * sliderFogNear = new QSlider( Qt::Horizontal, groupBoxFog );
		layout->addWidget( labelFogNear );
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
		auto * labelFogFar	= new QLabel( "Far", groupBoxFog );
		auto * sliderFogFar = new QSlider( Qt::Horizontal, groupBoxFog );
		layout->addWidget( labelFogFar );
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
		auto * labelFogDensity	= new QLabel( "Density", groupBoxFog );
		auto * sliderFogDensity = new QSlider( Qt::Horizontal, groupBoxFog );
		layout->addWidget( labelFogDensity );
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
		p_component->callback<Renderer::E_RENDER_SETTINGS::ACTIVE_FOG, bool>() +=
			[ checkboxFogActive,
			  colorPickerFog,
			  labelFogNear,
			  sliderFogNear,
			  labelFogFar,
			  sliderFogFar,
			  labelFogDensity,
			  sliderFogDensity ]( const bool p_value )
		{
			checkboxFogActive->setChecked( p_value ? Qt::Checked : Qt::Unchecked );
			colorPickerFog->setVisible( p_value );
			labelFogNear->setVisible( p_value );
			sliderFogNear->setVisible( p_value );
			labelFogFar->setVisible( p_value );
			sliderFogFar->setVisible( p_value );
			labelFogDensity->setVisible( p_value );
			sliderFogDensity->setVisible( p_value );
		};
		p_component->callback<Renderer::E_RENDER_SETTINGS::COLOR_FOG, Util::Color::Rgba>() +=
			[ colorPickerFog ]( const Util::Color::Rgba & p_color )
		{ colorPickerFog->setColor( Helper::toQColor( p_color ) ); };
		p_component->callback<Renderer::E_RENDER_SETTINGS::FOG_NEAR, float>() +=
			[ sliderFogNear ]( const float p_value ) { sliderFogNear->setValue( p_value * 100 ); };
		p_component->callback<Renderer::E_RENDER_SETTINGS::FOG_FAR, float>() +=
			[ sliderFogFar ]( const float p_value ) { sliderFogFar->setValue( p_value * 100 ); };
		p_component->callback<Renderer::E_RENDER_SETTINGS::FOG_DENSITY, float>() +=
			[ sliderFogDensity ]( const float p_value ) { sliderFogDensity->setValue( p_value * 100 ); };

		// Emit init.
		emit checkboxFogActive->stateChanged( checkboxFogActive->checkState() );
	}

	void RenderSettings::_createGroupBoxSelection( App::Component::Representation::RenderSettings * const p_component )
	{
		auto * groupBoxSelection = new QGroupBox( QString::fromStdString( "Selection" ) );
		auto * layout			 = new QVBoxLayout( groupBoxSelection );
		_layout->addWidget( groupBoxSelection );

		// Active.
		auto * checkboxActive = new QCheckBox( "Active", groupBoxSelection );
		layout->addWidget( checkboxActive );
		checkboxActive->setChecked( p_component->getSettings().activeSelection );
		connect(
			checkboxActive,
			&QCheckBox::stateChanged,
			[ p_component ]( const int p_state )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::E_RENDER_SETTINGS::ACTIVE_SELECTION, bool>>(
						p_state == Qt::Checked
					);
			}
		);

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
		p_component->callback<Renderer::E_RENDER_SETTINGS::ACTIVE_SELECTION, bool>() +=
			[ checkboxActive, colorPickerSelection ]( const bool p_value )
		{
			checkboxActive->setChecked( p_value ? Qt::Checked : Qt::Unchecked );
			colorPickerSelection->setVisible( p_value );
		};
		p_component->callback<Renderer::E_RENDER_SETTINGS::COLOR_SELECTION, Util::Color::Rgba>() +=
			[ colorPickerSelection ]( const Util::Color::Rgba & p_color )
		{ colorPickerSelection->setColor( Helper::toQColor( p_color ) ); };

		// Emit init.
		emit checkboxActive->stateChanged( checkboxActive->checkState() );
	}

} // namespace VTX::UI::QT::DockWidget
