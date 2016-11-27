#include "wrapper.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#define KEYS_SET_FILE "chimera-leak.txt"
#define OUT_KEY_FILE "found.key"
#define DEFAULT_KEY_LEN 0x483

using namespace std;

BYTE *g_privKey = new BYTE [DEFAULT_KEY_LEN + 1];
size_t g_keyLen = 0;

BYTE* HexToBytes(const std::string& hex) {
	size_t hex_len = hex.length();
	size_t key_len = hex_len/2 + 1;

	memset(g_privKey, 0, key_len);

	const char* hex_str = hex.c_str();
	char chunk[3];
	memset(chunk, 0, 3);
	BYTE mByte = 0;
	g_keyLen = 0;

	for (size_t i = 0, j = 0; i < hex_len; i += 2, j++) {
		chunk[0] = hex_str[i];
		chunk[1] = hex_str[i + 1];

		g_privKey[g_keyLen] = static_cast<BYTE>(std::stoul(chunk, nullptr, 16));
		g_keyLen++;
	}
	return g_privKey;
}

bool save_to_file(const char* path, std::string line)
{
	std::ofstream outfile(path);
	if (outfile.is_open() == false) {
		return false;
	}
	outfile << line << endl;
	outfile.close();
	return true;
}

bool file_exist(const char* path)
{
	std::ifstream infile(path);
	if (infile.is_open() == false) {
		return false;
	}
	infile.close();
	return true;
}

bool search_priv_key(std::ifstream &infile, const char* file_path)
{
	t_DecryptFileWrapper func1 = loadDecryptFunc();
	if (func1 == NULL) {
		system("pause");
		return false;
	}

	std::string line;
	size_t key_len = 0;
	size_t counter = 0;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		key_len = line.length() / 2;
		printf("key num: %d len = %#x = %d\n", counter++, key_len, key_len);

		BYTE *priv_key = HexToBytes(line);

		if (func1(file_path, priv_key, DEFAULT_KEY_LEN)) {
			printf("Success!\n");
			if (save_to_file(OUT_KEY_FILE, line)) {
				printf("Found key saved to the file: %s\n", OUT_KEY_FILE);
			} else {
				printf("[ERROR] Cannot save to the file: %s\n", file_path);
			}
			return true;
		}
	}
	return false;
}

bool rename_file(std::string filename)
{
	std::size_t found = filename.find_last_of(".");
	std::string newname = filename.substr(0, found);
	std::string ext = filename.substr(found+1);
	if (ext == "crypt") {
		MoveFileA(filename.c_str(), newname.c_str());
		printf("Renamed decrypted file to: %s\n", newname.c_str());
		return true;
	}
	return false;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("USAGE:\nchimera_dec.exe <encrypted file> <key file>\nEXAMPLE:\nchimera_dec.exe myFile.docx.crypt chimera-leak.txt\n");
		system("pause");
		return -1;
	}

	const char *path = path = argv[1];
	const char *key_leak_file = argv[2];

	std::ifstream infile(key_leak_file);
	if (infile.is_open() == false) {
		printf("[ERROR] Cannot open the file with keys: %s\n", key_leak_file);
		system("pause");
		return -1;
	}

	if (file_exist(path) == false) {
		printf("[ERROR] Cannot open encrypted file: %s\n", path);
		system("pause");
		return -1;
	}

	if (search_priv_key(infile, path)) {
		rename_file(path);
		printf("Hurray! The set contains your key!\n");
	} else {
		printf("Sorry! None of the keys fit to your file!\n");
	}

	system("pause");
	return 0;
}
