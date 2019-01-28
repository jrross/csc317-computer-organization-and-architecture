#include "functions.h"

vector<long int> hexToDec(vector<string> hex)
{
	vector<long int> dec;
	long int temp;
	for(int i = 0; i < (int)hex.size(); i++)
	{
		stringstream ss;
		ss << std::hex << hex[i];	//step through and convert every
		ss >> temp;					//string in hexidecimal to an
		dec.push_back(temp);		//unsigned int in decimal.
	}
	return dec;
}

void fillArray(long int *arr, vector<long int> vect)
{
	int i = 0;
	while(i < (int)vect.size() - 1) 	//stop when it gets to the program counter
	{
		int temp = i;		//save address in temp
		int j;
		i++;
		for(j = 0; j < vect[i]; j++)	//Take the count of how many data pieces
		{								//to store and store them in order
			arr[vect[temp] + j] = vect[i + j + 1];
		}
		i = i + j + 1;	//increment to the next address for storage and repeat
	}
}

//make a value with bits from a to b being all 1s
long int createMask(long int a, long int b)
{
   long int value = 0;
   for (long int i=a; i<=b; i++)
       value |= 1 << i;

   return value;
}

//The main datapath of the program
void runProgram(long int *arr, int ir)	//ir = instruction register
{
	int ic = ir;		//instruction counter
	int xn[4] = {0};	//index registers
	long int acc = 0;	//accumulator
	long int alu = 0;	//arithmetic logic unit 
	long int dbus = 0;	//data bus

	while(true)
	{
		ir = ic;		//set ir to hold new instruction
		ic = ic + 1;	//set ic to hold next instr. (no branch)

		//VARIABLES USED ONLY TO KEEP CODE CONCISE INSTEAD OF REPEATEDLY
		//WRITING CREATEMASK, THIS DOES NOT ADD TO THE MACHINE.
		int reg = arr[ir] & createMask(0,1);
		int am = arr[ir] & createMask(2, 5);
		int op = (arr[ir] & createMask(6, 11)) >> 6;
		int addr = (arr[ir] & createMask(12, 23)) >> 12;		


		long int ea;
		//DIRECT || IMMEDIATE ADDRESSING MODES
		if(am == 0 || am == 4)
			ea = addr;
		//INDEXED ADDRESSING MODE
		else if(am == 8)
		{	//sum the contents of the specified register with specified address
			ea = xn[reg] + addr;
		}
		//INDIRECT ADDRESSING MODE
		else if(am == 16)
		{	//get the memory contents at addr and take top 12 bits
			ea = (arr[addr] & createMask(12,23)) >> 12;
		}
		//INDEXED INDIRECT ADDRESSING MODE
		else if(am == 24)
		{	//get the memory contents at addr with offset and take top 12 bits
			ea = ((arr[addr + xn[reg]]) & createMask(12,23)) >> 12;
		}
		//ILLEGAL ADDRESSING MODE
		else
		{
			print(ir, arr, acc, xn, -2);
			cout << "Machine Halted - Illegal addressing mode\n";
			break;
		}

		//UNDEFINED OPCODES
		if(commandName(op) == "????")
		{
			print(ir, arr, acc, xn, ea);
			cout << "Machine Halted - Undefined opcode\n";
			break;
		}

		//HALT OR NOP (00)
		if((arr[ir] & createMask(10,11)) == 0)
		{
			if(op == 0)
			{
				print(ir, arr, acc, xn, -1);
				cout << "Machine Halted - HALT instruction executed\n";
				break;
			}		
		}
		//MEMORY INSTRUCTIONS (01)
		else if(((arr[ir] & createMask(10,11)) >> 10) == 1)
		{
			if(op == 16)	//LD
			{
				if(am == 4)	//immediate instruction, use immediate value
					acc = ea;
				else
					acc = arr[ea];	//use value from memory
			}
			else if(op == 17)	//ST
			{
				if(am == 4)	//if immediate instruction
				{
					print(ir, arr, acc, xn, -2);
					cout << "Machine Halted - Illegal addressing mode\n";
					break;
				}
				arr[ea] = acc;
			}
			else if(op == 18)	//EM
			{
				if(am == 4)	//if immediate instruction
				{
					print(ir, arr, acc, xn, -2);
					cout << "Machine Halted - Illegal addressing mode\n";
					break;
				}
				dbus = arr[ea];	//perform swap
				arr[ea] = acc;
				acc = dbus;
			}
			else if(op == 24)	//LDX
			{
				if(am != 0 && am != 4)//neither immediate or direct
				{
					print(ir, arr, acc, xn, -2);
					cout << "Machine Halted - Illegal addressing mode\n";
					break;
				}
				if(am == 4)//immediate
					xn[reg] = ea;
				else
				//load the top 12 bits of the value specified by ea into register
				xn[reg] = (arr[ea] & createMask(12,23)) >> 12;
				
			}
			else if(op == 25)	//STX
			{
				if(am != 0)//if am is not direct
				{
					print(ir, arr, acc, xn, -2);
					cout << "Machine Halted - Illegal addressing mode\n";
					break;
				}
				//store the register value into the top 12 bits of the word in
				//memory and maintain the bottom 12 bits of the word.
				arr[ea] = (xn[reg] << 12) + (arr[ea] & createMask(0,11));
			}
			else if(op == 26)	//EMX
			{
				if(am != 0)//if am is not direct
				{
					print(ir, arr, acc, xn, -2);
					cout << "Machine Halted - Illegal addressing mode\n";
					break;
				}//swaps contents of addr with contents of the register specified
				dbus = xn[reg] << 12;
				xn[reg] = (arr[ea] & createMask(12,23)) >> 12;
				arr[ea] = dbus + (arr[ea] & createMask(0,11));
			}
		}

		//LOGIC AND ARITHMETIC INSTRUCTIONS (10)
		else if(((arr[ir] & createMask(10,11)) >> 10) == 2)
		{	//if they are not register instructions
			if(op != 40 && op != 41 && op != 42)
			{
				if(am == 4)	//immediate instruction, use immediate value
					alu = performALU(ea, acc, op);
				else		//not immediate, access memory
					alu = performALU(arr[ea], acc, op);
				alu = alu & createMask(0,23);	//maintain the 24 bits			
				acc = alu;	//put results of operations into accumulator
			}
			//Register instructions
			else
			{
				if(op == 40)	//ADDX
				{	//Direct and immediate addressing only
					if(am != 0 && am != 4)
					{
						print(ir, arr, acc, xn, -2);
						cout << "Machine Halted - Illegal addressing mode\n";
						break;
					}
					if(am == 4)	//immediate
						xn[reg] = xn[reg] + ea;
					else	//add with value from memory
						xn[reg] = xn[reg] + ((arr[ea] & createMask(12,23)) >> 12);
					//ensure that it stays at 12 bits
					xn[reg] = xn[reg] & createMask(0,11);
				}
				if(op == 41)	//SUBX
				{	//Direct and immediate addressing only
					if(am != 0 && am != 4)
					{
						print(ir, arr, acc, xn, -2);
						cout << "Machine Halted - Illegal addressing mode\n";
						break;
					}
					if(am == 4)	//immediate
						xn[reg] = xn[reg] - ea;
					else	//subtract with value from memory
						xn[reg] = xn[reg] - ((arr[ea] & createMask(12,23)) >> 12);
					//ensure that it stays at 12 bits
					xn[reg] = xn[reg] & createMask(0,11);
				}

				if(op == 42)	//CLRX
				{	//set register to 0
					xn[reg] = 0;
				}

			}
		}

		//BRANCH INSTRUCTIONS (11)
		else
		{
			if(am == 4)	//if immediate instruction
			{
				print(ir, arr, acc, xn, -2);
				cout << "Machine Halted - Illegal addressing mode\n";
				break;
			}
			if(jump(acc, op))	//check if a branch should be taken
				ic = addr;
		}

		print(ir, arr, acc, xn, ea);	//print out each step
	}
}

