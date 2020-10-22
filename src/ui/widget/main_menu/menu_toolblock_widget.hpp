#ifndef __VTX_UI_WIDGET_BASE_MENUTOOLBLOCK_WIDGET__
#define __VTX_UI_WIDGET_BASE_MENUTOOLBLOCK_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "menu_toolbutton_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QGridLayout>
#include <QLabel>
#include <QWidget>
#include <vector>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				class MenuToolBlockWidget : public BaseManualWidget<QWidget>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  private:
					class TmpGridStructure
					{
					  private:
						inline static const size_t MAX_COLUMN_COUNT		 = 5;
						inline static const size_t MAX_ROW_COUNT		 = 3;
						inline static const int	   GRID_LAYOUT_ROW_COUNT = 6;

					  public:
						TmpGridStructure()
						{
							_columnsData = std::vector<std::vector<MenuToolButtonWidget *> *>();
							_columnsData.reserve( MAX_COLUMN_COUNT );
						};
						~TmpGridStructure();

						const size_t getNbColumns() { return _columnsData.size(); };

						void pushWidgetInColumn( const int p_column, MenuToolButtonWidget * const p_widget );
						void fillGridLayout( QGridLayout & p_gridLayout, const int p_startRow = 0 );

					  private:
						void addNewColumn( const size_t p_nbColumns = 1 );

						std::vector<std::vector<MenuToolButtonWidget *> *> _columnsData;
					};

				  public:
					~MenuToolBlockWidget()
					{
						if ( _tmpStructure != nullptr )
							delete _tmpStructure;
					};

					void localize() override;

					void setTitle( const QString & p_title );
					void validate();

					void pushButton( MenuToolButtonWidget & p_toolButton, const int p_column );

				  protected:
					MenuToolBlockWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) { _tmpStructure = new TmpGridStructure(); };
					virtual void _setupUi( const QString & p_name ) override;
					virtual void _setupSlots() override;

				  private:
					QGridLayout * _gridLayout = nullptr;
					QLabel *	  _title	  = nullptr;

					TmpGridStructure * _tmpStructure;
				};
			} // namespace MainMenu
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
