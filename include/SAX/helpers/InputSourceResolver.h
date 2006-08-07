#ifndef ARABICA_INPUT_SOURCE_RESOLVER_H
#define ARABICA_INPUT_SOURCE_RESOLVER_H

#include <SAX/ArabicaConfig.h>
#include <iosfwd>
#include <map>
#include <SAX/InputSource.h>

class InputSourceResolver
{
public:
  InputSourceResolver(const SAX::InputSource& inputSource);

  template<class stringT, class stringAdaptorT>
  InputSourceResolver(const SAX::basic_InputSource<stringT>& inputSource,
                      const stringAdaptorT& SA) :
    deleteStream_(false),
    byteStream_(0)
  {
    open(SA.asStdString(inputSource.getPublicId()),
         SA.asStdString(inputSource.getSystemId()),
         inputSource.getByteStream());
  } // InputSourceResolver
  ~InputSourceResolver();

  std::istream* resolve() const { return byteStream_; }

  typedef std::istream* (*URIResolver)(const std::string& url);
  static bool registerResolver(const std::string& method, URIResolver resolver);
  static bool unRegisterResolver(const std::string& method);

private:
  // no impl
  InputSourceResolver(const InputSourceResolver&);
  InputSourceResolver& operator=(const InputSourceResolver&);
  bool operator==(const InputSourceResolver&);

  // instance variables
  bool deleteStream_;
  std::istream* byteStream_;

  void open(const std::string& publicId, 
            const std::string& systemId,
            std::istream* byteStream);

  // class variables
  static URIResolver findResolver(std::string method);

  typedef std::map<std::string, URIResolver> resolverMapT;
  static resolverMapT& resolverMap()
  {
    static resolverMapT theMap;
    return theMap;
  } // resolverMap
}; // class InputSourceResolver

#endif
