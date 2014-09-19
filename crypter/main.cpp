/*
	Humble Bundle Challange 2 - Start Trek Books
	
	Try to crack the "crackme.zip.enc". Good luck! :D
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

enum Program_Message
{
	MSG_USAGE,
	MSG_ENC_SUCCES,
	MSG_DEC_SUCCES,
	MSG_ERROR_INPUT_FILE,
	MSG_ERROR_INPUT_FILE_HEADER,
	MSG_ERROR_INPUT_KEY_FILE,
	MSG_ERROR_INPUT_KEY_FILE_SIZE,
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
	else if(message == MSG_ERROR_INPUT_FILE_HEADER)
		cout << "Input file has a wrong header. Check if file is corrupted." << endl;
	else if(message == MSG_ERROR_INPUT_KEY_FILE)
		cout << "Failed to open input key file." << endl;
	else if(message == MSG_ERROR_INPUT_KEY_FILE_SIZE)
		cout << "Error on input file, this isn't a key file. 16 Bytes only accepted!" << endl;
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
	static char shuffle[16] = {	0x8, 0xA, 0x0, 0x1, 0x9, 0xC, 0xF, 0x3,
								0x6, 0x2, 0xD, 0xE, 0x4, 0x2, 0x5, 0x7 };
	
	for(unsigned int i = 0; i < 5; i++) {
		for(unsigned int j = 0; j < 16; j++) {
			unsigned int nr = (unsigned int)generate_key[j];
			generate_key[j] = ((shuffle[(nr / 16)] * 0x10) + shuffle[(nr % 16)] + master_key[j]) & 0xFF;
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
	
	static char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
							'8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
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
		DrawKey("Master key   = ", master_key);
		DrawKey("Generate key = ", generate_key);
		
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
		DrawKey("Generate key = ", generate_key);
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
	Speed up process bruteforce
*/
void GenerateKey2(unsigned char* master_key, unsigned char* generate_key, unsigned int j) {
	static char shuffle[16] = {	0x8, 0xA, 0x0, 0x1, 0x9, 0xC, 0xF, 0x3,
								0x6, 0x2, 0xD, 0xE, 0x4, 0x2, 0x5, 0x7 };
	
	for(unsigned int i = 0; i < 5; i++) {
		unsigned int nr = (unsigned int)generate_key[j];
		generate_key[j] = ((shuffle[(nr / 16)] * 0x10) + shuffle[(nr % 16)] + master_key[j]) & 0xFF;
	}
}

