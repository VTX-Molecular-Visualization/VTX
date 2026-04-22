#ifndef __VTX_UI_QT_MENU_COLOR_SCHEME__
#define __VTX_UI_QT_MENU_COLOR_SCHEME__

#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>
#include <QPainter>
#include <QWidgetAction>
#include <app/scene/color_layout.hpp>
#include <app/system/color.hpp>
#include <optional>
#include <renderer/color.hpp>

namespace VTX::UI::QT::Menu
{

	class ColorScheme : public Widget::BaseWidget<ColorScheme, QMenu>
	{
		Q_OBJECT

	  public:
		struct Selected
		{
			App::System::E_COLOR_SCHEME			scheme;
			std::optional<Renderer::ColorIndex> index = std::nullopt;
		};

		ColorScheme( QWidget * p_parent, const std::optional<App::System::E_COLOR_SCHEME> p_scheme = std::nullopt ) :
			BaseWidget( p_parent )
		{
			using namespace App;
			using namespace App::System;

			auto & colorLayoutIntance = ECS::getFirstComponent<Scene::ColorLayout>();
			auto & colorlayout		  = REG().get<Renderer::Color::Layout>( colorLayoutIntance.preset );

			setTitle( "Color scheme" );
			setIcon( STYLE().iconFromCodepoint( Style::Icons::COLOR_LAYOUT ) );

			auto addItem = [ this ](

							   const QString &									p_label,
							   const Selected &									p_data,
							   const std::optional<App::System::E_COLOR_SCHEME> p_currentScheme
						   )
			{
				QAction * a = QMenu::addAction( p_label );
				if ( p_currentScheme )
				{
					a->setCheckable( true );
					a->setChecked( *p_currentScheme == p_data.scheme );
				}
				connect( a, &QAction::triggered, this, [ this, p_data ]() { emit selected( p_data ); } );
			};

			addItem( "Atoms", { E_COLOR_SCHEME::ATOM }, p_scheme );
			addItem( "Residues", { E_COLOR_SCHEME::RESIDUE }, p_scheme );
			addItem( "Chains", { E_COLOR_SCHEME::CHAIN }, p_scheme );

			auto * subMenu = QMenu::addMenu( "Custom" );
			for ( Renderer::ColorIndex i = 0; i < Renderer::Color::LAYOUT_COUNT_CUSTOM; ++i )
			{
				auto * wa	= new QWidgetAction( subMenu );
				auto * item = new ColorItem( QT::Helper::toQColor( colorlayout.getCustomColor( i ) ) );
				item->setMinimumSize( 120, 24 );
				const Selected selected
					= { E_COLOR_SCHEME::CUSTOM,
						static_cast<Renderer::ColorIndex>( Renderer::Color::LAYOUT_OFFSET_CUSTOM + i ) };
				wa->setDefaultWidget( item );
				connect( wa, &QAction::triggered, this, [ this, selected ]() { emit this->selected( selected ); } );
				subMenu->addAction( wa );
			}
		}

	  signals:
		void selected( const Selected & );

	  private:
		/**
		 * @brief Private widget to display a color in the menu.
		 */
		class ColorItem : public QWidget
		{
		  public:
			ColorItem( const QColor & c, QWidget * parent = nullptr ) : QWidget( parent ), _color( c )
			{ setMinimumHeight( 24 ); }

		  protected:
			void paintEvent( QPaintEvent * ) override
			{
				QPainter p( this );
				p.fillRect( rect(), _color );
			}

		  private:
			QColor _color;
		};
	};

} // namespace VTX::UI::QT::Menu

#endif
