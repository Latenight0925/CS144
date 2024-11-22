#include "byte_stream.hh"
#include <stdexcept>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), error_( false ), buf_() {}

bool Writer::is_closed() const
{
  return closed_;
}

void Writer::push( string data )
{
  if ( closed_ || error_ )
    return;

  uint64_t writable = min( available_capacity(), static_cast<uint64_t>( data.size() ) );
  for ( uint64_t i = 0; i < writable; i++ ) {
    buf_.push_back( data[i] );
  }
  bytes_pushed_ += writable;
}

void Writer::push( char ch )
{
  std::string str( 1, ch );
  push( str );
}

void Writer::close()
{
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - buf_.size();
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  return closed_ && buf_.empty();
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}

string_view Reader::peek() const
{
  return { std::string_view( &buf_.front(), 1 ) };
}

void Reader::pop( uint64_t len )
{
  if ( len > bytes_buffered() ) {
    throw std::runtime_error( "pop len larger than buffer contain" );
  }
  for ( uint64_t i = 0; i < len; i++ ) {
    buf_.pop_front();
  }
  bytes_popped_ += len;
}

uint64_t Reader::bytes_buffered() const
{
  return buf_.size();
}
