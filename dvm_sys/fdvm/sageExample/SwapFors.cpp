
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cstdlib>
#define IN_ANAL_
#include "dvm.h"
#undef IN_ANAL_
#include "aks_structs.h"

char *fin_name = 0;
SgFile *current_file;


#define DEBUG true
#define DEBUG_LV2 false
#define DEBUG_LV3 false

#if 0
std::ostream &out = std::cout;
#else
std::ofstream out("log_swap.txt");
#endif

using namespace std;

void PrintVars(SgExpression *node, int LR, int d)
{
	if (LR == 0)
		out << "variant LHS " << node->variant() << " d = " << d << endl;	
	else
		out << "variant RHS " << node->variant() << " d = " << d << endl;
	
	if (node->lhs())
		PrintVars(node->lhs(), 0, d + 1);

	if(node->rhs())
		PrintVars(node->rhs(), 1, d + 1);
}

SageSymbols* SearchVarRef(SgExpression *expr, SageSymbols *smbl)
{
	SageSymbols *newS;
	if(expr->variant() == VAR_REF)
	{
		SageSymbols *tempS = smbl;
		bool flag = true;		
		while(tempS)
		{
			if(expr->symbol() == tempS->symb)
			{
				flag = !flag;
				break;
			}
			tempS = tempS->next;
		}
		if(flag)
		{
			newS = new SageSymbols();
			newS->symb = expr->symbol();
			newS->len = -1;
			newS->next = smbl;
		}
		else
			newS = smbl;
	}
	else
		newS = smbl;

	SgExpression *L = expr->lhs();
	SgExpression *R = expr->rhs();
	if(L)
		newS = SearchVarRef(L,newS);
	if(R)
		newS = SearchVarRef(R,newS);
	return newS;	
}


bool CheckSpecOp(SgExpression *expr, int Op)
{
	if(expr)
	{
		if(expr->variant() == Op)
			return false;
		else
			return CheckSpecOp(expr->lhs(),Op) && CheckSpecOp(expr->rhs(),Op); 
	}
	else
		return true;
}

bool CheckExpr(SgExpression *expr)
{
	if(expr)
	{
		switch(expr->variant())
		{
		case MULT_OP:
			return CheckSpecOp(expr->lhs(), VAR_REF)  ||  CheckSpecOp(expr->rhs(), VAR_REF);
		case MOD_OP:
			return CheckSpecOp(expr->lhs(), VAR_REF)  &&  CheckSpecOp(expr->rhs(), VAR_REF);
		case ARRAY_REF:
			return false;
		default:
			return CheckExpr(expr->lhs()) && CheckExpr(expr->rhs());
		}
	}
	else
		return true;
}

int CountExpr(SgExpression *, int);

int GetConstRef(SgExpression *expr)
{
	SgExpression *temp;
	SgConstantSymb *sc = isSgConstantSymb(expr->symbol());
	temp = &(sc->constantValue()->copy());
	return CountExpr(temp, 0);
}

int CountExpr(SgExpression *expr, int rewrite)
{
	switch(expr->variant())
	{
	case VAR_REF:
		return rewrite;
	case CONST_REF:
		return GetConstRef(expr);
	case INT_VAL:
		return expr->valueInteger();
	case ADD_OP:
		return CountExpr(expr->lhs(),rewrite) + CountExpr(expr->rhs(),rewrite);
	case MULT_OP:
		return CountExpr(expr->lhs(),rewrite) * CountExpr(expr->rhs(),rewrite);
	case SUBT_OP:
		return CountExpr(expr->lhs(),rewrite) - CountExpr(expr->rhs(),rewrite);	
	case MOD_OP:
		return CountExpr(expr->lhs(),rewrite) % CountExpr(expr->rhs(),rewrite);
	case MINUS_OP:
		return -CountExpr(expr->lhs(),rewrite);	
	case DIV_OP:
		return CountExpr(expr->lhs(),rewrite) / CountExpr(expr->rhs(),rewrite);
	case DDOT:
		{
			int L = 1;
			int R = 1;
			if(expr->lhs())
				L = CountExpr(expr->lhs(), 0);
			if(expr->rhs())
				R = CountExpr(expr->rhs(), 0);
			return abs(L - R);
		}
	case STAR_RANGE:
		if(DEBUG)
			out << " Error: unknown operator * in expr" << endl;
		return 1;
	case NULL:
		if(DEBUG)
			out << " unknown expr with NULL-end" << endl;
		break;
	default:
		if(DEBUG)
			out << " unknown operand with variant: " << expr->variant()<< endl;
		break;
	}
	return -1;
}

int CountLen(SgExpression *expr, int step)
{
	int ret_val = 0;
	int a = CountExpr(expr,0);
	int b = CountExpr(expr,step);
	ret_val = abs(a - b);
	
	return ret_val;
}

