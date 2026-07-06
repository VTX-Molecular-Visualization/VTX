#ifndef __VTX_UI_QT_MENU_COLOR_SCHEME__
#define __VTX_UI_QT_MENU_COLOR_SCHEME__

#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
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
			Renderer::E_COLOR_SCHEME			scheme;
			std::optional<Renderer::ColorIndex> index = std::nullopt;
		};

		struct SecondaryStructureSelected
		{
			Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE scheme;
			std::optional<Renderer::ColorIndex>			 index = std::nullopt;
		};

		ColorScheme(
			QWidget *														  p_parent,
			const std::optional<Renderer::E_COLOR_SCHEME>					  p_scheme					 = std::nullopt,
			const std::optional<Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE> p_secondaryStructureScheme = std::nullopt
		) : BaseWidget( p_parent )
		{
			using namespace Renderer;

			auto & colorLayoutIntance = App::ECS::getFirstComponent<App::Scene::ColorLayout>();
			auto & colorlayout		  = App::REG().get<Color::Layout>( colorLayoutIntance.preset );

			setTitle( "Color scheme" );
			setIcon( STYLE().iconFromCodepoint( Style::Icons::COLOR_LAYOUT ) );

			QMenu::addSection( "Primary structure" );

			auto addItem = [ this ](

							   const QString &					   p_label,
							   const Selected &					   p_data,
							   const std::optional<E_COLOR_SCHEME> p_currentScheme
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
			addItem( "Carbon by chain", { E_COLOR_SCHEME::CARBON_CHAIN }, p_scheme );
			addItem( "Residues", { E_COLOR_SCHEME::RESIDUE }, p_scheme );
			addItem( "Chains", { E_COLOR_SCHEME::CHAIN }, p_scheme );

			auto addCustomSubMenu = [ this, &colorlayout ]( const QString & p_label, const E_COLOR_SCHEME p_scheme )
			{
				auto * subMenu = QMenu::addMenu( p_label );
				for ( ColorIndex i = 0; i < Color::LAYOUT_COUNT_CUSTOM; ++i )
				{
					auto * wa	= new QWidgetAction( subMenu );
					auto * item = new ColorItem( QT::Helper::toQColor( colorlayout.getCustomColor( i ) ) );
					item->setMinimumSize( 120, 24 );
					const Selected selected = { p_scheme, static_cast<ColorIndex>( Color::LAYOUT_OFFSET_CUSTOM + i ) };
					wa->setDefaultWidget( item );
					connect( wa, &QAction::triggered, this, [ this, selected ]() { emit this->selected( selected ); } );
					subMenu->addAction( wa );
				}
			};

			addCustomSubMenu( "Custom", E_COLOR_SCHEME::CUSTOM );
			addCustomSubMenu( "Carbon custom", E_COLOR_SCHEME::CARBON_CUSTOM );

			if ( p_secondaryStructureScheme )
			{
				addSeparator();
				QMenu::addSection( "Secondary structure" );

				auto addSecondaryStructureSubItem
					= [ this ](
						  const QString &										  p_label,
						  const E_COLOR_SCHEME_SECONDARY_STRUCTURE				  p_scheme,
						  const std::optional<E_COLOR_SCHEME_SECONDARY_STRUCTURE> p_currentScheme
					  )
				{
					QAction * a = QMenu::addAction( p_label );
					if ( p_currentScheme )
					{
						a->setCheckable( true );
						a->setChecked( *p_currentScheme == p_scheme );
					}
					connect(
						a,
						&QAction::triggered,
						this,
						[ this, p_scheme ]()
						{ emit secondaryStructureSelected( SecondaryStructureSelected { p_scheme } ); }
					);
				};

				addSecondaryStructureSubItem(
					"Types", E_COLOR_SCHEME_SECONDARY_STRUCTURE::STRUCTURE, p_secondaryStructureScheme
				);
				addSecondaryStructureSubItem(
					"Chains", E_COLOR_SCHEME_SECONDARY_STRUCTURE::CHAIN, p_secondaryStructureScheme
				);
				addSecondaryStructureSubItem(
					"Residues", E_COLOR_SCHEME_SECONDARY_STRUCTURE::RESIDUE, p_secondaryStructureScheme
				);

				auto * const customSecondaryStructureMenu = QMenu::addMenu( "Custom" );
				for ( ColorIndex i = 0; i < Color::LAYOUT_COUNT_CUSTOM; ++i )
				{
					auto * wa	= new QWidgetAction( customSecondaryStructureMenu );
					auto * item = new ColorItem( QT::Helper::toQColor( colorlayout.getCustomColor( i ) ) );
					item->setMinimumSize( 120, 24 );
					const SecondaryStructureSelected selected
						= { E_COLOR_SCHEME_SECONDARY_STRUCTURE::CUSTOM,
							static_cast<ColorIndex>( Color::LAYOUT_OFFSET_CUSTOM + i ) };
					wa->setDefaultWidget( item );
					connect(
						wa,
						&QAction::triggered,
						this,
						[ this, selected ]() { emit this->secondaryStructureSelected( selected ); }
					);
					customSecondaryStructureMenu->addAction( wa );
				}
			}
		}

	  signals:
		void selected( const Selected & );
		void secondaryStructureSelected( const SecondaryStructureSelected & );

	  private:
		/**
		 * @brief Private widget to display a color in the menu.
		 */
		class ColorItem : public QWidget
		{
		  public:
			ColorItem( const QColor & c, QWidget * parent = nullptr ) : QWidget( parent ), _color( c )
			{
				setMinimumHeight( 24 );
			}

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
