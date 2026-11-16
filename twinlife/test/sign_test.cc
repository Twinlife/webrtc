#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "twinlife_crypto.h"

#define MAX_SIZE 256

// using namespace twinlife;
using twinlife::CryptoKey;
using twinlife::CryptoBox;

static int failures = 0;

// Count the failed checks: main() returns an error when there is one.
static const char* check(bool ok)
{
  if (!ok) {
    failures++;
  }
  return ok ? "OK" : "KO";
}

long perf_end(struct timespec *start)
{
  struct timespec now;
  long ns;

  clock_gettime(CLOCK_MONOTONIC, &now);
  if (now.tv_nsec > start->tv_nsec) {
    ns = now.tv_nsec - start->tv_nsec;
  } else {
    ns = now.tv_nsec - start->tv_nsec + 1000000000L;
  }
  return ns;
}

void print_time(long ns)
{
  if (ns < 1000) {
    printf("%3ld ns  ", ns);
  } else if (ns < 1000000L) {
    printf("%3ld.%1ld us", (ns / 1000L), ((ns % 1000L) / 100L));
  } else {
    printf("%3ld.%1ld ms", (ns / 1000000L), ((ns % 1000000L) / 100000L));    
  }
}

int perf_report(struct timespec* start, const char* msg)
{
  struct timespec now;
  long ns;

  clock_gettime(CLOCK_MONOTONIC, &now);
  if (now.tv_nsec > start->tv_nsec) {
    ns = now.tv_nsec - start->tv_nsec;
  } else {
    ns = now.tv_nsec - start->tv_nsec + 1000000000L;
  }
  if (ns < 1000) {
    printf("%-40.40s    %3.3ld ns\n", msg, ns);
  } else if (ns < 1000000L) {
    printf("%-40.40s    %3.3ld.%1ld us\n", msg, (ns / 1000L), ((ns % 1000L) / 100L));
  } else {
    printf("%-40.40s    %3.3ld.%1ld ms\n", msg, (ns / 1000000L), ((ns % 1000000L) / 100000L));    
  }
  return 0;
}

int report_perf(const char* msg, long binary_time, long base64_time)
{
  printf("%-50.50s ", msg);
  print_time(binary_time);
  printf("     ");
  print_time(base64_time);
  printf("\n");
  return 0;
}

int report_length(const char* msg, size_t binary_size, size_t base64_size)
{
  printf("%-50.50s %8ld     %8ld\n", msg, binary_size, base64_size);
  return 0;
}

