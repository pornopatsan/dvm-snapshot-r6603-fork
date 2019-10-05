// LS.cpp: implementation of the LS class.
//
//////////////////////////////////////////////////////////////////////

#include "Ls.h"

extern void s_s_intersect (long l1, long u1, long l2, long u2, long * l3, long *u3);
extern void r_s_intersect (long l1, long u1, long s1, long l2, long u2, 
						   long * l3, long * u3, long * s3);
extern void r_r_intersect (long l1, long u1, long s1, long l2, long u2, long s2, 
						   long * l3, long * u3, long * s3);


using namespace std;

#if (defined (_MSC_VER) && (_MSC_VER < 1300)) || (defined (__GNUG__) && (__GNUC__  < 3))
template <class T>
T min(T a, T b)
{
	return a < b ? a : b;
}

template <class T>
T max(T a, T b)
{
	return a >= b ? a : b;
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LS::LS()
{
	// create empty LS
	Lower = 0; 
	Upper = -1;
	Stride = 1;
}

LS::~LS()
{

}

LS::LS(long ALower, long AUpper, long AStride)
{

	if (ALower <= AUpper) {
		Lower = ALower;
		Upper = AUpper;
		Stride = AStride;
	} else {
		Lower = 0;
		Upper = -1;
		Stride = 1;
	}
}

bool operator == (const LS& x, const LS& y)
{
	return x.Lower == y.Lower && x.Upper == y.Upper && x.Stride == y.Stride;
}

bool operator < (const LS& x, const LS& y)
{
	return true;
}

LS LS::operator ^ (const LS & x) const
{
	LS temp;

	if ((Stride == 1) && (x.Stride == 1)) {
		s_s_intersect(Lower, Upper, x.Lower, x.Upper, &temp.Lower, &temp.Upper);
		temp.Stride = 1;
	} else if (Stride == 1) {
		r_s_intersect(x.Lower, x.Upper, x.Stride, Lower, Upper, 
			&temp.Lower, &temp.Upper, &temp.Stride);
	} else if (x.Stride == 1) {
		r_s_intersect(Lower, Upper, Stride, x.Lower, x.Upper, 
			&temp.Lower, &temp.Upper, &temp.Stride);
	} else {
		r_r_intersect(Lower, Upper, Stride, x.Lower, x.Upper, x.Stride, 
			&temp.Lower, &temp.Upper, &temp.Stride);
	}
	return temp;
}

long LS::GetLSSize() const
{
//====
//printf("GETSIZE Lower=%d Upper=%d Stride=%d\n",Lower,Upper,Stride);
//was return IsEmpty() ? 0 : Upper - Lower + 1;
	if(IsEmpty() || Upper<0 || Lower<0) return 0;
	else return (Upper - Lower) / Stride + 1;
//=***
}

bool LS::IsEmpty() const
{
	return (Lower > Upper) ? true : false;
}

// Преобразует тсходный LS в соответствии с обратнам отображением
// i = A * j * B ==> j = (i - B) / A  и соответствующая коррекция

void LS::transform(long A, long B, long daDimSize)
{
	long daB1, daB2;
	long daLower, daUpper;
	long displace, displace_0, temp;

 	daB1 = (Lower - B) / A;
	daB2 = (Upper - B) / A;
	displace = (Lower - B) % A;
	displace_0 = (Upper - B) % A;
	if (A < 0)
	{
		temp = displace;
		displace = displace_0;
		displace_0 = -temp;
	}
	daLower = min(daB1, daB2);
	daUpper = max(daB1, daB2);

	if ((daLower < 0 && daUpper < 0) || 
		(daLower >= daDimSize) ||
		(daUpper == 0 && displace_0 < 0))
	{
		Lower = -1;
		return;
	}

	if (displace != 0 && daUpper != 0)
		daLower++;
	if (daLower < 0)
		daLower = 0;
	if (daUpper >= daDimSize)
		daUpper = daDimSize - 1;
	if (daUpper < daLower)
	{		
		Lower = -1;
		return;
	}

	Lower = daLower;
	Upper = daUpper;

}

bool LS::IsBoundIn(long ALeftBSize, long ARightBSize) const
{
	if (ALeftBSize > GetLSSize())
		return false;
	if (ARightBSize > GetLSSize())
		return false;
	return true;
}

bool LS::IsLeft(long elem) const
{
	return IsEmpty() ? false : Lower > elem;
}

bool LS::IsRight(long elem) const
{
	return IsEmpty() ? false : Upper < elem;
}

//====
long LS::GetLower()
{ return Lower;
}

long LS::GetUpper()
{ return Upper;
}
//=***
