#include "ui/qt/dock_widget/render_settings.hpp"
#include "ui/qt/helper.hpp"
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

		// Shading combo box.
		auto * groupBoxShading = new QGroupBox( QString::fromStdString( "Shading" ) );
		auto * layout		   = new QVBoxLayout( groupBoxShading );
		auto * comboBoxShading = new QComboBox( groupBoxShading );
		layout->addWidget( comboBoxShading );
		_layout->addWidget( groupBoxShading );

		// TODO: not hardcoded.
		comboBoxShading->addItem( "Diffuse" );
		comboBoxShading->addItem( "Glossy " );
		comboBoxShading->addItem( "Toon" );
		comboBoxShading->addItem( "Flat" );

		// Background color.

		// Connect the combo box to the component.
		connect(
			comboBoxShading,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ component ]( const int p_index )
			{
				App::ACTION_SYSTEM()
					.execute<App::Action::RenderSettings::
								 ChangeRenderSetting<Renderer::Proxy::E_RENDER_SETTINGS::SHADING_MODE, uint>>( p_index
					);
			}
		);

		// Spacer.
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

		// Callbacks.
		component->getCallback<Renderer::Proxy::E_RENDER_SETTINGS::SHADING_MODE, uint>() +=
			[ comboBoxShading ]( const uint p_value )
		{
			// TODO: check with this is triggering another QComboBox::currentIndexChanged.
			comboBoxShading->setCurrentIndex( p_value );
		};
	}

} // namespace VTX::UI::QT::DockWidget
