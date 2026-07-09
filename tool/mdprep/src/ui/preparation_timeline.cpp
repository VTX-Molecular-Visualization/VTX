#include <array>
#include <functional>
//
#include <QDesktopServices>
#include <QDir>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVBoxLayout>
//
#include <app/services.hpp>
#include <ui/qt/application.hpp>
#include <ui/qt/services.hpp>
#include <ui/qt/widget/main_window.hpp>
#include <util/event_hub.hpp>
//
#include <tool/mdprep/actions/jobs.hpp>
//
#include <tool/mdprep/ui/preparation_timeline.hpp>

namespace VTX::Tool::Mdprep::ui
{
	namespace
	{
		enum class E_STEP_STATE
		{
			pending,
			running,
			done,
			failed
		};

		struct StateStyle
		{
			const char * color;
			const char * label;
		};

		StateStyle stateStyle( E_STEP_STATE p_state ) noexcept
		{
			switch ( p_state )
			{
			case E_STEP_STATE::running: return { "#4c9aff", "Running…" };
			case E_STEP_STATE::done: return { "#57d38c", "Done" };
			case E_STEP_STATE::failed: return { "#ff6b6b", "Failed" };
			case E_STEP_STATE::pending:
			default: return { "#8a9099", "Pending" };
			}
		}

		// The steps displayed in the timeline. The first g_NUM_PREPARATION_JOBS entries map, in order, to
		// GromacsInstructions::jobData indexes fired by the step events. The last entry is the packing step, driven by
		// the SystemPacked event.
		struct StepInfo
		{
			const char * title;
			const char * why;
		};

		const std::array<StepInfo, 10> g_steps { {
			{ "pdb2gmx — topology",
			  "Converts the input structure into a GROMACS topology and coordinate file, assigning the selected force "
			  "field and adding hydrogens." },
			{ "editconf — define box", "Places the solute inside a simulation box of the chosen shape and size." },
			{ "solvate", "Fills the simulation box with water molecules around the solute." },
			{ "trjconv",
			  "Post-processes the coordinates (periodic boundary handling) to prepare the system for ion addition." },
			{ "grompp — ions",
			  "Compiles the topology and coordinates into the binary .tpr file required to add ions." },
			{ "genion", "Adds ions to neutralise the system and reach the requested salt concentration." },
			{ "grompp — position restraints",
			  "Compiles the system with position restraints, used to keep the solute in place during equilibration." },
			{ "grompp — minimization", "Compiles the system for the energy-minimization run." },
			{ "editconf — export", "Exports the final prepared structure as a PDB file." },
			{ "Pack system",
			  "Gathers every file needed to run the MD simulation (topology, coordinates, .mdp run parameters and "
			  "workflow script) into the output directory." },
		} };

		// A widget row that reports clicks through a callback, avoiding a dedicated moc-compiled subclass.
		class ClickableRow : public QWidget
		{
		  public:
			using QWidget::QWidget;
			std::function<void()> onClick;

		  protected:
			void mousePressEvent( QMouseEvent * p_event ) override
			{
				if ( p_event->button() == Qt::LeftButton && onClick )
				{
					onClick();
				}
				QWidget::mousePressEvent( p_event );
			}
		};

	} // namespace

	// One node of the timeline: a state-colored card with a clickable header and a collapsible details area.
	class StepNode : public QFrame
	{
	  public:
		StepNode( const QString & p_title, const QString & p_why, QWidget * p_parent = nullptr ) : QFrame( p_parent )
		{
			setObjectName( "mdprepStepNode" );

			QVBoxLayout * outer = new QVBoxLayout( this );
			outer->setContentsMargins( 8, 6, 8, 6 );
			outer->setSpacing( 6 );

			// Header.
			_header					= new ClickableRow;
			QHBoxLayout * headerLay = new QHBoxLayout( _header );
			headerLay->setContentsMargins( 0, 0, 0, 0 );
			headerLay->setSpacing( 8 );

			_dot = new QLabel;
			_dot->setFixedSize( 12, 12 );

			_titleLabel = new QLabel( p_title );
			{
				QFont f = _titleLabel->font();
				f.setBold( true );
				_titleLabel->setFont( f );
			}

			_stateLabel = new QLabel;
			_arrow		= new QLabel( QStringLiteral( "▸" ) ); // right-pointing triangle

			headerLay->addWidget( _dot );
			headerLay->addWidget( _titleLabel );
			headerLay->addStretch( 1 );
			headerLay->addWidget( _stateLabel );
			headerLay->addWidget( _arrow );
			_header->setCursor( Qt::PointingHandCursor );
			_header->onClick = [ this ] { setExpanded( not _expanded ); };
			outer->addWidget( _header );

			// Details.
			_details			 = new QWidget;
			QVBoxLayout * detLay = new QVBoxLayout( _details );
			detLay->setContentsMargins( 20, 0, 0, 0 );
			detLay->setSpacing( 6 );

			_whyLabel = new QLabel( p_why );
			_whyLabel->setWordWrap( true );
			_whyLabel->setStyleSheet( "color:#9aa0a6;" );
			detLay->addWidget( _whyLabel );

			_errorsLabel = new QLabel;
			_errorsLabel->setWordWrap( true );
			_errorsLabel->setStyleSheet( "color:#ff6b6b;" );
			_errorsLabel->hide();
			detLay->addWidget( _errorsLabel );

			_stdoutHeading = new QLabel( QStringLiteral( "Standard output" ) );
			_stdoutHeading->setStyleSheet( "color:#8a9099; font-size:11px;" );
			_stdoutHeading->hide();
			detLay->addWidget( _stdoutHeading );
			_stdout = _makeOutputBox();
			detLay->addWidget( _stdout );

			_stderrHeading = new QLabel( QStringLiteral( "Standard error" ) );
			_stderrHeading->setStyleSheet( "color:#8a9099; font-size:11px;" );
			_stderrHeading->hide();
			detLay->addWidget( _stderrHeading );
			_stderr = _makeOutputBox();
			detLay->addWidget( _stderr );

			_details->hide();
			outer->addWidget( _details );

			_applyState();
		}

