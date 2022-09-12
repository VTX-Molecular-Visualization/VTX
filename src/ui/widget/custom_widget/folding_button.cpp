#include "folding_button.hpp"
#include "style.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace VTX::UI::Widget::CustomWidget
{
	FoldingButton::FoldingButton( QWidget * p_parent, QWidget * p_content ) :
		BaseManualWidget( p_parent ), _content( p_content ) {};

	void FoldingButton::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QVBoxLayout * verticalLayout = new QVBoxLayout( this );
		verticalLayout->setContentsMargins( 0, 0, 0, 0 );

		QHBoxLayout * const foldButtonHLayout = new QHBoxLayout();
		foldButtonHLayout->setContentsMargins( 0, 0, 0, 0 );

		_foldButton = new QPushButton( this );
		_foldButton->setFlat( true );
		_foldButton->setIcon( Style::IconConst::get().FOLDED_PIXMAP );
		_foldButton->setContentsMargins( 0, 10, 10, 10 );

		foldButtonHLayout->addWidget( _foldButton );
		foldButtonHLayout->addStretch( 1000 );

		QHBoxLayout * const contentHLayout = new QHBoxLayout();
		contentHLayout->setContentsMargins( 0, 0, 0, 0 );

		_content->setVisible( false );
		contentHLayout->addSpacing( Style::DATA_GRID_HORIZONTAL_SPACE );
		contentHLayout->addWidget( _content );

		verticalLayout->addLayout( foldButtonHLayout );
		verticalLayout->addLayout( contentHLayout );

		setFoldState( false );
	}

	void FoldingButton::_setupSlots()
	{
		connect( _foldButton, &QPushButton::clicked, this, &FoldingButton::toggleFoldState );
	}
	void FoldingButton::localize() {};

	void FoldingButton::setTitle( const QString & p_title ) const { _foldButton->setText( p_title ); }

	void FoldingButton::toggleFoldState() const { setFoldState( !_content->isVisible() ); }

	void FoldingButton::setFoldState( const bool p_expand ) const
	{
		_content->setVisible( p_expand );

		const QIcon & icon = p_expand ? Style::IconConst::get().UNFOLDED_PIXMAP : Style::IconConst::get().FOLDED_PIXMAP;
		_foldButton->setIcon( icon );
	}

} // namespace VTX::UI::Widget::CustomWidget
