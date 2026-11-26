#ifndef VTX_UI_QT_MENU_REPRESENTATION_HPP
#define VTX_UI_QT_MENU_REPRESENTATION_HPP

#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>
#include <app/generic/name.hpp>
#include <optional>
#include <renderer/representation.hpp>

namespace VTX::UI::QT::Menu
{

	class Representation : public Widget::BaseWidget<Representation, QMenu>
	{
		Q_OBJECT

	  public:
		Representation( QWidget * const p_parent, const std::optional<Entity> p_representation = std::nullopt ) :
			BaseWidget( p_parent )
		{
			setTitle( "Representation" );
			setIcon( STYLE().iconFromCodepoint( Style::Icons::REPRESENTATION ) );

			auto & reg	= App::REG();
			auto   view = reg.view<Renderer::Representation>();

			for ( const auto ent : view )
			{
				const auto & name = App::REG().get<App::Generic::Name>( ent );

				QAction * a = QMenu::addAction( QString::fromStdString( name.name ) );
				if ( p_representation )
				{
					a->setCheckable( true );
					a->setChecked( *p_representation == ent );
				}
				connect( a, &QAction::triggered, this, [ this, ent ]() { emit selected( ent ); } );
			}
		}

	  signals:
		void selected( Entity );
	};

} // namespace VTX::UI::QT::Menu

#endif
