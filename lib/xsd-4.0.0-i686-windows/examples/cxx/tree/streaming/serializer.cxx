#include <vector>
#include <cassert>
#include <cstddef>

#include <xercesc/util/XMLUni.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/impl/DOMLSSerializerImpl.hpp>

#include <xsd/cxx/xml/string.hxx>
#include <xsd/cxx/xml/dom/bits/error-handler-proxy.hxx>
#include <xsd/cxx/xml/dom/serialization-source.hxx>

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

#include "serializer.hxx"

using namespace std;
using namespace xercesc;

namespace xml = xsd::cxx::xml;
namespace tree = xsd::cxx::tree;

static const XMLCh  gEOLSeq[] =
{
  chLF, chNull
};

static const XMLCh  gUTF8[] =
{
  chLatin_U, chLatin_T, chLatin_F, chDash, chDigit_8, chNull
};

static const XMLCh  gEndElement[] =
{
  chOpenAngle, chForwardSlash, chNull
};

static const int DISCARD_DEFAULT_CONTENT_ID       = 0x1;
static const int ENTITIES_ID                      = 0x2;
static const int FORMAT_PRETTY_PRINT_1ST_LEVEL_ID = 0xA;

class StreamingDOMSerializer: public DOMLSSerializerImpl
{
public:
  StreamingDOMSerializer (MemoryManager* manager)
    : DOMLSSerializerImpl (manager)
  {
  }

  bool
  startOpen (const DOMElement* e, DOMLSOutput* const destination)
  {
    const DOMDocument* docu (e->getOwnerDocument ());
    assert (docu != 0);

    // Code adapted from DOMLSSerializerImpl::write().
    //
    target_ = destination->getByteStream();

    fEncodingUsed = gUTF8;

    const XMLCh* lsEncoding=destination->getEncoding();
    if (lsEncoding && *lsEncoding)
    {
      fEncodingUsed = lsEncoding;
    }
    else if (docu)
    {
      const XMLCh* tmpEncoding = docu->getInputEncoding();

      if ( tmpEncoding && *tmpEncoding)
      {
        fEncodingUsed = tmpEncoding;
      }
      else
      {
        tmpEncoding = docu->getXmlEncoding();

        if ( tmpEncoding && *tmpEncoding)
        {
          fEncodingUsed = tmpEncoding;
        }
      }
    }

    fNewLineUsed = (fNewLine && *fNewLine)? fNewLine : gEOLSeq;

    fDocumentVersion = (docu->getXmlVersion() && *(docu->getXmlVersion()))
      ? docu->getXmlVersion()
      : XMLUni::fgVersion1_0;

    fErrorCount = 0;

    fLineFeedInTextNodePrinted = false;
    fLastWhiteSpaceInTextNode = 0;

    level_ = 0;
    namespace_map_.clear ();

    fFormatter = new (fMemoryManager) XMLFormatter( fEncodingUsed
                                                    ,fDocumentVersion
                                                    ,target_
                                                    ,XMLFormatter::NoEscapes
                                                    ,XMLFormatter::UnRep_CharRef
                                                    ,fMemoryManager);
    formatter_.reset (fFormatter);

    // Write out the XML declaration, etc. Here we assume that the document
    // has no children (i.e., no root element).
    //
    processNode (docu, 0);
    fLineFeedInTextNodePrinted = true;

    return writeOpen (e);
  }

