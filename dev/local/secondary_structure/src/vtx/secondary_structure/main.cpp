#include <iostream>
#include <vtx/secondary_structure/files.hpp>
#include <vtx/secondary_structure/parse.hpp>
#include <vtx/secondary_structure/tests.hpp>
// TMP
#include <ranges>
#include <string_view>
// !TMP

int main()
{
	const auto toString	  = []( const auto & v ) -> std::string_view { return std::string_view( v.data(), v.size() ); };
	const auto notEmpty	  = []( const auto & v ) -> bool { return v.size() > 0; };
	const auto showString = []( const std::string_view & v ) { std::cout << "Obtaining string <" << v << ">\n"; };

	std::string_view testStr { "ein kleine stringe  jaaaa !" };

	std::ranges::for_each(
		testStr | std::views::split( ' ' ) | std::views::filter( notEmpty ) | std::views::transform( toString ),
		showString
	);
	// const std::ranges::owning_view ov {
	//	std::views::transform( std::views::filter( std::views::split( testStr, ' ' ), notEmpty ), toString )
	// };
	auto view = testStr | std::views::split( ' ' ) | std::views::filter( notEmpty ) | std::views::transform( toString );
	// auto view = std::views::transform( std::views::filter( std::views::split( testStr, ' ' ), notEmpty ), toString );
	// const std::ranges::ref_view rv { view };
	std::cout << "Size of view : <" << std::ranges::distance( view ) << ">\n";
	std::cout << "Third position : <" << *std::ranges::next( view.begin(), 0 ) << ">\n";
	for ( const auto e : view )
		std::cout << "Obtaining string <" << e << ">\n";
	// auto view = std::views::transform( std::views::filter( std::views::split( testStr, ' ' ), notEmpty ), toString );
	// const std::ranges::ref_view rv { view };
	// for ( const auto e : rv )
	//	std::cout << "Obtaining string <" << e << ">\n";

	return 0;

	try
	{
		pdb100::Context context;
		pdb100::reporter( context.log );
		pdb100::enumerateFiles( context );
		pdb100::testSystem( *context.pdb100_system.begin(), context.results );
	}
	catch ( std::exception & e )
	{
		std::cout << "Error : " << e.what();
	}
	return 0;
}
