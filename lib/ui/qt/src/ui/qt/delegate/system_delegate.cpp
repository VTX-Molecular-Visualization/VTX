#include "ui/qt/delegate/system_delegate.hpp"
#include <QApplication>
#include <QPainter>

namespace VTX::UI::QT::Delegate
{

	SystemDelegate::SystemDelegate( const App::ECS::Entity p_system, QObject * p_parent ) :
		QStyledItemDelegate( p_parent ), _system( p_system )
	{
	}

	void SystemDelegate::paint(
		QPainter *					 p_painter,
		const QStyleOptionViewItem & p_option,
		const QModelIndex &			 p_index
	) const
	{
		QStyledItemDelegate::paint( p_painter, p_option, p_index );
	}

	QSize SystemDelegate::sizeHint( const QStyleOptionViewItem & p_option, const QModelIndex & p_index ) const
	{
		return QStyledItemDelegate::sizeHint( p_option, p_index );
	}

	QWidget * SystemDelegate::createEditor(
		QWidget *					 p_parent,
		const QStyleOptionViewItem & p_option,
		const QModelIndex &			 p_index
	) const
	{
		return QStyledItemDelegate::createEditor( p_parent, p_option, p_index );
	}

	void SystemDelegate::setEditorData( QWidget * p_editor, const QModelIndex & p_index ) const
	{
		QStyledItemDelegate::setEditorData( p_editor, p_index );
	}

	void SystemDelegate::setModelData(
		QWidget *			 p_editor,
		QAbstractItemModel * p_model,
		const QModelIndex &	 p_index
	) const
	{
		QStyledItemDelegate::setModelData( p_editor, p_model, p_index );
	}

} // namespace VTX::UI::QT::Delegate
