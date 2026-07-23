# Bibliography Queue

## Purpose and Minimality Rule

This register records why a source is relevant before it enters the book. A
DOI is a persistent identifier for a publication; it is not software and it
does not become a project dependency. An RFC is a published technical
specification. Across Rock--Paper--Scissors, Hash Functions, the
manuscript-only Trust repair, the verified Tic-Tac-Toe public checker, and its
private-proof route-selection note, the only external C library remains
libsodium. The Tic-Tac-Toe targets use only the C11 standard library and do not
link that backend. The route selection adds papers, not software.

Add a citation only when it supports a sentence that the chapter actually
needs. A related paper must not be presented as proof of a different
construction. Metadata and claim scope are separate verification questions.

## RPS Slice Sources

| ID | Bibliography key | Source and stable identifier | Narrow use in this project | What it does **not** establish | Verification status |
|---|---|---|---|---|---|
| BQ-RPS-01 | `blum1983coinflipping` | Manuel Blum, “Coin Flipping by Telephone: A Protocol for Solving Impossible Problems,” *ACM SIGACT News* 15(1), 23--27 (1983), DOI `10.1145/1008908.1008911` | Historical context for using a commit/reveal-like chronology in remote joint choice | The exact v1 encoding, the nonce-and-hash construction, RPS fairness, or this project's novelty | Metadata cross-checked against the DOI-indexed ACM record and DBLP; used only for historical context |
| BQ-RPS-02 | `naor1991bitcommitment` | Moni Naor, “Bit Commitment Using Pseudorandomness,” *Journal of Cryptology* 4(2), 151--158 (1991), DOI `10.1007/BF00196774` | Formal background for the hiding/binding vocabulary and the existence of computational commitment constructions | The exact construction (H(\mathit{context}\|\mathit{nonce}\|\mathit{move})); Naor studies a different pseudorandom-generator construction | Metadata and abstract cross-checked against the author-affiliated IBM record and DOI-indexed journal metadata; construction mismatch explicitly recorded |
| BQ-RPS-03 | `saarinen2015blake2` | M.-J. Saarinen and J.-P. Aumasson, RFC 7693, “The BLAKE2 Cryptographic Hash and Message Authentication Code (MAC)” (2015), DOI `10.17487/RFC7693` | Algorithm specification and parameter reference for BLAKE2b | A proof that the v1 commitment is hiding, binding, fair, authenticated, or production-ready | Verified against the RFC Editor record and text, including authors, date, status, and DOI |
| BQ-RPS-04 | `libsodium-generichash` | libsodium documentation, “Generic Hashing,” `https://doc.libsodium.org/hashing/generic_hashing` | Documents that the selected API uses BLAKE2b and how the streaming API is called | Cryptographic proof or independent security evaluation | Checked against the official documentation on 2026-07-21; implementation documentation only |
| BQ-RPS-05 | `libsodium-randombytes` | libsodium documentation, “Generating Random Data,” `https://doc.libsodium.org/generating_random_data` | Documents `randombytes_buf()` and its operating-system-backed behavior | Proof that a particular host is uncompromised or that every generated nonce is unpredictable | Checked against the official documentation on 2026-07-21; implementation documentation only |

All five verified entries above are present in `document/references.bib`. The
chapter cites them sparingly and owns no `\bibliography{...}` declaration.

## Deliberately Deferred Sources

| ID | Candidate | Reason deferred | Reconsider when |
|---|---|---|---|
| BQ-RPS-D01 | Richard Cleve, “Limits on the Security of Coin Flips when Half the Processors are Faulty,” STOC 1986, pp. 364--369, DOI `10.1145/12130.12168` | The chapter demonstrates one concrete selective-abort trace. It does not need to inflate that trace into Cleve's broader impossibility theorem. | A later chapter states and carefully matches a formal two-party fairness theorem. |
| BQ-RPS-D02 | BLAKE2 design paper referenced by RFC 7693 | RFC 7693 is sufficient for the narrow algorithm/parameter statement in this chapter. | The hash appendix studies design rationale or cryptanalysis rather than merely specifying the backend. |
| BQ-RPS-D03 | A primary proof for the exact nonce-and-hash commitment used here | No matching primary proof was identified in this pass. Related commitment literature is not interchangeable with this construction. | A source with the same encoding and explicit hash model is found, or the construction is replaced by one with a directly applicable proof. |

