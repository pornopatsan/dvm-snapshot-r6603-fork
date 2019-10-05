#include <assert.h>
#include "DArray.h"
//#include "ModelStructs.h" 
#include <iostream>
using namespace std;

extern ofstream prot; 
 
extern bool SynchCopy; //====//
DArray::DArray(const vector<long>& ASizeArray, const vector<long>& ALowShdWidthArray,
		const vector<long>& AHiShdWidthArray, int ATypeSize) : 

		Space(ASizeArray),
		LowShdWidthArray(ALowShdWidthArray),
		HiShdWidthArray(AHiShdWidthArray),
		TypeSize(abs(ATypeSize)), //====//=*** - is needed in case of negative TypeSize - when it is fixed in trace file, then this 'abs' no need here
		AM_Dis(0),
		Repl(0),
		AlignRule(vector<AlignAxis>(0))
{
}

DArray::~DArray()
{
}

void DArray::PrepareAlign(long& TempRank, const vector<long>& AAxisArray, 
		const vector<long>&  ACoeffArray, const vector<long>& AConstArray, 
		vector<AlignAxis>& IniRule)
{
	long ArrRank, ARSize;
	int i;

	ArrRank = Rank();
	ARSize = ArrRank + TempRank;

	IniRule.reserve(ARSize);

	// Предварительная инициализация
	for (i = 0; i < ArrRank; i++)
		IniRule.push_back(AlignAxis(align_COLLAPSE, i+1, 0));
	for (i = ArrRank; i < ARSize; i++)
		IniRule.push_back(AlignAxis(align_NORMTAXIS, 0, i-ArrRank+1));

	// Заполнение DistRule в соответствии с параметрами
	for (i = 0; i < TempRank; i++) {
		if (AAxisArray[i] < 0)
			IniRule[i+ArrRank] = AlignAxis(align_REPLICATE, 0, i+1);
		else if (ACoeffArray[i] == 0 || AAxisArray[i] == 0) 
			// in new version last condition not require
			IniRule[i+ArrRank] = AlignAxis(align_CONSTANT, 0, i+1, 0, AConstArray[i]);
		else {
			IniRule[i+ArrRank] = AlignAxis(align_NORMTAXIS, AAxisArray[i], i+1);
			IniRule[AAxisArray[i]-1] = AlignAxis(align_NORMAL, AAxisArray[i], i+1, 
														ACoeffArray[i], AConstArray[i]);
		}
	}
}

void DArray::AlnDA(AMView *APattern,  const vector<long>& AAxisArray, 
		const vector<long>& ACoeffArray, const vector<long>& AConstArray)
{
	long i;
	int dir;

	vector<AlignAxis> IniRule;

	long TempRank = APattern->Rank();

	AM_Dis = APattern;
	APattern->AddDA(this);

	 // Оформление признака полностью размноженного массива 
	//for (i = 0; i < TempRank; i++)
	//	if (AAxisArray[i] != -1) 
	//		break;
	//if(i == TempRank)
	//	Repl = 1;    // размножение по всем измерениям 
	//else
	//	Repl = 0;

	// инициализация AlignRule
	PrepareAlign(TempRank, AAxisArray, ACoeffArray, AConstArray, IniRule);
	AlignRule = IniRule;

	// Оформление признака полностью размноженного массива 
	Repl = 1;
	for (i = 0; i < TempRank; i++) {
		switch (AlignRule[i + Rank()].Attr) {
			case align_CONSTANT :
				if (AM_Dis->DistRule[AlignRule[i + Rank()].TAxis - 1].Attr == map_BLOCK)
					Repl = 0;
				break;
			case align_NORMTAXIS:
				if (GetMapDim(AlignRule[i + Rank()].Axis, dir) > 0)
					Repl = 0;
				break;
		}

		if (!Repl)
			break;
	}

	#ifdef _TIME_TRACE_
	// Не нужно
	vector<AlignAxis>::iterator first = AlignRule.begin(), last = AlignRule.end();
	while(first != last) {
		prot << endl << first->Attr << " " << first->Axis << " " << first->TAxis << " " << first->A << " " << first->B << " " << first->Bound << endl;
		++first;
	}
	#endif
}

