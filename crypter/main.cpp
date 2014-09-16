/*
	Humble Bundle Challange 2 - Start Trek Books
	
	Try to crack the "crackme.zip.enc". Good luck! :D
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

enum Program_Message
{
	MSG_USAGE,
	MSG_ENC_SUCCES,
	MSG_DEC_SUCCES,
	MSG_ERROR_INPUT_FILE,
	MSG_ERROR_INPUT_KEY_FILE,
	MSG_ERROR_OUTPUT_FILE,
	MSG_ERROR_OUTPUT_KEY_FILE,
	MSG_ERROR_MALFUNCTION
};

void ShowMessageAndExit(Program_Message message) {
	if(message == MSG_USAGE)
		cout << "Type: crypter <input file> <key file> <output file>" << endl;
	else if(message == MSG_ENC_SUCCES)
		cout << "Encryption finished!" << endl;
	else if(message == MSG_ENC_SUCCES)
		cout << "Decryption finished!" << endl;
	else if(message == MSG_ERROR_INPUT_FILE)
		cout << "Failed to open input file." << endl;
	else if(message == MSG_ERROR_INPUT_KEY_FILE)
		cout << "Failed to open input key file." << endl;
	else if(message == MSG_ERROR_OUTPUT_FILE)
		cout << "Failed to create output file." << endl;
	else if(message == MSG_ERROR_OUTPUT_KEY_FILE)
		cout << "Failed to open output key file." << endl;
	else if(message == MSG_ERROR_MALFUNCTION)
		cout << "Something went terribly wrong!" << endl;
		
	//Exit
	if(message == MSG_USAGE || message == MSG_ENC_SUCCES || message == MSG_DEC_SUCCES)
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}

void GenerateKey(unsigned char* master_key, unsigned char* generate_key) {
	char shuffle[16] = { 0x8, 0xA, 0x0, 0x1, 0x9, 0xC, 0xF, 0x3, 0x6, 0x2, 0xD, 0xE, 0x4, 0x2, 0x5, 0x7 };
	
	for(unsigned int i = 0; i < 5; i++) {
		for(unsigned int j = 0; j < 16; j++) {
			unsigned int nr = (unsigned int)generate_key[j];
			generate_key[j] = (shuffle[(nr / 16)] * 0x10) + shuffle[(nr % 16)];
		}
		
		for(unsigned int j = 0; j < 16; j++) {
			generate_key[j] = (((unsigned int)generate_key[j]) + (unsigned int)master_key[j]) & 0xFF;
		}
	}
}

void InitKeys(FILE* key_file, unsigned char* master_key, unsigned char* generate_key) {
	unsigned char data[16];
	unsigned int length;
	
	//Init
	for(unsigned int i = 0; i < 16; i++) {
		master_key[i] = 0x42;  //https://www.youtube.com/watch?v=aboZctrHfK8 <-- WATCH IT!!!! Else you are doomed!
		generate_key[i] = 0x00;
	}
	
	//Make Master Key
	do {
		length = fread(data, 1, 16, key_file);
		
		if(length != 0) {
			for(unsigned int i = 0; i < 16; i++) {
				master_key[i] ^= data[i % length];
			}
		}
	}
	while(length == 16);
	
	//Make Generate Key
	GenerateKey(master_key, generate_key);
}

void DrawKey(const char* name, unsigned char* key) {
	cout << name;
	
	char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	for(unsigned int i = 0; i < 16; i++) {
		cout << " " << hex[(unsigned int)key[i] / 16] << hex[(unsigned int)key[i] % 16];
	}
	
	cout << endl;
}

/*
	_input_file = File to encrypt
	_input_key_file = Master key
	_output_file = encrypted output file
	_output_key_file = end point of generated key
*/
void Encrypt(char* _input_file, char* _input_key_file, char* _output_file, char* _output_key_file) {
	FILE* input_file = NULL;
	FILE* input_key_file = NULL;
	FILE* output_file = NULL;
	FILE* output_key_file = NULL;
	
	Program_Message message;
	
	unsigned char master_key[16];
	unsigned char generate_key[16];
	
	try
	{
		//Open Files
		input_file = fopen(_input_file,"rb");
		if(input_file == NULL)
			throw MSG_ERROR_INPUT_FILE;
		
		input_key_file = fopen(_input_key_file,"rb");
		if(input_key_file == NULL)
			throw MSG_ERROR_INPUT_KEY_FILE;
		
		output_file = fopen(_output_file,"wb");
		if(output_file == NULL)
			throw MSG_ERROR_OUTPUT_FILE;
		
		output_key_file = fopen(_output_key_file,"wb");
		if(output_key_file == NULL)
			throw MSG_ERROR_OUTPUT_KEY_FILE;
		
		//Init
		InitKeys(input_key_file, master_key, generate_key);
		//DrawKey("Master key   = ", master_key);
		//DrawKey("Generate key = ", generate_key);
		
		//Output File
		char data[16];
		unsigned int length;
		fwrite("/ENC", sizeof(char), 4, output_file);
		do {
			GenerateKey(master_key, generate_key);
			//DrawKey("Generate key = ", generate_key);
			
			length = fread(data, sizeof(char), 16, input_file);
			
			if(length != 0) {
				for(unsigned int i = 0; i < 16; i++) {
					data[i] ^= generate_key[i];
				}
			}
			
			fwrite(data, sizeof(char), length, output_file);
		}
		while(length == 16);
		
		//Output Key
		GenerateKey(master_key, generate_key);
		//DrawKey("Generate key = ", generate_key);
		fwrite(generate_key, sizeof(char), 16, output_key_file);
		
		throw MSG_ENC_SUCCES;
	}
	catch(Program_Message _message)
	{
		message = _message;
	}
	catch(int e)
	{
		message = MSG_ERROR_MALFUNCTION;
	}
	
	if(input_file != NULL)
		fclose(input_file);
		
	if(input_key_file != NULL)
		fclose(input_key_file);
		
	if(output_file != NULL)
		fclose(output_file);
	
	if(output_key_file != NULL)
		fclose(output_key_file);
	
	ShowMessageAndExit(message);
}

/*
	_input_file = encrypted file to decrypt
	_input_key_file = end point of generated key to decrypt
	_output_file = decrypted file
	_output_key_file = orginal Master key
*/
void Decrypt(char* _input_file, char* _input_key_file, char* _output_file, char* _output_key_file) {
	
	// It's gone!! Ow noooo!!!
	
	ShowMessageAndExit(MSG_DEC_SUCCES);
}

int main(int argc, char *argv[]) {
	if(argc == 6 && strcmp(argv[1], "-e") == 0)
	{
		Encrypt(argv[2], argv[3], argv[4], argv[5]);
	}
	else if(argc == 6 && strcmp(argv[1], "-d") == 0)
	{
		Decrypt(argv[2], argv[3], argv[4], argv[5]);
	}
	else
	{
		ShowMessageAndExit(MSG_USAGE);
	}
	
	return 0;
}