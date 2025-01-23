#ifndef __VTX_UI_QT_WIDGET_SCENE_TREE_MODEL__
#define __VTX_UI_QT_WIDGET_SCENE_TREE_MODEL__

#include "item.hpp"
#include <QAbstractItemModel>

namespace VTX::UI::QT::Widget::SceneTree
{
	class Model : public QAbstractItemModel
	{
		Q_OBJECT

	  public:
		Q_DISABLE_COPY_MOVE( Model )

		explicit Model( const QString & data, QObject * parent = nullptr );
		~Model() override;

		QVariant	  data( const QModelIndex & index, int role ) const override;
		Qt::ItemFlags flags( const QModelIndex & index ) const override;
		QVariant	  headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
		QModelIndex	  index( int row, int column, const QModelIndex & parent = {} ) const override;
		QModelIndex	  parent( const QModelIndex & index ) const override;
		int			  rowCount( const QModelIndex & parent = {} ) const override;
		int			  columnCount( const QModelIndex & parent = {} ) const override;

	  private:
		static void setupModelData( const QList<QStringView> & lines, Item * parent );

		std::unique_ptr<Item> rootItem;
	};
} // namespace VTX::UI::QT::Widget::SceneTree

#endif
