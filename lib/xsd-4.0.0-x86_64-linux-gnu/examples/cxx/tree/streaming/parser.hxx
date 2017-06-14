#ifndef PARSER_HXX
#define PARSER_HXX

#include <string>
#include <iosfwd>
#include <cstddef> // std::size_t
#include <memory>  // std::auto_ptr

#include <xercesc/dom/DOMDocument.hpp>

#include <xsd/cxx/xml/dom/auto-ptr.hxx>

class parser_impl;

class parser
{
public:
  // We can specify embedded XML Schema grammar to be used by the parser
  // that was created by the xsdbin utility from the 'embedded' example.
  //
  parser (const XMLByte* grammar = 0, std::size_t grammar_size = 0);
  ~parser ();

  // The start function prepares everything for parsing a new document.
  //
  void
  start (std::istream& is, const std::string& id, bool validate);

  typedef xsd::cxx::xml::dom::auto_ptr<xercesc::DOMDocument> document_ptr;

  // The peek function parses just the next element (ignoring any
  // preceding content assuming it is whitespace) without parsing
  // any of its nested content (but it includes the element's
  // attributes). It returns NULL if there are no more elements
  // at this level (there could still be on outer levels in case
  // of nested streaming).
  //
  document_ptr
  peek ();

  // The next function parses (or finishes parsing after peek) the
  // next element including its nested content. It returns NULL if
  // there are no more elements at this level (there could still
  // be on outer levels in case of nested streaming).
  //
  // If doc is not NULL, then it should be the document returned
  // by peek(). That is, a document with only the root element.
  // In this case next() finishes parsing this element.
  //
  // If outer_doc is not NULL, then next() will first add doc to
  // outer_doc as a child of the document root.
  //
  document_ptr
  next (document_ptr doc = document_ptr (),
        document_ptr outer_doc = document_ptr ());

private:
  parser (const parser&);

  parser&
  operator= (const parser&);

private:
  std::auto_ptr<parser_impl> impl_;
};

#endif // PARSER_HXX
