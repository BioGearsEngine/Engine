// file      : examples/cxx/tree/streaming/driver.cxx
// copyright : not copyrighted - public domain

#include <iostream>
#include <fstream>

#include <xercesc/dom/DOM.hpp>

#include <xsd/cxx/xml/string.hxx>  // xml::string

#include "parser.hxx"
#include "serializer.hxx"
#include "position.hxx"

using namespace std;
using namespace xercesc;

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " position.xml" << endl;
    return 1;
  }

  int r (0);

  // We need to initialize the Xerces-C++ runtime because we are doing
  // the XML-to-DOM parsing ourselves.
  //
  xercesc::XMLPlatformUtils::Initialize ();

  try
  {
    using namespace op;
    namespace xml = xsd::cxx::xml;

    // Parse and serialize at the same time, in the streaming mode.
    //

    ifstream ifs;
    ifs.exceptions (ifstream::badbit | ifstream::failbit);
    ifs.open (argv[1]);

    ofstream ofs;
    ofs.exceptions (ios_base::badbit | ios_base::failbit);
    ofs.open ("out.xml");

    xml_schema::namespace_infomap ns_map;
    ns_map["op"].name = "http://www.codesynthesis.com/op";
    ns_map["op"].schema = "position.xsd";

    parser p;
    serializer s;

    p.start (ifs, argv[1], true);
    s.start (ofs);

    typedef xml_schema::dom::auto_ptr<DOMDocument> document_ptr;

    // Peek at the root element. This way we only get the "carcase"
    // of the document, that is, the root element with its name, all
    // the attributes, and namespace declarations but without any of
    // the nested elements.
    //
    document_ptr docr (p.peek ());
    bool parsed (false);

    // Parse first-level elements.
    //
    for (document_ptr doc1 (p.peek ()); doc1.get () != 0; doc1 = p.peek ())
    {
      // Check whether it is an element that we should stream (position) or
      // just add to the root (header).
      //
      string n1 (xml::transcode<char> (
                   doc1->getDocumentElement ()->getLocalName ()));

      // If we see the first streaming element, then parse the root carcase.
      //
      if (!parsed && n1 == "position")
      {
        object o (*docr->getDocumentElement ());

        cerr << "id:   " << o.id () << endl
             << "name: " << o.header ().name () << endl
             << "type: " << o.header ().type () << endl;

        // Start serializing the document by writing out the root carcase.
        // Note that we leave it open so that we can serialize more elements.
        //
        s.next_open (ns_map["op"].name, "op:object", ns_map, o);
        parsed = true;
      }

      // Handle elements that need streaming.
      //
      if (n1 == "position")
      {
        // Position has no nested elements that we need to stream so we
        // finish parsing it in one go.
        //
        doc1 = p.next (doc1);
        position pos (*doc1->getDocumentElement ());

        cerr << "lat: " << pos.lat () << " lon: " << pos.lon () << endl;

        // Serialize it (append) to the root element.
        //
        s.next ("position", pos);
      }
      else
      {
        // Element that doesn't require streaming (header in our case). Add
        // to the root element and finish parsing.
        //
        docr = p.next (doc1, docr);
      }
    }

    // Close the root element in serializer.
    //
    s.next_close ("op:object");
  }
  catch (const xml_schema::exception& e)
  {
    cerr << e << endl;
    r = 1;
  }
  catch (const ios_base::failure&)
  {
    cerr << "io failure" << endl;
    r = 1;
  }

  xercesc::XMLPlatformUtils::Terminate ();
  return r;
}
