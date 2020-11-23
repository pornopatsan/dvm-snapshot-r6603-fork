#ifndef DistAxisH
#define DistAxisH

//////////////////////////////////////////////////////////////////////
//
// DistAxis.h: interface for the DistAxis class.
//
//////////////////////////////////////////////////////////////////////

enum map_Type {
	map_BLOCK		= 1,	// 1
	map_COLLAPSE,			// 2
	map_REPLICATE,			// 3
	map_NORMVMAXIS			// 4
};


class DistAxis {
public:
	map_Type Attr;  // Тип распределения
	long Axis;  // Измерение AMView 
	long PAxis;  // Измерение VM (на него отображается Axis)

	DistAxis(map_Type AAttr, long AAxis, long APAxis);
	DistAxis();
	virtual ~DistAxis();

	DistAxis& operator = (const DistAxis&); 
	friend bool operator == (const DistAxis& x, const DistAxis& y);
	friend bool operator < (const DistAxis& x, const DistAxis& y);

};

#endif 