SageArrayIdxs* AnalizeRef(SgExpression *expr, SageArrayIdxs *arr, int step)
{
	SgExpression *count = expr;
	int p = 0;
	while(count)
	{
		p++;
		count = count->rhs();
	}
	arr->dim = p;
	arr->symb = new SageSymbols*[p];

	p = 0;
	while(expr)
	{
		SageSymbols *allSymb  = new SageSymbols();
		allSymb->symb = NULL;
		allSymb->next = NULL;
		allSymb->len = 0;
		allSymb = SearchVarRef(expr->lhs(),allSymb);
		arr->symb[p] = allSymb;
		if(allSymb->next)
			if(allSymb->next->symb == NULL && CheckExpr(expr->lhs()))
			{	
				arr->symb[p]->len = CountLen(expr->lhs(), step);
				arr->symb[p]->next = NULL;
			}
			else
			{
				while (allSymb->next->len != 0)
					allSymb = allSymb->next;
				allSymb->next = NULL;				
			}
		expr = expr->rhs();
		p++;
	}
	return arr;
}

SageArrayIdxs* AnalizeAssignStat(SgExpression *expr, SageArrayIdxs *retval, int read_write, int step)
{
	switch(expr->variant())
	{
	case ARRAY_REF:
		{
			SageArrayIdxs *outRef = new SageArrayIdxs();
			outRef->array_expr = expr;		
			outRef = AnalizeRef(expr->lhs(),outRef,step);
			outRef->next = NULL;
			outRef->read_write = read_write;
			retval->next = outRef;
			return retval->next;		
			break;
		}
	default:
		break;
	}	
		
	SgExpression *R = expr->rhs();
	SgExpression *L = expr->lhs();

	if(R)
	{
		retval = AnalizeAssignStat(R, retval, read_write, step);
	}
	if(L)
	{
		retval = AnalizeAssignStat(L, retval, read_write, step);
	}
	return retval;
}

void PrintStr(SageArrayIdxs *p)
{
	out << p->dim << endl;
}

SageArrayIdxs* Analize(SgForStmt *loop, int count)
{	
	SageArrayIdxs *q = new SageArrayIdxs();
	SageArrayIdxs *AllRef = q;
	int step = 1;
	if(loop->step())
	{
		step = loop->step()->valueInteger();
	}

	SgStatement *finish = loop->childList1(count - 1);
	SgStatement *temp = loop->childList1(0);
	while(true)
	{
		switch(temp->variant())
		{
		case ASSIGN_STAT:
			if(DEBUG)
				out << " ASSIGN_STAT in line: " << temp->lineNumber() << " ";
			q = AnalizeAssignStat(temp->expr(0),q,0, step);
			q = AnalizeAssignStat(temp->expr(1),q,1, step);
			if(DEBUG)
				out << " success" << endl;
			break;
		case IF_NODE:
			if(DEBUG)
				out << " IF_NODE condition in line: " << temp->lineNumber() << " ";
			q = AnalizeAssignStat(temp->expr(0),q,1, step);
			if(DEBUG)
				out << " success" << endl;
		case ELSEIF_NODE:
			if(DEBUG)
				out << " ELSEIF_NODE condition in line: " << temp->lineNumber() << " ";
			q = AnalizeAssignStat(temp->expr(0),q,1, step);
			if(DEBUG)
				out << " success" << endl;
			break;
		case ARITHIF_NODE:
			if(DEBUG)
				out << " ARITHIF_NODE condition in line: " << temp->lineNumber() << " ";
			q = AnalizeAssignStat(temp->expr(0),q,1, step);
			if(DEBUG)
				out << " success" << endl;
			break;
		case LOGIF_NODE:
			if(DEBUG)
				out << " LOGIF_NODE condition in line: " << temp->lineNumber() << " ";
			q = AnalizeAssignStat(temp->expr(0),q,1, step);
			if(DEBUG)
				out << " success" << endl;
			break;
		case PROC_STAT:
			{
				if(DEBUG)
					out << " PROC_STAT in line: " << temp->lineNumber() << " ";
				SgExpression *ex = temp->expr(0);
				while (ex)
				{
					q = AnalizeAssignStat(ex->lhs(),q,1, step);
					ex = ex->rhs();
				}
					if(DEBUG)
						out << " success" << endl;
				break;
			}
			
		default:
			/*if(DEBUG)
				out << " unknown op in line: " << temp->lineNumber() << " " << temp->variant() << endl;*/
			break;
		}
				
		if (temp == finish)
			break;
		temp = temp->lexNext();		
	}
	return AllRef->next;
}

void SwapFors( SgForStmt *first, SgForStmt *second)
{
	SgExpression *oldStart;
	SgExpression *oldfin;
	SgExpression *oldStep;
	SgSymbol *oldSymb;
	SgValueExp *one = new SgValueExp(1);

	oldStart = first->start();
	oldfin = first->end();
	oldStep = first->step();
	
	first->setStart(*second->start());
	first->setEnd(*second->end());
	if(second->step())
		first->setStep(*second->step());
	else
		first->setStep(*one);

	second->setStart(*oldStart);
	second->setEnd(*oldfin);
	if(oldStep)
		second->setStep(*oldStep);
	else
		second->setStep(*one);

	oldSymb = first->symbol();
	first->setDoName(*second->symbol());
	second->setDoName(*oldSymb);
}