Until BQ-RPS-D03 is resolved, the manuscript must use the label
“instructional hash-based construction with a random-oracle-style security
argument,” never “proved commitment scheme.”

## Hash Foundation Sources

| ID | Bibliography key | Source and stable identifier | Narrow use in this project | What it does **not** establish | Verification status |
|---|---|---|---|---|---|
| BQ-HASH-01 | `rogaway2004hashbasics` | Phillip Rogaway and Thomas Shrimpton, “Cryptographic Hash-Function Basics,” FSE 2004, DOI `10.1007/978-3-540-25937-4_24` | Primary definitions background showing that preimage, second-preimage, and collision resistance require distinct, precisely sampled games | Security of BLAKE2b, the exact AC commitment, or the simplified textbook games under every formulation | Author-hosted paper and DOI metadata checked on 2026-07-21; chapter uses it to warn about definitional precision |
| BQ-HASH-02 | `saarinen2015blake2` | RFC 7693, “The BLAKE2 Cryptographic Hash and Message Authentication Code (MAC),” DOI `10.17487/RFC7693` | BLAKE2b algorithm and parameter reference, including the 32-byte profile's target label | An independent security assertion or proof for BLAKE2b, this wrapper, or any surrounding protocol | RFC text checked; its explicit non-assertion is retained |
| BQ-HASH-03 | `rfc6151md5` | RFC 6151, “Updated Security Considerations for the MD5 Message-Digest and the HMAC-MD5 Algorithms,” DOI `10.17487/RFC6151` | Historical conclusion that MD5 is unsuitable where collision resistance is required | A universal ban on every legacy checksum use or a proof about BLAKE2b | RFC Editor text checked on 2026-07-21 |
| BQ-HASH-04 | `rfc2104hmac` | RFC 2104, “HMAC: Keyed-Hashing for Message Authentication,” DOI `10.17487/RFC2104` | Distinguishes the keyed HMAC construction from a bare unkeyed digest | That every use of a hash authenticates a sender or that the AC wrapper is a MAC | RFC Editor text checked on 2026-07-21 |
| BQ-HASH-05 | `nist800185` | NIST SP 800-185, “SHA-3 Derived Functions: cSHAKE, KMAC, TupleHash, and ParallelHash,” DOI `10.6028/NIST.SP.800-185` | A concrete standards example of explicit function naming, customization, and tuple-aware hashing | A requirement to adopt SHA-3 or a proof that any ad hoc encoding is safe | Official NIST publication metadata and text checked on 2026-07-21 |
| BQ-HASH-06 | `libsodium-generichash` | libsodium documentation, “Generic Hashing,” `https://doc.libsodium.org/hashing/generic_hashing` | Documents the BLAKE2b backend/API and warns that generic hashing is not password hashing | Independent cryptanalysis, password security, or protocol correctness | Official documentation checked on 2026-07-21; implementation documentation only |
| BQ-HASH-07 | `wang2005md5` | Xiaoyun Wang and Hongbo Yu, “How to Break MD5 and Other Hash Functions,” EUROCRYPT 2005, DOI `10.1007/11426639_2` | Historical collision-attack background | Provenance of every circulating collision byte string or any claim about BLAKE2b | DOI metadata checked; exact retained vector is independently executed rather than attributed solely to the paper |

The appendix cites only the entries needed near their claims. The executable
companion imports none of these publications as software.

## Trust Establishment Sources

