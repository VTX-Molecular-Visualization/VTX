#ifndef __VTX_UI_WIDGET_SCENE__
#define __VTX_UI_WIDGET_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/base_model.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QDockWidget>
#include <QTreeWidget>
#include <QVariant>

Q_DECLARE_METATYPE( VTX::Model::Model_ID )

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Scene
			{
				class SceneWidget : public BaseManualWidget<QDockWidget>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void receiveEvent( const Event::VTXEvent & p_event ) override;
					void localize() override;

					void			  addItem( QTreeWidgetItem * p_item );
					void			  addItem( QTreeWidgetItem * p_item, QTreeWidgetItem * p_parent );
					QTreeWidgetItem * takeItem( QTreeWidgetItem * p_item );
					void			  deleteItem( QTreeWidgetItem * p_item );

				  protected:
					SceneWidget( QWidget * p_parent );
					void							 _setupUi( const QString & p_name ) override;
					void							 _setupSlots() override;

				  private:
					QTreeWidget * _treeWidget = nullptr;

					void _onSelectionChange();
					void _onItemChange( QTreeWidgetItem * item, int column );
					void _onItemClicked( QTreeWidgetItem *, int );

					inline Model::Model_ID _getModelID( const QTreeWidgetItem & p_item ) const
					{
						const QVariant & dataID = p_item.data( 0, Qt::UserRole );
						return dataID.value<VTX::Model::Model_ID>();
					};
					void _sendEnableStateChangeAction( const Model::Model_ID & p_modelID, const bool modelEnabled ) const;

					const Qt::CheckState _getCheckState( const bool p_enable ) const { return p_enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked; };
				};

			} // namespace Scene
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX

#endif
