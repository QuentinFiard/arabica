#ifndef ARABICA_SAX_TAGGLE_PARSER_HPP
#define ARABICA_SAX_TAGGLE_PARSER_HPP

#include <algorithm>
#include <locale>
#include <memory>
#include <unordered_map>
#include <vector>
#include <Arabica/StringAdaptor.hpp>
#include <SAX/helpers/DefaultHandler.hpp>
#include <SAX/helpers/InputSourceResolver.hpp>
#include <SAX/SAXParseException.hpp>
#include <SAX/XMLReader.hpp>
#include <text/normalize_whitespace.hpp>
#include <XML/XMLCharacterClasses.hpp>
#include <io/uri.hpp>
#include "ScanHandler.hpp"

namespace Arabica
{

namespace SAX
{

/**
The Taggle SAX parser class.

Based on code from John Cowan's super TagSoup package
**/
template <class string_type,
          class string_adaptor = Arabica::default_string_adaptor<string_type> >
class TaggleBase  :
    public XMLReaderInterface<string_type, string_adaptor>,
    private DefaultHandler<string_type, string_adaptor>,
    private ScanHandler<string_type, string_adaptor>
{
public:
  typedef XMLReaderInterface<string_type, string_adaptor> XMLReaderT;
  typedef ContentHandler<string_type, string_adaptor> ContentHandlerT;
  typedef LexicalHandler<string_type, string_adaptor> LexicalHandlerT;
  typedef DeclHandler<string_type, string_adaptor> DeclHandlerT;
  typedef DTDHandler<string_type, string_adaptor> DTDHandlerT;
  typedef ErrorHandler<string_type, string_adaptor> ErrorHandlerT;
  typedef EntityResolver<string_type, string_adaptor> EntityResolverT;
  typedef InputSource<string_type, string_adaptor> InputSourceT;
  typedef Locator<string_type, string_adaptor> LocatorT;
  typedef typename string_type::value_type char_type;
  typedef std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>
      UTF32Encoder;


  /**
  A value of "true" indicates namespace URIs and unprefixed local
  names for element and attribute names will be available.
  **/
  static const string_type namespacesFeature;

  /**
  A value of "true" indicates that XML qualified names (with prefixes)
  and attributes (including xmlns* attributes) will be available.
  We don't support this value.
  **/
  static const string_type namespacePrefixesFeature;

  /**
  Reports whether this parser processes external general entities
  (it doesn't).
  **/
  static const string_type externalGeneralEntitiesFeature;

  /**
  Reports whether this parser processes external parameter entities
  (it doesn't).
  **/
  static const string_type externalParameterEntitiesFeature;

  /**
  May be examined only during a parse, after the startDocument()
  callback has been completed; read-only. The value is true if
  the document specified standalone="yes" in its XML declaration,
  and otherwise is false.  (It's always false.)
  **/
  static const string_type isStandaloneFeature;

  /**
  A value of "true" indicates that the LexicalHandler will report
  the beginning and end of parameter entities (it won't).
  **/
  static const string_type lexicalHandlerParameterEntitiesFeature;

  /**
  A value of "true" indicates that system IDs in declarations will
  be absolutized (relative to their base URIs) before reporting.
  (This returns true but doesn't actually do anything.)
  **/
  static const string_type resolveDTDURIsFeature;

  /**
  Has a value of "true" if all XML names (for elements,
  prefixes, attributes, entities, notations, and local
  names), as well as Namespace URIs, will have been interned
  using java.lang.String.intern. This supports fast testing of
  equality/inequality against string constants, rather than forcing
  slower calls to String.equals().  (We always intern.)
  **/
  static const string_type stringInterningFeature;

  /**
  Returns "true" if the Attributes objects passed by this
  parser in ContentHandler.startElement() implement the
  org.xml.sax.ext.Attributes2 interface.  (They don't.)
  **/
  static const string_type useAttributes2Feature;

  /**
  Returns "true" if the Locator objects passed by this parser
  in ContentHandler.setDocumentLocator() implement the
  org.xml.sax.ext.Locator2 interface.  (They don't.)
  **/
  static const string_type useLocator2Feature;

  /**
  Returns "true" if, when setEntityResolver is given an object
  implementing the org.xml.sax.ext.EntityResolver2 interface,
  those new methods will be used.  (They won't be.)
  **/
  static const string_type useEntityResolver2Feature;

  /**
  Controls whether the parser is reporting all validity errors
  (We don't report any validity errors.)
  **/
  static const string_type validationFeature;

  /**
  Controls whether the parser reports Unicode normalization
  errors as described in section 2.13 and Appendix B of the XML
  1.1 Recommendation.  (We don't normalize.)
  **/
  static const string_type unicodeNormalizationCheckingFeature;

  /**
  Controls whether, when the namespace-prefixes feature is set,
  the parser treats namespace declaration attributes as being in
  the http://www.w3.org/2000/xmlns/ namespace.  (It doesn't.)
  **/
  static const string_type xmlnsURIsFeature;

  /**
  Returns "true" if the parser supports both XML 1.1 and XML 1.0.
  (Always false.)
  **/
  static const string_type XML11Feature;

  /**
  A value of "true" indicates that the parser will ignore
  unknown elements.
  **/
  static const string_type ignoreBogonsFeature;

  /**
  A value of "true" indicates that the parser will give unknown
  elements a content model of EMPTY; a value of "false", a
  content model of ANY.
  **/
  static const string_type bogonsEmptyFeature;

  /**
  A value of "true" indicates that the parser will allow unknown
  elements to be the root element.
  **/
  static const string_type rootBogonsFeature;

  /**
  A value of "true" indicates that the parser will return default
  attribute values for missing attributes that have default values.
  **/
  static const string_type defaultAttributesFeature;

  /**
  A value of "true" indicates that the parser will 
  translate colons into underscores in names.
  **/
  static const string_type translateColonsFeature;

  /**
  A value of "true" indicates that the parser will 
  attempt to restart the restartable elements.
  **/
  static const string_type restartElementsFeature;

  /**
  A value of "true" indicates that the parser will 
  transmit whitespace in element-only content via the SAX
  ignorableWhitespace callback.  Normally this is not done,
  because HTML is an SGML application and SGML suppresses
  such whitespace.
  **/
  static const string_type ignorableWhitespaceFeature;

  /**
  A value of "true" indicates that the parser will treat CDATA
  elements specially.  Normally true, since the input is by
  default HTML.
  **/
  static const string_type CDATAElementsFeature;

  /**
  Used to see some syntax events that are essential in some
  applications: comments, CDATA delimiters, selected general
  entity inclusions, and the start and end of the DTD (and
  declaration of document element name). The Object must implement
  org.xml.sax.ext.LexicalHandler.
  **/
  static const string_type lexicalHandlerProperty;

  /**
  Specifies the Scanner object this Parser uses.
  **/
  static const string_type scannerProperty;

  /**
  Specifies the Schema object this Parser uses.
  **/
  static const string_type schemaProperty;

  /**
  Specifies the AutoDetector (for encoding detection) this Parser uses.
  **/
  static const string_type autoDetectorProperty;

private:
  // Default values for feature flags
  static bool DEFAULT_NAMESPACES;
  static bool DEFAULT_IGNORE_BOGONS;
  static bool DEFAULT_BOGONS_EMPTY;
  static bool DEFAULT_ROOT_BOGONS;
  static bool DEFAULT_DEFAULT_ATTRIBUTES;
  static bool DEFAULT_TRANSLATE_COLONS;
  static bool DEFAULT_RESTART_ELEMENTS;
  static bool DEFAULT_IGNORABLE_WHITESPACE;
  static bool DEFAULT_CDATA_ELEMENTS;

  static const string_type legal;

  typedef Attributes<string_type, string_adaptor> AttributesT;
  typedef AttributesImpl<string_type, string_adaptor> AttributesImplT;
  typedef std::unordered_map<string_type, bool> FeatureMapT;
  typedef Element<string_type, string_adaptor> ElementT;
  typedef HTMLScanner<string_type, string_adaptor> HTMLScannerT;
  typedef HTMLSchema<string_type, string_adaptor> HTMLSchemaT;
  typedef ElementType<string_type, string_adaptor> ElementTypeT;
  typedef Scanner<string_type, string_adaptor> ScannerT;
  typedef Schema<string_type, string_adaptor> SchemaT;

  // XMLReader implementation
  ContentHandlerT* contentHandler_;
  LexicalHandlerT* lexicalHandler_;
  DTDHandlerT* dtdHandler_;
  ErrorHandlerT* errorHandler_;
  EntityResolverT* entityResolver_;
  std::unique_ptr<SchemaT> schema_;
  std::unique_ptr<ScannerT> scanner_;
  FeatureMapT features_;
  ElementT newElement_;
  string_type attributeName_;
  bool doctypeIsPresent_;
  string_type doctypePublicId_;
  string_type doctypeSystemId_;
  string_type doctypeName_;
  string_type piTarget_;
  ElementT stack_;
  ElementT saved_;
  ElementT pcdata_;
  int entity_;

  // Feature flags.  
  bool namespaces;
  bool ignoreBogons;
  bool bogonsEmpty;
  bool rootBogons;
  bool defaultAttributes;
  bool translateColons;
  bool restartElements;
  bool ignorableWhitespace;
  bool CDATAElements;
  bool virginStack;

public:
  TaggleBase() :
    contentHandler_(0),
    lexicalHandler_(0),
    dtdHandler_(0),
    errorHandler_(0),
    entityResolver_(0),
    features_(initialFeatures()),
    newElement_(ElementT::Null),
    attributeName_(),
    doctypeIsPresent_(false),
    doctypePublicId_(),
    doctypeSystemId_(),
    doctypeName_(),
    piTarget_(),
    stack_(ElementT::Null),
    saved_(ElementT::Null),
    pcdata_(ElementT::Null),
    entity_(0),
    namespaces(DEFAULT_NAMESPACES),
    ignoreBogons(DEFAULT_IGNORE_BOGONS),
    bogonsEmpty(DEFAULT_BOGONS_EMPTY),
    rootBogons(DEFAULT_ROOT_BOGONS),
    defaultAttributes(DEFAULT_DEFAULT_ATTRIBUTES),
    translateColons(DEFAULT_TRANSLATE_COLONS),
    restartElements(DEFAULT_RESTART_ELEMENTS),
    ignorableWhitespace(DEFAULT_IGNORABLE_WHITESPACE),
    CDATAElements(DEFAULT_CDATA_ELEMENTS),
    virginStack(true)
  {
    contentHandler_ = this;
    lexicalHandler_ = this;
    dtdHandler_ = this;
    errorHandler_ = this;
    entityResolver_ = this;
  } // Taggle

private:
  static FeatureMapT initialFeatures() 
  {
    FeatureMapT features;
    features[namespacesFeature] = DEFAULT_NAMESPACES;
    features[namespacePrefixesFeature] = false;
    features[externalGeneralEntitiesFeature] = false;
    features[externalParameterEntitiesFeature] = false;
    features[isStandaloneFeature] = false;
    features[lexicalHandlerParameterEntitiesFeature] =  false;
    features[resolveDTDURIsFeature] = true;
    features[stringInterningFeature] = true;
    features[useAttributes2Feature] = false;
    features[useLocator2Feature] = false;
    features[useEntityResolver2Feature] = false;
    features[validationFeature] = false;
    features[xmlnsURIsFeature] = false;
    features[xmlnsURIsFeature] = false;
    features[XML11Feature] = false;
    features[ignoreBogonsFeature] = DEFAULT_IGNORE_BOGONS;
    features[bogonsEmptyFeature] = DEFAULT_BOGONS_EMPTY;
    features[rootBogonsFeature] = DEFAULT_ROOT_BOGONS;
    features[defaultAttributesFeature] = DEFAULT_DEFAULT_ATTRIBUTES;
    features[translateColonsFeature] = DEFAULT_TRANSLATE_COLONS;
    features[restartElementsFeature] = DEFAULT_RESTART_ELEMENTS;
    features[ignorableWhitespaceFeature] = DEFAULT_IGNORABLE_WHITESPACE;
    features[CDATAElementsFeature] = DEFAULT_CDATA_ELEMENTS;
    return features;
  } // initialFeatures

public:
  ///////////////////////////////////////////////////
  // XMLReader
  bool getFeature(const string_type& name) const 
  {
    typename FeatureMapT::const_iterator b = features_.find(name);
    if(b == features_.end()) 
    {
      throw SAXNotRecognizedException("Unknown feature " + string_adaptor::asStdString(name));
    }
    return b->second;
  } // getFeature

  void setFeature(const string_type& name, bool value)
  {
    typename FeatureMapT::iterator b = features_.find(name);
    if(b == features_.end()) 
    {
      throw SAXNotRecognizedException("Unknown feature " + string_adaptor::asStdString(name));
    }

    features_[name] = value;

    if(name == namespacesFeature)
      namespaces = value;
    else if(name == ignoreBogonsFeature) 
      ignoreBogons = value;
    else if(name == bogonsEmptyFeature) 
      bogonsEmpty = value;
    else if(name == rootBogonsFeature) 
      rootBogons = value;
    else if(name == defaultAttributesFeature) 
      defaultAttributes = value;
    else if(name == translateColonsFeature) 
      translateColons = value;
    else if(name == restartElementsFeature) 
      restartElements = value;
    else if(name == ignorableWhitespaceFeature) 
      ignorableWhitespace = value;
    else if(name == CDATAElementsFeature) 
      CDATAElements = value;
  } // setFeature

  typedef typename XMLReaderInterface<string_type, string_adaptor>::PropertyBase PropertyBaseT;
  virtual std::auto_ptr<PropertyBaseT> doGetProperty(const string_type& /*name*/)
  {
    return std::auto_ptr<PropertyBaseT>(0);
  } // doGetProperty

  virtual void doSetProperty(const string_type& /*name*/, std::auto_ptr<PropertyBaseT> /*value*/)
  {
  } // doSetProperty

  /*
  Object getProperty (std::string name)
  {
    if(name.equals(lexicalHandlerProperty)) 
    {
      return lexicalHandler_ == this ? null : lexicalHandler_;
    }
    else if(name.equals(scannerProperty)) 
    {
      return scanner_;
    }
    else if(name.equals(schemaProperty)) 
    {
      return schema_;
    }
    else if(name.equals(autoDetectorProperty)) 
    {
      return theAutoDetector;
    }
    else 
    {
      throw new SAXNotRecognizedException("Unknown property " + name);
    }
  } // getProperty

  void setProperty (std::string name, Object value)
  {
    if(name.equals(lexicalHandlerProperty)) 
    {
      if(value == null) 
      {
        lexicalHandler_ = this;
      }
      else if(value instanceof LexicalHandler) 
      {
        lexicalHandler_ = (LexicalHandler)value;
      }
      else 
      {
        throw new SAXNotSupportedException("Your lexical handler is not a LexicalHandler");
      }
    }
    else if(name.equals(scannerProperty)) 
    {
      if(value instanceof Scanner) {
        scanner_ = (Scanner)value;
        }
      else {
        throw new SAXNotSupportedException("Your scanner is not a Scanner");
        }
      }
    else if(name.equals(schemaProperty)) {
      if(value instanceof Schema) {
        schema_ = (Schema)value;
        }
      else {
         throw new SAXNotSupportedException("Your schema is not a Schema");
        }
      }
    else if(name.equals(autoDetectorProperty)) {
      if(value instanceof AutoDetector) {
        theAutoDetector = (AutoDetector)value;
        }
      else {
        throw new SAXNotSupportedException("Your auto-detector is not an AutoDetector");
        }
      }
    else {
      throw new SAXNotRecognizedException("Unknown property " + name);
      }
    }
*/

  virtual void setEntityResolver(EntityResolverT& resolver) 
  {
    entityResolver_ = &resolver;
  } // setEntityResolver

  virtual EntityResolverT* getEntityResolver() const
  {
    return (entityResolver_ == this) ? 0 : entityResolver_;
  } // getEntityResolver

  virtual void setDTDHandler(DTDHandlerT& handler) 
  {
    dtdHandler_ = &handler;
  } // setDTDHandler

  virtual DTDHandlerT* getDTDHandler() const
  {
    return (dtdHandler_ == this) ? 0 : dtdHandler_;
  } // getDTDHandler

  virtual void setContentHandler(ContentHandlerT& handler) 
  {
    contentHandler_ = &handler;
  } // setContentHandler

  virtual ContentHandlerT* getContentHandler() const 
  {
    return (contentHandler_ == this) ? 0 : contentHandler_;
  } // getContentHandler

  virtual void setErrorHandler(ErrorHandlerT& handler) 
  {
    errorHandler_ = &handler;
  } // setErrorHandler

  virtual ErrorHandlerT* getErrorHandler() const
  {
    return (errorHandler_ == this) ? 0 : errorHandler_;
  } // getErrorHandler

  virtual void setDeclHandler(DeclHandlerT& /*handler*/)
  {
  } // setDeclHandler

  virtual DeclHandlerT* getDeclHandler() const
  {
    return 0;
  } // getDeclHandler

  virtual void setLexicalHandler(LexicalHandlerT& handler)
  {
    lexicalHandler_ = &handler;
  } // setLexicalHandler

  virtual LexicalHandlerT* getLexicalHandler() const
  {
    return (lexicalHandler_ == this) ? 0 : lexicalHandler_;
  } // getLexicalHandler

  virtual void convert_stream(
      std::istream& in, std::basic_stringstream<char_type> &out) = 0;

  virtual void parse(InputSourceT& input) 
  {
    setup();

    InputSourceResolver is(input, string_adaptor());
    if(is.resolve() == 0)
    {
      reportError("Could not resolve XML document", true);
      return;
    } // if(is.resolver() == 0)

    contentHandler_->startDocument();
    scanner_->resetDocumentLocator(input.getPublicId(), input.getSystemId());

    LocatorT* document_locator = dynamic_cast<LocatorT*>(scanner_.get());
    if(document_locator)
      contentHandler_->setDocumentLocator(*document_locator);

    if(!string_adaptor::empty(schema_->getURI()))
      contentHandler_->startPrefixMapping(schema_->getPrefix(),
                                          schema_->getURI());

    std::basic_stringstream<char_type> input_stream;
    convert_stream(*is.resolve(), input_stream);
    scanner_->scan(input_stream, *this);
  } // parse

private:
  static typename string_type::value_type S(char c)
  {
    return string_adaptor::construct_from_utf8(&c, 1)[0];
  } // S

  static string_type S(const std::string& s)
  {
    return string_adaptor::construct_from_utf8(s.c_str());
  } // S

  static string_type S(const char* s)
  {
    return string_adaptor::construct_from_utf8(s);
  } // S

  static wchar_t ToWChar(char_type c) {
    return string_adaptor::asStdWString(string_type(1, c))[0];
  }

  static bool is_digit(char_type c) {
    return Arabica::XML::is_digit(ToWChar(c));
  }

  static bool is_letter(char_type c) {
    return Arabica::XML::is_letter(ToWChar(c));
  }

  static bool is_letter_or_digit(char_type c) {
    return Arabica::XML::is_letter_or_digit(ToWChar(c));
  }

  static bool is_space(char_type c) {
    return Arabica::XML::is_space(ToWChar(c));
  }

  // Sets up instance variables that haven't been set by setFeature
  void setup() 
  {
    schema_.reset(static_cast<SchemaT*>(new HTMLSchemaT()));
    scanner_.reset(new HTMLScannerT());

    stack_ = ElementT(schema_->getElementType(S("<root>")), defaultAttributes);
    pcdata_ = ElementT(schema_->getElementType(S("<pcdata>")), defaultAttributes);

    newElement_ = ElementT::Null;
    attributeName_ = string_adaptor::empty_string();
    piTarget_ = string_adaptor::empty_string();
    saved_ = ElementT::Null;
    entity_ = 0;
    virginStack = true;
    doctypeName_ = doctypePublicId_ = doctypeSystemId_ = string_adaptor::empty_string();
  } // setup

  ///////////////////////////////////////////////////////
  // ScanHandler implementation
  virtual void adup(const string_type& /*buff*/)
  {
    // std::cerr << "adup(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    if(newElement_ == ElementT::Null || string_adaptor::empty(attributeName_))
      return;
    newElement_.setAttribute(attributeName_, string_adaptor::empty_string(), attributeName_);
    attributeName_ = string_adaptor::empty_string();
  } // adup

  virtual void aname(const string_type& buff)
  {
    // std::cerr << "aname(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    if(newElement_ == ElementT::Null)
      return;
    // Currently we don't rely on Schema to canonicalize
    // attribute names.
    attributeName_ = lower_case(makeName(buff));
  } // aname

  virtual void aval(const string_type& buff)
  {
    // std::cerr << "aval(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    if(newElement_ == ElementT::Null || string_adaptor::empty(attributeName_))
      return;
    string_type value = expandEntities(buff);
    newElement_.setAttribute(attributeName_, string_adaptor::empty_string(), value);
    attributeName_ = string_adaptor::empty_string();
  } // aval

  // Expand entity references in attribute values selectively.
  // Currently we expand a reference iff it is properly terminated
  // with a semicolon.
  string_type expandEntities(string_type src)
  {
    UTF32Encoder unicode_encoder;
    size_t refStart = string_adaptor::npos();
    string_type dst;
    for(auto i = string_adaptor::begin(src), ie = string_adaptor::end(src); i != ie; ++i)
    {
      char_type ch = *i;
      string_adaptor::append(dst, ch);
      if(ch == S('&') && refStart == string_adaptor::npos())
      {
        // start of a ref excluding &
        refStart = string_adaptor::length(dst);
      }
      else if(refStart == string_adaptor::npos())
      {
        // not in a ref
      }
      else if(Arabica::XML::is_letter_or_digit(ch) || ch == S('#'))
      {
        // valid entity char
      }
      else if(ch == S(';'))
      {
        // properly terminated ref
        int ent = lookupEntity(string_adaptor::substr(dst, refStart, dst.size() - refStart - 1));
        if(ent != 0)
        {
          string_adaptor::resize(dst, refStart - 1);
          dst += S(unicode_encoder.to_bytes(ent));
        }
        refStart = string_adaptor::npos();
      }
      else 
      {
        // improperly terminated ref
        refStart = string_adaptor::npos();
      } // if ...
    } // for ...
    return string_adaptor::construct(dst);
  } // expandEntities

  virtual void entity(const string_type& buff)
  {
    entity_ = lookupEntity(buff);
  } // entity

  // Process numeric character references,
  // deferring to the schema for named ones.
  int lookupEntity(const string_type& buff)
  {
    int result = 0;
    if(string_adaptor::empty(buff))
      return result;

    if(buff[0] == S('#'))
    {
      const char *b = string_adaptor::asStdString(buff).c_str();
      if(string_adaptor::length(buff) > 1 && (buff[1] == S('x') || buff[1] == S('X')))
        return strtol(b + 2, NULL, 16);
      return strtol(b + 1, NULL, 10);
    }
    return schema_->getEntity(buff);
  } // lookupEntity

  virtual void eof(const string_type& /*buff*/)
  {
    if(virginStack) 
      rectify(pcdata_);
    while (stack_.next() != ElementT::Null)
    {
      pop();
    }
    if(!string_adaptor::empty(schema_->getURI()))
      contentHandler_->endPrefixMapping(schema_->getPrefix());
    contentHandler_->endDocument();
  } // eof

  virtual void etag(const string_type& buff)
  {
    // std::cerr << "etag(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    if(etag_cdata(buff))
      return;
    etag_basic(buff);
  } // etag

  bool etag_cdata(const string_type& buff)
  {
    string_type currentName = stack_.name();
    if (!CDATAElements || !(stack_.flags() & SchemaT::F_CDATA)) return false;
    // If this is a CDATA element and the tag doesn't match,
    // or isn't properly formed (junk after the name),
    // restart CDATA mode and process the tag as characters.
    if(string_adaptor::length(buff) ==
       string_adaptor::length(currentName)) {
      string_type buffl = lower_case(buff);
      string_type currentl = lower_case(currentName);
      if (std::equal(string_adaptor::begin(buffl), string_adaptor::end(buffl),
                     string_adaptor::begin(currentl))) return false;
    }
    contentHandler_->characters(S("</"));
    contentHandler_->characters(buff);
    contentHandler_->characters(S(">"));
    scanner_->startCDATA();
    return true;
  } // etag_cdata

  void etag_basic(const string_type& buff)
  {
    newElement_ = ElementT::Null;
    string_type name;
    if(!string_adaptor::empty(buff))
    {
      // Canonicalize case of name
      name = makeName(buff);
      ElementTypeT& type = schema_->getElementType(name);
      if(type == ElementTypeT::Null)
        return;  // mysterious end-tag
      name = type.name();
    }
    else
    {
      name = stack_.name();
    }

    ElementT sp = ElementT::Null;
    bool inNoforce = false;
    for (sp = stack_; sp != ElementT::Null; sp = sp.next())
    {
      if(sp.name() == name) 
        break;
      if((sp.flags() & SchemaT::F_NOFORCE) != 0)
        inNoforce = true;
    } // for ...

    if(sp == ElementT::Null)
      return;    // Ignore unknown etags
    if(sp.next() == ElementT::Null ||
       sp.next().next() == ElementT::Null)
      return;
    if(inNoforce) 
    {    // inside an F_NOFORCE element?
      sp.preclose();    // preclose the matching element
    }
    else 
    {      // restartably pop everything above us
      while (stack_ != sp) 
        restartablyPop();
      pop();
    }
    // pop any preclosed elements now at the top
    while (stack_.isPreclosed()) 
      pop();
    restart(ElementT::Null);
  } // etag_basic

  // Push restartables on the stack if possible
  // e is the next element to be started, if we know what it is
  void restart(ElementT e)
  {
    while (saved_ != ElementT::Null &&
           stack_.canContain(saved_) &&
           (e == ElementT::Null ||
            saved_.canContain(e)))
    {
      ElementT next = saved_.next();
      push(saved_);
      saved_ = next;
    } // while ...
  } // restart

  // Pop the stack irrevocably
  void pop() 
  {
    if(stack_ == ElementT::Null)
      return;    // empty stack
    string_type name = stack_.name();
    string_type localName = stack_.localName();
    string_type namespaceName = stack_.namespaceName();
    string_type prefix = prefixOf(name);

    if(!namespaces) 
      namespaceName = localName = string_adaptor::empty_string();
    contentHandler_->endElement(namespaceName,
                                localName,
                                name);
    if(foreign(prefix, namespaceName)) 
      contentHandler_->endPrefixMapping(prefix);

    const AttributesT& atts = stack_.atts();
    for (int i = atts.getLength() - 1; i >= 0; i--) 
    {
      string_type attNamespace = atts.getURI(i);
      string_type attPrefix = prefixOf(atts.getQName(i));
      if(foreign(attPrefix, attNamespace)) 
        contentHandler_->endPrefixMapping(attPrefix);
    } // for ...
    stack_ = stack_.next();
  } // pop

  // Pop the stack restartably
  void restartablyPop() 
  {
    ElementT popped = stack_;
    pop();
    if(restartElements && (popped.flags() & SchemaT::F_RESTART) != 0)
    {
      popped.anonymize();
      popped.setNext(saved_);
      saved_ = popped;
    } // if ...
  } // restartablyPop

  // Push element onto stack
  void push(ElementT e)
  {
    string_type name = e.name();
    string_type localName = e.localName();
    string_type namespaceName = e.namespaceName();
    string_type prefix = prefixOf(name);

    e.clean();
    if(!namespaces) 
      namespaceName = localName = string_adaptor::empty_string();
    if(virginStack && (lower_case(localName) == lower_case(doctypeName_))) 
      entityResolver_->resolveEntity(doctypePublicId_, doctypeSystemId_);
    if(foreign(prefix, namespaceName)) 
      contentHandler_->startPrefixMapping(prefix, namespaceName);
    
    AttributesImplT atts;
    int len = e.atts().getLength();
    for (int i = 0; i != len; ++i) 
    {
      string_type attNamespace = e.atts().getURI(i);
      string_type attPrefix = prefixOf(e.atts().getQName(i));
      if(foreign(attPrefix, attNamespace)) 
        contentHandler_->startPrefixMapping(attPrefix, attNamespace);

      atts.addAttribute(e.atts().getURI(i),
                        e.atts().getLocalName(i),
                        e.atts().getQName(i),
                        e.atts().getType(i),
                        e.atts().getValue(i));
    } // for ...
    contentHandler_->startElement(namespaceName, localName, name, atts);

    e.setNext(stack_);
    stack_ = e;
    virginStack = false;
    if(CDATAElements && (stack_.flags() & SchemaT::F_CDATA) != 0)
      scanner_->startCDATA();
  } // push 

  // Get the prefix from a QName
  string_type prefixOf(const string_type &name)
  {
    size_t i = string_adaptor::find(name, S(':'));
    string_type prefix;
    if(i != string_adaptor::npos())
      prefix = string_adaptor::substr(name, 0, i);
    return prefix;
  } // prefixOf

  // Return true if we have a foreign name
  bool foreign(const string_type &prefix, const string_type &namespaceName)
  {
    return !(
        string_adaptor::empty(prefix) || string_adaptor::empty(namespaceName) ||
        namespaceName == schema_->getURI());
  } // foreign

  /**
   * Parsing the complete XML Document Type Definition is way too complex,
   * but for many simple cases we can extract something useful from it.
   *
   * doctypedecl  ::= '<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)? '>'
   *  DeclSep     ::= PEReference | S
   *  intSubset   ::= (markupdecl | DeclSep)*
   *  markupdecl  ::= elementdecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment
   *  ExternalID  ::= 'SYSTEM' S SystemLiteral | 'PUBLIC' S PubidLiteral S SystemLiteral
   */

  virtual void decl(const string_type& buff)
  {
    // std::cerr << "decl(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    string_type name;
    string_type systemid;
    string_type publicid;
    std::vector<string_type> v = split(buff);
    if(v.size() > 0 && S("DOCTYPE") == v[0])
    {
      if(doctypeIsPresent_) 
        return;    // one doctype only!
      doctypeIsPresent_ = true;
      if(v.size() > 1) 
      {
        name = v[1];
        if(v.size()>3 && S("SYSTEM") == v[2])
        {
          systemid = v[3];
        }
        else if(v.size() > 3 && S("PUBLIC") == v[2])
        {
          publicid = v[3];
          if(v.size() > 4) 
          {
            systemid = v[4];
          }
        }
      }
    }
    publicid = trimquotes(publicid);
    systemid = trimquotes(systemid);
    if(!string_adaptor::empty(name))
    {
      publicid = cleanPublicid(publicid);
      lexicalHandler_->startDTD(name, publicid, systemid);
      lexicalHandler_->endDTD();
      doctypeName_ = name;
      doctypePublicId_ = publicid;
      LocatorT* document_locator = dynamic_cast<LocatorT*>(scanner_.get());
      if(document_locator)
      {    // Must resolve systemid
        doctypeSystemId_  = document_locator->getSystemId();
        doctypeSystemId_ = string_adaptor::construct_from_utf8(
            Arabica::io::URI(
                string_adaptor::asStdString(doctypeSystemId_),
                string_adaptor::asStdString(systemid)).as_string().c_str());
      } // if ...
    } // if ...
  } // decl

  // If the String is quoted, trim the quotes.
  static string_type trimquotes(const string_type& in)
  {
    size_t length = string_adaptor::length(in);
    if(length == 0) 
      return in;
    char_type s = in[0], e = in[length - 1];
    if(s == e && (s == S('\'') || s == S('"')))
      return string_adaptor::substr(in, 1, length - 1);
    return in;
  } // trimquotes

  // Split the supplied String into words or phrases seperated by spaces.
  // Recognises quotes around a phrase and doesn't split it.
  static std::vector<string_type> split(const string_type& val)
  {
    std::vector<string_type> splits;

    string_type v = Arabica::text::normalize_whitespace<string_type, string_adaptor>(val);
    if(string_adaptor::empty(v))
    {
      splits.push_back(v);
      return splits;
    }

    size_t s = 0;
    size_t e = 0;
    bool sq = false;  // single quote
    bool dq = false;  // double quote
    char_type lastc = 0;
    size_t len = string_adaptor::length(v);
    for(e=0; e < len; ++e) 
    {
      char_type c = v[e];
      if(!dq && c == S('\'') && lastc != S('\\'))
      {
			  sq = !sq;
        if(s == string_adaptor::npos())
          s = e;
      }
      else if(!sq && c == S('\"') && lastc != S('\\'))
      {
			  dq = !dq;
			  if(s == string_adaptor::npos())
          s = e;
      }
      else if(!sq && !dq) 
      {
        if(is_space(c))
        {
          splits.push_back(string_adaptor::substr(v, s, e));
				  s = string_adaptor::npos();
				}
        else if(s == string_adaptor::npos() && c != S(' '))
        {
          s = e;
        } 
      }
      lastc = c;
    } // for ...
    splits.push_back(string_adaptor::substr(v, s, e));

    return splits;
  } // split

  // Replace junk in publicids with spaces
  string_type cleanPublicid(const string_type& src)
  {
    string_type dst;
    bool suppressSpace = true;
    for(typename string_type::const_iterator i = string_adaptor::begin(src),
        ie = string_adaptor::end(src); i != ie; ++i)
    {
      if(string_adaptor::find(legal, *i) != string_adaptor::npos())
      {   
        // legal but not whitespace
        string_adaptor::append(dst, *i);
        suppressSpace = false;
      }
      else if(suppressSpace) 
      {  // normalizable whitespace or junk
        ;
      }
      else 
      {
        string_adaptor::append(dst, S(' '));
        suppressSpace = true;
      }
    }
    return dst;
  } // cleanPublicId

  virtual void gi(const string_type& buff)
  {
    // std::cerr << "gi(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    if(newElement_ != ElementT::Null)
      return;
    string_type name = makeName(buff);
    if(string_adaptor::empty(name))
      return;
    ElementTypeT* type = &schema_->getElementType(name);
    if(*type == ElementTypeT::Null)
    {
      // Suppress unknown elements if ignore-bogons is on
      if(ignoreBogons) 
        return;
      int bogonModel = bogonsEmpty ? SchemaT::M_EMPTY : SchemaT::M_ANY;
      int bogonMemberOf = rootBogons ? SchemaT::M_ANY : (SchemaT::M_ANY & ~SchemaT::M_ROOT);
      schema_->elementType(name, bogonModel, bogonMemberOf, 0);
      if(!rootBogons) 
        schema_->parent(name, schema_->rootElementType().name());
      type = &schema_->getElementType(name);
    } // if ...

    newElement_ = ElementT(*type, defaultAttributes);
  } // gi

  virtual void cdsect(const string_type& buff)
  {
    // std::cerr << "cdsect(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    lexicalHandler_->startCDATA();
    pcdata(buff);
    lexicalHandler_->endCDATA();
  } // cdsect
  
  virtual void pcdata(const string_type& buff)
  {
    // std::cerr << "pcdata(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    if(string_adaptor::empty(buff))
      return;
    bool allWhite = true;
    for (typename string_type::const_iterator i = buff.begin(), ie = buff.end(); i != ie; ++i)
    {
      if(!is_space(*i))
        allWhite = false;
    } // for ...
    if(allWhite && !stack_.canContain(pcdata_)) 
    {
      if(ignorableWhitespace) 
        contentHandler_->ignorableWhitespace(buff);
    }
    else 
    {
      rectify(pcdata_);
      contentHandler_->characters(buff);
    } // if ...
  } // pcdata

  virtual void pitarget(const string_type& buff)
  {
    // std::cerr << "pitarget(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    if(newElement_ != ElementT::Null)
      return;
    string_type name = makeName(buff);
    size_t colon = string_adaptor::find(name, S(':'));
    while(colon != string_adaptor::npos())
    {
      name[colon] = S('_');
      colon = string_adaptor::find(name, S(':'));
    } // while
    piTarget_ = name;
  } // pitarget

  virtual void pi(const string_type& buff)
  {
    // std::cerr << "pi(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    if(newElement_ != ElementT::Null ||
       string_adaptor::empty(piTarget_))
      return;
    if(S("xml") == lower_case(piTarget_))
      return;
    size_t length = string_adaptor::length(buff);
    if((length > 0) && (buff[length - 1] == S('?')))
      length--;  // remove trailing ?
    contentHandler_->processingInstruction(piTarget_,
                                           string_adaptor::substr(buff, 0, length));
    piTarget_ = string_adaptor::empty_string();
  } // pi

