#include "functions.h"

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		cout << "Improper usage, run as: \"main filename\"\n";
		return 0;
	}

	long int array[4096] = {0};	//array to simulate 4096 words of memory
	ifstream file;
	file.open(argv[1]);
	if(!file)
	{
		cout << "Unable to open file.\n";
		return 0;
	}

	vector<string> hex;
	int i = 0;
	string temp;
	while (!file.eof())	//read the entire file into a vector
	{
		file >> temp;
		hex.push_back(temp);
		i++;
	}
	hex.erase(hex.end());	//erase the repeat at the end of the vector
	vector<long int> dec = hexToDec(hex);	//convert the vector from strings of hex to
											//long ints in dec for easy array filling
	fillArray(array, dec);	//fill the "memory" as is specified by data
	int pc = dec[dec.size() - 1];	//save program counter
	runProgram(array, pc);
}
