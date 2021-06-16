#ifndef __VTX_UI_WIDGET_BASE_MENUTOOLBLOCK_WIDGET__
#define __VTX_UI_WIDGET_BASE_MENUTOOLBLOCK_WIDGET__

#include "menu_toolbutton_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

namespace VTX::UI::Widget::MainMenu
{
	class MenuToolBlockWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET

	  public:
		inline static const size_t MAX_COLUMN_COUNT		 = 5;
		inline static const size_t MAX_ROW_COUNT		 = 3;
		inline static const int	   GRID_LAYOUT_ROW_COUNT = 6;

	  private:
		class ColumnData
		{
		  public:
			ColumnData( const int p_maxRowCount ) { _data.reserve( p_maxRowCount ); }

			void addButton( MenuToolButtonWidget * const p_widget ) { _data.emplace_back( p_widget ); }
			MenuToolButtonWidget * const getButton( const int p_row ) { return _data[ p_row ]; }
			int							 getRowCount() const { return int( _data.size() ); }

			void setNbRows( const int p_size ) { _forcedSize = p_size; }
			int	 getNbRowsDisplayed() const { return _forcedSize > 0 ? _forcedSize : int( _data.size() ); }

		  private:
			std::vector<MenuToolButtonWidget *> _data		= std::vector<MenuToolButtonWidget *>();
			int									_forcedSize = -1;
		};

		class TmpGridStructure
		{
		  public:
			TmpGridStructure()
			{
				_columnsData = std::vector<ColumnData>();
				_columnsData.reserve( MAX_COLUMN_COUNT );
			};
			~TmpGridStructure();

			const int getNbColumns() { return int( _columnsData.size() ); };
			void	  setNbRowsInColumn( const int p_column, const int p_size )
			{
				_columnsData[ p_column ].setNbRows( p_size );
			}

			void pushWidgetInColumn( const int p_column, MenuToolButtonWidget * const p_widget );
			void fillGridLayout( QGridLayout & p_gridLayout, const int p_startRow = 0 );

		  private:
			void					addNewColumn( const size_t p_nbColumns = 1 );
			std::vector<ColumnData> _columnsData;
		};

	  public:
		~MenuToolBlockWidget()
		{
			if ( _tmpStructure != nullptr )
				delete _tmpStructure;

			delete _gridLayout;
		};

		void localize() override;

		void setTitle( const QString & p_title );
		void validate();

		void pushButton( MenuToolButtonWidget & p_toolButton, const int p_column );
		int	 pushButtonInNextColumn( MenuToolButtonWidget & p_toolButton );
		void reset();

	  protected:
		MenuToolBlockWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
		{
			_tmpStructure = new TmpGridStructure();
		};
		virtual void _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		int	 getColumnCount() const { return _tmpStructure->getNbColumns(); }
		void setRowCountInColumn( const int p_column, const int p_nbRows )
		{
			_tmpStructure->setNbRowsInColumn( p_column, p_nbRows );
		}

	  private:
		QVBoxLayout * _verticalLayout = nullptr;
		QGridLayout * _gridLayout	  = nullptr;
		QLabel *	  _title		  = nullptr;

		TmpGridStructure * _tmpStructure;
	};
} // namespace VTX::UI::Widget::MainMenu
#endif
