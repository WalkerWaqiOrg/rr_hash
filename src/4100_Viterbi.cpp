#include <iostream>
#include <string>
#include <list>
#include <stdint.h>
#include "util.h"
#include "Tracer.h"
using namespace std;

extern Tracer* TRACER;

struct Pair {
	uint32_t a;
	uint32_t b;
};

static void fillPairArray(uint8_t* seedIn, int seedSize, Pair arr[], int n) {
	uint32_t* rand1024=new uint32_t[1024]; //1024 32-b values 
	expandRand4KB(seedIn, seedSize, (uint8_t*)rand1024);
	int counter=0;
	for(int i=0; i<1024; i++) {
		for(int j=0; j<1024; j+=2) {
			if(j==i) continue;
			arr[counter].a=mulxor(rand1024[i],rand1024[j]);
			arr[counter].b=mulxor(rand1024[i],rand1024[j+1]);
			//printf("%d-%d: a:%08x b:%08x %08x %08x \n",i,j,
			//		arr[counter].a,arr[counter].b,
			//		rand1024[i],rand1024[j]);
			counter++;
			if(counter==n) {
				delete[] rand1024;
				return;
			}
		}
	}
	assert(false);
}
 

using namespace std;

void Viterbi(double pi[3], double C[3][3], double E[3][2]) {
    string output[4] = { "R", "W", "R","W" };
    int row = 3;
    int column = 3;
    //��������ռ�
    double **delta = new double *[row];
    int **path = new int *[row];
    int i, j,k;
    for (i = 0; i < 3; i++)
    {
        delta[i] = new double[3];
        path[i] = new int[3];
    }
    //�����״̬תΪ����
    int outnum[4];
    for (i = 0; i < row; i++)
    {
        if (output[i] == "R")
            outnum[i] = 0;
        else if (output[i] == "W")
            outnum[i] = 1;
    }

    //��ʼ��
    for (i = 0; i < column; i++)
    {
        path[i][0] = 0;
        delta[i][0] = pi[i] * E[i][0];
        //cout << delta[i][0] << endl;
    }

    //�ݹ�
    for (j = 1; j < row; j++)//���б����������б���
    {

        for (k = 0; k < column; k++)//״̬����
        {
            double pro = 0;
            int sta = 0;
            for (i = 0; i < column; i++)//�����б���
            {
                double temp = delta[i][j - 1] *C[i][k]* E[k][outnum[j]] ;//delta[i][j-1]*ת�Ƹ���*�������
                //cout << delta[i][j - 1] << " " << E[k][outnum[j]] << endl;
                //cout << temp << endl;
                if (temp > pro)
                {
                    pro = temp;
                    sta = i;//��¼·����Ϣ
                }
            }
            delta[k][j] = pro;
            path[k][j]= sta;
        }   
    }
    //��ֹ���ҵ��������ֵ
    double max = 0;
    int sta = 0;
    for (i = 0; i < column; i++)
    {
        if (delta[i][row - 1] > max)
        {
            max = delta[i][row - 1];
            sta = i;
        }
    }

    //���ݣ��ҵ����·�������Ӧ��״ֵ̬
    list<int> listPath;
    listPath.push_back(sta+1);
    for (j = row - 1; j > 0; j--)
    {
        sta = path[sta][j];
        listPath.push_back(sta+1);
    }

    //���
    //cout << "max probability: " << max << endl;
    list<int> ::iterator itepath;
	int s=0;
    for (itepath = listPath.begin(); itepath != listPath.end(); itepath++) {
		s=(s<<2)|*itepath;
        //cout << *itepath << " ";
	}
	TRACER->meet(int64_t(max)^s);
}

//int main(void)
//{
//    double pi[3] = { 1.2, 2.4, 4.4 };
//    double C[3][3] = { 0.5, 0.2, 5.3, 0.3, 1.5, 0.2, 1.2, 0.3, 0.5 };
//    double E[3][2] = { 0.5, 1.5, 1.4, 0.6, 1.7, 0.3 };
//	Viterbi(pi, C, E);
//	return 0;
//}

void run_Viterbi(uint8_t* seedIn, int seedSize) {
	const int Count=160*1024;
	Pair* tmp=new Pair[Count/2];
	fillPairArray(seedIn, seedSize, tmp, Count/2);
	int* arr=(int*)tmp;
    double pi[3];
    double C[3][3];
    double E[3][2];
	for(int i=0; i<Count; i+=18) {
		pi[0]=arr[i+0]; pi[1]=arr[i+1]; pi[2]=arr[i+2];
		C[0][0]=arr[i+3]; C[0][1]=arr[i+4]; C[0][2]=arr[i+5];
		C[1][0]=arr[i+6]; C[1][1]=arr[i+7]; C[1][2]=arr[i+8];
		C[2][0]=arr[i+9]; C[2][1]=arr[i+10]; C[2][2]=arr[i+11];
		E[0][0]=arr[i+12]; E[0][1]=arr[i+13];
		E[1][0]=arr[i+14]; E[1][1]=arr[i+15];
		E[2][0]=arr[i+16]; E[2][1]=arr[i+17];
		Viterbi(pi, C, E);
	}
	delete[] tmp;
}

#ifdef SELF_TEST
Tracer* TRACER;
int main() {
	TRACER=new Tracer;
	char hello[100]="aer39invqbj43to;5j46354q34534999!@#%@#$%^&$&ADGSGWREF";
	int len=strlen(hello);
	for(int i=0; i<50; i++) {
		run_Viterbi((uint8_t*)hello,len);
	}
	delete TRACER;
	return 0;
}
#endif