int AnalizeLoopNest(SgForStmt *stmt, int d)
{
	int ret_val = d;
	int ch_list = stmt->numberOfChildrenList1();

	if(ch_list == 1)
	{
		if (isSgForStmt(stmt->childList1(0)))
		{
			ret_val = AnalizeLoopNest(isSgForStmt(stmt->childList1(0)), ret_val + 1);
		}
	}
	else if(ch_list == 2)
	{
		if (isSgForStmt(stmt->childList1(0)) && stmt->childList1(1)->variant() == CONTROL_END)
		{
			ret_val = AnalizeLoopNest(isSgForStmt(stmt->childList1(0)), ret_val + 1);
		}
	}

	return ret_val;
}

void SearchInnerFor(SgForStmt *in, int dep, SageStOp *parent)
{
	SgStatement *temp;
	SgForStmt *loop;
	SageStOp *FirstNode;
	SageStOp *p;
	bool flag = true;
	for(int i = 0; i < in->numberOfChildrenList1(); ++i)
	{
		SageStOp *find = new SageStOp();
		temp = in->childList1(i);
		if (loop = isSgForStmt(temp))
		{
			loop->convertLoop();
			parent->count_inner_loops++;

			find->depth = dep;
			find->line_code = loop->lineNumber();
			find->loop_op = loop;
			find->numChList1 = loop->numberOfChildrenList1();
			find->numChList2 = loop->numberOfChildrenList2();
			find->count_inner_loops = 0;
			find->inner_loops = NULL;
			find->LoopNest = AnalizeLoopNest(loop,1);

			if (flag)
			{
				FirstNode = find;
				p = find;
				flag = !flag;
			} 
			else
			{
				p->next = find;
				p = find;				
			}
			
			if(DEBUG)
			{
				for(int k = 0; k < dep; k ++)
					out << "  ";
				out << "found loop in " << loop->lineNumber() << " line with " << loop->numberOfChildrenList1() << " inner ops\n";
			}
			
			SearchInnerFor(loop, dep + 1, find);
		}		
	}
	if (parent->count_inner_loops != 0)
	{
		parent->inner_loops = FirstNode;
	}
	else
	{
		parent->inner_loops = NULL;
	}	
}

SageStOp* ForSearch(SgStatement *start)
{
	SgForStmt *loop;
	SgStatement *op = start;
	int count_of_parent = 0;
	SageStOp *AllForLoops;
	SageStOp *q = NULL;
	while(op)
	{
		SageStOp *p = new SageStOp();
		if(loop = isSgForStmt(op))
		{
			loop->convertLoop();		
			
			count_of_parent++;
			p->depth = 0;
			p->count_inner_loops = 0;
			p->line_code = loop->lineNumber();
			p->loop_op = loop;
			p->numChList1 = loop->numberOfChildrenList1();
			p->numChList2 = loop->numberOfChildrenList2();
			p->LoopNest = AnalizeLoopNest(loop,1);

			if (q == NULL)
			{
				AllForLoops = p;
				q = p;
			} 
			else
			{
				q->next = p;
				q = p;
			}

			if(DEBUG)
				out << "found parent loop in " << loop->lineNumber() << " line with " << loop->numberOfChildrenList1() << " inner ops\n";
			SearchInnerFor(loop, 1, p);
			op = loop->childList1(loop->numberOfChildrenList1() - 1);	
		}
		op = op->lexNext();
	}
	return AllForLoops;
}

bool EqualSageSymbols(SageSymbols *first, SageSymbols *second)
{
	SageSymbols *q = first;
	SageSymbols *p = second;
	int count_f = 0;
	int count_s = 0;
	bool ret_val = false;

	while (q)
	{
		count_f++;
		q = q->next;
	}
	while (p)
	{
		count_s++;
		p = p->next;
	}
	if(count_f == count_s)
	{
		q = first;
		p = second; 
		ret_val = true;
		for (int i = 0; i < count_f; ++i)
		{
			if(p->symb && q->symb)
			{
				if(p->symb->id() != q->symb->id())
				{
					ret_val = false;
					break;
				}
			}
			else
			{
				ret_val = false;
				break;
			}
			p = p->next;
			q = q->next;
		}
	}
	
	return ret_val;
}

bool CheckExist(SageSymbols *templateParallel, SageSymbols *oneElem)
{
	bool ret_val = false;
	SageSymbols *q = templateParallel;
	while (q)
	{
		if(oneElem->symb != NULL)
			if (oneElem->symb->id() == q->symb->id())
			{
				ret_val = true;
				break;
			}
		q = q->next;
	}
	return ret_val;
}

bool CheckExist(Templates *t, SageSymbols *oneElem)
{
	bool ret_val = false;
	Templates *q = t;
	while (q)
	{
		if (EqualSageSymbols(q->first,oneElem))
		{
			q->count++;
			ret_val = true;
			break;
		}
		q = q->next;
	}
	return ret_val;
}

