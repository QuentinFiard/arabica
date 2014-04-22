#ifndef ARABICA_SAX_TAGSOUP_ELEMENT_HPP
#define ARABICA_SAX_TAGSOUP_ELEMENT_HPP

#include <Arabica/StringAdaptor.hpp>
#include <SAX/helpers/AttributesImpl.hpp>
#include "ElementType.hpp"

namespace Arabica
{

namespace SAX
{

/**
 The internal representation of an actual element (not an element type).
 An Element has an element type, attributes, and a successor Element
 for use in constructing stacks and queues of Elements.
 @see ElementType
 @see AttributesImpl
   
 Based on code from John Cowan's super TagSoup package
*/

template <class string_type, class string_adaptor>
class Element_impl;

template <class string_type,
          class string_adaptor = Arabica::default_string_adaptor<string_type> >
class Element
{
public:
  typedef AttributesImpl<string_type, string_adaptor> AttributesImplT;
  typedef Element<string_type, string_adaptor> ElementT;
  typedef ElementType<string_type, string_adaptor> ElementTypeT;
  typedef Element_impl<string_type, string_adaptor> Element_implT;

  static const ElementT Null;

  Element(ElementTypeT& type, bool defaultAttributes);
  Element(const ElementT& rhs);
  ~Element();

  ElementT& operator=(const ElementT& rhs);
  bool operator==(const ElementT& rhs) const;
  bool operator!=(const ElementT& rhs) const;

  /** 
     Return the element type.
     @return The element type.
  */
  const ElementTypeT& type() const;

  /**
     Return the attributes as an AttributesImpl object.
     @return The attributes
     @see AttributesImpl
  */
  const AttributesImplT& atts() const;

  /**
     return the next element in the element stack or queue
     @return the next element
  */
  ElementT next() const;

  /**
     Change the next element in an element stack or queue.
     @param next The new next element
  */
  void setNext(const ElementT& next);

  /**
     Return the name of the element's type.
     Convenience method.
     @return the element type name
  */
  string_type name() const;


  /**
     Return the namespace name of the element's type.
     Convenience method.
     @return The element type namespace name
  */
  string_type namespaceName() const;

  /**
     Return the local name of the element's type.
     Convenience method.
     @return The element type local name
  */
  string_type localName() const;

  /**
     Return the content model vector of the element's type.
     Convenience method.
     @return The content model vector
  */
  int model() const; 


  /**
     Return the member-of vector of the element's type.
     Convenience method.
     @return The member-of vector
  */
  int memberOf() const; 

  /**
     Return the flags vector of the element's type.
     Convenience method.
     @return The flags vector
  */
  int flags() const;

  /**
     Return the parent element type of the element's type
     Convenience method
     @return the parent element type
  */
  ElementTypeT& parent() const;

  /**
     Return true if the type of this element can contain the type of
     another element.
     Convenience method.
     @param other The other element
  */
  bool canContain(const ElementT& other) const;

  /**
     Set an attribute and its value into this element.
     @param name The attribute name (Qname)
     @param type The attribute type
     @param value The attribute value
  */
  void setAttribute(const string_type& name, const string_type& type, const string_type& value);

  /**
     Make this element anonymous.
     Remove any <tt>id</tt> or <tt>name</tt> attribute present
     in the element's attributes.
  */
  void anonymize();

  /**
     Clean the attributes of this element.
     Attributes with null name (the name was ill-formed)
     or null value (the attribute was present in the element type but
     not in this actual element) are removed.
  */
  void clean();

  /**
     Force this element to preclosed status, meaning that an end-tag has
     been seen but the element cannot yet be closed for structural reasons.
  */
  void preclose();

  /**
     Return true if this element has been preclosed.
  */
  bool isPreclosed() const;

private:
  Element() : impl_(0) { }
  bool is_null() const { return impl_ == 0; }

  Element_implT* impl_;

  template <class string_type2, class string_adaptor2>
  friend class Element_impl;
}; // class Element

template <class string_type,
          class string_adaptor = Arabica::default_string_adaptor<string_type> >
class Element_impl 
{
private:
  typedef AttributesImpl<string_type, string_adaptor> AttributesImplT;
  typedef Element<string_type, string_adaptor> ElementT;
  typedef ElementType<string_type, string_adaptor> ElementTypeT;
  typedef Element_impl<string_type, string_adaptor> Element_implT;

  ElementTypeT* type_;		// type of element
  AttributesImplT atts_;		// attributes of element
  ElementT next_;		// successor of element
  bool preclosed_;		// this element has been preclosed
  int refCount_;

public:
  /**
     Return an Element from a specified ElementType.
     @param type The element type of the newly constructed element
     @param defaultAttributes True if default attributes are wanted
  */
  Element_impl(ElementTypeT& type, bool defaultAttributes) :
    type_(&type),
    atts_(),
    next_(),
    preclosed_(false),
    refCount_(1)
  {
    if (defaultAttributes) 
      atts_ = type.atts();
  } // Element_impl

  const ElementTypeT& type() const { return *type_; }
  const AttributesImplT& atts() const { return atts_; }

  ElementT next() const 
  { 
    if(next_.is_null())
      return ElementT::Null;
    return next_; 
  } // next()
  void setNext(const ElementT& next) { next_ = next; }

  string_type name() const { return type_->name(); }
  string_type namespaceName() const { return type_->namespaceName(); }
  string_type localName() const { return type_->localName(); }
  int model() const { return type_->model(); }
  int memberOf() const { return type_->memberOf(); }
  int flags() const { return type_->flags(); }
  ElementTypeT& parent() const { return type_->parent(); }
  bool canContain(const Element_implT* const other) const { return type_->canContain(*(other->type_)); }

