// file      : examples/cxx/tree/order/mixed/driver.cxx
// copyright : not copyrighted - public domain

#include <memory>   // std::auto_ptr
#include <cassert>
#include <iostream>

#include "text.hxx"

using std::cerr;
using std::endl;

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " text.xml" << endl;
    return 1;
  }

  try
  {
    std::auto_ptr<text> t (text_ (argv[1]));

    // Print what we've got in content order.
    //
    for (text::content_order_const_iterator i (t->content_order ().begin ());
         i != t->content_order ().end ();
         ++i)
    {
      switch (i->id)
      {
      case text::a_id:
        {
          const anchor& a (t->a ()[i->index]);
          cerr << a << "[" << a.href () << "]";
          break;
        }
      case text::text_content_id:
        {
          const xml_schema::string& s (t->text_content ()[i->index]);
          cerr << s;
          break;
        }
      default:
        {
          assert (false); // Unknown content id.
        }
      }
    }

    cerr << endl;

    // Modify the document. Note that we have to update both the content
    // itself and content order sequences.
    //
    typedef text::content_order_type order_type;

    text::content_order_sequence& co (t->content_order ());
    text::text_content_sequence& tc (t->text_content ());

    tc.push_back ("The last paragraph doesn't talk about ");
    co.push_back (order_type (text::text_content_id, tc.size () - 1));

    t->a ().push_back (anchor ("anything", "http://en.wikipedia.org"));
    co.push_back (order_type (text::a_id, t->a ().size () - 1));

    tc.push_back (" in particular.\n\n");
    co.push_back (order_type (text::text_content_id, tc.size () - 1));

    // Serialize the modified document back to XML.
    //
    xml_schema::namespace_infomap map;

    map[""].schema = "text.xsd";

    text_ (std::cout,
           *t,
           map,
           "UTF-8",
           xml_schema::flags::dont_pretty_print);
  }
  catch (const xml_schema::exception& e)
  {
    cerr << e << endl;
    return 1;
  }
}
