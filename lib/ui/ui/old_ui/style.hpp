#ifndef __VTX_UI_STYLE__
#define __VTX_UI_STYLE__

#include "old_ui/util/ui.hpp"
#include <QBitmap>
#include <QChar>
#include <QColor>
#include <QFont>
#include <QPalette>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <app/old_app/id.hpp>
#include <app/old_app/model/category_enum.hpp>
#include <app/old_app/model/representation/representation_enum.hpp>
#include <app/old_app/selection/selection_enum.hpp>
#include <app/old_app/util/label.hpp>
#include <iostream>
#include <string>
#include <util/logger.hpp>
#include <vector>

namespace VTX::UI
{
	namespace Style
	{
		inline static const std::vector<std::string> SYMBOL_DISPLAY_MODE_STRING { "Short", "Long" };

		class IconConst
		{
		  public:
			inline static IconConst & get()
			{
				static IconConst instance;
				return instance;
			};

			const QPixmap UNFOLDED_PIXMAP;
			const QPixmap FOLDED_PIXMAP;
			const QPixmap CLOSE_PIXMAP;

			const QPixmap REMOVE_LINE_ICON;
			const QPixmap DELETE_LINE_ICON;

			const QPixmap FULLSCREEN_ICON;
			const QPixmap WINDOWED_ICON;

			const QPixmap SCENE_WINDOW_ICON;
			const QPixmap INSPECTOR_WINDOW_ICON;
			const QPixmap RENDER_WINDOW_ICON;
			const QPixmap SETTING_WINDOW_ICON;
			const QPixmap SELECTION_WINDOW_ICON;
			const QPixmap CONSOLE_WINDOW_ICON;
			const QPixmap SEQUENCE_WINDOW_ICON;

			const QPixmap OPEN_FOLDER_ICON;

			const QPixmap DEFAULT_ITEM;

			const QPixmap MOLECULE_SYMBOL;
			const QPixmap CHAIN_SYMBOL;
			const QPixmap RESIDUE_SYMBOL;
			const QPixmap ATOM_SYMBOL;
			const QPixmap REPRESENTATION_SYMBOL;
			const QPixmap VIEWPOINT_SYMBOL;
			const QPixmap LABEL_SYMBOL;

			const QPixmap REPRESENTATION_STICK_ICON;
			const QPixmap REPRESENTATION_BALL_AND_STICK_ICON;
			const QPixmap REPRESENTATION_VDW_ICON;
			const QPixmap REPRESENTATION_CARTOON_ICON;
			const QPixmap REPRESENTATION_STICK_AND_CARTOON_ICON;
			const QPixmap REPRESENTATION_BALL_STICK_AND_CARTOON_ICON;
			const QPixmap REPRESENTATION_TRACE_ICON;
			const QPixmap REPRESENTATION_SAS_ICON;
			const QPixmap REPRESENTATION_SES_ICON;

			const QPixmap RESIDUE_CATEGORY_ICON;

			const QBitmap DIHEDRAL_ANGLE_RENDER_ICON_MASK;

			const QPixmap TOOLTAB_SEPARATOR;

			const QPixmap * const getWindowIcon( const ID::VTX_ID & p_id ) const
			{
				const QPixmap * res;

				if ( p_id == ID::UI::Window::SCENE )
					res = &SCENE_WINDOW_ICON;
				else if ( p_id == ID::UI::Window::RENDER )
					res = &RENDER_WINDOW_ICON;
				else if ( p_id == ID::UI::Window::INSPECTOR )
					res = &INSPECTOR_WINDOW_ICON;
				else if ( p_id == ID::UI::Window::CONSOLE )
					res = &CONSOLE_WINDOW_ICON;
				else if ( p_id == ID::UI::Window::SETTINGS )
					res = &SETTING_WINDOW_ICON;
				// !V0.1
				// else if ( p_id == ID::UI::Window::SELECTION )
				//	res = &SELECTION_WINDOW_ICON;
				else if ( p_id == ID::UI::Window::SEQUENCE )
					res = &SEQUENCE_WINDOW_ICON;
				else
				{
					VTX_WARNING( "Symbol for model " + p_id + " not managed in IconConst::getModelSymbol." );
					res = nullptr;
				}

				return res;
			}

