#include <cassert>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/impl/DOMTextImpl.hpp>

#include <xercesc/validators/common/Grammar.hpp> // xercesc::Grammar
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>

#include <xsd/cxx/auto-array.hxx>

#include <xsd/cxx/xml/sax/std-input-source.hxx>
#include <xsd/cxx/xml/sax/bits/error-handler-proxy.hxx>

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

#include "parser.hxx"
#include "grammar-input-stream.hxx"

using namespace std;
using namespace xercesc;

namespace xml = xsd::cxx::xml;
namespace tree = xsd::cxx::tree;

typedef parser::document_ptr document_ptr;

class parser_impl: public DefaultHandler
{
public:
  parser_impl (const XMLByte* grammar, size_t grammar_size);

  void
  start (istream& is, const string& id, bool validate);

  document_ptr
  peek ();

  document_ptr
  next (document_ptr doc = document_ptr (),
        document_ptr outer_doc = document_ptr ());

  // SAX event handlers.
  //
private:
  virtual void
  startElement (const XMLCh* const uri,
                const XMLCh* const lname,
                const XMLCh* const qname,
                const Attributes& attributes);

  virtual void
  endElement (const XMLCh* const uri,
              const XMLCh* const lname,
              const XMLCh* const qname);

  virtual void
  characters (const XMLCh* const s,
              const XMLSize_t length);

private:
  // SAX parser.
  //
  bool clean_;
  auto_ptr<XMLGrammarPool> grammar_pool_;
  auto_ptr<SAX2XMLReader> parser_;
  XMLPScanToken token_;
  tree::error_handler<char> error_handler_;
  xml::sax::bits::error_handler_proxy<char> error_proxy_;
  auto_ptr<xml::sax::std_input_source> isrc_;

  size_t depth_;
  size_t whitespace_depth_; // Depth at which to ignore whitespaces.

  bool peek_;
  size_t next_depth_; // Depth at which next() should work.

  // DOM document being built.
  //
  DOMImplementation& dom_impl_;
  document_ptr doc_;
  DOMElement* cur_;
};

const XMLCh ls[] = {chLatin_L, chLatin_S, chNull};

parser_impl::
parser_impl (const XMLByte* grammar, size_t grammar_size)
    : clean_ (true),
      error_proxy_ (error_handler_),
      dom_impl_ (*DOMImplementationRegistry::getDOMImplementation (ls))
{
  MemoryManager* mm (XMLPlatformUtils::fgMemoryManager);

  if (grammar != 0)
  {
    assert (grammar_size != 0);
    grammar_pool_.reset (new XMLGrammarPoolImpl (mm));

    grammar_input_stream is (grammar, grammar_size);
    grammar_pool_->deserializeGrammars(&is);
    grammar_pool_->lockPool ();
  }

  parser_.reset (XMLReaderFactory::createXMLReader (mm, grammar_pool_.get ()));

  parser_->setFeature (XMLUni::fgSAX2CoreNameSpaces, true);
  parser_->setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, true);
  parser_->setFeature (XMLUni::fgXercesValidationErrorAsFatal, true);
  parser_->setFeature (XMLUni::fgXercesSchemaFullChecking, false);

  // Xerces-C++ 3.1.0 is the first version with working multi import
  // support. It also allows us to disable buffering in the parser
  // so that the data is parsed and returned as soon as it is
  // available.
  //
#if _XERCES_VERSION >= 30100
  parser_->setFeature (XMLUni::fgXercesHandleMultipleImports, true);

  XMLSize_t lwm = 0;
  parser_->setProperty (XMLUni::fgXercesLowWaterMark, &lwm);
#endif

  parser_->setErrorHandler (&error_proxy_);
  parser_->setContentHandler (this);
}

void parser_impl::
start (istream& is, const string& id, bool val)
{
  // Reset our state.
  //
  depth_ = 0;
  peek_ = false;
  doc_.reset ();
  error_handler_.reset ();

  if (!clean_)
    parser_->parseReset (token_);
  else
    clean_ = false;

  isrc_.reset (new xml::sax::std_input_source (is, id));

  parser_->setFeature (XMLUni::fgSAX2CoreValidation, val);
  parser_->setFeature (XMLUni::fgXercesSchema, val);

  if (val && grammar_pool_.get () != 0)
  {
    // Use the loaded grammar during parsing.
    //
    parser_->setFeature (XMLUni::fgXercesUseCachedGrammarInParse, true);

    // Disable loading schemas via other means (e.g., schemaLocation).
    //
    parser_->setFeature (XMLUni::fgXercesLoadSchema, false);
  }

  parser_->parseFirst (*isrc_, token_);
  error_handler_.throw_if_failed<tree::parsing<char> > ();
}

