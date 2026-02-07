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

		ColorScheme( QWidget * p_parent, const std::optional<Selected> p_selected = std::nullopt ) :
			BaseWidget( p_parent )
		{
			using namespace App::System;

			setTitle( "Color scheme" );

			auto addItem = [ this ]( const QString & p_label, const Selected & p_data )
			{
				QAction * a = QWidget::addAction( p_label );
				a->setCheckable( true );
				// a->setChecked( checked );
				a->setData( QVariant::fromValue( p_data ) );
			};

			addItem( "Atoms", { E_COLOR_SCHEME::ATOM } );
			addItem( "Residues", { E_COLOR_SCHEME::RESIDUE } );
			addItem( "Chains", { E_COLOR_SCHEME::CHAIN } );
			// addItem( "Plain" );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