			const QPixmap * const getModelSymbol( const ID::VTX_ID & p_id ) const
			{
				const QPixmap * res;

				if ( p_id == VTX::ID::Model::MODEL_MOLECULE )
					res = &MOLECULE_SYMBOL;
				else if ( p_id == VTX::ID::Model::MODEL_CHAIN )
					res = &CHAIN_SYMBOL;
				else if ( p_id == VTX::ID::Model::MODEL_RESIDUE )
					res = &RESIDUE_SYMBOL;
				else if ( p_id == VTX::ID::Model::MODEL_ATOM )
					res = &ATOM_SYMBOL;
				else if ( p_id == VTX::ID::Model::MODEL_PATH )
					res = &VIEWPOINT_SYMBOL;
				else if ( p_id == VTX::ID::Model::MODEL_VIEWPOINT )
					res = &VIEWPOINT_SYMBOL;
				else if ( Util::Label::isLabelType( p_id ) )
				{
					res = &LABEL_SYMBOL;
				}
				else
				{
					VTX_WARNING( "Symbol for model " + p_id + " not managed in IconConst::getModelSymbol." );
					res = nullptr;
				}

				return res;
			};

			const QPixmap * const getRepresentationIcon( const Generic::REPRESENTATION & p_representation ) const
			{
				const QPixmap * res;

				switch ( p_representation )
				{
				case Generic::REPRESENTATION::BALL_AND_STICK: res = &REPRESENTATION_BALL_AND_STICK_ICON; break;
				case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON:
					res = &REPRESENTATION_BALL_STICK_AND_CARTOON_ICON;
					break;
				case Generic::REPRESENTATION::CARTOON: res = &REPRESENTATION_CARTOON_ICON; break;
				case Generic::REPRESENTATION::SAS: res = &REPRESENTATION_SAS_ICON; break;
				case Generic::REPRESENTATION::STICK: res = &REPRESENTATION_STICK_ICON; break;
				case Generic::REPRESENTATION::STICK_AND_CARTOON: res = &REPRESENTATION_STICK_AND_CARTOON_ICON; break;
				case Generic::REPRESENTATION::SES:
					res = &REPRESENTATION_SES_ICON;
					break;

					// !V0.1
					// case Generic::REPRESENTATION::TRACE: res = &REPRESENTATION_TRACE_ICON; break;
				case Generic::REPRESENTATION::VAN_DER_WAALS: res = &REPRESENTATION_VDW_ICON; break;
				default:

					VTX_WARNING( "Representation " + std::to_string( int( p_representation ) )
								 + " not managed in IconConst::getRepresentationIcon." );
					res = &REPRESENTATION_STICK_ICON;
					break;
				}

				return res;
			}

			const QPixmap * const getResidueCategorySymbol( const CATEGORY_ENUM & p_category ) const
			{
				return &RESIDUE_CATEGORY_ICON;
			}

		  private:
			IconConst() :
				UNFOLDED_PIXMAP( QPixmap( ":/sprite/treeview_unfolded_molecule_icon.png" ) ),
				FOLDED_PIXMAP( QPixmap( ":/sprite/treeview_folded_molecule_icon.png" ) ),
				CLOSE_PIXMAP( QPixmap( ":/sprite/close_icon.png" ) ),

				REMOVE_LINE_ICON( QPixmap( ":/sprite/remove_line_icon.png" ) ),
				DELETE_LINE_ICON( QPixmap( ":/sprite/remove_icon.png" ) ),

				FULLSCREEN_ICON( QPixmap( ":/sprite/fullscreen_icon.png" ) ),
				WINDOWED_ICON( QPixmap( ":/sprite/windowed_icon.png" ) ),

