cplusplus {{
  #include <s2f/architecture/p2p/pow/chain/Transaction.h>
  #include <s2f/architecture/p2p/pow/chain/Block.h>
  #include <s2f/os/crypto/hash.h>
}}

class s2f::os::crypto::sha256digest {  
    @existingClass;  
    @opaque;
}

namespace s2f::p2p::pow;

class Transaction {  
    @existingClass;  
    @opaque;
}

struct BlockHeader {  
    @existingClass;  
    @opaque;
}

class Block {  
    @existingClass;  
    @opaque;
}
