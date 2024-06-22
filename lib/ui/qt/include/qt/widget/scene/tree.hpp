#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_TREE_WIDGET__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_TREE_WIDGET__

#include "data_concept.hpp"
#include "qt/core/base_manual_widget.hpp"
#include <QTreeWidget>
#include <app/component/scene/scene_item_component.hpp>

namespace VTX::UI::QT::Widget::Scene
{
	class Tree : public QT::Core::BaseManualWidget<QTreeWidget>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		using WidgetData = size_t;
		struct TreeItemData
		{
			std::string name;
			WidgetData	data;
			size_t		childrenCount;
		};

		using LoadFunc = std::function<std::vector<TreeItemData>( const uint p_level, const WidgetData p_data )>;

		void localize() override;

		void addTopLevelData( const TreeItemData & p_data, const LoadFunc & p_loadFunc )
		{
			QTreeWidgetItem * item = new QTreeWidgetItem();

			auto * p = item->parent();

			static Qt::ItemFlags flags = Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsUserCheckable;

			item->setFlags( flags );
			item->setData( 0, Qt::UserRole, QVariant::fromValue( p_data.data ) );
			item->setText( 0, QString::fromUtf8( p_data.name ) );
			// item->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( model.getTypeId() ) );
			item->setCheckState( 0, Qt::Checked );

			if ( p_data.childrenCount != 0 )
			{
				_resetTreeItem( item );
			}

			assert( not _loadFuncs.contains( item ) );
			_loadFuncs.emplace( item, p_loadFunc );

			addTopLevelItem( item );
		}

	  protected:
		Tree( QWidget * p_parent = nullptr );
		~Tree() = default;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		std::map<QTreeWidgetItem *, LoadFunc> _loadFuncs;

		void _resetTreeItem( QTreeWidgetItem * const p_item );
	};
} // namespace VTX::UI::QT::Widget::Scene

#endif
