#ifndef __VTX_UI_WIDGET_CHAIN_SEQUENCE_WIDGET__
#define __VTX_UI_WIDGET_CHAIN_SEQUENCE_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "sequence_display_widget.hpp"
#include "ui/widget/view_item_widget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QString>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				class ChainSequenceWidget : public ViewItemWidget<Model::Chain>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void refresh() override;
					void localize() override;

				  protected:
					ChainSequenceWidget( QWidget * p_parent );
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;
					void _onSequenceSelectionChanged() const;

				  private:
					QLabel *				_scaleWidget		   = nullptr;
					SequenceDisplayWidget * _sequenceDisplayWidget = nullptr;
					QVBoxLayout *			_layout				   = nullptr;
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
