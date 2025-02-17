//DTMF.cpp

#include "DTMF.h"
using namespace std;


DTMF::DTMF()
{

}
DTMF::~DTMF()
{

}
void DTMF::CalcSignalDFT(float *signalInputArray,float *signalReArray,float *signalImArray,int signalLength)
{
	int index1,index2,index3;
	for(index1 = 0;index1 < signalLength;index1++)
	{
		signalReArray[index1] = 0;
		signalImArray[index1] = 0;
	}
	 
	//透過傅立葉轉換將輸入訊號(時域)轉成實部和虛部(頻域)
	for(index2 = 0;index2 < (signalLength/2);index2++)
	{
		for(index3 = 0;index3 < signalLength;index3++)
		{
			signalReArray[index2] += signalInputArray[index3]*cos((2*PI*index2*index3)/signalLength);//實部��
			signalImArray[index2] -= signalInputArray[index3]*sin((2*PI*index2*index3)/signalLength);//虛部���� 
		}
	}
}
void DTMF::DTMF_Generator(char keypadInput,int signalLength,int rate,float *dataOutput)
{
	int index;
	float hFreq;
	float lFreq;
	
	//判斷輸入按鍵所對應的高頻及低頻��
	for(index = 0;index < 16;index++)
	{
		if(keypadInput == *keypadTone[index].sign)
		{
			hFreq = keypadTone[index].highFrequency;
			lFreq = keypadTone[index].lowFrequency;
			//cout << hFreq << "," << lFreq << endl;
		}
	}

	//Generator DTMF Signal
	for(index = 0;index < signalLength;index++)
	{
		//高頻加低頻
		dataOutput[index] = 0.5*sinf((2*PI*hFreq*index)/rate); 
		dataOutput[index] += (0.5*sinf((2*PI*lFreq*index)/rate));
	}
}
char* DTMF::DTMF_Detector(float *dtmfSignal,int signalLength,int rate)
{
	float arrayReSignal[rate];
	float arrayImSignal[rate];
	float detectionSignal[8];
	int freqPositon[8];
	FILE *reFile,*imFile;
	
	reFile = fopen("ReData.dat","w");
	imFile = fopen("ImData.dat","w");
	
	//設定按鍵對應的頻率解析度(頻率*訊號長度/採樣頻率)
	freqPositon[0] = (697*signalLength)/rate;
	freqPositon[1] = (770*signalLength)/rate;
	freqPositon[2] = (852*signalLength)/rate;
	freqPositon[3] = (941*signalLength)/rate;
	freqPositon[4] = (1209*signalLength)/rate;
	freqPositon[5] = (1336*signalLength)/rate;
	freqPositon[6] = (1477*signalLength)/rate;
	freqPositon[7] = (1633*signalLength)/rate;

	//進行離散傅立葉轉換，輸出實部及虛部
	CalcSignalDFT(dtmfSignal,arrayReSignal,arrayImSignal,signalLength);
	
	int index;
	for(index = 0;index < signalLength;index++)
	{
		fprintf(reFile,"%f\n",arrayReSignal[index]);
		fprintf(imFile,"%f\n",arrayImSignal[index]);
	}
	
	int row = 0,column = 0;

	for(index = 0;index < 8;index++)
	{
		//取絕對值(實部平方加虛部平方再開根號)
		detectionSignal[index] = 
			sqrt(arrayReSignal[freqPositon[index]]*arrayReSignal[freqPositon[index]] 
			+ arrayImSignal[freqPositon[index]]*arrayImSignal[freqPositon[index]]);
		//cout << detectionSignal[index]  << endl;
	}
	float tempMaxValue = 0;
	//找低頻最大能量位置
	for(index = 0;index < 4  ;index++)
	{
		if(detectionSignal[index] > tempMaxValue)
			if(detectionSignal[index] > 0.0005)
			{	
				tempMaxValue = detectionSignal[index];
				row = index;
			}
	}
	tempMaxValue = 0;
	//找高頻最大能量位置
	for(index = 4;index < 8;index++)
	{
		if(detectionSignal[index] > tempMaxValue)
			if(detectionSignal[index] > 0.0005)
			{
				tempMaxValue = detectionSignal[index];
				column = index;	
			}
	}
	//依高低頻能量最大位置找出所按的按鍵����
	if(row == 0) //697Hz
	{
		if(column == 4)	//1209Hz
			return "1";
		else if(column == 5) //1336Hz
			return "2";
		else if(column == 6) //1477Hz
			return "3";
		else if(column == 7) //1633Hz
			return "A";
	}
	else if(row == 1) //770Hz
	{
		if(column == 4)
			return "4";
		else if(column == 5)
			return "5";
		else if(column == 6)
			return "6";
		else if(column == 7)
			return "B";
	}
	else if(row == 2) //852Hz
	{
		if(column == 4)
			return "7";
		else if(column == 5)
			return "8";
		else if(column == 6)
			return "9";
		else if(column == 7)
			return "C";
	}
	else if(row == 3) //852Hz	
	{
		if(column == 4)
			return "*";
		else if(column == 5)
			return "0";
		else if(column == 6)
			return "#";
		else if(column == 7)
			return "D";
	}
}
