#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <sstream>

std::string readFile(const std::string& filename, const std::string& searchString) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ostringstream contentStream;
    std::string line;
    while (std::getline(file, line)) {
        if (line.find(searchString) == std::string::npos) {
            contentStream << line;
        }
    }

    file.close();

    return contentStream.str();
}

unsigned int calculateHashValue(const std::string& input) {
    const int prime = 31;
    const int mod = 1e9 + 9;
    unsigned int hash_value = 0;

    for (char ch : input) {
        hash_value = (hash_value * prime + ch) % mod;
    }

    return hash_value;
}

unsigned int powerModulo(unsigned int base, unsigned int exp, unsigned int mod) {
    unsigned int result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

bool hasSignature(const std::string& filename, const std::string& searchString) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Error opening file for signature check: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(inFile, line)) {
        // Kontrola, či začiatok riadku obsahuje hľadaný reťazec
        if (line.substr(0, searchString.size()) == searchString) {
            inFile.close();
            return true;
        }
    }

    inFile.close();
    return false;
}

unsigned int performRSA(const std::string& filename, const int originalHashValue) {
    unsigned int p = 61;
    unsigned int q = 53;
    unsigned int n = p * q;
    unsigned int phi_n = (p - 1) * (q - 1);
    unsigned int e = 17;
    unsigned int d = powerModulo(e, -1, phi_n);

    unsigned int encryptedHash = powerModulo(originalHashValue, e, n);
    unsigned int decryptedHash = powerModulo(encryptedHash, d, n);

    std::cout << "Public Key (n, e): (" << n << ", " << e << ")\n";
    std::cout << "Private Key (n, d): (" << n << ", " << d << ")\n";
    std::cout << "Encrypted hash: " << encryptedHash << "\n";
    std::cout << "Decrypted hash: " << decryptedHash << "\n \n";

    return encryptedHash;
}

void writeToFile(const std::string& filename, const std::string& searchString, const int encryptedHash) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ostringstream contentStream;
    std::string line;
    bool searchStringFound = false;

    while (std::getline(inFile, line)) {
        // Ak riadok obsahuje hľadaný reťazec, nahraď ho novým obsahom
        if (line.find(searchString) != std::string::npos) {
            contentStream << "\n" << searchString << encryptedHash;
            searchStringFound = true;
        } else {
            contentStream << line;
        }
    }

    inFile.close();

    // Ak sa searchString v súbore nenašiel, pridaj nový riadok s Encrypted a šifrovaným hashom
    if (!searchStringFound) {
        contentStream << "\n" << searchString << encryptedHash;
    }

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    outFile << contentStream.str();
    outFile.close();
}
int main() {
    std::string filename = "example.txt";
    std::string searchString = "Encrypted: ";

    std::string originalFileContent = readFile(filename, searchString);
    unsigned int originalHashValue = calculateHashValue(originalFileContent);

    std::cout << "Original File Content: " << originalFileContent << "\n \n";
    std::cout << "Original Hash of File Content: " << originalHashValue << "\n \n";

    unsigned int encryptedHash = performRSA(filename, originalHashValue);

    bool hasExistingSignature = hasSignature(filename, searchString);

    if (hasExistingSignature) {
        std::cout << "Signature already exists. Comparing file changes...\n \n";

        std::ifstream inFile(filename);
        if (!inFile.is_open()) {
            std::cerr << "Error opening file for hash comparison: " << filename << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string updatedHashLine;
        unsigned int storedHash = 0;

        while (std::getline(inFile, updatedHashLine)) {
            size_t pos = updatedHashLine.find(searchString);
            if (pos != std::string::npos) {
                storedHash = std::stoul(updatedHashLine.substr(searchString.size()));
                break;
            }
        }

        inFile.close();

        bool hasChanged = (encryptedHash != storedHash);
        std::cout << "Changes: " << (hasChanged ? "File content has CHANGED" : "File content has NO CHANGES") << "\n \n";
        if(hasChanged){
          writeToFile(filename, searchString, encryptedHash);
        }
    } else {
        std::cout << "Signature does not exist. Adding hash signature...\n \n";
        writeToFile(filename, searchString, encryptedHash);
    }
    return 0;
}