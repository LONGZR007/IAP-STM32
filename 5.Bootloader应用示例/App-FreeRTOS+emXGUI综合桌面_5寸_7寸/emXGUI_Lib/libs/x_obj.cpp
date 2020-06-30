/*
 * x_obj.c
 *
 *  Created on: 2016��10��6��
 *      Author: Administrator
 */

#include <stddef.h>
#include <string.h>
#include "x_obj.h"
#include "x_libc.h"




 /*===================================================================================*/

static unsigned int __abs(int a)
{
	if (a < 0)
	{
		return -a;
	}
	return a;
}

/*===================================================================================*/

#if 1

extern "C"
{

	void*	GUI_MEM_Alloc(U32 size);
	void	GUI_MEM_Free(void *p);

}

void* operator new(size_t size)
{
	return (void*)GUI_MEM_Alloc(size);
}


void operator delete(void *p)
{
	GUI_MEM_Free(p);
}

////

void* operator new[](size_t size)
{
	return (void*)GUI_MEM_Alloc(size);
}

void operator delete[](void *p)
{
	GUI_MEM_Free(p);
}

#endif

/*===================================================================================*/

struct __x_obj_item *x_obj_create_class(const WCHAR *pText, u32 id, const RECT *prc, u32 flag, u32 cbExtra, fn_obj_draw *pfn_draw)
{
	struct __x_obj_item *obj;

	int i;

	i = sizeof(struct __x_obj_item) + cbExtra;

	obj = (struct __x_obj_item*)new u8[i];
	if (obj != NULL)
	{
		list_init(&obj->node);
		obj->rc = *prc;
		obj->flag = flag;
		obj->id = id;

		i = x_wstrlen(pText) + 1;
		obj->pszText = new WCHAR[i];
		if (pText != NULL)
		{
			x_wstrcpy(obj->pszText, pText);
		}

		obj->parent = NULL;
		obj->pfn_draw = pfn_draw;

	}
	return obj;
}

struct __x_obj_item *x_obj_create(const WCHAR *pText, u32 id, const RECT *prc, u32 flag, u32 cbExtra, struct __x_obj_item *parent)
{
	struct __x_obj_item *obj;

	int i;

	i = sizeof(struct __x_obj_item) + cbExtra;

	obj = (struct __x_obj_item*)new u8[i];
	if (obj != NULL)
	{
		list_init(&obj->node);
		obj->rc = *prc;
		obj->flag = flag;
		obj->id = id;

		i = x_wstrlen(pText) + 1;
		obj->pszText = new WCHAR[i];
		if (pText != NULL)
		{
			x_wstrcpy(obj->pszText, pText);
		}

		obj->pfn_draw = parent->pfn_draw;
		obj->parent = parent;
		if (parent != NULL)
		{
			list_add_tail(&obj->node, &parent->node);
		}
	}
	return obj;
}
/*===================================================================================*/

struct __x_obj_item *x_obj_get_first(struct __x_obj_item *parent)
{
	struct __x_obj_item *obj = NULL;
	list_t *lst, *n;

	if (parent != NULL)
	{
		lst = &parent->node;
		n = lst->next;
		if (n != lst)
		{
			obj = (struct __x_obj_item*)list_entry(n, struct __x_obj_item, node);
		}

	}
	return obj;
}

struct __x_obj_item *x_obj_get_last(struct __x_obj_item *parent)
{
	struct __x_obj_item *obj = NULL;
	list_t *lst, *n;

	if (parent != NULL)
	{
		lst = &parent->node;
		n = lst->prev;
		if (n != lst)
		{
			obj = (struct __x_obj_item*)list_entry(n, struct __x_obj_item, node);
		}

	}
	return obj;
}

struct __x_obj_item *x_obj_get_prev(struct __x_obj_item *obj)
{
	list_t *lst, *n;
	struct __x_obj_item *parent;

	if (obj != NULL)
	{
		parent = obj->parent;

		lst = &obj->node;
		n = lst->prev;
		if (n != lst)
		{
			obj = (struct __x_obj_item*)list_entry(n, struct __x_obj_item, node);
			if (obj != parent)
			{
				return obj;
			}
		}

	}
	return NULL;

}