int test_sign_ED25519()
{
  CryptoKey *sec = twinlife::CryptoKey::create<CryptoKey>(twinlife::CryptoKey::ED25519);
  CryptoKey *import;
  unsigned char buf[MAX_SIZE];
  unsigned char sign_binary[MAX_SIZE];
  unsigned char sign_base64[MAX_SIZE];
  unsigned char data[MAX_SIZE];
  int len;
  int sign_binary_len, sign_base64_len;
  struct timespec start;
  long binary_time, base64_time;
  long binary_len, base64_len;

  memset(data, 1, sizeof(data));
  clock_gettime(CLOCK_MONOTONIC, &start);
  binary_len = sign_binary_len = sec->signED25519(CryptoKey::Format::BINARY, data, MAX_SIZE, sign_binary, sizeof(sign_binary));
  binary_time = perf_end(&start);

  clock_gettime(CLOCK_MONOTONIC, &start);
  base64_len = sign_base64_len = sec->signED25519(CryptoKey::Format::BASE64, data, MAX_SIZE, sign_base64, sizeof(sign_base64));
  base64_time = perf_end(&start);

  report_perf("signED25519", binary_time, base64_time);
  report_length("ED25519 signature len", binary_len, base64_len);

  clock_gettime(CLOCK_MONOTONIC, &start);
  binary_len = sec->exportPrivateKey(CryptoKey::Format::BINARY, buf, sizeof(buf));
  binary_time = perf_end(&start);

  import = CryptoKey::importPrivateKey<CryptoKey>(CryptoKey::Format::BINARY, CryptoKey::Kind::ED25519, buf, binary_len);
  if (!import) {
    fprintf(stderr, "ED25519 Import private key failed\n");
    exit(3);
  } else {
    int import_len = import->exportPrivateKey(CryptoKey::Format::BINARY, buf, sizeof(buf));
    if (import_len < 0 || import_len != binary_len) {
      fprintf(stderr, "ED25519 Import private key incorrect\n");
      exit(3);
    }
    delete import;
  }

  clock_gettime(CLOCK_MONOTONIC, &start);
  base64_len = sec->exportPrivateKey(CryptoKey::Format::BASE64, buf, sizeof(buf));
  base64_time = perf_end(&start);

  report_perf("ED25519 exportPrivateKey", binary_time, base64_time);
  report_length("ED25519 private key len", binary_len, base64_len);

  clock_gettime(CLOCK_MONOTONIC, &start);
  binary_len = sec->exportPublicKey(CryptoKey::Format::BINARY, buf, sizeof(buf));
  binary_time = perf_end(&start);

  clock_gettime(CLOCK_MONOTONIC, &start);
  base64_len = sec->exportPublicKey(CryptoKey::Format::BASE64, buf, sizeof(buf));
  base64_time = perf_end(&start);

  report_perf("ED25519 exportPublicKey", binary_time, base64_time);
  report_length("ED25519 public key len", binary_len, base64_len);

  len = sec->exportPublicKey(CryptoKey::Format::BINARY, buf, sizeof(buf));
  clock_gettime(CLOCK_MONOTONIC, &start);
  CryptoKey *verifier_binary = CryptoKey::importPublicKey<CryptoKey>(CryptoKey::Format::BINARY, CryptoKey::Kind::ED25519, buf, len);
  binary_time = perf_end(&start);

  len = sec->exportPublicKey(CryptoKey::Format::BASE64, buf, sizeof(buf));
  clock_gettime(CLOCK_MONOTONIC, &start);
  CryptoKey *verifier_base64 = CryptoKey::importPublicKey<CryptoKey>(CryptoKey::Format::BASE64, CryptoKey::Kind::ED25519, buf, len);
  base64_time = perf_end(&start);

  report_perf("ED25519 importPublicKey", binary_time, base64_time);

  clock_gettime(CLOCK_MONOTONIC, &start);
  int result = verifier_binary->verifyED25519(CryptoKey::Format::BINARY, data, MAX_SIZE, sign_binary, sign_binary_len);
  binary_time = perf_end(&start);
  printf("Verify result: %d (%s)\n", result, check(result == 1));

  clock_gettime(CLOCK_MONOTONIC, &start);
  result = verifier_base64->verifyED25519(CryptoKey::Format::BASE64, data, MAX_SIZE, sign_base64, sign_base64_len);

  report_perf("ECDSA verifyECDSA", binary_time, base64_time);
  printf("Verify result: success: %d (%s)", result, check(result == 1));

  result = verifier_binary->verifyED25519(CryptoKey::Format::BINARY, data, MAX_SIZE, sign_binary, sign_binary_len - 1);
  printf(" short signature: %d (%s)", result, check(result == 0));

  result = verifier_binary->verifyED25519(CryptoKey::Format::BINARY, data, MAX_SIZE - 1, sign_binary, sign_binary_len);
  printf(" short data: %d (%s)", result, check(result == 0));

  data[1] = 2;
  result = verifier_binary->verifyED25519(CryptoKey::Format::BINARY, data, MAX_SIZE, sign_binary, sign_binary_len);
  printf(" corruption: %d (%s)\n", result, check(result == 0));

  // We cannot sign with the public key.
  result = verifier_binary->signED25519(CryptoKey::Format::BINARY, data, MAX_SIZE, sign_binary, sizeof(sign_binary));
  printf("ED25519 sign with public key: %d     %s\n", result, check(result == TWINLIFE_SIGN_ERROR));

  CryptoKey *sec2 = twinlife::CryptoKey::create<CryptoKey>(twinlife::CryptoKey::ED25519);
  len = sec2->signAuth(verifier_binary, "ABCDE", "FGHIJKLMN", data, sizeof(data));
  if (len <= 0) {
    fprintf(stderr, "Invalid sign auth: %d\n", len);
    exit(2);
  }
  printf("Auth: %s\n", data);
  len = sec2->verifyAuth(verifier_binary, "ABCDE", "FGHIJKLMN", (char*)data);
  if (len != 1) {
    fprintf(stderr, "Error %d\n", len);
    exit (3);
  }

  delete verifier_binary;
  delete verifier_base64;
  delete sec;
  delete sec2;
  return 0;
}

