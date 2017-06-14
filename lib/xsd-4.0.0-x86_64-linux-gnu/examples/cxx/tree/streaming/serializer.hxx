// file      : examples/cxx/tree/streaming/serializer.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : not copyrighted - public domain

#ifndef SERIALIZER_HXX
#define SERIALIZER_HXX

#include <string>
#include <iosfwd>
#include <memory> // std::auto_ptr

#include <xercesc/dom/DOMElement.hpp>

#include <xsd/cxx/xml/dom/auto-ptr.hxx>
#include <xsd/cxx/xml/dom/serialization-header.hxx> // namespace_infomap

class serializer_impl;

class serializer
{
public:
  typedef xsd::cxx::xml::dom::namespace_infomap<char> namespace_infomap;

  ~serializer ();
  serializer ();

  // Start the serialization process.
  //
  void
  start (std::ostream& is, const std::string& encoding = "UTF-8");

  // Serialize next object model fragment into an element with the specified
  // name.
  //
  template <typename T>
  void
  next (const std::string& name, const T& x);

  // Serialize next object model fragment into an element with the specified
  // name and namespace declarations.
  //
  template <typename T>
  void
  next (const std::string& name, const namespace_infomap&, const T& x);

  // Serialize next object model fragment into an element with the specified
  // namespace and qualified name.
  //
  template <typename T>
  void
  next (const std::string& ns, const std::string& name, const T& x);

  // Serialize next object model fragment into an element with the specified
  // namespace and qualified name as well as namespace declarations.
  //
  template <typename T>
  void
  next (const std::string& ns,
        const std::string& name,
        const namespace_infomap&,
        const T& x);

  // The next_open/close functions are like next() but split into two steps.
  // next_open() serializes the object model fragment into an element leaving
  // it open while next_close() closes the element.
  //
  template <typename T>
  void
  next_open (const std::string& name, const T& x);

  template <typename T>
  void
  next_open (const std::string& name, const namespace_infomap&, const T& x);

  template <typename T>
  void
  next_open (const std::string& ns, const std::string& name, const T& x);

  template <typename T>
  void
  next_open (const std::string& ns,
        const std::string& name,
        const namespace_infomap&,
        const T& x);

  void
  next_close (const std::string& name);

private:
  serializer (const serializer&);

  serializer&
  operator= (const serializer&);

private:
  xercesc::DOMElement*
  create (const std::string& name, const namespace_infomap&);

  xercesc::DOMElement*
  create (const std::string& ns,
          const std::string& name,
          const namespace_infomap&);

  void
  serialize (xsd::cxx::xml::dom::auto_ptr<xercesc::DOMElement>);

  void
  serialize_open (xsd::cxx::xml::dom::auto_ptr<xercesc::DOMElement>);

  void
  serialize_close (const std::string& name);

private:
  std::auto_ptr<serializer_impl> impl_;
};

template <typename T>
inline void serializer::
next (const std::string& name, const T& x)
{
  xsd::cxx::xml::dom::auto_ptr<xercesc::DOMElement> e (
    create (name, namespace_infomap ()));
  *e << x;
  serialize (e);
}

template <typename T>
inline void serializer::
next (const std::string& name, const namespace_infomap& map, const T& x)
{
  xsd::cxx::xml::dom::auto_ptr<xercesc::DOMElement> e (create (name, map));
  *e << x;
  serialize (e);
}

template <typename T>
inline void serializer::
next (const std::string& ns, const std::string& name, const T& x)
{
  xsd::cxx::xml::dom::auto_ptr<xercesc::DOMElement> e (
    create (ns, name, namespace_infomap ()));
  *e << x;
  serialize (e);
}

template <typename T>
inline void serializer::
next (const std::string& ns,
      const std::string& name,
      const namespace_infomap& map,
      const T& x)
{
  xsd::cxx::xml::dom::auto_ptr<xercesc::DOMElement> e (create (ns, name, map));
  *e << x;
  serialize (e);
}

template <typename T>
inline void serializer::
next_open (const std::string& name, const T& x)
{
  xsd::cxx::xml::dom::auto_ptr<xercesc::DOMElement> e (
    create (name, namespace_infomap ()));
  *e << x;
  serialize_open (e);
}

template <typename T>
inline void serializer::
next_open (const std::string& name, const namespace_infomap& map, const T& x)
{
  xsd::cxx::xml::dom::auto_ptr<xercesc::DOMElement> e (create (name, map));
  *e << x;
  serialize_open (e);
}

template <typename T>
inline void serializer::
next_open (const std::string& ns, const std::string& name, const T& x)
{
  xsd::cxx::xml::dom::auto_ptr<xercesc::DOMElement> e (
    create (ns, name, namespace_infomap ()));
  *e << x;
  serialize_open (e);
}

template <typename T>
inline void serializer::
next_open (const std::string& ns,
      const std::string& name,
      const namespace_infomap& map,
      const T& x)
{
  xsd::cxx::xml::dom::auto_ptr<xercesc::DOMElement> e (create (ns, name, map));
  *e << x;
  serialize_open (e);
}

inline void serializer::
next_close (const std::string& name)
{
  serialize_close (name);
}

#endif // SERIALIZER_HXX