struct __x_obj_item *x_obj_get_next(struct __x_obj_item *obj)
{
	list_t *lst, *n;
	struct __x_obj_item *parent;

	if (obj != NULL)
	{
		parent = obj->parent;

		lst = &obj->node;
		n = lst->next;
		if (n != lst)
		{
			obj = (struct __x_obj_item*)list_entry(n, struct __x_obj_item, node);
			if (obj != parent)
			{
				return obj;
			}
		}

	}
	return NULL;

}

/*===================================================================================*/

struct __x_obj_item *x_obj_get_left(struct __x_obj_item *obj)
{

	struct __x_obj_item *obj0, *obj_find = NULL;
	RECT rc;

	if (obj != NULL)
	{
		rc = obj->rc;

		obj0 = x_obj_get_first(obj->parent);

		while (obj0 != NULL)
		{
			if (obj0 != obj)
				if (obj0->rc.x < rc.x)
				{
					if (obj_find == NULL)
					{
						obj_find = obj0;
					}
					else
					{
						int obj0_d;
						int obj_find_d;

						obj0_d = __abs(obj0->rc.x - rc.x) + __abs(obj0->rc.y - rc.y);
						obj_find_d = __abs(obj_find->rc.x - rc.x) + __abs(obj_find->rc.y - rc.y);

						if (obj0_d < obj_find_d)
						{
							obj_find = obj0;
						}
					}
				}

			obj0 = x_obj_get_next(obj0);
		}

		if (obj_find == NULL)
		{
			//���û����
			obj_find = obj;
		}
	}
	return obj_find;
}

struct __x_obj_item *x_obj_get_right(struct __x_obj_item *obj)
{

	struct __x_obj_item *obj0, *obj_find = NULL;
	RECT rc;

	if (obj != NULL)
	{
		rc = obj->rc;

		obj0 = x_obj_get_first(obj->parent);

		while (obj0 != NULL)
		{
			if (obj0 != obj)
				if (obj0->rc.x > rc.x)
				{
					if (obj_find == NULL)
					{
						obj_find = obj0;
					}
					else
					{
						int obj0_d;
						int obj_find_d;

						obj0_d = __abs(obj0->rc.x - rc.x) + __abs(obj0->rc.y - rc.y);
						obj_find_d = __abs(obj_find->rc.x - rc.x) + __abs(obj_find->rc.y - rc.y);

						if (obj0_d < obj_find_d)
						{
							obj_find = obj0;
						}
					}
				}

			obj0 = x_obj_get_next(obj0);
		}

		if (obj_find == NULL)
		{
			//�ұ�û����
			obj_find = obj;
		}
	}
	return obj_find;
}

struct __x_obj_item *x_obj_get_up(struct __x_obj_item *obj)
{

	struct __x_obj_item *obj0, *obj_find = NULL;
	RECT rc;

	if (obj != NULL)
	{
		rc = obj->rc;

		obj0 = x_obj_get_first(obj->parent);

		while (obj0 != NULL)
		{
			if (obj0 != obj)
				if (obj0->rc.y < rc.y)
				{
					if (obj_find == NULL)
					{
						obj_find = obj0;
					}
					else
					{
						int obj0_d;
						int obj_find_d;

						obj0_d = __abs(obj0->rc.x - rc.x) + __abs(obj0->rc.y - rc.y);
						obj_find_d = __abs(obj_find->rc.x - rc.x) + __abs(obj_find->rc.y - rc.y);

						if (obj0_d < obj_find_d)
						{
							obj_find = obj0;
						}
					}
				}

			obj0 = x_obj_get_next(obj0);
		}

		if (obj_find == NULL)
		{
			//�ϱ�û����
			obj_find = obj;
		}
	}
	return obj_find;
}

struct __x_obj_item *x_obj_get_down(struct __x_obj_item *obj)
{

	struct __x_obj_item *obj0, *obj_find = NULL;
	RECT rc;

