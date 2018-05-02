/**
* \Ecava ISAC menu functions
*
* Author:
*   Dennis Chong
*   Boon Guan
*
* Copyright (C) 2008~2010 Ecava Sdn. Bhd. All Rights Reserved.
*
*/
#include <glib.h>
#include "../../sp-flowtext.h"
#include "../../sp-text.h"
#include "../../sp-rect.h"
#include "../../sp-item-group.h"
#include "../../2geom/forward.h"

#define ISAC_TEXT_DEF_ENTRY 8
#define ISAC_FUNCTION_DEF_ENTRY 1
#define ISAC_CLONE_DEF_ENTRY 8
#define ISAC_COLOR_DEF_ENTRY 8
#define ISAC_TOOLTIPS_DEF_ENTRY 5
#define ISAC_TF_LEN 15
#define ISAC_TF_SHORT_LEN 10
#define ISAC_LBL_LEN 13
#define ISAC_DEF_WIN_HEIGHT 550
#define ISAC_DEF_WIN_WIDTH 480 //HSH - previously 500 

//extern GtkWidget *dlg; // naim@ecava.com: moved from object-properties.cpp //HSH - not being used anymore
static int tagstore_index = -1;

typedef enum {
	Tab_Error,
	Tab_Completed,
	Tab_Ready
} TabStatus;

typedef enum {
	OBJECT_IS_GROUP,
	OBJECT_IS_TEXT,
	OBJECT_IS_RECT,
	OBJECT_IS_OTHERS
} TabObjectType;

typedef enum {
	CLONE_VAR_TEXT = 0,
	CLONE_VAR_EDITABLE,
	CLONE_EQUAL_TEXT,
	CLONE_EQUAL_EDITABLE,
	CLONE_VAL_TEXT,
	CLONE_VAL_EDITABLE,
} CloneListAttributes;

void sage_gui();
void sage_dialog_delete(Inkscape::Preferences *prefs, Glib::ustring const prefs_path);

static void sp_item_widget_modify_selection(SPWidget *spw, Inkscape::Selection *selection, guint flags, GtkWidget *itemw);
static void sp_item_widget_change_selection(SPWidget *spw, Inkscape::Selection *selection, GtkWidget *itemw);
static void sp_item_widget_setup(SPWidget *spw, Inkscape::Selection *selection);
static void sp_item_widget_sensitivity_toggled(GtkWidget *widget, SPWidget *spw);
static void sp_item_widget_hidden_toggled(GtkWidget *widget, SPWidget *spw);
static void sp_item_widget_label_changed(GtkWidget *widget, SPWidget *spw);