void DArray::AlnDA(DArray *APattern,  const vector<long>& AAxisArray, 
		const vector<long>& ACoeffArray, const vector<long>& AConstArray)
{
	int i;

	if (!APattern->IsAlign()) {
		prot << "Wrong call AlnDA" << endl;
		exit(1);
	}

	long				TempRank, 
						ArrRank, 
						ARSize;
	AMView	*			TempAMV;
	vector<AlignAxis>	TAlign, 
						IniRule;
	AlignAxis			aAl, 
						tAl;

	TAlign		= APattern->AlignRule;
	TempRank	= APattern->Rank();
	TempAMV		= APattern->AM_Dis;

	AM_Dis		= TempAMV;
	TempAMV->AddDA(this);

	// Оформление признака полностью размноженного массива 
	Repl = APattern->Repl; // наследование у шаблона 

	// начальная инициализация AlignRule
	PrepareAlign(TempRank, AAxisArray, ACoeffArray, AConstArray, IniRule); 

	// Формирование суперпозиции отображений
	ArrRank = Rank();
	ARSize = ArrRank + TempAMV->Rank(); 
	// Rank массива + Rank AMView куда массив в итоге выравнен(т.е. AM_Dis)
	AlignRule = vector<AlignAxis>(ARSize);
	AlignRule = IniRule;

	for (i = 0; i < ArrRank; i++)
	{
		aAl = IniRule[i];
		if (aAl.Attr  == align_NORMAL)
		{
			tAl = TAlign[aAl.TAxis - 1];
			switch (tAl.Attr)
			{
			  case align_NORMAL :   aAl.TAxis = tAl.TAxis;
                                    aAl.A *= tAl.A;
                                    aAl.B = aAl.B * tAl.A + tAl.B;
			// IniRule[i] = AlignAxis(align_NORMAL, i+1, tAl.TAxis, aAl.A*tAl.A, aAl.B*tAl.A+tAl.B);
                                    break;
              case align_COLLAPSE : aAl.TAxis = 0;
                                    aAl.Attr  = align_COLLAPSE;
			// IniRule[i] = AlignAxis(align_COLLAPSE, i+1, 0);
                                    break;
            }
        }
        AlignRule[i] = aAl;
    }
#ifdef nodef
    for (i = 0; i < TempAMV->Rank(); i++) { 
		AlignRule[i+ArrRank].Attr=TAlign[i+TempRank].Attr;
		AlignRule[i+ArrRank].TAxis=TAlign[i+TempRank].TAxis;
		AlignRule[i+ArrRank].A=TAlign[i+TempRank].A;
		AlignRule[i+ArrRank].B=TAlign[i+TempRank].B;
		AlignRule[i+ArrRank].Bound=TAlign[i+TempRank].Bound;
		AlignRule[i+ArrRank].Axis=IniRule[i+TempRank].Axis;
	}
#endif
	// начальная иниц. 2-ой части правила 
	// выравнивания (если шаблон размножен, то и массив тем более)

	for (i = 0; i < TempRank; i++)
	{  
		aAl = IniRule[i+ArrRank];
		switch (aAl.Attr)
        { 
		   case align_CONSTANT : tAl = TAlign[aAl.TAxis-1];
                                 if (tAl.Attr == align_NORMAL)
                                 {  
									 aAl.TAxis = tAl.TAxis;
									 aAl.B = tAl.A * aAl.B + tAl.B;
									 AlignRule[ArrRank+tAl.TAxis-1] = aAl;
								 }
								 break;
		   case align_REPLICATE: tAl = TAlign[aAl.TAxis-1];
			                     if (tAl.Attr == align_NORMAL)
								 {  
									 aAl.Attr = align_BOUNDREPL;
									 aAl.TAxis = tAl.TAxis;
									 aAl.A = tAl.A;
									 aAl.B = tAl.B;
									 aAl.Bound = APattern->GetSize(tAl.TAxis);
									 AlignRule[ArrRank+tAl.TAxis-1] = aAl;
								 }
								 break;
        }
    }

	#ifdef _TIME_TRACE_
	// Не нужно
	prot << "AlignRule:" << endl;
	vector<AlignAxis>::iterator first = AlignRule.begin(), last = AlignRule.end();
	while(first != last)
	{
		prot << endl << first->Attr << " " << first->Axis << " " << first->TAxis 
			 << " " << first->A << " " << first->B << " " << first->Bound << endl;
		++first;
	}
#ifdef nodef
	first = AlignRule.begin(), 
	last = AlignRule.end();
	while(first != last)
	{
		assert(first->Axis!=0);
		assert(first->TAxis!=0); 
		++first;
	}
#endif
#endif
}