void GetMasterKey(FILE* input_file, FILE* input_key_file, unsigned char* master_key) {
	unsigned char temp_master_key[16];
	unsigned char temp_generate_key[16];
	unsigned char final_generate_key[16];
	
	unsigned char encrypted_data[48];
	unsigned char decrypted_data[48];
	
	long int output_file_size;
	
	static char hex[16] = {	'0', '1', '2', '3', '4', '5', '6', '7',
							'8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	
	//output file
	fseek(input_file, 0, SEEK_END);
	output_file_size = ftell(input_file) - 4; // input_file - header
	fseek(input_file, 4, SEEK_SET);
	fread(encrypted_data, 1, 48, input_file);
	fseek(input_file, 4, SEEK_SET);
	
	//final generate key
	fread(final_generate_key, 1, 16, input_key_file);
	DrawKey("input_key_file = ", final_generate_key);
	
	//Init Master key
	for(unsigned int i = 0; i < 16; i++)
		master_key[i] = 0x00;
	
	//Init Decrypted Data
	for(unsigned int i = 0; i < 48; i++)
		decrypted_data[i] = 0x00;
	
	for(unsigned int i = 0; i < 16; i++) { // loop key position
		for(unsigned int j = 0; j <= 0xFF; j++) { // Bruteforce every charter
			temp_master_key[i] = j;
			temp_generate_key[i] = 0x00;
			
			GenerateKey2(temp_master_key, temp_generate_key, i);
			for(unsigned int k = 0; k < output_file_size; k += 16)
				GenerateKey2(temp_master_key, temp_generate_key, i);
			GenerateKey2(temp_master_key, temp_generate_key, i);
			
			if(temp_generate_key[i] == final_generate_key[i]) { // Found possible key
				unsigned char temp_decrypted_data[3];
				/*
					at this point we have 3.2 mil possibilities.
					With zip heuristics we can have less possibilities
				*/
				bool check = false;
				
				//Init
				temp_generate_key[i] = 0x00;
				GenerateKey2(temp_master_key, temp_generate_key, i);
				
				//Decrypt data parts
				GenerateKey2(temp_master_key, temp_generate_key, i);
				temp_decrypted_data[0] = temp_generate_key[i] ^ encrypted_data[i];
				GenerateKey2(temp_master_key, temp_generate_key, i);
				temp_decrypted_data[1] = temp_generate_key[i] ^ encrypted_data[i + 16];
				GenerateKey2(temp_master_key, temp_generate_key, i);
				temp_decrypted_data[2] = temp_generate_key[i] ^ encrypted_data[i + 32];
				
				/*
					http://petlibrary.tripod.com/ZIP.HTM
					
					00-03: PKZIP local file header signature
					If you look at the possibilities you can see the decrypted data
					is a zip header signature.
				*/
				if(i==0 && temp_decrypted_data[0] == 0x50) check = true;
				if(i==1 && temp_decrypted_data[0] == 0x4B) check = true;
				if(i==2 && temp_decrypted_data[0] == 0x03) check = true;
				if(i==3 && temp_decrypted_data[0] == 0x04) check = true;
				
				/*
					0C-0D: Last modified file date in MSDOS format
					Check the modify date it has been modified at 16-09-2014!
					same day the topic has been made!
				*/
				if(i==12 && ((unsigned int)temp_decrypted_data[0] & 0x1F) == 16) check = true;
				if(i==13 && (((unsigned int)temp_decrypted_data[0] >> 1) + 1980) == 2014) check = true;
				
				/*
					0A-0B: Last modified file time in MSDOS format
					Check the modify time. Topic has been made on 7:20 AM.
					Then the hour must be 7 or lower.
				*/
				if(i==11 && ((unsigned int)temp_decrypted_data[0] >> 3) <= 7) check = true;
				
				/*
					1A-1B: Filename length (FL)
					data[1A] Options: (0x0A, 0x33, 0x4A, 0x7A, 0x83)
					data[1B] Options: (0x00)
					If the filename length is example 10.
					Then most likely there has the be a charter "." at the 6e or 7e position.
				*/
				if(i==10 && temp_decrypted_data[1] == 0x0A) check = true;
				if(i==4 && temp_decrypted_data[2] == 0x2E) check = true; //Yes a "." charter! :D
				
				/*
					next at the "." has to be a extension. Let me see the possibilities:
					data[25] Options: (0x74 "t", 0x04 EOT)
					data[26] Options: (0x78 "x", 0x2B "+", 0x7B "{", 0x0B VT)
					data[27] Options: (0x74 "t")
					Logicaly thinking,... ".txt" you agree? :D NicE!
				*/
				if(i==5 && temp_decrypted_data[2] == 0x74) check = true;
				if(i==6 && temp_decrypted_data[2] == 0x78) check = true;
				if(i==7 && temp_decrypted_data[2] == 0x74) check = true;
				
				/*
					Now lets play with the filename. It begins at position 1E
					data[1E] Options: (0x82 ?, 0x52 "R", 0xB8 ?, 0x3D "=", 0xB8 ?)
					data[1F] Options: (0x45 "E", 0xD5 ?, 0x55 "U")
					data[20] Options: (0x41 "A")
					data[21] Options: (0x44 "D")
					data[22] Options: (0x4D "M")
					data[23] Options: (0x41 "E", 0x65 "e")
					Conclusion it can be "README.txt" or "READMe.txt"
				*/
				if(i==14 && temp_decrypted_data[1] == 0x52) check = true;
				if(i==15 && temp_decrypted_data[1] == 0x45) check = true;
				
				/*
					At this point we still have 16 possibilities.
					master_key[3]: 2 options
					master_key[6]: 2 options
					master_key[8]: 2 options
					master_key[9]: 2 options
					In the end we find out the right keys.
				*/
				if(i==3 && temp_decrypted_data[1] != 0x00) check = false;
				if(i==6 && temp_decrypted_data[0] != 0x00) check = false;
				if(i==8 && temp_decrypted_data[0] == 0x08) check = true;
				if(i==9 && temp_decrypted_data[1] == 0x00) check = true;
				
				if(check) {
					master_key[i] = temp_master_key[i];
					
					decrypted_data[i] = temp_decrypted_data[0];
					decrypted_data[i + 16] = temp_decrypted_data[1];
					decrypted_data[i + 32] = temp_decrypted_data[2];
					
					system("cls");
					cout << "Master key Options: " << endl;
					DrawKey("Master key   = ", master_key);
					
					cout << "Encrypted Data: " << endl;
					DrawKey("00000000 - ", encrypted_data);
					DrawKey("00000010 - ", encrypted_data + 16);
					DrawKey("00000020 - ", encrypted_data + 32);
					cout << endl;
					cout << "Decrypted Data: " << endl;
					DrawKey("00000000 - ", decrypted_data);
					DrawKey("00000010 - ", decrypted_data + 16);
					DrawKey("00000020 - ", decrypted_data + 32);
				}
			}
		}
		cout << endl;
	}
}

/*
	_input_file = encrypted file to decrypt
	_input_key_file = end point of generated key to decrypt
	_output_file = decrypted file
	_output_key_file = orginal Master key
*/
void Decrypt(char* _input_file, char* _input_key_file, char* _output_file, char* _output_key_file) {
	FILE* input_file = NULL;
	FILE* input_key_file = NULL;
	FILE* output_file = NULL;
	FILE* output_key_file = NULL;
	
	Program_Message message;
	
	unsigned char header[4];
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
		
		//File Checks
		fread(header, 1, 4, input_file);
		if(header[0] != 0x2f && header[1] != 0x45 && header[2] != 0x4e && header[3] != 0x43)
			throw MSG_ERROR_INPUT_FILE_HEADER;
		
		fseek(input_key_file, 0, SEEK_END);
		if(ftell(input_key_file) != 16)
			throw MSG_ERROR_INPUT_KEY_FILE_SIZE;
		fseek(input_key_file, 0, SEEK_SET);
		
		//Init
		for(unsigned int i = 0; i < 16; i++)
			generate_key[i] = 0x00;
		GetMasterKey(input_file, input_key_file, master_key);
		GenerateKey(master_key, generate_key);
		system("cls");
		DrawKey("Master key = ", master_key);
		
		char data[16];
		unsigned int length;
		fseek(input_file, 4, SEEK_SET);
		do {
			GenerateKey(master_key, generate_key);
			
			length = fread(data, sizeof(char), 16, input_file);
			
			if(length != 0) {
				for(unsigned int i = 0; i < 16; i++) {
					data[i] ^= generate_key[i];
				}
			}
			
			fwrite(data, sizeof(char), length, output_file);
		}
		while(length == 16);
		
		//Master key
		fwrite(master_key, sizeof(char), 16, output_key_file);
		
		throw MSG_DEC_SUCCES;
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