SageSymbols* CreateNeededOrder(SageSymbols *templateParallel, SageSymbols *ArrayOrder)
{
	int templ = 0;
	int arr = 0;
	SageSymbols *q = templateParallel;
	SageSymbols *ret_val;
	while (q)
	{
		templ++;
		q = q->next;
	}
	q = ArrayOrder;
	while (q)
	{
		arr++;
		q = q->next;
	}

	q = new SageSymbols();
	SageSymbols *temp = ArrayOrder;
	ret_val = q;
	for(int i = 0; i < arr; ++i)
	{
		if(CheckExist(templateParallel,temp))
		{
			q->next = new SageSymbols();
			q = q->next;
			q->len = temp->len;
			q->symb = temp->symb;
			q->next = NULL;
		}
		temp = temp->next;
	}
	return ret_val->next;
}

SageSymbols* CheckOrder(SageSymbols *templateParallel, SageSymbols *ArrayOrder)
{
	SageSymbols *q_t = templateParallel;
	SageSymbols *q_a = ArrayOrder;
	SageSymbols *neededOrder = new SageSymbols();
	neededOrder->len = 0;
	neededOrder->next = NULL;
	neededOrder->symb = NULL;

	int find_idx = 0;
	int all_idx = 0;

	while (q_t)
	{
		all_idx++;
		q_t = q_t->next;
	}

	while (q_a)
	{
		q_t = templateParallel;
		while (q_t)
		{
			if(q_a->symb)
				if (q_a->symb->id() == q_t->symb->id())
					find_idx++;
			q_t = q_t->next;
		}
		q_a = q_a->next;
	}
	if(find_idx != all_idx)
		neededOrder->len = -1;
	else
	{
		q_t = templateParallel;
		q_a = ArrayOrder;
		while (q_a)
		{
			if(q_a->symb)
				if (q_a->symb->id() == q_t->symb->id())
				{
					if (q_t->next)
						q_t = q_t->next;
					else	
					{
						q_t = NULL;
						break;
					}
				}
			q_a = q_a->next;
		}
		if (q_t == NULL)	
			neededOrder->len = 1;
		else			
		{
			delete neededOrder;
			neededOrder = CreateNeededOrder(templateParallel, ArrayOrder);
			neededOrder->len = 0;
		}
	}
	
	return neededOrder;
}

int compare (const void * first, const void * second)
{
	SageSymbols *p = (SageSymbols*)first;
	SageSymbols *q = (SageSymbols*)second;
	if(p->len > q->len)
		return -1;
	else if(p->len < q->len)
		return 1;
	else
		return 0;
}

SageSymbols* CreateOrderForArray( SageSymbols **Idxs, int count, int *dims)
{
	bool flag = true;
	SageSymbols *retval = NULL;

	// check for null var idxs in Array Ref
	/*for(int i = 0; i < count; ++i)
	{
		if(!Idxs[i]->symb)
		{
			for (int k = 0; k < count; ++k)
			{	
				if(Idxs[k]->symb)
					cerr << Idxs[k]->symb->identifier() << " ";
				else
					cerr << " NULL ";
			}
			cerr << endl;
			flag = false;
			break;
		}
	}*/
	if (flag)
	{
		int k = dims[0];
		SageSymbols *sortArr = new SageSymbols[count];
		for(int i = 1; i < count; ++i)
		{
			Idxs[i]->len = Idxs[i]->len * k;
			k *= dims[i];			
		}

		for(int i = 0; i < count; ++i)
		{
			sortArr[i] = *Idxs[i];
		}
		qsort(sortArr, count, sizeof(SageSymbols),compare);
				
		SageSymbols *p = new SageSymbols();
		retval = p;

		for(int i = 0; i < count; ++i)
		{
			SageSymbols *q = new SageSymbols();
			q->len = sortArr[i].len;
			q->next = NULL;
			q->symb = sortArr[i].symb;
			p->next = q;
			p = q;		
		}

		retval = retval->next;
	}

	return retval;
}

int* GetIdxOfArrRef(SgExpression *expr)
{
	int *ret_idx = NULL;
	int dim;
	SgArrayType *arrayt;

	if(arrayt = isSgArrayType(expr->symbol()->type()))
	{
		dim = arrayt->dimension();
		if (DEBUG_LV3)
			out << " dim = " << dim << endl;
		ret_idx = new int[dim];
		for(int i = 0; i < dim; i++) 
		{  
			if(arrayt->sizeInDim(i))
				if (arrayt->sizeInDim(i)->variant() == STAR_RANGE)
				{
					if (DEBUG)
						out << " Warning: array in place " << i << " has * dimention size - replaced by 100 dim" << endl;
					ret_idx[i] = 100;
					continue;
				}
			ret_idx[i] = CountExpr(arrayt->sizeInDim(i), 0);
			if(ret_idx[i] == 0)
			{
				if (DEBUG)
					out << " unknown value in dimention in place " << i << " with variant " << arrayt->sizeInDim(i)->variant() << endl;
				delete []ret_idx;
				ret_idx = NULL;
				break;
			}
			if (DEBUG_LV3)
				out << " size in " << i << " dim is " << ret_idx[i] << endl;
		}
	}
	else
	{
		out << "unknown type is " << expr->symbol()->variant() << endl;
	}
	return ret_idx;
}