  bool
  writeOpen (const DOMElement* e)
  {
    // Code adapted from the first part of ELEMENT_NODE case in
    // DOMLSSerializerImpl::processNode().
    //

    if (!fLineFeedInTextNodePrinted)
    {
      if(level_ == 1 && getFeature(FORMAT_PRETTY_PRINT_1ST_LEVEL_ID))
        printNewLine();

      printNewLine();
    }
    else
    {
      fLineFeedInTextNodePrinted = false;
    }

    printIndent(level_);

    RefHashTableOf<XMLCh>* namespaceMap = NULL;

    *fFormatter  << XMLFormatter::NoEscapes << chOpenAngle <<
      e->getNodeName ();

    setURCharRef();
    DOMNamedNodeMap *attributes = e->getAttributes();
    XMLSize_t attrCount = attributes->getLength();

    const XMLCh* prefix = e->getPrefix();
    const XMLCh* uri = e->getNamespaceURI();
    if((uri && uri[0]) ||
       ((prefix==0 || prefix[0]==0) && isDefaultNamespacePrefixDeclared()))
    {
      if(prefix==0 || prefix[0]==0)
        prefix=XMLUni::fgZeroLenString;
      if(!isNamespaceBindingActive(prefix, uri))
      {
        if(namespaceMap==NULL)
        {
          namespaceMap=new (fMemoryManager) RefHashTableOf<XMLCh>(12, false, fMemoryManager);
          fNamespaceStack->addElement(namespaceMap);
        }
        namespaceMap->put((void*)prefix,(XMLCh*)uri);
        *fFormatter  << XMLFormatter::NoEscapes
                     << chSpace << XMLUni::fgXMLNSString;

        if(!XMLString::equals(prefix,XMLUni::fgZeroLenString))
          *fFormatter  << chColon << prefix;

        *fFormatter  << chEqual << chDoubleQuote
                     << XMLFormatter::AttrEscapes
                     << uri
                     << XMLFormatter::NoEscapes
                     << chDoubleQuote;
      }
    }

    bool discard = getFeature(DISCARD_DEFAULT_CONTENT_ID);
    for (XMLSize_t i = 0; i < attrCount; i++)
    {
      DOMAttr*  attribute = (DOMAttr*)attributes->item(i);

      if (discard && !((DOMAttr*)attribute )->getSpecified())
        continue;

      // if this attribute is a namespace declaration, add it to the namespace map for the current level
      const XMLCh* ns = attribute->getNamespaceURI();
      if (ns != 0 )
      {
        if(XMLString::equals(ns, XMLUni::fgXMLNSURIName))
        {
          if(namespaceMap==NULL)
          {
            namespaceMap=new (fMemoryManager) RefHashTableOf<XMLCh>(12, false, fMemoryManager);
            fNamespaceStack->addElement(namespaceMap);
          }
          const XMLCh* nsPrefix = attribute->getLocalName();
          if(XMLString::equals(attribute->getNodeName(),XMLUni::fgXMLNSString))
            nsPrefix = XMLUni::fgZeroLenString;
          if(namespaceMap->containsKey((void*)nsPrefix))
            continue;
          namespaceMap->put((void*)attribute->getLocalName(),(XMLCh*)attribute->getNodeValue());
        }
        else if(!XMLString::equals(ns, XMLUni::fgXMLURIName))
        {
          // check if the namespace for the current node is already defined
          const XMLCh* prefix = attribute->getPrefix();
          if(prefix && prefix[0])
          {
            const XMLCh* uri = attribute->getNamespaceURI();
            if(!isNamespaceBindingActive(prefix, uri))
            {
              if(namespaceMap==NULL)
              {
                namespaceMap=new (fMemoryManager) RefHashTableOf<XMLCh>(12, false, fMemoryManager);
                fNamespaceStack->addElement(namespaceMap);
              }
              namespaceMap->put((void*)prefix,(XMLCh*)uri);
              *fFormatter  << XMLFormatter::NoEscapes
                           << chSpace << XMLUni::fgXMLNSString << chColon << prefix
                           << chEqual << chDoubleQuote
                           << XMLFormatter::AttrEscapes
                           << uri
                           << XMLFormatter::NoEscapes
                           << chDoubleQuote;
            }
          }
        }
      }

      if (XMLString::equals(ns, XMLUni::fgXMLNSURIName) || checkFilter(attribute) == DOMNodeFilter::FILTER_ACCEPT)
      {
        *fFormatter  << XMLFormatter::NoEscapes
                     << chSpace << attribute->getNodeName()
                     << chEqual << chDoubleQuote
                     << XMLFormatter::AttrEscapes;

        if (getFeature(ENTITIES_ID))
        {
          DOMNode* child = attribute->getFirstChild();
          while( child != 0)
          {
            if(child->getNodeType()==DOMNode::TEXT_NODE)
              *fFormatter  << child->getNodeValue();
            else if(child->getNodeType()==DOMNode::ENTITY_REFERENCE_NODE)
              *fFormatter << XMLFormatter::NoEscapes
                          << chAmpersand << child->getNodeName() << chSemiColon
                          << XMLFormatter::AttrEscapes;
            child = child->getNextSibling();
          }
        }
        else
          *fFormatter  << attribute->getNodeValue();

        *fFormatter  << XMLFormatter::NoEscapes << chDoubleQuote;
      }
    }

    *fFormatter << XMLFormatter::NoEscapes << chCloseAngle;

    // Keep track of whether we have added a namespace map for this
    // element. Used to pop it in writeClose().
    //
    namespace_map_.push_back (namespaceMap != 0);

    level_++;

    DOMNode* child = e->getFirstChild();
    while (child != 0)
    {
      processNode (child, level_);
      child = child->getNextSibling();
    }

    return fErrorCount == 0;
  }

