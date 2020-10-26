#ifndef __VTX_UI_WIDGET_SEQUENCE_DISPLAY_WIDGET__
#define __VTX_UI_WIDGET_SEQUENCE_DISPLAY_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include <QLabel>
#include <vector>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				class SequenceDisplayWidget : public QLabel
				{
					Q_OBJECT

				  public:
					SequenceDisplayWidget( QWidget * p_parent = nullptr ) : QLabel( p_parent )
					{
						_updateSelectionData();
						setTextInteractionFlags( Qt::TextInteractionFlag::TextSelectableByMouse );
					}

					void setupSequence( const Model::Molecule & p_molecule );

					const std::vector<uint> & getSelection() const { return _selection; };
					std::vector<uint> &		  getSelection() { return _selection; };

				  signals:
					void selectionChanged();

				  protected:
					void mousePressEvent( QMouseEvent * ev ) override;
					void mouseReleaseEvent( QMouseEvent * ev ) override;

				  private:
					bool	_lastHasSelection;
					int		_lastFirstIndexSelection;
					QString _lastSelection;

					const Model::Molecule * _model			  = nullptr;
					std::vector<uint>		_selection		  = std::vector<uint>();
					std::vector<int>		_chainsFirstIndex = std::vector<int>();

					void _buildSelection();

					inline void _updateSelectionData()
					{
						_lastHasSelection		 = hasSelectedText();
						_lastFirstIndexSelection = selectionStart();
						_lastSelection			 = selectedText();
					};
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
