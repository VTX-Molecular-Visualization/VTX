#ifndef __VTX_TOOL_TOPOLOGY_EDITOR_DIALOG_TOPOLOGY_EDITOR_DIALOG__
#define __VTX_TOOL_TOPOLOGY_EDITOR_DIALOG_TOPOLOGY_EDITOR_DIALOG__

#include <QPointer>
#include <app/ecs.hpp>
#include <core/struct/topology.hpp>
#include <array>
#include <memory>
#include <ui/qt/dialog/base_dialog.hpp>

class QLabel;
class QLineEdit;
class QPushButton;
class QShowEvent;
class QTableWidget;

namespace VTX::Tool::TopologyEditor::Dialog
{
	class TopologyEditorDialog : public UI::QT::Dialog::BaseDialog<TopologyEditorDialog>
	{
	  public:
		explicit TopologyEditorDialog( Entity p_system );
		Entity getSystem() const;

	  protected:
		void showEvent( QShowEvent * p_event ) override;

	  private:
		QPointer<QLabel>	  _systemName;
		QPointer<QLabel>	  _systemStatus;
		QPointer<QLineEdit>	  _chainsFilter;
		QPointer<QLineEdit>	  _residuesFilter;
		QPointer<QLineEdit>	  _atomsFilter;
		QPointer<QLineEdit>	  _bondsFilter;
		QPointer<QLineEdit>	  _validationFilter;
		QPointer<QTableWidget> _chainsTable;
		QPointer<QTableWidget> _residuesTable;
		QPointer<QTableWidget> _atomsTable;
		QPointer<QTableWidget> _bondsTable;
		QPointer<QTableWidget> _validationTable;
		QPointer<QLineEdit>	  _selectedType;
		QPointer<QLineEdit>	  _selectedName;
		std::array<QPointer<QLabel>, 6>	  _propertyLabels;
		std::array<QPointer<QLineEdit>, 6> _propertyValues;
		QPointer<QPushButton> _applyButton;

		Entity _system;

		void _buildUi();
		void _clearSystemViews();
		void _clearSelectionInspector();
		void _setInspectorProperty( const int p_index, const QString & p_label, const QString & p_value );
		void _populateTables();
		void _populateChainsTable();
		void _populateResiduesTable();
		void _populateAtomsTable();
		void _populateBondsTable();
		void _populateValidationTable();
		void _applyTableFilter( QTableWidget & p_table, const QString & p_filter );
		void _updateSelectionInspector( const Core::Struct::E_SYSTEM_ITEM p_item, const Index p_index );
		void _updateBondInspector( const Index p_index );
	};
} // namespace VTX::Tool::TopologyEditor::Dialog

#endif
