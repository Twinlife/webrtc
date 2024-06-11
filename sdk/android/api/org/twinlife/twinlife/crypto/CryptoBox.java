package org.twinlife.twinlife.crypto;

import androidx.annotation.NonNull;
import org.webrtc.CalledByNative;

/**
 * Class to provide encryption and decryption.
 */
public class CryptoBox {
    private long nativeCrypto; // pointer to the webrtc::jni::CryptoBox* instance

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
    public static final int KEY_LENGTH = 32;

    public enum Kind {
        AES_GCM,
        CHACHA20_POLY1305
    };

    @CalledByNative
    public CryptoBox(long nativeCryptoBox) {
        this.nativeCrypto = nativeCryptoBox;
    }

    /**
     * Create a crypto box to encrypt/decrypt with the given AEAD operation.
     *
     * @param kind the crypto operation to use.
     * @return the crypto box.
     */
    public static CryptoBox create(@NonNull Kind kind) {
        return nativeCreate(kind.ordinal());
    }

    /**
     * Prepare for use of AEAD with the peer's public key.  Derive a shared secret based on the private key
     * and peer's public key, compute the HMAC(sharedSecret, {salt || pubKeyA || pubKeyB}) digest,
     * setup the AEAD internal context to be ready to use `encryptAEAD` or `decryptAEAD`.
     * The `bind` is a costly operation compared to encryption and decryption.
     * Returns 1 when the operation succeeds or an error code.
     *
     * @param encrypt true for encryption and false for decryption
     * @param privateKey the private key for encryption or decryption
     * @param peerPublicKey the peer public key
     * @param salt the salt to use for the HKDF operation.
     * @return 0 or an error code.
     */
    public int bind(boolean encrypt, @NonNull CryptoKey privateKey, @NonNull CryptoKey peerPublicKey, @NonNull byte[] salt) {
        checkCryptoExists();
        privateKey.checkCryptoExists();
        peerPublicKey.checkCryptoExists();
        return nativeBind(nativeCrypto, encrypt, privateKey.internalCrypto(), peerPublicKey.internalCrypto(), salt);
    }

    /**
     * Prepare for use of AEAD with the secret key given in key and with the given length.
     * Returns 1 when the operation succeeds or an error code.
     *
     * @param key the encryption key.
     * @return 0 or an error code.
     */
    public int bind(@NonNull byte[] key) {
        checkCryptoExists();
        return nativeBindSecret(nativeCrypto, key);
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
     * Encrypt and sign with AES256-GCM or ChaCha20-Poly1305 the data buffer and auth buffer with a new nonce sequence.
     * Only the data buffer is encrypted.  The result buffer has the following format:
     * +-------------------------+----------------+
     * | auth data [auth_length] | encrypted data |
     * +-------------------------+----------------+    
     *
     * @param nonceSequence
     * @param data
     * @param auth
     * @param output
     * @return the length of the output buffer or a negative error code.
     */
    public int encryptAEAD(long nonceSequence, @NonNull byte[] data, @NonNull byte[] auth, @NonNull byte[] output) {
        checkCryptoExists();
        return nativeEncryptAEAD(nativeCrypto, nonceSequence, data, auth, output);
    }

    /**
     * Decrypt and verify the data with AES256-GCM or ChaCha20-Poly1305.  The data buffer is assumed
     * to use the following format:
     * +-------------------------+----------------+
     * | data [auth_length]      | data encrypted |
     * +-------------------------+----------------+    
     *
     * @param nonceSequence the nonce sequence used for encryption.
     * @param data the data buffer with the authenticate part followed by the encrypted part.
     * @param authLength the length of the authenticate part.
     * @param output the output buffer to store the decrypted part.
     * @return the length of the decrypted part or an error code.
     */
    public int decryptAEAD(long nonceSequence, @NonNull byte[] data, int authLength, @NonNull byte[] output) {
        checkCryptoExists();
        return nativeDecryptAEAD(nativeCrypto, nonceSequence, data, authLength, output);
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
    private static native int nativeBind(long nativeCryptoBox, boolean encrypt, long nativePrivateCryptoKey, long nativeBindCrypto, byte[] salt);
    private static native int nativeBindSecret(long nativeCryptoBox, byte[] key);
    private static native int nativeUnbind(long nativeCryptoBox);
    private static native int nativeEncryptAEAD(long nativeCryptoBox, long nonceSequence, byte[] data, byte[] auth, byte[] output);
    private static native int nativeDecryptAEAD(long nativeCryptoBox, long nonceSequence, byte[] data, int authLength, byte[] output);
    private static native void nativeDispose(long nativeCryptoBox);
}