double DArray::RAlnDA(AMView *APattern,  const vector<long>& AAxisArray, 
		const vector<long>& ACoeffArray, const vector<long>& AConstArray, 
		long ANewSign)
{
	if (!APattern->IsDistribute()) {
		prot << "Wrong call RAlnDA" << endl;
		exit(1);
	}

	if (!IsAlign()) {
		// массив не распределен - align
		AlnDA(APattern, AAxisArray, ACoeffArray, AConstArray);
		return 0.0;
	}

	if ( ANewSign != 0) {
		// обновление содержимого распределённого массива 

		// удаляем указатель на данный массив из AMView на которое он распределен
		AM_Dis->DelDA(this);
		// выравниваем данный массив на новый шаблон
		AlnDA(APattern, AAxisArray, ACoeffArray, AConstArray);
		return 0.0;
	}

	double time;
	DArray *oldDA = new DArray(*this);
	CommCost *ralCost = new CommCost(AM_Dis->VM_Dis);
	
	// удаляем указатель на данный массив из AMView на которое он распределен
	AM_Dis->DelDA(this);
	// убрать потом вывод и поменять int DelDA на void DelDA

	// так функции RAlnDA ни чем не отличаются кроме этих вызовов
	AlnDA(APattern, AAxisArray, ACoeffArray, AConstArray);
	ralCost->Update(oldDA, this);

	#ifdef _TIME_TRACE_
	// потом убрать
	int i, j;
	prot << endl;
	for (i = 0; i < AM_Dis->VM_Dis->GetLSize(); i++) {
		for (j = 0; j < AM_Dis->VM_Dis->GetLSize(); j++) {
			prot << "[" << i << "]" << "[" << j << "] = " << ralCost->transfer[i][j] << "; ";
		}
		prot << endl;
	}
	// потом убрать
	#endif

	time = ralCost->GetCost(); 
	delete oldDA;
	delete ralCost;
	return time; 
}

double DArray::RAlnDA(DArray *APattern,  const vector<long>& AAxisArray, 
		const vector<long>& ACoeffArray, const vector<long>& AConstArray, 
		long ANewSign)
{
	if (!APattern->IsAlign()) {
		prot << "Wrong call RAlnDA" << endl;
		exit(1);
	}

	if (!IsAlign()) {
		// массив не распределен - align
		AlnDA(APattern, AAxisArray, ACoeffArray, AConstArray);
		return 0.0;
	}

	if ( ANewSign != 0) {
		// обновление содержимого распределённого массива 
		return 0.0;
	}

	// массив и образец распределены

	double time;
	DArray *oldDA = new DArray(*this);
	CommCost *ralCost = new CommCost(AM_Dis->VM_Dis);

	// удаляем указатель на данный массив из AMView на которое он распределен
	AM_Dis->DelDA(this);
	// убрать потом вывод и поменять int DelDA на void DelDA

	AlnDA(APattern, AAxisArray, ACoeffArray, AConstArray);
//	printf("Ral begin\n");
	ralCost->Update(oldDA, this);
//	printf("Ral Update ended\n");

	#ifdef _TIME_TRACE_
	// потом убрать
	int i, j;
	prot << endl;
	for (i = 0; i < AM_Dis->VM_Dis->GetLSize(); i++) {
		for (j = 0; j < AM_Dis->VM_Dis->GetLSize(); j++) {
			prot << "[" << i << "]" << "[" << j << "] = " << ralCost->transfer[i][j] << "; ";
		}
		prot << endl;
	}
	// потом убрать
	#endif

	time = ralCost->GetCost(); 

	delete oldDA;
	delete ralCost;
	return time; 
}

// -------------------- Distributed --> Distributed ---------------------------------

