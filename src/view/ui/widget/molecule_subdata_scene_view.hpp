#ifndef __VTX_VIEW_UI_WIDGET_BASE_SCENE_ITEM_VIEW__
#define __VTX_VIEW_UI_WIDGET_BASE_SCENE_ITEM_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include <QTreeWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeSubDataSceneView : public QTreeWidgetItem
				{
				  public:
					MoleculeSubDataSceneView( QTreeWidgetItem * p_parent ) : QTreeWidgetItem( p_parent ) { _setupUi(); };
					inline void setData( const Model::Chain & p_chain ) { setText( 0, QString::fromStdString( p_chain.getName() ) ); };
					inline void setData( const Model::Residue & p_residue ) { setText( 0, QString::fromStdString( p_residue.getSymbolName() ) ); };

				  protected:
					inline void _setupUi() { setCheckState( 0, Qt::CheckState::Checked ); };
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