  bool
  writeClose (const XMLCh* name)
  {
    // Code adapted from the second part of ELEMENT_NODE case in
    // DOMLSSerializerImpl::processNode().
    //
    level_--;

    // Assume we are not on the same line (nodeLine != fCurrentLine).
    //
    {
      if (!fLineFeedInTextNodePrinted)
      {
        printNewLine();
      }
      else
      {
        fLineFeedInTextNodePrinted = false;
      }

      if(level_ == 0 && getFeature(FORMAT_PRETTY_PRINT_1ST_LEVEL_ID))
        printNewLine();

      printIndent(level_);
    }

    *fFormatter << XMLFormatter::NoEscapes << gEndElement <<
      name << chCloseAngle;

    if (namespace_map_.back ())
      fNamespaceStack->removeLastElement();

    namespace_map_.pop_back ();

    if (level_ == 0)
    {
      printNewLine();
      target_->flush ();
    }

    return fErrorCount == 0;
  }

  bool
  write (const DOMElement* e)
  {
    processNode (e, level_);
    return fErrorCount == 0;
  }

  using DOMLSSerializerImpl::write; // Whole document.

private:
  XMLFormatTarget* target_;
  std::auto_ptr<XMLFormatter> formatter_;
  int level_;

  std::vector<bool> namespace_map_;
};

class serializer_impl
{
public:
  typedef serializer::namespace_infomap namespace_infomap;

  serializer_impl ();

  void
  start (ostream& os, const string& encoding);

  DOMElement*
  create (const string& name, const namespace_infomap&);

  DOMElement*
  create (const string& ns, const string& qname, const namespace_infomap&);

  void
  serialize (xml::dom::auto_ptr<DOMElement>);

  void
  serialize_open (xml::dom::auto_ptr<DOMElement>);

  void
  serialize_close (const string&);

private:
  void
  clear_document ();

private:
  bool start_;

  // Serializer.
  //
  xml::dom::auto_ptr<DOMLSOutput> out_;
  xml::dom::auto_ptr<StreamingDOMSerializer> serializer_;

  auto_ptr<xml::dom::ostream_format_target> oft_;

  tree::error_handler<char> error_handler_;
  xml::dom::bits::error_handler_proxy<char> error_proxy_;

  // DOM document that we use to create the elements.
  //
  DOMImplementation& dom_impl_;
  xml::dom::auto_ptr<DOMDocument> doc_;
  vector<DOMElement*> element_stack_;

  size_t element_count_; // Number of elements serialized using current doc.
  static const size_t element_count_limit_ = 500;
};

const XMLCh ls[] = {chLatin_L, chLatin_S, chNull};

serializer_impl::
serializer_impl ()
    : error_proxy_ (error_handler_),
      dom_impl_ (*DOMImplementationRegistry::getDOMImplementation (ls))
{
  serializer_.reset (
    new (XMLPlatformUtils::fgMemoryManager)
    StreamingDOMSerializer (XMLPlatformUtils::fgMemoryManager));

  DOMConfiguration* conf (serializer_->getDomConfig ());
  conf->setParameter (XMLUni::fgDOMErrorHandler, &error_proxy_);
  conf->setParameter (XMLUni::fgDOMXMLDeclaration, true);
  conf->setParameter (XMLUni::fgDOMWRTDiscardDefaultContent, true);
  conf->setParameter (XMLUni::fgDOMWRTFormatPrettyPrint, true);
  conf->setParameter (XMLUni::fgDOMWRTXercesPrettyPrint, false);
}

void serializer_impl::
start (ostream& os, const string& encoding)
{
  element_stack_.clear ();
  doc_.reset (dom_impl_.createDocument ());
  element_count_ = 0;

  error_handler_.reset ();
  oft_.reset (new xml::dom::ostream_format_target (os));

  out_.reset (dom_impl_.createLSOutput ());
  out_->setEncoding (xml::string (encoding).c_str ());
  out_->setByteStream (oft_.get ());

  start_ = true;
}

