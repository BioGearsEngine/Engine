// file      : examples/cxx/tree/order/element/driver.cxx
// copyright : not copyrighted - public domain

#include <memory>   // std::auto_ptr
#include <cassert>
#include <iostream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "transactions.hxx"

// The following string class keeps us sane when working with Xerces.
// Include it after the generated header in order to get only char or
// wchar_t version depending on how you compiled your schemas.
//
#include <xsd/cxx/xml/string.hxx>

using std::cerr;
using std::endl;

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " transactions.xml" << endl;
    return 1;
  }

  using namespace xercesc;

  int r (0);

  // The Xerces-C++ DOM objects that will be used to store the
  // content matched by the wildcard "out-lives" the call to the
  // parsing function. Therefore we need to initialize the
  // Xerces-C++ runtime ourselves.
  //
  XMLPlatformUtils::Initialize ();

  try
  {
    using namespace transactions;

    // Parse the batch.
    //
    std::auto_ptr<batch> b (
      batch_ (argv[1], xml_schema::flags::dont_initialize));

    // Print what we've got in content order.
    //
    for (batch::content_order_const_iterator i (b->content_order ().begin ());
         i != b->content_order ().end ();
         ++i)
    {
      switch (i->id)
      {
      case batch::balance_id:
        {
          const balance& t (b->balance ()[i->index]);
          cerr << t.account () << " balance" << endl;
          break;
        }
      case batch::withdraw_id:
        {
          const withdraw& t (b->withdraw ()[i->index]);
          cerr << t.account () << " withdraw " << t.amount () << endl;
          break;
        }
      case batch::deposit_id:
        {
          const deposit& t (b->deposit ()[i->index]);
          cerr << t.account () << " deposit " << t.amount () << endl;
          break;
        }
      case batch::any_id:
        {
          namespace xml = xsd::cxx::xml;

          const DOMElement& e (b->any ()[i->index]);
          cerr << xml::transcode<char> (e.getLocalName ()) << endl;
          break;
        }
      default:
        {
          assert (false); // Unknown content id.
        }
      }
    }

    cerr << endl;

    // Modify the transaction batch. First remove the last transaction.
    // Note that we have to update both the content itself and content
    // order sequences.
    //
    batch::content_order_sequence& co (b->content_order ());

    co.pop_back ();
    b->withdraw ().pop_back ();

    // Now add a few more transactions. Again we have to add both the
    // content and its ordering. The order information consists of the
    // content id and, in case of a sequence, the index.
    //
    b->deposit ().push_back (deposit (123456789, 100000));
    co.push_back (
      batch::content_order_type (
        batch::deposit_id, b->deposit ().size () - 1));

    // The next transaction we add at the beginning of the batch.
    //
    b->balance ().push_back (balance (123456789));
    co.insert (co.begin (),
               batch::content_order_type (
                 batch::balance_id, b->balance ().size () - 1));

    // Note also that when we merely modify the content of one
    // of the elements in place, we don't need to update its
    // order. For example:
    //
    b->deposit ()[0].amount (2000000);

    // Serialize the modified transaction batch back to XML.
    //
    xml_schema::namespace_infomap map;

    map[""].name = "http://www.codesynthesis.com/transactions";
    map[""].schema = "transactions.xsd";
    map["te"].name = "http://www.codesynthesis.com/transactions-extras";

    batch_ (std::cout,
            *b,
            map,
            "UTF-8",
            xml_schema::flags::dont_initialize);
  }
  catch (const xml_schema::exception& e)
  {
    cerr << e << endl;
    r = 1;
  }

  XMLPlatformUtils::Terminate ();
  return r;
}