		void setState( E_STEP_STATE p_state ) noexcept
		{
			_state = p_state;
			_applyState();
		}

		void setOutputs( const QString & p_stdOut, const QString & p_stdErr, const QStringList & p_errors ) noexcept
		{
			if ( not p_errors.isEmpty() )
			{
				_errorsLabel->setText( p_errors.join( QStringLiteral( "\n" ) ) );
				_errorsLabel->show();
			}
			if ( not p_stdOut.isEmpty() )
			{
				_stdout->setPlainText( p_stdOut );
				_stdoutHeading->show();
				_stdout->show();
			}
			if ( not p_stdErr.isEmpty() )
			{
				_stderr->setPlainText( p_stdErr );
				_stderrHeading->show();
				_stderr->show();
			}
		}

		void setExpanded( bool p_expanded ) noexcept
		{
			_expanded = p_expanded;
			_details->setVisible( _expanded );
			_arrow->setText( _expanded ? QStringLiteral( "▾" ) : QStringLiteral( "▸" ) );
		}

		void resetNode() noexcept
		{
			_state = E_STEP_STATE::pending;
			_errorsLabel->clear();
			_errorsLabel->hide();
			_stdout->clear();
			_stdout->hide();
			_stdoutHeading->hide();
			_stderr->clear();
			_stderr->hide();
			_stderrHeading->hide();
			setExpanded( false );
			_applyState();
		}

	  private:
		E_STEP_STATE   _state	 = E_STEP_STATE::pending;
		bool		   _expanded = false;
		ClickableRow * _header	 = nullptr;
		QLabel *	   _dot		 = nullptr;
		QLabel *	   _titleLabel = nullptr;
		QLabel *	   _stateLabel = nullptr;
		QLabel *	   _arrow	 = nullptr;
		QWidget *	   _details	 = nullptr;
		QLabel *	   _whyLabel = nullptr;
		QLabel *	   _errorsLabel	   = nullptr;
		QLabel *	   _stdoutHeading  = nullptr;
		QLabel *	   _stderrHeading  = nullptr;
		QPlainTextEdit * _stdout = nullptr;
		QPlainTextEdit * _stderr = nullptr;

		static QPlainTextEdit * _makeOutputBox() noexcept
		{
			QPlainTextEdit * box = new QPlainTextEdit;
			box->setReadOnly( true );
			box->setMaximumHeight( 160 );
			box->setLineWrapMode( QPlainTextEdit::NoWrap );
			QFont f( QStringLiteral( "monospace" ) );
			f.setStyleHint( QFont::Monospace );
			box->setFont( f );
			box->hide();
			return box;
		}

		void _applyState() noexcept
		{
			const StateStyle style = stateStyle( _state );
			_dot->setStyleSheet(
				QStringLiteral( "background-color:%1; border-radius:6px;" ).arg( QString( style.color ) )
			);
			_stateLabel->setText( QString( style.label ) );
			_stateLabel->setStyleSheet( QStringLiteral( "color:%1;" ).arg( QString( style.color ) ) );
			setStyleSheet(
				QStringLiteral(
					"QFrame#mdprepStepNode { border-left:3px solid %1; border-radius:4px; "
					"background-color:rgba(127,127,127,0.06); }"
				)
					.arg( QString( style.color ) )
			);
		}
	};

