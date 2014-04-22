#ifndef ARABICA_UTILS_STRING_ADAPTOR_H
#define ARABICA_UTILS_STRING_ADAPTOR_H

#include <codecvt>
#include <locale>
#include <string>
#include <SAX/ArabicaConfig.hpp>
#include <io/convertstream.hpp>
#include <convert/utf8ucs2codecvt.hpp>
#include <Arabica/stringadaptortag.hpp>

namespace Arabica
{

template<class stringT> class default_string_adaptor;

template<class stringT>
class default_string_adaptor_base
{
public:
  typedef stringT string_type;
  typedef typename string_type::const_iterator const_iterator;
  typedef typename string_type::iterator mutable_iterator;
  typedef typename string_type::const_reverse_iterator const_reverse_iterator;
  typedef typename string_type::iterator iterator;
  typedef typename string_type::value_type value_type;
  typedef typename string_type::size_type size_type;
  typedef std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>
      UnicodeEncoder;

  virtual ~default_string_adaptor_base() {}

  static size_type npos()
  {
    return static_cast<size_type>(-1);
  }

  //todo: is this safe?
  template<class InputIterator>
  static inline string_type construct(InputIterator from, InputIterator to)
  {
    return string_type(from, to);
  }

  static inline string_type construct(const_iterator from, const_iterator to)
  {
    return string_type(from, to);
  }

  static inline string_type construct(const std::basic_string<value_type>& str)
  {
    return construct(str.begin(), str.end());
  } // construct

  static string_type construct(const value_type* str)
  {
    return str ? string_type(str) : string_type();
  }

  static const string_type& empty_string() { static string_type es; return es; }

  //todo: fix for utf8
  static size_type length(const string_type& str) { return str.length(); }

  // all these functions should operate as std::string member functions do
  static size_type capacity(string_type& str) { return str.capacity(); }
  static void clear(string_type& str) { str.clear(); }
  static bool empty(const string_type& str) { return str.empty(); }
  static string_type &erase(string_type& str, size_type pos) { return str.erase(pos); }
  static string_type &erase(string_type& str, size_type pos, size_type len) { return str.erase(pos, len); }
  static size_type find(const string_type& str, value_type what) { return str.find(what); }
  static size_type find(const string_type& str, const string_type& what) { return str.find(what); }
  static size_type find(const string_type& str, value_type what, size_type from) { return str.find(what, from); }
  static size_type find(const string_type& str, const string_type& what, size_type from) { return str.find(what, from); }
  static string_type substr(const string_type& str, const size_type& offset) { return str.substr(offset); }
  static string_type substr(const string_type& str, const size_type& offset, const size_type& count) { return str.substr(offset, count); }
  static void append(string_type& str, const string_type& a) { str.append(a); }
  static void append(string_type& str, const value_type& a) { str += a; }
  static string_type concat(const string_type& str, const string_type& a) { return str + a; }
  static string_type concat(const string_type& str, const value_type& a) { return str + a; }
  static void insert(string_type& str, size_type offset, const string_type& a) { str.insert(offset, a); }
  static void replace(string_type& str, size_type offset, size_type count, const string_type& a) { str.replace(offset, count, a); }
  static void reserve(string_type& str, size_type n) { str.reserve(n); }
  static void resize(string_type& str, size_type n) { str.resize(n); }

  static const_iterator begin(const string_type& str) { return str.begin(); }
  static const_iterator end(const string_type& str) { return str.end(); }

  static iterator begin(string_type& str) { return str.begin(); }
  static iterator end(string_type& str) { return str.end(); }

  static const_reverse_iterator rbegin(const string_type& str) { return str.rbegin(); }

  // only used to constuct error strings - don't have to be highly efficient!
  static std::string asStdString(const string_type& str);

#ifndef ARABICA_NO_WCHAR_T
  static string_type construct_from_utf16(const wchar_t* str);
  static string_type construct_from_utf16(const wchar_t* str, int length);
  static std::wstring asStdWString(const string_type& str);

