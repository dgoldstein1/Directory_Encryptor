# Dir Ecryptor

Encrypts all files in specified directory with specified password (AES)

## Install

```sh
sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils
```

## Run

Encryption
```sh
./run.ssh sample_dir/ encrypt samplePassword
```
Decryption
```sh
./run.ssh sample_dir/ decrypt samplePassword
```

## Dependencies

-Crypto AES (cbc mode: https://www.cryptopp.com/wiki/CBC_Mode)
-Cryptopp hmac authenticaion (https://www.cryptopp.com/docs/ref/class_c_b_c___m_a_c.html)

# Todo

-Add GUI