void PrintFindTemplates(Templates *t, bool flag)
{
	if (flag)
	{
		while(t)
		{
			if(t->first)
			{
				out << " template ( ";
				SageSymbols*p = t->first;
				while(p)
				{
					if(p->symb)
						out << p->symb->identifier() << " ";
					else
						out << " NULL ";
					p = p->next;
				}
				out << "): ";
				if (t->read_write == 1)
					out << "read = " << t->count << " write = " << t->count_write_read << endl;
				else
					out << "write = " << t->count << " read = " << t->count_write_read << endl;
			}
			else
			{
				if (t->read_write == 1)
					out << " unknown template: read = " << t->count << " write = " << t->count_write_read << endl;
				else
					out << " unknown template: write = " << t->count << " read = " << t->count_write_read << endl;
			}
			t = t->next;
		}
	}
	else
	{
		if(t->read_write == 0)
		{
			while(t)
			{
				if(t->first)
				{
					out << " write template ( ";
					SageSymbols*p = t->first;
					while(p)
					{
						if(p->symb)
							out << p->symb->identifier()<< " ";
						else
							out << " NULL ";
						p = p->next;
					}
					out << ") = " << t->count << endl;
				}
				else
				{
					out << " write unknown " << t->count << endl;
				}
				t = t->next;
			}
		}
		else
		{
			while(t)
			{
				if(t->first)
				{
					out << " read template ( ";
					SageSymbols*p = t->first;
					while(p)
					{
						if(p->symb)
							out << p->symb->identifier()<< " ";
						else
							out << " NULL ";
						p = p->next;
					}
					out << ") = " << t->count << endl;
				}
				else
				{
					out << " read unknown " << t->count << endl;
				}
				t = t->next;
			}
		}
	}
}

Templates* CopyAllTempl(Templates* copy_t)
{
	Templates *t = copy_t;
	Templates *p = new Templates();
	Templates *ret;
	ret = p;
	while(t)
	{	
		p->next = new Templates();
		p = p->next;
		p->count = t->count;
		p->first = t->first;
		p->count_write_read = t->count_write_read;
		p->read_write = t->read_write;
		t = t->next;
	}

	return ret->next;
}

Templates* mergeTemplates(Templates *first,Templates *second)
{
	Templates *fQ = CopyAllTempl(first);
	Templates *sQ = CopyAllTempl(second);
	Templates *retval = new Templates();
	Templates *temp;

	retval->count = fQ->count;
	retval->first = fQ->first;
	retval->read_write = fQ->read_write;
	retval->next = new Templates();
	retval->count_write_read = sQ->count;
	temp = retval->next;
	fQ = fQ->next;
	sQ = sQ->next;
	temp->count = fQ->count;
	temp->first = fQ->first;
	temp->read_write = fQ->read_write;
	temp->count_write_read = sQ->count;
	fQ = fQ->next;
	sQ = sQ->next;
	
	Templates *t = fQ;
	while(t)
	{
		temp->next = new Templates();
		temp = temp->next;
		temp->count = t->count;
		temp->first = t->first;
		temp->read_write = t->read_write;
		temp->count_write_read = 0;
		Templates *m = sQ;
		Templates *del_m = sQ;
		bool f = FALSE;
		while(m)
		{
			if (EqualSageSymbols(t->first,m->first))
			{
				temp->count_write_read = m->count;
				if(f)
					del_m->next = m->next;
				else
					sQ = NULL;
				break;
			}
			if (f)
				del_m = del_m->next;
			else
				f = TRUE;
			m = m->next;
		}
		t = t->next;
	}
	while(sQ)
	{
		temp->next = new Templates();
		temp = temp->next;
		temp->count = sQ->count;
		temp->first = sQ->first;
		temp->read_write = sQ->read_write;
		temp->count_write_read = 0;
		sQ = sQ->next;
	}
	temp->next = NULL;
	return retval;
}