| ID | Bibliography key | Source and stable identifier | Narrow use in this project | What it does **not** establish | Verification status |
|---|---|---|---|---|---|
| BQ-TRUST-01 | `nist80090b` | Meltem Sönmez Turan et al., NIST SP 800-90B, *Recommendation for the Entropy Sources Used for Random Bit Generation* (2018), DOI `10.6028/NIST.SP.800-90B` | Noise-source modeling, min-entropy assessment, deterministic-conditioning limits, documentation, and health testing | That a source is metaphysically "true random," that covariance establishes min-entropy, that the author's quantizer is valid, or that local randomness authenticates a peer | Official NIST publication page and PDF checked on 2026-07-21; the conditioned-output entropy and health-test requirements were matched to the cited prose |
| BQ-TRUST-02 | `rfc9846tls13` | Eric Rescorla, RFC 9846, *The Transport Layer Security (TLS) Protocol Version 1.3* (2026), DOI `10.17487/RFC9846` | A current official example separating key exchange, certificate/PSK authentication, key confirmation, handshake integrity, and external binding for otherwise unauthenticated operation | A new AC protocol, universal human identity, moral trust, availability, or proof that every unauthenticated exchange has identical details | RFC Editor metadata and text checked on 2026-07-21; RFC 9846 obsoletes RFC 8446 and is used only as an example |
| BQ-TRUST-03 | `maurer1993secretkey` | Ueli M. Maurer, *Secret Key Agreement by Public Discussion from Common Information*, IEEE Transactions on Information Theory 39(3), 733--742 (1993), DOI `10.1109/18.256484` | The exact repeated-source `X,Y,Z` model, authenticated-public-discussion assumption, and upper bound `S(X;Y || Z) <= min{I(X;Y),I(X;Y|Z)}` | Impossibility of computational key agreement, authentication over an actively modifiable channel, a market-source claim, or a universal achievable rate from positive mutual information | Author-hosted paper, theorem, channel assumptions, and DOI metadata checked on 2026-07-21 |
| BQ-TRUST-04 | `nist-randomness-beacons` | NIST, *Interoperable Randomness Beacons*, official project page | Public common randomness for auditable randomized procedures and the explicit warning not to use beacon outputs as secret cryptographic keys | Peer authentication, private shared randomness, universal resistance to operator bias, or any equivalence with a pre-shared secret | Official NIST project page checked on 2026-07-21; implementation is not imported |

The Trust chapter deliberately omits Diffie--Hellman history, a general AKE
literature survey, HKDF, NIST SP 800-56A, Shannon, and Maurer/Wolf surveys. None
is needed for an active sentence in this bounded taxonomy. Reopen a source only
when a later construction states a claim that requires it.

## Tic-Tac-Toe Public-Checker and Route-Selection Sources

