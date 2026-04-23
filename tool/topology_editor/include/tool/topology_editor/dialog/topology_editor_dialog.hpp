#ifndef __VTX_TOOL_TOPOLOGY_EDITOR_DIALOG_TOPOLOGY_EDITOR_DIALOG__
#define __VTX_TOOL_TOPOLOGY_EDITOR_DIALOG_TOPOLOGY_EDITOR_DIALOG__

#include <QPointer>
#include <app/ecs.hpp>
#include <memory>
#include <optional>
#include <ui/qt/dialog/base_dialog.hpp>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QShowEvent;
class QTableWidget;
class QTreeView;

namespace VTX::UI::QT::Model
{
	class SystemModel;
}

namespace VTX::Tool::TopologyEditor::Dialog
{
	class TopologyEditorDialog : public UI::QT::Dialog::BaseDialog<TopologyEditorDialog>
	{
	  public:
		TopologyEditorDialog();

	  protected:
		void showEvent( QShowEvent * p_event ) override;

	  private:
		QPointer<QComboBox>	  _systemSelector;
		QPointer<QLabel>	  _systemStatus;
		QPointer<QTreeView>	  _structureTree;
		QPointer<QTableWidget> _chainsTable;
		QPointer<QTableWidget> _residuesTable;
		QPointer<QTableWidget> _atomsTable;
		QPointer<QTableWidget> _bondsTable;
		QPointer<QTableWidget> _validationTable;
		QPointer<QLineEdit>	  _selectedType;
		QPointer<QLineEdit>	  _selectedName;
		QPointer<QLineEdit>	  _selectedParent;
		QPointer<QLineEdit>	  _selectedPosition;
		QPointer<QPushButton> _applyButton;

		std::unique_ptr<UI::QT::Model::SystemModel> _systemModel;
		std::optional<App::ECS::Entity>			   _currentSystem;

		void _buildUi();
		void _refreshSystems();
		void _setCurrentSystem( const App::ECS::Entity p_system );
		void _clearSystemViews();
		void _populateTables();
		void _populateChainsTable();
		void _populateResiduesTable();
		void _populateAtomsTable();
		void _populateBondsTable();
		void _populateValidationTable();
		void _updateSelectionInspector( const QModelIndex & p_index );
	};
} // namespace VTX::Tool::TopologyEditor::Dialog

#endif