				SCENE_WINDOW_ICON( QPixmap( ":/sprite/scene_window_icon.png" ) ),
				INSPECTOR_WINDOW_ICON( QPixmap( ":/sprite/inspector_window_icon.png" ) ),
				RENDER_WINDOW_ICON( QPixmap( ":/sprite/render_window_icon.png" ) ),
				SETTING_WINDOW_ICON( QPixmap( ":/sprite/setting_window_icon.png" ) ),
				SELECTION_WINDOW_ICON( QPixmap( ":/sprite/selection_window_icon.png" ) ),
				CONSOLE_WINDOW_ICON( QPixmap( ":/sprite/console_window_icon.png" ) ),
				SEQUENCE_WINDOW_ICON( QPixmap( ":/sprite/sequence_window_icon.png" ) ),

				OPEN_FOLDER_ICON( QPixmap( ":/sprite/open_session_icon.png" ) ),

				DEFAULT_ITEM( QPixmap( ":/sprite/default_item_icon.png" ) ),

				MOLECULE_SYMBOL( QPixmap( ":/sprite/symbol/molecule_symbol_icon.png" ) ),
				CHAIN_SYMBOL( QPixmap( ":/sprite/symbol/chain_symbol_icon.png" ) ),
				RESIDUE_SYMBOL( QPixmap( ":/sprite/symbol/residue_symbol_icon.png" ) ),
				ATOM_SYMBOL( QPixmap( ":/sprite/symbol/atom_symbol_icon.png" ) ),
				REPRESENTATION_SYMBOL( QPixmap( ":/sprite/symbol/atom_symbol_icon.png" ) ),
				VIEWPOINT_SYMBOL( QPixmap( ":/sprite/symbol/viewpoint_symbol_icon.png" ) ),
				LABEL_SYMBOL( QPixmap( ":/sprite/symbol/label_symbol_icon.png" ) ),

				REPRESENTATION_STICK_ICON( QPixmap( ":/sprite/representation_stick_icon.png" ) ),
				REPRESENTATION_BALL_AND_STICK_ICON( QPixmap( ":/sprite/representation_ball_and_stick_icon.png" ) ),
				REPRESENTATION_VDW_ICON( QPixmap( ":/sprite/representation_van_der_waals_icon.png" ) ),
				REPRESENTATION_CARTOON_ICON( QPixmap( ":/sprite/representation_cartoon_icon.png" ) ),
				REPRESENTATION_STICK_AND_CARTOON_ICON(
					QPixmap( ":/sprite/representation_stick_and_cartoon_icon.png" ) ),
				REPRESENTATION_BALL_STICK_AND_CARTOON_ICON(
					QPixmap( ":/sprite/representation_ball_stick_and_cartoon_icon.png" ) ),
				REPRESENTATION_TRACE_ICON( QPixmap( ":/sprite/representation_stick_icon.png" ) ),
				REPRESENTATION_SAS_ICON( QPixmap( ":/sprite/representation_sas_icon.png" ) ),
				REPRESENTATION_SES_ICON( QPixmap( ":/sprite/representation_ses_icon.png" ) ),

				RESIDUE_CATEGORY_ICON( QPixmap( ":/sprite/residue_category_icon.png" ) ),

				TOOLTAB_SEPARATOR( QPixmap( ":/sprite/main_menu_separator.png" ) ),

				DIHEDRAL_ANGLE_RENDER_ICON_MASK(
					Util::UI::generateAlphaMask( ":/sprite/render/dihedral_angle_icon.png" ) ) {};
		};

		namespace WidgetProperty
		{
			inline const char * const OVERIDDEN_PARAMETER			 = "OVERIDDEN_PARAMETER";
			inline const char * const ATTRIBUTE_LAYOUT_SECTION_TITLE = "ATTRIBUTE_LAYOUT_SECTION_TITLE";
			inline const char * const OVERLAY_STYLE					 = "OVERLAY_STYLE";
		} // namespace WidgetProperty

		inline static const std::string WINDOW_TITLE_SCENE_MODIFIED_FEEDBACK = "*";

		inline static const int LAYOUT_VERSION = 1;

		inline static const int STATUS_PROGRESS_BAR_CHUNKS = 10;
		inline static const int STATUS_PROGRESS_BAR_WIDTH  = 100;

