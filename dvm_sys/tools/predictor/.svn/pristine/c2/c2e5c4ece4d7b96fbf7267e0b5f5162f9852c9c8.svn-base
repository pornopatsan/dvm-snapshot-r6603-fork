#ifndef VM_H
#define VM_H
//////////////////////////////////////////////////////////////////////
//
// Vm.h: interface for the Virtual machine (VM) class.
//
//////////////////////////////////////////////////////////////////////

#include "Space.h"
#include  <vector>
//#include "ps.h"
//using namespace std;

enum mach_Type {
	mach_ETHERNET,			// 0
	mach_TRANSPUTER,		// 1
	mach_MYRINET			// 2
};
//grig
typedef std::vector<long> long_vect;
//\grig



class VM : public Space {

	const	VM*			parent;		// pointer to parent VM
	mach_Type			MType;		// system type: 0 - ethernet, 1 - transputers, 2 - myrinet
	int					numChanels;	// number of chanells for myrinet
	double				scale;
	double				TStart;		// информация о реальной машине:  start time
	double				TByte;		// информация о реальной машине:  byte trasfer time
	double				ProcPower;	// relative VM power 
	int					ProcCount;	// number of processors in VM
	std::vector<int>	mapping;	// map to absolute processors numbers
	std::vector<double> weight;		// vector - PS weights		
		
	// calculates number of processors in VM
	int		procCount();


//grig
	std::vector<double> vProcPower; // относительные производительности процессоров
	std::vector<double> vWeights;   // веса элементов измерений установленные процессорной системы
public:



	void SetvWeights(std::vector<double> & varray) { 
		  vWeights.resize(0);
		  vWeights.resize(varray.size());
		for(int i=0;i<varray.size();i++)
			vWeights[i]=varray[i];
	}
	double	getProcPower(int k) { return vProcPower[this->map(k)];}
//\grig

 public:
	
	// constructor for root VM
	VM(const std::vector<long>& ASizeArray, mach_Type AMType, int AnumChanels, 
		double Ascale, double ATStart, double ATByte, double AProcPower, std::vector<double>& AvProcPower);

	// constructor for child VM (crtps_)
	VM(const std::vector<long>& lb, const std::vector<long>& ASizeArray, const VM* Aparent);

	// constructor for child VM (psview_)
	VM(const std::vector<long>& ASizeArray, const VM* Aparent);

	//grig!!!!
	VM(const std::vector<long>& ASizeArray, mach_Type AMType, int AnumChanels, 
		double Ascale, double ATStart, double ATByte,  std::vector<double>& AvProcPower);
	//\grig

	// Destructor
    ~VM();

//	double	getTByte() const { return TByte; }
//	double	getTStart() const { return TStart; }
//	int		getMType() const { return MType; }
	int		getProcCount() const { return ProcCount; }
//comment by grig	double	getProcPower() const { return ProcPower; }
	double	getProcPower() const { return 0; }
	int		getNumChanels() const { return numChanels; }
//	double	getScale() const { return scale; }
	const	std::vector<long>& getSizeArray() const { return SizeArray; }
	int		map(int i) const { return mapping[i]; }

	// Set weights for VM
	void setWeight(const std::vector<double>& Aweight);

#ifdef P_DEBUG
	friend std::ostream& operator << (std::ostream& os, const VM& vm);
#endif
 };

extern VM	*	rootVM;			// pointer to root VM
extern VM	*	currentVM;		// pointer to current VM

//grig
extern long_vect MinSizesOfAM; // для автоматического поиска
//\grig

inline int	MPSProcCount() { return currentVM->getProcCount(); }

#endif
