// file      : examples/cxx/tree/embedded/grammar-input-stream.hxx
// copyright : not copyrighted - public domain

#ifndef GRAMMAR_INPUT_STREAM_HXX
#define GRAMMAR_INPUT_STREAM_HXX

#include <cstddef>
#include <xercesc/util/BinInputStream.hpp>

// Memory buffer input stream with the special-purpose schema
// grammar decompression.
//
class grammar_input_stream: public xercesc::BinInputStream
{
public :
  grammar_input_stream (const XMLByte* data, std::size_t size);

  virtual XMLFilePos
  curPos () const;

  virtual XMLSize_t
  readBytes (XMLByte* const buf, const XMLSize_t size);

  virtual const XMLCh*
  getContentType () const;

private :
  const XMLByte* data_;
  std::size_t size_;
  std::size_t pos_;
  std::size_t vpos_;

  // Compression data.
  //
  size_t cseq_;   // Number of bytes left in a compression sequence.
  bool alt_;      // Alternating or sequential sequence.
  bool add_zero_; // Add a zero on the next read.
};

#endif // GRAMMAR_INPUT_STREAM_HXX
