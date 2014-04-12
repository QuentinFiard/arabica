#ifndef ARABICA_DELETABLE_FACET_HPP
#define ARABICA_DELETABLE_FACET_HPP

namespace Arabica
{
namespace convert
{

// Utility wrapper to adapt locale-bound facets for wstring/wbuffer convert.
// Source: http://en.cppreference.com/w/cpp/locale/codecvt.
template<class Facet>
class deletable_facet : public Facet
{
public:
  template<class ...Args>
  deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
  ~deletable_facet() {}
}; // class deletable_facet

} // namespace convert
} // namespace Arabica

#endif
