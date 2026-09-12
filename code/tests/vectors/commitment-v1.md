# Commitment v1 deterministic vector

This vector freezes the canonical encoding used by commitment suite version 1.
It is an encoding and implementation cross-check, not evidence that the
construction is hiding or binding.

## Inputs

| Field | Value |
|---|---|
| Scheme domain | ASCII `AC-COMMITMENT-V1` (16 bytes, no NUL terminator) |
| Scheme version | `1` (`0001`) |
| Protocol identifier | ASCII `AC-RPS` followed by ten zero bytes |
| Protocol version | `1` (`0001`) |
| Session identifier | bytes `00` through `1f` in ascending order |
| Round | `1` (`00000001`) |
| Committer role | Alice, `1` (`01`) |
| Recipient role | Bob, `2` (`02`) |
| Payload type | RPS move, `1` (`0001`) |
| Nonce | bytes `a0` through `bf` in ascending order |
| Payload | Rock, `0` (`00`) |

All multibyte integers are unsigned big-endian. Fixed field lengths remain in
the encoded preimage as required by the v1 specification.

## Complete preimage

The preimage is 123 bytes:

```text
41432d434f4d4d49544d454e542d5631
0001
0010
41432d52505300000000000000000000
0001
0020
000102030405060708090a0b0c0d0e0f
101112131415161718191a1b1c1d1e1f
00000001
01
02
0001
0020
a0a1a2a3a4a5a6a7a8a9aaabacadaeaf
b0b1b2b3b4b5b6b7b8b9babbbcbdbebf
0000000000000001
00
```

Concatenated:

```text
41432d434f4d4d49544d454e542d56310001001041432d5250530000000000000000000000010020000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f00000001010200010020a0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebf000000000000000100
```

## Expected digest

BLAKE2b with a 32-byte output produces:

```text
d533a9e7051d5ae4b87dd3aa8068dc1572edd621b3e0bf4e1226b37c4203ca5d
```

## Independent reproduction

The value was independently computed on 2026-07-21 with CPython 3.12.13 using
the standard-library call `hashlib.blake2b(preimage, digest_size=32)`. The C
test embeds the complete expected digest and cross-checks the repository's
libsodium-backed implementation against it. Python is used only to reproduce
the published vector; it is not a project runtime or build dependency.
