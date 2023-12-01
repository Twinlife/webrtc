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

    @CalledByNative
    public Crypto(long nativeCrypto) {
        this.nativeCrypto = nativeCrypto;
    }

    public static Crypto create() {
        return nativeCreate();
    }

    public static Crypto importPublicKey(byte[] pubKey) {
        return nativeImportPublicKey(pubKey);
    }

    public static Crypto importPrivateKey(byte[] privateKey) {
        return nativeImportPrivateKey(privateKey);
    }

    public byte[] getPublicKey() {
        checkCryptoExists();
        return nativeGetPublicKey(nativeCrypto);
    }

    public byte[] getPrivateKey() {
        checkCryptoExists();
        return nativeGetPrivateKey(nativeCrypto);
    }

    public int bind(Crypto peerPublicKey, byte[] nonce, int maxIncrement) {
        checkCryptoExists();
        peerPublicKey.checkCryptoExists();
        return nativeBind(nativeCrypto, peerPublicKey.nativeCrypto, nonce, maxIncrement);
    }

    public int unbind() {
        checkCryptoExists();
        return nativeUnbind(nativeCrypto);
    }

    public int newNonce(byte[] nonce, int maxIncrement) {
        checkCryptoExists();
        return nativeNewNonce(nativeCrypto, nonce, maxIncremnet);        
    }

    public int signECDSA(byte[] data, byte[] signature) {
        checkCryptoExists();
        return nativeSignECDSA(nativeCrypto, data, signature);
    }

    public int verifyECDSA(byte[] data, byte[] signature) {
        checkCryptoExists();
        return nativeVerifyECDSA(nativeCrypto, data, signature);
    }

    public int encryptAEAD(byte[] data, byte[] auth, byte[] nonce, byte[] output) {
        checkCryptoExists();
        return nativeEncryptAEAD(nativeCrypto, data, auth, nonce, output);
    }

    public int decryptAEAD(byte[] encrypted, byte[] auth, byte[] nonce, byte[] output) {
        checkCryptoExists();
        return nativeDecryptAEAD(nativeCrypto, encrypted, auth, nonce, output);
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
    private static native Crypto nativeImportPublicKey(byte[] pubKey);
    private static native Crypto nativeImportPrivateKey(byte[] privateKey);
    private static native byte[] nativeGetPublicKey(long nativeCrypto);
    private static native byte[] nativeGetPrivateKey(long nativeCrypto);
    private static native int nativeBind(long nativeCrypto, long nativeBindCrypto, byte[] nonce, int maxIncrement); // 
    private static native int nativeUnbind(long nativeCrypto);
    private static native int nativeNewNonce(long nativeCrypto, byte[] nonce, int maxIncrement);
    private static native int nativeSignECDSA(long nativeCrypto, byte[] data, byte[] signature);
    private static native int nativeVerifyECDSA(long nativeCrypto, byte[] data, byte[] signature);
    private static native int nativeEncryptAEAD(long nativeCrypto, byte[] data, byte[] auth, byte[] nonce, byte[] output);
    private static native int nativeDecryptAEAD(long nativeCrypto, byte[] data, byte[] auth, byte[] nonce, byte[] output);
    private static native void nativeDispose(long nativeCrypto);
}
