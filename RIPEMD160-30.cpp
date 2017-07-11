#include "RIPEMD160.h"
#include <fstream>
#include <cstdlib>
using namespace std;

//ofstream outFile12("Q12.txt", ios::app);
/*rotate left*/
unsigned int LL(unsigned int number, int r){
	number = (number >> (32 - r)) | (number << r);
	return number;
}

/*rotate right*/
unsigned int RR(unsigned int number, int r){
	number = (number << (32 - r)) | (number >> r);
	return number;
}

/*Get the certain bit*/
inline unsigned int getBit(unsigned int number, int position){
	return (number >> position) & 0x1;
}

/*round function XOR*/
unsigned int XOR(unsigned int x, unsigned int y, unsigned int z){
	return x^y^z;
}

/*round function IFX*/
unsigned int IFX(unsigned int x, unsigned int y, unsigned int z){
	return (x&y) ^ ((~x)&z);
}

/*round function XOR*/
unsigned int ONZ(unsigned int x, unsigned int y, unsigned int z){
	return (x | (~y)) ^ z;
}

/*round function XOR*/
unsigned int IFZ(unsigned int x, unsigned int y, unsigned int z){
	return (x&z) ^ (y&(~z));
}

/*round function ONX*/
unsigned int ONX(unsigned int x, unsigned int y, unsigned int z){
	return x ^ (y | (~z));
}

//convert to binary
void toBinary(unsigned int num){
	unsigned int left = 0;
	int array[32];
	for (int i = 0; i < 32; i++){
		array[i] = 0;
	}
	int count = 0;
	while (num){
		left = num % 2;
		array[count++] = left;
		num = num / 2;
	}

	for (int i = 31; i >= 0; i--){
		cout << array[i];
		//outFile12 << array[i];
		if (i % 4 == 0){
			cout << " ";
			//outFile12 << " ";
		}
	}
	cout << endl;
	//outFile12 << endl;
	//outFile12.close();
}

RIPEMD160::RIPEMD160(){
	//Initialization of IV
	LIV[0] = RIV[0] = 0XC059D148;
	//LIV[1] = RIV[1] = 0X7C30F4B8;
	LIV[1] = RIV[1] = 0X7CE0F4B8;
	LIV[2] = RIV[2] = 0X1D840C95;
	LIV[3] = RIV[3] = 0X98BADCFE;
	//LIV[4] = RIV[4] = 0XEFCDAB89;
	LIV[4] = RIV[4] = 0X07C6A090;

	//Initialization of constants in the left branch.
	LC[0] = 0X0;
	LC[1] = 0X5A827999;
	LC[2] = 0X6ED9EBA1;
	LC[3] = 0X8F1BBCDC;
	LC[4] = 0XA953FD4E;

	//Initialization of constants in the right branch.
	RC[0] = 0X50A28BE6;
	RC[1] = 0X5C4DD124;
	RC[2] = 0X6D703EF3;
	RC[3] = 0X7A6D76E9;
	RC[4] = 0X0;

	unsigned char LST[5][16] = { 11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
		7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
		11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
		11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
		9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6 };

	unsigned RST[5][16] = { 8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
		9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
		9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
		15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
		8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11 };

	unsigned int LIT[5][16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
		3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
		1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
		4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13 };

	unsigned int RIT[5][16] = { 5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
		6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
		15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
		8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
		12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11 };

	for (int i = 0; i != 5; i++){
		for (int j = 0; j != 16; j++){
			LS[i][j] = LST[i][j];//Initialization of rotation values in the left branch.
			RS[i][j] = RST[i][j];//Initialization of rotation values in the right branch.
			LI[i][j] = LIT[i][j];//Initialization of word permutation in the left branch.
			RI[i][j] = RIT[i][j];//Initialization of word permutation in the right branch.
		}
	}

	for (int i = 0; i < 32; i++){
		runningtimes[i] = 0;
		round2run[i] = 0;
	}

	in[10] = 0x1000000;
	out[10] = 0x1;

	in[11] = 0x15;
	out[11] = 0xa800;
	
	in[12] = 0x6ffba800;
	out[12] = 0xea001bff;

	in[13] = 0x40400001;
	out[13] = 0x1010;

	in[14] = 0xafffff5f;
	out[14] = 0xfff5fb00;

	in[15] = 0x9d020;
	out[15] = 0x2740800;

	in[16] = 0x85f87f2;
	out[16] = 0xbf0fe410;

	in[17] = 0;
	out[17] = 0;

	in[18] = 0xffffd008;
	out[18] = 0xe8040000;

	in[19] = 0xd75fbffc;
	out[19] = 0xafdffdec;

	in[20] = 0x10200813;
	out[20] = 0x812102;

	in[21] = 0xff7edffe;
	out[21] = 0x7edffeff;

	in[22] = 0x81000001;
	out[22] = 0x102;

	in[23] = 0xffffff00;
	out[23] = 0xfff80000;

	in[24] = 0x80000;
	out[24] = 0x4000000;

	in[25] = 0x1000800;
	out[25] = 0x80040000;

	in[26] = 0x7ffc0000;
	out[26] = 0xbffff800;

	in[27] = 0;
	out[27] = 0;

	in[28] = 0xc0000000;
	out[28] = 0xfffffff0;

	in[29] = 0x10;
	out[29] = 0x80000;

	generateAccurateEXP();
}

void RIPEMD160::generateAccurateEXP(){
	double index[10] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
	double t;
	for (int k = 0; k<32; k++){
		for (int i = 0; i<10; i++){
			t = pow(2, index[i]);
			accurateEXP[k][i] = EXP[k] * t;
		}
	}
}

double RIPEMD160::getRange(unsigned int number){
	if (number == 0){
		return 0;
	}

	int index = 31;
	for (int i = 0; i<31; i++){
		if (number >= EXP[i] && number<EXP[i + 1]){
			index = i;
			break;
		}
	}

	double result = 0.9;
	double t[10] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
	for (int i = 0; i<9; i++){
		if (number == accurateEXP[index][i] || (number>accurateEXP[index][i] && number<accurateEXP[index][i + 1])){
			result = t[i];
			break;
		}
	}

	return result + index;
}

//Compute the hash value according to the start step and the steps
void RIPEMD160::computeHash(int start, int steps){
	for (int i = start; i < steps; i++){
		if (i < 5){
			switch (i){
			case 0:
				X[0] = LL(LIV[1], 10) + LL(LL(LIV[0], 10) + XOR(LIV[4], LIV[3], LL(LIV[2], 10)) + word[LI[0][0]] + LC[0], LS[0][0]);
				Q[0] = LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0];
				Y[0] = LL(RIV[1], 10) + LL(LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0], RS[0][0]);
				break;
			case 1:
				X[1] = LL(LIV[2], 10) + LL(LL(LIV[1], 10) + XOR(X[0], LIV[4], LL(LIV[3], 10)) + word[LI[0][1]] + LC[0], LS[0][1]);
				Q[1] = LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0];
				Y[1] = LL(RIV[2], 10) + LL(LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0], RS[0][1]);
				break;
			case 2:
				X[2] = LL(LIV[3], 10) + LL(LL(LIV[2], 10) + XOR(X[1], X[0], LL(LIV[4], 10)) + word[LI[0][2]] + LC[0], LS[0][2]);
				Q[2] = LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0];
				Y[2] = LL(RIV[3], 10) + LL(LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0], RS[0][2]);
				break;
			case 3:
				X[3] = LL(LIV[4], 10) + LL(LL(LIV[3], 10) + XOR(X[2], X[1], LL(X[0], 10)) + word[LI[0][3]] + LC[0], LS[0][3]);
				Q[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0];
				Y[3] = LL(RIV[4], 10) + LL(LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0], RS[0][3]);
				break;
			case 4:
				X[4] = LL(X[0], 10) + LL(LL(LIV[4], 10) + XOR(X[3], X[2], LL(X[1], 10)) + word[LI[0][4]] + LC[0], LS[0][4]);
				Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0];
				Y[4] = LL(Y[0], 10) + LL(LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0], RS[0][4]);
				break;
			}
		}
		else if (i < 16){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + XOR(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[0][i]] + LC[0], LS[0][i]);
			Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0], RS[0][i]);
		}
		else if (i < 32){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1], RS[1][i - 16]);
		}
		else if (i < 48){
			//X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + ONZ(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[2][i - 32]] + LC[2], LS[2][i - 32]);
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + ONZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[2][i - 32]] + RC[2], RS[2][i - 32]);
		}
		else if (i < 64){
			//X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFZ(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[3][i - 48]] + LC[3], LS[2][i - 48]);
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + IFX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[3][i - 48]] + RC[3], RS[2][i - 48]);
		}
		else{
			//X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + ONX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[4][i - 64]] + LC[4], LS[2][i - 64]);
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + XOR(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[4][i - 64]] + RC[4], RS[2][i - 64]);
		}
	}
	//cout << hex << X[steps - 1] << " " << Y[steps - 1] << endl;
	/*cout << "Y:" << endl;
	for (unsigned int i = start; i < steps; i++){
		//cout << hex << i << ":" << X[i] << "\t" << Y[i] << endl;
		//cout << hex << i << ":" << Y[i] << endl;
		cout << i << ":";
		toBinary(Y[i]);
	}*/
	//cout << hex << i << ":" << X[i] << "\t" << Y[i] << endl;
	//cout << hex << i << ":" << Y[i] << endl;
	//cout  << "M11:";
	//toBinary(word[11]);

	//cout << endl;

	/*cout << "Q:" << endl;
	for (unsigned int i = start; i < steps; i++){
		//cout << hex << i << ":" << X[i] << "\t" << Y[i] << endl;
		//cout << hex << i << ":" << Y[i] << endl;
		cout << i << ":";
		toBinary(Q[i]);
	}
	cout << endl;*/
}

//Get random value
inline unsigned int RIPEMD160::getRand(){
	unsigned int num1 = 0, num2 = 0;
	num1 = rand() & 0xffff;
	num2 = rand() & 0xffff;
	return (num1 << 16) | num2;
}

inline bool RIPEMD160::checkQ(unsigned int q, int index,int shift){
	if (LL(q + in[index], shift) != out[index] + LL(q, shift)){
		return false;
	}
	return true;
}

void RIPEMD160::getModification(unsigned int &number, int *pos, int length, int condition){
	for (int i = 0; i < length; i++){
		unsigned int t = getBit(number, pos[i]);
		if (t != condition){
			number = number ^ EXP[pos[i]];
		}
	}
}

unsigned int RIPEMD160::getMultiModification(unsigned int number1, unsigned int number2, int *pos1, int *pos2, int length, int condition){
	for (int i = 0; i < length; i++){
		unsigned int t1 = getBit(number1, pos1[i]);
		unsigned int t2 = getBit(number2, pos2[i]);
		if (t1==1 && t2==0){
			number2 = number2 ^ EXP[pos2[i]];
		}
	}
	return number2;
}

inline void RIPEMD160::modifyY0(){
	int condition0[22] = { 23, 4, 3, 2, 1, 0, 28, 25, 17, 31, 24, 22, 20, 16, 7, 15, 30, 9, 8,14,27,21 };
	getModification(Y[0], condition0, 22, 0);

	int condition1[1] = { 29 };
	getModification(Y[0], condition1, 1, 1);
}