	PreparationTimeline::PreparationTimeline( QWidget * p_parent ) :
		BaseDockWidget( p_parent, "MD Preparation – Timeline" )
	{
		setWindowIcon( QIcon( ":/sprite/icon_tool_mdprep_mainButton.png" ) );
		_setupUi();

		_stepStartedConnection
			= App::HUB().connect<Actions::PreparationStepStarted, &PreparationTimeline::_onStepStarted>( this );
		_stepFinishedConnection
			= App::HUB().connect<Actions::PreparationStepFinished, &PreparationTimeline::_onStepFinished>( this );
		_systemPackedConnection
			= App::HUB().connect<Actions::SystemPacked, &PreparationTimeline::_onSystemPacked>( this );
	}

	void PreparationTimeline::_setupUi() noexcept
	{
		QLabel * intro = new QLabel(
			"Progress of the Molecular Dynamics preparation. Click a step to read what it does and, once finished, its "
			"GROMACS output."
		);
		intro->setWordWrap( true );
		intro->setContentsMargins( 10, 10, 10, 4 );
		_layout->addWidget( intro );

		for ( size_t i = 0; i < g_steps.size(); i++ )
		{
			StepNode * node = new StepNode( g_steps[ i ].title, g_steps[ i ].why );
			_layout->addWidget( node );
			if ( i + 1 == g_steps.size() )
			{
				_packNode = node;
			}
			else
			{
				_nodes.push_back( node );
			}
		}

		_layout->addStretch( 1 );

		_openFolderBtn = new QPushButton( QStringLiteral( "Open system folder" ) );
		_openFolderBtn->setEnabled( false );
		_openFolderBtn->setToolTip( QStringLiteral( "Available once the system has been packed successfully." ) );
		{
			QFont f = _openFolderBtn->font();
			f.setPointSize( f.pointSize() + 1 );
			_openFolderBtn->setFont( f );
		}
		_openFolderBtn->setContentsMargins( 10, 10, 10, 10 );
		QObject::connect(
			_openFolderBtn,
			&QPushButton::clicked,
			[ this ]
			{
				if ( _packedPath.empty() )
				{
					return;
				}
				QDesktopServices::openUrl(
					QUrl::fromLocalFile( QDir( QString::fromStdString( _packedPath ) ).absolutePath() )
				);
			}
		);
		_layout->addWidget( _openFolderBtn );
	}

	void PreparationTimeline::reset() noexcept
	{
		for ( StepNode * node : _nodes )
		{
			node->resetNode();
		}
		if ( _packNode )
		{
			_packNode->resetNode();
		}
		_packedPath.clear();
		if ( _openFolderBtn )
		{
			_openFolderBtn->setEnabled( false );
		}
	}

	void PreparationTimeline::_onStepStarted( const Actions::PreparationStepStarted & p_ev ) noexcept
	{
		if ( p_ev.index < 0 || static_cast<size_t>( p_ev.index ) >= _nodes.size() )
		{
			return;
		}
		_nodes[ p_ev.index ]->setState( E_STEP_STATE::running );
	}

	void PreparationTimeline::_onStepFinished( const Actions::PreparationStepFinished & p_ev ) noexcept
	{
		if ( p_ev.index < 0 || static_cast<size_t>( p_ev.index ) >= _nodes.size() )
		{
			return;
		}
		StepNode * node = _nodes[ p_ev.index ];

		QStringList errors;
		for ( const std::string & e : p_ev.errors )
		{
			errors << QString::fromStdString( e );
		}
		node->setOutputs(
			QString::fromStdString( p_ev.stdOut ), QString::fromStdString( p_ev.stdErr ), errors
		);
		node->setState( p_ev.success ? E_STEP_STATE::done : E_STEP_STATE::failed );
		if ( not p_ev.success )
		{
			node->setExpanded( true );
		}
	}

	void PreparationTimeline::_onSystemPacked( const Actions::SystemPacked & p_ev ) noexcept
	{
		if ( _packNode )
		{
			_packNode->setState( p_ev.success ? E_STEP_STATE::done : E_STEP_STATE::failed );
		}
		if ( p_ev.success )
		{
			_packedPath = p_ev.path;
			if ( _openFolderBtn )
			{
				_openFolderBtn->setEnabled( true );
				_openFolderBtn->setToolTip( QString::fromStdString( _packedPath ) );
			}
		}
	}

	namespace
	{
		PreparationTimeline * g_timeline = nullptr;
	}

	void showPreparationTimeline() noexcept
	{
		if ( g_timeline == nullptr )
		{
			g_timeline = UI::QT::MAIN_WINDOW().createDockWidget<PreparationTimeline>( Qt::RightDockWidgetArea );
		}

		g_timeline->reset();
		if ( g_timeline->isHidden() )
		{
			g_timeline->resize( PreparationTimeline::PREFERRED_SIZE );
			g_timeline->show();
		}
		g_timeline->raise();
	}

} // namespace VTX::Tool::Mdprep::ui
