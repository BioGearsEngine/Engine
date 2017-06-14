#include <memory> // std::auto_ptr
#include <iostream>

#include "test.hxx"

using namespace std;
using namespace test;

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " test.xml" << endl;
    return 1;
  }

  try
  {
    auto_ptr<type> r (root (argv[1]));

    for (type::a_iterator i (r->a ().begin ()); i != r->a ().end (); ++i)
    {
      cerr << *i << endl;
    }

    for (type::b_iterator i (r->b ().begin ()); i != r->b ().end (); ++i)
    {
      cerr << *i << endl;
    }

    xml_schema::namespace_infomap map;
    map["t"].name = "test";
    map["t"].schema = "test.xsd";

    root (cout, *r, map);
  }
  catch (xml_schema::exception const& e)
  {
    cerr << e << endl;
    return 1;
  }
}
