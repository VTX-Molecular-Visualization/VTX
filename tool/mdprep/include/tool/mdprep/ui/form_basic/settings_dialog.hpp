#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC_SETTINGS_DIALOG__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC_SETTINGS_DIALOG__

#include <optional>
#include <util/concepts.hpp>
#include <variant>

class QWidget;
class QLineEdit;
class QDoubleSpinBox;
class QPushButton;

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParametersStepData;
	struct EngineSpecificCommonInformation;
	struct MinimizationData;
	struct SystemData;
	class MdEngineFieldPlacer;
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
				}, "You need to implement 'void applied() noexcept' class method"
			);
		}

		void applied() noexcept
		{
			if ( _ptr )
				_ptr->applied();
		}

	  private:
		struct _interface
		{
			virtual ~_interface() = default;

			virtual void applied() noexcept = 0;
		};
		std::unique_ptr<_interface> _ptr = nullptr;

		template<typename T>
		struct _wrapper final : public _interface
		{
			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}

			virtual void applied() noexcept { _obj.applied(); }

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
		SettingsDialog(
			QWidget *												p_parent,
			const char *											p_title,
			SupportedData											p_dataFields,
			MdEngineSpecificFieldPlacer								p_engineFieldPlacer,
			std::optional<Gateway::EngineSpecificCommonInformation> p_engineInfo   = std::nullopt,
			ApplyVisitor											p_applyVisitor = ApplyVisitor()
		);

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

		void _setupUi( QWidget * p_parent, const char * p_title );
		void _setupSlots() noexcept;
		void _applyChanges() noexcept;
		void _cancelChanges() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui::form_basic

#endif
