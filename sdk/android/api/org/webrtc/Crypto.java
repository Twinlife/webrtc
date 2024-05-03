package org.webrtc;

public class Crypto {
    private long nativeCrypto; // pointer to the webrtc::jni::Crypto* instance

    // Error codes as defined in twinlife_crypto.h
    public static final int BAD_PARAM = (-1);
    public static final int TOO_SMALL = (-2);
    public static final int BAD_ALLOC = (-3);
    public static final int BAD_EC_KEY = (-4);
    public static final int SIGN_ERROR = (-5);
    public static final int BAD_SIGNATURE = (-6);
    public static final int AEAD_FAIL = (-7);
    public static final int NONCE_ERROR = (-8);

    public static final int NONCE_LENGTH = 12;
    public static final int MAX_KEY_LENGTH = 256;
    public static final int MAX_SIG_LENGTH = 128;

    @CalledByNative
    public Crypto(long nativeCrypto) {
        this.nativeCrypto = nativeCrypto;
    }

    public static Crypto create() {
        return nativeCreate();
    }

    public static Crypto importPublicKey(byte[] pubKey, boolean isBase64) {
        return nativeImportPublicKey(pubKey, isBase64);
    }

    public static Crypto importPrivateKey(byte[] privateKey, boolean isBase64) {
        return nativeImportPrivateKey(privateKey, isBase64);
    }

    public byte[] getPublicKey(boolean useBase64) {
        checkCryptoExists();
        return nativeGetPublicKey(nativeCrypto, useBase64);
    }

    public byte[] getPrivateKey(boolean useBase64) {
        checkCryptoExists();
        return nativeGetPrivateKey(nativeCrypto, useBase64);
    }

    /**
     * Prepare for use of AEAD with the peer's public key.  Derive a shared secret based on the private key
     * and peer's public key, compute the SHA256 digest of that secret, setup the AEAD internal context
     * to be ready to use `encryptAEAD` or `decryptAEAD`.  The `bind` is a costly operation compared
     * to encryption and decryption.  The encryption nonce is pre-initialized with the given buffer
     * and will be incremented before each encryptAEAD() a maximum of `maxIncrement` times.
     *
     * @param peerPublicKey
     * @param nonce
     * @param maxIncrement
     * @return 0 or an error code.
     */
    public int bind(Crypto peerPublicKey, byte[] nonce, int maxIncrement) {
        checkCryptoExists();
        peerPublicKey.checkCryptoExists();
        return nativeBind(nativeCrypto, peerPublicKey.nativeCrypto, nonce, maxIncrement);
    }

    /**
     * Unbind with peer's public key and release the AEAD context.  This operation must be called when
     * encryption and decryption are not necessary any more.
     *
     * @return 0 or an error code.
     */
    public int unbind() {
        checkCryptoExists();
        return nativeUnbind(nativeCrypto);
    }

    /**
     * Setup a new nonce for encryptAEAD().
     *
     * @param nonce the new nonce buffer of size NONCE_LENGTH
     * @param maxIncrement
     * @return 0 or an error code.
     */
    public int newNonce(byte[] nonce, int maxIncrement) {
        checkCryptoExists();
        return nativeNewNonce(nativeCrypto, nonce, maxIncrement);
    }

    /**
     * Sign the content of the data buffer with the private key and encode the ECDSA signature in Base64
     * in the signature buffer.
     *
     * @param data the data buffer to verify.
     * @param signature the output signature buffer (Must be large enough).
     * @return Return the length of the signature or a negative error code.
     */
    public int signECDSA(byte[] data, byte[] signature) {
        checkCryptoExists();
        return nativeSignECDSA(nativeCrypto, data, signature);
    }

    /**
     * Verify with the public key that the data buffer corresponds to the Base64 ECDSA signature.
     *
     * @param data the data buffer to verify.
     * @param signature the signature.
     * @return 1 if the signature is verified, 0 if the data does not match or a negative error code.
     */
    public int verifyECDSA(byte[] data, byte[] signature) {
        checkCryptoExists();
        return nativeVerifyECDSA(nativeCrypto, data, signature);
    }

    /**
     * Encrypt and sign with AES256-GCM the data buffer and auth buffer with a new nonce.
     * Only the data buffer is encrypted.  The nonce buffer will be filled with a new nonce of 12 bytes.
     *
     * @param data
     * @param auth
     * @param nonce
     * @param output
     * @return the length of the output buffer or a negative error code.
     */
    public int encryptAEAD(byte[] data, byte[] auth, byte[] output) {
        checkCryptoExists();
        return nativeEncryptAEAD(nativeCrypto, data, auth, output);
    }

    /**
     * Decrypt and verify the data with AES256-GCM.  Only the encryptedData buffer is decrypted.
     *
     * @param encrypted
     * @param auth
     * @param nonce
     * @param output
     * @return
     */
    public int decryptAEAD(byte[] data, int authLength, byte[] output) {
        checkCryptoExists();
        return nativeDecryptAEAD(nativeCrypto, data, authLength, output);
    }

    public void dispose() {
        long n = nativeCrypto;
        if (n != 0) {
            nativeCrypto = 0;
            nativeDispose(n);            
        }
    }

    private void checkCryptoExists() {
        if (nativeCrypto == 0) {
            throw new IllegalStateException("Crypto has been disposed.");
        }
    }

    private static native Crypto nativeCreate();
    private static native Crypto nativeImportPublicKey(byte[] pubKey, boolean isBase64);
    private static native Crypto nativeImportPrivateKey(byte[] privateKey, boolean isBase64);
    private static native byte[] nativeGetPublicKey(long nativeCrypto, boolean useBase64);
    private static native byte[] nativeGetPrivateKey(long nativeCrypto, boolean useBase64);
    private static native int nativeBind(long nativeCrypto, long nativeBindCrypto, byte[] nonce, int maxIncrement); // 
    private static native int nativeUnbind(long nativeCrypto);
    private static native int nativeNewNonce(long nativeCrypto, byte[] nonce, int maxIncrement);
    private static native int nativeSignECDSA(long nativeCrypto, byte[] data, byte[] signature);
    private static native int nativeVerifyECDSA(long nativeCrypto, byte[] data, byte[] signature);
    private static native int nativeEncryptAEAD(long nativeCrypto, byte[] data, byte[] auth, byte[] output);
    private static native int nativeDecryptAEAD(long nativeCrypto, byte[] data, int authLength, byte[] output);
    private static native void nativeDispose(long nativeCrypto);
}
