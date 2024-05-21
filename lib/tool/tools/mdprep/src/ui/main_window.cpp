#include "tools/mdprep/ui/form_data.hpp"
//
#include "tools/mdprep/ui/engine_form_gromacs.hpp"
#include "tools/mdprep/ui/md_engine_form.hpp"
//
#include "tools/mdprep/ui/main_window.hpp"
//
#include <qformlayout.h>
#include <qlayout.h>
#include <qtabwidget.h>

namespace VTX::Tool::Mdprep::ui
{
	constexpr const char * string( const E_MD_ENGINE & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_MD_ENGINE::gromacs: return { "Gromacs" };
		default: break;
		}
		return "Please provide a user string for the MD engine"; // For developers that add new MD Engine support
	}
	namespace
	{
		constexpr std::array<const char *, MD_ENGINE_NUMBER> createMdEngineStringList()
		{
			std::array<const char *, MD_ENGINE_NUMBER> out;
			for ( int i = 0; i < MD_ENGINE_NUMBER; i++ )
			{
				out[ i ] = string( static_cast<E_MD_ENGINE>( i ) );
			}
			return out;
		}
	} // namespace
	constexpr const std::array<const char *, MD_ENGINE_NUMBER> g_mdEngineStrings = createMdEngineStringList();

	const std::array<const char *, MD_ENGINE_NUMBER> & mdEngineStrings() { return g_mdEngineStrings; }

	MdEngineForm form( const E_MD_ENGINE & p_engine, FormLayouts p_layout ) noexcept
	{
		switch ( p_engine )
		{
		case E_MD_ENGINE::gromacs: return { EngineFormGromacs( std::move( p_layout ) ) };
		default: return { EngineFormGromacs( std::move( p_layout ) ) }; // Default is gromacs. Period.
		}
	}

	void MdFieldsOrganizer::setupUi( QLayout * p_layout ) noexcept
	{
		QTabWidget * wTab = new QTabWidget();
		p_layout->addWidget( wTab );

		containerParamBasic	   = new QWidget();
		containerParamAdvanced = new QWidget();

		wTab->addTab( containerParamBasic, "Basic Settings" );
		wTab->addTab( containerParamAdvanced, "Advanced Settings" );
	}

} // namespace VTX::Tool::Mdprep::ui
