#ifndef BlockH
#define BlockH

//////////////////////////////////////////////////////////////////////
//
// Block.h: interface for the Block class.
//
//////////////////////////////////////////////////////////////////////


#include <vector>

#include "Ls.h"
#include "DArray.h"

class DArray;

// Rectangular section of elements

class Block {
	// std::vector of LS for every dimensions
	std::vector<LS> LSDim;

public:	

	Block(std::vector<LS> &v);
	Block();
	Block(DArray *da, long ProcLI);
	//grig
	Block(DArray *da, long ProcLI,int a); // a - не значащий параметр
	//\grig


	virtual ~Block();

	// множитель для определения размера блока, получаемый как 
	//произведение размеров по всем измерениям кроме указанного 
	long GetBlockSizeMult2(long dim1, long dim2);
	long GetBlockSizeMult(long dim);

	// true если для данного блока по данному измерению данный элемент 
	// находится левее (правее) элементов блока. Для пустого блока false
	bool IsLeft(long arrDim, long elem); 
	bool IsRight(long arrDim, long elem);

	// Проверка принадлежности границы локальному блоку
	bool IsBoundIn(const std::vector<long>& ALeftBSizeArray, 
		const std::vector<long>& ARightBSizeArray);

	Block & operator =(const Block & x);
	long GetRank();
	bool empty();
	 
	// число элементов в блоке
	long GetBlockSize();

	friend Block operator^ (Block &x, Block &y); // intersection

	//====
	long GetUpper(long i);
	long GetLower(long i);
	//=***

};

#endif 