document_ptr parser_impl::
peek ()
{
  bool r (true);

  size_t d (depth_);
  whitespace_depth_ = d;

  peek_ = true;

  // Parse (skip whitespace content) until the depth increases or we get
  // a document. The latter test covers <element/> cases where both start
  // and end events will trigger and therefore leave the depth unchanged.
  //
  while (r && depth_ == d && doc_.get () == 0)
  {
    r = parser_->parseNext (token_);
    error_handler_.throw_if_failed<tree::parsing<char> > ();
  }

  if (!r)
    return document_ptr (0);

  return doc_;
}

document_ptr parser_impl::
next (document_ptr doc, document_ptr outer_doc)
{
  assert (peek_ == (doc.get () != 0));

  // Install doc/outer_doc as the document we are parsing.
  //
  if (doc.get () != 0)
  {
    if (outer_doc.get () != 0)
    {
      // Copy doc to outer_doc.
      //
      doc_ = outer_doc;
      cur_ = static_cast<DOMElement*> (
        doc_->importNode (doc->getDocumentElement (), true));
      doc_->getDocumentElement ()->appendChild (cur_);
    }
    else
    {
      doc_ = doc;
      cur_ = doc_->getDocumentElement ();
    }

    // This handles the <element/> case where we get both start and
    // end events in peek(). In this case the element is fully parsed
    // and next() has nothing to do.
    //
    if (depth_ != next_depth_)
    {
      peek_ = false;
      return doc_;
    }
  }

  bool r (true);

  // If we peeked, then we have already seen the start tag and our
  // return depth is one above the current depth.
  //
  size_t d (peek_ ? depth_ - 1 : depth_);
  whitespace_depth_ = d;

  peek_ = false;

  // Keep calling parseNext() until we either move to a greater depth or
  // get a document. This way we skip the text (presumably whitespaces)
  // that may be preceding this chunk.
  //
  while (r && depth_ == d && doc_.get () == 0)
  {
    parser_->parseNext (token_);
    error_handler_.throw_if_failed<tree::parsing<char> > ();
  }

  if (!r)
    return document_ptr (0);

  // If we are not at our start depth, keep calling parseNext() until we
  // get there again.
  //
  while (r && depth_ != d)
  {
    r = parser_->parseNext (token_);
    error_handler_.throw_if_failed<tree::parsing<char> > ();
  }

  if (!r)
    return document_ptr (0);

  return doc_;
}

// DOM builder.
//

void parser_impl::
startElement (const XMLCh* const uri,
              const XMLCh* const /*lname*/,
              const XMLCh* const qname,
              const Attributes& attr)
{
  if (doc_.get () == 0)
  {
    doc_.reset (dom_impl_.createDocument (uri, qname, 0));
    cur_ = doc_->getDocumentElement ();
  }
  else
  {
    DOMElement* e = doc_->createElementNS (uri, qname);
    cur_->appendChild (e);
    cur_ = e;
  }

  // Set attributes.
  //
  for (XMLSize_t i (0), end (attr.getLength()); i < end; ++i)
  {
    const XMLCh* qn (attr.getQName (i));
    const XMLCh* ns (attr.getURI (i));

    // When SAX2 reports the xmlns attribute, it does not include
    // the proper attribute namespace. So we have to detect and
    // handle this case.
    //
    if (XMLString::equals (qn, XMLUni::fgXMLNSString))
      ns = XMLUni::fgXMLNSURIName;

    cur_->setAttributeNS (ns, qn, attr.getValue (i));
  }

  depth_++;

  if (peek_)
    next_depth_ = depth_;
}

void parser_impl::
endElement (const XMLCh* const /*uri*/,
            const XMLCh* const /*lname*/,
            const XMLCh* const /*qname*/)
{
  // We have an element parent only on depth 2 or greater.
  //
  if (--depth_ > 1)
    cur_ = static_cast<DOMElement*> (cur_->getParentNode ());
}

void parser_impl::
characters (const XMLCh* const s, const XMLSize_t length)
{
  const XMLCh empty[] = {chNull};

  // Ignore text content (presumably whitespaces) while looking for
  // the next element.
  //
  if (depth_ > whitespace_depth_)
  {
    DOMText* t = doc_->createTextNode (empty);
    static_cast<DOMTextImpl*> (t)->appendData (s, length);
    cur_->appendChild (t);
  }
}

//
// parser
//

parser::
~parser ()
{
}

parser::
parser (const XMLByte* grammar, size_t grammar_size)
    : impl_ (new parser_impl (grammar, grammar_size))
{
}

void parser::
start (istream& is, const string& id, bool val)
{
  return impl_->start (is, id, val);
}

document_ptr parser::
peek ()
{
  return impl_->peek ();
}

document_ptr parser::
next (document_ptr doc, document_ptr outer_doc)
{
  return impl_->next (doc, outer_doc);
}