//prints the values for each step taken
void print(int ir, long int *arr, long int acc, int xn[], long int ea)
{
	cout << setfill('0') << right << setw(3) << std::hex << ir << ":  ";
	cout << right << setw(6) << std::hex << arr[ir] << "  ";
	cout << left << setw(4) << setfill(' ') << 
		commandName((arr[ir] & createMask(6, 11)) >> 6) << "  ";
	cout << setfill('0');

	if(ea == -1)	//HALT called
		cout << "     ";
	else if(ea == -2)	//Illegal / unimplemented addressing
		cout << "???  ";
	else if((arr[ir] & createMask(2, 5)) == 4)	//check for immediate
		cout << "IMM  ";
	else
		cout << right << setw(3) << ea << "  ";

		cout << "AC["  << setw(6) << right << std::hex << acc << "]  ";

	for(int i = 0; i < 4; i++)
		cout << "X" << i << "[" << setw(3) << std::hex << xn[i] <<"]  ";
	cout << endl;
}

long int performALU(long int val, long int acc, int op)
{
	if(op == 32)	//ADD
		return val + acc;
	if(op == 33)	//SUB
		return acc - val;
	if(op == 34)	//CLR
		return 0;
	if(op == 35)	//COM
		return ~acc;
	if(op == 36)	//AND
		return val & acc;
	if(op == 37)	//OR
		return val | acc;
	if(op == 38)	//XOR
		return val ^ acc;
	return acc;
}

bool jump(int acc, int op)
{
	if(op == 48)	//110000  (J)
		return true;
	if(op == 49)	//110001  (JZ)
	{
		if(acc == 0)
			return true;
		return false;
	}		
	if(op == 50)	//110010  (JN)
	{	//check sign bit
		if(((acc & createMask(23,23)) >> 23) == 1)
			return true;
		return false;
	}
	if(op == 51)	//110011  (JP)
	{	//check sign bit
		if(((acc & createMask(23,23)) >> 23) == 0)
			return true;
		return false;
	}
	return false;
}

string commandName(int op)
{
	if(op == 0)		//000000
		return "HALT";
	if(op == 16)	//010000
		return "LD";
	if(op == 32)	//100000
		return "ADD";
	if(op == 48)	//110000
		return "J";
	if(op == 1)		//000001
		return "NOP";
	if(op == 17)	//010001
		return "ST";
	if(op == 33)	//100001
		return "SUB";
	if(op == 49)	//110001
		return "JZ";
	if(op == 18)	//010010
		return "EM";
	if(op == 34)	//100010
		return "CLR";
	if(op == 50)	//110010
		return "JN";
	if(op == 35)	//100011
		return "COM";
	if(op == 51)	//110011
		return "JP";
	if(op == 36)	//100100
		return "AND";
	if(op == 37)	//100101
		return "OR";
	if(op == 38)	//100110
		return "XOR";
	if(op == 24)	//011000
		return "LDX";
	if(op == 40)	//101000
		return "ADDX";
	if(op == 25)	//011001
		return "STX";
	if(op == 41)	//101001
		return "SUBX";
	if(op == 26)	//011010
		return "EMX";
	if(op == 42)	//101010
		return "CLRX";
	return "????";
}
