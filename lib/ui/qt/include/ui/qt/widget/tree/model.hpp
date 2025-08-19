#ifndef __VTX_UI_QT_WIDGET_TREE_MODEL__
#define __VTX_UI_QT_WIDGET_TREE_MODEL__

#include <QAbstractItemModel>
#include <QString>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <vector>

namespace VTX::UI::QT::Widget::Tree
{

	class Model : public QAbstractItemModel
	{
		Q_OBJECT
	  public:
		Model( const App::Component::Chemistry::System & p_system, QObject * parent = nullptr ) :
			QAbstractItemModel( parent ), _system( p_system )
		{
		}

		// QModelIndex = quint64 = size_t

		/*
		int columnCount( const QModelIndex & p_parent = QModelIndex() ) const override {}

		int rowCount( const QModelIndex & p_parent = QModelIndex() ) const override {}

		QVariant data( const QModelIndex & p_index, int p_role = Qt::DisplayRole ) const override {}

		QModelIndex index( int p_row, int p_column, const QModelIndex & p_parent = QModelIndex() ) const override {}

		QModelIndex parent( const QModelIndex & p_index ) const override {}
		*/

	  private:
		const App::Component::Chemistry::System & _system;
	};

} // namespace VTX::UI::QT::Widget::Tree
#endif
