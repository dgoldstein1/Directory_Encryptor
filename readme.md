# Dir Ecryptor

Encrypts all files in specified directory with specified password (AES)

## Install

Install Dependencies
```sh
sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils
```

(Optional) add script as environment variable

```sh
mkdir ~/bin/dir_encryptor
mv . ~/bin/dir_encryptor/
chmod +x ~/bin/dir_encryptor/
echo "export PATH=$PATH:~/bin/dir_encryptor/" >> ~/.bashrc
```

## Test Install

./test.sh

If all goes well should return "SUCCESS"


## Run

Encryption
```sh
./run.ssh sample_dir/ encrypt samplePassword
```

Encrypts all of sample_dir a MAC created from the user specified password and username. Extension .encrypted added to all files successfully encrypted.


Decryption
```sh
./run.ssh sample_dir/ decrypt samplePassword
```



## Dependencies

-Crypto AES (cbc mode: https://www.cryptopp.com/wiki/CBC_Mode)
-Cryptopp hmac authenticaion (https://www.cryptopp.com/docs/ref/class_c_b_c___m_a_c.html)

# Todo

-create streamlined script for creating custom command
-upload to launchpad and create PPA (apt-get install dirEncryptor)




