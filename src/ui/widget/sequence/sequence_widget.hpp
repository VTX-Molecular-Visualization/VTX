#ifndef __VTX_UI_WIDGET_SEQUENCE__
#define __VTX_UI_WIDGET_SEQUENCE__

#ifdef _MSC_VER
#pragma once
#endif

#include "molecule_sequence_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QDockWidget>
#include <QVBoxLayout>
#include <unordered_set>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				class SequenceWidget : public BaseManualWidget<QDockWidget>
				{
					VTX_WIDGET

				  public:
					void receiveEvent( const Event::VTXEvent & p_event ) override;
					void localize() override;
					void refreshSelection() const;

				  protected:
					SequenceWidget( QWidget * p_parent );
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

				  private:
					QVBoxLayout *								 _layout = nullptr;
					std::unordered_set<MoleculeSequenceWidget *> _moleculeWidgets;
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
