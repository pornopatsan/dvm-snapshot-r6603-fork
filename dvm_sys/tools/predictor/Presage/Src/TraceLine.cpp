#include <string.h>
#include <assert.h>
#include <memory>
#include <stdlib.h>

#include <fstream>

#include "TraceLine.h"
#include "ParseString.h"
#include "Ver.h"

#ifdef _UNIX_
#define _strdup strdup
#endif

using namespace std;
extern ofstream prot;

VectorTraceLine *	traceLines;		// lines from file, partially decoded

char	*	TraceLine::first_file_name = NULL;
int			TraceLine::first_line_number = -1;
//bool		VectorTraceLine::startStoreLinesRet = false;
//bool		VectorTraceLine::startStoreLines = false;

// --------------------------------- TraceLine ---------------------------------------

// -------------------------------- CONSTRUCTORS ------------------------------------- 

TraceLine::TraceLine(char * buffer)
{
	const	int		LINE_BUFFER_SIZE = 256;
	char			f_file[LINE_BUFFER_SIZE];
	char			f_name[LINE_BUFFER_SIZE];
	char	*		fn_pos;
    int				str_len;

    if (ParamFromString("TIME", func_time, buffer) &&
		ParamFromString("LINE", source_line, buffer) &&
		ParamFromString("FILE", f_file, buffer)) {

    
		if ((fn_pos=strstr(buffer,"call_"))!=NULL) { 
			line_type = Call_; 
			fn_pos += strlen("Call_"); 
		} else if ((fn_pos=strstr(buffer,"ret_"))!=NULL) { 
			line_type = Ret_; 
			fn_pos += strlen("Ret_"); 
		} else if ((fn_pos=strstr(buffer,"Event_"))!=NULL) { 
			line_type = Event_; 
			fn_pos += strlen("Event_");;
		} else 
			goto unknown;

		str_len = strcspn(fn_pos," ");
		strncpy(f_name, fn_pos, str_len);
		f_name[str_len]='\0';
		func_id = EventNameToID((string) f_name);
		source_file =_strdup(f_file);
		if (first_file_name == NULL) {
			first_file_name = _strdup(f_file);
			first_line_number = source_line;
		}
		info_line = _strdup(buffer); //was NULL
		if (func_id == Unknown_Func)
#ifdef P_DEBUG
			prot << "    Warning : unknown function: " << f_name 
				 << " file = " << source_file 
				 << " line = " << source_line << endl;
#else
		;
#endif
	} else {

unknown:

	func_time = 0.0;
	source_line = 0;
	source_file = NULL;
	func_id = Unknown_Func; 
	source_file = NULL;
	line_type = Unknown_;
	info_line =_strdup(buffer);
	}
}

// ---------------------------- COPY CONSTRACTOR ----------------------------------- 

TraceLine::TraceLine(TraceLine& tr) :
    line_type(tr.line_type),
    func_id(tr.func_id),
    func_time(tr.func_time),
    source_line(tr.source_line)
{
    source_file = tr.source_file;
	tr.source_file = NULL;
    info_line = tr.info_line;
	tr.info_line = NULL;
}

// -------------------------------- DESTRUCTOR ------------------------------------- 

TraceLine::~TraceLine()
{
	if (source_file != NULL)
		delete source_file;
	if (info_line != NULL)
		delete info_line;
}

// ------------------------------ VectorTraceLine ------------------------------------

// -------------------------------- CONSTRUCTOR -------------------------------------- 

VectorTraceLine::VectorTraceLine(char* file_name) :
	endOfVector(false),
	count(0), 
	size(0), 
	lines(NULL),
	p_count(0),
	p_size(0),
	p_lines(NULL),
	startStoreLines(false),
	startStoreLinesRet(false)
{
//    trace_file.open(file_name);   //ZIP
    if ((trace_file = gzopen(file_name, "rb")) == NULL) { 
// 	if (!trace_file.is_open()) {  //ZIP
		cerr << "Can't open trace file '" << file_name << '\'' << endl;
//		trace_file.exceptions(ostream::badbit | ostream::failbit | ostream::eofbit);
        exit(1);
	} else {
		
        prot << "    Reading/initial decoding of trace file..." << endl;
		
		// read preamble lines
		while (!startStoreLines && getLine())
			;

		getFrame();
		prot << "    Completed." << endl;
	}
}

