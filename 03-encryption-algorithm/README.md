# Encryption Algorithm Project

This project implements two cryptographic hash functions in C: SHA-256 and RUDRA-512.

## SHA-256

SHA-256 (Secure Hash Algorithm 2) is a cryptographic hash function that produces a 256-bit (32-byte) digest. It is widely used in security protocols (SSL/TLS), digital signatures, and blockchain technologies (like Bitcoin).

### Algorithm Overview

SHA-256 works by processing the message in 512-bit blocks. It uses a message schedule of sixty-four 32-bit words and a compression function with eight 32-bit working variables.

```
Message Padding
      |
      v
Message Schedule (W0 - W63)
      |
      v
Compression Function
(8 working variables A-H, 64 rounds)
      |
   +--+--+
   |     |
   v     v
Logical Functions (Ch, Maj, Σ0, Σ1, σ0, σ1)
   |
   v
Final Hash State update
```

### Usage in other projects
- **VCS project #22**: Used for file content fingerprinting and commit IDs.
- **Blockchain project #18**: Used for block hashing and Proof-of-Work.

## RUDRA-512

RUDRA-512 is an experimental, research-oriented 512-bit hash function originally designed by Ayush Anand. It produces a 128-character hexadecimal digest.

### Key Features
- **512-bit output**: 128-character hex digest.
- **ARX Core**: Built on Addition-Rotation-XOR mixing.
- **BPE Preprocessing**: Uses a byte-level fallback for tokenization.
- **Input Scattering**: Structured salt and data interleaving.
- **Configurable Rounds**: Default is 32 rounds for balanced security/performance.

## Known SHA-256 Test Vectors

- **Empty String**: `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`
- **"abc"**: `ba7816bf8f01cfea414140de5dae2ec73b00361bbef0469348423f656b5c1d5`

## Compilation and Execution

### Main Demonstration
```bash
gcc -o crypto src/sha256.c src/rudra512.c src/main.c
./crypto
```

### Unit Tests
```bash
gcc -o tests tests/test_crypto.c src/sha256.c src/rudra512.c
./tests
```

## What was learned
- Implementing SHA-256 from the FIPS 180-4 specification.
- Porting complex cryptographic logic from C++ to C.
- Handling dynamic memory and custom data structures in C.
- Implementing avalanche effect analysis and performance benchmarking.
- Importance of domain separation and structured padding in hash functions.
