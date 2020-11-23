#ifndef TRACELINE_H
#define TRACELINE_H

#include <fstream>

#include "Event.h"
#include "CallInfoStructs.h"
//#include "../../Zlib/Include/zlib.h"
#include "zlib.h"

// Structure for the first stage of file parsing -- file reading

class TraceLine {

public:

    LineType	line_type;
    Event		func_id;			// Only for call-type lines
    double		func_time;
    int			source_line;
    char*		source_file;
    char*		info_line;			// Only for info-type lines

	static char* first_file_name;
	static int	 first_line_number;

				TraceLine(char * buffer);
				TraceLine(TraceLine& tr);
				~TraceLine();
				TraceLine& operator = (TraceLine& tr);
};

class VectorTraceLine {

	/*static*/ bool	startStoreLinesRet;
	/*static*/ bool	startStoreLines;
	bool		endOfVector;

	unsigned	count;		// current line
	unsigned	size;		// vector size
	TraceLine**	lines;		// array of pointers to lines

	unsigned	p_count;	// current preamble line
public:
	unsigned	p_size;		// preamble vector size
	char**		p_lines;	// array of pointers to preamble lines

//	std::ifstream	trace_file;
    //gzFile      trace_file;
private:
	bool		getLine();
	bool		getFrame();
public:
	gzFile      trace_file;


	VectorTraceLine(unsigned sz = 0) : lines(NULL), count(0), size(sz) {}
	VectorTraceLine(char* file_name);
	~VectorTraceLine();

	TraceLine*		current() { return lines[count]; } 
	void			GetUnknownLines(int& il_count, char**& info_lines);
	TraceLine*		next();
	bool			end() const { return endOfVector; }

	friend root_Info* Get_Root_Info();

	//grig
	void restore() {
			VectorTraceLine::startStoreLinesRet = false;
		VectorTraceLine::startStoreLines = false;
	}

	//\grig

	//====
	unsigned Get_p_size();
	char * Get_p_lines(int i);
	//=***

};

extern VectorTraceLine *	traceLines;		// lines from file, partially decoded
	
#endif