#include <iostream>
#include <cstring>
#include <openssl/sha.h>

int main() {
    const char* text = "Hello, World!";
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // Compute SHA256 hash
    SHA256(reinterpret_cast<const unsigned char*>(text), strlen(text), hash);

    std::cout << "SHA256 Hash: ";
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    std::cout << std::endl;

    return 0;
}