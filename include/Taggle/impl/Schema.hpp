#ifndef ARABICA_SAX_TAGGLE_SCHEMA_HPP
#define ARABICA_SAX_TAGGLE_SCHEMA_HPP

#include <Arabica/StringAdaptor.hpp>

namespace Arabica
{
namespace SAX
{

template<class string_type, class string_adaptor>
class ElementType;

/**
Abstract class representing a TSSL schema.
Actual TSSL schemas are compiled into concrete subclasses of this class.

Based on code from John Cowan's super TagSoup package
**/
template<class string_type,
         class string_adaptor = Arabica::default_string_adaptor<string_type> >
class Schema
{
public:
  typedef ElementType<string_type, string_adaptor> ElementTypeT;
	static const int M_ANY;
	static const int M_EMPTY;
	static const int M_PCDATA;
	static const int M_ROOT;

	static const int F_RESTART;
	static const int F_CDATA;
	static const int F_NOFORCE;

	virtual void elementType(const string_type& name, int model, int memberOf, int flags) = 0;
	virtual ElementTypeT& rootElementType() = 0;
	virtual void parent(const string_type &name, const string_type &parentName) = 0;

  virtual ElementTypeT& getElementType(const string_type& name) = 0;
  virtual int getEntity(const string_type& name) const = 0;
  virtual const string_type& getURI() const = 0;
	virtual const string_type& getPrefix() const = 0;

  virtual ~Schema() { }
}; // class Schema

template<class string_type, class string_adaptor>
const int Schema<string_type, string_adaptor>::M_ANY = 0xFFFFFFFF;
template<class string_type, class string_adaptor>
const int Schema<string_type, string_adaptor>::M_EMPTY = 0;
template<class string_type, class string_adaptor>
const int Schema<string_type, string_adaptor>::M_PCDATA = 1 << 30;
template<class string_type, class string_adaptor>
const int Schema<string_type, string_adaptor>::M_ROOT = 1 << 31;

template<class string_type, class string_adaptor>
const int Schema<string_type, string_adaptor>::F_RESTART = 1;
template<class string_type, class string_adaptor>
const int Schema<string_type, string_adaptor>::F_CDATA = 2;
template<class string_type, class string_adaptor>
const int Schema<string_type, string_adaptor>::F_NOFORCE = 4;

} // namespace SAX

} // namespace Arabica
#endif
