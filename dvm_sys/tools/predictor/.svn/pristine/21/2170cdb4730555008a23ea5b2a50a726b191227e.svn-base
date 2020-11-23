#ifndef SpaceH
#define SpaceH
//////////////////////////////////////////////////////////////////////
//
// Space.h: interface for the Space base class.
//
//////////////////////////////////////////////////////////////////////

#include <vector>
#include <algorithm>
#include <fstream>

class Space {
protected:
	std::vector<long> SizeArray;	// Size of every dimension
	std::vector<long> MultArray;	// Multiplier for each dimension
public:
	Space();
	Space(const std::vector<long>& ASizeArray, std::vector<long> AMultArray);
	Space(const Space &);
 	Space(const std::vector<long>& ASizeArray);
    ~Space();

	long	GetNumInDim(long LI, long dimNum);
	long	GetCenterLI();

	// функци€ возвращает линейный индекс процессора, который смещЄн на shift 
	// по измерению dim от данного процессора заданного линейным индексом 
	long	GetSpecLI(long LI, long dim, int shift); 

	// ¬ычисл€ет линейнный индекс по координатам в данном пространстве
	long	GetLI(const std::vector<long> & SI);

	// длина минимального пути между двум€ процессорами
	long	GetDistance(long LI1, long LI2);

	Space&	operator= (const Space &x);

	// ¬ычисл€ет координаты в данном пространстве по линейному индексу
	// (Space Index - SI)
	void	GetSI(long LI, std::vector<long> & SI);

	  // ¬озвращает линейный размер пространства
	long	GetLSize();

//    inline long GetRank() { return Rank; }

    long GetSize(long AAxis); // измерени€ с 1
	long GetMult(long AAxis); // измерени€ с 1

	unsigned int Rank() { return SizeArray.size(); }

#ifdef P_DEBUG
	friend std::ostream& operator << (std::ostream& os, const Space& s);
#endif

};

#if defined (__GNUG__) && (__GNUC__  < 3)
inline long abs(long x)
{
	return (x < 0) ? (-x) : (x);
};
#endif

#endif
