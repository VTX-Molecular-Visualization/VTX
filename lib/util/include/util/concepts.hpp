#ifndef __VTX_UTIL_CONCEPTS__
#define __VTX_UTIL_CONCEPTS__

#include <concepts>
#include <iterator>
#include <type_traits>

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
		  };

	template<typename ContainerType, typename ValueType>
	concept ContainerOfType = requires( ContainerType p_container ) {
								  requires Container<ContainerType>;
								  {
									  *( p_container.begin() )
									  } -> std::convertible_to<ValueType>;
							  };

	template<typename EnumType>
	concept EnumConcept = std::is_enum<EnumType>::value;

	template<typename StringType>
	concept StringConcept = requires( StringType & p_strLike ) { std::string( p_strLike ); };
} // namespace VTX

#endif
