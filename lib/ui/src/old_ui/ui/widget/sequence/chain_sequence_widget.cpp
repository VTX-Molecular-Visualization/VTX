#include "ui/old_ui/ui/widget/sequence/chain_sequence_widget.hpp"
#include "ui/old_ui/style.hpp"
#include <QFont>
#include <QString>
#include <QVBoxLayout>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <string>
#include <util/logger.hpp>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				ChainSequenceWidget::ChainSequenceWidget( QWidget * p_parent ) : ViewItemWidget( p_parent ) {}

				ChainSequenceWidget::~ChainSequenceWidget()
				{
					if ( _chainData != nullptr )
						delete _chainData;
				}

				void ChainSequenceWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					_layout = new QVBoxLayout( this );
					_layout->setContentsMargins( 0, 0, 0, 0 );
					_layout->setSpacing( 1 );
					_sequenceDisplayWidget = new SequenceDisplayWidget( this );
					_sequenceDisplayWidget->setContentsMargins( 0, 0, 0, 0 );
					_scaleWidget = new QLabel();
					_scaleWidget->setContentsMargins( 0, 0, 0, 0 );
					_scaleWidget->setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum );
					_scaleWidget->setFont( Style::SEQUENCE_DISPLAY_FONT() );
					_scaleWidget->setAlignment( Qt::AlignmentFlag::AlignTop );

					_layout->addWidget( _scaleWidget );
					_layout->addWidget( _sequenceDisplayWidget );
					_layout->addStretch( 1000 );
				}

				void ChainSequenceWidget::_setupSlots() {}

				void ChainSequenceWidget::localize() {}
				void ChainSequenceWidget::refresh()
				{
					_chainData = new SequenceChainData( *_model );
					_sequenceDisplayWidget->setupSequence( _chainData );

					const QString scaleString = _chainData->getScale();
					_scaleWidget->setText( scaleString );
				}

				App::Component::Chemistry::Residue * const ChainSequenceWidget::getResidueAtPos( const QPoint & p_pos ) const
				{
					const QPoint localPos = _sequenceDisplayWidget->mapFrom( parentWidget(), p_pos );
					return _sequenceDisplayWidget->getResidueAtPos( localPos );
				}

				App::Component::Chemistry::Residue & ChainSequenceWidget::getClosestResidueFromPos( const QPoint & p_pos,
																				const bool	   p_takeForward ) const
				{
					const QPoint localPos = _sequenceDisplayWidget->mapFrom( parentWidget(), p_pos );
					return *( _sequenceDisplayWidget->getClosestResidueFromPos( localPos, p_takeForward ) );
				}

			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
