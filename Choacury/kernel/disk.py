import argparse, binascii, logging, socket, struct, sys

REQ_CONF = '00'
REQ_WR   = '01'
REQ_RD   = '02'

ACK_OKAY = '00'
ACK_FAIL = '01'

# 00 command means a query operation: we pack the block size 
# and count into a single datum, then return it.

def conf( req ) :
  data  = struct.pack( '<l', args.block_num )
  data += struct.pack( '<l', args.block_len )

  return [ ACK_OKAY, data ]

# 01 command means a write operation:
# - if the address provided is invalid the request fails, else
# - we write the block to   the disk, then flush  the data.

def   wr( req ) :
  address = struct.unpack( '<l', binascii.unhexlify( req[ 1 ] ) )[ 0 ]
  data    =                      binascii.unhexlify( req[ 2 ] )

  print address

  if( address >= args.block_num ) :
    return [ ACK_FAIL ]

  fd.seek( address * args.block_len ) 
  fd.write( data )

  fd.flush()

  return [ ACK_OKAY       ]

# 02 command means a read  operation:
# - if the address provided is invalid the request fails, else
# - we read  the block from the disk, then return the data.

def   rd( req ) :
  address = struct.unpack( '<l', binascii.unhexlify( req[ 1 ] ) )[ 0 ]

  print address

  if( address >= args.block_num ) :
    return [ ACK_FAIL ]

  fd.seek( address * args.block_len )
  data = fd.read( args.block_len )

  fd.flush()

  return [ ACK_OKAY, data ]

# The command line interface basically just parses the arguments
# which configure the disk etc. then enters an infinite loop: it
# reads requests and writes acknowledgements one at a time until
# terminated.

if ( __name__ == '__main__' ) :
  # parse command line arguments

  parser = argparse.ArgumentParser()

  parser.add_argument( '--host',      type =  str, action = 'store'      )
  parser.add_argument( '--port',      type =  int, action = 'store'      )
  parser.add_argument( '--file',      type =  str, action = 'store'      )

  parser.add_argument( '--block-num', type =  int, action = 'store'      )
  parser.add_argument( '--block-len', type =  int, action = 'store'      )

  parser.add_argument( '--debug',                  action = 'store_true' )

  args = parser.parse_args()

  if ( args.debug ) :
    l = logging.DEBUG
  else :
    l = logging.INFO

  logging.basicConfig( stream = sys.stdout, level = l, format = '%(filename)s : %(asctime)s : %(message)s', datefmt = '%d/%m/%y @ %H:%M:%S' )

  # open disk image

  fd = open( args.file, 'rb+' )
  
  # open network connection

  s = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
  
  s.connect( ( args.host, args.port ) ) ; sd = s.makefile()

  # read request, process it and write acknowledgement
  
  while ( True ) :
    req = sd.readline().strip().split( ' ' )
  
    if   ( req[ 0 ] == REQ_CONF ) :
      ack = conf( req )
    elif ( req[ 0 ] == REQ_WR   ) :
      ack =   wr( req )
    elif ( req[ 0 ] == REQ_RD   ) :
      ack =   rd( req )
    else :
      ack = [ ACK_FAIL ]

    logging.debug( 'req = ' + str( req ) )  
    logging.debug( 'ack = ' + str( ack ) )

    if ( len( ack ) > 1 ) :
      ack = ack[ 0 ] + ' ' + ' '.join( [ binascii.hexlify( x ) for x in ack[ 1 : ] ] )
    else :
      ack = ack[ 0 ]

    sd.write( ack + '\n' ) ; sd.flush()
  
  # close network connection

  sd.close()

  # close disk image

  fd.close()
