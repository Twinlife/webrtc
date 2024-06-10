package org.twinlife.twinlife.crypto;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import org.webrtc.CalledByNative;

/**
 * Class to hold a public/private key and provide operations to sign or verify a signature.
 */
public class CryptoKey {
    private long nativeCrypto; // pointer to the webrtc::jni::CryptoKey* instance

    // Error codes as defined in twinlife_crypto.h
    public static final int BAD_PARAM = (-1);
    public static final int TOO_SMALL = (-2);
    public static final int BAD_ALLOC = (-3);
    public static final int BAD_EC_KEY = (-4);
    public static final int SIGN_ERROR = (-5);
    public static final int BAD_SIGNATURE = (-6);
    public static final int AEAD_FAIL = (-7);
    public static final int NONCE_ERROR = (-8);

    public static final int MAX_KEY_LENGTH = 256;
    public static final int MAX_SIG_LENGTH = 128;
    public static final int ECDSA_PUBKEY_LENGTH = 124;

    public enum Kind {
        ECDSA,
        ED25519,
        X25519
    };

    @CalledByNative
    public CryptoKey(long nativeCrypto) {
        this.nativeCrypto = nativeCrypto;
    }

    /**
     * Create and generate a new private/public keypair of the specified kind.
     *
     * @param kind the kind of private/public key to generate.
     * @return the new private/public keypair.
     */
    public static CryptoKey create(@NonNull Kind kind) {
        return nativeCreate(kind.ordinal());
    }

    /**
     * Import a public key of the specified type.  The public key is either encoded in binary
     * or Base64url.
     *
     * @param kind the kind of public key to generate.
     * @param pubKey the public key as binary or base64url.
     * @param isBase64 true if the key is encoded in base64url.
     * @return the public key or null if it was invalid.
     */
    @Nullable
    public static CryptoKey importPublicKey(@NonNull Kind kind, @NonNull byte[] pubKey, boolean isBase64) {
        return nativeImportPublicKey(kind.ordinal(), pubKey, isBase64);
    }

    /**
     * Import a private/public keypair of the specified type.  The private key is either encoded in binary
     * or Base64url.
     *
     * @param kind the kind of keypair to generate.
     * @param privateKey the private key as binary or base64url.
     * @param isBase64 true if the key is encoded in base64url.
     * @return the private/public keypair or null if it was invalid.
     */
    @Nullable
    public static CryptoKey importPrivateKey(@NonNull Kind kind, @NonNull byte[] privateKey, boolean isBase64) {
        return nativeImportPrivateKey(kind.ordinal(), privateKey, isBase64);
    }

    /**
     * Get the public key either in binary or encoded in base64url.
     *
     * @param useBase64 true to encode the public key in base64url.
     * @return the public key or null.
     */
    @Nullable
    public byte[] getPublicKey(boolean useBase64) {
        checkCryptoExists();
        return nativeGetPublicKey(nativeCrypto, useBase64);
    }

    /**
     * Get the private key either in binary or encoded in base64url.
     *
     * @param useBase64 true to encode the private key in base64url.
     * @return the private key or null.
     */
    @Nullable
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
     * @param isBase64 true if the signature must be encoded in base64url.
     * @return Return the length of the signature or a negative error code.
     */
    public int sign(@NonNull byte[] data, @NonNull byte[] signature, boolean isBase64) {
        checkCryptoExists();
        return nativeSign(nativeCrypto, data, signature, isBase64);
    }

    /**
     * Verify with the public key that the data buffer corresponds to the signature.
     *
     * @param data the data buffer to verify.
     * @param signature the signature.
     * @param isBase64 true if the signature is encoded in base64url.
     * @return 1 if the signature is verified, 0 if the data does not match or a negative error code.
     */
    public int verify(@NonNull byte[] data, @NonNull byte[] signature, boolean isBase64) {
        checkCryptoExists();
        return nativeVerify(nativeCrypto, data, signature, isBase64);
    }

    /**
     * Release the internal memory allocated for the public/private keypair.
     */
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
