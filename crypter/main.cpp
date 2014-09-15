/*
	Humble Bundle Challange 2
	
	
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

enum Message
{
	MSG_USAGE,
	MSG_ENC_SUCCES,

	MSG_ERROR_INPUT_FILE,
	MSG_ERROR_KEY_FILE,
	MSG_ERROR_OUTPUT_FILE,

	MSG_ERROR_MALFUNCTION
};

void ShowMessageAndExit(Message id)
{
	//Draw
	if(id == MSG_USAGE)
		cout << "Type: crypter <input file> <key file> <output file>" << endl;
	else if(id == MSG_ENC_SUCCES)
		cout << "Encryption finished!" << endl;
	
	else if(id == MSG_ERROR_INPUT_FILE)
		cout << "Failed to open input file." << endl;
	else if(id == MSG_ERROR_KEY_FILE)
		cout << "Failed to open key file." << endl;
	else if(id == MSG_ERROR_OUTPUT_FILE)
		cout << "Failed to create output file." << endl;
	
	else if(id == MSG_ERROR_MALFUNCTION)
		cout << "Something went terribly wrong!" << endl;
		
	//Exit
	if(id == MSG_USAGE || id == MSG_ENC_SUCCES)
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}

void CloseFiles(FILE* input_file, FILE* key_file, FILE* output_file, Message id) {
	if(input_file != NULL)
		fclose(input_file);
		
	if(key_file != NULL)
		fclose(key_file);
		
	if(output_file != NULL)
		fclose(output_file);
	
	ShowMessageAndExit(id);
}

void Encrypt(char* _input_file, char* _key_file, char* _output_file) {
	FILE* input_file = NULL;
	FILE* key_file = NULL;
	FILE* output_file = NULL;
	
	//Open Files
	input_file = fopen(_input_file,"rb");
	if(input_file == NULL) {
		CloseFiles(input_file, key_file, output_file, MSG_ERROR_INPUT_FILE);
	}
	
	key_file = fopen(_key_file,"rb");
	if(key_file == NULL) {
		CloseFiles(input_file, key_file, output_file, MSG_ERROR_KEY_FILE);
	}
	
	output_file = fopen(_output_file,"wb");
	if(output_file == NULL)  {
		CloseFiles(input_file, key_file, output_file, MSG_ERROR_OUTPUT_FILE);
	}
	
	//Get Data
	char key[30];
	char data[30];
	size_t data_length;
	
	data_length = fread(data, 1, 30, key_file);
	data[data_length] = 0x00;
	
	cout << "length = " << data_length << endl;
	cout << "data = " << data << endl;
	
	//Finished
	CloseFiles(input_file, key_file, output_file, MSG_ENC_SUCCES);
}

int main(int argc, char *argv[]) {
	if(argc == 4)
	{
		Encrypt(argv[1], argv[2], argv[3]);
	}
	else
	{
		ShowMessageAndExit(MSG_USAGE);
	}
	
	return 0;
}