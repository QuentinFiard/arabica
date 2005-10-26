#ifndef test_SAX_H
#define test_SAX_H

#include "../CppUnit/framework/TestCase.h"
#include "../CppUnit/framework/TestSuite.h"
#include "../CppUnit/framework/TestCaller.h"
#include <sstream>
#include <DOM/SAX2DOM/SAX2DOM.h>

template<class string_type, class string_adaptor>
class SAXTest : public TestCase 
{
  public: 
    SAXTest(std::string name) :
        TestCase(name) 
    {
    } // SAXTest
    
    void setUp() 
    {
    } // setUp

    DOM::Document<string_type> parse(string_type str)
    {
      std::stringstream ss;
      ss << str;

      SAX::InputSource is(ss);
      SAX2DOM::Parser<string_type> parser;
      parser.parse(is);       
      return parser.getDocument();
    } // parse

    void test1() 
    {
      DOM::Element<string_type> d;
      DOM::Node<string_type> n;
      assert(d == 0);
      assert(n == 0);
      assert(n == d);
    } // test1

    void test2()
    {
      DOM::Document<string_type> d = parse("<root/>");
      DOM::Element<string_type> elem = d.getDocumentElement();
      assert(elem.getParentNode() == d);
      assert(elem.getOwnerDocument() == d);
    } // test2

    void test3()
    {
      DOM::Document<string_type> d = parse("<root attr=\"poop\"/>");
      DOM::Element<string_type> elem = d.getDocumentElement();
      assert(elem.hasAttributes() == true);
      assert(elem.getAttribute("attr") == "poop");
    } // test3

    void test4()
    {
      DOM::Document<string_type> d = parse("<root><child attr=\"poop\"/></root>");
      DOM::Element<string_type> elem = DOM::Element<string_type>(d.getDocumentElement().getFirstChild());
      assertEquals("child", elem.getNodeName());
      assertEquals(true, elem.hasAttributes());
      assertEquals("poop", elem.getAttribute("attr"));
    } // test4

    void test5()
    {
      DOM::Document<string_type> d = parse("<stuff:elem attr='something' xmlns:stuff='http://example.com/stuff'/>");
      DOM::Element<string_type> elem = d.getDocumentElement();
      assertEquals(true, elem.hasNamespaceURI());
      assertEquals("http://example.com/stuff", elem.getNamespaceURI());

      DOM::Attr<string_type> attr = elem.getAttributeNode("attr");
      assertEquals(false, attr.hasNamespaceURI());
    } // test5

    void test6()
    {
      DOM::Document<string_type> d = parse("<stuff:elem stuff:attr='something' xmlns:stuff='http://example.com/stuff'/>");
      DOM::Element<string_type> elem = d.getDocumentElement();
      assertEquals(true, elem.hasNamespaceURI());
      assertEquals("http://example.com/stuff", elem.getNamespaceURI());

      DOM::Attr<string_type> attr = elem.getAttributeNodeNS("http://example.com/stuff", "attr");
      assertEquals(true, attr.hasNamespaceURI());
      assertEquals("http://example.com/stuff", attr.getNamespaceURI());
    } // test6

    void test7()
    {
      DOM::Document<string_type> d = parse("<elem stuff:attr='something' xmlns:stuff='http://example.com/stuff'/>");
      DOM::Element<string_type> elem = d.getDocumentElement();
      assertEquals(false, elem.hasNamespaceURI());

      DOM::Attr<string_type> attr = elem.getAttributeNodeNS("http://example.com/stuff", "attr");
      assertEquals(true, attr.hasNamespaceURI());
      assertEquals("http://example.com/stuff", attr.getNamespaceURI());
    } // test7

    void test8()
    {
      DOM::Document<string_type> d = parse("<root attr=\"poop\"><child/></root>");
      DOM::Element<string_type> elem = d.getDocumentElement();

      DOM::Element<string_type> e2 = DOM::Element<string_type>(elem.cloneNode(true));
      assert(e2.getOwnerDocument() == d);
      assert(e2.getParentNode() == 0);
      assert(e2.hasAttributes() == true);
      assert(e2.getAttribute("attr") == "poop");
      assert(e2.getFirstChild().getNodeName() == "child");
    } // test8

    void test9()
    {
      DOM::Document<string_type> d = parse("<elem attr='something' xmlns:stuff='http://example.com/stuff'/>");
      DOM::Element<string_type> elem = d.getDocumentElement();
      assertEquals(false, elem.hasNamespaceURI());

      DOM::Attr<string_type> attr = elem.getAttributeNode("attr");
      assertEquals(false, attr.hasNamespaceURI());
    } // test9

    void test10()
    {
      DOM::Document<string_type> d = parse("<elem stuff:attr='something' poop:attr='fail' xmlns:stuff='http://example.com/stuff'/>");
      assert(d == 0);
    } // test10

    void test11()
    {
      SAX2DOM::Parser<string_type> parser;
      assert(parser.getFeature("http://xml.org/sax/features/validation") == true);
      parser.setFeature("http://xml.org/sax/features/validation", false);
      assert(parser.getFeature("http://xml.org/sax/features/validation") == false);

      parser.parse("<root attr=\"poop\"><child/></root>");
    } // test11

    void test12()
    {
      SAX2DOM::Parser<string_type> parser;
      try
      {
        parser.getFeature("made up name");
        assert(false);
      }
      catch(const SAX::SAXNotRecognizedException&)
      {
      } 
    } // test12
};

template<class string_type, class string_adaptor>
TestSuite* SAXTest_suite() 
{
  TestSuite *suiteOfTests = new TestSuite;
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test1", &SAXTest<string_type, string_adaptor>::test1));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test2", &SAXTest<string_type, string_adaptor>::test2));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test3", &SAXTest<string_type, string_adaptor>::test3));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test4", &SAXTest<string_type, string_adaptor>::test4));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test5", &SAXTest<string_type, string_adaptor>::test5));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test6", &SAXTest<string_type, string_adaptor>::test6));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test7", &SAXTest<string_type, string_adaptor>::test7));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test8", &SAXTest<string_type, string_adaptor>::test8));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test9", &SAXTest<string_type, string_adaptor>::test9));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test10", &SAXTest<string_type, string_adaptor>::test10));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test11", &SAXTest<string_type, string_adaptor>::test11));
  suiteOfTests->addTest(new TestCaller<SAXTest<string_type, string_adaptor> >("test12", &SAXTest<string_type, string_adaptor>::test12));
  return suiteOfTests;
} // SAXTest_suite

#endif