void script_onchange(GtkWidget *widget, SPWidget *spw);
static gboolean script_onchange_save(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
void script_onchange_save_toggle(GtkWidget *widget, SPWidget *spw);
void script_keydown_options(gboolean on, SPWidget *spw);

void color_delete_item(GtkWidget *button, SPWidget *spw);
//static gboolean color_list_keypressevent(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);	//HSH - Left behind by previous programmer (NOT USED)
void color_new_list(GtkWidget *button, SPWidget *spw);
void color_deletelastrow();
void color_list_move_up(GtkWidget *button, GtkWidget *treeview);
void color_list_move_down(GtkWidget *button, GtkWidget *treeview);
//static gboolean color_ontabpress(GtkWidget *w, GdkEvent *ev, gpointer data); //HSH - REMOVED (NOT USED)
void color_list_edited_column1(GtkCellRendererText *cell, const char *path_string, const char *new_text, SPWidget *spw);
void color_list_edited_column2(GtkCellRendererText *cell, const char *path_string, const char *new_text, SPWidget *spw);
void color_list_edited_column3(GtkCellRendererText *cell, const char *path_string, const char *new_text, SPWidget *spw);
void color_list_editing_started(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path, SPWidget *spw);	//HSH 
static gboolean color_list_on_key_press (GtkWidget *widget, GdkEventKey *event, SPWidget *spw);	//HSH

void clone_delete_item(GtkWidget *button, SPWidget *spw);
void clone_delete_selected_item();
void clone_new_list(GtkWidget *button, SPWidget *spw);
void clone_deletelastrow();
void clone_list_move_up(GtkWidget *button, GtkWidget *treeview);
void clone_list_move_down(GtkWidget *button, GtkWidget *treeview);
void clone_list_edited_column1(GtkCellRendererText *cell, const char *path_string, const char *new_text, SPWidget *spw);
void clone_list_edited_column2(GtkCellRendererText *cell, const char *path_string, const char *new_text, SPWidget *spw);
void clone_list_edited_column3(GtkCellRendererText *cell, const char *path_string, const char *new_text, SPWidget *spw);
void clone_list_editing_started(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path, SPWidget *spw);	//HSH 
static gboolean clone_list_on_key_press (GtkWidget *widget, GdkEventKey *event, SPWidget *spw);	//HSH
void clone_list_store_current_entry();
void clone_list_add_item(gchar* var, gchar* val, GtkTreeIter* iter);
gboolean clone_list_get_iter_first_incomplete(GtkTreeIter* iter);

void get_tags_by_index(int pageindex);
void get_tags_by_keyword_index(char *keyword, int pageindex);
gboolean findtag_search_entry_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void findtag_search_entry_changed_event(GtkEditable *editable, gpointer user_data);
void findbutton_clicked_event(GtkButton *button, GtkWidget *search_entry);
void findtag_next_clicked_event(GtkButton *button, gpointer user_data);
void findtag_prev_clicked_event(GtkButton *button, gpointer user_data);
void findtag_first_clicked_event(GtkButton *button, gpointer user_data);
void findtag_last_clicked_event(GtkButton *button, gpointer user_data);
void findtag_ok_clicked_event(GtkButton *button, GtkWidget *tf1);
void findtag_color_ok_clicked_event(GtkButton *button, gpointer user_data);
gboolean findtag_taglist_tree1_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean findtag_taglist_tree2_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void findtag_taglist_tree_row_activated(GtkTreeView *treeview, GtkTreePath *arg1, GtkTreeViewColumn *arg2, gpointer user_data);
void findtag_cancel_clicked_event(GtkButton *button, gpointer user_data);
void resetbutton_clicked_event();
void findtag_button_destroy_event(GtkWidget *widget);
void findtag_button_press_callback(GtkButton *button, gpointer user_data);

void onload_attr(GtkWidget *widget, SPWidget *spw);

void isac_update_syntax(GtkWidget *widget, SPWidget *spw);
void isac_apply_button(GtkWidget *widget, SPWidget *spw);
void isac_ok_button(GtkWidget *widget, SPWidget *spw);
void isac_cancel_button(GtkWidget *widget, SPWidget *spw);
static void color_selection(GtkWidget *button, SPWidget *spw);
void isac_json_parse_syntax(const gchar *syntax, SPWidget *spw);
static void isac_set_tab_active(GtkWidget *lbl, int active);
static void isac_set_text_color(GtkWidget *widget, bool error);
static void isac_notebook_new(SPWidget *spw);
void isac_change_mode(SPWidget *spw);
static void isac_setActiveTab(gchar *lbl, SPWidget *spw);
static double scale_round(double val, double factor);
static gchar *reformat_backspc(const gchar *str);
static gchar *remove_backspc(gchar *str);
static gchar *replace_chr(const gchar *str);
static gchar *remove_chr(gchar *str);
static gboolean is_num(const gchar *str);
static gboolean is_alphanum(const gchar *str);

static void isac_color_list_onselected(GtkWidget *widget, SPWidget *spw);
static void isac_open_combo_changed(GtkWidget *widget, SPWidget *spw);
static void isac_zoom_combo_changed(GtkWidget *widget, SPWidget *spw);

static gboolean isac_ontabchanged(GtkWidget *widget, GtkNotebookPage *page, guint tabindex, SPWidget *spw);
void isac_menu_init(GtkTable *table, GtkObject *spw);

/*** tab menu initialization. ***/
GtkWidget *isac_menu_bar_init(GtkObject *spw);
GtkWidget *isac_menu_clone_init(GtkObject *spw);
GtkWidget *isac_menu_color_init(GtkObject *spw);
GtkWidget *isac_menu_function_init(GtkObject *spw);
GtkWidget *isac_menu_get_init(GtkObject *spw);
GtkWidget *isac_menu_opacity_init(GtkObject *spw);
GtkWidget *isac_menu_open_init(GtkObject *spw);
GtkWidget *isac_menu_popup_init(GtkObject *spw);
GtkWidget *isac_menu_rotate_init(GtkObject *spw);
GtkWidget *isac_menu_script_init(GtkObject *spw);
GtkWidget *isac_menu_set_init(GtkObject *spw);
GtkWidget *isac_menu_slider_init(GtkObject *spw);
GtkWidget *isac_menu_text_init(GtkObject *spw);
GtkWidget *isac_menu_tooltips_init(GtkObject *spw);
GtkWidget *isac_menu_zoom_init(GtkObject *spw);
GtkWidget *isac_menu_about_init(GtkObject *spw);
GtkWidget *isac_menu_t_area_init(GtkObject *spw); // track area
GtkWidget *isac_menu_t_link_init(GtkObject *spw); // track link
GtkWidget *isac_menu_t_item_init(GtkObject *spw); // track item

/*** Parse and format fields values into recognizable ISAC syntax. ***/
gchar *isac_get_bar_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_clone_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_color_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_function_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_get_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_opacity_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_open_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_popup_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_rotate_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_script_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_set_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_slider_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_text_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_tooltips_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_zoom_syntax(gchar *syntax, SPWidget *spw);
gchar *isac_get_t_area_syntax(gchar *syntax, SPWidget *spw); // track area
gchar *isac_get_t_link_syntax(gchar *syntax, SPWidget *spw); // track link
gchar *isac_get_t_item_syntax(gchar *syntax, SPWidget *spw); // track item

/*** Retrieve ISAC syntax value ***/
static void isac_bar_assign_syntax(char *token, SPWidget *spw);
static void isac_clone_assign_syntax(char *token, SPWidget *spw);
static void isac_color_assign_syntax(char *token, SPWidget *spw);
static void isac_function_assign_syntax(char *token, SPWidget *spw);
static void isac_get_assign_syntax(char *token, SPWidget *spw);
static void isac_open_assign_syntax(char *token, SPWidget *spw);
static void isac_opacity_assign_syntax(char *token, SPWidget *spw);
static void isac_popup_assign_syntax(char *token, SPWidget *spw);
static void isac_rotate_assign_syntax(char *token, SPWidget *spw);
static void isac_script_assign_syntax(char *token, SPWidget *spw);
static void isac_set_assign_syntax(char *token, SPWidget *spw);
static void isac_slider_assign_syntax(char *token, SPWidget *spw);
static void isac_text_assign_syntax(char *token, SPWidget *spw);
static void isac_tooltips_assign_syntax(char *token, SPWidget *spw);
static void isac_zoom_assign_syntax(char *token, SPWidget *spw);
static void isac_t_area_assign_syntax(char *token, SPWidget *spw); // track area
static void isac_t_link_assign_syntax(char *token, SPWidget *spw); // track link
static void isac_t_item_assign_syntax(char *token, SPWidget *spw); // track item

/*** On focus in function ***/
static void isac_oncolorfocusin(GtkWidget *widget, GdkEventFocus *event, SPWidget *spw);
static void isac_onclonefocusin(GtkWidget *widget, GdkEventFocus *event, SPWidget *spw);

/*** On focus out function ***/
static gboolean isac_onbarfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_onclonefocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_oncolorfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_onfunctionfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_ongetfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_onopacityfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_onopenfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_onpopupfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_onrotatefocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_onscriptfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_onsetfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_onsliderfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_ontextfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_ontooltipsfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);
static gboolean isac_onzoomfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw);

/*** Validation/checking on input fields of respective tab menu ***/
static gboolean isac_validate_bar(SPWidget *spw);
static gboolean isac_validate_clone(SPWidget *spw);
static gboolean isac_validate_color(SPWidget *spw);
static gboolean isac_validate_function(SPWidget *spw);
static gboolean isac_validate_get(SPWidget *spw);
static gboolean isac_validate_opacity(SPWidget *spw);
static gboolean isac_validate_open(SPWidget *spw);
static gboolean isac_validate_popup(SPWidget *spw);
static gboolean isac_validate_rotate(SPWidget *spw);
static gboolean isac_validate_script(SPWidget *spw);
static gboolean isac_validate_set(SPWidget *spw);
static gboolean isac_validate_slider(SPWidget *spw);
static gboolean isac_validate_text(SPWidget *spw);
static gboolean isac_validate_tooltips(SPWidget *spw);
static gboolean isac_validate_zoom(SPWidget *spw);
