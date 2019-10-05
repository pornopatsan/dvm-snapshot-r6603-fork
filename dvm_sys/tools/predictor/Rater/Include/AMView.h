#ifndef AMViewH
#define AMViewH

//////////////////////////////////////////////////////////////////////
//
// AMView.h: interface for the AMView class.
//
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <fstream>
#include <list>
#include <vector>
#include <algorithm>

#include "Vm.h"
#include "DistAxis.h"
#include "DArray.h"

class DArray;


//grig
class WeightClass
{
public :
	long ID;				     // PS , при отображении в которую  будут использоваться веса 
	std::vector<double> body;    // opt weights
	
	WeightClass()
	{
		ID=NULL;
		body.resize(0);
	}

	WeightClass(long AID,std::vector<double>& init_weights)
	{
		ID=AID;
		body.resize(0);
		for(int i=0;i<init_weights.size();i++)
		{
			body.push_back(init_weights[i]);
		}

	}

	void GetWeights(std::vector<double> & AAweights)
	{
//		printf("Get SZ id=%lx %d\n",ID,body.size());
		AAweights.resize(body.size());
		for(int i=0;i<body.size();i++)
			AAweights[i]=body[i];
	}

	void SetWeights(long AID,std::vector<double>& init_weights)
	{// printf("Set SZ id=%lx %d\n",ID,init_weights.size());
	
		ID=AID;
		body.resize(0);
		for(int i=0;i<init_weights.size();i++)
		{
			body.push_back(init_weights[i]);
		}
	}
	 ~WeightClass()
	 {
	   body.resize(0);	 
	 }

	 long GetSize()
	 {
		 return body.size();	 
	 }
};
//\grig
class AMView : public Space {

public:	 
	VM *VM_Dis;							// VM на которую отображено AMView
	std::list<DArray*> AlignArrays;		// Список выравненых массивов
	std::vector<DistAxis> DistRule;		// Параметры отображения в виртуальную машину
	std::vector<long> FillArr;			// ??? 
	std::vector<long> BSize;		//====// блоки будут кратны этому размеру 

	int Repl;							// признак полностью размноженного по AM_Dis массива 

//grig
	WeightClass weightEl;           // веса для отображения шаблона
//\grig
 
 	AMView(const std::vector<long>& ASizeArray);
	AMView(const AMView &);			// !!! копируется только VM_Dis и DistRule и FillArr
    ~AMView();

	long	GetMapDim(long arrDim, int & dir);
	int		DelDA(DArray* RAln_da);		// удалить DArray из списка AlignArrays
	void	AddDA(DArray* Aln_da);		// добавить DArray в список AlignArrays

    // Отображение представления абстрактной машины в виртуальную машину
 	void DisAM(VM *AVM_Dis, const std::vector<long>& AAxisArray, const std::vector<long>& ADistrParamArray);

    // Изменение отображения представления абстрактной машины в виртуальную машину
    double RDisAM(const std::vector<long>& AAxisArray, const std::vector<long>& ADistrParamArray, 
		long ANewSign);

	bool IsDistribute();
};





#endif