  virtual void stagc(const string_type& buff)
  {
    // std::cerr << "stagc(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    if(newElement_ == ElementT::Null)
      return;
    rectify(newElement_);
    if(stack_.model() == SchemaT::M_EMPTY)
    {
      // Force an immediate end tag
      etag_basic(buff);
    } // if ...
  } // stagc

  virtual void stage(const string_type& buff)
  {
    // std::cerr << "stage(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    if(newElement_ == ElementT::Null)
      return;
    rectify(newElement_);
    // Force an immediate end tag
    etag_basic(buff);
  } // stage

  // Comment buffer is twice the size of the output buffer
  virtual void cmnt(const string_type& buff)
  {
    // std::cerr << "cmnt(\"" << buff.substr(offset, length) << "\", " << offset << ", " << length << ")" << std::endl;
    lexicalHandler_->comment(buff);
  } // cmnt

  // Rectify the stack, pushing and popping as needed
  // so that the argument can be safely pushed
  void rectify(ElementT e)
  {
    ElementT sp = ElementT::Null;
    while (true) 
    {
      for (sp = stack_; sp != ElementT::Null; sp = sp.next())
      {
        if(sp.canContain(e)) 
          break;
      } // for ...
      if(sp != ElementT::Null)
        break;
      ElementTypeT& parentType = e.parent();
      if(parentType == ElementTypeT::Null)
        break;
      ElementT parent = ElementT(parentType, defaultAttributes);
      parent.setNext(e);
      e = parent;
    } // while ...
    if(sp == ElementT::Null)
      return;    // don't know what to do
    while (stack_ != sp) 
    {
      if(stack_ == ElementT::Null ||
         stack_.next() == ElementT::Null ||
         stack_.next().next() == ElementT::Null)
        break;
      restartablyPop();
    } // while ...
    while (e != ElementT::Null)
    {
      ElementT nexte = e.next();
      if(e.name() != S("<pcdata>"))
        push(e);
      e = nexte;
      restart(e);
    } // while ...
    newElement_ = ElementT::Null;
  } // rectify

