# Hash Foundations Version 1 Vectors

## Status and Scope

These vectors freeze byte-level behavior for the educational C companion. They
are implementation checks, not evidence that BLAKE2b or MD5 satisfies a
security definition.

The active implementation uses unkeyed BLAKE2b with a 32-byte output. Output
length is an algorithm parameter: these values are BLAKE2b-256 results, not the
first half of BLAKE2b-512.

## BLAKE2b-256 Vectors

### Empty byte string

- Input length: 0 bytes
- Digest:

```text
0e5751c026e543b2e8ab2eb06099daa1d1e5df47778f7787faab45cdf12fe3a8
```

### ASCII `abc`

- Input bytes: `61 62 63`
- Input length: 3 bytes
- Digest:

```text
bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319
```

### Binary sequence `00` through `ff`

- Input byte at offset `i`: the value `i`, for offsets 0 through 255
- Input length: 256 bytes
- Digest:

```text
39a7eb9fedc19aabc83425c6755dd90e6f9d0c804964a1f4aaeea3b9fb599835
```

### One-bit diffusion observation

The first input is 32 zero bytes. The second is 32 bytes with only bit zero of
the first byte set.

```text
first  = 0000000000000000000000000000000000000000000000000000000000000000
second = 0100000000000000000000000000000000000000000000000000000000000000
```

Their BLAKE2b-256 digests are:

```text
first  = 89eb0d6a8a691dae2cd15ed0369931ce0a949ecafa5c3f93f8121833646e15c3
second = afbc1c053c2f278e3cbd4409c1c094f184aa459dd2f7fca96d6077730ab9ffe3
```

Exactly 130 of the 256 digest bits differ. This is one deterministic
observation for these inputs. It proves none of the preimage, second-preimage,
or collision-resistance notions.

## Historical MD5 Collision Exercise

The original draft attempted to record this classic 128-byte collision but
contained corrupted nibbles. The corrected distinct messages are:

```text
d131dd02c5e6eec4693d9a0698aff95c
2fcab58712467eab4004583eb8fb7f89
55ad340609f4b30283e488832571415a
085125e8f7cdc99fd91dbdf280373c5b
d8823e3156348f5bae6dacd436c919c6
dd53e2b487da03fd02396306d248cda0
e99f33420f577ee8ce54b67080a80d1e
c69821bcb6a8839396f9652b6ff72a70
```

and:

```text
d131dd02c5e6eec4693d9a0698aff95c
2fcab50712467eab4004583eb8fb7f89
55ad340609f4b30283e4888325f1415a
085125e8f7cdc99fd91dbd7280373c5b
d8823e3156348f5bae6dacd436c919c6
dd53e23487da03fd02396306d248cda0
e99f33420f577ee8ce54b67080280d1e
c69821bcb6a8839396f965ab6ff72a70
```

They differ in six bits and both have the historical MD5 digest:

```text
79054025255fb1a26e4bc422aef54eb4
```

Their BLAKE2b-256 digests are distinct:

```text
75deaf2191d1caf6ef01f72ca374a3740722d5edfd281248539206b7fe5dd0b8
9c5a2dfa02c53a2dfd945493074be1978a7b2a86699c75938edd58ee82e6ee2c
```

MD5 is not linked into the C companion. The equality above is retained solely
as an independently reproduced historical attack example; it is not a project
runtime dependency or approved algorithm.

## Independent Reproduction

The values were independently recomputed with Python 3.12's standard
`hashlib.blake2b(..., digest_size=32)` and, for the historical section only,
`hashlib.md5(..., usedforsecurity=False)`. The project test binary recomputes
the BLAKE2b vectors through libsodium. Python is verification tooling here, not
a C or runtime dependency.

The already frozen structured commitment example remains in
`test-vectors/commitment-v1.md`; it is not duplicated here.
