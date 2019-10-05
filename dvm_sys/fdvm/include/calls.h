#pragma once

#ifdef __SPF 
extern "C" void addToCollection(const int line, const char *file, void *pointer, int type);
extern "C" void removeFromCollection(void *pointer);
#endif

struct graph_node {
    int id;   //a number of node
    graph_node *next;
    graph_node *next_header_node; //???
    graph_node *Inext;
    SgFile *file;
    int file_id;
    int header_id;
    SgStatement *st_header;
    SgStatement *st_last;
    SgStatement *st_copy;
    SgSymbol *symb;              //??? st_header->symbol()
    char *name;
    struct edge *to_called;      //outcoming
    struct edge *from_calling;   //incoming
    int type;      //flag - type of procedure: 1-external,2-internal,3-module
    int split;     //flag
    int tmplt;     //flag
    int visited;   //flag for partition algorithm
    int clone;     //flag is clone node
    int count;     //counter of inline expansions or calls

#if __SPF
    graph_node() { addToCollection(__LINE__, __FILE__, this, 1); }
    ~graph_node() { removeFromCollection(this); }
#endif
};

struct graph_node_list {
    graph_node_list *next;
    graph_node *node;

#if __SPF
    graph_node_list() { addToCollection(__LINE__, __FILE__, this, 1); }
    ~graph_node_list() { removeFromCollection(this); }
#endif
};

struct edge {
    edge *next;
    graph_node *from;
    graph_node *to;
    int inlined; //1 - inlined, 0 - not inlined

#if __SPF
    edge() { addToCollection(__LINE__, __FILE__, this, 1); }
    ~edge() { removeFromCollection(this); }
#endif
};

struct edge_list {
    edge_list *next;
    edge *edg;

#if __SPF
    edge_list() { addToCollection(__LINE__, __FILE__, this, 1); }
    ~edge_list() { removeFromCollection(this); }
#endif
};