  void setAttribute(const string_type& name, const string_type& type, const string_type& value) 
  {
    type_->setAttribute(atts_, name, type, value);
  } // setAttribute
  
  void anonymize() 
  {
    for (int i = atts_.getLength() - 1; i >= 0; i--) 
    {
      if((atts_.getType(i) == string_adaptor::construct_from_utf8("ID")) ||
         (atts_.getQName(i) == string_adaptor::construct_from_utf8("name")))
	atts_.removeAttribute(i);
    } // for ...
  } // anonymize

  void clean() 
  {
    for (int i = atts_.getLength() - 1; i >= 0; i--) 
    {
      const string_type& name = atts_.getLocalName(i);
      if (string_adaptor::empty(atts_.getValue(i)) || string_adaptor::empty(name))
      {
	atts_.removeAttribute(i);
	continue;
      } // if ...
    } // for ...
  } // clean

  void preclose() 
  {
    preclosed_ = true;
  } // preclose

  bool isPreclosed() const
  {
    return preclosed_;
  } // isPreclosed

  void add_ref() 
  { 
    ++refCount_; 
  } // add_ref

  void remove_ref() 
  {
    --refCount_;
    if(refCount_ == 0)
      delete this;
  } // remove_ref

private:
  Element_impl();

  ~Element_impl()
  {
  } // ~Element_impl


  Element_impl(const Element_implT& rhs);
  Element_implT& operator=(const Element_implT& rhs);
  bool operator==(const Element_implT& rhs) const;
  bool operator!=(const Element_implT& rhs) const;
}; // class Element_impl

template <class string_type, class string_adaptor>
const Element<string_type, string_adaptor> Element<string_type, string_adaptor>::Null(
    ElementType<string_type, string_adaptor>::Null, false);

//////////////////////////////////////////////
template <class string_type, class string_adaptor>
Element<string_type, string_adaptor>::Element(const Element& rhs) :
  impl_(rhs.impl_)
{
  impl_->add_ref();
} // Element

template <class string_type, class string_adaptor>
Element<string_type, string_adaptor>::Element(ElementType<string_type, string_adaptor>& type, bool defaultAttributes) :
    impl_(new Element_implT(type, defaultAttributes))
{
} // Element

template <class string_type, class string_adaptor>
Element<string_type, string_adaptor>::~Element()
{
  if(impl_)
    impl_->remove_ref();
} // ~Element

template <class string_type, class string_adaptor>
Element<string_type, string_adaptor>& Element<string_type, string_adaptor>::operator=(const Element<string_type, string_adaptor>& rhs)
{
  if(impl_ == rhs.impl_)
    return *this;

  if(impl_)
   impl_->remove_ref();
  impl_ = rhs.impl_;
  if(impl_)
    impl_->add_ref();
  return *this;
} // operator=

template <class string_type, class string_adaptor>
bool Element<string_type, string_adaptor>::operator==(const Element<string_type, string_adaptor>& rhs) const
{
  return impl_ == rhs.impl_;
} // operator==

template <class string_type, class string_adaptor>
bool Element<string_type, string_adaptor>::operator!=(const Element<string_type, string_adaptor>& rhs) const
{
  return !(operator==(rhs));
} // operator!=

template <class string_type, class string_adaptor>
const ElementType<string_type, string_adaptor>& Element<string_type, string_adaptor>::type() const
{
  return impl_->type();
} // type

template <class string_type, class string_adaptor>
const AttributesImpl<string_type, string_adaptor>& Element<string_type, string_adaptor>::atts() const
{
  return impl_->atts();
} // atts

template <class string_type, class string_adaptor>
Element<string_type, string_adaptor> Element<string_type, string_adaptor>::next() const
{
  return impl_->next();
} // next

template <class string_type, class string_adaptor>
void Element<string_type, string_adaptor>::setNext(const Element& next)
{
  impl_->setNext(next);
} // setNext

template <class string_type, class string_adaptor>
string_type Element<string_type, string_adaptor>::name() const { return impl_->name(); }
template <class string_type, class string_adaptor>
string_type Element<string_type, string_adaptor>::namespaceName() const { return impl_->namespaceName(); }
template <class string_type, class string_adaptor>
string_type Element<string_type, string_adaptor>::localName() const { return impl_->localName(); }

template <class string_type, class string_adaptor>
int Element<string_type, string_adaptor>::model() const { return impl_->model(); }
template <class string_type, class string_adaptor>
int Element<string_type, string_adaptor>::memberOf() const { return impl_->memberOf(); }
template <class string_type, class string_adaptor>
int Element<string_type, string_adaptor>::flags() const { return impl_->flags(); }

template <class string_type, class string_adaptor>
ElementType<string_type, string_adaptor>& Element<string_type, string_adaptor>::parent() const { return impl_->parent(); }
template <class string_type, class string_adaptor>
bool Element<string_type, string_adaptor>::canContain(const Element& other) const { return impl_->canContain(other.impl_); }

template <class string_type, class string_adaptor>
void Element<string_type, string_adaptor>::setAttribute(const string_type& name, const string_type& type, const string_type& value)
{
  impl_->setAttribute(name, type, value);
} // setAttribute

template <class string_type, class string_adaptor>
void Element<string_type, string_adaptor>::anonymize() { impl_->anonymize(); }
template <class string_type, class string_adaptor>
void Element<string_type, string_adaptor>::clean() { impl_->clean(); }
template <class string_type, class string_adaptor>
void Element<string_type, string_adaptor>::preclose() { impl_->preclose(); }
template <class string_type, class string_adaptor>
bool Element<string_type, string_adaptor>::isPreclosed() const { return impl_->isPreclosed(); }

} // namespace SAX

} // namespace Arabica

#endif
