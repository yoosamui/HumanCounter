/*
 *  created by Juan R. Gonzalez & Juan Alvarez 2016
 */
#ifndef XORENCRYPT_H
#define XORENCRYPT_H

#include <stdio.h>
#include <stdlib.h>

using namespace std;

class XorEncrypt
{
public:

    string encrypt(string toEncrypt) {
        return encryptDecrypt(toEncrypt);
    }

    string decrypt(string toDecrypt) {
        return encryptDecrypt(toDecrypt);
    }



private:

    //Any chars will work, in an array of any size
    string encryptDecrypt(string toEncrypt) {
        char key[] = {'K', 'C', 'Q','m','k','i','u','o','6','e','7','@','_','!'}; 
//        char key[13];
//        
//        for (int i = 64,  idx = 0; i < 90; i += 2, idx++) {
//            char ch = i;
//            key[idx] = ch;
//        }

        string output = toEncrypt;

        for (int i = 0; i < toEncrypt.size(); i++)
            output[i] = toEncrypt[i] ^ key[i % (sizeof (key) / sizeof (char))];

        return output;
    }

};

#endif /* XORENCRYPT_H */