int test_crypt_X25519()
{
  CryptoKey *sec = twinlife::CryptoKey::create<CryptoKey>(twinlife::CryptoKey::X25519);
  unsigned char buf[MAX_SIZE];
  unsigned char data[MAX_SIZE];
  int len;
  struct timespec start;
  long binary_time, base64_time;
  long binary_len, base64_len;

  memset(data, 1, sizeof(data));

  clock_gettime(CLOCK_MONOTONIC, &start);
  binary_len = sec->exportPrivateKey(CryptoKey::Format::BINARY, buf, sizeof(buf));
  binary_time = perf_end(&start);

  clock_gettime(CLOCK_MONOTONIC, &start);
  base64_len = sec->exportPrivateKey(CryptoKey::Format::BASE64, buf, sizeof(buf));
  base64_time = perf_end(&start);

  report_perf("X25519 exportPrivateKey", binary_time, base64_time);
  report_length("X25519 private key len", binary_len, base64_len);

  clock_gettime(CLOCK_MONOTONIC, &start);
  binary_len = sec->exportPublicKey(CryptoKey::Format::BINARY, buf, sizeof(buf));
  binary_time = perf_end(&start);

  clock_gettime(CLOCK_MONOTONIC, &start);
  base64_len = sec->exportPublicKey(CryptoKey::Format::BASE64, buf, sizeof(buf));
  base64_time = perf_end(&start);

  report_perf("X25519 exportPublicKey", binary_time, base64_time);
  report_length("X25519 public key len", binary_len, base64_len);

  unsigned char crypt[1024];
  unsigned char nonce[12];
  unsigned char auth[32];
  uint64_t nonceSequence = 2345;

  memset(auth, 2, sizeof(auth));
  memset(nonce, 0, sizeof(nonce));
  strcpy((char*) data, "Hello world\n");

  CryptoKey *peer = CryptoKey::create<CryptoKey>(CryptoKey::X25519);
  CryptoBox *cipher = CryptoBox::create<CryptoBox>(CryptoBox::AES_GCM);
  clock_gettime(CLOCK_MONOTONIC, &start);
  int result = cipher->bind(true, peer, sec, nonce, 10);
  binary_time = perf_end(&start);
  printf("bind result: %d    %s\n", result, check(result == 1));
  report_perf("X25519 bind", binary_time, binary_time);

  clock_gettime(CLOCK_MONOTONIC, &start);
  int crypt_len = cipher->encryptAEAD(data, MAX_SIZE, auth, 10, nonceSequence, crypt, sizeof(crypt));
  binary_time = perf_end(&start);
  report_perf("X25519 encryptAEAD", binary_time, binary_time);
  printf("Encrypt length=%d size=%d\n", result, MAX_SIZE);

  len = peer->exportPublicKey(CryptoKey::Format::BINARY, buf, sizeof(buf));

  unsigned char decrypt[1024];
  CryptoKey *peerPubKey = CryptoKey::importPublicKey<CryptoKey>(CryptoKey::Format::BINARY, CryptoKey::Kind::X25519, buf, len);
  result = cipher->bind(false, sec, peerPubKey, nonce, 10);

  clock_gettime(CLOCK_MONOTONIC, &start);
  result = cipher->decryptAEAD(crypt, crypt_len, 10, nonceSequence, decrypt, sizeof(crypt));
  binary_time = perf_end(&start);
  report_perf("X25519 decryptAEAD", binary_time, binary_time);
  printf("Decrypt length=%d size=%d    %s\n", result, MAX_SIZE,
         check(result == MAX_SIZE && memcmp(decrypt, data, MAX_SIZE) == 0));

  delete sec;
  delete peer;
  delete peerPubKey;
  delete cipher;
  return 0;
}

