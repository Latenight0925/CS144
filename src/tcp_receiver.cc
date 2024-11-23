#include "tcp_receiver.hh"

#include <iostream>
#include <ostream>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if ( message.RST ) {
    reassembler_.reader().set_error();
    RST_ = reassembler_.reader().has_error();
  } else if ( RST_ ) {
    return;
  }

  if ( message.SYN ) {
    SYN_ = true;
    zero_point_ = message.seqno;
  }
  if ( !SYN_ ) {
    return;
  }

  uint64_t first_index = message.seqno.unwrap(
    zero_point_.value(), reassembler_.writer().bytes_pushed() + SYN_ + reassembler_.writer().is_closed() );
  first_index -= !message.SYN;

  reassembler_.insert( first_index, message.payload, message.FIN );

  ackno_ = Wrap32::wrap( reassembler_.writer().bytes_pushed() + SYN_ + reassembler_.writer().is_closed(),
                         zero_point_.value() ); // 将SYN和FIN算上
}

TCPReceiverMessage TCPReceiver::send() const
{

  return { .ackno = ackno_,
           .window_size
           = static_cast<uint16_t>( min<uint64_t>( reassembler_.writer().available_capacity(), UINT16_MAX ) ),
           .RST = RST_ || reassembler_.reader().has_error() };
}
