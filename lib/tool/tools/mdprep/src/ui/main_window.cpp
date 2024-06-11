#include <string>
#include <vector>
//
#include "tools/mdprep/gateway/backend_gromacs.hpp"
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/gateway/shared.hpp"
#include "tools/mdprep/ui/shared.hpp"
//
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tools/mdprep/ui/field_placer_gromacs.hpp"
#include "tools/mdprep/ui/md_engine_field_placer.hpp"
//
#include <qformlayout.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
//
#include "tools/mdprep/ui/main_window.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	constexpr const char * string( const E_MD_ENGINE & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_MD_ENGINE::gromacs: return { "Gromacs" };
		default: break;
		}
		return "DEBUUUG"; // For developers that add new MD Engine support
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

	namespace
	{

		class NoopForm
		{
			Gateway::EngineSpecificCommonInformation _data;

		  public:
			NoopForm() = default;
			inline void
			get( MdEngineSpecificFieldPlacer & p_ptr, const VTX::Tool::Mdprep::ui::E_FIELD_SECTION & ) noexcept
			{
			}
			inline void get( const Gateway::EngineSpecificCommonInformation *& p_ptr ) const noexcept
			{
				p_ptr = &_data;
			}
			inline void		   activate() noexcept {}
			inline void		   deactivate() noexcept {}
			friend inline void get( const NoopForm &, Gateway::EngineSpecificCommonInformation & ) noexcept {}
		};
	} // namespace

	MdEngineFieldPlacer form( const E_MD_ENGINE & p_engine, FormLayouts p_layout ) noexcept
	{
		switch ( p_engine )
		{
		case E_MD_ENGINE::gromacs: return { GromacsFieldPlacer( std::move( p_layout ) ) };
		default: return { NoopForm() };
		}
	}

	void MdFieldsOrganizer::setupUi( QLayout * p_layout, const E_FORM_MODE & p_mode ) noexcept
	{
		_buttonViewSwitch		   = new QPushButton;
		QWidget * wButtonContainer = new QWidget;
		p_layout->addWidget( wButtonContainer );
		QHBoxLayout * qLayoutButton = new QHBoxLayout( wButtonContainer );
		qLayoutButton->setContentsMargins( { 0, 0, 0, 0 } );
		qLayoutButton->addStretch( 1 );
		qLayoutButton->addWidget( _buttonViewSwitch );
		QObject::connect( _buttonViewSwitch, &QPushButton::clicked, [ & ]() { this->switchFormMode(); } );
		containerParamBasic	   = new QWidget();
		containerParamAdvanced = new QWidget();
		p_layout->addWidget( containerParamBasic );
		p_layout->addWidget( containerParamAdvanced );

		_mode = p_mode;
		if ( _mode == E_FORM_MODE::basic )
			_changeModeBasic();
		else
			_changeModeAdvanced();
	}

	void MdFieldsOrganizer::switchFormMode() noexcept
	{
		if ( _mode == E_FORM_MODE::basic )
		{
			_changeModeAdvanced();
			_mode = E_FORM_MODE::advanced;
		}
		else
		{
			_changeModeBasic();
			_mode = E_FORM_MODE::basic;
		}
	}

	void MdFieldsOrganizer::_changeModeBasic() noexcept
	{
		_buttonViewSwitch->setText( "Switch to Advanced view" );
		containerParamAdvanced->hide();
		containerParamBasic->show();
	}

	void MdFieldsOrganizer::_changeModeAdvanced() noexcept
	{
		_buttonViewSwitch->setText( "Switch to Simple view" );
		containerParamBasic->hide();
		containerParamAdvanced->show();
	}

} // namespace VTX::Tool::Mdprep::ui
