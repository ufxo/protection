#pragma once

#include <vector>
#include <cstdint>
#include <chrono>

class EncKey {
public:
    EncKey() {
        uint64_t nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();

        uint64_t last5 = nanos % 100000;

        srand(static_cast<unsigned>(time(nullptr)));

        value = (static_cast<double>(time(nullptr)) * .000155721 +
            (rand() % 10000 * 0.619) * 3.51) * (last5 * .0054);
    }

    double getKey() {
        std::cout << "[encryption.h] enckey: " << value;
        return value;
    }

private:
    double value;
};


namespace encryption {
	std::string encrypt(const std::string& plaintext, int key);

	std::string decrypt(const std::string& ciphertext, int key);

	std::vector<std::wstring> encryptWVector(const std::vector<std::wstring>& input, int key);
	std::vector<std::wstring> decryptWVector(const std::vector<std::wstring>& input, int key);
}