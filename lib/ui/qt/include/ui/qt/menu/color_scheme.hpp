#ifndef __VTX_UI_QT_MENU_COLOR_SCHEME__
#define __VTX_UI_QT_MENU_COLOR_SCHEME__

#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>
#include <app/system/color.hpp>
#include <optional>
#include <renderer/color.hpp>

namespace VTX::UI::QT::Menu
{

	class ColorScheme : public Widget::BaseWidget<ColorScheme, QMenu>
	{
	  public:
		struct Selected
		{
			App::System::E_COLOR_SCHEME			scheme;
			std::optional<Renderer::ColorIndex> index = std::nullopt;
		};

		ColorScheme( QWidget * p_parent, const std::optional<App::System::E_COLOR_SCHEME> p_scheme = std::nullopt ) :
			BaseWidget( p_parent )
		{
			using namespace App::System;

			setTitle( "Color scheme" );

			auto addItem = [ this ](
							   const QString &									p_label,
							   const Selected &									p_data,
							   const std::optional<App::System::E_COLOR_SCHEME> p_currentScheme
						   )
			{
				QAction * a = QWidget::addAction( p_label );
				a->setCheckable( true );
				a->setChecked( p_currentScheme && *p_currentScheme == p_data.scheme );
				a->setData( QVariant::fromValue( p_data ) );
			};

			addItem( "Atoms", { E_COLOR_SCHEME::ATOM }, p_scheme );
			addItem( "Residues", { E_COLOR_SCHEME::RESIDUE }, p_scheme );
			addItem( "Chains", { E_COLOR_SCHEME::CHAIN }, p_scheme );
			// addItem( "Plain" );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
