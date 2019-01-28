#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <bitset>

using namespace std;

vector<long int> hexToDec(vector<string>);
void fillArray(long int *, vector<long int>);
void runProgram(long int *, int);
long int createMask(long int, long int);
string commandName(int);
long int performALU(long int, long int, int);
bool jump(int, int);
void print(int, long int *, long int, int [], long int);
