#ifndef DArrayH
#define DArrayH

//////////////////////////////////////////////////////////////////////
//
// DArray.h: interface for the DArray class.
//
//////////////////////////////////////////////////////////////////////


#include <vector>
#include <fstream>

#include "Space.h"
#include "AMView.h"
#include "AlignAxis.h"
#include "Block.h"
#include "CommCost.h"
//#include "ModelStructs.h"

class AMView;

class DArray : public Space {
	void PrepareAlign(long& TempRank, const std::vector<long>& AAxisArray, 
		const std::vector<long>&  ACoeffArray, const std::vector<long>& AConstArray, 
		std::vector<AlignAxis>& IniRule);

	
	//  проверяет индексы на выход за пределы массива. 
	// Возвращается число элементов в блоке. 0 если их нет или вышли за массив
	long CheckIndex(const std::vector<long>& InitIndexArray, 
		std::vector<long>& LastIndexArray, 
		const std::vector<long>& StepArray);

public:

	std::vector<long>	LowShdWidthArray;
	std::vector<long>	HiShdWidthArray;
	long TypeSize;		// Размер в байтах одного элемента массива
	AMView *AM_Dis;		// AMView  в которую отображается DArray
	// Правило выравнивания на AM_Dis - ? 
	// для чего нужен - для Pattern или еще для чего-нибудь тоже
	std::vector<AlignAxis> AlignRule; 
	int Repl; // признак полностью размноженного по AM_Dis массива 

	DArray();
 	DArray(const std::vector<long>& ASizeArray, const std::vector<long>& ALowShdWidthArray,
		const std::vector<long>& AHiShdWidthArray, int ATypeSize);
	DArray(const DArray &);
    ~DArray();
	DArray & operator= (const DArray &x);

	void AlnDA(AMView *APattern, const std::vector<long>& AAxisArray, 
		const std::vector<long>& ACoeffArray, const std::vector<long>& AConstArray);
    void AlnDA(DArray* APattern,  const std::vector<long>& AAxisArray, 
		const std::vector<long>& ACoeffArray, const std::vector<long>& AConstArray);

    double RAlnDA(AMView *APattern,  const std::vector<long>& AAxisArray, 
		const std::vector<long>& ACoeffArray, const std::vector<long>& AConstArray, 
		long ANewSign);
    double RAlnDA(DArray* APattern,  const std::vector<long>& AAxisArray, 
		const std::vector<long>& ACoeffArray, const std::vector<long>& AConstArray, 
		long ANewSign);

    friend double ArrayCopy(
		DArray* AFromArray, 
		const std::vector<long>& AFromInitIndexArray, 
		const std::vector<long>& AFromLastIndexArray, 
		const std::vector<long>& AFromStepArray, 
		DArray* AToArray, 
		const std::vector<long>& AToInitIndexArray, 
		const std::vector<long>& AToLastIndexArray, 
		const std::vector<long>& AToStepArray); 

    friend double ArrayCopy(DArray* AFromArray, 
		const std::vector<long>& AFromInitIndexArray, 
		const std::vector<long>& AFromLastIndexArray, 
		const std::vector<long>& AFromStepArray, 
		long ACopyRegim);


    // ArrCpy ? - какие параметры, что делает

	long GetMapDim(long arrDim, int &dir); // Функция возвращает номер 
	// измерения VM на которое отображено указанное измерение массива
	// (если измерение массива размножено по всем направлениям матрицы виртуальных процессоров - 0).
	// в dir заносится 1 или -1 в зависимости от направления разбиения измерения массива
	bool IsAlign();

	double RDisDA(const std::vector<long>& AAxisArray, const std::vector<long>& ADistrParamArray, 
					long ANewSign);
#ifdef P_DEBUG
	friend std::ostream& operator << (std::ostream& os, const DArray& s);
#endif

};


#endif