		static const QSize INSPECTOR_PREFERRED_SIZE			   = QSize( 300, 100 );
		static const QSize INSPECTOR_MINIMUM_SIZE			   = QSize( 1, 1 );
		static const QSize RENDER_PREFERRED_SIZE			   = QSize( 1000, 1000 );
		static const QSize RENDER_MINIMUM_SIZE				   = QSize( 1, 1 );
		static const QSize SCENE_PREFERRED_SIZE				   = QSize( 200, 100 );
		static const QSize SCENE_MINIMUM_SIZE				   = QSize( 1, 1 );
		static const QSize CONSOLE_PREFERRED_SIZE			   = QSize( 100, 80 );
		static const QSize CONSOLE_MINIMUM_SIZE				   = QSize( 1, 1 );
		static const QSize SEQUENCE_PREFERRED_SIZE			   = QSize( 100, 70 );
		static const QSize SEQUENCE_MINIMUM_SIZE			   = QSize( 1, 1 );
		static const QSize SELECTION_PREFERRED_SIZE			   = QSize( 100, 150 );
		static const QSize SELECTION_MINIMUM_SIZE			   = QSize( 1, 1 );
		static const QSize SETTINGS_PREFERRED_SIZE			   = QSize( 550, 600 );
		static const QSize SETTINGS_MINIMUM_SIZE			   = QSize( 1, 1 );
		static const QSize INFORMATION_PREFERRED_SIZE		   = QSize( 550, 600 );
		static const QSize INFORMATION_MINIMUM_SIZE			   = QSize( 100, 100 );
		static const QSize STRUCTURAL_ALIGNMENT_PREFERRED_SIZE = QSize( 550, 600 );
		static const QSize STRUCTURAL_ALIGNMENT_MINIMUM_SIZE   = QSize( 1, 1 );

		inline static const int MAIN_MENU_MAX_BUTTON_PRESET_WIDTH = 125;

		inline static const int			MOLECULE_NAME_MAXIMUM_SIZE = 32;
		inline static const std::string VIEWPOINT_GROUP_NAME	   = "Viewpoints";
		inline static const std::string VIEWPOINT_DEFAULT_NAME	   = "Viewpoint";

		inline static const int INSPECTOR_HEADER_HEIGHT						 = 30;
		inline static const int INSPECTOR_HEADER_NAME_SPACING				 = 10;
		inline static const int INSPECTOR_CONTENT_HORIZONTAL_OFFSET			 = 10;
		inline static const int MULTIPLE_INSPECTOR_INFO_DATA_COUNT_DISPLAYED = 5;
		inline static const int INSPECTOR_INFO_BOND_COUNT_DISPLAYED			 = 100;

		inline static const std::string SEQUENCE_CHAIN_NAME_SEPARATOR				 = "/";
		inline static const int			SEQUENCE_CHAIN_SCALE_STEP					 = 5;
		inline static const QColor		SEQUENCE_FOREGROUND_SELECTION_COLOR			 = QColor( 0, 122, 204, 125 );
		inline static const int			SEQUENCE_MAX_MISSING_RESIDUE_BEFORE_COLLAPSE = 5;
		inline static const QChar		SEQUENCE_MISSING_RESIDUE_SYMBOL				 = '-';

		inline static const int SCENE_SPACE_BETWEEN_ITEMS  = 4;
		inline static const int DATA_GRID_VERTICAL_SPACE   = 2;
		inline static const int DATA_GRID_HORIZONTAL_SPACE = 10;
		inline static const int DATA_GRID_INDENT_SIZE	   = 10;
		inline static const int DATA_GRID_DEFAULT_V_SPACER = 10;

		inline static const int ATTRIBUTE_LIST_LAYOUT_SMALL_SECTION_SPACER = 15;
		inline static const int ATTRIBUTE_LIST_LAYOUT_BIG_SECTION_SPACER   = 30;

		enum class RENDER_OVERLAY_STYLE : int
		{
			STYLE_OPAQUE,
			STYLE_TRANSPARENT,
		};
		inline static const std::vector<QString> RENDER_OVERLAY_STYLE_NAME			 = { "OPAQUE", "TRANSPARENT" };
		const int								 RENDER_OVERLAY_ANCHOR_OFFSET_TOP	 = 5;
		const int								 RENDER_OVERLAY_ANCHOR_OFFSET_BOTTOM = 5;
		const int								 RENDER_OVERLAY_ANCHOR_OFFSET_LEFT	 = 5;
		const int								 RENDER_OVERLAY_ANCHOR_OFFSET_RIGHT	 = 5;

