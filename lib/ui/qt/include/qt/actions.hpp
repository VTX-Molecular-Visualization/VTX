#ifndef __VTX_UI_QT_ACTIONS__
#define __VTX_UI_QT_ACTIONS__

#include <QAction>
#include <QActionGroup>
#include <ui/actions.hpp>
#include <util/hashing.hpp>

namespace VTX::UI::QT
{
	template<typename A>
	concept ConceptAction = std::is_base_of_v<UI::DescAction, A>;

	inline std::unordered_map<Util::Hashing::Hash, QAction *> ACTIONS;
	inline std::unordered_map<std::string, QActionGroup *>	  ACTION_GROUPS;

	QAction * const ACTION( const Util::Hashing::Hash & p_hash, const UI::DescAction & p_action );

	inline QAction * const ACTION( const UI::DescAction & p_action )
	{
		return ACTION( Util::Hashing::hash( &p_action ), p_action );
	}

	template<ConceptAction A>
	inline QAction * const ACTION()
	{
		return ACTION( Util::Hashing::hash( typeid( A ).name() ), A() );
	}

	namespace Action
	{
		struct ShowToolbarText : public DescAction
		{
			ShowToolbarText()
			{
				name = "Show toolbar text";
				tip	 = "Show/hide text beside tool buttons";
			}
		};

	} // namespace Action

} // namespace VTX::UI::QT

#endif
