// file      : examples/cxx/tree/performance/parsing.cxx
// copyright : not copyrighted - public domain

#include <memory>  // std::auto_ptr
#include <cstddef> // std::size_t
#include <fstream>
#include <iostream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>

#include <xercesc/validators/common/Grammar.hpp>

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

#include <xercesc/util/XMLUniDefs.hpp>

#include <xsd/cxx/xml/dom/bits/error-handler-proxy.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

#include "time.hxx"
#include "test.hxx"

using namespace std;

bool
parsing (const char* file, unsigned long iter, bool validate)
{
  try
  {
    cerr << "parsing:" << endl;

    ifstream ifs;
    ifs.exceptions (ios_base::failbit);
    ifs.open (file, ios::in | ios::ate);

    size_t size (ifs.tellg ());
    ifs.seekg (0, ios::beg);

    char* buf = new char[size];
    ifs.read (buf, size);
    ifs.close ();

    cerr << "  document size:  " << size << " bytes" << endl
         << "  iterations:     " << iter << endl;

    // Create XML parser that we are going to use in all iterations.
    //
    using namespace xercesc;

    const XMLCh ls_id[] =
      {xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull};

    DOMImplementation* impl (
      DOMImplementationRegistry::getDOMImplementation (ls_id));

    // Use the error handler implementation provided by the XSD runtime.
    //
    xsd::cxx::tree::error_handler<char> eh;
    xsd::cxx::xml::dom::bits::error_handler_proxy<char> ehp (eh);

    xml_schema::dom::auto_ptr<DOMLSParser> parser (
      impl->createLSParser (DOMImplementationLS::MODE_SYNCHRONOUS, 0));

    DOMConfiguration* conf (parser->getDomConfig ());

    conf->setParameter (XMLUni::fgDOMComments, false);
    conf->setParameter (XMLUni::fgDOMDatatypeNormalization, true);
    conf->setParameter (XMLUni::fgDOMEntities, false);
    conf->setParameter (XMLUni::fgDOMNamespaces, true);
    conf->setParameter (XMLUni::fgDOMElementContentWhitespace, false);

    // Set error handler.
    //
    conf->setParameter (XMLUni::fgDOMErrorHandler, &ehp);

    if (validate)
    {
      conf->setParameter (XMLUni::fgDOMValidate, true);
      conf->setParameter (XMLUni::fgXercesSchema, true);
      conf->setParameter (XMLUni::fgXercesSchemaFullChecking, false);

      // Xerces-C++ 3.1.0 is the first version with working multi import
      // support.
      //
#if _XERCES_VERSION >= 30100
      conf->setParameter (XMLUni::fgXercesHandleMultipleImports, true);
#endif

      // If we are validating, pre-load and cache the schema.
      //
      if (!parser->loadGrammar ("test.xsd", Grammar::SchemaGrammarType, true))
      {
        // In Xerces-C++ grammar loading failure results in just a warning.
        // Make it a fatal error.
        //
        eh.handle ("test.xsd", 0, 0,
                   xsd::cxx::tree::error_handler<char>::severity::fatal,
                   "unable to load schema");
      }

      eh.throw_if_failed<xml_schema::parsing> ();
      conf->setParameter (XMLUni::fgXercesUseCachedGrammarInParse, true);
      conf->setParameter (XMLUni::fgXercesLoadSchema, false);
    }
    else
    {
      conf->setParameter (XMLUni::fgDOMValidate, false);
      conf->setParameter (XMLUni::fgXercesSchema, false);
      conf->setParameter (XMLUni::fgXercesSchemaFullChecking, false);
    }

    conf->setParameter (XMLUni::fgXercesUserAdoptsDOMDocument, true);

    // Create memory buffer input source.
    //
    MemBufInputSource is (
      reinterpret_cast<XMLByte*> (buf), size, file, false);
    is.setCopyBufToStream (false);
    Wrapper4InputSource wis (&is, false);

    // Parsing loop.
    //
    os::time start;

    for (unsigned long i (0); i < iter; ++i)
    {
      // First parse XML to DOM reusing the parser we created above.
      //
      xml_schema::dom::auto_ptr<DOMDocument> doc (parser->parse (&wis));
      eh.throw_if_failed<xml_schema::parsing> ();

      // Then parse DOM to the object model.
      //
      auto_ptr<test::root> r (test::root_ (*doc));
    }

    os::time end;
    os::time time (end - start);

    delete[] buf;

    cerr << "  time:           " << time << " sec" << endl;

    double ms (time.sec () * 1000000ULL + time.nsec () / 1000ULL);

    // Calculate throughput in documents/sec.
    //
    double tpd ((iter / ms) * 1000000);
    cerr << "  throughput:     " << tpd << " documents/sec" << endl;

    // Calculate throughput in MBytes/sec.
    //
    double tpb (((size * iter) / ms) * 1000000/(1024*1024));
    cerr << "  throughput:     " << tpb << " MBytes/sec" << endl;
  }
  catch (const xml_schema::exception& e)
  {
    cerr << e << endl;
    return false;
  }
  catch (std::ios_base::failure const&)
  {
    cerr << "io failure" << endl;
    return false;
  }

  return true;
}
