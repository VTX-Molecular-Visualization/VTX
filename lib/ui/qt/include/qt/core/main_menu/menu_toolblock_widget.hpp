#ifndef __VTX_UI_QT_WIDGET_BASE_MENUTOOLBLOCK_WIDGET__
#define __VTX_UI_QT_WIDGET_BASE_MENUTOOLBLOCK_WIDGET__

#include "menu_toolbutton_widget.hpp"
#include "qt/core/base_manual_widget.hpp"
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <ui/core/main_menu/main_menu_toolblock.hpp>
#include <vector>

namespace VTX::UI::QT::Core::MainMenu
{
	class MenuToolBlockWidget : public UI::Core::MainMenu::MainMenuToolBlock, public BaseManualWidget<QWidget>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		inline static const size_t MAX_COLUMN_COUNT		 = 5;
		inline static const size_t MAX_ROW_COUNT		 = 3;
		inline static const int	   GRID_LAYOUT_ROW_COUNT = 6;

	  public:
		void localize() override;

		void			  setTitle( const std::string & p_title ) override;
		const std::string getTitle() const override;

		void pushButton( MenuToolButtonWidget & p_toolButton );
		void pushButton( MenuToolButtonWidget & p_toolButton1, MenuToolButtonWidget & p_toolButton2 );
		void pushButton(
			MenuToolButtonWidget & p_toolButton1,
			MenuToolButtonWidget & p_toolButton2,
			MenuToolButtonWidget & p_toolButton3
		);

		virtual void clear() override;

	  protected:
		MenuToolBlockWidget( QWidget * p_parent ) :
			UI::Core::MainMenu::MainMenuToolBlock(), BaseManualWidget( p_parent ) {};

		virtual void _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		// void setRowCountInColumn( const int p_column, const int p_nbRows )
		//{
		//	_tmpStructure.setNbRowsInColumn( p_column, p_nbRows );
		// }

		MenuToolButtonWidget * _findPreviousButton(
			const MenuToolBlockWidget * const p_toolBlock,
			QLayoutItem * const				  p_item,
			const int						  p_row
		) const;
		MenuToolButtonWidget * _findNextButton(
			const MenuToolBlockWidget * const p_toolBlock,
			QLayoutItem * const				  p_item,
			const int						  p_row
		) const;

		void _initButton( MenuToolButtonWidget & p_toolButton ) const;
		void _addButtonInLayout( MenuToolButtonWidget & p_toolButton );

	  private:
		QVBoxLayout * _verticalLayout = nullptr;
		QGridLayout * _gridLayout	  = nullptr;
		QLabel *	  _title		  = nullptr;

		void _connectNavigationEventToButton( MenuToolButtonWidget & p_toolButton ) const;

		void _navigationToLeft() const;
		void _navigationToRight() const;
		void _navigationToUp() const;
		void _navigationToDown() const;
	};
} // namespace VTX::UI::QT::Core::MainMenu
#endif
