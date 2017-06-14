// file      : examples/cxx/tree/messaging/dom-parse.cxx
// copyright : not copyrighted - public domain

#include "dom-parse.hxx"

#include <istream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp> // chLatin_*
#include <xercesc/framework/Wrapper4InputSource.hpp>

#include <xsd/cxx/xml/sax/std-input-source.hxx>
#include <xsd/cxx/xml/dom/bits/error-handler-proxy.hxx>

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

using namespace xercesc;
namespace xml = xsd::cxx::xml;
namespace tree = xsd::cxx::tree;

xml::dom::auto_ptr<DOMDocument>
parse (std::istream& is, const std::string& id, bool validate)
{
  const XMLCh ls_id [] = {chLatin_L, chLatin_S, chNull};

  // Get an implementation of the Load-Store (LS) interface.
  //
  DOMImplementation* impl (
    DOMImplementationRegistry::getDOMImplementation (ls_id));

  xml::dom::auto_ptr<DOMLSParser> parser (
    impl->createLSParser (DOMImplementationLS::MODE_SYNCHRONOUS, 0));

  DOMConfiguration* conf (parser->getDomConfig ());

  // Discard comment nodes in the document.
  //
  conf->setParameter (XMLUni::fgDOMComments, false);

  // Enable datatype normalization.
  //
  conf->setParameter (XMLUni::fgDOMDatatypeNormalization, true);

  // Do not create EntityReference nodes in the DOM tree. No
  // EntityReference nodes will be created, only the nodes
  // corresponding to their fully expanded substitution text
  // will be created.
  //
  conf->setParameter (XMLUni::fgDOMEntities, false);

  // Perform namespace processing.
  //
  conf->setParameter (XMLUni::fgDOMNamespaces, true);

  // Do not include ignorable whitespace in the DOM tree.
  //
  conf->setParameter (XMLUni::fgDOMElementContentWhitespace, false);

  // Enable/Disable validation.
  //
  conf->setParameter (XMLUni::fgDOMValidate, validate);
  conf->setParameter (XMLUni::fgXercesSchema, validate);
  conf->setParameter (XMLUni::fgXercesSchemaFullChecking, false);

  // Xerces-C++ 3.1.0 is the first version with working multi import
  // support.
  //
#if _XERCES_VERSION >= 30100
  conf->setParameter (XMLUni::fgXercesHandleMultipleImports, true);
#endif

  // We will release the DOM document ourselves.
  //
  conf->setParameter (XMLUni::fgXercesUserAdoptsDOMDocument, true);

  // Set error handler.
  //
  tree::error_handler<char> eh;
  xml::dom::bits::error_handler_proxy<char> ehp (eh);
  conf->setParameter (XMLUni::fgDOMErrorHandler, &ehp);

  // Prepare input stream.
  //
  xml::sax::std_input_source isrc (is, id);
  Wrapper4InputSource wrap (&isrc, false);

  xml::dom::auto_ptr<DOMDocument> doc (parser->parse (&wrap));

  eh.throw_if_failed<tree::parsing<char> > ();

  return doc;
}
