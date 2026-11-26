#ifndef VTX_UI_QT_WIDGET_TREE_BASE_TREE_PRESETS_HPP
#define VTX_UI_QT_WIDGET_TREE_BASE_TREE_PRESETS_HPP

#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/library/preset_selector.hpp"
#include "ui/qt/widget/tree/base_tree.hpp"
#include <QTreeWidget>
#include <app/action/preset.hpp>
#include <app/generic/name.hpp>
#include <app/generic/readonly.hpp>
#include <app/services.hpp>
#include <memory>
#include <util/action_manager.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget::Tree
{
	template<typename P>
	class BaseTreePreset : public Widget::Tree::BaseTree<BaseTreePreset<P>>
	{
	  public:
		using W = Widget::Tree::BaseTree<BaseTreePreset<P>>;

		BaseTreePreset( QWidget * const p_parent ) :
			W( p_parent ),
			_onRenameConnection(
				App::HUB().connect<App::Events::PresetRename<P>, &BaseTreePreset::_onPresetRename>( this )
			)
		{
			W::setExpandsOnDoubleClick( true );

			auto & reg			   = App::REG();
			_onConstructConnection = reg.on_construct<P>().template connect<&BaseTreePreset::_addPreset>( this );
			_onDestroyConnection   = reg.on_destroy<P>().template connect<&BaseTreePreset::_removePreset>( this );

			// Connect double click to apply the preset.
			W::connect(
				this,
				&QTreeWidget::itemDoubleClicked,
				this,
				[]( QTreeWidgetItem * item, int column )
				{
					if ( item == nullptr || not item->data( 0, Qt::UserRole ).isValid() || column != 0 )
					{
						return;
					}

					Entity entity = item->data( 0, Qt::UserRole ).value<Entity>();
					App::ACTION().execute<App::Action::Preset::Apply<P>>( entity );
				}
			);
		}

	  protected:
		void _addGroups()
		{
			assert( W::topLevelItem( 0 ) != nullptr );

			_builtInItem = new QTreeWidgetItem( W::topLevelItem( 0 ), QStringList { "Built-in" } );
			_builtInItem->setData(
				0, Library::PRESET_SCOPE_ROLE, static_cast<int>( Library::E_PRESET_SCOPE::BUILT_IN )
			);
			_builtInItem->setExpanded( true );
			_userItem = new QTreeWidgetItem( W::topLevelItem( 0 ), QStringList { "User" } );
			_userItem->setData( 0, Library::PRESET_SCOPE_ROLE, static_cast<int>( Library::E_PRESET_SCOPE::USER ) );
			_userItem->setExpanded( true );
		}

		void _restoreExpansionState( const QAnyStringView p_settingKey )
		{
			assert( W::topLevelItem( 0 ) != nullptr );

			W::topLevelItem( 0 )->setExpanded( SETTINGS().value( p_settingKey, true ).toBool() );

			W::connect(
				this,
				&QTreeView::expanded,
				this,
				[ p_settingKey ]( const QModelIndex & p_index )
				{
					if ( not p_index.parent().isValid() )
					{
						SETTINGS().setValue( p_settingKey, true );
					}
				}
			);
			W::connect(
				this,
				&QTreeView::collapsed,
				this,
				[ p_settingKey ]( const QModelIndex & p_index )
				{
					if ( not p_index.parent().isValid() )
					{
						SETTINGS().setValue( p_settingKey, false );
					}
				}
			);
		}

		void _setAppliedPreset( const Entity p_preset )
		{
			if ( _appliedPreset == p_preset )
			{
				return;
			}

			_setPresetApplied( _appliedPreset, false );
			_appliedPreset = p_preset;
			_setPresetApplied( _appliedPreset, true );
			W::updateGeometry();
		}

	  private:
		/**
		 * @brief Map entities to tree items.
		 */
		std::unordered_map<Entity, QTreeWidgetItem *> _entityToItemMap;
		Entity										  _appliedPreset = InvalidEntity;
		QTreeWidgetItem *							  _builtInItem	 = nullptr;
		QTreeWidgetItem *							  _userItem		 = nullptr;

		/**
		 * @brief Connections to App.
		 */
		VTX::Util::EventHub::ScopedConnection _onConstructConnection;
		VTX::Util::EventHub::ScopedConnection _onDestroyConnection;

		/**
		 * @brief Add a preset to the tree.
		 */
		void _addPreset( Registry & p_r, Entity p_e )
		{
			assert( not _entityToItemMap.contains( p_e ) );

			const auto &			name = p_r.get<App::Generic::Name>( p_e );
			QTreeWidgetItem * const presetItem
				= new QTreeWidgetItem( QStringList() << QString::fromStdString( name.name ) );
			presetItem->setData( 0, Qt::UserRole, QVariant::fromValue( p_e ) );
			_setPresetItemApplied( presetItem, p_e == _appliedPreset );
			QTreeWidgetItem * const group = p_r.all_of<App::Generic::ReadOnly>( p_e ) ? _builtInItem : _userItem;
			group->addChild( presetItem );
			group->sortChildren( 0, Qt::AscendingOrder );
			_entityToItemMap.emplace( p_e, presetItem );
			W::updateGeometry();
		}

		/**
		 * @brief Remove a preset from the tree.
		 */
		void _removePreset( Registry & p_r, Entity p_e )
		{
			assert( _entityToItemMap.contains( p_e ) );

			QTreeWidgetItem * const presetItem = _entityToItemMap.at( p_e );
			auto * const			parent	   = presetItem->parent();
			std::unique_ptr<QTreeWidgetItem> { parent->takeChild( parent->indexOfChild( presetItem ) ) }.reset();
			_entityToItemMap.erase( p_e );
			if ( _appliedPreset == p_e )
			{
				_appliedPreset = InvalidEntity;
			}
			W::updateGeometry();
		}

		/**
		 * @brief Update widget when a preset is renamed from App.
		 */
		void _onPresetRename( const App::Events::PresetRename<P> & p_event )
		{
			assert( _entityToItemMap.contains( p_event.preset ) );

			QTreeWidgetItem * const presetItem = _entityToItemMap.at( p_event.preset );
			presetItem->setText( 0, QString::fromStdString( p_event.name ) );
			presetItem->parent()->sortChildren( 0, Qt::AscendingOrder );
		}

		/**
		 * @brief Set the applied state.
		 */
		void _setPresetApplied( const Entity p_preset, const bool p_applied )
		{
			const auto itemIt = _entityToItemMap.find( p_preset );
			if ( itemIt != _entityToItemMap.end() )
			{
				_setPresetItemApplied( itemIt->second, p_applied );
			}
		}

		/**
		 * @brief Set current preset bold.
		 */
		void _setPresetItemApplied( QTreeWidgetItem * const p_item, const bool p_applied )
		{
			QFont font = p_item->font( 0 );
			font.setBold( p_applied );
			p_item->setFont( 0, font );
		}

		VTX::Util::EventHub::ScopedConnection _onRenameConnection;
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
