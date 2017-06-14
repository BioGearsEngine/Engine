// file      : examples/cxx/tree/messaging/dom-serialize.cxx
// copyright : not copyrighted - public domain

#include "dom-serialize.hxx"

#include <ostream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

#include <xsd/cxx/xml/string.hxx>
#include <xsd/cxx/xml/dom/auto-ptr.hxx>
#include <xsd/cxx/xml/dom/serialization-source.hxx>
#include <xsd/cxx/xml/dom/bits/error-handler-proxy.hxx>

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

using namespace xercesc;
namespace xml = xsd::cxx::xml;
namespace tree = xsd::cxx::tree;

void
serialize (std::ostream& os,
           const xercesc::DOMDocument& doc,
           const std::string& encoding)
{
  const XMLCh ls_id [] = {chLatin_L, chLatin_S, chNull};

  // Get an implementation of the Load-Store (LS) interface.
  //
  DOMImplementation* impl (
    DOMImplementationRegistry::getDOMImplementation (ls_id));

  tree::error_handler<char> eh;
  xml::dom::bits::error_handler_proxy<char> ehp (eh);

  xml::dom::ostream_format_target oft (os);

  // Create a DOMSerializer.
  //
  xml::dom::auto_ptr<DOMLSSerializer> writer (
    impl->createLSSerializer ());

  DOMConfiguration* conf (writer->getDomConfig ());

  // Set error handler.
  //
  conf->setParameter (XMLUni::fgDOMErrorHandler, &ehp);

  // Set some generally nice features.
  //
  conf->setParameter (XMLUni::fgDOMWRTDiscardDefaultContent, true);
  conf->setParameter (XMLUni::fgDOMWRTFormatPrettyPrint, true);
  conf->setParameter (XMLUni::fgDOMWRTXercesPrettyPrint, false);

  xml::dom::auto_ptr<DOMLSOutput> out (impl->createLSOutput ());
  out->setEncoding (xml::string (encoding).c_str ());
  out->setByteStream (&oft);

  writer->write (&doc, out.get ());

  eh.throw_if_failed<tree::serialization<char> > ();
}
