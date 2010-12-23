
#ifdef _MSC_VER
#pragma warning(disable: 4786 4250 4503 4244)
#endif

#include "dom_test_suite.hpp"
#include "../silly_string/silly_string.hpp"

////////////////////////////////////////////////
int main(int argc, const char* argv[])
{
  DOM_test_suite<silly_string, silly_string_adaptor>(argc, argv);
  return 77;
} // main

// end of file
