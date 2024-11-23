#include "byte_stream.hh"
#include <iostream>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  return closed_;
}

void Writer::push( string data )
{
  if ( available_capacity() > 0 ) {
    const uint64_t len = min( data.size(), available_capacity() );
    buffer_.insert( buffer_.end(), data.begin(), data.begin() + len );
    count_pushed += len;
  }
}

void Writer::close()
{
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
  return count_pushed;
}

bool Reader::is_finished() const
{
  return closed_ && buffer_.empty();
}

uint64_t Reader::bytes_popped() const
{
  return count_popped;
}

string_view Reader::peek() const
{
  return string_view( buffer_ );
}

void Reader::pop( uint64_t len )
{
  if ( bytes_buffered() < len ) {
    set_error();
    return;
  }
  buffer_.erase( buffer_.begin(), buffer_.begin() + len );
  count_popped += len;
}

uint64_t Reader::bytes_buffered() const
{
  return buffer_.size();
}