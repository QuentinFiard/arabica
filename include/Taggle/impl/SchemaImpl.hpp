#ifndef ARABICA_SAX_TAGGLE_SCHEMAIMPL_HPP
#define ARABICA_SAX_TAGGLE_SCHEMAIMPL_HPP

#include <cctype>
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <Arabica/StringAdaptor.hpp>
#include "ElementType.hpp"
#include "Schema.hpp"

namespace Arabica
{
namespace SAX
{

/**
Abstract class representing a TSSL schema.
Actual TSSL schemas are compiled into concrete subclasses of this class.

Based on code from John Cowan's super TagSoup package
**/
template<class string_type,
         class string_adaptor = Arabica::default_string_adaptor<string_type> >
class SchemaImpl : public Schema<string_type, string_adaptor>
{
public:
  typedef ElementType<string_type, string_adaptor> ElementTypeT;
  typedef Schema<string_type, string_adaptor> SchemaT;
  using SchemaT::M_ANY;
  using SchemaT::M_EMPTY;
  using SchemaT::M_PCDATA;
  using SchemaT::M_ROOT;

  using SchemaT::F_RESTART;
  using SchemaT::F_CDATA;
  using SchemaT::F_NOFORCE;

private:
  std::map<string_type, int> entities_;
  std::map<string_type, std::unique_ptr<ElementTypeT> > elementTypes_;

	string_type URI_;
	string_type prefix_;
	ElementTypeT* root_;

public:
	/**
	Add or replace an element type for this schema.
	@param name Name (Qname) of the element
	@param model Models of the element's content as a vector of bits
	@param memberOf Models the element is a member of as a vector of bits
	@param flags Flags for the element
	**/
	void elementType(const string_type& name, int model, int memberOf, int flags)
  {
		std::unique_ptr<ElementTypeT> e(new ElementTypeT(name, model, memberOf, flags, *this));
    string_type lname = lower_case(name);
		elementTypes_[lname] = std::move(e);
		if(memberOf == M_ROOT)
      root_ = elementTypes_[lname].get();
	} // elementType

	/**
	Get the root element of this schema
	**/
	ElementTypeT& rootElementType()
  {
		return *root_;
	} // rootElementType

	/**
	Add or replace a default attribute for an element type in this schema.
	@param elemName Name (Qname) of the element type
	@param attrName Name (Qname) of the attribute
	@param type Type of the attribute
	@param value Default value of the attribute; null if no default
	**/
	void attribute(const string_type& elemName, const string_type& attrName, const string_type& type, const string_type& value)
  {
		ElementTypeT& e = getElementType(elemName);
    if (e == ElementTypeT::Null)
    {
      throw std::runtime_error(
          "Attribute " + string_adaptor::asStdString(attrName) +
				  " specified for unknown element type " +
				  string_adaptor::asStdString(elemName));
		}
		e.setAttribute(attrName, type, value);
	} // attribute

	/**
	Specify natural parent of an element in this schema.
	@param name Name of the child element
	@param parentName Name of the parent element
	**/
	void parent(const string_type& name, const string_type& parentName)
  {
		ElementTypeT& child = getElementType(name);
		ElementTypeT& parent = getElementType(parentName);
    if (child == ElementTypeT::Null)
    {
      throw std::runtime_error(
          "No child " + string_adaptor::asStdString(name) + " for parent " +
          string_adaptor::asStdString(parentName));
		}
		if (parent == ElementTypeT::Null)
    {
			throw std::runtime_error(
          "No parent " + string_adaptor::asStdString(parentName) +
          " for child " + string_adaptor::asStdString(name));
		}
		child.setParent(parent);
	} // parent

	/**
	Add to or replace a character entity in this schema.
	@param name Name of the entity
	@param value Value of the entity
	**/
	void entity(const string_type& name, int value)
  {
		entities_[name] = value;
	} // entity

	/**
	Get an ElementTypeT by name.
	@param name Name (Qname) of the element type
	@return The corresponding ElementTypeT
	**/
	ElementTypeT& getElementType(const string_type& name)
  {
    const auto &elemType = elementTypes_.find(lower_case(name));
    if(elemType == elementTypes_.end())
      return ElementTypeT::Null;
    return *elemType->second;
	} // getElementType

	/**
	Get an entity value by name.
	@param name Name of the entity
	@return The corresponding character, or 0 if none
	**/
	int getEntity(const string_type& name) const
  {
    const auto &ent = entities_.find(name);
    if(ent == entities_.end())
      return 0;
		return ent->second;
	} // getEntity

	/**
	Return the URI (namespace name) of this schema.
	**/
	const string_type& getURI() const
  {
		return URI_;
	} // getURI

	/**
	Return the prefix of this schema.
	**/
	const string_type& getPrefix() const
  {
		return prefix_;
	} // getPrefix

	/**
	Change the URI (namespace name) of this schema.
	**/
	void setURI(const string_type &uri)
  {
		URI_ = uri;
	} // setURI

	/**
	Change the prefix of this schema.
	**/
	void setPrefix(const string_type &prefix)
  {
		prefix_ = prefix;
	} // setPrefix

private:
  static string_type lower_case(const string_type& str)
  {
    std::wstring lower = string_adaptor::asStdWString(str);
    std::transform(lower.begin(), lower.end(), lower.begin(), std::towlower);
    return string_adaptor::construct_from_utf16(lower.c_str(), lower.size());
  } // lower_case
}; // class Schema

} // namespace SAX

} // namespace Arabica
#endif
