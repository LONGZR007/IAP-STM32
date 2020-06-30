
#ifndef	__X_OBJ_H__
#define	__X_OBJ_H__

#ifdef	__cplusplus
extern	"C" {
#endif

#include "def.h"
#include "Clist.h"
#include <emXGUI.h>

	/*===================================================================================*/

#define	X_OBJ_VISIBLE	(1<<0)
#define	X_OBJ_DISABLE	(1<<1)

	typedef	void fn_obj_draw(HDC hdc, struct __x_obj_item * obj);

	struct __x_obj_item {

		RECT rc;
		u32  flag;
		u32  id;
		u32  cbExtra;
		WCHAR *pszText;
		struct __x_obj_item *parent;
		fn_obj_draw *pfn_draw;

		list_t node;

	};

	struct __x_obj_item *x_obj_create_class(const WCHAR *pText, u32 id, const RECT *prc, u32 flag, u32 cbExtra, fn_obj_draw *pfn_draw);
	struct __x_obj_item* x_obj_create(const WCHAR *pText, u32 id, const RECT *prc, u32 flag, u32 cbExtra, struct __x_obj_item *parent);
	struct __x_obj_item* x_obj_get_from_id(struct __x_obj_item *parent, u32 id);
	struct __x_obj_item* x_obj_get_from_pos(struct __x_obj_item *parent, int x, int y);
	struct __x_obj_item* x_obj_get_first(struct __x_obj_item *parent);
	struct __x_obj_item* x_obj_get_last(struct __x_obj_item *parent);
	struct __x_obj_item* x_obj_get_prev(struct __x_obj_item *obj);
	struct __x_obj_item* x_obj_get_next(struct __x_obj_item *obj);
	struct __x_obj_item* x_obj_get_left(struct __x_obj_item *obj);
	struct __x_obj_item* x_obj_get_right(struct __x_obj_item *obj);
	struct __x_obj_item* x_obj_get_up(struct __x_obj_item *obj);
	struct __x_obj_item* x_obj_get_down(struct __x_obj_item *obj);

	void 	x_obj_draw(HDC hdc, struct __x_obj_item *obj);
	void x_obj_handler(struct __x_obj_item *obj, UINT msg, WPARAM wParam, LPARAM lParam);

	BOOL	x_obj_del(struct __x_obj_item *obj);

	void x_obj_set_text(struct __x_obj_item *obj, const WCHAR *pText);
	void x_obj_move(struct __x_obj_item *obj, int dx, int dy);
	void x_obj_move_to(struct __x_obj_item *obj, int x, int y);
	void* x_obj_get_extra_ptr(struct __x_obj_item *obj);

	/*===================================================================================*/

#ifdef	__cplusplus
}
#endif
#endif