DOMElement* serializer_impl::
create (const string& name, const namespace_infomap& map)
{
  DOMElement* r (doc_->createElement (xml::string (name).c_str ()));

  if (!map.empty ())
    xml::dom::add_namespaces<char> (*r, map);

  // Add the element as the child of the stack "tip" so that it
  // "sees" all the namespace declarations active from this point.
  //
  if (!element_stack_.empty ())
    element_stack_.back ()->appendChild (r);

  return r;
}

DOMElement* serializer_impl::
create (const string& ns, const string& qname, const namespace_infomap& map)
{
  DOMElement* r (
    doc_->createElementNS (
      xml::string (ns).c_str (), xml::string (qname).c_str ()));

  if (!map.empty ())
    xml::dom::add_namespaces<char> (*r, map);

  // Add the element as the child of the stack "tip" so that it
  // "sees" all the namespace declarations active from this point.
  //
  if (!element_stack_.empty ())
    element_stack_.back ()->appendChild (r);

  return r;
}

void serializer_impl::
serialize (xml::dom::auto_ptr<DOMElement> p)
{
  DOMElement* e (p.get ());

  if (start_)
  {
    serializer_->write (e, out_.get ());
    start_ = false;
  }
  else
    serializer_->write (e);

  error_handler_.throw_if_failed<tree::serialization<char> > ();

  // Remove this element from its parent before we release.
  //
  if (!element_stack_.empty ())
    element_stack_.back ()->removeChild (e);

  p.reset (); // Release it before we may clear the document below.

  if (element_count_++ > element_count_limit_)
    clear_document ();
}

void serializer_impl::
serialize_open (xml::dom::auto_ptr<DOMElement> p)
{
  DOMElement* e (p.get ());

  if (start_)
  {
    serializer_->startOpen (e, out_.get ());
    start_ = false;
  }
  else
    serializer_->writeOpen (e);

  error_handler_.throw_if_failed<tree::serialization<char> > ();

  // Add this element to the element stack. serialize_close() is
  // responsible for its removal and releasing.
  //
  element_stack_.push_back (e);
  p.release ();
}

void serializer_impl::
serialize_close (const string& name)
{
  serializer_->writeClose (xml::string (name).c_str ());
  error_handler_.throw_if_failed<tree::serialization<char> > ();

  // Release the element.
  //
  DOMElement* e (element_stack_.back ());
  element_stack_.pop_back ();

  if (!element_stack_.empty ())
    element_stack_.back ()->removeChild (e);

  e->release ();

  if (element_count_++ > element_count_limit_)
    clear_document ();
}

void serializer_impl::
clear_document ()
{
  // Re-create the document in order to force deallocation of its
  // internal heap. While Xerces-C++ DOM tries to re-use memory,
  // it still accumulates no longer used memory blocks.
  //
  xml::dom::auto_ptr<DOMDocument> doc (dom_impl_.createDocument ());

  if (!element_stack_.empty ())
  {
    DOMElement* e (
      static_cast<DOMElement*> (
        doc->importNode (element_stack_.front (), true)));

    for (vector<DOMElement*>::iterator i (element_stack_.begin ());
         i != element_stack_.end ();
         ++i)
    {
      *i = e;
      e = static_cast<DOMElement*> (e->getFirstChild ());
    }
  }

  doc_ = doc;
  element_count_ = 0;
}

//
// serializer
//

serializer::
~serializer ()
{
}

serializer::
serializer ()
    : impl_ (new serializer_impl)
{
}

void serializer::
start (ostream& os, const string& encoding)
{
  impl_->start (os, encoding);
}

DOMElement* serializer::
create (const string& name, const namespace_infomap& map)
{
  return impl_->create (name, map);
}

DOMElement* serializer::
create (const string& ns, const string& qname, const namespace_infomap& map)
{
  return impl_->create (ns, qname, map);
}

void serializer::
serialize (xml::dom::auto_ptr<DOMElement> e)
{
  impl_->serialize (e);
}

void serializer::
serialize_open (xml::dom::auto_ptr<DOMElement> e)
{
  impl_->serialize_open (e);
}

void serializer::
serialize_close (const string& name)
{
  impl_->serialize_close (name);
}
