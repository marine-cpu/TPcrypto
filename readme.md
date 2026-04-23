# Exercise 2 — Construction of a Hash Function

## 1. Objective

The goal of this exercise is to construct a cryptographic hash function from a block cipher using classical constructions:

- SPECK block cipher
- Davies–Meyer compression function
- Merkle–Damgård iterative construction

The implementation is tested for three block sizes:
- BLOCKSIZE = 32 bits
- BLOCKSIZE = 48 bits
- BLOCKSIZE = 64 bits

---

## 2. Cryptographic Primitives

### 2.1 SPECK Block Cipher

We use a simplified SPECK cipher.

| BLOCKSIZE | Key size (KLEN) | Block size (HLEN) |
|----------|------------------|-------------------|
| 32       | 8 bytes          | 4 bytes           |
| 48       | 12 bytes         | 6 bytes           |
| 64       | 16 bytes         | 8 bytes           |

---

### 2.2 Compression Function (Davies–Meyer)

The compression function is defined as:

```

f(h, m) = E_m(h) XOR h

```

Where:
- `m` is used as encryption key
- `h` is the chaining value
- `E_m(h)` is SPECK encryption

---

### 2.3 Hash Function (Merkle–Damgård)

The hash function is defined iteratively:

```

h_{i+1} = f(h_i, m_i)

````

### Initialization Vector (IV)

- BLOCKSIZE = 32 → `03020100`
- BLOCKSIZE = 48 → `050403020100`
- BLOCKSIZE = 64 → `0706050403020100`

---

### Padding

1. Append bit `1` (0x80)
2. Fill with zeros
3. Append message length in bits (64-bit integer)

---

## 3. Implemented Functions

### compression

```c
void compression(byte h[HLEN], const byte m[BLEN]);
````

Computes:

```
h = E_m(h) XOR h
```

---

### hash

```c
void hash(const byte *m, size_t len, byte h[HLEN]);
```

Steps:

* Initialize with IV
* Process message blocks
* Apply padding
* Append length block
* Output final digest

---

### intermediate_digests

```c
void intermediate_digests(const byte *m, size_t len, byte *h);
```

Stores all chaining values:

```
h0, h1, ..., ht
```

Where:

* h0 = IV
* hi = state after each compression step

---

## 4. Files

* hash.c → implementation
* hash.h → declarations
* utils.c → SPECK cipher
* utils.h → constants and types
* test_hash.c → test program
* Makefile → compilation

---

## 5. Compilation

```bash
make clean
make test_hash BLOCKSIZE=32
make test_hash BLOCKSIZE=48
make test_hash BLOCKSIZE=64
```

---

## 6. Execution

```bash
./test_hash
```

---

## 7. Example Output

### BLOCKSIZE 32

```
h0 = 03020100
h1 = 40fd37ca
h2 = c07a5a89
```

### BLOCKSIZE 48

```
h0 = 050403020100
h1 = 4e33de573ce0
h2 = 9310a0fa68ca
```

### BLOCKSIZE 64

```
h0 = 0706050403020100
h1 = 2364af5f1011c178
h2 = 33072820a622e226
```

---


## 8. Conclusion

This exercise shows how to build a hash function from:

* SPECK block cipher
* Davies–Meyer compression function
* Merkle–Damgård construction

The implementation works correctly for all block sizes (32, 48, 64).