#include "reassembler.hh"

#include <iostream>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  uint64_t k { 0 };
  for ( k = 0; k < data.size(); k++ ) {
    if ( !flag_.contains( first_index + k ) ) {
      break;
    }
  }

  if ( k == data.size() ) {
    try_close( first_index, data, is_last_substring );
    return;
  }
  first_index += k;
  data = data.substr( k );

  uint64_t start_index, max_index;
  if ( expect_next_index_ < first_index ) {

    max_index = max( output_.writer().available_capacity() + expect_next_index_, data.size() + first_index );
    start_index = first_index;

  } else {
    max_index = max( output_.writer().available_capacity() + expect_next_index_, data.size() + first_index );
    start_index = expect_next_index_;
  }

  string buffer {};

  buffer.reserve( output_.writer().available_capacity() );
  uint64_t i { start_index };
  while ( i < max_index ) {

    if ( i >= expect_next_index_ + output_.writer().available_capacity() ) {
      break;
    }
    if ( flag_.contains( i ) ) {

      uint64_t l = i + data_[i].size();

      buffer += data_[i];
      data_.erase( i );

      i = l;
      continue;
    }

    if ( i >= first_index + data.size() ) {

      break;
    }
    flag_.insert( i );

    buffer += data[i - first_index];

    i++;

    bytes_pending_++;
  }

  if ( !buffer.empty() ) {
    if ( expect_next_index_ < first_index ) {

      data_[start_index] = buffer;

    } else {

      output_.writer().push( buffer );
      expect_next_index_ = i;
      bytes_pending_ -= i - start_index;
      for ( uint64_t j = start_index; j < i; j++ ) {
        flag_.erase( j );
      }
    }
  }

  try_close( first_index, data, is_last_substring );
}

uint64_t Reassembler::bytes_pending() const
{
  return bytes_pending_;
}

void Reassembler::try_close( uint64_t first_index, const std::string data, bool is_last_substring )
{
  if ( is_last_substring ) {
    last_byte_next_index_ = first_index + data.size();
  }
  if ( expect_next_index_ == last_byte_next_index_ ) {
    output_.writer().close();
  }
}