double ArrayCopy(DArray* AFromArray, const vector<long>& BFromInitIndexArray, 
		const vector<long>& BFromLastIndexArray, const vector<long>& BFromStepArray, 
		DArray* AToArray, const vector<long>& BToInitIndexArray, 
		const vector<long>& BToLastIndexArray, const vector<long>& BToStepArray) 
{
	double time;
	long i,DimSize;
//====
	long p1;
//was	vector<LS> blockIni;
//=***

	unsigned sz = BFromInitIndexArray.size();
	unsigned szt= BToInitIndexArray.size();
	unsigned j;

	vector<long> AFromInitIndexArray(sz); 
	vector<long> AFromLastIndexArray(sz);
	vector<long> AFromStepArray(sz);

	vector<long> AToInitIndexArray(szt); 
	vector<long> AToLastIndexArray(szt);
	vector<long> AToStepArray(szt);
//====
// printf("Distr-Distr; Replicated %d %d \n",AToArray->Repl,AFromArray->Repl);
//=***

	for ( j = 0; j < sz; j++) {
		DimSize = AFromArray->SizeArray[j];
		if (BFromInitIndexArray[j] == -1) {
			AFromInitIndexArray[j]	= 0; 
			AFromLastIndexArray[j]	= AFromArray->GetSize(j + 1) - 1;
			AFromStepArray[j]		= 1;
		} else {
			AFromInitIndexArray[j]	= BFromInitIndexArray[j]; 
			AFromLastIndexArray[j]	= BFromLastIndexArray[j];
			AFromStepArray[j]		= BFromStepArray[j];
			if (AFromInitIndexArray[j] > AFromLastIndexArray[j])
				AFromLastIndexArray[j] = AFromLastIndexArray[j];
			if (AFromLastIndexArray[j] >= DimSize)
				AFromLastIndexArray[j] = DimSize - 1;
		}
	}

	for ( j = 0; j < szt; j++) {
		if (BToInitIndexArray[j] == -1) {
			AToInitIndexArray[j]	= 0; 
			AToLastIndexArray[j]	= AToArray->GetSize(j + 1) - 1;
			AToStepArray[j]			= 1;
		} else {
			AToInitIndexArray[j]	= BToInitIndexArray[j]; 
			AToLastIndexArray[j]	= BToLastIndexArray[j];
			AToStepArray[j]			= BToStepArray[j];
		}
	}

	CommCost *copyCost = new CommCost(AFromArray->AM_Dis->VM_Dis);

	if (!AFromArray->CheckIndex(AFromInitIndexArray, AFromLastIndexArray, AFromStepArray))
	{
		prot << "Wrong call ArrayCopy" << endl;
		exit(1);
	}
	if (!AToArray->CheckIndex(AToInitIndexArray, AToLastIndexArray, AToStepArray))
	{
		prot << "Wrong call ArrayCopy" << endl;
		exit(1);
	}

//====
/* was
	for (i = 0; i < AFromArray->Rank(); i++) {
//		cout << AFromArray->Rank() << endl;
//		cout << "AFromInitIndexArray[" << i << "] = " << AFromInitIndexArray[i] << endl;
//		cout << "AFromLastIndexArray[" << i << "] = " << AFromLastIndexArray[i] << endl;
		blockIni.push_back(LS(AFromInitIndexArray[i], AFromLastIndexArray[i]));
	}

	i = 5;

	Block readBlock(blockIni);

	if (AFromArray->Repl)
		time = 0;
	else {
		copyCost->CopyUpdate(AFromArray, readBlock);
		time = copyCost->GetCost();
	}
*/

//тут был цикл с cout & i=5; & Block...
//=***
	if (AFromArray->Repl)
		time = 0;
	else {
//====
		if(AToArray->Repl)
		{
//=***
		 	vector<LS> blockIni;

		  	for (i = 0; i < AFromArray->Rank(); i++) {
//				cout << AFromArray->Rank() << endl;
//				cout << "AFromInitIndexArray[" << i << "] = " << AFromInitIndexArray[i] << endl;
//				cout << "AFromLastIndexArray[" << i << "] = " << AFromLastIndexArray[i] << endl;
//				cout << "AFromStepArray[" << i << "] = " << AFromStepArray[i] << endl;
				if(AFromStepArray[i]>0)
					blockIni.push_back(LS(AFromInitIndexArray[i], AFromLastIndexArray[i], AFromStepArray[i]));
				else
					blockIni.push_back(LS(AFromLastIndexArray[i], AFromInitIndexArray[i], -AFromStepArray[i]));
			}
	
			Block readBlock(blockIni);
			copyCost->CopyUpdate(AFromArray, readBlock);
			time = copyCost->GetCost();
//			printf("Synch=%d time=%f\n",SynchCopy,time);

//====
		}
		else
		{		long x1,x2,x3;

	   		for (p1 = 0; p1 < copyCost->GetLSize(); p1++) {
			 	vector<LS> blockIni;
			 	vector<LS> blockIni1;

				Block locBlock(AToArray, p1, 1);

				for (i = 0; i < AToArray->Rank(); i++) 
				{	if(AToStepArray[i]>0)
						blockIni1.push_back(LS(AToInitIndexArray[i], AToLastIndexArray[i], AToStepArray[i]));
					else
						blockIni1.push_back(LS(AToLastIndexArray[i], AToInitIndexArray[i], -AToStepArray[i]));
				}
				Block writeBlock(blockIni1);

//printf("lockAll[%d].0=%d-%d\n",p1,locBlock.GetLower(0),locBlock.GetUpper(0));
//printf("lockAll[%d].0=%d-%d\n",p1,locBlock.GetLower(1),locBlock.GetUpper(1));
//printf("lockAll[%d].empty=%d\n",p1,locBlock.empty());
//printf("writeBl[%d].1=%d-%d\n",p1,writeBlock.GetLower(0),writeBlock.GetUpper(0));
//printf("writeBl[%d].1=%d-%d\n",p1,writeBlock.GetLower(1),writeBlock.GetUpper(1));
				Block writeLocBlock = locBlock ^ writeBlock;

				if(writeLocBlock.empty()) continue; //блок вычислений не на этом процессоре
//printf("ATo[%d].0=%d-%d\n",p1,writeLocBlock.GetLower(0),writeLocBlock.GetUpper(0));
//printf("ATo[%d].1=%d-%d\n",p1,writeLocBlock.GetLower(1),writeLocBlock.GetUpper(1));

				for (i = 0; i < AFromArray->Rank(); i++) 
				{
// printf("FROM ***%d-%d***\n",AFromInitIndexArray[i],AFromLastIndexArray[i]);

					if(AToStepArray[i]==0 && AToInitIndexArray[i]==AToLastIndexArray[i]) AToStepArray[i]=1; // если копируется в одну строку, то шаг не считать нулевым а можно считать каким угодно, например единицей

					if(AFromStepArray[i]>0 && AToStepArray[i]>0)
						x1 = AFromInitIndexArray[i] + (writeLocBlock.GetLower(i) - AToInitIndexArray[i])*(AFromStepArray[i]/AToStepArray[i]);
					if(AFromStepArray[i]>0 && AToStepArray[i]<0)
						x1 = AFromInitIndexArray[i] + (writeLocBlock.GetLower(i) - AToLastIndexArray[i])*(-AFromStepArray[i]/AToStepArray[i]);

					if(AFromStepArray[i]<=0 && AToStepArray[i]>0)
						x1 = AFromLastIndexArray[i] + (writeLocBlock.GetLower(i) - AToInitIndexArray[i])*(-AFromStepArray[i]/AToStepArray[i]);
					if(AFromStepArray[i]<=0 && AToStepArray[i]<0)
						x1 = AFromLastIndexArray[i] + (writeLocBlock.GetLower(i) - AToLastIndexArray[i])*(AFromStepArray[i]/AToStepArray[i]);

					x2 = x1 + (writeLocBlock.GetUpper(i) - writeLocBlock.GetLower(i))*(abs(AFromStepArray[i]/AToStepArray[i]));
					x3 = abs(AFromStepArray[i]);

					blockIni.push_back(LS(x1,x2,x3));
				}

				Block readBlock(blockIni);
// printf("AFrom[%d].0=%d-%d\n",p1,readBlock.GetLower(0),readBlock.GetUpper(0));
// printf("AFrom[%d].1=%d-%d\n",p1,readBlock.GetLower(1),readBlock.GetUpper(1));

				copyCost->CopyUpdateDistr(AFromArray, readBlock, p1);
			}
			time = copyCost->GetCost();

//			printf("Synch=%d time=%f\n",SynchCopy,time);

		}
//=***
	}

	#ifdef _TIME_TRACE_
	// потом убрать
	//long j;
	prot << endl;
	for (i = 0; i < AFromArray->AM_Dis->VM_Dis->GetLSize(); i++)
	{
		for (j = 0; j < AFromArray->AM_Dis->VM_Dis->GetLSize(); j++)
		{
			prot << "[" << i << "]" << "[" << j << "] = " << copyCost->transfer[i][j] << "; ";
		}
		prot << endl;
	}
	// потом убрать
	#endif

	// ??? так вобщем-то всё, но в будущем сюда надо добавить еще два вида копирования
	// один - с не распределенного массива на распределенный(можно и сечас добавить так как time = 0
	// другой - когда оба массива распределенны(это хуже)
	// это не рассматривая случая с процессором ввода-вывода
	delete copyCost;
	return time;
}

