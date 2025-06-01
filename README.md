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