		inline static const QFont  WORLD_LABEL_FONT			  = QFont( "Arial", 20, QFont::Weight::Bold, false );
		inline static const float  WORLD_LABEL_NEAR_CLIP	  = 20.f;
		inline static const float  WORLD_LABEL_FAR_CLIP		  = 150.0f;
		inline static const QColor WORLD_LABEL_OUTLINE_COLOR  = Qt::GlobalColor::black;
		inline static const QColor WORLD_LABEL_FILL_COLOR	  = Qt::GlobalColor::white;
		inline static const float  WORLD_LABEL_MIN_TEXT_SCALE = 0.1f;
		inline static const float  WORLD_LABEL_MAX_TEXT_SCALE = 1.f;

		inline static const int MEASUREMENT_DISTANCE_NB_DECIMALS = 2;

		inline static const int LABEL_RENDER_POINT_RADIUS		= 1;
		inline static const int LABEL_RENDER_MIN_LINE_THICKNESS = 1;
		inline static const int LABEL_RENDER_MAX_LINE_THICKNESS = 5;
		inline static const int LABEL_RENDER_POINT_MAX_DIAMETER
			= LABEL_RENDER_MAX_LINE_THICKNESS + LABEL_RENDER_POINT_RADIUS * 2;
		inline static const QColor LABEL_RENDER_COLOR = Qt::GlobalColor::blue;

		inline static const int	   MEASUREMENT_DISTANCE_LABEL_POINT_RADIUS		 = LABEL_RENDER_POINT_RADIUS;
		inline static const int	   MEASUREMENT_DISTANCE_LABEL_MIN_LINE_THICKNESS = LABEL_RENDER_MIN_LINE_THICKNESS;
		inline static const int	   MEASUREMENT_DISTANCE_LABEL_MAX_LINE_THICKNESS = LABEL_RENDER_MAX_LINE_THICKNESS;
		inline static const QColor MEASUREMENT_DISTANCE_LABEL_LINE_COLOR		 = LABEL_RENDER_COLOR;
		inline static const Qt::PenStyle MEASUREMENT_DISTANCE_LABEL_LINE_STYLE	 = Qt::PenStyle::DashLine;

		inline static const int			 MEASUREMENT_ANGLE_LABEL_POINT_RADIUS		= LABEL_RENDER_POINT_RADIUS;
		inline static const int			 MEASUREMENT_ANGLE_LABEL_MIN_LINE_THICKNESS = LABEL_RENDER_MIN_LINE_THICKNESS;
		inline static const int			 MEASUREMENT_ANGLE_LABEL_MAX_LINE_THICKNESS = LABEL_RENDER_MAX_LINE_THICKNESS;
		inline static const int			 MEASUREMENT_ANGLE_LABEL_TEXT_OFFSET		= 20;
		inline static const QColor		 MEASUREMENT_ANGLE_LINE_COLOR				= Qt::GlobalColor::blue;
		inline static const Qt::PenStyle MEASUREMENT_ANGLE_LINE_STYLE				= Qt::PenStyle::DashLine;
		inline static const int			 MEASUREMENT_ANGLE_ARC_RADIUS				= 50;
		inline static const QColor		 MEASUREMENT_ANGLE_ARC_COLOR				= Qt::GlobalColor::blue;
		inline static const Qt::PenStyle MEASUREMENT_ANGLE_ARC_STYLE				= Qt::PenStyle::SolidLine;