// -------------------- Distributed --> Replicated  ---------------------------------
//Что то у меня эта фукнция вызывалась только на sor одномерном на фортране написанном при коипровании в файл массива

double ArrayCopy(DArray* AFromArray, const vector<long>& BFromInitIndexArray, 
		const vector<long>& BFromLastIndexArray, const vector<long>& BFromStepArray, 
		long ACopyRegim)
{
	double time;
	long i;
	long DimSize;
	vector<LS> blockIni;
	Block readBlock;
	unsigned sz = BFromInitIndexArray.size();
	unsigned j;
// printf("Distr-Repl; Replicated %d \n",AFromArray->Repl);

	vector<long> AFromInitIndexArray(sz); 
	vector<long> AFromLastIndexArray(sz);
	vector<long> AFromStepArray(sz);

	for ( j = 0; j < sz; j++) {
		DimSize = AFromArray->SizeArray[j];
		if (BFromInitIndexArray[j] == -1) {
			AFromInitIndexArray[j] = 0; 
			AFromLastIndexArray[j] = AFromArray->GetSize(j + 1) - 1;
			AFromStepArray[j] = 1;
		} else {
			AFromInitIndexArray[j] = BFromInitIndexArray[j]; 
			AFromLastIndexArray[j] = BFromLastIndexArray[j];
			AFromStepArray[j] = BFromStepArray[j];
			if (AFromInitIndexArray[j] > AFromLastIndexArray[j])
				AFromLastIndexArray[j] = AFromLastIndexArray[j];
			if (AFromLastIndexArray[j] >= DimSize)
				AFromLastIndexArray[j] = DimSize - 1;
		}
	}
#ifdef P_DEBUG
	prot << "ArrayCopy: " << *AFromArray;
	prot << "ArrayCopy: AFromInitIndexArray: ";
	for ( j = 0; j < sz; j++)
		prot << AFromInitIndexArray[j] << ',';
	prot << endl;
	prot << "ArrayCopy: AFromLastIndexArray: ";
	for ( j = 0; j < sz; j++)
		prot << AFromLastIndexArray[j] << ',';
	prot << endl;
	prot << "ArrayCopy: AFromStepArray: ";
	for ( j = 0; j < sz; j++)
		prot << AFromStepArray[j] << ',';
	prot << endl;
#endif

	CommCost *copyCost = new CommCost(AFromArray->AM_Dis->VM_Dis);

	if (!AFromArray->CheckIndex(AFromInitIndexArray, AFromLastIndexArray, AFromStepArray)) {
		prot << "Wrong call ArrayCopy" << endl;
		exit(1);
	}

	for (i = 0; i < AFromArray->Rank(); i++) {
		blockIni.push_back(LS(AFromInitIndexArray[i], AFromLastIndexArray[i]));
	}
	readBlock = Block(blockIni);

	if (AFromArray->Repl)
		time = 0;
	else{ 
//====
		int p1;
	   		for (p1 = 0; p1 < copyCost->GetLSize(); p1++) {
			 	vector<LS> blockIni;

				for (i = 0; i < AFromArray->Rank(); i++) {
//printf("***%d %d - %d %d***\n",AFromInitIndexArray[i],AFromLastIndexArray[i],AFromInitIndexArray[i]+locBlock.GetLower(i),AFromInitIndexArray[i]+locBlock.GetUpper(i));
					if(AFromStepArray[i]>0)
						blockIni.push_back(LS(AFromInitIndexArray[i], AFromInitIndexArray[i], AFromStepArray[i]));
					else
						blockIni.push_back(LS(AFromLastIndexArray[i], AFromLastIndexArray[i], -AFromStepArray[i]));
				}
				Block readBlock(blockIni);

				copyCost->CopyUpdateDistr(AFromArray, readBlock, p1);
			}
			time = copyCost->GetCost();
//			printf("Synch=%d time=%f\n",SynchCopy,time);
		}
//=***

	#ifdef _TIME_TRACE_
	// потом убрать
	//long j;
	prot << endl;
	for (i = 0; i < AFromArray->AM_Dis->VM_Dis->GetLSize(); i++)
	{
		for (j = 0; j < AFromArray->AM_Dis->VM_Dis->GetLSize(); j++)
		{
			prot << "[" << i << "]" << "[" << j << "] = " << copyCost->transfer[i][j] << "; ";
		}
		prot << endl;
	}
	// потом убрать
	#endif

	// ??? так вобщем-то всё, но в будущем сюда надо добавить еще два вида копирования
	// один - с не распределенного массива на распределенный(можно и сечас добавить так как time = 0
	// другой - когда оба массива распределенны(это хуже)
	// это не рассматривая случая с процессором ввода-вывода
	delete copyCost;
	return time;
}