int test_sign_ECDSA()
{
  CryptoKey *sec = twinlife::CryptoKey::create<CryptoKey>(CryptoKey::Kind::ECDSA);
  unsigned char buf[MAX_SIZE];
  unsigned char sign_binary[MAX_SIZE];
  unsigned char sign_base64[MAX_SIZE];
  unsigned char data[MAX_SIZE];
  int len;
  int sign_binary_len, sign_base64_len;
  struct timespec start;
  long binary_time, base64_time;
  long binary_len, base64_len;

  memset(data, 1, sizeof(data));
  clock_gettime(CLOCK_MONOTONIC, &start);
  binary_len = sign_binary_len = sec->signECDSA(CryptoKey::Format::BINARY, data, MAX_SIZE, sign_binary, sizeof(sign_binary));
  binary_time = perf_end(&start);

  clock_gettime(CLOCK_MONOTONIC, &start);
  base64_len = sign_base64_len = sec->signECDSA(CryptoKey::Format::BASE64, data, MAX_SIZE, sign_base64, sizeof(sign_base64));
  base64_time = perf_end(&start);

  report_perf("signECDSA", binary_time, base64_time);
  report_length("ECDSA signature len", binary_len, base64_len);

  clock_gettime(CLOCK_MONOTONIC, &start);
  binary_len = sec->exportPrivateKey(CryptoKey::Format::BINARY, buf, sizeof(buf));
  binary_time = perf_end(&start);

  clock_gettime(CLOCK_MONOTONIC, &start);
  base64_len = sec->exportPrivateKey(CryptoKey::Format::BASE64, buf, sizeof(buf));
  base64_time = perf_end(&start);

  report_perf("ECDSA exportPrivateKey", binary_time, base64_time);
  report_length("ECDSA private key len", binary_len, base64_len);
  
  clock_gettime(CLOCK_MONOTONIC, &start);
  binary_len = sec->exportPublicKey(CryptoKey::Format::BINARY, buf, sizeof(buf));
  binary_time = perf_end(&start);

  clock_gettime(CLOCK_MONOTONIC, &start);
  base64_len = sec->exportPublicKey(CryptoKey::Format::BASE64, buf, sizeof(buf));
  base64_time = perf_end(&start);

  report_perf("ECDSA exportPublicKey", binary_time, base64_time);
  report_length("ECDSA public key len", binary_len, base64_len);

  len = sec->exportPublicKey(CryptoKey::Format::BINARY, buf, sizeof(buf));
  clock_gettime(CLOCK_MONOTONIC, &start);
  CryptoKey *verifier_binary = CryptoKey::importPublicKey<CryptoKey>(CryptoKey::Format::BINARY, CryptoKey::Kind::ECDSA, buf, len);
  base64_time = perf_end(&start);

  len = sec->exportPublicKey(CryptoKey::Format::BASE64, buf, sizeof(buf));
  clock_gettime(CLOCK_MONOTONIC, &start);
  CryptoKey *verifier_base64 = CryptoKey::importPublicKey<CryptoKey>(CryptoKey::Format::BASE64, CryptoKey::Kind::ECDSA, buf, len);
  base64_time = perf_end(&start);

  report_perf("ECDSA importPublicKey", binary_time, base64_time);

  clock_gettime(CLOCK_MONOTONIC, &start);
  int result = verifier_binary->verifyECDSA(CryptoKey::Format::BINARY, data, MAX_SIZE, sign_binary, sign_binary_len);
  binary_time = perf_end(&start);
  printf("Verify result: %d (%s)\n", result, check(result == 1));

  clock_gettime(CLOCK_MONOTONIC, &start);
  result = verifier_base64->verifyECDSA(CryptoKey::Format::BASE64, data, MAX_SIZE, sign_base64, sign_base64_len);
  base64_time = perf_end(&start);

  report_perf("ECDSA verifyECDSA", binary_time, base64_time);
  printf("Verify result: %d (%s)", result, check(result == 1));

  result = verifier_binary->verifyECDSA(CryptoKey::Format::BINARY, data, MAX_SIZE, sign_binary, sign_binary_len - 1);
  printf(" short signature: %d (%s)", result, check(result == 0));

  result = verifier_binary->verifyECDSA(CryptoKey::Format::BINARY, data, MAX_SIZE - 1, sign_binary, sign_binary_len);
  printf(" short data: %d (%s)", result, check(result == 0));

  data[1] = 2;
  result = verifier_binary->verifyECDSA(CryptoKey::Format::BINARY, data, MAX_SIZE, sign_binary, sign_binary_len);
  printf(" corruption: %d (%s)\n", result, check(result == 0));

  // We cannot sign with the public key.
  result = verifier_binary->signECDSA(CryptoKey::Format::BINARY, data, MAX_SIZE, sign_binary, sizeof(sign_binary));
  printf("sign result: %d     %s\n", result, check(result == TWINLIFE_SIGN_ERROR));

  unsigned char crypt[1024];
  unsigned char salt[12];
  unsigned char auth[32];
  uint64_t nonceSequence = 2345;

  memset(auth, 2, sizeof(auth));
  memset(salt, 0, sizeof(salt));
  strcpy((char*) data, "Hello world\n");

  CryptoKey *peer = CryptoKey::create<CryptoKey>(CryptoKey::Kind::ECDSA);
  CryptoBox *cipher = CryptoBox::create<CryptoBox>(CryptoBox::AES_GCM);
  clock_gettime(CLOCK_MONOTONIC, &start);
  result = cipher->bind(true, peer, verifier_binary, salt, sizeof(salt));
  binary_time = perf_end(&start);
  printf("bind result: %d    %s\n", result, check(result == 1));
  report_perf("ECDSA bind", binary_time, binary_time);

  clock_gettime(CLOCK_MONOTONIC, &start);
  int crypt_len = cipher->encryptAEAD(data, MAX_SIZE, auth, 10, nonceSequence, crypt, sizeof(crypt));
  binary_time = perf_end(&start);
  report_perf("ECDSA encryptAEAD", binary_time, binary_time);
  printf("Encrypt length=%d size=%d\n", crypt_len, MAX_SIZE);

  len = peer->exportPublicKey(CryptoKey::Format::BINARY, buf, sizeof(buf));

  unsigned char decrypt[1024];
  CryptoKey *peerPubKey = CryptoKey::importPublicKey<CryptoKey>(CryptoKey::Format::BINARY, CryptoKey::Kind::ECDSA, buf, len);
  cipher->bind(false, sec, peerPubKey, salt, sizeof(salt));

  clock_gettime(CLOCK_MONOTONIC, &start);
  result = cipher->decryptAEAD(crypt, crypt_len, 10, nonceSequence, decrypt, sizeof(crypt));
  binary_time = perf_end(&start);
  printf("Decrypt length=%d size=%d    %s\n", result, MAX_SIZE,
         check(result == MAX_SIZE && memcmp(decrypt, data, MAX_SIZE) == 0));
  report_perf("ECDSA decryptAEAD", binary_time, binary_time);

  result = cipher->decryptAEAD(crypt, crypt_len - 1, 10, nonceSequence, decrypt, sizeof(crypt));
  printf("ECDSA decrypt error  %d    %s\n", result, check(result == TWINLIFE_AEAD_FAIL));

  // Change a bit in the auth area
  crypt[0] ^= 0x10;
  result = cipher->decryptAEAD(crypt, crypt_len, 10, nonceSequence, decrypt, sizeof(crypt));
  printf("ECDSA decrypt error  %d    %s\n", result, check(result == TWINLIFE_AEAD_FAIL));

  // Change a bit in the encrypted area
  crypt[0] ^= 0x10;
  crypt[crypt_len - 1] ^= 0x10;
  result = cipher->decryptAEAD(crypt, crypt_len, 10, nonceSequence, decrypt, sizeof(crypt));
  printf("ECDSA decrypt error  %d    %s\n", result, check(result == TWINLIFE_AEAD_FAIL));

  CryptoKey *sec2 = twinlife::CryptoKey::create<CryptoKey>(twinlife::CryptoKey::ED25519);
  len = sec2->signAuth(verifier_binary, "ABCDE", "FGHIJKLMN", data, sizeof(data));
  if (len <= 0) {
    fprintf(stderr, "Invalid sign auth: %d\n", len);
    exit(2);
  }
  printf("Auth: %s\n", data);
  len = sec2->verifyAuth(verifier_binary, "ABCDE", "FGHIJKLMN", (char*)data);
  if (len != 1) {
    fprintf(stderr, "Error %d\n", len);
    exit(3);
  }

  len = sec2->extractAuthPublicKey((const char*)data, buf, sizeof(buf));
  if (len <= 0) {
    fprintf(stderr, "extractAuthPublicKey failed: %d\n", len);
    exit(4);
  }
  delete verifier_binary;

  verifier_binary = CryptoKey::importPublicKey<CryptoKey>(CryptoKey::Format::BINARY, CryptoKey::Kind::ED25519, buf, len);
  if (!verifier_binary) {
    fprintf(stderr, "extractAuthPublicKey invalid public key\n");
    exit(4);
  }

  delete verifier_binary;
  delete verifier_base64;
  delete sec;
  delete peer;
  delete peerPubKey;
  delete cipher;
  delete sec2;
  return 0;
}

