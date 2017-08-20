#ifndef _RIPEMD160_H
#define _RIPEMD160_H
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <cstdio>
#include <cmath>
using namespace std;

#define LENGTH 32
#define WORD_NUM 16
#define VALIDNUM 0xff00000
const long long M = 1024;
const long long MOD = M*M*M * 4;

const unsigned int EXP[32] = {  0x1, 0x2, 0x4, 0x8,
							0x10, 0x20, 0x40, 0x80,
							0x100, 0x200, 0x400, 0x800,
							0x1000, 0x2000, 0x4000, 0x8000,
							0x10000, 0x20000, 0x40000, 0x80000,
							0x100000, 0x200000, 0x400000, 0x800000,
							0x1000000, 0x2000000, 0x4000000, 0x8000000,
							0x10000000, 0x20000000, 0x40000000, 0x80000000 };

class RIPEMD160{
private:
	unsigned int word[WORD_NUM];//message word
	unsigned int preWord[WORD_NUM];
	unsigned int LIV[5];//IV in the left branch
	unsigned int RIV[5];//IV in the right branch
	unsigned int X[36];//Internal state in the left branch
	unsigned int Y[36];//Internal state in the right branch
	unsigned int Q[36];
	unsigned int LDiff[36];//The difference of the internal state in the left branch
	unsigned int RDiff[36];//The difference of the internal state in the right branch
	unsigned int LC[5];//constants in the left branch
	unsigned int RC[5];//constants values in the right branch
	unsigned char LS[5][16];//rotation values in the left branch
	unsigned char RS[5][16];//rotation values in the right branch
	unsigned char LI[5][16];//word permutation in the left branch
	unsigned char RI[5][16];//word permutation in the right branch

	unsigned int in[32];
	unsigned int out[32];

	int runningtimes[32];
	int round2run[32];

	unsigned int accurateEXP[32][10];
public:
	RIPEMD160();
	void initialize();

	void generateAccurateEXP();
	double getRange(unsigned int number);

	void computeHash(int start, int steps);
	inline unsigned int getRand();

	void getModification(unsigned int &number, int *pos, int length, int condition);
	unsigned int getMultiModification(unsigned int number1, unsigned int number2, int *pos1, int *pos2, int length, int condition);
	
	inline bool checkQ(unsigned int q, int index,int shift);

	void start(int testTimes);
	bool modifyRound1();
	inline void modifyY0();
	inline void modifyY1();
	inline void modifyY2();
	inline void modifyY3();//extra conditions
	inline void modifyY4();//extra conditions
	inline void modifyY5();//extra conditions
	inline void modifyY6();//extra conditions
	inline void modifyY7();//extra conditions
	inline void modifyY8();
	inline void modifyY9();
	inline void modifyY10();
	inline void modifyY11();
	inline void modifyY12();
	inline void modifyY13();
	inline void modifyY14();
	inline void modifyY15();

	bool modifyRound2();
	bool modifyY16();
	bool modifyY17();
	bool modifyY18();
	bool modifyY19();
	bool modifyY20();
	bool modifyY21();
	bool modifyY22();

	inline bool checkY16();
	inline bool checkY17();
	inline bool checkY18();
	inline bool checkY19();
	inline bool checkY20();
	inline bool checkY21();
	inline bool checkY22();
	inline bool checkY22_8();

	void startTestLeftBranch(int testTimes);
	bool testLeftBranch();
	bool testLeftBranchUntilX26();
	inline bool checkX(int i);
	inline bool checkX14();
	inline bool checkX15();
	inline bool checkX16();
	inline bool checkX17();
	inline bool checkX18();
	inline bool checkX19();
	inline bool checkX20();
	inline bool checkX21();
	inline bool checkX22();
	inline bool checkX23();
	inline bool checkX24();
	inline bool checkX25();
	inline bool checkX26();
	inline bool checkX27();
	inline bool checkX28();

	void startTestRightBranch(int testTimes);
	bool testRightBranch();
	void getMask(int *pos, int *condition, int length, unsigned int &and, unsigned int &mask);
	//inline bool checkY22();
	inline bool checkY23();
	inline bool checkY24();
	inline bool checkY25();
	inline bool checkY26();
	inline bool checkY27();

	void generate();//generate a good Y-1.

	unsigned int getRandWord(unsigned int times);
};
#endif