inline void RIPEMD160::modifyY1(){
	int condition0[19] = { 11, 12, 14, 20, 21, 22, 23, 24, 27, 28, 29, 5, 7, 9, 2, 0, 30, 26,31 };
	getModification(Y[1], condition0, 19, 0);
	
	int condition1[6] = { 8, 10, 17, 25, 18, 19 };
	getModification(Y[1], condition1, 6, 1);
}

inline void RIPEMD160::modifyY3(){
	int condition0[16] = { 9, 30, 0, 19, 25, 29, 23, 22, 21, 14, 12, 10, 8, 7, 6, 31 };
						// { 9, 30, 0, 19, 25, 29, 23, 22, 21, 14, 12, 10, 8, 7, 6, 31}
	getModification(Y[3], condition0, 16, 0);

	int condition1[4] = { 5,28,27,26 };
	getModification(Y[3], condition1, 4, 1);
}

inline void RIPEMD160::modifyY4(){
	int condition0[17] = { 14, 15, 25, 26, 27, 28, 4, 5, 11, 12, 2,8, 19, 10,29,3,9};
	int condition1[10] = { 7, 1, 0, 31, 24, 22, 20, 18, 17, 16};
	getModification(Y[4], condition0, 17, 0);
	getModification(Y[4], condition1, 10, 1);
}

inline void RIPEMD160::modifyY5(){
	//int condition0[1] = { 20};
	//getModification(Y[5], condition0,1, 0);
}

inline void RIPEMD160::modifyY6(){
	int condition1[2] = { 24,10};
	getModification(Y[6], condition1, 2, 1);

	int condition0[18] = { 13, 7, 6, 5, 3, 23, 22, 15, 14, 1, 2, 16, 17, 18, 19, 20, 21, 12 };
	getModification(Y[6], condition0, 18, 0);
}

inline void RIPEMD160::modifyY7(){

	int condition0[22] = { 21, 19, 20, 2, 23, 17, 16, 15, 13, 1, 0, 25, 24, 11, 12, 26, 27, 28, 29, 30, 31, 22};
	getModification(Y[7], condition0, 22, 0);
}

inline void RIPEMD160::modifyY8(){
	//11 bits
	int condition0[15] = { 20, 2, 27, 28, 30, 0,10, 11, 17,18,9,1,8,31,19 };
	int condition1[5] = { 3, 21, 22, 24, 26, };
	getModification(Y[8], condition0, 15, 0);
	getModification(Y[8], condition1, 5, 1);
}

inline void RIPEMD160::modifyY9(){
	//19 bits
	int condition1[5] = { 20, 17, 7, 3, 1};
	int condition0[16] = { 31, 27, 26, 25, 24, 23, 22, 13, 12, 11, 10, 5, 4, 2, 0, 30};
	getModification(Y[9], condition1, 5, 1);
	getModification(Y[9], condition0, 16, 0);
}

inline void RIPEMD160::modifyY10(){
	//26 bits
	int condition1[10] = { 31,19, 18, 16, 15, 12, 3, 2, 1, 0 };
	int condition0[17] = { 30, 27, 23, 22, 21, 20, 17, 14, 13, 11, 10, 9, 8, 7, 6, 5, 4 };
	getModification(Y[10], condition1, 10, 1);
	getModification(Y[10], condition0, 17, 0);

	//multi modification
	int pos1[5] = { 19, 18, 16, 15, 14 };
	int pos2[5] = { 29, 28, 26, 25, 24 };
	Y[10] = getMultiModification(Y[9], Y[10], pos1, pos2, 5, 1);
}

inline void RIPEMD160::modifyY11(){
	//28 bits
	int condition1[18] = { 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 3, 2};
	int condition0[10] = { 31, 14, 13, 12, 11, 10, 9, 4, 1, 0 };
	getModification(Y[11], condition1, 18, 1);
	getModification(Y[11], condition0, 10, 0);
}

inline void RIPEMD160::modifyY12(){
	//27 bits
	int condition1[19] = { 30, 27, 26, 25, 23, 22, 21,20, 19, 17, 13, 12, 9, 8, 6, 5, 3, 2, 1 };
	int condition0[11] = { 31, 29, 28, 18, 14, 11, 10, 7, 4, 0,16 };
	getModification(Y[12], condition1, 19, 1);
	getModification(Y[12], condition0, 11, 0);
}

inline void RIPEMD160::modifyY13(){
	//25 bits
	int condition1[19] = { 30, 25, 24, 23, 22, 17, 15, 12, 11, 9, 7, 4, 3, 1, 0, 6, 21, 20, 19 };
	int condition0[9] = { 29, 28, 27, 26, 14, 13, 10, 8, 5};
	getModification(Y[13], condition1, 19, 1);
	getModification(Y[13], condition0, 9, 0);
}

inline void RIPEMD160::modifyY14(){
	//21 bits
	int condition1[11] = { 27,25, 24, 23, 22, 19, 17, 15, 13, 8, 0};
	int condition0[16] = { 29,28,31, 30, 26, 20, 16,14, 12, 11, 10, 3, 1, 5,21,9 };
	getModification(Y[14], condition1, 11, 1);
	getModification(Y[14], condition0, 16, 0);
}

inline void RIPEMD160::modifyY15(){
	//25 bits
	int condition1[15] = { 31, 30, 29, 28,26, 25, 24, 19, 12, 5, 4, 3, 2, 1, 0 };
	int condition0[11] = { 27, 23, 22, 21, 20, 18, 15, 14, 13, 11, 6 };
	getModification(Y[15], condition1, 15, 1);
	getModification(Y[15], condition0, 11, 0);
}

//modifiy Round1
bool RIPEMD160::modifyRound1(){
	int i = 0;
	//unsigned int in = 0;
	//unsigned int out = 0;
	int shift = 0;
	for (i = 0; i < 5; i++){
		if (i < 5){
			switch (i){
			case 0:
				Q[0] = LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0];
				Y[0] = LL(RIV[1], 10) + LL(Q[0], RS[0][0]);
				modifyY0();
				word[RI[0][0]] = RR(Y[0] - LL(RIV[1], 10), RS[0][0]) - (Q[0] - word[RI[0][0]]);
				Q[0] = LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0];
				break;
			case 1:
				Q[1] = LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0];
				Y[1] = LL(RIV[2], 10) + LL(Q[1], RS[0][1]);
				modifyY1();
				word[RI[0][1]] = RR(Y[1] - LL(RIV[2], 10), RS[0][1]) - (Q[1] - word[RI[0][1]]);
				Q[1] = LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0];
				break;
			case 2:
				Q[2] = LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0];
				Y[2] = LL(RIV[3], 10) + LL(Q[2], RS[0][2]);
				break;
			case 3:
				Q[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0];
				Y[3] = LL(RIV[4], 10) + LL(Q[3], RS[0][3]);
				modifyY3();
				word[RI[0][3]] = RR(Y[3] - LL(RIV[4], 10), RS[0][3]) - (Q[3] - word[RI[0][3]]);
				Q[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0];
				break;
			case 4:
				Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0];
				Y[4] = LL(Y[0], 10) + LL(Q[4], RS[0][4]);
				modifyY4();
				word[RI[0][4]] = RR(Y[4] - LL(Y[0], 10), RS[0][4]) - (Q[4] - word[RI[0][4]]);
				Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0];
				break;
			}
		}
	}
	
	for (int i = 5; i < 10; i++){
		Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
		Y[i] = LL(Y[i - 4], 10) + LL(Q[i], RS[0][i]);
		if (i == 5){
			modifyY5();
		}
		else if (i == 6){
			modifyY6();
		}
		else if (i == 7){
			modifyY7();
		}
		else if (i == 8){
			modifyY8();
		}
		else if (i == 9){
			modifyY9();
		}
		word[RI[0][i]] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]) - (Q[i] - word[RI[0][i]]);
		Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
	}

	/*i = 6;//Y7
	Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
	Y[i] = LL(Y[i - 4], 10) + LL(Q[i], RS[0][i]);
	modifyY6();
	word[RI[0][i]] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]) - (Q[i] - word[RI[0][i]]);
	Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];

	i = 7;//Y7
	Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
	Y[i] = LL(Y[i - 4], 10) + LL(Q[i], RS[0][i]);
	modifyY7();
	word[RI[0][i]] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]) - (Q[i] - word[RI[0][i]]);
	Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];

	i = 8;//Y8
	Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
	Y[i] = LL(Y[i - 4], 10) + LL(Q[i], RS[0][i]);
	modifyY8();
	word[RI[0][i]] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]) - (Q[i] - word[RI[0][i]]);
	Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];

	i = 9;//Y9
	Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
	Y[i] = LL(Y[i - 4], 10) + LL(Q[i], RS[0][i]);
	modifyY9();
	word[RI[0][i]] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]) - (Q[i] - word[RI[0][i]]);
	Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
	*/

	for (i = 10; i < 16; i++){//Y10-Y15
		Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
		Y[i] = LL(Y[i - 4], 10) + LL(Q[i], RS[0][i]);

		if (i==10)
			modifyY10();
		else if (i == 11)
			modifyY11();
		else if (i == 12)
			modifyY12();
		else if (i == 13)
			modifyY13();
		else if (i == 14)
			modifyY14();
		else if (i == 15)
			modifyY15();

		word[RI[0][i]] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]) - (Q[i] - word[RI[0][i]]);
		Q[i] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]);
		shift = RS[0][i];

		if (checkQ(Q[i], i, shift) == false){//check Q[i]
			cout <<hex<<i<<":";
			toBinary(Q[i]);
			runningtimes[i]++;
			//system("pause");
			return false;
		}

	}
	return true;
}

