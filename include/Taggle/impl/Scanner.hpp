#ifndef ARABICA_SAX_SCANNER_HPP
#define ARABICA_SAX_SCANNER_HPP

#include <istream>

namespace Arabica
{
namespace SAX
{

template<class string_type, class string_adaptor>
class ScanHandler;

/**
  An interface allowing Parser to invoke scanners.

  This code is derived from John Cowan's splendid TagSoup package
*/
template<class string_type,
         class string_adaptor = Arabica::default_string_adaptor<string_type> >
class Scanner 
{
public:
  typedef ScanHandler<string_type, string_adaptor> ScanHandlerT;

  virtual ~Scanner() {}

	/**
  Invoke a scanner.
	@param r A source of characters to scan
	@param h A ScanHandler to report events to
	**/
	virtual void scan(std::basic_istream<typename string_type::value_type>& r,
                    ScanHandlerT& h) = 0;

	/**
	Reset the embedded locator.
	@param publicid The publicid of the source
	@param systemid The systemid of the source
	**/
  virtual void resetDocumentLocator(
      const string_type& publicid, const string_type& systemid) = 0;

	/**
	Signal to the scanner to start CDATA content mode.
	**/
	virtual void startCDATA() = 0;
}; // Scanner


} // namespace SAX
} // namespace Arabica
#endif
