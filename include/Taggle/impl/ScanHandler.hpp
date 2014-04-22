#ifndef ARABICA_SCAN_HANDLER_HPP
#define ARABICA_SCAN_HANDLER_HPP

#include <Arabica/StringAdaptor.hpp>

namespace Arabica
{

namespace SAX
{

/**
 An interface that Scanners use to report events in the input stream.
 
 This code is derived from John Cowan's splendid TagSoup package
*/
template<class string_type,
         class string_adaptor = Arabica::default_string_adaptor<string_type> >
class ScanHandler
{
protected:
  ~ScanHandler() {}

public:
	/**
	Reports an attribute name without a value.
	**/
	virtual void adup(const string_type& buff) = 0;

	/**
	Reports an attribute name; a value will follow.
	**/
	virtual void aname(const string_type& buff) = 0;

	/**
	Reports an attribute value.
	**/
	virtual void aval(const string_type& buff) = 0;

	/**
	  * Reports the content of a CDATA section (not a CDATA element)
	  */
	virtual void cdsect(const string_type& buff) = 0;

	/**
   * Reports a <!....> declaration - typically a DOCTYPE
   */
	virtual void decl(const string_type& buff) = 0;

	/**
	Reports an entity reference or character reference.
	**/
	virtual void entity(const string_type& buff) = 0;

	/**
	Reports EOF.
	**/
	virtual void eof(const string_type& buff) = 0;

	/**
	Reports an end-tag.
	**/
	virtual void etag(const string_type& buff) = 0;

	/**
	Reports the general identifier (element type name) of a start-tag.
	**/
	virtual void gi(const string_type& buff) = 0;

	/**
	Reports character content.
	**/
	virtual void pcdata(const string_type& buff) = 0;

	/**
	Reports the data part of a processing instruction.
	**/
	virtual void pi(const string_type& buff) = 0;

	/**
	Reports the target part of a processing instruction.
	**/
	virtual void pitarget(const string_type& buff) = 0;

	/**
	Reports the close of a start-tag.
	**/
	virtual void stagc(const string_type& buff) = 0;

	/**
	Reports the close of an empty-tag.
	**/
	virtual void stage(const string_type& buff) = 0;

	/**
	Reports a comment.
	**/
	virtual void cmnt(const string_type& buff) = 0;

	/**
	Returns the value of the last entity or character reference reported.
	**/
	virtual int getEntity() = 0;
}; // class ScanHandler

} // namespace SAX

} // namespace Arabica

#endif
