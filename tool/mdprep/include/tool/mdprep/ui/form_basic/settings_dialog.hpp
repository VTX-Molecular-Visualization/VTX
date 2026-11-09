#ifndef VTX_TOOL_MDPREP_UI_FORM_BASIC_SETTINGS_DIALOG_HPP
#define VTX_TOOL_MDPREP_UI_FORM_BASIC_SETTINGS_DIALOG_HPP

#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
#include <memory>
#include <optional>
#include <util/concepts.hpp>
#include <utility>
#include <variant>

class QWidget;
class QDialog;
class QComboBox;
class QLineEdit;
class QDoubleSpinBox;
class QPushButton;
class QVBoxLayout;

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParametersStepData;
	struct EngineSpecificCommonInformation;
	struct MinimizationData;
	struct SystemData;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui::form_basic
{
	using namespace VTX::Tool::Mdprep::Gateway;

	// Class that aim to provide a way to signal when the use clicks the "Apply" button
	class ApplyVisitor
	{
	  public:
		ApplyVisitor() = default;

		template<typename T>
			requires( not VTX::SameUnalteredType<T, ApplyVisitor> )
		ApplyVisitor( T && p_ ) : _ptr( new _wrapper<T>( std::forward<T>( p_ ) ) )
		{
			static_assert(
				requires( T t ) {
					{ t.applied() };
				}, "You need to implement 'void applied()' class method" );
		}

		void applied()
		{
			if ( _ptr )
			{
				_ptr->applied();
			}
		}

	  private:
		struct _interface
		{
			virtual ~_interface() = default;

			virtual void applied() = 0;
		};

		std::unique_ptr<_interface> _ptr = nullptr;

		template<typename T>
		struct _wrapper final : public _interface
		{
			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}

			void applied() override { _obj.applied(); }

		  private:
			T _obj;
		};
	};

	// Class responsible for managing a settings panel for advanced options of an MD step, but from the basic form.
	class SettingsDialog
	{
	  public:
		using SupportedData = std::variant<MdParametersStepData *, MinimizationData *, SystemData *>;
		SettingsDialog()	= default;
		SettingsDialog( QWidget *												p_parent,
						const char *											p_title,
						SupportedData											p_dataFields,
						MdEngineSpecificFieldPlacer								p_engineFieldPlacer,
						std::optional<Gateway::EngineSpecificCommonInformation> p_engineInfo   = std::nullopt,
						ApplyVisitor											p_applyVisitor = ApplyVisitor() );

		SettingsDialog( const SettingsDialog & )			 = delete;
		SettingsDialog & operator=( const SettingsDialog & ) = delete;
		SettingsDialog( SettingsDialog && )					 = default;
		SettingsDialog & operator=( SettingsDialog && )		 = default;
		~SettingsDialog() noexcept;

	  private:
		SupportedData _data;

		struct UiData
		{
			QDialog *		 container				  = nullptr;
			QDoubleSpinBox * fieldDt				  = nullptr;
			QLineEdit *		 fieldSaveInterval		  = nullptr;
			QDoubleSpinBox * fieldMinEnergyBreakpoint = nullptr;
			QDoubleSpinBox * fieldMinStepSize		  = nullptr;
			QPushButton *	 buttonApply			  = nullptr;
			QPushButton *	 buttonCancel			  = nullptr;
			QComboBox *		 fieldForceFieldBio		  = nullptr;
			QComboBox *		 fieldForceFieldChem	  = nullptr;
			QComboBox *		 fieldBoxShape			  = nullptr;
			QComboBox *		 fieldWaterModel		  = nullptr;

			std::optional<Gateway::EngineSpecificCommonInformation> engineInfo;
		};

		UiData						_uiObjects;
		MdEngineSpecificFieldPlacer _engineFieldPlacer;
		ApplyVisitor				_applyVisitor;

		void		_setupUi( QWidget * p_parent, const char * p_title );
		static void _setupDataUi( QVBoxLayout *				   p_qMainLayout,
								  UiData &					   p_uiObjects,
								  const MdParametersStepData * p_data );
		static void _setupDataUi( QVBoxLayout * p_qMainLayout, UiData & p_uiObjects, const MinimizationData * p_data );
		static void _setupDataUi( QVBoxLayout * p_qMainLayout, UiData & p_uiObjects, const SystemData * p_data );
		static void _applyDataChange( const UiData & p_uiObjects, MinimizationData * p_data );
		static void _applyDataChange( const UiData & p_uiObjects, MdParametersStepData * p_data );
		static void _applyDataChange( const UiData & p_uiObjects, SystemData * p_data );
		void		_setupSlots();
		void		_applyChanges();
		void		_cancelChanges() const;
	};
} // namespace VTX::Tool::Mdprep::ui::form_basic

#endif
