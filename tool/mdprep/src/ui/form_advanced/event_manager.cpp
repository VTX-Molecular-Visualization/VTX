#include <qboxlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>
//
#include <ui/qt/util.hpp>
#include <util/sentry.hpp>
//
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/gateway/shared.hpp"
#include "tool/mdprep/ui/form_advanced/shared.hpp"
//
#include "tool/mdprep/ui/input_checker.hpp"
#include "tool/mdprep/ui/report.hpp"
//
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
#include "tool/mdprep/ui/shared.hpp"
//
#include "tool/mdprep/ui/form_advanced/event_manager.hpp"

namespace VTX::Tool::Mdprep::ui::form_advanced
{
	EventManager::EventManager( ReportManager & p_reportManager, Gateway::MdParameters p_param ) :
		parameters( std::move( p_param ) ), reportManager( &p_reportManager )
	{
	}
	EventManager::~EventManager()
	{
		if ( _fieldSystemBoxShape != nullptr )
			_fieldSystemBoxShape->disconnect();
	}
	EventManager::EventManager( EventManager && p_other ) noexcept :
		fieldPlacers( std::move( p_other.fieldPlacers ) ), _fieldSystemBoxShape( p_other._fieldSystemBoxShape ),
		_fieldBioForceField( p_other._fieldBioForceField ), parameters( std::move( p_other.parameters ) ),
		reportManager( std::move( p_other.reportManager ) ), _sentry( std::move( p_other._sentry ) )
	{
		if ( _fieldSystemBoxShape )
			_fieldSystemBoxShape->disconnect();
		if ( _fieldBioForceField )
			_fieldBioForceField->disconnect();
		_connectAll();
	}
	EventManager & EventManager::operator=( EventManager && p_other ) noexcept
	{
		if ( this == &p_other )
			return *this;

		if ( _fieldSystemBoxShape )
			_fieldSystemBoxShape->disconnect();
		if ( p_other._fieldSystemBoxShape )
			p_other._fieldSystemBoxShape->disconnect();

		if ( _fieldBioForceField )
			_fieldBioForceField->disconnect();
		if ( p_other._fieldBioForceField )
			p_other._fieldBioForceField->disconnect();

		_bioForcefieldResultLayout = p_other._bioForcefieldResultLayout;
		_fieldSystemBoxShape	   = p_other._fieldSystemBoxShape;
		_fieldBioForceField		   = p_other._fieldBioForceField;

		parameters	  = std::move( p_other.parameters );
		reportManager = std::move( p_other.reportManager );
		_sentry		  = std::move( p_other._sentry );

		p_other._bioForcefieldResultLayout = nullptr;
		p_other._fieldSystemBoxShape	   = nullptr;
		p_other._fieldBioForceField		   = nullptr;

		fieldPlacers = std::move( p_other.fieldPlacers );

		_connectAll();

		return *this;
	}
	void EventManager::connectBoxShape( QComboBox * p_fieldSystemBoxShape ) noexcept
	{
		if ( _fieldSystemBoxShape )
			_fieldSystemBoxShape->disconnect();
		_fieldSystemBoxShape = p_fieldSystemBoxShape;
		_connectBoxShape();
	}
	void EventManager::connectInputCheck( QPushButton * p_buttonStartInputCheck ) noexcept
	{
		if ( _buttonStartInputCheck )
			_buttonStartInputCheck->disconnect();
		_buttonStartInputCheck = p_buttonStartInputCheck;
		_connectInputCheck();
	}
	void EventManager::connectBioForceField( QComboBox * p_fieldBioForceField, QVBoxLayout * p_target ) noexcept
	{
		if ( _fieldBioForceField )
			_fieldBioForceField->disconnect();
		_fieldBioForceField		   = p_fieldBioForceField;
		_bioForcefieldResultLayout = p_target;
		this->reportManager->resetReportLocation( p_target, _sentry );
		_connectForceField();
	}
	void EventManager::performFirstInputCheck( const Gateway::MdParameters & p_params ) noexcept
	{
		if ( reportManager->hasFirstCheckBeenDone() == false )
			reportManager->checkInputs( p_params );
	}

	void EventManager::_connectAll() noexcept
	{
		_connectBoxShape();
		_connectForceField();
	}
	void EventManager::_connectBoxShape() noexcept
	{
		if ( _fieldSystemBoxShape )
			QComboBox::connect(
				_fieldSystemBoxShape,
				&QComboBox::currentTextChanged,
				[ & ]
				{
					this->fieldPlacers[ 0 ].update( EngineSpecificCommonInformationFieldUpdate {
						.field	  = E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD ::boxShape,
						.newValue = static_cast<size_t>( _fieldSystemBoxShape->currentIndex() ) } );
				}
			);
	}
	void EventManager::_connectInputCheck() noexcept
	{
		if ( _buttonStartInputCheck == nullptr )
			return;

		QPushButton::connect( _buttonStartInputCheck, &QPushButton::clicked, _getInputCheckCallback() );
	}
	void EventManager::_connectForceField() noexcept
	{
		if ( _fieldBioForceField == nullptr )
			return;

		QComboBox::connect( _fieldBioForceField, &QComboBox::currentTextChanged, _getInputCheckCallback() );
	}
	std::function<void()> EventManager::_getInputCheckCallback() noexcept
	{
		return [ & ]()
		{
			Gateway::MdParameters params;
			params.system.forcefieldBio = this->_fieldBioForceField->currentText().toStdString();
			this->reportManager->checkInputs( params );
		};
	}
} // namespace VTX::Tool::Mdprep::ui::form_advanced
