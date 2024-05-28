package org.twinlife.twinlife.crypto;

import androidx.annotation.NonNull;
import org.webrtc.CalledByNative;

public class CryptoBox {
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
        AES_GCM,
        CHACHA20_POLY1305
    };

    @CalledByNative
    public CryptoBox(long nativeCryptoBox) {
        this.nativeCrypto = nativeCryptoBox;
    }

    public static CryptoBox create(@NonNull Kind kind) {
        return nativeCreate(kind.ordinal());
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
    public int bind(@NonNull CryptoKey privateKey, @NonNull CryptoKey peerPublicKey, @NonNull byte[] nonce, int maxIncrement) {
        checkCryptoExists();
        privateKey.checkCryptoExists();
        peerPublicKey.checkCryptoExists();
        return nativeBind(nativeCrypto, privateKey.internalCrypto(), peerPublicKey.internalCrypto(), nonce, maxIncrement);
    }

    public int bind(@NonNull byte[] key, @NonNull byte[] nonce, int maxIncrement) {
        checkCryptoExists();
        return nativeBindSecret(nativeCrypto, key, nonce, maxIncrement);
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
            throw new IllegalStateException("CryptoBox has been disposed.");
        }
    }

    private static native CryptoBox nativeCreate(int kind);
    private static native int nativeBind(long nativeCryptoBox, long nativePrivateCryptoKey, long nativeBindCrypto, byte[] nonce, int maxIncrement);
    private static native int nativeBindSecret(long nativeCryptoBox, byte[] key, byte[] nonce, int maxIncrement);
    private static native int nativeUnbind(long nativeCryptoBox);
    private static native int nativeNewNonce(long nativeCryptoBox, byte[] nonce, int maxIncrement);
    private static native int nativeEncryptAEAD(long nativeCryptoBox, byte[] data, byte[] auth, byte[] output);
    private static native int nativeDecryptAEAD(long nativeCryptoBox, byte[] data, int authLength, byte[] output);
    private static native void nativeDispose(long nativeCryptoBox);
}