// -------------------------------- DESTRUCTOR ------------------------------------- 

VectorTraceLine::~VectorTraceLine()
{ 
	for (int i = 0; i < size; i++)
		delete lines[i];
	free(lines);
	//gzclose(trace_file);
}

bool VectorTraceLine::getLine()
{
	const	int		LINE_BUFFER_SIZE = 256;
	char			buffer[LINE_BUFFER_SIZE];
	int				len;
	int				spn;
	TraceLine *		tmp;
	
//	if (!trace_file)
	if (trace_file == NULL)
		return false;
	else {
		do {
//			if (!trace_file.getline(buffer, LINE_BUFFER_SIZE)) {
            if (gzgets(trace_file, buffer, LINE_BUFFER_SIZE) == Z_NULL) { 
				cerr << "Bad trace file" << endl;
				prot << "Bad trace file" << endl;
				exit(0);
			}
			len=strlen(buffer);
            if ((buffer[len - 1] == 0x0a) && (buffer[len - 2] == 0x0d))
                len -= 2;
            else if (buffer[len - 1] == 0x0a)
                len -= 1;
            buffer[len] = 0;
			spn=strspn(buffer," ");
		} while (	len==0		|| 
					len==spn	|| 
					(len==1 && *buffer==0x0d) || 
					(strncmp(buffer+spn, "----------", 10) == 0)); 

		tmp = new TraceLine(buffer + spn);
		// control event dvm_exit
		if (tmp->line_type == Event_) {
			endOfVector = true;
			return false;
		}

		
		if ((!startStoreLinesRet) && (tmp->line_type == Ret_) && (tmp->func_id == dvm_Init))
			startStoreLinesRet = true;

		if ((!startStoreLines) && (tmp->line_type == Call_) && startStoreLinesRet)
			startStoreLines = true;


		if (startStoreLines) {
			// store trace line
			lines = (TraceLine **) realloc(lines, sizeof(TraceLine *) * ++size);
			lines[size - 1] = tmp;
		} else {
			// store preamble trace line
			p_lines = (char **) realloc(p_lines, sizeof(char *) * ++p_size);
			p_lines[p_size - 1] = tmp->info_line;
		}
		return true;
	}
}

bool VectorTraceLine::getFrame()
{
	if (lines[size - 1]->line_type == Call_) {
		// next call exist in the trace file
		int i;

		// save last Call_ line from frame
		TraceLine *		tmp = new TraceLine(*lines[size - 1]);
		// delete previouse frame;
		for (i = 0; i < size; i++) 
			delete lines[i];
		free(lines);

		size = count = 0;
		lines = NULL;

		// store trace line Call_ in the object
		lines = (TraceLine **) realloc(lines, sizeof(TraceLine *) * ++size);
		lines[size - 1] = tmp;

		// store the rest of the frame
		while (getLine() && lines[size - 1]->line_type != Call_)
			;
		return true;
	} else 
		return false;
}

TraceLine* VectorTraceLine::next() 
{
		count++;
		if (count == size) {
			// get next trace frame
			if (getFrame()) {
				return lines[++count]; 
			} else 
				return NULL;
		} else
			return lines[count]; 
}


// ------------------------------ GetUnknownLines ------------------------------------ 

void VectorTraceLine::GetUnknownLines(int& il_count, char**& info_lines)
{
	while ((count < size) && (lines[count]->line_type == Unknown_)) {

		info_lines = (char**) realloc(info_lines, sizeof(char*) * (++il_count));
		assert(info_lines != NULL);
        info_lines[il_count-1] = _strdup(lines[count]->info_line);
        count++;

	}
}



