#include <iostream>
#include <random>
#include <ctime>
#include <string>
#include <cstdint>

long long tK = static_cast<long long>(
    time(nullptr) * 53.6989123491239 +
    (rand() % 10000 * 0.6192349) * 312.5143959832
);

int gGK()
{
	return tK;
}

std::string xorCipher(const std::string& input, int key) {
    std::string output = input;
    uint8_t keyBytes[4] = {
        static_cast<uint8_t>(key & 0xFF),
        static_cast<uint8_t>((key >> 8) & 0xFF),
        static_cast<uint8_t>((key >> 16) & 0xFF),
        static_cast<uint8_t>((key >> 24) & 0xFF)
    };

    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] ^ keyBytes[i % 4];
    }
    return output;
}

namespace encryption {
    std::string encrypt(const std::string& plaintext, int key) {
        return xorCipher(plaintext, key);
    }

    std::string decrypt(const std::string& ciphertext, int key) {
        return xorCipher(ciphertext, key);
    }





    // thanks https://claude.ai 

    std::string wstringToBytes(const std::wstring& ws) {
        return std::string(
            reinterpret_cast<const char*>(ws.data()),
            ws.size() * sizeof(wchar_t)
        );
    }

    std::wstring bytesToWstring(const std::string& bytes) {
        std::wstring ws(bytes.size() / sizeof(wchar_t), L'\0');
        std::memcpy(&ws[0], bytes.data(), bytes.size());
        return ws;
    }

    // --- Encrypt / decrypt a whole vector<wstring> ---
    std::vector<std::wstring> encryptWVector(const std::vector<std::wstring>& input, int key) {
        std::vector<std::wstring> result;
        result.reserve(input.size());
        for (const auto& ws : input) {
            result.push_back(bytesToWstring(encrypt(wstringToBytes(ws), key)));
        }
        return result;
    }

    std::vector<std::wstring> decryptWVector(const std::vector<std::wstring>& input, int key) {
        std::vector<std::wstring> result;
        result.reserve(input.size());
        for (const auto& s : input) {
            result.push_back(bytesToWstring(decrypt(wstringToBytes(s), key)));
        }
        return result;
    }
}