#ifndef __VTX_UI_QT_LAYOUT_FLOW_LAYOUT__
#define __VTX_UI_QT_LAYOUT_FLOW_LAYOUT__

#include <QLayout>
#include <QRect>
#include <QStyle>
#include <QWidget>

namespace VTX::UI::QT::Layout
{
	/**
	 * @brief https://doc.qt.io/qt-6.7/qtwidgets-layouts-flowlayout-example.html
	 */
	class FlowLayout : public QLayout
	{
	  public:
		explicit FlowLayout( QWidget *, int = -1, int = -1, int = -1 );
		explicit FlowLayout( int = -1, int = -1, int = -1 );
		~FlowLayout();

		void			 addItem( QLayoutItem * ) override;
		int				 horizontalSpacing() const;
		int				 verticalSpacing() const;
		Qt::Orientations expandingDirections() const override;
		bool			 hasHeightForWidth() const override;
		int				 heightForWidth( int ) const override;
		int				 count() const override;
		QLayoutItem *	 itemAt( int index ) const override;
		QSize			 minimumSize() const override;
		void			 setGeometry( const QRect & rect ) override;
		QSize			 sizeHint() const override;
		QLayoutItem *	 takeAt( int index ) override;

	  private:
		int _doLayout( const QRect & rect, bool testOnly ) const;
		int _smartSpacing( QStyle::PixelMetric pm ) const;

		QList<QLayoutItem *> _itemList;
		int					 _hSpace;
		int					 _vSpace;
	};
} // namespace VTX::UI::QT::Layout

#endif
