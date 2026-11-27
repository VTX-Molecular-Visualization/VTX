#include "ui/qt/dock_widget/inspector.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/camera.hpp"
#include "ui/qt/widget/controller.hpp"
#include "ui/qt/widget/library/read_only_notice.hpp"
#include "ui/qt/widget/selection.hpp"
#include "ui/qt/widget/tree/scripts.hpp"
#include <QDesktopServices>
#include <QFontDatabase>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <algorithm>
#include <app/events.hpp>
#include <app/helper/script.hpp>
#include <app/services.hpp>
#include <app/system/selection.hpp>
#include <renderer/camera.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{
	Inspector::Inspector( QWidget * const p_parent ) : BaseDockWidget( p_parent, "Selection" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::SELECTION ) );

		_lockButton = Helper::makeActionButton( *UI_ACTIONS().getAction( Action::Selection::LOCK ), this );
		_layout->addWidget( _lockButton );

		_filler = Helper::makeQObject<QWidget>( this );
		_filler->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
		_layout->addWidget( _filler );

		connect(
			&SELECTION(),
			&SelectionManager::selectionChanged,
			this,
			[ this ]( const E_SELECTION_GROUP p_group, const QItemSelection & p_selection )
			{
				if ( p_selection.isEmpty() )
				{
					if ( _selectionListWidget == nullptr )
					{
						_clear();
					}
					return;
				}

				const QModelIndex index
					= p_selection.indexes().isEmpty() ? QModelIndex() : p_selection.indexes().first();
				const QVariant data		 = index.data( Qt::UserRole );
				const Entity   preset	 = data.value<Entity>();
				const bool	   hasPreset = data.isValid();

				const QVariant scopeData
					= ( hasPreset ? index.parent() : index ).data( Widget::Library::PRESET_SCOPE_ROLE );
				const Widget::Library::E_PRESET_SCOPE scope = scopeData.isValid()
																  ? Widget::Library::E_PRESET_SCOPE( scopeData.toInt() )
																  : Widget::Library::E_PRESET_SCOPE::ALL;

				// Insert widget.
				switch ( p_group )
				{
				case E_SELECTION_GROUP::CAMERA:
				{
					_clear();
					if ( hasPreset )
					{
						_viewPointWidget = Helper::makeQObject<Widget::ViewPoint>( preset, this );
						_layout->insertWidget( _layout->indexOf( _filler ), _viewPointWidget );
					}
					else
					{
						const auto [ ent, _ ]	= App::ECS::getFirstEntityWithComponents<Renderer::Camera>();
						auto * cameraWidget		= Helper::makeQObject<Widget::Camera>( ent, this );
						auto * controllerWidget = Helper::makeQObject<Widget::Controller>( ent, this );
						_layout->insertWidget( _layout->indexOf( _filler ), cameraWidget );
						_layout->insertWidget( _layout->indexOf( _filler ), controllerWidget );
					}
					break;
				}
				case E_SELECTION_GROUP::GRAPHICS_CONFIG:
				{
					if ( _graphicsConfigWidget )
					{
						_graphicsConfigWidget->setScope( scope );
						if ( hasPreset )
						{
							_graphicsConfigWidget->setCurrentPreset( preset );
						}
						else
						{
							_graphicsConfigWidget->refresh();
						}
						break;
					}

					_clear();
					_graphicsConfigWidget = Helper::makeQObject<Widget::Library::GraphicsConfigWidget>( this );
					_graphicsConfigWidget->setScope( scope );
					if ( hasPreset )
					{
						_graphicsConfigWidget->setCurrentPreset( preset );
					}
					else
					{
						_graphicsConfigWidget->refresh();
					}
					_layout->insertWidget( _layout->indexOf( _filler ), _graphicsConfigWidget );
					break;
				}
				case E_SELECTION_GROUP::COLOR_LAYOUT:
				{
					if ( _colorLayoutWidget )
					{
						_colorLayoutWidget->setScope( scope );
						if ( hasPreset )
						{
							_colorLayoutWidget->setCurrentPreset( preset );
						}
						else
						{
							_colorLayoutWidget->refresh();
						}
						break;
					}

					_clear();
					_colorLayoutWidget = Helper::makeQObject<Widget::Library::ColorLayout>( this );
					_colorLayoutWidget->setScope( scope );
					if ( hasPreset )
					{
						_colorLayoutWidget->setCurrentPreset( preset );
					}
					else
					{
						_colorLayoutWidget->refresh();
					}
					_layout->insertWidget( _layout->indexOf( _filler ), _colorLayoutWidget );
					break;
				}
				case E_SELECTION_GROUP::REPRESENTATION:
				{
					if ( _representationWidget )
					{
						_representationWidget->setScope( scope );
						if ( hasPreset )
						{
							_representationWidget->setCurrentPreset( preset );
						}
						else
						{
							_representationWidget->refresh();
						}
						break;
					}

					_clear();
					_representationWidget = Helper::makeQObject<Widget::Library::Representation>( this );
					_representationWidget->setScope( scope );
					if ( hasPreset )
					{
						_representationWidget->setCurrentPreset( preset );
					}
					else
					{
						_representationWidget->refresh();
					}
					_layout->insertWidget( _layout->indexOf( _filler ), _representationWidget );
					break;
				}
				case E_SELECTION_GROUP::SCRIPT:
				{
					_clear();
					QPushButton * const runButton
						= Helper::makeActionButton( *UI_ACTIONS().getAction( Action::Script::RUN ), this );

					const FilePath path = index.data( Widget::Tree::Scripts::PATH_ROLE ).toString().toStdString();
					if ( App::Helper::Script::isReadOnly( path ) )
					{
						auto * const notice = Helper::makeQObject<Widget::Library::ReadOnlyNotice>(
							this,
							"Built-in scripts are read-only\nDuplicate to edit",
							"Copy this script into your scripts"
						);
						notice->setVisible( true );
						_layout->insertWidget( _layout->indexOf( _filler ), notice );
						connect(
							notice->button(),
							&QPushButton::clicked,
							this,
							[ path ]() { Helper::duplicateScript( path ); }
						);
					}
					else
					{
						QPushButton * const saveButton
							= Helper::makeActionButton( *UI_ACTIONS().getAction( Action::Script::SAVE ), this );
						_layout->insertWidget( _layout->indexOf( _filler ), saveButton );

						auto * const openFolderButton = Helper::makeQObject<QPushButton>( this );
						openFolderButton->setText( "Open folder" );
						openFolderButton->setIcon( STYLE().iconFromCodepoint( Style::Icons::OPEN ) );
						openFolderButton->setToolTip( "Open the folder containing this script" );
						_layout->insertWidget( _layout->indexOf( _filler ), openFolderButton );
						connect(
							openFolderButton,
							&QPushButton::clicked,
							this,
							[ path ]()
							{
								QDesktopServices::openUrl(
									QUrl::fromLocalFile( QString::fromStdString( path.parent_path().string() ) )
								);
							}
						);

						auto * const copyButton = Helper::makeQObject<QPushButton>( this );
						copyButton->setText( "Copy" );
						copyButton->setIcon( STYLE().iconFromCodepoint( Style::Icons::COPY ) );
						copyButton->setToolTip( "Duplicate this script next to it" );
						_layout->insertWidget( _layout->indexOf( _filler ), copyButton );
						connect(
							copyButton, &QPushButton::clicked, this, [ path ]() { Helper::duplicateScript( path ); }
						);
					}

					_layout->insertWidget( _layout->indexOf( _filler ), runButton );
					break;
				}
				default: break;
				}
			}
		);

		_selectionUpdatedConnection
			= App::REG().on_update<App::System::Selection>().connect<&Inspector::_onSystemSelectionUpdated>( this );
		_systemDestroyedConnection
			= App::HUB().connect<App::Events::SystemDestroyed, &Inspector::_onSystemDestroyed>( this );
	}

	void Inspector::_clear()
	{
		for ( int i = _layout->count() - 1; i >= 0; --i )
		{
			if ( _layout->itemAt( i )->widget() != _filler && _layout->itemAt( i )->widget() != _lockButton )
			{
				QWidget * w = _layout->itemAt( i )->widget();
				if ( w == _selectionListWidget )
				{
					_selectionListWidget = nullptr;
				}
				if ( w == _graphicsConfigWidget )
				{
					_graphicsConfigWidget = nullptr;
				}
				if ( w == _colorLayoutWidget )
				{
					_colorLayoutWidget = nullptr;
				}
				if ( w == _representationWidget )
				{
					_representationWidget = nullptr;
				}
				if ( w == _viewPointWidget )
				{
					_viewPointWidget = nullptr;
				}
				_layout->removeWidget( w );
				w->hide();
				w->deleteLater();
			}
		}
	}

	bool Inspector::_hasSystemSelection() const
	{
		return std::ranges::any_of(
			App::REG().view<App::System::Selection>(),
			[]( const Entity entity ) { return App::REG().get<App::System::Selection>( entity ).atoms.any(); }
		);
	}

	void Inspector::_onSystemSelectionUpdated( Registry &, Entity )
	{
		const Util::ScopedChrono timer( "Inspector::_onSystemSelectionUpdated" );

		if ( not _hasSystemSelection() )
		{
			if ( _selectionListWidget )
			{
				_clear();
			}
			return;
		}

		// Already displaying system selection.
		// if ( _selectionListWidget )
		//{
		//	_selectionListWidget->refresh();
		//	return;
		//}

		_clear();
		_selectionListWidget = Helper::makeQObject<Widget::Selection>( this );
		_layout->insertWidget( _layout->indexOf( _filler ), _selectionListWidget );
	}

	void Inspector::_onSystemDestroyed( const App::Events::SystemDestroyed & p_event )
	{ _onSystemSelectionUpdated( App::REG(), p_event.system ); }

} // namespace VTX::UI::QT::DockWidget