bool RIPEMD160::modifyY16(){
	round2run[16]++;
	//runningtimes[16]++;
	//Modify from low bit to high bit
	int bitpos[27] =    { 1,2, 3, 4, 5, 7, 8, 9, 10,11, 12, 13, 14, 15, 17, 18, 19, 20, 21, 23, 24, 26, 27, 28, 29, 30, 31 };
	int condition[27] = { 1,0, 0, 0, 0, 0, 0, 0, 1,  0,  1,  1,  0,  0,  1,  1,  1,  0,  1,  0,  1,  1,  1,  0,  1,  1,  1 };
	unsigned int x = 0;
	int pos = 0;
	int k = 0;

	for (int i = 0; i < 27; i++){
		//修改
		if (getBit(Y[16], bitpos[i]) != condition[i]){
			pos = (bitpos[i] - 9 + 32) % 32;
			//Case 4: bitpos[i]=3
			if (bitpos[i] == 3){
				//cout << "3 is wrong!" << endl;
				word[6] = word[6] + EXP[26];
				Q[9] = Q[9] + EXP[26];
				Y[5] = RR(Y[9] - LL(Q[9], RS[0][9]), 10);

				for (k = 5; k < 11; k++){//Y[5]--Y[10]
					if (k != 9){
						word[RI[0][k]] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]) - LL(Y[k - 5], 10) - ONX(Y[k - 1], Y[k - 2], LL(Y[k - 3], 10)) - RC[0];
						Q[k] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]);
					}
				}
			}
			//Case 3: Y[7],Y[8],Y[9],Y[10],Y[11],Y[12]
			//pos: 26,20
			else if (bitpos[i] == 11 || bitpos[i] == 29){
				k = 7;
				x = RR(LL(Q[k], RS[0][k]) + EXP[pos], RS[0][k]) - Q[k];
				word[RI[0][k]] = word[RI[0][k]] + x;
				Q[k] = Q[k] + x;
				Y[k] = LL(Y[k-4], 10) + LL(Q[k], RS[0][k]);

				for (k = 8; k < 13; k++){
					word[RI[0][k]] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]) - LL(Y[k - 5], 10) - ONX(Y[k - 1], Y[k - 2], LL(Y[k - 3], 10)) - RC[0];
					Q[k] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]);
				}
				if (checkQ(Q[11], 11, RS[0][11]) == false){
					cout << "Modify Y16, wrong Q11!" << endl;
					return false;
				}
			}

			//Case 2: Y[8],Y[9],Y[10],Y[11],Y[12],Y[13]
			//pos: 27,28,30, 0,10,11, 17,18
			else if (bitpos[i] == 4 || bitpos[i] == 5 || bitpos[i] == 7 || bitpos[i] == 8 || bitpos[i] == 9 || bitpos[i] == 10 || bitpos[i] == 17 || bitpos[i] == 18 || bitpos[i] == 19 || bitpos[i] == 20 || bitpos[i] == 26 || bitpos[i] == 27 || bitpos[i] == 28){//Case 2
				//if (bitpos[i] == 28){
					//cout << "Y16,28 wrong!" << endl;
				//}
				k = 8;
				x = RR(LL(Q[k], RS[0][k]) + EXP[pos], RS[0][k]) - Q[k];
				word[RI[0][k]] = word[RI[0][k]] + x;
				Q[k] = Q[k] + x;
				Y[k] = LL(Y[k - 4], 10) + LL(Q[k], RS[0][k]);

				for (k = 9; k < 14; k++){
					word[RI[0][k]] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]) - LL(Y[k - 5], 10) - ONX(Y[k - 1], Y[k - 2], LL(Y[k - 3], 10)) - RC[0];
					Q[k] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]);
				}
				if (checkQ(Q[12], 12, RS[0][12]) == false){
					cout << "Modify Y16, wrong Q12!" << endl;
					return false;
				}
			}

			//Case 1: Y[4],Y[5],Y[6],Y[7],Y[8],Y[9]
			else{

				k = 4;
				x = RR(LL(Q[k], RS[0][k]) + EXP[(pos-10+32)%32], RS[0][k]) - Q[k];
				word[RI[0][k]] = word[RI[0][k]] + x;
				Q[k] = Q[k] + x;
				Y[k] = LL(Y[k - 4], 10) + LL(Q[k], RS[0][k]);

				for (k = 5; k < 10; k++){
					word[RI[0][k]] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]) - LL(Y[k - 5], 10) - ONX(Y[k - 1], Y[k - 2], LL(Y[k - 3], 10)) - RC[0];
					Q[k] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]);
				}
			}

			Q[16] = LL(Y[11], 10) + IFZ(Y[15], Y[14], LL(Y[13], 10)) + word[6] + RC[1];
			Y[16] = LL(Y[12], 10) + LL(Q[16], RS[1][0]);

			if (getBit(Y[16], bitpos[i]) != condition[i]){
				cout << "Y16(Inner): " << dec<<bitpos[i] << endl;
				return false;
			}
		}
	}
	if (checkY16() == false){
		runningtimes[16]++;
		return false;
	}


	if (checkQ(Q[16], 16, RS[1][0]) == false){//check Q[16]
		cout << "Modify Q16,Wrong Q16!" << endl;
		cout << "Q16':";
		toBinary(LL(Q[16], RS[1][0]));
		cout << "Y12':";
		toBinary(LL(Y[12], 10));
		cout << "Y16:";
		toBinary(Y[16]);
		cout << endl;
		//runningtimes[16]++;
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkY16(){
	int bitpos[27] = { 1,2, 3, 4, 5, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 20, 21, 23, 24, 26, 27, 28, 29, 30, 31 };
	int condition[27] = { 1,0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1 };

	for (int i = 0; i < 27; i++){
		if (getBit(Y[16], bitpos[i]) != condition[i]){
			/*cout << hex << "Y16" << ": ";
			toBinary(Y[16]);
			cout << hex << "Y16" << ": " << Y[16] << endl;
			cout << "Y16(Out): " << dec << getBit(Y[16], bitpos[i]) << endl;
			cout << "Y16(Out): " << dec << condition[i] << endl;
			cout << "Y16(Out): " <<dec<< bitpos[i] << endl;*/
			//cout << "Y16(Out): " << dec << bitpos[i] << endl;
			return false;
		}
	}
	return true;
}

bool RIPEMD160::modifyY17(){
	round2run[17]++;

	//Modify from low bit to high bit
	int bitpos[20] =    { 0, 2, 3, 5, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 22, 23, 28, 30, 31 };
	int condition[20] = { 1, 0, 0, 0,  0,  0,  0,  0,  0,  1,  2,  1,  1,  0,  2,  0,  0,  2,  1,  0 };
	unsigned int x = 0;
	int pos = 0;
	for (int i = 0; i < 20; i++){
		if ((condition[i] != 2 && getBit(Y[17], bitpos[i]) != condition[i]) || (condition[i] == 2 && getBit(Y[17], bitpos[i]) != getBit(Y[16], bitpos[i]))){
			pos = (bitpos[i] - 23 + 32) % 32;


			if (bitpos[i] == 0 || bitpos[i] == 10 || bitpos[i] == 16 || bitpos[i] == 21 || bitpos[i] == 22 || bitpos[i] == 23){//Case 2 Y[3]--Y[8]
				x = RR(LL(Q[3], RS[0][3]) + EXP[pos], RS[0][3]) - Q[3];
				Q[3] = Q[3] + x;
				word[0] = word[0] + x;
				Y[3] = LL(RIV[4], 10) + LL(Q[3], RS[0][3]);//new Y[3]  Y[3,pos]

				word[9] = RR(Y[4] - LL(Y[0], 10), RS[0][4]) - (LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + RC[0]);//Y[4] stay the same value
				word[2] = RR(Y[5] - LL(Y[1], 10), RS[0][5]) - (LL(Y[0], 10) + ONX(Y[4], Y[3], LL(Y[2], 10)) + RC[0]);//Y[5] stay the same value
				word[11] = RR(Y[6] - LL(Y[2], 10), RS[0][6]) - (LL(Y[1], 10) + ONX(Y[5], Y[4], LL(Y[3], 10)) + RC[0]);//Y[6] stay the same value
				word[4] = RR(Y[7] - LL(Y[3], 10), RS[0][7]) - (LL(Y[2], 10) + ONX(Y[6], Y[5], LL(Y[4], 10)) + RC[0]);//Y[7] stay the same value
				Q[7] = LL(Y[2], 10) + ONX(Y[6], Y[5], LL(Y[4], 10)) + RC[0] + word[4];//update Q[7].
				word[13] = RR(Y[8] - LL(Y[4], 10), RS[0][8]) - (LL(Y[3], 10) + ONX(Y[7], Y[6], LL(Y[5], 10)) + RC[0]);//Y[8] stay the same value
			}

			else if (bitpos[i] == 31){//Case 3: Y[2]--Y[7]
				//cout << "31 is wrong!" << endl;
				word[11] = word[11] + EXP[18];
				Q[6] = Q[6] + EXP[18];
				Y[2] = RR(Y[6] - LL(Q[6], RS[0][6]), 10);

				for (int t = 2; t < 8; t++){
					if (t < 5){
						switch (t){
						case 2:
							word[RI[0][2]] = RR(Y[2] - LL(RIV[3], 10), RS[0][2]) - LL(RIV[2], 10) - ONX(Y[1], Y[0], LL(RIV[4], 10)) - RC[0];
							Q[2] = LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0];
							break;
						case 3:
							word[RI[0][3]] = RR(Y[3] - LL(RIV[4], 10), RS[0][3]) - LL(RIV[3], 10) - ONX(Y[2], Y[1], LL(Y[0], 10)) - RC[0];
							Q[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0];
							//Y[3] = LL(RIV[4], 10) + LL(Q[3], RS[0][3]);
							break;
						case 4:
							word[RI[0][4]] = RR(Y[4] - LL(Y[0], 10), RS[0][4]) - LL(RIV[4], 10) - ONX(Y[3], Y[2], LL(Y[1], 10)) - RC[0];
							Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0];
							//Y[4] = LL(Y[0], 10) + LL(Q[4], RS[0][4]);
							break;
						}
					}
					else if(t!=6){
						word[RI[0][t]] = RR(Y[t] - LL(Y[t - 4], 10), RS[0][t]) - LL(Y[t - 5], 10) - ONX(Y[t - 1], Y[t - 2], LL(Y[t - 3], 10)) - RC[0];
						Q[t] = LL(Y[t - 5], 10) + ONX(Y[t - 1], Y[t - 2], LL(Y[t - 3], 10)) + word[RI[0][t]] + RC[0];
						//Y[t] = LL(Y[t - 4], 10) + LL(Q[t], RS[0][t]);
					}
				}
			}

			else{//Case 1 Y[1]--Y[6]
				//Y[3,pos-10]
				x = RR(LL(Q[1], RS[0][1]) + EXP[pos], RS[0][1]) - Q[1];
				Q[1] = Q[1] + x;
				word[14] = word[14] + x;

				Y[1] = LL(RIV[2], 10) + LL(Q[1], RS[0][1]);//new Y[1]  Y[3,pos-10]
				word[7] = RR(Y[2] - LL(RIV[3], 10), RS[0][2]) - (LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + RC[0]);//Y[2] stay the same value
				word[0] = RR(Y[3] - LL(RIV[4], 10), RS[0][3]) - (LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + RC[0]);//Y[3] stay the same value
				word[9] = RR(Y[4] - LL(Y[0], 10), RS[0][4]) - (LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + RC[0]);//Y[4] stay the same value
				word[2] = RR(Y[5] - LL(Y[1], 10), RS[0][5]) - (LL(Y[0], 10) + ONX(Y[4], Y[3], LL(Y[2], 10)) + RC[0]);//Y[5] stay the same value
				Q[5] = LL(Y[0], 10) + ONX(Y[4], Y[3], LL(Y[2], 10)) + RC[0] + word[2];//Q[5] should be updated.
				word[11] = word[11] - EXP[(pos + 10) % 32];//Y[6] stay the same value

				/*///Test Y[3],Y[4],Y[5],Y[6],Y[7],Y[8]
				for (int i = 3; i < 9; i++){
				if (i == 3){
				Y[3] = LL(RIV[4], 10) + LL(LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0], RS[0][3]);
				}
				else if (i == 4){
				Y[4] = LL(Y[0], 10) + LL(LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0], RS[0][4]);
				}
				else{
				Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0], RS[0][i]);
				}
				cout << hex << "Y" << i << ": ";
				toBinary(Y[i]);
				cout << hex << "Y" << i << ": " << Y[i] << endl;
				}
				cout << endl;
				system("pause");
				///*/
			}

			Q[17] = LL(Y[12], 10) + IFZ(Y[16], Y[15], LL(Y[14], 10)) + word[11] + RC[1];
			Y[17] = LL(Y[13], 10) + LL(Q[17], RS[1][1]);
			if ((condition[i] != 2 && getBit(Y[17], bitpos[i]) != condition[i]) || (condition[i] == 2 && getBit(Y[17], bitpos[i]) != getBit(Y[16], bitpos[i]))){
				cout << "Y117(Inner) :" << bitpos[i] << endl;
				system("pause");
				return false;
			}
		}
	}

	if (checkY17() == false){
		runningtimes[17]++;
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkY17(){
	int bitpos[20] =    { 0, 2, 3, 5, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 22, 23, 28, 30, 31 };
	int condition[20] = { 1, 0, 0, 0,  0,  0,  0,  0,  0,  1,  2,  1,  1,  0,  2,  0,  0,  2,  1, 0 };

	for (int i = 0; i < 20; i++){
		if (condition[i] != 2){
			if (getBit(Y[17], bitpos[i]) != condition[i]){
				//cout << "Y17(Out): " << bitpos[i] << endl;
				return false;
			}
		}
		else{
			if (getBit(Y[17], bitpos[i]) != getBit(Y[16], bitpos[i])){
				//cout << "Y17(Out): " << bitpos[i] << endl;
				return false;
			}
		}
	}
	return true;
}

bool RIPEMD160::modifyY18(){
	round2run[18]++;
	int bitpos[14] = { 19, 18, 15, 13, 31, 30, 23, 20, 14, 2, 0, 17, 8, 1 };
	int condition[14] = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2 };
	unsigned int x;
	unsigned int pos;
	int k = 0;
	for (int i = 0; i < 14; i++){
		if ((condition[i] != 2 && getBit(Y[18], bitpos[i]) != condition[i]) || (condition[i] == 2 && getBit(Y[18], bitpos[i]) != getBit(Y[17], bitpos[i]))){
			pos = bitpos[i];

			if (bitpos[i] == 19){
				Q[14] = RR(Y[14] - LL(Y[10], 10), RS[0][14]);
				word[3] = word[3] + EXP[4];
				if (LL(Q[14], 12) + EXP[16] != LL(Q[14] + EXP[4], 12)){//Pro:1-2^{-16}
					return false;
				}
				Q[14] = Q[14] + EXP[4];
				if (checkQ(Q[14], 14, RS[0][14]) == false){
					cout << "Modify Y18, WRONG Q14!" << endl;
					return false;
				}
				Y[14] = LL(Y[10], 10) + LL(Q[14], RS[0][14]);//Y[14,+16]
				word[12] = RR(Y[15] - LL(Y[11], 10), RS[0][15]) - (LL(Y[10], 10) + ONX(Y[14], Y[13], LL(Y[12], 10)) + RC[0]);//Y[15] stay the same value
				//Y[16],Y[17] stay the same
			}

			/*else if(bitpos[i]==14){
				cout << "Y" << dec << 5 << ":";
				toBinary(Y[5]);
				cout << hex << Y[5] << endl;
				for (int t = 8; t < 10; t++){
					cout << "Y" << dec << t << ":";
					toBinary(Y[t]);
					cout << hex << Y[t] << endl;

					cout << "Q" << dec << t << ":";
					toBinary(Q[t]);
					cout << hex << Q[t] << endl;
				}
				cout << endl;

				k = 8;
				x = RR(LL(Q[k], RS[0][k]) + EXP[pos], RS[0][k]) - Q[k];
				word[RI[0][k]] = word[RI[0][k]] + x;
				Q[k] = Q[k] + x;
				Y[k] = LL(Y[k - 4], 10) + LL(Q[k], RS[0][k]);

				Q[k + 1] = LL(Y[k - 4], 10) + ONX(Y[k], Y[k - 1], LL(Y[k - 2], 10)) + word[RI[0][k + 1]] + RC[0];
				if (LL(Q[9] + EXP[pos], RS[0][9]) != LL(Q[9], RS[0][9]) + EXP[(pos + RS[0][9]) % 32]){
					cout << "Modify Y18,14, wrong Q9!" << endl;
					system("pause");
				}

				Y[k + 1] = LL(Y[k - 3], 10) + LL(Q[k + 1], RS[0][k + 1]);

				for (k = 10; k < 15; k++){
					word[RI[0][k]] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]) - LL(Y[k - 5], 10) - ONX(Y[k - 1], Y[k - 2], LL(Y[k - 3], 10)) - RC[0];
					Q[k] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]);
					Y[k] = LL(Y[k - 4], 10) + LL(Q[k], RS[0][k]);
				}
				if (checkQ(Q[12], 12, RS[0][12]) == false){
					cout << "Modify Y18, wrong Q12!" << endl;
					return false;
				}

				cout << "Y" << dec << 5 << ":";
				toBinary(Y[5]);
				cout << hex << Y[5] << endl;
				for (int t = 8; t < 10; t++){
					cout << "Y" << dec << t << ":";
					toBinary(Y[t]);
					cout << hex << Y[t] << endl;

					cout << "Q" << dec << t << ":";
					toBinary(Q[t]);
					cout << hex << Q[t] << endl;
				}
				cout << endl;

				if (checkQ(Q[13], 13, RS[0][13]) == false){
					cout << "Q13':";
					toBinary(LL(Q[13], RS[0][13]));
					cout << "Y9':" << endl;
					toBinary(LL(Y[9], 10));
					cout << "Y13 :" << endl;
					toBinary(Y[13]);
					cout << "WRONG Q13!" << endl;
					system("pause");

					cout << "Modify Y18, wrong Q13!" << endl;
					return false;
				}
			}*/

			else{
				return  false;
			}

			Q[18] = LL(Y[13], 10) + IFZ(Y[17], Y[16], LL(Y[15], 10)) + word[3] + RC[1];
			Y[18] = LL(Y[14], 10) + LL(Q[18], RS[1][2]);

			if ((condition[i] != 2 && getBit(Y[18], bitpos[i]) != condition[i]) || (condition[i] == 2 && getBit(Y[18], bitpos[i]) != getBit(Y[17], bitpos[i]))){
				cout << "Y18(Inner): " <<dec<< bitpos[i] << endl;
				return false;
			}

		}
	}
	
	if (checkY18() == false){
		runningtimes[18]++;
		return false;
	}
	if (checkQ(Q[18], 18, RS[1][2]) == false){
		cout << "Q18':";
		toBinary(LL(Q[18], RS[1][2]));
		cout << "Y14':" << endl;
		toBinary(LL(Y[14], 10));
		cout << "Y18 :" << endl;
		toBinary(Y[18]);
		cout << "WRONG Q18!" << endl;
		system("pause");
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkY18(){
	int bitpos[14] =    { 19, 18, 15, 13, 31, 30, 23, 20, 14, 2, 0, 17, 8, 1 };
	int condition[14] = {  0,  0,  0,  0,  1,  1,  1,  1,  1, 1, 1,  2, 2, 2};
	for (int i = 0; i < 14; i++){
		if (condition[i] != 2){
			if (getBit(Y[18], bitpos[i]) != condition[i]){
				//cout << "Y18(Out): " << bitpos[i] << endl;
				return false;
			}
		}
		else{
			if (getBit(Y[18], bitpos[i]) != getBit(Y[17], bitpos[i])){
				//cout << "Y18(Out): " << bitpos[i] << endl;
				return false;
			}
		}
	}
	return true;
}

bool RIPEMD160::modifyY19(){
	round2run[19]++;
	int bitpos[11] =    { 0, 2, 3,7, 8, 9, 10, 11, 21, 24,30 };
	int condition[11] = { 0, 1, 1,1, 1, 1,  0,  0,  1,  2, 0 };
	int pos = 0;
	unsigned int x = 0;

	for (int i = 0; i < 11; i++){
		if ((condition[i] != 2 && getBit(Y[19], bitpos[i]) != condition[i]) || (condition[i] == 2 && getBit(Y[19], bitpos[i]) != getBit(Y[18], bitpos[i]))){
			/*///////
			for (int t = 0; t < 6; t++){
				cout << hex << "Y" << t << ": ";
				toBinary(Y[t]);
				cout << hex << "Y" << t << ": " << Y[t] << endl;
			}
			cout << endl;
			///////*/
			pos = (bitpos[i] - 7 + 32) % 32;
			x = RR(LL(Q[0], RS[0][0]) + EXP[pos], RS[0][0]) - Q[0];
			Q[0] = Q[0] + x;
			word[5] = word[5] + x;

			Y[0] = LL(RIV[1], 10) + LL(Q[0], RS[0][0]);//new Y[0]  Y[0,pos-10]
			word[14] = RR(Y[1] - LL(RIV[2], 10), RS[0][1]) - (LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + RC[0]);//Y[1] stay the same value
			word[7] = RR(Y[2] - LL(RIV[3], 10), RS[0][2]) - (LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + RC[0]);//Y[2] stay the same value
			word[0] = RR(Y[3] - LL(RIV[4], 10), RS[0][3]) - (LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + RC[0]);//Y[3] stay the same value
			word[9] = RR(Y[4] - LL(Y[0], 10), RS[0][4]) - (LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + RC[0]);//Y[4] stay the same value
			Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + RC[0] + word[9];//Q[4] should be updated.
			word[2] = RR(Y[5] - LL(Y[1], 10), RS[0][5]) - (LL(Y[0], 10) + ONX(Y[4], Y[3], LL(Y[2], 10)) + RC[0]);//Y[5] stay the same value

			/*///////
			for (int t = 0; t < 6; t++){
				if (t < 5){
					switch (t){
					case 0:
						Q[0] = LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0], RS[0][0];
						Y[0] = LL(RIV[1], 10) + LL(LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0], RS[0][0]);
						break;
					case 1:
						Q[1] = LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0], RS[0][1];
						Y[1] = LL(RIV[2], 10) + LL(LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0], RS[0][1]);
						break;
					case 2:
						Q[2] = LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0], RS[0][2];
						Y[2] = LL(RIV[3], 10) + LL(LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0], RS[0][2]);
						break;
					case 3:
						Q[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0], RS[0][3];
						Y[3] = LL(RIV[4], 10) + LL(LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0], RS[0][3]);
						break;
					case 4:
						Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0], RS[0][4];
						Y[4] = LL(Y[0], 10) + LL(LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0], RS[0][4]);
						break;
					}
				}
				else{
					Y[t] = LL(Y[t - 4], 10) + LL(LL(Y[t - 5], 10) + ONX(Y[t - 1], Y[t - 2], LL(Y[t - 3], 10)) + word[RI[0][t]] + RC[0], RS[0][t]);
				}
				cout << hex << "Y" << t << ": ";
				toBinary(Y[t]);
				cout << hex << "Y" << t << ": " << Y[t] << endl;
			}
			cout << endl;
			system("pause");
			///////*/

			Q[19] = LL(Y[14], 10) + IFZ(Y[18], Y[17], LL(Y[16], 10)) + word[7] + RC[1];//
			Y[19] = LL(Y[15], 10) + LL(Q[19], RS[1][3]);

			if ((condition[i] != 2 && getBit(Y[19], bitpos[i]) != condition[i]) || (condition[i] == 2 && getBit(Y[19], bitpos[i]) != getBit(Y[18], bitpos[i]))){
				cout << "Y19(Inner): " << bitpos[i] << endl;
				return false;
			}
		}
	}
	if (checkQ(Q[19], 19, RS[1][3]) == false){
		/*cout << "Q19':";
		toBinary(LL(Q[19], RS[1][3]));
		cout << "Y15':" << endl;
		toBinary(LL(Y[15], 10));
		cout << "Y19 :" << endl;
		toBinary(Y[19]);
		cout << "WRONG Q19!" << endl;
		system("pause");*/
		cout << "WRONG Q19!" << endl;
		runningtimes[19]++;
		return false;
	}
	if (checkY19()==false){
		runningtimes[19]++;
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkY19(){
	int bitpos[11] = { 0, 2, 3, 7, 8, 9, 10, 11, 21, 24, 30 };
	int condition[11] = { 0, 1, 1, 1, 1, 1, 0, 0, 1, 2, 0 };
	for (int i = 0; i < 11; i++){
		if (condition[i] != 2){
			if (getBit(Y[19], bitpos[i]) != condition[i]){
				cout << "Y19(Out): " << bitpos[i] << endl;
				return false;
			}
		}
		else{
			if (getBit(Y[19], bitpos[i]) != getBit(Y[18], bitpos[i])){
				cout << "Y19(Out): " << bitpos[i] << endl;
				return false;
			}
		}
	}
	return true;
}

bool RIPEMD160::modifyY20(){
	round2run[20]++;
	int bitpos[15] =    { 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 21, 22, 23, 24, 30 };
	int condition[15] = { 1, 1, 0, 1,  2,  1,  1,  1,  1,  1,  1,  1,  0,  1,  1 };
	int pos = 0;
	unsigned int x = 0;
	int k = 0;

	for (int i = 0; i < 15; i++){
		if ((condition[i] != 2 && getBit(Y[20], bitpos[i]) != condition[i]) || (condition[i] == 2 && getBit(Y[20], bitpos[i]) != getBit(Y[16], bitpos[i]-10))){
			pos = (bitpos[i] - 12 + 32) % 32;

			//cout << "Y20: pos:" << dec << bitpos[i] << endl;

			if (bitpos[i] == 6 || bitpos[i] == 10 || bitpos[i] == 11 || bitpos[i] == 12 || bitpos[i] == 14 || bitpos[i] == 21){//Case 2: Y[0]--Y[5]
				/*////////
				for (int t = 0; t < 6; t++){
					cout << hex << "Y" << t << ": ";
					toBinary(Y[t]);
					cout << hex << "Y" << t << ": " << Y[t] << endl;
				}
				cout << endl;
				////////*/
				//cout << "word0:";
				//toBinary(word[0]);

				//if (bitpos[i] == 11){
					//cout << "Y20,11 is wrong!" << endl;
					//system("pause");
				//}

				k = 0;
				x = RR(LL(Q[k], RS[0][k]) + EXP[(pos-10+32)%32], RS[0][k]) - Q[k];
				word[RI[0][k]] = word[RI[0][k]] + x;
				Q[k] = Q[k] + x;
				Y[k] = LL(RIV[1], 10) + LL(Q[k], RS[0][k]);

				word[RI[0][1]] = RR(Y[1] - LL(RIV[2], 10), RS[0][1]) - LL(RIV[1], 10) - ONX(Y[0], RIV[4], LL(RIV[3], 10)) - RC[0];//Y[1] stay the same value
				Q[1] = LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + RC[0] + word[RI[0][1]];
				//Y[2] stay the same value
				word[RI[0][3]] = RR(Y[3] - LL(RIV[4], 10), RS[0][3]) - (LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + RC[0]);//Y[3] stay the same value
				Q[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + RC[0] + word[RI[0][3]];

				word[RI[0][4]] = RR(Y[4] - LL(Y[0], 10), RS[0][4]) - (LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + RC[0]);//Y[4] stay the same value
				Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + RC[0] + word[9];//Q[4] should be updated.

				word[RI[0][5]] = RR(Y[5] - LL(Y[1], 10), RS[0][5]) - (LL(Y[0], 10) + ONX(Y[4], Y[3], LL(Y[2], 10)) + RC[0]);//Y[5] stay the same value
				Q[5] = LL(Y[0], 10) + ONX(Y[4], Y[3], LL(Y[2], 10)) + RC[0] + word[RI[0][5]];

				//cout << "word0:";
				//toBinary(word[0]);

				/*///////
				for (int t = 0; t < 6; t++){
					if (t < 5){
						switch (t){
						case 0:
							Q[0] = LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0], RS[0][0];
							Y[0] = LL(RIV[1], 10) + LL(Q[0], RS[0][0]);
							break;
						case 1:
							Q[1] = LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0], RS[0][1];
							Y[1] = LL(RIV[2], 10) + LL(Q[1], RS[0][1]);
							break;
						case 2:
							Q[2] = LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0], RS[0][2];
							Y[2] = LL(RIV[3], 10) + LL(Q[2], RS[0][2]);
							break;
						case 3:
							Q[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0], RS[0][3];
							Y[3] = LL(RIV[4], 10) + LL(Q[3], RS[0][3]);
							break;
						case 4:
							Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0], RS[0][4];
							Y[4] = LL(Y[0], 10) + LL(Q[4], RS[0][4]);
							break;
						}
					}
					else{
						Q[t] = LL(Y[t - 5], 10) + ONX(Y[t - 1], Y[t - 2], LL(Y[t - 3], 10)) + word[RI[0][t]] + RC[0];
						Y[t] = LL(Y[t - 4], 10) + LL(Q[t], RS[0][t]);
					}
					cout << hex << "Y" << t << ": ";
					toBinary(Y[t]);
					cout << hex << "Y" << t << ": " << Y[t] << endl;
				}
				cout << endl;
				system("pause");
				///////*/
			}

			else{// Case 2: Y[3]--Y[8]
				//check whether (Q[3]+EXP[pos])<<<12 =	Q[3]<<<12 + EXP[pos+12] (pos:18,12,11,10,3,1,31,29,28,27)
				if (LL(Q[3] + EXP[pos], RS[0][3]) != LL(Q[3], RS[0][3]) + EXP[(pos + RS[0][3]) % 32]){//Holds with high probability
					cout << "Modify Y20, Propagation:" << dec << pos << endl;
					cout << "Q3':";
					toBinary(LL(Q[3], RS[0][3]));
					cout << "Y1':";
					toBinary(LL(RIV[4], 10));
					cout << "Y3:";
					toBinary(Y[3]);
					runningtimes[20]++;
					return false;
				}

				Q[3] = Q[3] + EXP[pos];
				word[0] = word[0] + EXP[pos];
				Y[3] = LL(RIV[4], 10) + LL(Q[3], RS[0][3]);//new Y[3]  Y[3,pos]

				for (k = 4; k < 9; k++){//Y[5]--Y[10]
					if (k == 4){
						word[RI[0][k]] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]) - LL(RIV[4], 10) - ONX(Y[k - 1], Y[k - 2], LL(Y[k - 3], 10)) - RC[0];
						Q[k] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]);
					}
					else{
						word[RI[0][k]] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]) - LL(Y[k - 5], 10) - ONX(Y[k - 1], Y[k - 2], LL(Y[k - 3], 10)) - RC[0];
						Q[k] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]);
					}
				}
				//word[9] = RR(Y[4] - LL(Y[0], 10), RS[0][4]) - (LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + RC[0]);//Y[4] stay the same value
				//word[2] = RR(Y[5] - LL(Y[1], 10), RS[0][5]) - (LL(Y[0], 10) + ONX(Y[4], Y[3], LL(Y[2], 10)) + RC[0]);//Y[5] stay the same value
				//Y[6] stay the same value by adding conditions on Y[4]
				//word[4] = RR(Y[7] - LL(Y[3], 10), RS[0][7]) - (LL(Y[2], 10) + ONX(Y[6], Y[5], LL(Y[4], 10)) + RC[0]);//Y[7] stay the same value
				//Q[7] = LL(Y[2], 10) + ONX(Y[6], Y[5], LL(Y[4], 10)) + RC[0] + word[4];//update Q[7].
				//word[13] = RR(Y[8] - LL(Y[4], 10), RS[0][8]) - (LL(Y[3], 10) + ONX(Y[7], Y[6], LL(Y[5], 10)) + RC[0]);//Y[8] stay the same value
				
			}

			Q[20] = LL(Y[15], 10) + IFZ(Y[19], Y[18], LL(Y[17], 10)) + word[RI[1][4]] + RC[1];//
			Y[20] = LL(Y[16], 10) + LL(Q[20], RS[1][4]);

			if ((condition[i] != 2 && getBit(Y[20], bitpos[i]) != condition[i]) || (condition[i] == 2 && getBit(Y[20], bitpos[i]) != getBit(Y[16], bitpos[i]-10))){
				cout << "Y20:" << bitpos[i] << endl;
				runningtimes[20]++;
				return false;
			}
		}
	}
	if (checkQ(Q[20], 20, RS[1][4])==false){
		cout << "Modify Y20, wrong Q20!" << endl;
		cout << "Y16':";
		toBinary(LL(Y[16], 10));
		cout << "Q20':";
		toBinary(LL(Q[20], RS[1][4]));
		cout << "Y20:";
		toBinary(Y[20]);
		cout << endl;
		return false;
	}
	if (checkY20() == false){
		runningtimes[20]++;
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkY20(){
	int bitpos[15] = { 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 21, 22, 23, 24, 30 };
	int condition[15] = { 1, 1, 0, 1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1 };
	for (int i = 0; i < 15; i++){
		if (condition[i] != 2){
			if (getBit(Y[20], bitpos[i]) != condition[i]){
				cout << bitpos[i] << endl;
				return false;
			}
		}
		else{
			if (getBit(Y[20], bitpos[i]) != getBit(Y[16], bitpos[i]- 10)){
				cout << bitpos[i] << endl;
				return false;
			}
		}
	}
	return true;
}

bool RIPEMD160::modifyY21(){
	round2run[21]++;
	int bitpos[18] =    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 19, 20, 21, 23, 24, 25, 30, 31 };
	int condition[18] = { 0, 0, 2, 2, 2, 2, 2, 2, 1, 1,  3,  3,  1,  1,  0,  0,  1,  1 };
	unsigned int x = 0;
	int pos = 0;
	int k = 0;
	for (int i = 0; i < 18; i++){
		if ((condition[i] <2 && getBit(Y[21], bitpos[i]) != condition[i]) || (condition[i] == 2 && getBit(Y[21], bitpos[i]) != getBit(Y[17], (bitpos[i] - 10 + 32) % 32)) || (condition[i] == 3 && getBit(Y[21], bitpos[i]) != getBit(Y[20], bitpos[i]))){
			//Y7--Y12
			//cout << dec << "pos:" << bitpos[i] << endl;

			unsigned int preF = ONX(Y[7], Y[6], LL(Y[5], 10));

			pos = (bitpos[i] - 8 + 32) % 32;
			k = 7;
			x = RR(LL(Q[k], RS[0][k]) + EXP[pos], RS[0][k]) - Q[k];
			word[RI[0][k]] = word[RI[0][k]] + x;
			Q[k] = Q[k] + x;
			Y[k] = LL(Y[k - 4], 10) + LL(Q[k], RS[0][k]);
		
			word[13] = word[13] + preF - ONX(Y[7], Y[6], LL(Y[5], 10));
			Q[8] = ONX(Y[7], Y[6], LL(Y[5], 10)) + word[13] + LL(Y[3], 10) + RC[0];
			Y[8] = LL(Y[4], 10) + LL(Q[8], RS[0][8]);

			for (k = 9; k < 13; k++){
				word[RI[0][k]] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]) - LL(Y[k - 5], 10) - ONX(Y[k - 1], Y[k - 2], LL(Y[k - 3], 10)) - RC[0];
				Q[k] = RR(Y[k] - LL(Y[k - 4], 10), RS[0][k]);
				Y[k] = LL(Y[k - 4], 10) + LL(Q[k], RS[0][k]);
			}

			if (checkQ(Q[11], 11, RS[0][11]) == false){
				cout << "Modify Y21, wrong Q11!" << endl;
				return false;
			}

			Q[21] = LL(Y[16], 10) + IFZ(Y[20], Y[19], LL(Y[18], 10)) + word[RI[1][5]] + RC[1];//
			Y[21] = LL(Y[17], 10) + LL(Q[21], RS[1][5]);

			if ((condition[i] <2 && getBit(Y[21], bitpos[i]) != condition[i]) || (condition[i] == 2 && getBit(Y[21], bitpos[i]) != getBit(Y[17], (bitpos[i] - 10 + 32) % 32)) || (condition[i] == 3 && getBit(Y[21], bitpos[i]) != getBit(Y[20], bitpos[i]))){
				cout << "Y21:" << bitpos[i] << endl;
				runningtimes[21]++;
				system("pause");
				return false;
			}
		}
	}
	if (checkY21() == false){
		runningtimes[21]++;
		return false;
	}
	if (checkQ(Q[21], 21, RS[1][5]) == false){
		cout << "Modify Y21, wrong Q21!" << endl;
		cout << "Y17':";
		toBinary(LL(Y[17], 10));
		cout << "Q21':";
		toBinary(LL(Q[21], RS[1][5]));
		cout << "Y21:";
		toBinary(Y[21]);
		cout << endl;
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkY21(){
	int bitpos[18] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 19, 20, 21, 23, 24, 25, 30, 31 };
	int condition[18] = { 0, 0, 2, 2, 2, 2, 2, 2, 1, 1, 3, 3, 1, 1, 0, 0, 1, 1 };

	for (int i = 0; i < 18; i++){
		if (condition[i] < 2){
			if (getBit(Y[21], bitpos[i]) != condition[i]){
				cout << "Y21(Out): "<<bitpos[i] << endl;
				return false;
			}
		}
		else if(condition[i]==2){
			if (getBit(Y[21], bitpos[i]) != getBit(Y[17], (bitpos[i] - 10+32)%32)){
				cout << "Y21(Out): "<<bitpos[i] << endl;
				return false;
			}
		}
		else if (condition[i] == 3){
			if (getBit(Y[21], bitpos[i]) != getBit(Y[20], bitpos[i])){
				cout << "Y21(Out): " << bitpos[i] << endl;
				return false;
			}
		}
	}
	return true;
}

