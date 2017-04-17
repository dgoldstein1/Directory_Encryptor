// g++ -DDEBUG=1 -g3 -Og -Wall -Wextra -Wno-unused-parameter -I/usr/local/include/cryptopp cryptopp-authenc.cpp -o cryptopp-authenc.exe /usr/local/lib/libcryptopp.a
// g++ -DNDEBUG=1 -g3 -O2 -Wall -Wextra -Wno-unused-parameter -I/usr/local/include/cryptopp cryptopp-authenc.cpp -o cryptopp-authenc.exe /usr/local/lib/libcryptopp.a

#include <iostream>
using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include <fstream>

#include <string>
using std::string;

#include <cryptopp/cryptlib.h>
using CryptoPP::lword;
using CryptoPP::word32;
using CryptoPP::word64;
using CryptoPP::Exception;

#include <cryptopp/secblock.h>
using CryptoPP::SecByteBlock;
using CryptoPP::SecBlock;

#include <cryptopp/hex.h>
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include <cryptopp/filters.h>
using CryptoPP::Redirector;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;
using CryptoPP::HashVerificationFilter;
using CryptoPP::HashFilter;

#include <cryptopp/files.h>
using CryptoPP::FileSink;
using CryptoPP::FileSource;

#include <cryptopp/sha.h>
using CryptoPP::SHA256;
using CryptoPP::SHA512;

#include <cryptopp/aes.h>
using CryptoPP::AES;

#include <cryptopp/modes.h>
using CryptoPP::CBC_Mode;

#include <cryptopp/pwdbased.h>
using CryptoPP::PKCS5_PBKDF2_HMAC;

#include <cryptopp/hmac.h>
using CryptoPP::HMAC;

using namespace std;

void DeriveKeyAndIV(const string& master, const string& salt,
    unsigned int iterations,
    SecByteBlock& ekey, unsigned int eksize,
    SecByteBlock& iv, unsigned int vsize,
    SecByteBlock& akey, unsigned int aksize);

void PrintKeyAndIV(SecByteBlock& ekey,
   SecByteBlock& iv,
   SecByteBlock& akey);

int EncryptMsg(string password, string source, string destination, string username);
int DecryptMsg(string password, string source, string destination, string username);

// g++ -DDEBUG=1 -g3 -O1 -Wall -Wextra -Wno-unused-parameter -I/usr/local/include/cryptopp cryptopp-authenc.cpp /usr/local/lib/libcryptopp.a -o cryptopp-authenc.exe

int main(int argc, char* argv[])
{
    (void)argc; (void)argv;

    //parse args
    if (argc != 6){
        cout << "incorrect args entered. Example './cryptopp-authenc.exe encrypt/decrypt sourceFile.txt password destinationFile.txt username" << endl ;
        return 1;  
    } 
    string password, source, mode, destination, username; 
    mode = string(argv[1]);
    source = string(argv[2]);
    password = string(argv[3]);
    destination = string(argv[4]);
    username = string(argv[5]);


    // encrypt or decrypt
    if (!mode.compare("encrypt")){
        return EncryptMsg(password, source, destination, username);
    } else if (!mode.compare("decrypt")){
        return DecryptMsg(password, source, destination, username);
    } 
    return 1;
}



/*
* encrpyts message in AES
*/
int EncryptMsg(string password, string source, string destination, string username){  
    try {

        
        // derived parameters
        SecByteBlock ekey(16), iv(16), akey(16);
        DeriveKeyAndIV(password, username, 100,
           ekey, ekey.size(), iv, iv.size(), akey, akey.size());
        
        // Create and key objects
        CBC_Mode<AES>::Encryption encryptor;
        encryptor.SetKeyWithIV(ekey, ekey.size(), iv, iv.size());
        HMAC< SHA256> hmac1;
        hmac1.SetKey(akey, akey.size());
        

        //string >> char*
        char * fileIn = new char[source.length() + 1];
        std::strcpy(fileIn,source.c_str());

        // Encrypt and authenticate data
        string encrypted;
        FileSource fs1(fileIn, true /*pumpAll*/,
        new StreamTransformationFilter(encryptor,
            new HashFilter(hmac1,
               new StringSink(encrypted), true /*putMessage*/)));

        //write to file
        char * fileOut = new char[destination.length() + 1];
        std::strcpy(fileOut,destination.c_str());
        std::ofstream out(fileOut);
        out << encrypted;
        out.close();
    }
    catch(CryptoPP::Exception& ex)
    {
        cerr << ex.what() << endl;
        return 1;
    }

    return 0;
}