DArray::DArray(const DArray &x) : Space(x)
{
	TypeSize = x.TypeSize;		
    AM_Dis = x.AM_Dis;	
	AlignRule = x.AlignRule;
	Repl = x.Repl;
}

DArray & DArray::operator =(const DArray & x)
{
	if (this != &x) {
		Space::operator =(x);
		TypeSize = x.TypeSize;		
		AM_Dis = x.AM_Dis;	
		AlignRule = x.AlignRule; 
		Repl = x.Repl;
	}
	return * this;
}

DArray::DArray() : Space()
{
	TypeSize = 0;
	AM_Dis = 0;
	AlignRule = vector<AlignAxis>(0);
	Repl = 0;
}

bool DArray::IsAlign()
{
	if (AM_Dis == 0)
		return false;
	return true;
}

long DArray::GetMapDim(long arrDim, int & dir)
{
	long vmDim = 0;
	AlignAxis align;
	DistAxis dist;
	long amDim;

	align = AlignRule[arrDim-1];

	if (align.Attr == align_NORMAL)
	{  
		amDim = align.TAxis;
		dir = (align.A > 0) ? 1 : -1;
		dist = AM_Dis->DistRule[amDim-1];
		if(dist.Attr == map_BLOCK)
			vmDim = dist.PAxis;
	}
	return vmDim;
}