bool SwapIfNeed(Templates *merge, SgForStmt *loop, SgExpression *DVM_par)
{
	bool swap_ret = false;
	int count_swap_op = 0;
	Templates *swap = NULL;

	Templates *t = merge->next->next;
	int max = merge->count + merge->count_write_read;
	while(t)
	{
		if(max < t->count + t->count_write_read)
		{
			max = t->count + t->count_write_read;
			swap = t;
		}
		t = t->next;
	}

	if (swap != NULL)
	{
		swap_ret = true;
		out << " need swap for ( ";
		for(SageSymbols *t = swap->first;t != NULL; t = t->next)
		{
			out << t->symb->identifier() << " ";
		}
		out << ") template\n";

		SageSymbols *t = swap->first;
		SgExpression *e = DVM_par;
		int count = 0;
		while(t)
		{
			e->lhs()->setSymbol(t->symb);
			e = e->rhs();
			t = t->next;
			count++;
		}

		SgForStmt **swapFor = new SgForStmt*[count];
		SgForStmt *forL = loop;
		count = 0;
		while(forL)
		{
			t = swap->first;
			while(t)
			{
				if(forL->symbol()->id() == t->symb->id())
				{
					swapFor[count] = forL;
					count++;
					break;
				}
				t = t->next;
			}
			forL = isSgForStmt(forL->getNextLoop());
		}

		t = swap->first;
		for (int i = 0; i < count; ++i)
		{			
			if(swapFor[i]->symbol()->id() != t->symb->id())
			{
				int s = i + 1;
				for( ;s < count ;s++)
				{
					if(swapFor[s]->symbol()->id() == t->symb->id())
						break;
				}
				SwapFors(swapFor[i], swapFor[s]);
			}
			t = t->next;
		}
	}
	else
	{
		out << " not swap need " << endl;
	}
	return swap_ret;
}
bool SwapIfNeed(Templates *read, Templates *write, SgForStmt *loop, SgExpression *DVM_par)
{
	bool swap_ret = false;
	int count_read = 0;
	int count_write = 0;
	Templates *swapR = NULL;
	Templates *swapW = NULL;

	Templates *t = read->next->next;
	int max = read->count;
	while(t)
	{
		if(max < t->count)
		{
			max = t->count;
			swapR = t;
		}
		t = t->next;
	}
	if(max < write->count)
	{
		max = write->count;
		swapR = NULL;
	}
	t = write->next->next;
	while(t)
	{
		if(max < t->count)
		{
			max = t->count;
			swapW = t;
		}
		t = t->next;
	}
	if (swapR != NULL)
	{
		swap_ret = true;
		out << " need swap for read " << endl;
		SageSymbols *t = swapR->first;
		SgExpression *e = DVM_par;
		int count = 0;
		while(t)
		{
			e->lhs()->setSymbol(t->symb);
			e = e->rhs();
			t = t->next;
			count++;
		}

		SgForStmt **swapFor = new SgForStmt*[count];
		SgForStmt *forL = loop;
		count = 0;
		while(forL)
		{
			t = swapR->first;
			while(t)
			{
				if(forL->symbol()->id() == t->symb->id())
				{
					swapFor[count] = forL;
					count++;
					break;
				}
				t = t->next;
			}
			forL = isSgForStmt(forL->getNextLoop());
		}

		t = swapR->first;
		for (int i = 0; i < count; ++i)
		{			
			if(swapFor[i]->symbol()->id() != t->symb->id())
			{
				int s = i + 1;
				for( ;s < count ;s++)
				{
					if(swapFor[s]->symbol()->id() == t->symb->id())
						break;
				}
				SwapFors(swapFor[i], swapFor[s]);
			}
			t = t->next;
		}
	}
	else if (swapW != NULL)
	{
		swap_ret = true;
		out << " need swap for write " << endl;
		SageSymbols *t = swapW->first;
		SgExpression *e = DVM_par;
		int count = 0;
		while(t)
		{
			e->lhs()->setSymbol(t->symb);
			e = e->rhs();
			t = t->next;
			count++;
		}

		SgForStmt **swapFor = new SgForStmt*[count];
		SgForStmt *forL = loop;
		count = 0;
		while(forL)
		{
			t = swapW->first;
			while(t)
			{
				if(forL->symbol()->id() == t->symb->id())
				{
					swapFor[count] = forL;
					count++;
					break;
				}
				t = t->next;
			}			
			forL = isSgForStmt(forL->getNextLoop());
		}

		t = swapW->first;
		for (int i = 0; i < count; ++i)
		{
			if(swapFor[i]->symbol()->id() != t->symb->id())
			{
				int s = i + 1;
				for( ;s < count; s++)
				{
					if(swapFor[s]->symbol()->id() == t->symb->id())
						break;
				}
				SwapFors(swapFor[i], swapFor[s]);
			}
			t = t->next;
		}
	}
	else
	{
		out << " not swap need " << endl;
	}
	return swap_ret;
}