		inline static const int MEASUREMENT_DIHEDRAL_ANGLE_LABEL_POINT_RADIUS		= LABEL_RENDER_POINT_RADIUS;
		inline static const int MEASUREMENT_DIHEDRAL_ANGLE_LABEL_MIN_LINE_THICKNESS = LABEL_RENDER_MIN_LINE_THICKNESS;
		inline static const int MEASUREMENT_DIHEDRAL_ANGLE_LABEL_MAX_LINE_THICKNESS = LABEL_RENDER_MAX_LINE_THICKNESS;
		inline static const int MEASUREMENT_DIHEDRAL_ANGLE_LABEL_TEXT_OFFSET		= 2;
		inline static const QColor		 MEASUREMENT_DIHEDRAL_ANGLE_LINE_COLOR		= Qt::GlobalColor::blue;
		inline static const Qt::PenStyle MEASUREMENT_DIHEDRAL_ANGLE_LINE_STYLE		= Qt::PenStyle::DashLine;
		inline static const int			 MEASUREMENT_DIHEDRAL_ANGLE_ICON_MIN_SIZE	= 10;
		inline static const int			 MEASUREMENT_DIHEDRAL_ANGLE_ICON_MAX_SIZE	= 48;

		inline static const QString ALIGNMENT_WINDOW_UNKNOWN_RMSD = "-";

		inline static const QColor CONSOLE_INFO_COLOR	 = QColor( "white" );
		inline static const QColor CONSOLE_DEBUG_COLOR	 = QColor( "white" );
		inline static const QColor CONSOLE_WARNING_COLOR = QColor( "yellow" );
		inline static const QColor CONSOLE_ERROR_COLOR	 = QColor( "red" );

		inline static const QString DIFFERENT_MULTIPLE_DATA_STRING = "-";

		inline static const QSize EXPORT_IMAGE_PREVIEW_SIZE = QSize( 320, 320 );

		const float ORIENT_ZOOM_FACTOR = 0.666f;

		inline static const QFont TOOL_MENU_BUTTON_FONT()
		{
			static QFont font = QFont( "Helvetica", 10 );
			return font;
		}
		inline static const QFont SEQUENCE_DISPLAY_FONT()
		{
			static QFont font = QFont( "Courier", 10 );
			return font;
		}
		inline static const bool VIDEO_IN_INFORMATION = true;

		inline static const QColor BLACK_COLOR			  = QColor( 0, 0, 0 );
		inline static const QColor BACKGROUND_COLOR		  = QColor( 34, 34, 34 );
		inline static const QColor CANCELED_COLOR		  = QColor( 71, 71, 71 );
		inline static const QColor HIGHLIGHTED_COLOR	  = QColor( 85, 85, 85 );
		inline static const QColor CANCELED_TEXT_COLOR	  = QColor( 185, 185, 185 );
		inline static const QColor TEXT_COLOR			  = QColor( 255, 255, 255 );
		inline static const QColor PLACEHOLDER_TEXT_COLOR = QColor( 200, 200, 200 );

		inline static const void applyApplicationPaletteInPalette( QPalette & p_palette )
		{
			p_palette.setColor( QPalette::ColorRole::Window, BACKGROUND_COLOR );
			p_palette.setColor( QPalette::ColorRole::WindowText, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::WindowText, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::Base, BACKGROUND_COLOR );
			p_palette.setColor( QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, CANCELED_COLOR );
			p_palette.setColor( QPalette::ColorRole::Text, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::PlaceholderText, PLACEHOLDER_TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::ToolTipBase, BLACK_COLOR );
			p_palette.setColor( QPalette::ColorRole::ToolTipText, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::Button, BACKGROUND_COLOR );
			p_palette.setColor( QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, CANCELED_COLOR );
			p_palette.setColor( QPalette::ColorRole::ButtonText, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, CANCELED_TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::BrightText, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::Link, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::LinkVisited, TEXT_COLOR );

			p_palette.setColor( QPalette::ColorRole::Light, HIGHLIGHTED_COLOR );
			p_palette.setColor( QPalette::ColorRole::Midlight, BACKGROUND_COLOR );
			p_palette.setColor( QPalette::ColorRole::Mid, BACKGROUND_COLOR );
			p_palette.setColor( QPalette::ColorRole::Shadow, BLACK_COLOR );
			p_palette.setColor( QPalette::ColorRole::Dark, BLACK_COLOR );

			p_palette.setColor( QPalette::ColorRole::Highlight, HIGHLIGHTED_COLOR );
		};

	} // namespace Style
} // namespace VTX::UI

#endif
