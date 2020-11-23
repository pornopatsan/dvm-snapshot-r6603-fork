/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

#include <stdio.h>
#include "defs.h"
#include "bif.h"
#include "ll.h"
#include "symb.h"
#include "db.h"

extern PTR_TYPE global_default;
extern PTR_LLND make_llnd();
extern PTR_FILE fi;

void errstr();

/*
 * set_ll_list attaches a new node to a given old_list and
 * makes sure that the old_list is of the same type as ll_type
 */
PTR_LLND 
set_ll_list(old_list, node, ll_type)
	PTR_LLND old_list, node;
	int     ll_type;
{
	register PTR_LLND temp, new;

	if (!node) {	/* the first item in the list */
		new = make_llnd(fi,ll_type, old_list, LLNULL, SMNULL);
		new->type = global_default;
		return (new);
	}
	if (old_list->variant != ll_type) { 	/* first two items */
		new = make_llnd(fi,ll_type, old_list,
				make_llnd(fi,ll_type, node, LLNULL, SMNULL),
				SMNULL);
		new->type = global_default;
		new->entry.Template.ll_ptr2->type = global_default;
		return (new);
}
	new = make_llnd(fi,ll_type, node, LLNULL, SMNULL);
	for (temp = old_list;
	     temp->entry.Template.ll_ptr2;
	     temp = temp->entry.Template.ll_ptr2)
		;
	temp->entry.Template.ll_ptr2 = new;
	new->type = global_default;
	return (old_list);
}


/*
 * set_id_list takes an old id list and runs down the list attaching new 
 * id at the end
 */
PTR_SYMB 
set_id_list(old_list, id)
	PTR_SYMB old_list, id;
{
	register PTR_SYMB temp;

	if (!id)
	    return (old_list);
        if(id->id_list) {
            errstr("'%s' is a duplicate dummy argument",id->ident,121); /*podd 9.03.00*/
            return(old_list);
        }
	for (temp = old_list; temp->id_list; temp = temp->id_list)
		;

	temp->id_list = id;
	return (old_list);
}

PTR_LLND
add_to_lowLevelList(ele, oldList)
PTR_LLND ele, oldList;
{
	register PTR_LLND temp;

	if (!ele)
	     return (oldList);

	for (temp = oldList; temp->entry.list.next; temp = temp->entry.list.next)
		;

	temp->entry.list.next = ele;
	return (oldList);
}
     
PTR_LLND
add_to_lowList(ele, oldList)
PTR_LLND ele, oldList;
{
	register PTR_LLND temp;

	if (!ele)
	     return (oldList);

	for (temp = oldList; temp->entry.Template.ll_ptr2; temp = temp->entry.Template.ll_ptr2)
	     ;

	temp->entry.Template.ll_ptr2 = ele;
	
	return (oldList);
}