int allSwaps = 0;
void AnalizeAllFors(SageStOp *first)
{
	for (SageStOp *p = first; p != NULL; p = p->next)
	{
		if (p->LoopNest > 1)
		{
			SgStatement *temp = p->loop_op->lexPrev();
			
			if (temp->variant() == DVM_PARALLEL_ON_DIR)
			{
				// create template from parallel directive
				SageSymbols *templateParalell;
				SageSymbols *p_t = new SageSymbols();
				templateParalell = p_t;

				SgExpression *first = temp->expr(2);
				while(first)
				{
					SageSymbols *q = new SageSymbols();
					q->len = -1;
					q->next = NULL;
					q->symb = first->lhs()->symbol();
					p_t->next = q;
					p_t = q;
					first = first->rhs();
				}
				//p_t = templateParalell;
				templateParalell = templateParalell->next;
				//p_t->next = NULL;
				//delete p_t;

				Templates *R_temp = new Templates();
				Templates *W_temp = new Templates();
				Templates *ReadOper = R_temp;
				Templates *WriteOper = W_temp;
				
				ReadOper->count = 0;
				ReadOper->first = templateParalell;
				ReadOper->next = new Templates();
				ReadOper->read_write = 1;
				ReadOper->next->count = 0;
				ReadOper->next->first = NULL;
				ReadOper->next->next = NULL;
				ReadOper->next->read_write = 1;
				ReadOper->count_write_read = 0;

				WriteOper->count = 0;
				WriteOper->first = templateParalell;
				WriteOper->next = new Templates();
				WriteOper->read_write = 0;
				WriteOper->next->first = NULL;
				WriteOper->next->next = NULL;
				WriteOper->next->read_write = 0;
				WriteOper->count_write_read = 0;

				R_temp = R_temp->next;
				W_temp = W_temp->next;

				SageStOp *q = p;
				for(int i = 0; i < p->LoopNest - 1; ++i)
				{
					q = q->inner_loops;
				}
				out << " \nAnalize LoopNest in line " << p->loop_op->lineNumber() << endl;
				SageArrayIdxs *AllArrayRefInLoop;					
				AllArrayRefInLoop = Analize(q->loop_op,q->numChList1);
				
				if(DEBUG_LV2)
					out << " Analize func OK\n";
				while (AllArrayRefInLoop)
				{
					bool flag;
					if(AllArrayRefInLoop->dim)
						flag = true;
					else
						flag = false;
					for(int i = 0; i < AllArrayRefInLoop->dim; ++i)
					{
						//out << " len " << AllArrayRefInLoop->symb[i]->len << " for idx " << AllArrayRefInLoop->symb[i]->symb->identifier();
						if (AllArrayRefInLoop->symb[i]->len == -1)
						{
							flag = !flag;
							break;
						}
					}
					if (flag)
					{		
						SageSymbols *params, *neededOrder;
						bool flag_null = false;
						if(DEBUG_LV2)
							out << "before GetIdxOfArrRef func\n";
						int *dims = GetIdxOfArrRef(AllArrayRefInLoop->array_expr);
						if(DEBUG_LV2)
							out << "after GetIdxOfArrRef func\n before CreateOrderForArray func\n";
						if (dims)
						{
							params = CreateOrderForArray(AllArrayRefInLoop->symb, AllArrayRefInLoop->dim, dims);
							if(DEBUG_LV2)
								out << "after CreateOrderForArray func\n before CheckOrder func\n";
							if (params)
							{
								neededOrder = CheckOrder(templateParalell,params);
								if(DEBUG_LV2)
									out << "after CheckOrder func\n";
							}
							else							
								flag_null = true;							
						}
						else
							flag_null = true;
						
						if(flag_null)
						{
							neededOrder = new SageSymbols();
							neededOrder->len = -1;
							neededOrder->next = NULL;
							neededOrder->symb = NULL;
						}
						//out << " :rez = " << neededOrder->len;
						switch(neededOrder->len)
						{
						case 0:
							if(AllArrayRefInLoop->read_write)
							{
								if(!CheckExist(ReadOper, neededOrder))
								{
									R_temp->next = new Templates();
									R_temp = R_temp->next;
									R_temp->count = 1;
									R_temp->first = neededOrder;
									R_temp->next = NULL;
									R_temp->read_write = 1;
									R_temp->count_write_read = 0;
								}
							}
							else
								if(!CheckExist(WriteOper, neededOrder))
								{
									W_temp->next = new Templates();
									W_temp = W_temp->next;
									W_temp->count = 1;
									W_temp->first = neededOrder;
									W_temp->next = NULL;
									W_temp->read_write = 0;
									W_temp->count_write_read = 0;
								}
							break;
						case 1:
							if(AllArrayRefInLoop->read_write)
								ReadOper->count++;
							else
								WriteOper->count++;
							break;
						case -1:
							if(AllArrayRefInLoop->read_write)
								ReadOper->next->count++;
							else
								WriteOper->next->count++;
							break;
						default:
							if(DEBUG)
								out << " unknown case in switch-template\n";
							break;
						}
					}
					else
					{
						if(AllArrayRefInLoop->read_write)
							ReadOper->next->count++;
						else
							WriteOper->next->count++;
					}
					//out << endl;					
					AllArrayRefInLoop = AllArrayRefInLoop->next;
										
				}
				Templates *MerT = mergeTemplates(ReadOper, WriteOper);
				if(DEBUG)
				{
					PrintFindTemplates(MerT, true);
					//PrintFindTemplates(ReadOper, false);
					//PrintFindTemplates(WriteOper, false);
				}
				if(DEBUG_LV2)
					out << " before SwapIfNeed func\n";
				if(SwapIfNeed(MerT, p->loop_op, temp->expr(2)))
					allSwaps++;
				if(DEBUG_LV2)
					out << " after SwapIfNeed func\n";
			}
		}
		
		if (p->inner_loops != NULL)
		{
			AnalizeAllFors(p->inner_loops);
		}		
	}	
}

void Print(SageStOp *first)
{
	for (SageStOp *p = first; p != NULL; p = p->next)
	{
		out << "for in line " << p->line_code << " has " << p->LoopNest << " LoopNest\n";
		//out << "   count " << p->count_inner_loops << endl; 
		if (p->inner_loops != NULL)
		{
			Print(p->inner_loops);
		}
	}	
}