  virtual int getEntity() 
  {
    return entity_;
  } // getEntity

  // Return the argument as a valid XML name
  // This no longer lowercases the result: we depend on Schema to
  // canonicalize case.
  string_type makeName(const string_type& buff)
  {
    string_type dst;
    bool seenColon = false;
    bool start = true;
//    String src = new String(buff, offset, length); // DEBUG
    for(auto ch = string_adaptor::begin(buff), che = string_adaptor::end(buff);
        ch != che; ++ch)
    {
      if(is_letter(*ch) || *ch == S('_'))
      {
        start = false;
        string_adaptor::append(dst, *ch);
      }
      else if(is_digit(*ch) || *ch == S('-') || *ch == S('.'))
      {
        if(start) 
          string_adaptor::append(dst, S('_'));
        start = false;
        string_adaptor::append(dst, *ch);
      }
      else if(*ch == S(':') && !seenColon)
      {
        seenColon = true;
        if(start) 
          string_adaptor::append(dst, S('_'));
        start = true;
        string_adaptor::append(dst, translateColons ? S('_') : *ch);
      } 
    } // for ...
    size_t dstLength = string_adaptor::length(dst);
    if(dstLength == 0 || dst[dstLength - 1] == S(':'))
      string_adaptor::append(dst, S('_'));
    return dst;
  } // makeName

