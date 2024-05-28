package org.twinlife.twinlife.crypto;

import androidx.annotation.NonNull;

import org.webrtc.CalledByNative;

public class CryptoKey {
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

    public enum Kind {
        ECDSA,
        ED25519,
        X25519
    };

    @CalledByNative
    public CryptoKey(long nativeCrypto) {
        this.nativeCrypto = nativeCrypto;
    }

    public static CryptoKey create(@NonNull Kind kind) {
        return nativeCreate(kind.ordinal());
    }

    public static CryptoKey importPublicKey(@NonNull Kind kind, byte[] pubKey, boolean isBase64) {
        return nativeImportPublicKey(kind.ordinal(), pubKey, isBase64);
    }

    public static CryptoKey importPrivateKey(@NonNull Kind kind, byte[] privateKey, boolean isBase64) {
        return nativeImportPrivateKey(kind.ordinal(), privateKey, isBase64);
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
     * Sign the content of the data buffer with the private key and encode the ECDSA signature in Base64
     * in the signature buffer.
     *
     * @param data the data buffer to verify.
     * @param signature the output signature buffer (Must be large enough).
     * @return Return the length of the signature or a negative error code.
     */
    public int sign(byte[] data, byte[] signature, boolean isBase64) {
        checkCryptoExists();
        return nativeSign(nativeCrypto, data, signature, isBase64);
    }

    /**
     * Verify with the public key that the data buffer corresponds to the Base64 ECDSA signature.
     *
     * @param data the data buffer to verify.
     * @param signature the signature.
     * @return 1 if the signature is verified, 0 if the data does not match or a negative error code.
     */
    public int verify(byte[] data, byte[] signature, boolean isBase64) {
        checkCryptoExists();
        return nativeVerify(nativeCrypto, data, signature, isBase64);
    }

    public void dispose() {
        long n = nativeCrypto;
        if (n != 0) {
            nativeCrypto = 0;
            nativeDispose(n);            
        }
    }

    long internalCrypto() {
        return nativeCrypto;
    }

    void checkCryptoExists() {
        if (nativeCrypto == 0) {
            throw new IllegalStateException("Crypto has been disposed.");
        }
    }

    private static native CryptoKey nativeCreate(int kind);
    private static native CryptoKey nativeImportPublicKey(int kind, byte[] pubKey, boolean isBase64);
    private static native CryptoKey nativeImportPrivateKey(int kind, byte[] privateKey, boolean isBase64);
    private static native byte[] nativeGetPublicKey(long nativeCryptoKey, boolean useBase64);
    private static native byte[] nativeGetPrivateKey(long nativeCryptoKey, boolean useBase64);
    private static native int nativeSign(long nativeCryptoKey, byte[] data, byte[] signature, boolean isBase64);
    private static native int nativeVerify(long nativeCryptoKey, byte[] data, byte[] signature, boolean isBase64);
    private static native void nativeDispose(long nativeCryptoKey);
}