int DecryptMsg(string password, string source, string destination, string username){  
    try {

        // derived parameters
        SecByteBlock ekey(16), iv(16), akey(16);
        DeriveKeyAndIV(password, username, 100,
           ekey, ekey.size(), iv, iv.size(), akey, akey.size());
        
        // Create and key objects
        CBC_Mode<AES>::Decryption decryptor;
        decryptor.SetKeyWithIV(ekey, ekey.size(), iv, iv.size());
        HMAC< SHA256> hmac2;
        hmac2.SetKey(akey, akey.size());
        
        // Authenticate and decrypt data
        static const word32 flags = CryptoPP::HashVerificationFilter::HASH_AT_END |
        CryptoPP::HashVerificationFilter::PUT_MESSAGE |
        CryptoPP::HashVerificationFilter::THROW_EXCEPTION;


        //string >> char *
        char * fileIn = new char[source.length() + 1];
        std::strcpy(fileIn,source.c_str());

        string decrypted;
        FileSource fs1(fileIn, true /*pumpAll*/,
               new HashVerificationFilter(hmac2,
                new StreamTransformationFilter(decryptor,
                   new StringSink(decrypted)),
                flags));

        //write to file
        char * fileOut = new char[destination.length() + 1];
        std::strcpy(fileOut,destination.c_str());
        std::ofstream out(fileOut);
        out << decrypted;
        out.close();
        
    }
    catch(CryptoPP::Exception& ex)
    {
        cout << "File Skipped: Invalid Password" << endl;
        return 1;
    }

    return 0;

}


void DeriveKeyAndIV(const string& master, const string& salt,
    unsigned int iterations,
    SecByteBlock& ekey, unsigned int eksize,
    SecByteBlock& iv, unsigned int vsize,
    SecByteBlock& akey, unsigned int aksize)
{

    SecByteBlock tb, ts(SHA512::DIGESTSIZE), tm(SHA512::DIGESTSIZE);
    
    // Temporary salt, stretch size.
    SHA512 hash;
    hash.CalculateDigest(ts, (const byte*)salt.data(), salt.size());
    
    static const string s1 = "master key";
    tb = SecByteBlock((const byte*)master.data(), master.size()) + SecByteBlock((const byte*)s1.data(), s1.size());
    
    PKCS5_PBKDF2_HMAC<SHA512> pbkdf;
    const byte unused = 0;
    pbkdf.DeriveKey(tm, tm.size(),
        unused,
        tb, tb.size(),
        ts, ts.size(),
        100);
    
    static const string s2 = "encryption key";
    ekey.resize(eksize);
    tb = tm + SecByteBlock((const byte*)s2.data(), s2.size());
    pbkdf.DeriveKey(ekey, ekey.size(),
        unused,
        tb, tb.size(),
        ts, ts.size(),
        100);
    
    static const string s3 = "initialization vector";
    iv.resize(vsize);
    tb = tm + SecByteBlock((const byte*)s3.data(), s3.size());
    pbkdf.DeriveKey(iv, iv.size(),
        unused,
        tb, tb.size(),
        ts, ts.size(),
        100);
    
    static const string s4 = "authentication key";
    akey.resize(aksize);
    tb = tm + SecByteBlock((const byte*)s4.data(), s4.size());
    pbkdf.DeriveKey(akey, iv.size(),
        unused,
        tb, tb.size(),
        ts, ts.size(),
        100);
}

void PrintKeyAndIV(SecByteBlock& ekey,
   SecByteBlock& iv,
   SecByteBlock& akey)
{
    // Print them
    HexEncoder encoder(new FileSink(cout));
    
    cout << "AES key: ";
    encoder.Put(ekey.data(), ekey.size());
    encoder.MessageEnd(); cout << endl;
    
    cout << "AES IV: ";
    encoder.Put(iv.data(), iv.size());
    encoder.MessageEnd(); cout << endl;
    
    cout << "HMAC key: ";
    encoder.Put(akey.data(), akey.size());
    encoder.MessageEnd(); cout << endl;
}