  typedef Arabica::io::basic_iconvertstream<wchar_t, std::char_traits<wchar_t>,
                               char, std::char_traits<char> > widener_t;
  typedef Arabica::io::basic_oconvertstream<wchar_t, std::char_traits<wchar_t>,
                               char, std::char_traits<char> > narrower_t;


  static const std::locale& utf8ucs2_locale()
  {
    static const std::locale loc = std::locale(std::locale(), new Arabica::convert::utf8ucs2codecvt);
    return loc;
  }
#endif //ARABICA_NO_WCHAR_T

}; // class default_string_adaptor_base


// specialize for std::string and std::wstring
template<>
class default_string_adaptor<std::string> :
  public string_adaptor_tag,
  public default_string_adaptor_base<std::string>
{
public:

  static char convert_from_utf8(char c) { return c; }

  static std::string construct_from_utf8(const char* str)
  {
    return str ? std::string(str) : std::string();
  } // construct_from_utf8

  static std::string construct_from_utf8(const char* str, int length)
  {
    return std::string(str, length);
  } // construct_from_utf8

  static const std::string& asStdString(const std::string& str)
  {
    return str;
  } // asStdString

#ifndef ARABICA_NO_WCHAR_T
  static std::string construct_from_utf16(const wchar_t* str)
  {
    UnicodeEncoder unicode_encoder;
    return unicode_encoder.to_bytes(str);
  }

  static std::string construct_from_utf16(const wchar_t* str, int length)
  {
    UnicodeEncoder unicode_encoder;
    return unicode_encoder.to_bytes(std::wstring(str, length));
  } // construct_from_utf16

  static std::wstring asStdWString(const std::string& str)
  {
    UnicodeEncoder unicode_encoder;
    return unicode_encoder.from_bytes(str);
  } // toStdWString
#endif //ARABICA_NO_WCHAR_T

}; // class default_string_adaptor<std::string>


#ifndef ARABICA_NO_WCHAR_T

template<>
class default_string_adaptor<std::wstring> :
  public string_adaptor_tag,
  public default_string_adaptor_base<std::wstring>
{
public:

  static wchar_t makeValueT(char c)
  {
    return static_cast<wchar_t>(c);
  } // makeValueT

  template<class InputIterator>
  static inline string_type construct(InputIterator from, InputIterator to)
  {
    return string_type(from, to);
  }

  static inline string_type construct(const_iterator from, const_iterator to)
  {
    return string_type(from, to);
  }

  static inline string_type construct(const std::basic_string<value_type>& str)
  {
    return construct(str.begin(), str.end());
  } // construct

  static string_type construct(const value_type* str)
  {
    return str ? string_type(str) : string_type();
  }

  static std::wstring construct(const std::string& str)
  {
    return construct_from_utf8(str.c_str());
  } 

  static std::wstring construct_from_utf8(const char* str)
  {
    UnicodeEncoder unicode_encoder;
    return unicode_encoder.from_bytes(str);
  }

  static std::wstring construct_from_utf8(const char* str, int length)
  {
    UnicodeEncoder unicode_encoder;
    return unicode_encoder.from_bytes(std::string(str, length));
  }

  static std::wstring construct_from_utf16(const wchar_t* str)
  {
    return str ? std::wstring(str) : std::wstring();
  }

  static std::wstring construct_from_utf16(const wchar_t* str, int length)
  {
    return std::wstring(str, length);
  }

  static std::string asStdString(const std::wstring& str)
  {
    UnicodeEncoder unicode_encoder;
    return unicode_encoder.to_bytes(str);
  } // toStdString

  static const std::wstring& asStdWString(const std::wstring& str)
  {
    return str;
  } // toStdWString

}; // class default_string_adaptor<std::wstring>


#endif // ARABICA_NO_WCHAR_T

} // namespace Arabica

#endif
// end of file
