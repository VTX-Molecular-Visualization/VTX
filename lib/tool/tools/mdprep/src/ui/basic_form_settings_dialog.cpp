#include <qdialog.h>
#include <qformlayout.h>
#include <qspinbox.h>
//
#include <ui/qt/util.hpp>
//
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tools/mdprep/ui/basic_form_settings_dialog.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	BasicFormSettingsDialog::BasicFormSettingsDialog(
		QWidget *					p_parent,
		MdParametersStepData &		p_dataFields,
		MdEngineSpecificFieldPlacer p_engineFieldPlacer,
		uint64_t &					p_nstxout
	) :
		_dataFields( &p_dataFields ),
		_nstxout( &p_nstxout ), _engineFieldPlacer( std::move( p_engineFieldPlacer ) )
	{
		_setupUi( p_parent );
	}
	BasicFormSettingsDialog::BasicFormSettingsDialog(
		QWidget *					p_parent,
		MdParametersStepData &		p_dataFields,
		MdEngineSpecificFieldPlacer p_engineFieldPlacer
	) :
		_dataFields( &p_dataFields ),
		_engineFieldPlacer( std::move( p_engineFieldPlacer ) )
	{
		_setupUi( p_parent );
	}
	BasicFormSettingsDialog::~BasicFormSettingsDialog() noexcept
	{
		if ( _uiObjects.container )
			_uiObjects.container->close();
	}
	void BasicFormSettingsDialog::_setupUi( QWidget * p_parent )
	{
		_uiObjects.container  = new QDialog( p_parent );
		QFormLayout * qLayout = new QFormLayout( _uiObjects.container );

		_uiObjects.fieldDt = new QDoubleSpinBox;
		_uiObjects.fieldDt->setMinimum( 1e-3 );
		_uiObjects.fieldDt->setMaximum( 1e-1 );
		_uiObjects.fieldDt->setSingleStep( 1e-3 );
		qLayout->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Time step interval", "TODO", VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			_uiObjects.fieldDt
		);

		_uiObjects.fieldSaveInterval = VTX::UI::QT::Util::addUInt64Field( qLayout, "Save Interval", "TODO" );
		_uiObjects.container->open();
	}
} // namespace VTX::Tool::Mdprep::ui