int test_crypt_AEAD()
{
  CryptoBox *sec = twinlife::CryptoBox::create<CryptoBox>(CryptoBox::Kind::CHACHA20_POLY1305);
  unsigned char data[MAX_SIZE];
  struct timespec start;
  long binary_time;

  unsigned char crypt[1024];
  unsigned char nonce[12];
  unsigned char auth[32];
  unsigned char key[32];
  uint64_t nonceSequence = 1234;

  memset(data, 0, sizeof(data));
  memset(auth, 2, sizeof(auth));
  memset(nonce, 0, sizeof(nonce));
  strcpy((char*) data, "Hello world\n");
  for (int i = 0; i < 32; i++) {
    key[i] = i;
  }

  clock_gettime(CLOCK_MONOTONIC, &start);
  int result = sec->bind(key, sizeof(key));
  binary_time = perf_end(&start);
  printf("bind result: %d    %s\n", result, check(result == 1));
  report_perf("Binary bind", binary_time, binary_time);

  clock_gettime(CLOCK_MONOTONIC, &start);
  int crypt_len = sec->encryptAEAD(data, MAX_SIZE, auth, 10, nonceSequence,
                                   crypt, sizeof(crypt));
  binary_time = perf_end(&start);
  report_perf("Binary encryptAEAD", binary_time, binary_time);
  printf("Encrypt length=%d size=%d\n", crypt_len, MAX_SIZE);

  unsigned char decrypt[1024];
  clock_gettime(CLOCK_MONOTONIC, &start);
  result = sec->decryptAEAD(crypt, crypt_len, 10, nonceSequence, decrypt, sizeof(crypt));
  binary_time = perf_end(&start);
  printf("Decrypt length=%d size=%d    %s\n", result, MAX_SIZE,
         check(result == MAX_SIZE && memcmp(decrypt, data, MAX_SIZE) == 0));
  report_perf("ECDSA decryptAEAD", binary_time, binary_time);

  result = sec->decryptAEAD(crypt, crypt_len - 1, 10, nonceSequence, decrypt, sizeof(crypt));
  printf("ECDSA decrypt error  %d    %s\n", result, check(result == TWINLIFE_AEAD_FAIL));

  // Change a bit in the auth area
  crypt[0] ^= 0x10;
  result = sec->decryptAEAD(crypt, crypt_len, 10, nonceSequence, decrypt, sizeof(crypt));
  printf("ECDSA decrypt error  %d    %s\n", result, check(result == TWINLIFE_AEAD_FAIL));

  // Change a bit in the encrypted area
  crypt[0] ^= 0x10;
  crypt[crypt_len - 1] ^= 0x10;
  result = sec->decryptAEAD(crypt, crypt_len, 10, nonceSequence, decrypt, sizeof(crypt));
  printf("ECDSA decrypt error  %d    %s\n", result, check(result == TWINLIFE_AEAD_FAIL));

  delete sec;
  return 0;
}