bool RIPEMD160::modifyY22(){
	round2run[22]++;
	int bitpos[6] =   { 1, 8, 9, 10, 16, 31 };
	int condition[6] = { 0, 2, 1,  0,  0,  1 };

	int pos = 0;
	unsigned int x = 0;
	int k = 0;

	//cout << "Q[0]:";
	//toBinary(Q[0]);
	for (int i = 0; i < 6; i++){
		if ((bitpos[i] != 8 && getBit(Y[22], bitpos[i]) != condition[i]) || (bitpos[i] == 8 && checkY22_8() == false)){
			pos = (bitpos[i] - 9 + 32) % 32;
			//cout << "Y22," << dec << bitpos[i] << endl;
			if (bitpos[i] == 1){//We can't modify Y22,1
				//cout << "Y22,1 is wrong!" << endl << endl;
				return false;
			}

			/*////////
			for (int t = 0; t < 6; t++){
				cout << hex << "Y" << t << ": ";
				toBinary(Y[t]);
				cout << hex << "Y" << t << ": " << Y[t] << endl;
			}
			cout << endl;
			////////*/
			//cout << "word0:";
			//toBinary(word[0]);

			/*if (bitpos[i] == 8){
				cout << "Q00':";
				toBinary(LL(Q[0], RS[0][0]));
				cout << "Y-4':";
				toBinary(LL(RIV[1], 10));
				cout << "Y0:";
				toBinary(Y[0]);
				system("pause");
			}*/

			if (LL(Q[0] + EXP[pos], RS[0][0]) != LL(Q[0], RS[0][0]) + EXP[(pos + RS[0][0]) % 32]){//Holds with high probability
				cout << "Modify Y22, Propagation:" << dec << bitpos[i] << endl << endl;

				cout << "Q00':";
				toBinary(LL(Q[0], RS[0][0]));
				cout << "Y-4':";
				toBinary(LL(RIV[1], 10));
				cout << "Y0:";
				toBinary(Y[0]);

				runningtimes[22]++;
				system("pause");
				return false;
			}

			k = 0;
			word[RI[0][k]] = word[RI[0][k]] + EXP[pos];
			Q[k] = Q[k] + EXP[pos];
			Y[k] = LL(RIV[1], 10) + LL(Q[k], RS[0][k]);

			word[RI[0][1]] = RR(Y[1] - LL(RIV[2], 10), RS[0][1]) - LL(RIV[1], 10) - ONX(Y[0], RIV[4], LL(RIV[3], 10)) - RC[0];//Y[1] stay the same value
			Q[1] = LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + RC[0] + word[RI[0][1]];
			//Y[2] stay the same value
			//word[RI[0][3]] = RR(Y[3] - LL(RIV[4], 10), RS[0][3]) - (LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + RC[0]);//Y[3] stay the same value
			//Q[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + RC[0] + word[RI[0][3]];
			//Y[3] stay the same value
			word[RI[0][4]] = RR(Y[4] - LL(Y[0], 10), RS[0][4]) - (LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + RC[0]);//Y[4] stay the same value
			Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + RC[0] + word[9];//Q[4] should be updated.

			word[RI[0][5]] = RR(Y[5] - LL(Y[1], 10), RS[0][5]) - (LL(Y[0], 10) + ONX(Y[4], Y[3], LL(Y[2], 10)) + RC[0]);//Y[5] stay the same value
			Q[5] = LL(Y[0], 10) + ONX(Y[4], Y[3], LL(Y[2], 10)) + RC[0] + word[RI[0][5]];

			//cout << "word0:";
			//toBinary(word[0]);

			/*///////
			for (int t = 0; t < 6; t++){
				if (t < 5){
					switch (t){
					case 0:
						Q[0] = LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0], RS[0][0];
						Y[0] = LL(RIV[1], 10) + LL(Q[0], RS[0][0]);
						break;
					case 1:
						Q[1] = LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0], RS[0][1];
						Y[1] = LL(RIV[2], 10) + LL(Q[1], RS[0][1]);
						break;
					case 2:
						Q[2] = LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0], RS[0][2];
						Y[2] = LL(RIV[3], 10) + LL(Q[2], RS[0][2]);
						break;
					case 3:
						Q[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0], RS[0][3];
						Y[3] = LL(RIV[4], 10) + LL(Q[3], RS[0][3]);
						break;
					case 4:
						Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0], RS[0][4];
						Y[4] = LL(Y[0], 10) + LL(Q[4], RS[0][4]);
						break;
					}
				}
				else{
					Q[t] = LL(Y[t - 5], 10) + ONX(Y[t - 1], Y[t - 2], LL(Y[t - 3], 10)) + word[RI[0][t]] + RC[0];
					Y[t] = LL(Y[t - 4], 10) + LL(Q[t], RS[0][t]);
				}
				cout << hex << "Y" << t << ": ";
				toBinary(Y[t]);
				cout << hex << "Y" << t << ": " << Y[t] << endl;
			}
			cout << endl;
			system("pause");
			///////*/

			Q[22] = LL(Y[17], 10) + IFZ(Y[21], Y[20], LL(Y[19], 10)) + word[RI[1][6]] + RC[1];//
			Y[22] = LL(Y[18], 10) + LL(Q[22], RS[1][6]);

			if ((bitpos[i] != 8 && getBit(Y[22], bitpos[i]) != condition[i]) || (bitpos[i] == 8 && checkY22_8() == false)){
				cout << "Modify Y22 (In):" << dec << bitpos[i] << endl;
				return false;
			}
		}
	}

	if (checkY22() == false){
		runningtimes[22]++;
		return false;
	}

	if (checkQ(Q[22], 22, RS[1][6]) == false){
		cout << "WRONG Q22!" << endl;
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkY22_8(){
	//cout << "Y22:";
	//toBinary(Y[22]);
	//cout << "Y18:";
	//toBinary(LL(Y[18], 10));
	unsigned int lowY22_8 = Y[22] & 0xff;
	unsigned int lowY18_8 = LL(Y[18], 10) & 0xff;
	unsigned int Y22_8 = getBit(Y[22], 8);
	unsigned int Y18_30 = getBit(Y[18], 30);
	if (lowY22_8 >= lowY18_8 && Y22_8 != Y18_30){
		return true;
	}
	else if (lowY22_8 < lowY18_8 && Y22_8 == Y18_30){
		return true;
	}
	return false;
}

inline bool RIPEMD160::checkY22(){
	int bitpos[5] = { 1,  9, 10, 16, 31 };
	int condition[6] = { 0, 1, 0, 0, 1 };

	for (int i = 0; i < 5; i++){
		if (getBit(Y[22], bitpos[i]) != condition[i]){
			cout << "Y22(out):" << bitpos[i] << endl;
			return false;
		}
	}

	return true;
}

bool RIPEMD160::modifyRound2(){
	for (int i = 16; i < 23; i++){
		Q[i] = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
		Y[i] = LL(Y[i - 4], 10) + LL(Q[i], RS[1][i - 16]);
		if (i == 16){
			if (modifyY16()==false){
				return false;
			}
		}
		else if (i == 17){
			//cout << "Modify Y17:" << endl;
			if (modifyY17() == false){
				return false;
			}
		}
		else if (i == 18){
			//cout << "Modify Y18:" << endl;
			if (modifyY18() == false){
				return false;
			}
		}
		else if (i == 19){
			//cout << "Modify Y19:" << endl;
			if (modifyY19() == false){
				return false;
			}
		}
		else if (i == 20){
			//cout << "Modify Y20:" << endl;
			if (modifyY20() == false){
				return false;
			}
		}
		else if (i == 21){
			//cout << "Modify Y21:" << endl;
			if (modifyY21() == false){
				return false;
			}
		}
		else if (i == 22){
			//cout << "Modify Y21:" << endl;
			if (modifyY22() == false){
				return false;
			}
		}
	}
	return true;
}

void RIPEMD160::start(int testTimes){
	bool flag = false;
	srand(time(NULL));

	unsigned int timeCount[32][10];
	for (int i = 0; i < 32; i++){
		for (int j = 0; j < 10;j++)
			timeCount[i][j] = 0;
	}

	double m[10] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
	int max = 0;
	int min = 0x7fffffff;
	double total = 0;

	for (int count = 0; count < testTimes; count++){//test for 100 times.
		flag = false;

		for (int k = 0; k < 32; k++){
			runningtimes[k] = 0;
			round2run[k] = 0;
		}

		while (flag == false){
			for (int i = 0; i < 16; i++){
				word[i] = getRand();
			}
			flag = modifyRound1();

			if (flag == true){
				if (modifyRound2() == true){
					flag = true;
				}
				else{
					flag = false;
				}
			}
		}
		/*for (int i = 0; i < 16; i++){
			cout << hex << word[i] << endl;
		}*/

		int end = 23;
		unsigned int T1[32], T2[32];//储存
		cout << endl << "Check times:" << endl;

		for (int i = 0; i < end; i++){
			cout << i << ":" << hex << runningtimes[i] << endl;
		}

		cout << endl << "Round two running times:" << endl;
		for (int i = 16; i < end; i++){
			cout << i << ":" << hex << round2run[i] << endl;
		}

		/*Write the result to the file*/
		/////
		cout << endl << "Result:" << endl;
		int index=0;
		for (int i = 16; i < end; i++){
			cout << dec << "Y" << i << ":\t";
			cout << hex << runningtimes[i] << "\t" << round2run[i] << "\t";

			double runningtimeIndex = 31, round2runIndex = 31;
			runningtimeIndex = getRange(runningtimes[i]);
			round2runIndex = getRange(round2run[i]);

			if (runningtimes[i] != 0){
				cout << "Probability of being wrong corrected: 2^{-" << dec << round2runIndex - runningtimeIndex << "}\t";
			}
			else{
				cout << "Probability of being wrong corrected: 0\t";
			}

			if (i == 16){
				index = round2runIndex * 10;
				timeCount[index/10][index%10]++;
				if (index / 10 > max){
					max = index / 10;
				}
				if (index / 10 < min){
					min = index / 10;
				}
				total = round2runIndex;
			}
			cout << endl;
		}
		cout << "Total running times: 2^{" << total << "}";
		//outFiles << "Probability: 2^{-" << total << "}" << endl;;
		/////

		computeHash(0, end);
		cout << endl;

		for (int i = 0; i < end; i++){
			T1[i] = Y[i];
		}

		word[15] = word[15] + EXP[24];
		computeHash(0, end);

		for (int i = 0; i < end; i++){
			T2[i] = Y[i];
		}
		cout << endl;

		unsigned int in = 0, out = 0, t;
		bool ff = true;
		//求差分Q[10,11,12,13,14,15][in,out]
		for (int i = 10; i < end; i++){
			if (i < 16){
				t = LL(T1[i - 5], 10) + ONX(T1[i - 1], T1[i - 2], LL(T1[i - 3], 10)) + word[RI[0][i]] + RC[0];
				in = LL(T2[i - 5], 10) + ONX(T2[i - 1], T2[i - 2], LL(T2[i - 3], 10)) - ONX(T1[i - 1], T1[i - 2], LL(T1[i - 3], 10)) - LL(T1[i - 5], 10);
				if (i == 10){
					in += EXP[24];
				}
				out = LL(t + in, RS[0][i]) - LL(t, RS[0][i]);
			}
			else if (i >= 16 && i < 32){
				t = LL(T1[i - 5], 10) + IFZ(T1[i - 1], T1[i - 2], LL(T1[i - 3], 10)) + word[RI[0][i]] + RC[1];
				in = LL(T2[i - 5], 10) + IFZ(T2[i - 1], T2[i - 2], LL(T2[i - 3], 10)) - IFZ(T1[i - 1], T1[i - 2], LL(T1[i - 3], 10)) - LL(T1[i - 5], 10);
				out = LL(t + in, RS[1][i - 16]) - LL(t, RS[1][i - 16]);
			}

			//out = T2[i] - LL(T2[i - 4], 10) - (T1[i] - LL(T1[i - 4], 10));

			cout << "Q" << i << endl;
			cout << hex << "out:" << out << endl;
			cout << hex << "in:" << in << endl;
			out = (T2[i] - T1[i]) - LL(T2[i - 4], 10) + LL(T1[i - 4], 10);
			cout << hex << "out:" << out << endl;
			cout << endl;

			if (in != this->in[i] || out != this->out[i]){
				cout << "WRONG IN OUT!" << endl;
				system("pause");
			}
		}
	}

	ofstream outFiles("modify.txt", ios::app);
	outFiles << "----------------------------------------" << endl;

	outFiles << endl;
	outFiles << "We run our program for " << testTimes << " times, and count the running times when all conditions are corrected!" << endl;
	outFiles << endl;
	outFiles << "Running times: count" << endl;

	cout << endl;
	cout << "We run our program for " << testTimes << " times, and count the running times when all conditions are corrected!" << endl;
	cout << "Running times: count"<<endl;

	int count = 0;
	for (int i = min; i <=max; i++){
		count = 0;
		for (int j = 0; j < 10; j++){
			if (j != 0 && j % 5 == 0){
				cout << endl;
				outFiles << endl;
			}
			count += timeCount[i][j];
			if (j == 0){
				cout << dec << "2^{" << i << ".0}: " << setw(3)<<timeCount[i][j] << "  ";
				outFiles << dec << "2^{" << i << ".0}: " << setw(3) << timeCount[i][j] << "  ";
			}
			else{
				cout << dec << "2^{" << i + m[j] << "}: " << setw(3) << timeCount[i][j] << "  ";
				outFiles << dec << "2^{" << i + m[j] << "}: " << setw(3) << timeCount[i][j] << "  ";
			}
		}
		cout << endl << dec << "  Total: " << count << endl << endl;
		outFiles << dec << "  Total: " << count << endl << endl;
	}
	outFiles.close();
}

void RIPEMD160::startTestRightBranch(int testTimes){
	unsigned int mask[5] = {0x3f631ffa,0xbfdff072,0xbe1f043f,0x3c5ffcfc,0x7ffef9fd};
	unsigned int OR[5] = { 0xc0904005, 0x0020038c, 0x4160fac0, 0xc0a00300,0x80000200 };
	bool flag = false;
	double index = 31;
	int integer = 0;
	int max = 0;
	int min = 0x7fffffff;
	double m[10] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };

	srand(time(NULL));
	unsigned long long times = 0;
	
	unsigned int timeCount[32][10];
	for (int i = 0; i < 32; i++){
		for (int j = 0; j < 10; j++)
			timeCount[i][j] = 0;
	}

	for (int k = 0; k < testTimes; k++){
		cout << dec << k << endl;

		flag = false;
		times = 0;
		while (flag == false){
			times++;
			for (int i = 0; i < 16; i++){
				word[i] = getRand();
			}

			//randomly generate Y[18],Y[19],Y[20],Y[21],Y[22]
			for (int i = 0; i < 5; i++){
				Y[i + 18] = getRand();
				Y[i + 18] = Y[i + 18] & mask[i];
				Y[i + 18] = Y[i + 18] | OR[i];
			}

			if (getBit(Y[19], 24) != getBit(Y[18], 24)){
				Y[19] = Y[19] ^ EXP[24];
			}

			if (getBit(Y[21], 19) != getBit(Y[20], 19)){
				Y[21] = Y[21] ^ EXP[19];
			}
			if (getBit(Y[21], 20) != getBit(Y[20], 20)){
				Y[21] = Y[21] ^ EXP[20];
			}

			if (testRightBranch() == true){
				flag = true;
			}
		}

		index = getRange(times);
		integer = index * 10;
		if (integer / 10 > max){
			max = integer / 10;
		}
		if (integer / 10 < min){
			min = integer / 10;
		}
		timeCount[integer/10][integer%10]++;
		cout << dec << "Index:"<<index << endl;
		cout << dec << "timeCount[" << integer / 10 << "," << integer % 10<<"]: " << timeCount[integer / 10][integer % 10] << endl;
		//outFile << hex << "Running times(HEX): "<<setw(8)<<times << ".\t";
		//outFile << dec << "Therefore, the probability is about 2^{-" << getRange(times) << "}." << endl;
	}

	ofstream outFile("rightRunningTimes.txt", ios::app);
	outFile << "----------------------------------------" << endl;

	outFile << endl;
	outFile << "We run our program for " << testTimes << " times, and count the running times when all conditions hold!" << endl;
	outFile << endl;
	outFile << "Running times: count" << endl;

	cout << endl;
	cout << "We run our program for " << testTimes << " times, and count the running times when all conditions hold!" << endl;
	cout << "Running times: count" << endl;

	int count = 0;
	for (int i = min; i <= max; i++){
		count = 0;
		for (int j = 0; j < 10; j++){
			if (j != 0 && j % 5 == 0){
				cout << endl;
				outFile << endl;
			}
			count += timeCount[i][j];
			if (j == 0){
				cout << dec << "2^{" << i << ".0}: " << setw(3) << timeCount[i][j] << "  ";
				outFile << dec << "2^{" << i << ".0}: " << setw(3) << timeCount[i][j] << "  ";
			}
			else{
				cout << dec << "2^{" << i + m[j] << "}: " << setw(3) << timeCount[i][j] << "  ";
				outFile << dec << "2^{" << i + m[j] << "}: " << setw(3) << timeCount[i][j] << "  ";
			}
		}
		cout << endl << dec << "  Total: " << count << endl << endl;
		outFile << dec << "  Total: " << count << endl << endl;
	}
	outFile.close();
}

bool RIPEMD160::testRightBranch(){
	int i = 23;
	for (i = 23; i < 30; i++){
		Q[i] = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
		Y[i] = LL(Y[i - 4], 10) + LL(Q[i], RS[1][i - 16]);
	
		if (i == 23){
			if (checkY23() == false){
				return false;
			}
		}

		else if (i == 24){
			if (checkY24() == false){
				return false;
			}
		}

		else if (i == 25){
			if (checkY25() == false){
				return false;
			}
		}

		else if (i == 26){
			if (checkY26() == false){
				return false;
			}
		}

		else if (i == 27){
			if (checkY27() == false){
				return false;
			}
		}

		if (checkQ(Q[i], i, RS[1][i - 16]) == false){

			/*cout << hex << "Q[i]: ";
			toBinary(LL(Q[i],RS[1][i-16]));

			cout << hex << "Y[i-4]: ";
			toBinary(LL(Y[i-4], 10));

			cout << hex << "Y[i]: " ;
			toBinary(Y[i]);*/

			//cout << dec << i << endl;
			//system("pause");
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkY23(){
	int bitpos[6] = { 11, 1, 0, 31, 16, 9, };
	int condition[6] = { 0, 0, 0, 1, 1,1 };

	for (int i = 0; i < 6; i++){
		if (getBit(Y[23], bitpos[i]) != condition[i]){
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkY24(){
	int bitpos[6] = { 26, 20, 1, 31, 11, 0, };
	int condition[6] = { 0, 0, 0, 1, 1, 1 };

	for (int i = 0; i < 6; i++){
		if (getBit(Y[24], bitpos[i]) != condition[i]){
				return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkY25(){
	int bitpos[4] = { 20, 10, 9, 11, };
	int condition[4] = { 0, 0, 0, 1 };

	for (int i = 0; i < 4; i++){
		if (getBit(Y[25], bitpos[i]) != condition[i]){
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkY26(){
	if (getBit(Y[26], 30) != 1){
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkY27(){
	int bitpos[3] = { 19, 20, 21 };

	for (int i = 0; i < 3; i++){
		if (getBit(Y[27], bitpos[i]) != getBit(Y[26], bitpos[i])){
			return false;
		}
	}
	return true;
}

void RIPEMD160::startTestLeftBranch(int testTimes){
	bool flag = false;
	srand(time(NULL));
	unsigned int times = 0;

	double index = 31;
	int integer = 0;
	int max = 0;
	int min = 0x7fffffff;
	double m[10] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };

	unsigned int timeCount[32][10];
	for (int i = 0; i < 32; i++){
		for (int j = 0; j < 10; j++)
			timeCount[i][j] = 0;
	}

	for (int k = 0; k < testTimes; k++){
		flag = false;
		times = 0;
		cout << dec << k << endl;
		while (flag == false){
			times++;
			for (int i = 0; i < 16; i++){
				word[i] = getRand();
			}
			if (testLeftBranchUntilX26() == true){
				flag = true;
			}
		}

		index = getRange(times);
		integer = index * 10;
		if (integer / 10 > max){
			max = integer / 10;
		}
		if (integer / 10 < min){
			min = integer / 10;
		}
		timeCount[integer / 10][integer % 10]++;
		cout << dec << "Index:" << index << endl;
		cout << dec << "timeCount[" << integer / 10 << "," << integer % 10 << "]: " << timeCount[integer / 10][integer % 10] << endl;
	}

	ofstream outFile("leftRunningTimes.txt", ios::app);
	outFile << "----------------------------------------" << endl;

	outFile << endl;
	outFile << "We run our program for " << testTimes << " times, and count the running times when all conditions on X15--X26 hold!" << endl;
	outFile << endl;
	outFile << "Running times: count" << endl;

	cout << endl;
	cout << "We run our program for " << testTimes << " times, and count the running times when all conditions on X15--X26 hold!" << endl;
	cout << "Running times: count" << endl;

	int count = 0;
	for (int i = min; i <= max; i++){
		count = 0;
		for (int j = 0; j < 10; j++){
			if (j != 0 && j % 5 == 0){
				cout << endl;
				outFile << endl;
			}
			count += timeCount[i][j];
			if (j == 0){
				cout << dec << "2^{" << i << ".0}: " << setw(3) << timeCount[i][j] << "  ";
				outFile << dec << "2^{" << i << ".0}: " << setw(3) << timeCount[i][j] << "  ";
			}
			else{
				cout << dec << "2^{" << i + m[j] << "}: " << setw(3) << timeCount[i][j] << "  ";
				outFile << dec << "2^{" << i + m[j] << "}: " << setw(3) << timeCount[i][j] << "  ";
			}
		}
		cout << endl << dec << "  Total: " << count << endl << endl;
		outFile << dec << "  Total: " << count << endl << endl;
	}
	outFile.close();
}

bool RIPEMD160::testLeftBranch(){
	for (int i = 0; i < 30; i++){
		if (i < 5){
			switch (i){
			case 0:
				X[0] = LL(LIV[1], 10) + LL(LL(LIV[0], 10) + XOR(LIV[4], LIV[3], LL(LIV[2], 10)) + word[LI[0][0]] + LC[0], LS[0][0]);
				break;
			case 1:
				X[1] = LL(LIV[2], 10) + LL(LL(LIV[1], 10) + XOR(X[0], LIV[4], LL(LIV[3], 10)) + word[LI[0][1]] + LC[0], LS[0][1]);
				break;
			case 2:
				X[2] = LL(LIV[3], 10) + LL(LL(LIV[2], 10) + XOR(X[1], X[0], LL(LIV[4], 10)) + word[LI[0][2]] + LC[0], LS[0][2]);
				break;
			case 3:
				X[3] = LL(LIV[4], 10) + LL(LL(LIV[3], 10) + XOR(X[2], X[1], LL(X[0], 10)) + word[LI[0][3]] + LC[0], LS[0][3]);
				break;
			case 4:
				X[4] = LL(X[0], 10) + LL(LL(LIV[4], 10) + XOR(X[3], X[2], LL(X[1], 10)) + word[LI[0][4]] + LC[0], LS[0][4]);
				break;
			}
		}
		else if (i < 16){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + XOR(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[0][i]] + LC[0], LS[0][i]);
			if (i == 14 && checkX(14) == false){
				return false;
			}
			if (i == 15 && checkX(15) == false){
				return false;
			}
		}
			
		else if (i < 29){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);
			if (checkX(i) == false){
				return false;
			}
		}
	}
	return true;
}

/*We divide the test for the left branch into two phases.
The first phase is to test the probability that X0-X26 hold.
The second phase is to test the probability tha X27-X29 hold, which is about 2^{-4}
 since there are four bit conditions on them.*/
bool RIPEMD160::testLeftBranchUntilX26(){
	for (int i = 0; i < 27; i++){
		if (i < 5){
			switch (i){
			case 0:
				X[0] = LL(LIV[1], 10) + LL(LL(LIV[0], 10) + XOR(LIV[4], LIV[3], LL(LIV[2], 10)) + word[LI[0][0]] + LC[0], LS[0][0]);
				break;
			case 1:
				X[1] = LL(LIV[2], 10) + LL(LL(LIV[1], 10) + XOR(X[0], LIV[4], LL(LIV[3], 10)) + word[LI[0][1]] + LC[0], LS[0][1]);
				break;
			case 2:
				X[2] = LL(LIV[3], 10) + LL(LL(LIV[2], 10) + XOR(X[1], X[0], LL(LIV[4], 10)) + word[LI[0][2]] + LC[0], LS[0][2]);
				break;
			case 3:
				X[3] = LL(LIV[4], 10) + LL(LL(LIV[3], 10) + XOR(X[2], X[1], LL(X[0], 10)) + word[LI[0][3]] + LC[0], LS[0][3]);
				break;
			case 4:
				X[4] = LL(X[0], 10) + LL(LL(LIV[4], 10) + XOR(X[3], X[2], LL(X[1], 10)) + word[LI[0][4]] + LC[0], LS[0][4]);
				break;
			}
		}
		else if (i < 16){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + XOR(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[0][i]] + LC[0], LS[0][i]);
			if (i == 14 && checkX(14) == false){
				return false;
			}
			else if (i == 15 && checkX(15) == false){
				return false;
			}
		}

		else if (i < 29){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);
			if (checkX(i) == false){
				return false;
			}
		}
	}
	return true;
}

inline bool RIPEMD160::checkX(int i){
	switch (i){
	case 14:
		if (checkX14() == false){
			return false;
		}
		break;
	case 15:
		if (checkX15() == false){
			return false;
		}
		break;
	case 16:
		if (checkX16() == false){
			return false;
		}
		break;
	case 17:
		if (checkX17() == false){
			return false;
		}
		break;
	case 18:
		if (checkX18() == false){
			return false;
		}
		break;
	case 19:
		if (checkX19() == false){
			return false;
		}
		break;
	case 20:
		if (checkX20() == false){
			return false;
		}
		break;
	case 21:
		if (checkX21() == false){
			return false;
		}
		break;
	case 22:
		if (checkX22() == false){
			return false;
		}
		break;
	case 23:
		if (checkX23() == false){
			return false;
		}
		break;
	case 24:
		if (checkX24() == false){
			return false;
		}
		break;
	case 25:
		if (checkX25() == false){
			return false;
		}
		break;
	case 26:
		if (checkX26() == false){
			return false;
		}
		break;
	case 27:
		if (checkX27() == false){
			return false;
		}
		break;
	case 28:
		if (checkX28() == false){
			return false;
		}
		break;
	}
	//cout << dec << i << endl;
	return true;
}

inline bool RIPEMD160::checkX14(){
	if (getBit(X[13], 22) != getBit(X[14], 0)){
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkX15(){
	if (getBit(X[15], 0) != 0){
		return false;
	}
	//check Q10
	unsigned int q = LL(LIV[0], 10) + XOR(LIV[4], LIV[3], LL(LIV[2], 10)) + word[LI[0][0]] + LC[0];
	unsigned int in = EXP[24];
	unsigned int out = 0x1;

	if (LL(q + in, LS[0][15]) != LL(q, LS[0][15]) + out){
		//cout << "q";
		//toBinary(q);
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkX16(){
	if (getBit(X[16], 0) != 0){
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkX17(){
	int bitpos[2] = { 10, 0 };
	int condition[2] = { 1,1 };

	for (int i = 0; i < 2; i++){
		if (getBit(X[17], bitpos[i]) != condition[i]){
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkX18(){
	if (getBit(X[18], 10) != 0){
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkX19(){
	if (getBit(X[19], 10) != 0){
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkX20(){
	if (getBit(X[20], 10) != 0){
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkX21(){
	int bitpos[2] = { 20,10 };
	int condition[2] = { 1, 1 };

	for (int i = 0; i < 2; i++){
		if (getBit(X[21], bitpos[i]) != condition[i]){
			return false;
		}
	}

	if (getBit(X[21], 31) != getBit(X[20], 21)){
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkX22(){
	int bitpos[2] = { 31, 20 };
	int condition[2] = { 0, 0 };

	for (int i = 0; i < 2; i++){
		if (getBit(X[22], bitpos[i]) != condition[i]){
			return false;
		}
	}

	//check Q10
	unsigned int q = LL(X[17], 10) + IFX(X[21], X[20], LL(X[19], 10)) + word[LI[1][6]] + LC[1];
	unsigned int in = EXP[24];
	unsigned int out = EXP[31];

	if (LL(q + in, LS[1][6]) != LL(q, LS[1][6]) + out){
		return false;
	}

	return true;
}

inline bool RIPEMD160::checkX23(){
	int bitpos[2] = { 31, 20 };
	int condition[2] = { 0, 0 };

	for (int i = 0; i < 2; i++){
		if (getBit(X[23], bitpos[i]) != condition[i]){
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkX24(){
	int bitpos[3] = { 20,31,9};
	int condition[3] = { 0, 1,1 };

	for (int i = 0; i < 3; i++){
		if (getBit(X[24], bitpos[i]) != condition[i]){
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkX25(){
	//cout << "25" << endl;
	int bitpos[3] = { 9,30,20 };
	int condition[3] = { 0, 1, 1 };

	for (int i = 0; i < 3; i++){
		if (getBit(X[25], bitpos[i]) != condition[i]){
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkX26(){
	//cout << "26" << endl;
	int bitpos[2] = { 30, 9};
	int condition[3] = { 0,0 };

	for (int i = 0; i < 2; i++){
		if (getBit(X[26], bitpos[i]) != condition[i]){
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkX27(){
	cout << "Now it is checking the conditions on X27!" << endl;
	int bitpos[2] = { 30, 9 };
	int condition[3] = { 0, 0 };

	for (int i = 0; i < 2; i++){
		if (getBit(X[27], bitpos[i]) != condition[i]){
			cout << dec << "Wrong! The condition that Y27," << bitpos[i] << " dosen't hold!" << endl << endl;
			return false;
		}
	}
	cout << "The conditions on X27 is satisfied!" << endl;
	return true;
}

inline bool RIPEMD160::checkX28(){
	cout << "28" << endl;
	int bitpos[2] = { 30, 19 };
	int condition[3] = { 0, 1 };

	for (int i = 0; i < 2; i++){
		if (getBit(X[28], bitpos[i]) != condition[i]){
			return false;
		}
	}
	//cout << "29" << endl;
	return true;
}
