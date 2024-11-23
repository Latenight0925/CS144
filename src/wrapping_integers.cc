#include "wrapping_integers.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return Wrap32 { static_cast<uint32_t>( ( n + zero_point.raw_value_ ) % mod ) };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  uint64_t width = ( this->raw_value_ - zero_point.raw_value_ + mod ) % mod;
  if ( checkpoint <= width ) {
    return width;
  }
  uint64_t k = ( checkpoint - width ) / mod;
  if ( checkpoint <= width + k * mod + mod / 2 ) {
    return k * mod + width;
  }
  return ( k + 1 ) * mod + width;
}