	if (obj != NULL)
	{
		rc = obj->rc;

		obj0 = x_obj_get_first(obj->parent);

		while (obj0 != NULL)
		{
			if (obj0 != obj)
				if (obj0->rc.y > rc.y)
				{
					if (obj_find == NULL)
					{
						obj_find = obj0;
					}
					else
					{
						int obj0_d;
						int obj_find_d;

						obj0_d = __abs(obj0->rc.x - rc.x) + __abs(obj0->rc.y - rc.y);
						obj_find_d = __abs(obj_find->rc.x - rc.x) + __abs(obj_find->rc.y - rc.y);

						if (obj0_d < obj_find_d)
						{
							obj_find = obj0;
						}
					}
				}

			obj0 = x_obj_get_next(obj0);
		}

		if (obj_find == NULL)
		{
			//�±�û����
			obj_find = obj;
		}
	}
	return obj_find;
}

/*===================================================================================*/

void x_obj_draw(HDC hdc, struct __x_obj_item *obj)
{
	if (obj->parent == NULL)
	{
		obj = x_obj_get_first(obj);
		while (obj != NULL)
		{
			obj->pfn_draw(hdc, obj);
			obj = x_obj_get_next(obj);
		}
	}
	else
	{
		obj->pfn_draw(hdc, obj);
	}
}

/*===================================================================================*/

BOOL	x_obj_del(struct __x_obj_item *obj)
{

	if (obj != NULL)
	{
		if (obj->parent == NULL)
		{
			struct __x_obj_item *item, *next;

			item = x_obj_get_first(obj);
			while (item != NULL)
			{
				next = x_obj_get_next(item);
				x_obj_del(item);
				item = next;
			}
		}

		list_del_init(&obj->node);
		delete obj->pszText;
		delete obj;
		return TRUE;

	}
	return FALSE;
}

/*===================================================================================*/

struct __x_obj_item *x_obj_get_from_id(struct __x_obj_item *parent, u32 id)
{
	list_t *lst, *n;
	struct __x_obj_item *obj;

	obj = NULL;
	lst = &parent->node;
	n = lst->next;
	while (n != lst)
	{
		obj = (struct __x_obj_item*)list_entry(n, struct __x_obj_item, node);
		if (obj->id == id)
		{
			return obj;
		}

		n = n->next;
	}
	return NULL;
}

/*===================================================================================*/

struct __x_obj_item *x_obj_get_from_pos(struct __x_obj_item *parent, int x, int y)
{
	list_t *lst, *n;
	struct __x_obj_item *obj;
	POINT pt;

	if (parent == NULL)
	{
		return NULL;
	}

	pt.x = x;
	pt.y = y;
	/*
		if(!PtInRect(&parent->rc,&pt))
		{
			return NULL;
		}
	*/
	obj = NULL;
	lst = &parent->node;
	n = lst->prev;
	while (n != lst)
	{
		obj = (struct __x_obj_item*)list_entry(n, struct __x_obj_item, node);

		//if(!(obj->flag & X_OBJ_DISABLE))
		//if(obj->flag& X_OBJ_VISIBLE)
		if (PtInRect(&obj->rc, &pt))
		{
			return obj;
		}

		n = n->prev;
	}
	return NULL;
}
/*===================================================================================*/

void x_obj_set_text(struct __x_obj_item *obj, const WCHAR *pText)
{
	int i;
	WCHAR *wstr;

	if (obj != NULL)
	{
		i = x_wstrlen(pText) + 1;
		wstr = (WCHAR*)new WCHAR[i];
		if (wstr != NULL)
		{
			x_wstrcpy(wstr, pText);
			delete obj->pszText;
			obj->pszText = wstr;
		}
	}

}

void x_obj_move(struct __x_obj_item *obj, int dx, int dy)
{
	if (obj != NULL)
	{
		OffsetRect(&obj->rc, dx, dy);
	}
}

void x_obj_move_to(struct __x_obj_item *obj, int x, int y)
{
	if (obj != NULL)
	{
		obj->rc.x = x;
		obj->rc.y = y;
	}
}

void* x_obj_get_extra_ptr(struct __x_obj_item *obj)
{
	if (obj != NULL)
	{
		return (void*)((u8*)obj + sizeof(struct __x_obj_item));
	}
	return NULL;
}

/*===================================================================================*/