int verify() {
  const char* sig = "1rBiUx8E3oo-1ebdxqMDkD_l1Z-AaULwtnLp8HxerOA.Zfi5JBDaps3LcwMZv-nAEYWH6Z6rqetT3ACxgLGrZb8.BEBK0bM3LC3Dlupo4PNmO80DHXd3vRxf6_ALt3w63cOqJgQYGN_JR0moB8N_oJqmNYDQaH1feQ-Ov2PoqSu-Cw";
  int len;
  unsigned char pubKey[64];

  len = CryptoKey::extractAuthPublicKey(sig, pubKey, sizeof(pubKey));
  if (len <= 0) {
    fprintf(stderr, "Invalid signature\n");
    exit (10);
  }

  CryptoKey *pkey = CryptoKey::importPublicKey<CryptoKey>(CryptoKey::Format::BINARY, CryptoKey::Kind::ED25519, pubKey, len);
  if (!pkey) {
    fprintf(stderr, "Key creation failed\n");
    exit (12);
  }
  const char* p = strchr(sig, '.');
  if (!p) {
    delete pkey;
    return TWINLIFE_BAD_SIGNATURE;
  }
  len = (size_t) (p - sig);
  if (len >= TWINLIFE_MAX_SIZE) {
    delete pkey;
    return TWINLIFE_BAD_SIGNATURE;
  }
  const char* sigStart = strchr(&p[1], '.');
  sigStart++;
  unsigned char hash[TWINLIFE_MAX_SIZE];
  len = CryptoKey::decodeBase64((const unsigned char*)sig, 43, hash, sizeof(hash));
  if (len != 32) {
    delete pkey;
    return TWINLIFE_BAD_SIGNATURE;
  }
  int result = pkey->verify(CryptoKey::Format::BASE64, hash, len, (const unsigned char*)sigStart, strlen(sigStart));
  fprintf(stderr, "Verify: %d\n", result);
  delete pkey;
  return result;
}

int main()
{
  test_sign_ECDSA();
  printf("\n");
  test_sign_ED25519();
  printf("\n");
  test_crypt_X25519();
  printf("\n");
  test_crypt_AEAD();
  printf("\n");

  // A signature made by an earlier build: it fails when the signature format changes.
  int result = verify();
  printf("Known signature: %d (%s)\n", result, check(result == 1));
  if (failures > 0) {
    fprintf(stderr, "%d check(s) failed\n", failures);
    return 1;
  }
  return 0;
}

