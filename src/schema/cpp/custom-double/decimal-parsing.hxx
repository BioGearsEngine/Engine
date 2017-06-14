#ifndef DECIMAL_PARSING_HXX
#define DECIMAL_PARSING_HXX

#include <limits>
#include <locale>

#include <xsd/cxx/ro-string.hxx>
#include <xsd/cxx/zc-istream.hxx>

#include <xsd/cxx/xml/string.hxx> // xml::transcode

#include <xsd/cxx/tree/text.hxx>  // text_content

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      template <typename C>
      struct traits<long double, C>
      {
        typedef long double type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };

      template <typename C>
      long double traits<long double, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      long double traits<long double, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      long double traits<long double, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        // This type cannot have whitespaces in its values. As result we
        // don't need to waste time collapsing whitespaces. All we need to
        // do is trim the string representation which can be done without
        // copying.
        //
        ro_string<C> tmp (s);
        trim (tmp);

        zc_istream<C> is (tmp);
        is.imbue (std::locale::classic ());

        type t;
        is >> t;

        return t;
      }
    }
  }
}

#endif // DECIMAL_PARSING_HXX
