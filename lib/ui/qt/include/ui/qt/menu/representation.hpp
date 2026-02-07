#ifndef __VTX_UI_QT_MENU_REPRESENTATION__
#define __VTX_UI_QT_MENU_REPRESENTATION__

#include <QMenu>
#include <app/preset/name.hpp>
#include <optional>
#include <renderer/representation.hpp>

namespace VTX::UI::QT::Menu
{

	class Representation : public Widget::BaseWidget<Representation, QMenu>
	{
	  public:
		Representation( QWidget * p_parent, const std::optional<App::ECS::Entity> p_representation = std::nullopt ) :
			BaseWidget( p_parent )
		{
			setTitle( "Representation" );

			auto & reg	= App::REG();
			auto   view = reg.view<Renderer::Representation>();

			for ( const auto ent : view )
			{
				const auto & representation = view.get<Renderer::Representation>( ent );
				const auto & name			= App::REG().get<App::Preset::Name>( ent );

				QAction * a = QMenu::addAction( QString::fromStdString( name.name ) );
				a->setCheckable( true );
				a->setChecked( p_representation && *p_representation == ent );
				a->setData( QVariant::fromValue( ent ) );
			}
		}
	};

} // namespace VTX::UI::QT::Menu

#endif
