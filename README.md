# ORAM-PIR

# Notes
- ORAM storage is arranged with each level being an array
    - these are flat, so to get a bucket, we simply grab several entries

# TODO:
- work on that ciphertext struct, currently you pass around c0 and c1 [ ]

## `ORAMStorage`
- `ORAMStorage::read` should probably read into preallocated things
- `ORAMStorage::read` should take a ciphertext as input at some point [ ]

## Linear Hash
- sample vector of size N from finite field of size N
- currently just sha-hashing index

## data to polynomial encoding
- encoding needs to be standardized [ ]
- learn more about relative size of message space to ctext space to pack more densely
- uint64_t is or isn't correct type for polynomials?
- not taking modulus for the coefficients
- __MUST DO THIS__ [x] (Done!)


## THINGS AREN"T BEING STORED IN NTT FORM [ ]

## Note on 6/2
- Basic ORAM access seems to work with the following caveats
    - this is just running ORAM between the ORAMClient / ORAMServer
    - the rebuilding scheme is basically "dump everything to the client and make them rebuild"
    - various memory usage inefficiencies


# Next Steps:
- refactor
- basically making this stuff all oblivious
    1. oblivious rebuilding
    2. oblivious writeback
- efficiency?