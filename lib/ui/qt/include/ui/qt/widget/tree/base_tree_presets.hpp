#ifndef __VTX_UI_QT_WIDGET_TREE_BASE_TREE_PRESETS__
#define __VTX_UI_QT_WIDGET_TREE_BASE_TREE_PRESETS__

#include "ui/qt/widget/tree/base_tree.hpp"
#include <QTreeWidget>
#include <app/preset/name.hpp>

namespace VTX::UI::QT::Widget::Tree
{
	template<typename P>
	class BaseTreePreset : public Widget::Tree::BaseTree<BaseTreePreset<P>>
	{
	  public:
		using W = Widget::Tree::BaseTree<BaseTreePreset<P>>;

		BaseTreePreset( QWidget * p_parent ) : W( p_parent )
		{
			W::setExpandsOnDoubleClick( true );

			auto & reg = App::REG();
			reg.on_construct<P>().template connect<&BaseTreePreset::_addPreset>( this );
			reg.on_destroy<P>().template connect<&BaseTreePreset::_removePreset>( this );
			App::HUB().connect<App::Events::PresetRename<P>, &BaseTreePreset::_onPresetRename>( this );
		}

		virtual ~BaseTreePreset() = default;

	  private:
		/**
		 * @brief Map entities to tree items.
		 */
		std::unordered_map<App::ECS::Entity, QTreeWidgetItem *> _entityToItemMap;

		/**
		 * @brief Add a preset to the tree.
		 */
		void _addPreset( App::ECS::Registry & p_r, App::ECS::Entity p_e )
		{
			assert( not _entityToItemMap.contains( p_e ) );

			const auto &			name = p_r.get<App::Preset::Name>( p_e );
			QTreeWidgetItem * const presetItem
				= new QTreeWidgetItem( QStringList() << QString::fromStdString( name.name ) );
			W::topLevelItem( 0 )->addChild( presetItem );
			_entityToItemMap.emplace( p_e, presetItem );
		}

		/**
		 * @brief Remove a preset from the tree.
		 */
		void _removePreset( App::ECS::Registry & p_r, App::ECS::Entity p_e )
		{
			assert( _entityToItemMap.contains( p_e ) );

			QTreeWidgetItem * const presetItem = _entityToItemMap.at( p_e );
			delete presetItem;
			_entityToItemMap.erase( p_e );
		}

		/**
		 * @brief Update widget when a preset is renamed from App.
		 */
		void _onPresetRename( const App::Events::PresetRename<P> & p_event )
		{
			assert( _entityToItemMap.contains( p_event.preset ) );

			QTreeWidgetItem * const presetItem = _entityToItemMap.at( p_event.preset );
			presetItem->setText( 0, QString::fromStdString( p_event.name ) );
		}
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