| ID | Bibliography key | Source and stable identifier | Narrow use in this project | What it does **not** establish | Verification status |
|---|---|---|---|---|---|
| BQ-TTT-01 | `naor1991bitcommitment` | Moni Naor, “Bit Commitment Using Pseudorandomness,” *Journal of Cryptology* 4(2), 151--158 (1991), DOI `10.1007/BF00196774` | Reused only for formal commitment background when explaining the shape of a possible future committed-policy relation | The repository's nonce-and-hash construction, the Tic-Tac-Toe predicate, knowledge of an opening, zero knowledge, or any implemented private proof | Existing verified RPS entry reused with the same construction-mismatch warning; no new source or dependency |
| BQ-TTT-02 | `goldwasser1989knowledgecomplexity` | Shafi Goldwasser, Silvio Micali, and Charles Rackoff, “The Knowledge Complexity of Interactive Proof Systems,” *SIAM Journal on Computing* 18(1), 186--208 (1989), DOI `10.1137/0218012` | Foundational zero-knowledge and knowledge-complexity terminology used to distinguish a future proof property from public verification | The TTT relation, an extractor-based proof-of-knowledge guarantee for this project, a practical backend, composition, or this implementation | Official SIAM metadata and DOI verified; citation-to-claim scope audited |
| BQ-TTT-03 | `bellare1993definingknowledge` | Mihir Bellare and Oded Goldreich, “On Defining Proofs of Knowledge,” *Advances in Cryptology—CRYPTO ’92*, LNCS 740, 390--420, Springer (proceedings published 1993), DOI `10.1007/3-540-48071-4_28` | Extractor-based proof-of-knowledge definition used to explain why a commitment or accepting verifier alone does not establish knowledge | A TTT proof-of-knowledge instantiation, zero knowledge automatically, commitment knowledge, efficiency, or composition | Author-hosted paper, Springer metadata, DBLP, and DOI cross-checked; citation-to-claim scope audited |
| BQ-TTT-04 | `goldreich1991proofsvalidity` | Oded Goldreich, Silvio Micali, and Avi Wigderson, “Proofs That Yield Nothing but Their Validity or All Languages in NP Have Zero-Knowledge Proof Systems,” *Journal of the ACM* 38(3), 691--729 (1991), DOI `10.1145/116825.116852` | Conceptual existence result for zero-knowledge proofs for NP under the paper's stated assumptions; used only to motivate the later relation as a legitimate research direction | An efficient or concrete backend, proof of knowledge automatically, malicious composition, the AC commitment, or any property of the public C checker | IAS author page/PDF, ACM DOI metadata, and bibliographic metadata cross-checked; citation-to-claim scope audited |
| BQ-TTT-05 | `ishai2007zeroknowledgemp` | Yuval Ishai, Eyal Kushilevitz, Rafail Ostrovsky, and Amit Sahai, "Zero-Knowledge from Secure Multiparty Computation," *STOC '07*, 21--30, DOI `10.1145/1250790.1250794` | Foundational MPC-in-the-head construction and the dependence of communication on the circuit verifying the relation | A concrete AC protocol, ZKBoo's later three-party specialization, commitment security, or measured cost for the TTT relation | Author-hosted paper, institutional metadata, ACM DOI, title, authors, venue, year, and pages cross-checked |
| BQ-TTT-06 | `giacomelli2016zkboo` | Irene Giacomelli, Jesper Madsen, and Claudio Orlandi, "ZKBoo: Faster Zero-Knowledge for Boolean Circuits," *25th USENIX Security Symposium*, 1069--1083 (2016), stable USENIX proceedings URL | Conditional base classification in Proposition 4.2: in the commitment-hybrid model and for a correct 2-private `(2,3)`-decomposition, the basic three-message protocol has special honest-verifier zero knowledge and 3-special soundness; also used for Boolean-circuit, proof-internal view-commitment, and Fiat--Shamir boundaries | Malicious-verifier zero knowledge by itself, a full argument-of-knowledge transformation for AC, concrete view-commitment binding/hiding or compatibility with the external policy commitment, a total soundness bound, circuit equivalence, a proof-size estimate before gate counts, or safe reuse of the paper's C code | Official USENIX page, open paper, authors, pages, year, Proposition 4.2 conditions, and stated properties cross-checked |
| BQ-TTT-07 | `setty2020spartan` | Srinath Setty, "Spartan: Efficient and General-Purpose zkSNARKs Without Trusted Setup," *CRYPTO 2020, Part III*, LNCS 12172, 704--737, DOI `10.1007/978-3-030-56877-1_25` | Transparent R1CS argument-of-knowledge comparison and evidence that a succinct route entails a substantial polynomial-commitment and circuit stack | A selected dependency, compatibility with the AC commitment, an implementation recommendation, post-quantum security for every variant, or any property of the TTT checker | IACR proceedings paper, Microsoft Research page, DOI, volume, and pages cross-checked |

Primary metadata and stable identifiers for these entries were verified, and
each citation is limited to the distinction it supports. They are bibliography
only, not executable dependencies. MPC in the head is retained only as the
leading family for a later measured educational experiment; no concrete
zero-knowledge transformation, protocol instantiation, or production backend
is selected. The canonical chapter owns its `\cite{...}` calls; `section_ttt`
owns bibliography emission through the shared wrapper.

## Broader Book Queue

Phase 0 identified unresolved bibliography work for mental poker, Goodhart's
Law, standards beyond the repaired Hash appendix, and institutional claims.
Those sources remain outside the completed source gates. Their absence
must not be hidden by adding placeholder entries.
