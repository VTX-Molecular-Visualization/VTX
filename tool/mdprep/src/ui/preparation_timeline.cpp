#include <array>
#include <functional>
#include <ui/qt/helper.hpp>
//
#include <QColor>
#include <QDesktopServices>
#include <QDir>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QPalette>
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
#include <tool/mdprep/gateway/shared.hpp>
//
#include <tool/mdprep/ui/preparation_timeline.hpp>

namespace VTX::Tool::Mdprep::ui
{
	namespace
	{
		enum class E_STEP_STATE : uint8_t
		{
			pending,
			running,
			done,
			failed
		};

		// A step state maps to a fixed, semantic status color (not a theme palette role) and a label. The color is
		// used both for the rounded dot/card accent (through QSS, the only way to get rounded geometry) and for the
		// state label text (through the palette).
		struct StateStyle
		{
			QColor	color;
			QString label;
		};

		StateStyle stateStyle( E_STEP_STATE p_state ) noexcept
		{
			switch ( p_state )
			{
			case E_STEP_STATE::running: return { QColor( 0x4c, 0x9a, 0xff ), QStringLiteral( "Running…" ) };
			case E_STEP_STATE::done: return { QColor( 0x57, 0xd3, 0x8c ), QStringLiteral( "Done" ) };
			case E_STEP_STATE::failed: return { QColor( 0xff, 0x6b, 0x6b ), QStringLiteral( "Failed" ) };
			case E_STEP_STATE::pending:
			default: return { QColor( 0x8a, 0x90, 0x99 ), QStringLiteral( "Pending" ) };
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

		constexpr auto g_steps = std::to_array<StepInfo>( {
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
		} );

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
		StepNode( const QString & p_title, const QString & p_why, QWidget * const p_parent = nullptr ) :
			QFrame( p_parent ), _header( VTX::UI::QT::Helper::makeQObject<ClickableRow>( this ) ),
			_dot( VTX::UI::QT::Helper::makeQObject<QLabel>( _header ) ),
			_titleLabel( VTX::UI::QT::Helper::makeQObject<QLabel>( p_title, _header ) ),
			_stateLabel( VTX::UI::QT::Helper::makeQObject<QLabel>( _header ) ),
			_arrow(
				VTX::UI::QT::Helper::makeQObject<QLabel>( QStringLiteral( "▸" ), _header )
			), // right-pointing triangle
			_details( VTX::UI::QT::Helper::makeQObject<QWidget>( this ) ),
			_whyLabel( VTX::UI::QT::Helper::makeQObject<QLabel>( p_why, _details ) ),
			_errorsLabel( VTX::UI::QT::Helper::makeQObject<QLabel>( _details ) ),
			_stdoutHeading( _makeHeading( QStringLiteral( "Standard output" ) ) ),
			_stderrHeading( _makeHeading( QStringLiteral( "Standard error" ) ) ), _stdout( _makeOutputBox() ),
			_stderr( _makeOutputBox() )
		{
			setObjectName( "mdprepStepNode" );

			QVBoxLayout * outer = VTX::UI::QT::Helper::makeQObject<QVBoxLayout>( this );
			outer->setContentsMargins( 8, 6, 8, 6 );
			outer->setSpacing( 6 );

			// Header.
			QHBoxLayout * headerLay = VTX::UI::QT::Helper::makeQObject<QHBoxLayout>( _header );
			headerLay->setContentsMargins( 0, 0, 0, 0 );
			headerLay->setSpacing( 8 );

			_dot->setFixedSize( 12, 12 );

			{
				QFont f = _titleLabel->font();
				f.setBold( true );
				_titleLabel->setFont( f );
			}

			headerLay->addWidget( _dot );
			headerLay->addWidget( _titleLabel );
			headerLay->addStretch( 1 );
			headerLay->addWidget( _stateLabel );
			headerLay->addWidget( _arrow );
			_header->setCursor( Qt::PointingHandCursor );
			_header->onClick = [ this ] { setExpanded( not _expanded ); };
			outer->addWidget( _header );

			// Details.
			QVBoxLayout * detLay = VTX::UI::QT::Helper::makeQObject<QVBoxLayout>( _details );
			detLay->setContentsMargins( 20, 0, 0, 0 );
			detLay->setSpacing( 6 );

			_whyLabel->setWordWrap( true );
			_whyLabel->setForegroundRole( QPalette::PlaceholderText ); // theme-aware muted text
			detLay->addWidget( _whyLabel );

			_errorsLabel->setWordWrap( true );
			{
				QPalette pal = _errorsLabel->palette();
				pal.setColor( _errorsLabel->foregroundRole(), stateStyle( E_STEP_STATE::failed ).color );
				_errorsLabel->setPalette( pal );
			}
			_errorsLabel->hide();
			detLay->addWidget( _errorsLabel );

			detLay->addWidget( _stdoutHeading );
			detLay->addWidget( _stdout );

			detLay->addWidget( _stderrHeading );
			detLay->addWidget( _stderr );

			_details->hide();
			outer->addWidget( _details );

			_applyState();
		}

		void setState( E_STEP_STATE p_state )
		{
			_state = p_state;
			_applyState();
		}

		void setOutputs( const QString & p_stdOut, const QString & p_stdErr, const QStringList & p_errors )
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

		void setExpanded( bool p_expanded )
		{
			_expanded = p_expanded;
			_details->setVisible( _expanded );
			_arrow->setText( _expanded ? QStringLiteral( "▾" ) : QStringLiteral( "▸" ) );
		}

		void resetNode()
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
		E_STEP_STATE	 _state			= E_STEP_STATE::pending;
		bool			 _expanded		= false;
		ClickableRow *	 _header		= nullptr;
		QLabel *		 _dot			= nullptr;
		QLabel *		 _titleLabel	= nullptr;
		QLabel *		 _stateLabel	= nullptr;
		QLabel *		 _arrow			= nullptr;
		QWidget *		 _details		= nullptr;
		QLabel *		 _whyLabel		= nullptr;
		QLabel *		 _errorsLabel	= nullptr;
		QLabel *		 _stdoutHeading = nullptr;
		QLabel *		 _stderrHeading = nullptr;
		QPlainTextEdit * _stdout		= nullptr;
		QPlainTextEdit * _stderr		= nullptr;

		// A dimmed, slightly smaller section heading. Color comes from the theme (palette role) and the size from the
		// font API, so nothing here needs a stylesheet.
		QLabel * _makeHeading( const QString & p_text )
		{
			QLabel * label = VTX::UI::QT::Helper::makeQObject<QLabel>( p_text, _details );
			label->setForegroundRole( QPalette::PlaceholderText );
			QFont	  f	 = label->font();
			const int pt = f.pointSize();
			if ( pt > 1 )
			{
				f.setPointSize( pt - 1 );
			}
			label->setFont( f );
			label->hide();
			return label;
		}

		QPlainTextEdit * _makeOutputBox()
		{
			QPlainTextEdit * box = VTX::UI::QT::Helper::makeQObject<QPlainTextEdit>( _details );
			box->setReadOnly( true );
			box->setMaximumHeight( 160 );
			box->setLineWrapMode( QPlainTextEdit::NoWrap );
			QFont f( QStringLiteral( "monospace" ) );
			f.setStyleHint( QFont::Monospace );
			box->setFont( f );
			box->hide();
			return box;
		}

		void _applyState()
		{
			const StateStyle style = stateStyle( _state );

			// QSS is kept strictly for the rounded geometry that the widget API cannot express: the dot circle and the
			// card corners / left accent. The injected color is a fixed semantic status color, not a theme role.
			_dot->setStyleSheet(
				QStringLiteral( "background-color:%1; border-radius:6px;" ).arg( style.color.name() )
			);
			setStyleSheet( QStringLiteral( "QFrame#mdprepStepNode { border-left:3px solid %1; border-radius:4px; }" )
							   .arg( style.color.name() ) );

			// State label text color goes through the palette, like the rest of VTX.
			_stateLabel->setText( style.label );
			QPalette pal = _stateLabel->palette();
			pal.setColor( _stateLabel->foregroundRole(), style.color );
			_stateLabel->setPalette( pal );
		}
	};

	PreparationTimeline::PreparationTimeline( QWidget * const p_parent ) :
		BaseDockWidget( p_parent, "MD Preparation – Timeline" )
	{
		setWindowIcon( QIcon( ":/sprite/icon_tool_mdprep_mainButton.png" ) );
		_setupUi();

		_stepStartedConnection
			= App::HUB().connect<Gateway::PreparationStepStarted, &PreparationTimeline::_onStepStarted>( this );
		_stepFinishedConnection
			= App::HUB().connect<Gateway::PreparationStepFinished, &PreparationTimeline::_onStepFinished>( this );
		_systemPackedConnection
			= App::HUB().connect<Gateway::SystemPacked, &PreparationTimeline::_onSystemPacked>( this );
	}

	void PreparationTimeline::_setupUi()
	{
		QLabel * intro = VTX::UI::QT::Helper::makeQObject<QLabel>(
			"Progress of the Molecular Dynamics preparation. Click a step to read what it does and, once finished, its "
			"GROMACS output.",
			_layout->parentWidget()
		);
		intro->setWordWrap( true );
		intro->setContentsMargins( 10, 10, 10, 4 );
		_layout->addWidget( intro );

		for ( size_t i = 0; i < g_steps.size(); i++ )
		{
			StepNode * node = VTX::UI::QT::Helper::makeQObject<StepNode>(
				g_steps[ i ].title, g_steps[ i ].why, _layout->parentWidget()
			);
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

		_openFolderBtn = VTX::UI::QT::Helper::makeQObject<QPushButton>(
			QStringLiteral( "Open system folder" ), _layout->parentWidget()
		);
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

	void PreparationTimeline::reset()
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

	void PreparationTimeline::_onStepStarted( const Gateway::PreparationStepStarted & p_ev )
	{
		if ( p_ev.index < 0 || static_cast<size_t>( p_ev.index ) >= _nodes.size() )
		{
			return;
		}
		_nodes[ p_ev.index ]->setState( E_STEP_STATE::running );
	}

	void PreparationTimeline::_onStepFinished( const Gateway::PreparationStepFinished & p_ev )
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
		node->setOutputs( QString::fromStdString( p_ev.stdOut ), QString::fromStdString( p_ev.stdErr ), errors );
		node->setState( p_ev.success ? E_STEP_STATE::done : E_STEP_STATE::failed );
		if ( not p_ev.success )
		{
			node->setExpanded( true );
		}
	}

	void PreparationTimeline::_onSystemPacked( const Gateway::SystemPacked & p_ev )
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

	void showPreparationTimeline()
	{
		static PreparationTimeline * timeline = nullptr;

		if ( timeline == nullptr )
		{
			timeline = UI::QT::MAIN_WINDOW().createDockWidget<PreparationTimeline>( Qt::RightDockWidgetArea );
		}

		timeline->reset();
		if ( timeline->isHidden() )
		{
			timeline->resize( PreparationTimeline::PREFERRED_SIZE );
			timeline->show();
		}
		timeline->raise();
	}

} // namespace VTX::Tool::Mdprep::ui