SgExpression* findDirect(SgExpression *inExpr, int DIR)
{
	SgExpression *temp = NULL;
	if (inExpr)
	{
		if (inExpr->variant() == DIR)
		{
			return inExpr;
		}
		else
		{
			if (inExpr->lhs())
				temp = findDirect(inExpr->lhs(), DIR);
			if(inExpr->rhs() && temp == NULL)
				temp = findDirect(inExpr->rhs(), DIR);
		}
	}
	return temp;
}
void GlobalAnalizeFunctionOfSwapFors(SageStOp *AllFors)
{	
	AnalizeAllFors(AllFors);
	cerr << " ALL Swaps = " << allSwaps << endl;
}

SageArrayIdxs* GetIdxInParDir(SgExpression *on, SgExpression *across)
{
	SageArrayIdxs *ret = new SageArrayIdxs();
	ret->next = NULL;
	ret->array_expr = NULL;
	ret->read_write = -1;
	ret->dim = 0;
	
	SgExpression *temp = on;
	while (temp)
	{
		ret->dim++;
		temp = temp->rhs();
	}

	ret->symb = new SageSymbols*[ret->dim];
	temp = on;
	for (int i = 0; i < ret->dim; ++i)
	{
		ret->symb[i] = new SageSymbols();
		ret->symb[i]->symb = temp->lhs()->symbol();
		ret->symb[i]->across_left = 0;
		ret->symb[i]->across_right = 0;
		ret->symb[i]->next = NULL;
		temp = temp->rhs();
	}
	
	while (across)
	{
		SgExpression *t = across->lhs();
		int dim = 0;
		if(t->variant() == ARRAY_REF)
			t = t->lhs();
		else if(t->variant() == ARRAY_OP)
			t = t->lhs()->lhs();
		else
		{
			if(DEBUG)
				out << "!!! unknown vatiant in ACROSS dir: " << t->variant() << endl;
		}

		while (t)
		{
			ret->symb[dim]->across_left = MAX(ret->symb[dim]->across_left, t->lhs()->lhs()->valueInteger());
			ret->symb[dim]->across_right = MAX(ret->symb[dim]->across_right, t->lhs()->rhs()->valueInteger());
			dim ++;
			t = t->rhs();
		}
		across = across->rhs();
	}

	return ret;
}

void GlobalAnalizeFunction(SgFile *file)
{
	out << endl << fin_name << endl;

	SgStatement *op = file->firstStatement();
	SageStOp *AllFors = ForSearch(file->firstStatement()->lexNext());
	int par_loops = 0;
	for(SageStOp *p = AllFors; p != NULL; p = p->next)
	{
		par_loops++;
	}			
	if(DEBUG)
	{
		out << "\n ***** FOUND ***** " << par_loops << " parent fors\n\n";
		//Print(AllFors);
	}
	GlobalAnalizeFunctionOfSwapFors(AllFors);
}

char* OnlyName(char *filename)
{  
	char *basename;
	int i;
	basename = new char[strlen(filename) + 4];

	strcpy (basename,filename);
	for (i = strlen(filename) - 1 ; i >= 0 ; --i)
	{
		if ( basename[i] == '.' )
		{
			basename[i] ='\0';
			break;
		}
	}

	return(basename); 
}

int main(int argc, char *argv[])
{ 
	FILE *fout;
	char fout_name[80];
	char *db_name = NULL;
	char *proj_name = "dvm.proj";
	int i,n, i_file;
	SgFile *file;

	argv ++;
	if(argc > 1)
	{  
		proj_name = *argv;
		db_name = OnlyName(proj_name);
	}
	
	SgProject project(proj_name);
	fin_name = new char[80];
	n = project.numberOfFiles();
	cerr << "number of files in project " << n << endl;
	fin_name = project.fileName(n - 1); 

	// looking through the file list of project (second time)
	for(i = n - 1; i >= 0; --i)
	{
		file = &(project.file(i));  
		current_file = file;   // global variable 
		i_file = i;
		fin_name = project.fileName(i);   
		cerr <<  "Analyzing: " << fin_name << endl;
		GlobalAnalizeFunction(file);
		sprintf(fout_name,"%s.f",OnlyName(fin_name));
		file->saveDepFile(strcat(OnlyName(fin_name),".dep"));
		cerr << "Saving in file: " << strcat(OnlyName(fin_name),".dep") << endl;
		//unparsing into file
		if(DEBUG_LV2)
		{
			if((fout = fopen(fout_name,"w")) == NULL) 
			{
				cerr << "Can't open file " << fout_name << " for write\n";
				return 1;
			}   
			cerr << "Unparsing: " << fout_name << endl;
			file->unparse(fout);  
			if((fclose(fout)) < 0) 
			{
				cerr << "Could not close " << fout_name << endl;
				return 1;
			}
		}
	}  
	cerr << "All work done successfully!\n";
	getchar();
	return 0;	
}
