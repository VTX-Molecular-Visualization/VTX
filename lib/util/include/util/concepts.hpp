#ifndef __VTX_UTIL_CONCEPTS__
#define __VTX_UTIL_CONCEPTS__

#include <concepts>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

namespace VTX
{
	// https://stackoverflow.com/questions/60449592/how-do-you-define-a-c-concept-for-the-standard-library-containers
	template<class ContainerType>
	concept Container
		= requires( ContainerType a, const ContainerType b ) {
			  requires std::regular<ContainerType>;
			  requires std::swappable<ContainerType>;
			  requires std::destructible<typename ContainerType::value_type>;
			  requires std::same_as<typename ContainerType::reference, typename ContainerType::value_type &>;
			  requires std::
				  same_as<typename ContainerType::const_reference, const typename ContainerType::value_type &>;
			  requires std::forward_iterator<typename ContainerType::iterator>;
			  requires std::forward_iterator<typename ContainerType::const_iterator>;
			  requires std::signed_integral<typename ContainerType::difference_type>;
			  requires std::same_as<
				  typename ContainerType::difference_type,
				  typename std::iterator_traits<typename ContainerType::iterator>::difference_type>;
			  requires std::same_as<
				  typename ContainerType::difference_type,
				  typename std::iterator_traits<typename ContainerType::const_iterator>::difference_type>;
			  {
				  a.begin()
				  } -> std::same_as<typename ContainerType::iterator>;
			  {
				  a.end()
				  } -> std::same_as<typename ContainerType::iterator>;
			  {
				  b.begin()
				  } -> std::same_as<typename ContainerType::const_iterator>;
			  {
				  b.end()
				  } -> std::same_as<typename ContainerType::const_iterator>;
			  {
				  a.cbegin()
				  } -> std::same_as<typename ContainerType::const_iterator>;
			  {
				  a.cend()
				  } -> std::same_as<typename ContainerType::const_iterator>;
			  {
				  a.size()
				  } -> std::same_as<typename ContainerType::size_type>;
			  {
				  a.max_size()
				  } -> std::same_as<typename ContainerType::size_type>;
			  {
				  a.empty()
				  } -> std::same_as<bool>;
		  }
	// Special case for std::vector<bool> because of its special management in the standard which doesn't fit the previous requirements (Proxy on ContainerType::reference & const reference which return bool instead of const bool &)
	// https://en.cppreference.com/w/cpp/container/vector_bool
	|| std::same_as<ContainerType, std::vector<bool>>;

	// Concept to template any container of ValueType
	template<typename ContainerType, typename ValueType>
	concept ContainerOfType = requires( ContainerType p_container ) {
		requires Container<ContainerType>;
		{ *( p_container.begin() ) } -> std::convertible_to<ValueType>;
	};

	template<typename EnumType>
	concept EnumConcept = std::is_enum<EnumType>::value;

	template<typename StringType>
	concept StringConcept = requires( StringType & p_strLike ) { std::string( p_strLike ); };

	// Following struct are defined in order to create a concept later on.

	template<class T>
	struct RemoveConst
	{
		typedef T type;
	};

	template<class T>
	struct RemoveConst<const T>
	{
		typedef T type;
	};

	template<class T>
	struct RemoveRef
	{
		typedef T type;
	};

	template<class T>
	struct RemoveRef<T &>
	{
		typedef T type;
	};
	template<class T>
	struct RemoveConstRef
	{
		typedef T type;
	};

	template<class T>
	struct RemoveConstRef<const T &>
	{
		typedef T type;
	};
	template<class T>
	struct RemoveGlobalRef
	{
		typedef T type;
	};

	template<class T>
	struct RemoveGlobalRef<T &&>
	{
		typedef T type;
	};

	// Remove const, reference, global reference, and a combination of the 3 from a type.
	template<class T>
	struct RemoveTypeAlterations
	{
		typedef T type;
	};

	template<class T>
	struct RemoveTypeAlterations<T &>
	{
		typedef T type;
	};
	template<class T>
	struct RemoveTypeAlterations<T &&>
	{
		typedef T type;
	};
	template<class T>
	struct RemoveTypeAlterations<const T &>
	{
		typedef T type;
	};
	template<class T>
	struct RemoveTypeAlterations<const T &&>
	{
		typedef T type;
	};
	template<class T>
	struct RemoveTypeAlterations<const T>
	{
		typedef T type;
	};

	// Compare two types and return true if they are the same, regardless of their const-ness and reference-ness
	template<typename LEFT, typename RIGHT>
	concept SameUnalteredType
		= std::same_as<typename RemoveTypeAlterations<LEFT>::type, typename RemoveTypeAlterations<RIGHT>::type>;

	template<typename T>
	concept IsSmartPtr = requires( T a ) {
		{ a.get() } -> std::same_as<typename std::pointer_traits<T>::element_type *>;
	};

} // namespace VTX

#endif