long DArray::CheckIndex(const vector<long>& InitIndexArray, 
		vector<long>& LastIndexArray, const vector<long>& StepArray)
{
	int i;
	long DimSize, BlockSize = 1;

	for (i = 0; i < Rank(); i++) {
		DimSize = SizeArray[i];

		if ((InitIndexArray[i] >= DimSize || InitIndexArray[i] < 0) ||
			(LastIndexArray[i] >= DimSize || LastIndexArray[i] < 0)) { //====//=*** в будущем заменить на <= 0 //was (StepArray[i] !=1)
			prot << "i = " << i << " InitIndexArray[i] = " << InitIndexArray[i]
				 << " LastIndexArray[i] = " << LastIndexArray[i]
				 << " StepArray[i] = " << StepArray[i]
				 << " DimSize = " << DimSize <<endl;
			return 0;
		}
	}

	// вычисление числа элементов в блоке
	for (i = 0; i < Rank(); i++) {
//=== change if
//		printf("Step=%d\n",StepArray[i]);
		if(StepArray[i]!=0) 
			DimSize = (LastIndexArray[i] - InitIndexArray[i]) / StepArray[i] + 1; // для случая когда |Step| может быть > 1
		else
			DimSize = 1;
//was	if ((LastIndexArray[i] - InitIndexArray[i]) % StepArray[i])
//was 		DimSize++;
//=***
		BlockSize *= DimSize;
	}

	return BlockSize;
}

double DArray::RDisDA(const vector<long>& AAxisArray, const vector<long>& ADistrParamArray, long ANewSign)
{
	return AM_Dis->RDisAM(AAxisArray, ADistrParamArray, ANewSign);
}


#ifdef P_DEBUG
ostream& operator << (ostream& os, const DArray& da)
{
	int i;

	os << "DArray: TypeSize = " << da.TypeSize << ", AM_Dis = " << (void*) da.AM_Dis <<endl;
	os << "       " << (Space&) da;
	os << "       " << "LowShdWidthArray:HiShdWidthArray = ";
	for (i = 0; i < da.LowShdWidthArray.size(); i++)
		os << da.LowShdWidthArray[i] << ':' << da.HiShdWidthArray[i] << ' ';
	os << endl;
	os << "       AlignRule:" << endl;
	for (i = 0; i < da.AlignRule.size(); i++)
		os << "       " << i << ' ' << da.AlignRule[i] << endl;

	return os;
}
#endif
