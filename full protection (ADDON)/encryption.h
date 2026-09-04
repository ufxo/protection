#pragma once

#include <vector>

int gGK();

namespace encryption {
	std::string encrypt(const std::string& plaintext, int key);

	std::string decrypt(const std::string& ciphertext, int key);

	std::vector<std::wstring> encryptWVector(const std::vector<std::wstring>& input, int key);
	std::vector<std::wstring> decryptWVector(const std::vector<std::wstring>& input, int key);
}