#ifndef ARABICA_TEST_TAGGLE_HPP
#define ARABICA_TEST_TAGGLE_HPP

#include <memory>
#include <sstream>
#include <iostream>

#include <SAX/filter/Writer.hpp>
#include <Taggle/Taggle.hpp>

#include "../CppUnit/framework/TestCase.h"
#include "../CppUnit/framework/TestSuite.h"
#include "../CppUnit/framework/TestCaller.h"

class TaggleTest : public TestCase 
{
  public: 
    TaggleTest(std::string name) :
        TestCase(name) 
    {
    } // TaggleTest
    
    void setUp() 
    {
    } // setUp

    void senseTest() 
    {
      Arabica::SAX::Taggle<std::string> parser;
      std::ostringstream sink;
      Arabica::SAX::Writer<std::string> writer(sink, parser);

      writer.parse(*source<std::string>("<html><body>woo!<br></body></html>"));
      assertEquals("<?xml version=\"1.0\"?>\n<html xmlns:html=\"http://www.w3.org/1999/xhtml\">\n  <body>woo!\n    <br clear=\"none\"/>\n  </body>\n</html>\n", sink.str());
    } // senseTest

    void unicodeTest()
    {
      Arabica::SAX::Taggle<std::wstring> parser;
      std::wostringstream sink;
      Arabica::SAX::Writer<std::wstring> writer(sink, parser);

      writer.parse(*source<std::wstring>("<html><body>收藏品</body></html>"));
      std::wcout << sink.str();
      assertTrue(L"<?xml version=\"1.0\"?>\n<html xmlns:html=\"http://www.w3.org/1999/xhtml\">\n  <body>收藏品</body>\n</html>\n" == sink.str());
    } // senseTest

  private:
    template <class string_type>
    std::auto_ptr<Arabica::SAX::InputSource<string_type> > source(const std::string& str)
    {
      std::auto_ptr<std::iostream> ss(new std::stringstream());
      (*ss) << str;
      return std::auto_ptr<Arabica::SAX::InputSource<string_type> >(new Arabica::SAX::InputSource<string_type>(ss));
    } // source
}; // TaggleTest

TestSuite* Taggle_test_suite() 
{
  TestSuite *suiteOfTests = new TestSuite;

  suiteOfTests->addTest(new TestCaller<TaggleTest>("senseTest", &TaggleTest::senseTest));
  suiteOfTests->addTest(new TestCaller<TaggleTest>("unicodeTest", &TaggleTest::unicodeTest));

  return suiteOfTests;
} // TaggleTest_suite

#endif