  static string_type lower_case(const string_type& str)
  {
    std::wstring lower = string_adaptor::asStdWString(str);
    std::transform(lower.begin(), lower.end(), lower.begin(), std::towlower);
    return string_adaptor::construct_from_utf16(lower.c_str(), lower.size());
  } // lower_case

  void reportError(const std::string& message, bool fatal)
  {
    SAXParseException<string_type, string_adaptor> e(
        message, S("<public-id>"), S("<system-id>"), -1, -1);
    if(fatal)
      errorHandler_->fatalError(e);
    else
      errorHandler_->error(e);
  } // reportError
}; // class Taggle

template <class string_type,
          class string_adaptor = Arabica::default_string_adaptor<string_type> >
class Taggle : public TaggleBase<string_type, string_adaptor> {
private:
  typedef typename string_type::value_type char_type;
  typedef std::wstring_convert<std::codecvt_utf8<char_type>, char_type>
      UnicodeEncoder;
  virtual void convert_stream(
      std::istream& in, std::basic_stringstream<char_type> &out) {
    std::stringstream byte_stream;
    byte_stream << in.rdbuf();
    out << UnicodeEncoder().from_bytes(byte_stream.str());
  }
};

template <class string_adaptor>
class Taggle<std::string, string_adaptor> : public TaggleBase<std::string, string_adaptor> {
private:
  virtual void convert_stream(
      std::istream& in, std::basic_stringstream<char> &out) {
    out << in.rdbuf();
  }
};

template<class string_type, class string_adaptor>
bool TaggleBase<string_type, string_adaptor>::DEFAULT_NAMESPACES = true;
template<class string_type, class string_adaptor>
bool TaggleBase<string_type, string_adaptor>::DEFAULT_IGNORE_BOGONS = false;
template<class string_type, class string_adaptor>
bool TaggleBase<string_type, string_adaptor>::DEFAULT_BOGONS_EMPTY = false;
template<class string_type, class string_adaptor>
bool TaggleBase<string_type, string_adaptor>::DEFAULT_ROOT_BOGONS = true;
template<class string_type, class string_adaptor>
bool TaggleBase<string_type, string_adaptor>::DEFAULT_DEFAULT_ATTRIBUTES = true;
template<class string_type, class string_adaptor>
bool TaggleBase<string_type, string_adaptor>::DEFAULT_TRANSLATE_COLONS = false;
template<class string_type, class string_adaptor>
bool TaggleBase<string_type, string_adaptor>::DEFAULT_RESTART_ELEMENTS = true;
template<class string_type, class string_adaptor>
bool TaggleBase<string_type, string_adaptor>::DEFAULT_IGNORABLE_WHITESPACE = false;
template<class string_type, class string_adaptor>
bool TaggleBase<string_type, string_adaptor>::DEFAULT_CDATA_ELEMENTS = true;

template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::namespacesFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/namespaces");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::namespacePrefixesFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/namespace-prefixes");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::externalGeneralEntitiesFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/external-general-entities");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::externalParameterEntitiesFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/external-parameter-entities");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::isStandaloneFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/is-standalone");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::lexicalHandlerParameterEntitiesFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/lexical-handler/parameter-entities");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::resolveDTDURIsFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/resolve-dtd-uris");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::stringInterningFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/string-interning");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::useAttributes2Feature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/use-attributes2");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::useLocator2Feature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/use-locator2");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::useEntityResolver2Feature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/use-entity-resolver2");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::validationFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/validation");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::unicodeNormalizationCheckingFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/unicode-normalization-checking");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::xmlnsURIsFeature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/xmlns-uris");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::XML11Feature = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/features/xml-1.1");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::ignoreBogonsFeature = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/features/ignore-bogons");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::bogonsEmptyFeature = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/features/bogons-empty");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::rootBogonsFeature = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/features/root-bogons");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::defaultAttributesFeature = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/features/default-attributes");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::translateColonsFeature = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/features/translate-colons");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::restartElementsFeature = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/features/restart-elements");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::ignorableWhitespaceFeature = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/features/ignorable-whitespace");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::CDATAElementsFeature = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/features/cdata-elements");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::lexicalHandlerProperty = TaggleBase<string_type, string_adaptor>::S("http://xml.org/sax/properties/lexical-handler");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::scannerProperty = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/properties/scanner");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::schemaProperty = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/properties/schema");
template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::autoDetectorProperty = TaggleBase<string_type, string_adaptor>::S("http://www.ccil.org/~cowan/tagsoup/properties/auto-detector");

template<class string_type, class string_adaptor>
const string_type TaggleBase<string_type, string_adaptor>::legal =
  TaggleBase<string_type, string_adaptor>::S("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-'()+,./:=?;!*#@$_%");

} // namespace SAX

} // namespace Arabica
#endif

