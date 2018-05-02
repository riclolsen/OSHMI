/**
* \Ecava ISAC menu functions
*
* Author:
*   BGTeow
*
* Copyright (C) 2010 Ecava Sdn. Bhd. All Rights Reserved.
*
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#include <windows.h>
#endif
#include <gtk/gtk.h>
#include "Json.h"

#include "../../desktop.h"
#include "../../ui/dialog-events.h"
#include "../../document.h"
#include <glibmm/i18n.h>
#include "../../helper/window.h"
#include "../../inkscape.h"
#include "../../ui/interface.h"
#include "../../macros.h"
#include "../../preferences.h"
#include "../../selection.h"
#include "../../sp-item.h"
#include "../../verbs.h"
#include "../../widgets/sp-attribute-widget.h"
#include "../../widgets/sp-widget.h"
#include "isac-menu.h"

// naim@ecava.com
//#include <igcr8a/igcr8a.h>
//#include "../../../../igcr8a/igcr8a.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#define MIN_ONSCREEN_DISTANCE 50

static Glib::ustring const prefs_path = "/dialogs/object/";

//ISAC prerequisite definition
GSList *tt_value_list, *clone_mapfrom_list, *clone_mapto_list, *color_value_list, *color_code_list, *text_value_list, *text_display_list;
GtkWidget *bar_lbl, *clone_lbl, *color_lbl, *function_lbl, *get_lbl, *opacity_lbl, *open_lbl, *popup_lbl, *rotate_lbl, *script_lbl, *set_lbl, *slider_lbl, *tooltips_lbl, *text_lbl, *zoom_lbl, *about_lbl;
GtkWidget *color_list = NULL, *script_list = NULL, *clone_list = NULL;

//GtkWidget *t_area_lbl, *t_link_lbl, *t_item_lbl; // track label
gint nbtype = 0, nbtypepage = 0;

// Color tab, isac_menu_color_init()
GtkTreePath *g_curr_gtreepath;
GtkListStore *store;
GtkTreeViewColumn *column1, *column2, *column3;
GtkCellRenderer *renderer1, *renderer2, *renderer3;
GtkWidget *hbox, *vbox, *bttn, *scroll, /**list,*/ *deleteb, *newb, *upb, *downb, *colorb;

// Find tag
GtkWidget *findtag_window, *filename_label, *taglist_tree1, *taglist_tree2, *findtag_total, *search_entry;
GtkListStore *tag_store1, *tag_store2;
std::vector<std::string> taglist_vector;
int findtag_index = 0, findtag_rowCount = 0, findtag_pageCount = 0;
int findtag_index2 = 0, findtag_rowCount2 = 0, findtag_pageCount2 = 0;
bool is_pageindex_zero = false, done_loading_tag_names = true, is_processing_tag_names = false, is_pageindex_last = false;
bool is_pageindex_zero2 = false, done_loading_tag_names2 = true, is_processing_tag_names2 = false, is_pageindex_last2 = false;
bool is_igx_open = false, use_search_result = false;
char *g_igx_filename = "";
char *passed_to_dll;
char *completepath;

//HSH - these pointers are set when editing on a cell has started for color_list
GtkCellRenderer *currentRenderer;	//HSH - points to the current GtkCellRenderer
GtkEntry *currentEntry;	//HSH - points to the current GtkEntry

//HSH, 25/03/2015, Faceplate tab list, isac_menu_color
GtkTreePath *clone_g_curr_gtreepath;
GtkListStore *clone_store;
GtkTreeViewColumn *clone_column1, *clone_column2, *clone_column3;
GtkCellRenderer *clone_renderer1, *clone_renderer2, *clone_renderer3;
GtkWidget *clone_hbox, *clone_vbox, *clone_bttn, *clone_scroll;
GtkWidget *clone_deleteb, *clone_newb, *clone_upb, *clone_downb;

char *replace(const char *str, const char *old, const char *cnew) {
	char *ret, *r;
	const char *p, *q;
	size_t len_str = strlen(str);
	size_t len_old = strlen(old);
	size_t len_new = strlen(cnew);
	size_t count;
	for (count = 0, p = str; (p = strstr(p, old)); p += len_old)
		count++;
	ret = (char*)malloc(count * (len_new - len_old) + len_str + 1);
	if (!ret)
		return NULL;
	for (r = ret, p = str; (q = strstr(p, old)); p = q + len_old) {
		count = q - p;
		memcpy(r, p, count);
		r += count;
		strcpy(r, cnew);
		r += len_new;
	}
	strcpy(r, p);
	return ret;
}
void sage_gui()
{
	g_slist_free(clone_mapfrom_list);
	g_slist_free(clone_mapto_list);
	g_slist_free(color_value_list);
	g_slist_free(color_code_list);
	g_slist_free(text_value_list);
	g_slist_free(text_display_list);
	g_slist_free(tt_value_list);

	text_value_list = NULL;
	text_display_list = NULL;
	clone_mapfrom_list = NULL;
	clone_mapto_list = NULL;
	color_value_list = NULL;
	color_code_list = NULL;
	tt_value_list = NULL;
}
void sage_dialog_delete(Inkscape::Preferences *prefs, Glib::ustring const prefs_path)
{
	prefs->setInt(prefs_path + "nbtype", nbtype);
	prefs->setInt(prefs_path + "nbtypepage", nbtypepage);
	color_deletelastrow(); // naim@ecava.com
	clone_deletelastrow();
}
void isac_menu_init(GtkTable *table, GtkObject *spw)
{
	/***************************************************
	ISAC FEATURES:
	ISAC menu initialization.
	Table grid and layout will be initialized and then all the menu tab will be
	definied.
	***************************************************/
	GtkWidget *frm, *nb, *frame_hb, *frame_vb, /**frame_hb_butt, *apply_bttn, *ok_bttn, *cancel_bttn,*/ *frame_nb;	//HSH - removed unused pointers
	/*ISAC frame
	Parent Table -> Frame -> Horizontal Box -> Vertical Box -> Notebook tab
	*/

	//table = gtk_table_new (10, 4, FALSE);
	frm = gtk_frame_new(_("XSAC"));
	gtk_table_attach(GTK_TABLE(table), frm, 0, 3, 4, 10,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);

	nb = gtk_notebook_new();

	//apply_bttn = gtk_button_new_with_mnemonic(_("_Apply"));
	//gtk_signal_connect(GTK_OBJECT(apply_bttn), "clicked",
	//	GTK_SIGNAL_FUNC(isac_apply_button),
	//	spw);

	//ok_bttn = gtk_button_new_with_mnemonic(_("_Ok"));
	//gtk_signal_connect(GTK_OBJECT(ok_bttn), "clicked",
	//	GTK_SIGNAL_FUNC(isac_ok_button),
	//	spw);

	//cancel_bttn = gtk_button_new_with_mnemonic(_("_Cancel"));
	//gtk_signal_connect(GTK_OBJECT(cancel_bttn), "clicked",
	//	GTK_SIGNAL_FUNC(isac_cancel_button),
	//	spw);
	
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(nb), TRUE);

	frame_nb = gtk_vbox_new(FALSE, 0);
	frame_hb = gtk_hbox_new(FALSE, 0);
	//frame_hb_butt = gtk_hbox_new(FALSE, 3);	//HSH - not used
	frame_vb = gtk_vbox_new(FALSE, 4);

	gtk_object_set_data(GTK_OBJECT(spw), "isac_nb", nb);
	//g_object_ref(nb);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_nb_frame", frame_nb);
	//g_object_ref(frame_nb);

	gtk_box_pack_start(GTK_BOX(frame_nb), nb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(frame_vb), frame_nb, TRUE, TRUE, 5);  //5 - vertical padding from the frame border

	//gtk_box_pack_start(GTK_BOX(frame_hb_butt), apply_bttn, TRUE, TRUE, 0);
	//gtk_box_pack_start(GTK_BOX(frame_hb_butt), ok_bttn, TRUE, TRUE, 0);
	//gtk_box_pack_start(GTK_BOX(frame_hb_butt), cancel_bttn, TRUE, TRUE, 0);

	//gtk_box_pack_start(GTK_BOX(frame_vb), frame_hb_butt, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(frame_hb), frame_vb, TRUE, TRUE, 5); //5 - horizontal padding from the frame border
	gtk_container_add(GTK_CONTAINER(frm), frame_hb);

	//GtkWidget *bar_lbl, *clone_lbl, *color_lbl, *function_lbl, *get_lbl, *opacity_lbl, *open_lbl, *rotate_lbl, *set_lbl, *text_lbl, *zoom_lbl;

	clone_lbl = gtk_label_new("Faceplate ");	//formerly known as Clone
	bar_lbl = gtk_label_new("Bar ");	
	color_lbl = gtk_label_new("Color ");
	function_lbl = gtk_label_new("Function ");
	get_lbl = gtk_label_new("Get ");
	opacity_lbl = gtk_label_new("Opacity ");
	open_lbl = gtk_label_new("Open ");
	popup_lbl = gtk_label_new("Pop up ");
	rotate_lbl = gtk_label_new("Rotate ");
	script_lbl = gtk_label_new("Script ");
	set_lbl = gtk_label_new("Set ");
	slider_lbl = gtk_label_new("Slider ");
	text_lbl = gtk_label_new("Text ");
	tooltips_lbl = gtk_label_new("Tooltips ");
	zoom_lbl = gtk_label_new("Zoom ");
	about_lbl = gtk_label_new("About ");
	//t_area_lbl = gtk_label_new("Track Area ");
	//t_link_lbl = gtk_label_new("Track Link ");
	//t_item_lbl = gtk_label_new("Track Item ");

	isac_set_tab_active(clone_lbl, Tab_Ready);
	isac_set_tab_active(bar_lbl, Tab_Ready);	
	isac_set_tab_active(color_lbl, Tab_Ready);
	isac_set_tab_active(function_lbl, Tab_Ready);
	isac_set_tab_active(get_lbl, Tab_Ready);
	isac_set_tab_active(opacity_lbl, Tab_Ready);
	isac_set_tab_active(open_lbl, Tab_Ready);
	isac_set_tab_active(popup_lbl, Tab_Ready);
	isac_set_tab_active(rotate_lbl, Tab_Ready);
	isac_set_tab_active(script_lbl, Tab_Ready);
	isac_set_tab_active(set_lbl, Tab_Ready);
	isac_set_tab_active(slider_lbl, Tab_Ready);
	isac_set_tab_active(text_lbl, Tab_Ready);
	isac_set_tab_active(tooltips_lbl, Tab_Ready);
	isac_set_tab_active(zoom_lbl, Tab_Ready);
	isac_set_tab_active(about_lbl, Tab_Ready);
	//isac_set_tab_active (t_area_lbl, FALSE);
	//isac_set_tab_active (t_link_lbl, FALSE);
	//isac_set_tab_active (t_item_lbl, FALSE);
}
static void isac_notebook_new(SPWidget *spw)
{
	/**************************************************
	ISAC Features:
	Notebook tab initialization.
	The notebook tab will be initialized base on the object type: RECT, TEXT,
	GROUP and OTHER.
	The corresponding tab will be set up and displayed.
	**************************************************/
	GtkWidget *nb;
	SPDesktop *desktop = SP_ACTIVE_DESKTOP;
	Inkscape::Selection *selection = SP_ACTIVE_DESKTOP->getSelection();
	Inkscape::Preferences *prefs = Inkscape::Preferences::get();

	gchar const *start, *end;

	SPItem *item = selection->singleItem();
	SPObject *obj = (SPObject*)item;
	start = item->getAttribute("inkscape:connection-start", NULL);
	end = item->getAttribute("inkscape:connection-end", NULL);

	nb = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_nb"));
	GtkWidget *frame_nb = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_nb_frame"));

	if (nb == NULL)
	{
		nb = gtk_notebook_new();
		gtk_notebook_set_scrollable(GTK_NOTEBOOK(nb), TRUE);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_nb", nb);

		gtk_box_pack_start(GTK_BOX(frame_nb), nb, TRUE, TRUE, 0);
	}
	if (SP_IS_GROUP(item))
	{
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_clone_init(GTK_OBJECT(spw)), clone_lbl);
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_function_init(GTK_OBJECT(spw)), function_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_opacity_init(GTK_OBJECT(spw)), opacity_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_open_init(GTK_OBJECT(spw)), open_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_popup_init(GTK_OBJECT(spw)), popup_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_rotate_init(GTK_OBJECT(spw)), rotate_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_script_init(GTK_OBJECT(spw)), script_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_set_init(GTK_OBJECT(spw)), set_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_slider_init(GTK_OBJECT(spw)), slider_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_tooltips_init(GTK_OBJECT(spw)), tooltips_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_zoom_init(GTK_OBJECT(spw)), zoom_lbl);
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_t_item_init(GTK_OBJECT(spw)), t_item_lbl); // track item
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_about_init(GTK_OBJECT(spw)), about_lbl);

		gint prev_nbtype = prefs->getInt(prefs_path + "nbtype", 0);
		gint prev_nbtypepage = prefs->getInt(prefs_path + "nbtypepage", 0);

		if (prev_nbtype == OBJECT_IS_GROUP)
		{
			gtk_widget_show_all(GTK_WIDGET(spw));
			gtk_notebook_set_current_page(GTK_NOTEBOOK(nb), prev_nbtypepage);
		}
		nbtype = OBJECT_IS_GROUP;

	}
	else if (SP_IS_FLOWTEXT(item) || SP_IS_TEXT(item))
	{
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_clone_init(GTK_OBJECT(spw)), clone_lbl);	//Added Faceplate(Clone) to TEXT type objects
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_color_init(GTK_OBJECT(spw)), color_lbl);
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_function_init(GTK_OBJECT(spw)), function_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_get_init(GTK_OBJECT(spw)), get_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_opacity_init(GTK_OBJECT(spw)), opacity_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_open_init(GTK_OBJECT(spw)), open_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_popup_init(GTK_OBJECT(spw)), popup_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_rotate_init(GTK_OBJECT(spw)), rotate_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_script_init(GTK_OBJECT(spw)), script_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_set_init(GTK_OBJECT(spw)), set_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_slider_init(GTK_OBJECT(spw)), slider_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_text_init(GTK_OBJECT(spw)), text_lbl);
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_t_item_init(GTK_OBJECT(spw)), t_item_lbl); // track item
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_about_init(GTK_OBJECT(spw)), about_lbl);

		gint prev_nbtype = prefs->getInt(prefs_path + "nbtype", 0);
		gint prev_nbtypepage = prefs->getInt(prefs_path + "nbtypepage", 0);

		if (prev_nbtype == OBJECT_IS_TEXT)
		{
			gtk_widget_show_all(GTK_WIDGET(spw));
			gtk_notebook_set_current_page(GTK_NOTEBOOK(nb), prev_nbtypepage);
		}

		nbtype = OBJECT_IS_TEXT;
	}
	else if (SP_IS_RECT(item))
	{
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_bar_init(GTK_OBJECT(spw)), bar_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_clone_init(GTK_OBJECT(spw)), clone_lbl);	//Added Faceplate(Clone) to RECT type objects
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_color_init(GTK_OBJECT(spw)), color_lbl);
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_function_init(GTK_OBJECT(spw)), function_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_opacity_init(GTK_OBJECT(spw)), opacity_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_open_init(GTK_OBJECT(spw)), open_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_popup_init(GTK_OBJECT(spw)), popup_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_rotate_init(GTK_OBJECT(spw)), rotate_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_script_init(GTK_OBJECT(spw)), script_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_set_init(GTK_OBJECT(spw)), set_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_slider_init(GTK_OBJECT(spw)), slider_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_tooltips_init(GTK_OBJECT(spw)), tooltips_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_zoom_init(GTK_OBJECT(spw)), zoom_lbl);
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_t_area_init(GTK_OBJECT(spw)), t_area_lbl); // track area
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_t_item_init(GTK_OBJECT(spw)), t_item_lbl); // track item
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_about_init(GTK_OBJECT(spw)), about_lbl);

		gint prev_nbtype = prefs->getInt(prefs_path + "nbtype", 0);
		gint prev_nbtypepage = prefs->getInt(prefs_path + "nbtypepage", 0);

		if (prev_nbtype == OBJECT_IS_RECT)
		{
			gtk_widget_show_all(GTK_WIDGET(spw));
			gtk_notebook_set_current_page(GTK_NOTEBOOK(nb), prev_nbtypepage);
		}
		//else	//HSH, 27/03/2015, removed so the notebook will go to first page 
		//{
		//	gtk_widget_show_all(GTK_WIDGET(spw));
		//	gtk_notebook_set_current_page(GTK_NOTEBOOK(nb), 1);
		//}

		nbtype = OBJECT_IS_RECT;
	}
	else if (start && end)
	{
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_clone_init(GTK_OBJECT(spw)), clone_lbl);	//Added Faceplate(Clone) to OTHER type objects
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_color_init(GTK_OBJECT(spw)), color_lbl);
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_function_init(GTK_OBJECT(spw)), function_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_opacity_init(GTK_OBJECT(spw)), opacity_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_open_init(GTK_OBJECT(spw)), open_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_popup_init(GTK_OBJECT(spw)), popup_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_rotate_init(GTK_OBJECT(spw)), rotate_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_script_init(GTK_OBJECT(spw)), script_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_set_init(GTK_OBJECT(spw)), set_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_slider_init(GTK_OBJECT(spw)), slider_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_tooltips_init(GTK_OBJECT(spw)), tooltips_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_zoom_init(GTK_OBJECT(spw)), zoom_lbl);
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_t_link_init(GTK_OBJECT(spw)), t_link_lbl); // track link
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_t_item_init(GTK_OBJECT(spw)), t_item_lbl); // track item
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_about_init(GTK_OBJECT(spw)), about_lbl);

		gint prev_nbtype = prefs->getInt(prefs_path + "nbtype", 0);
		gint prev_nbtypepage = prefs->getInt(prefs_path + "nbtypepage", 0);

		if (prev_nbtype == OBJECT_IS_OTHERS)
		{
			gtk_widget_show_all(GTK_WIDGET(spw));
			gtk_notebook_set_current_page(GTK_NOTEBOOK(nb), prev_nbtypepage);
		}

		nbtype = OBJECT_IS_OTHERS;

	}
	else
	{
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_clone_init(GTK_OBJECT(spw)), clone_lbl);	//Added Faceplate(Clone) to OTHER type objects
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_color_init(GTK_OBJECT(spw)), color_lbl);
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_function_init(GTK_OBJECT(spw)), function_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_opacity_init(GTK_OBJECT(spw)), opacity_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_open_init(GTK_OBJECT(spw)), open_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_popup_init(GTK_OBJECT(spw)), popup_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_rotate_init(GTK_OBJECT(spw)), rotate_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_script_init(GTK_OBJECT(spw)), script_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_set_init(GTK_OBJECT(spw)), set_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_slider_init(GTK_OBJECT(spw)), slider_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_tooltips_init(GTK_OBJECT(spw)), tooltips_lbl);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_zoom_init(GTK_OBJECT(spw)), zoom_lbl);
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_t_link_init(GTK_OBJECT(spw)), t_link_lbl); // track link
		//gtk_notebook_append_page(GTK_NOTEBOOK(nb),isac_menu_t_item_init(GTK_OBJECT(spw)), t_item_lbl); // track item
		gtk_notebook_append_page(GTK_NOTEBOOK(nb), isac_menu_about_init(GTK_OBJECT(spw)), about_lbl);

		gint prev_nbtype = prefs->getInt(prefs_path + "nbtype", 0);
		gint prev_nbtypepage = prefs->getInt(prefs_path + "nbtypepage", 0);

		if (prev_nbtype == OBJECT_IS_OTHERS)
		{
			gtk_widget_show_all(GTK_WIDGET(spw));
			gtk_notebook_set_current_page(GTK_NOTEBOOK(nb), prev_nbtypepage);
		}

		nbtype = OBJECT_IS_OTHERS;
	}

	g_signal_connect(GTK_OBJECT(nb), "switch-page",
		GTK_SIGNAL_FUNC(isac_ontabchanged), spw);

}
static void isac_open_combo_changed(GtkWidget *widget, SPWidget *spw)
{
	/*****************************
	ISAC Features:
	Validate triggered when selection of combo field entry "type" in tab menu "Open" is changed.
	When type = Current, field "xpos", "ypos", "width" and "height" shall be disabled,
	validation wont be checked on these fields;
	When type = New, field "xpos", "ypos", "width" and "height" shall be enabled, validation
	shall be checked on these fields.
	*****************************/
	GtkWidget *wg_xpos, *wg_ypos, *wg_width, *wg_height, *wg_type;

	gint type = -1;

	wg_xpos = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_xpos"));

	wg_ypos = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_ypos"));

	wg_width = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_width"));

	wg_height = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_height"));

	wg_type = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_type"));
	type = gtk_combo_box_get_active(GTK_COMBO_BOX(wg_type));

	if (type == 0)
	{
		gtk_widget_set_sensitive(wg_xpos, FALSE);
		gtk_entry_set_text(GTK_ENTRY(wg_xpos), "100");
		gtk_widget_set_sensitive(wg_ypos, FALSE);
		gtk_entry_set_text(GTK_ENTRY(wg_ypos), "100");
		gtk_widget_set_sensitive(wg_height, FALSE);
		gtk_entry_set_text(GTK_ENTRY(wg_height), "400");
		gtk_widget_set_sensitive(wg_width, FALSE);
		gtk_entry_set_text(GTK_ENTRY(wg_width), "500");
	}

	else
	{
		gtk_widget_set_sensitive(wg_xpos, TRUE);
		gtk_widget_set_sensitive(wg_ypos, TRUE);
		gtk_widget_set_sensitive(wg_height, TRUE);
		gtk_widget_set_sensitive(wg_width, TRUE);

	}

	isac_validate_open(spw);

	return;
}
static void isac_zoom_combo_changed(GtkWidget *widget, SPWidget *spw)
{
	/*****************************
	ISAC Features:
	Validate triggered when selection in combo field in tab menu "Zoom" has changed.

	*****************************/
	isac_validate_zoom(spw);
	return;
}
static void isac_set_text_color(GtkWidget *widget, bool error) {
	/*****************************
	ISAC Features:
	Set text color to red or black depend on validation result.
	If the field value is invalid, text color will be set to red;
	If the field value is valid, text color will be set to black;
	*****************************/
	const GdkColor errorcolor = { 0, 65535, 0, 0 };
	const GdkColor normalcolor = { 0, 0, 0, 0 };

	if (error == 1)
		gtk_widget_modify_text(GTK_WIDGET(widget), GTK_STATE_NORMAL, &errorcolor);
	else
		gtk_widget_modify_text(GTK_WIDGET(widget), GTK_STATE_NORMAL, &normalcolor);
}
static void isac_set_tab_active(GtkWidget *lbl, int active){
	/*****************************
	ISAC Features:
	Set the tab inactive or active depend on the fields' validation result.
	If any of the field in a tab menu contain invalid values, the tab's text label
	will be set to original style - "black";
	If all of the fields in a tab menu is valid, the tab's text label
	will be set to style - "black", "italic", "underline" and "bold";
	*****************************/
	const char *text = gtk_label_get_text(GTK_LABEL(lbl));
	char *markup;

	//char str[100]; // not used
	//gint i=0;
	//sprintf(str,"Page %d", active);

	if (Tab_Error == active)
		markup = g_markup_printf_escaped("<span foreground=\"#FF0000\">%s</span>", text);

	else if (Tab_Completed == active)
		markup = g_markup_printf_escaped("<span style=\"italic\" underline=\"single\">%s</span>", text);

	else
		markup = g_markup_printf_escaped("<span>%s</span>", text);

	gtk_label_set_markup(GTK_LABEL(lbl), markup);
	g_free(markup);
}
static gchar* remove_backspc(gchar *str)
{
	/*****************************
	ISAC Features:
	Any backspace character '\\' found in the ISAC text value shall be removed before
	assigned back to the text field during start up.
	*****************************/
	gint j = 0;
	gchar *newstr;
	newstr = new gchar[strlen(str) * 2];

	for (gint unsigned i = 0; i<strlen(str); i++)
	{

		if (str[i] == '\\')
		{
			while (str[i] == '\\') i++;

			newstr[j] = str[i];
			j++;
		}
		else
		{
			newstr[j] = str[i];
			j++;
		}
	}
	newstr[j] = '\0';
	return newstr;
}
static double scale_round(double val, double factor)
{
	/*****************************
	ISAC Features:
	Rounding feature for parsing the color code
	*****************************/
	val = floor(val * factor + 0.5);
	val = MAX(val, 0);
	val = MIN(val, factor);
	return val;
}
static gboolean is_num(const gchar *str)
{
	/*****************************
	ISAC Features:
	Validation function for checking numerical values
	*****************************/
	gboolean isNum = TRUE;
	int dot = 0;
	for (unsigned int i = 0; i < strlen(str); i++) {
		if (i == 0)
		{
			if (str[i] == '-') continue;
		}
		if (!isdigit(str[i])) {
			if (str[i] != '.'){
				isNum = FALSE;
				break;
			}
			else{
				dot++;
			}

			if (dot>1){
				isNum = FALSE;
				break;
			}
		}
	}
	return isNum;
}
static gboolean is_alphanum(const gchar *str)
{
	/*****************************
	ISAC Features:
	Validation function for checking alpha-numerical values
	*****************************/
	gboolean isalphanum = TRUE;
	for (unsigned int i = 0; i < strlen(str); i++) {
		if (!isalnum(str[i])) {
			isalphanum = FALSE;
			break;
		}
	}
	return isalphanum;
}
void isac_update_syntax(GtkWidget *widget, SPWidget *spw)
{
	/*****************************
	ISAC Features:
	Update all the fields value into the ISAC syntax.
	*****************************/
	GtkWidget *label_entry;
	gchar *isac_syntax = "";
	label_entry = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "label"));

	SPDesktop *desktop = SP_ACTIVE_DESKTOP;
	Inkscape::Selection *selection = SP_ACTIVE_DESKTOP->getSelection();

	SPItem *item = selection->singleItem();

	if (SP_IS_GROUP(item))
	{
		isac_syntax = isac_get_clone_syntax(isac_syntax, spw);
		//isac_syntax = isac_get_function_syntax(isac_syntax, spw);
		isac_syntax = isac_get_opacity_syntax(isac_syntax, spw);
		isac_syntax = isac_get_open_syntax(isac_syntax, spw);
		isac_syntax = isac_get_popup_syntax(isac_syntax, spw);
		isac_syntax = isac_get_rotate_syntax(isac_syntax, spw);
		isac_syntax = isac_get_set_syntax(isac_syntax, spw);
		isac_syntax = isac_get_slider_syntax(isac_syntax, spw);
		isac_syntax = isac_get_tooltips_syntax(isac_syntax, spw);
		isac_syntax = isac_get_zoom_syntax(isac_syntax, spw);
		isac_syntax = isac_get_script_syntax(isac_syntax, spw);
	}

	else if (SP_IS_FLOWTEXT(item) || SP_IS_TEXT(item))
	{
		isac_syntax = isac_get_clone_syntax(isac_syntax, spw);	//Updates syntax for Faceplate(Clone) on TEXT objects
		isac_syntax = isac_get_color_syntax(isac_syntax, spw);
		//isac_syntax = isac_get_function_syntax(isac_syntax, spw);
		isac_syntax = isac_get_get_syntax(isac_syntax, spw);
		isac_syntax = isac_get_opacity_syntax(isac_syntax, spw);
		isac_syntax = isac_get_open_syntax(isac_syntax, spw);
		isac_syntax = isac_get_popup_syntax(isac_syntax, spw);
		isac_syntax = isac_get_rotate_syntax(isac_syntax, spw);
		isac_syntax = isac_get_set_syntax(isac_syntax, spw);
		isac_syntax = isac_get_slider_syntax(isac_syntax, spw);
		isac_syntax = isac_get_text_syntax(isac_syntax, spw);
		isac_syntax = isac_get_script_syntax(isac_syntax, spw);
	}

	else if (SP_IS_RECT(item))
	{
		isac_syntax = isac_get_clone_syntax(isac_syntax, spw);	//Updates syntax for Faceplate(Clone) on RECT objects
		isac_syntax = isac_get_bar_syntax(isac_syntax, spw);
		isac_syntax = isac_get_color_syntax(isac_syntax, spw);
		//isac_syntax = isac_get_function_syntax(isac_syntax, spw);
		isac_syntax = isac_get_opacity_syntax(isac_syntax, spw);
		isac_syntax = isac_get_open_syntax(isac_syntax, spw);
		isac_syntax = isac_get_popup_syntax(isac_syntax, spw);
		isac_syntax = isac_get_rotate_syntax(isac_syntax, spw);
		isac_syntax = isac_get_set_syntax(isac_syntax, spw);
		isac_syntax = isac_get_slider_syntax(isac_syntax, spw);
		isac_syntax = isac_get_tooltips_syntax(isac_syntax, spw);
		isac_syntax = isac_get_zoom_syntax(isac_syntax, spw);
		isac_syntax = isac_get_script_syntax(isac_syntax, spw);
	}

	else
	{
		isac_syntax = isac_get_clone_syntax(isac_syntax, spw);	//Updates syntax for Faceplate(Clone) on OTHER objects
		isac_syntax = isac_get_color_syntax(isac_syntax, spw);
		//isac_syntax = isac_get_function_syntax(isac_syntax, spw);
		isac_syntax = isac_get_opacity_syntax(isac_syntax, spw);
		isac_syntax = isac_get_open_syntax(isac_syntax, spw);
		isac_syntax = isac_get_popup_syntax(isac_syntax, spw);
		isac_syntax = isac_get_rotate_syntax(isac_syntax, spw);
		isac_syntax = isac_get_set_syntax(isac_syntax, spw);
		isac_syntax = isac_get_slider_syntax(isac_syntax, spw);
		isac_syntax = isac_get_tooltips_syntax(isac_syntax, spw);
		isac_syntax = isac_get_zoom_syntax(isac_syntax, spw);
		isac_syntax = isac_get_script_syntax(isac_syntax, spw);
	}

	gtk_entry_set_text(GTK_ENTRY(label_entry), isac_syntax);

	g_free(isac_syntax);
}
void isac_json_parse_syntax(const gchar *syntax, SPWidget *spw){
	//------------------------------------------------
	// ISAC Features:
	// Retrieve ISAC syntax value and parse it and assigned it back to each 
	// corresponding fields during start up.
	//------------------------------------------------
	JSONVALUE j, getj;
	char *attr, *token;
	const gchar *new_syntax;
	int end = 0;
	std::string jvalue;
	string *jsyntax;
	gchar *actpage = "", *jArray = "";

	jArray = g_strconcat("[", syntax, "]", NULL);
	getj.Parse(jArray, JSON_FLAG_ALLOWUNQUOTENAME);
	size_t nCount = getj.Size();

	for (size_t i = 0; i<nCount; i++)
	{
		j = getj.At(i);

		j.ToString(jvalue, 0);
		token = (char*)jvalue.c_str(); // get whole JSON syntax

		end = strcspn(token, "@");

		if (end < 3)
			jsyntax = j["@ttr"].val.strVal;
		else
			jsyntax = j["attr"].val.strVal;

		attr = (char*)jsyntax->c_str(); // get attribute
		
		if (strcmp(attr, "clone") == 0)
		{
			isac_clone_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Faceplate ";	//Formerly known as Clone
		}
		else if (strcmp(attr, "bar") == 0)
		{
			isac_bar_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Bar ";
		}		
		else if (strcmp(attr, "color") == 0)
		{
			isac_color_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Color ";
		}
		else if (strcmp(attr, "get") == 0)
		{
			isac_get_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Get ";
		}
		else if (strcmp(attr, "opac") == 0)
		{
			isac_opacity_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Opacity ";
		}
		else if (strcmp(attr, "open") == 0)
		{
			isac_open_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Open ";
		}
		else if (strcmp(attr, "popup") == 0)
		{
			isac_popup_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Pop up ";
		}
		else if (strcmp(attr, "rotate") == 0)
		{
			isac_rotate_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Rotate ";
		}
		else if (strcmp(attr, "script") == 0)
		{
			isac_script_assign_syntax(token, spw);
			if (strlen(actpage) == 0) {
				actpage = "Script ";
				//check script_list selection (to disable keydown options if needed)
				GtkWidget* selection = GTK_WIDGET(gtk_tree_view_get_selection(GTK_TREE_VIEW(script_list)));
				script_onchange(selection, spw);
			}
		}
		else if (strcmp(attr, "set") == 0)
		{
			isac_set_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Set ";
		}
		else if (strcmp(attr, "slider") == 0)
		{
			isac_slider_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Slider ";
		}
		else if (strcmp(attr, "text") == 0)
		{
			isac_text_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Text ";
		}
		else if (strcmp(attr, "tooltips") == 0)
		{
			isac_tooltips_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Tooltips ";
		}
		else if (strcmp(attr, "zoom") == 0)
		{
			isac_zoom_assign_syntax(token, spw);
			if (strlen(actpage) == 0)
				actpage = "Zoom ";
		}
	}

	isac_setActiveTab(actpage, spw);
}
static void isac_setActiveTab(gchar *lbl, SPWidget *spw) // isac_setActiveTab("Color ", spw);
{
	//------------------------------------------------
	//    ISAC Features:
	//    Function to set the first animated tab to be 
	//    active/in-focus. 
	//------------------------------------------------
	GtkNotebook *nb = GTK_NOTEBOOK(gtk_object_get_data(GTK_OBJECT(spw), "isac_nb"));
	gint n = gtk_notebook_get_n_pages(nb);

	gint actpage = 0;
	gboolean set = FALSE;
	for (gint i = 0; i<n; i++)
	{
		const gchar *str = gtk_notebook_get_tab_label_text(nb, gtk_notebook_get_nth_page(nb, i));

		if (strcmp(lbl, str) == 0)
		{
			actpage = i;
			set = TRUE;
			break;
		}
	}
	if (set)
		gtk_notebook_set_current_page(nb, actpage);
	else
		gtk_notebook_set_current_page(nb, 0);
}
void isac_change_mode(SPWidget *spw)
{
	//------------------------------------------------
	//    ISAC Features:
	//    Trigger whenever the object focus point has changed (for e.g. 
	//    switch to different object type),
	//    reinitialized of the item properties windows needed to reflect
	//    the tab menu for the new object type.
	//------------------------------------------------
	GtkWidget *nb = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_nb"));

	gtk_widget_destroy(nb);
	nb = NULL;
	gtk_object_set_data(GTK_OBJECT(spw), "isac_nb", nb);
	
	clone_lbl = gtk_label_new("Faceplate "); //Formerly known as Clone
	bar_lbl = gtk_label_new("Bar ");	
	color_lbl = gtk_label_new("Color ");
	function_lbl = gtk_label_new("Function ");
	get_lbl = gtk_label_new("Get ");
	opacity_lbl = gtk_label_new("Opacity ");
	open_lbl = gtk_label_new("Open ");
	popup_lbl = gtk_label_new("Pop up ");
	rotate_lbl = gtk_label_new("Rotate ");
	script_lbl = gtk_label_new("Script ");
	set_lbl = gtk_label_new("Set ");
	slider_lbl = gtk_label_new("Slider ");
	text_lbl = gtk_label_new("Text ");
	tooltips_lbl = gtk_label_new("Tooltips ");
	zoom_lbl = gtk_label_new("Zoom ");
	about_lbl = gtk_label_new("About ");
	//t_area_lbl = gtk_label_new("Track Area ");
	//t_link_lbl = gtk_label_new("Track Link ");
	//t_item_lbl = gtk_label_new("Track Item ");

	isac_set_tab_active(clone_lbl, Tab_Ready);
	isac_set_tab_active(bar_lbl, Tab_Ready);	
	isac_set_tab_active(color_lbl, Tab_Ready);
	isac_set_tab_active(function_lbl, Tab_Ready);
	isac_set_tab_active(get_lbl, Tab_Ready);
	isac_set_tab_active(opacity_lbl, Tab_Ready);
	isac_set_tab_active(open_lbl, Tab_Ready);
	isac_set_tab_active(popup_lbl, Tab_Ready);
	isac_set_tab_active(rotate_lbl, Tab_Ready);
	isac_set_tab_active(script_lbl, Tab_Ready);
	isac_set_tab_active(set_lbl, Tab_Ready);
	isac_set_tab_active(slider_lbl, Tab_Ready);
	isac_set_tab_active(text_lbl, Tab_Ready);
	isac_set_tab_active(tooltips_lbl, Tab_Ready);
	isac_set_tab_active(zoom_lbl, Tab_Ready);
	isac_set_tab_active(about_lbl, Tab_Ready);
	//isac_set_tab_active (t_area_lbl, FALSE); // Track Area
	//isac_set_tab_active (t_link_lbl, FALSE); // Track Link
	//isac_set_tab_active (t_item_lbl, FALSE); // Track Item

	g_slist_free(clone_mapfrom_list);
	g_slist_free(clone_mapto_list);
	g_slist_free(color_value_list);
	g_slist_free(color_code_list);
	g_slist_free(text_value_list);
	g_slist_free(text_display_list);
	g_slist_free(tt_value_list);

	text_value_list = NULL;
	text_display_list = NULL;
	clone_mapfrom_list = NULL;
	clone_mapto_list = NULL;
	color_value_list = NULL;
	color_code_list = NULL;
	tt_value_list = NULL;
	
	//HSH - notebook was destroyed, which destroys everything in it. This global pointer is pointing to nothing.
	color_list = NULL;
	script_list = NULL;
	clone_list = NULL;

	isac_notebook_new(spw);

	gtk_widget_show_all(GTK_WIDGET(spw));
}
void onload_attr(GtkWidget *widget, SPWidget *spw)
{
	static SPDesktop *desktop = SP_ACTIVE_DESKTOP;
	//static SPObject *root = SP_DOCUMENT_ROOT (sp_desktop_document (desktop));
	//Inkscape::XML::Node *getReprRoot()
	//SPRoot *root = SP_ROOT(desktop);
	Inkscape::Selection *selection = SP_ACTIVE_DESKTOP->getSelection();
	SPItem *item = selection->singleItem();
	item->setAttribute("onload", "var src; if (document.documentURI) src = document.documentURI; else if (this.getSrc) src = this.getSrc(); else src = document.location.href + ''; try {parent.preload.load(src);}catch(e) {}", NULL);
}
void script_keydown_options(gboolean show, SPWidget *spw)
{
	GtkWidget *wg_keydown_table;

	wg_keydown_table = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_table"));
	if (wg_keydown_table != NULL) {
		if (!show) {
			gtk_widget_set_sensitive(wg_keydown_table, FALSE);
			gtk_widget_hide(wg_keydown_table);
		} else {
			gtk_widget_set_sensitive(wg_keydown_table, TRUE);
			gtk_widget_show(wg_keydown_table);
		}
	}
}
void script_onchange_save_toggle(GtkWidget *widget, SPWidget *spw)
{
	script_onchange_save(widget, NULL, spw);
}

static gboolean script_onchange_save(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	GtkTreeSelection *selection;
	GtkListStore *store;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTextView *wg_script;
	GtkTextBuffer *buffer;
	GtkTextIter start, end;
	char *script, *evt;
	guint underline;
	
	GtkEntry * wg_key;
	GtkWidget *wg_ctrl, *wg_alt, *wg_shift;
	const char *ckey;
	char *key;
	gboolean bCtrl, bAlt, bShift;

	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(script_list)));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(script_list));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(script_list));

	wg_script = GTK_TEXT_VIEW(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_code"));
	buffer = gtk_text_view_get_buffer(wg_script);
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	script = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
	
	wg_key = GTK_ENTRY(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_key"));	//HSH keydown key
	ckey = gtk_entry_get_text(wg_key);
	//remove whitespaces in key
	key = const_cast<char*>(ckey);
	key = replace(key, " ", "");
	
	wg_ctrl = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_ctrl"));	//HSH keydown ctrl
	bCtrl = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wg_ctrl));
	
	wg_alt = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_alt"));	//HSH keydown ctrl
	bAlt = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wg_alt));

	wg_shift = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_shift"));	//HSH keydown ctrl
	bShift = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wg_shift));
	
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
		gtk_tree_model_get(model, &iter, 0, &evt, -1);
		underline = PANGO_UNDERLINE_SINGLE;

		if (strcmp(evt, "keydown") == 0) { 
			if (strlen(key) == 0 && strlen(script) != 0) {
				underline = PANGO_UNDERLINE_ERROR;
			} else if (strlen(script) == 0 && strlen(key) == 0 && !bCtrl && !bAlt && !bShift) {
				underline = PANGO_UNDERLINE_NONE;		
			} 
		} else  {
			bCtrl = bAlt = bShift = FALSE;
			key = "";
			if (strlen(script) == 0) {
				underline = PANGO_UNDERLINE_NONE;				
			}
		}

		gtk_list_store_set(store, &iter, 1, underline, 2, script, 3, bCtrl, 4, bAlt, 5, bShift, 6, key, -1);		
		
	}
	return 0;
}
void script_onchange(GtkWidget *widget, SPWidget *spw)
{
	GtkListStore *store;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTextView *wg_script;
	GtkTextBuffer *buffer;
	char *scr, *evt;

	GtkWidget *wg_key, *wg_ctrl, *wg_alt, *wg_shift;
	char *key;
	gboolean bCtrl, bAlt, bShift;
	
	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(script_list)));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(script_list));

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)) {
		
		gtk_tree_model_get(model, &iter, 0, &evt, 2, &scr, 3, &bCtrl, 4, &bAlt, 5, &bShift, 6, &key, -1);
		
		script_keydown_options(strcmp(evt, "keydown") == 0, spw);		

		wg_script = GTK_TEXT_VIEW(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_code"));
		buffer = gtk_text_view_get_buffer(wg_script);
		gtk_text_buffer_set_text(buffer, scr, -1);
		g_free(scr);

		wg_key = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_key"));
		gtk_entry_set_text(GTK_ENTRY(wg_key), key);
		g_free(key);
		
		wg_ctrl = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_ctrl"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wg_ctrl), bCtrl);

		wg_alt = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_alt"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wg_alt), bAlt);

		wg_shift = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_shift"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wg_shift), bShift);

	} else {
		script_keydown_options(FALSE, spw);
	}
}
static gboolean isac_ontabchanged(GtkWidget *widget, GtkNotebookPage *page, guint tabindex, SPWidget *spw)
{
	//------------------------------------------------
	//    ISAC Features:
	//    Trigger whenever the ISAC menu tab has changed/switched.
	//------------------------------------------------
	
	nbtypepage = tabindex;

	GtkWidget *nb = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_nb"));
	gint cur_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));
	GtkWidget* child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(nb), cur_page);
	GtkWidget* label = gtk_notebook_get_tab_label(GTK_NOTEBOOK(nb), child);
	const gchar* cur_text = gtk_label_get_text(GTK_LABEL(label));
	if (strcmp(cur_text, "Color ") == 0)
		color_new_list(NULL, spw);
	else if (strcmp(cur_text, "Faceplate ") == 0)
		clone_new_list(NULL, spw);

	GtkWidget* current_page = GTK_WIDGET(page);
	label = gtk_notebook_get_tab_label(GTK_NOTEBOOK(nb), current_page);
	const gchar* label_text = gtk_label_get_text(GTK_LABEL(label));
	if (strcmp(label_text, "Script ") == 0) {
		GtkWidget* selection = GTK_WIDGET(gtk_tree_view_get_selection(GTK_TREE_VIEW(script_list)));
		script_onchange(selection, spw);
	}
	
	return false;
}

static void color_selection(GtkWidget *button, SPWidget *spw)
{
	/*****************************
	ISAC Features:
	Triggered when the color text is changed by clicking on the color piker button under
	tab menu "Color".
	Upon closing the color selection window, the selected color code shall be update
	in the treeview list.
	*****************************/
	GtkTreeSelection *selection;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
	GtkTreeIter iter;
	GtkTreePath *path;
	GtkWidget *colorseldlg;
	gchar *colorstring = "", *ptr;
	char *colorcode;
	gint response;
	GtkColorSelection *colorsel;
	GdkColor color = { 0, 0, 0, 0 };

	/* Create color selection dialog */
	colorseldlg = gtk_color_selection_dialog_new("Color Picker");

	/* Get the ColorSelection widget */
	colorsel = (GtkColorSelection*)gtk_color_selection_dialog_get_color_selection(GTK_COLOR_SELECTION_DIALOG(colorseldlg));
	gtk_color_selection_set_has_palette(colorsel, TRUE);

	/* Show the dialog */
	response = gtk_dialog_run(GTK_DIALOG(colorseldlg));

	if (response == GTK_RESPONSE_OK) {
		gtk_color_selection_get_current_color(colorsel, &color);

		colorstring = g_strdup_printf("#%2X%2X%2X",
			(guint)(scale_round(CLAMP(color.red / 65535.0, 0.0, 1.0), 255)),
			(guint)(scale_round(CLAMP(color.green / 65535.0, 0.0, 1.0), 255)),
			(guint)(scale_round(CLAMP(color.blue / 65535.0, 0.0, 1.0), 255)));

		for (ptr = colorstring; *ptr; ptr++) {
			if (*ptr == ' ')
				*ptr = '0';
		}

		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));

		if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
			gtk_widget_hide(colorseldlg);
		}

		else {
			gtk_tree_model_get(model, &iter, 4, &colorcode, -1);

			if (strlen(colorcode) == 0) gtk_list_store_set(GTK_LIST_STORE(model), &iter, 4, colorstring, -1);
			else gtk_list_store_set(GTK_LIST_STORE(model), &iter, 4, g_strdup_printf("%s/%s", colorcode, colorstring), -1);
		}

		gtk_widget_hide(colorseldlg);
	}

	else {
		gtk_widget_hide(colorseldlg);
	}
	isac_validate_color(spw);
	gtk_tree_path_free(path);
}
static void isac_color_list_onselected(GtkWidget *widget, SPWidget *spw)
{
	GtkWidget *bttn_picker;
	GtkTreeIter iter;
	GtkTreeModel *model;

	bttn_picker = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_color_picker_bttn"));
	GtkWidget *color_find_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_color_find_tag"));

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter))
	{
		gtk_widget_set_sensitive(bttn_picker, TRUE);
		gtk_widget_set_sensitive(color_find_tag, TRUE);
	}
}
void color_list_move_up(GtkWidget *button, GtkWidget *treeview)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	GtkTreePath *patha;
	GtkTreeIter iter, iterb;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))return;

	patha = gtk_tree_model_get_path(model, &iter);
	if (!gtk_tree_path_prev(patha)) return;
	if (!gtk_tree_model_get_iter(model, &iterb, patha))
	{
		gtk_tree_path_free(patha);
		return;
	}
	gtk_tree_path_free(patha);
	gtk_list_store_swap(GTK_LIST_STORE(model), &iterb, &iter);
}
void color_list_move_down(GtkWidget *button, GtkWidget *treeview)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	GtkTreePath *patha;
	GtkTreeIter iter, iterb;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))return;

	patha = gtk_tree_model_get_path(model, &iter);
	gtk_tree_path_next(patha);
	if (!gtk_tree_model_get_iter(model, &iterb, patha))
	{
		gtk_tree_path_free(patha);
		return;
	}
	gtk_tree_path_free(patha);
	gtk_list_store_swap(GTK_LIST_STORE(model), &iterb, &iter);
}
void color_delete_item(GtkWidget *button, SPWidget *spw)
{
	GtkWidget *bttn_off;
	GtkTreeSelection *selection;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
	if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return;

	gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
	isac_validate_color(spw);

	bttn_off = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_color_picker_bttn"));
	gtk_widget_set_sensitive(bttn_off, FALSE);
	GtkWidget *color_find_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_color_find_tag"));
	gtk_widget_set_sensitive(color_find_tag, FALSE);
}
void color_list_edited_column1(GtkCellRendererText *cellrt, const char *path_string, const char *new_text, SPWidget *spw)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
	GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
	g_curr_gtreepath = gtk_tree_path_copy((const GtkTreePath *)path);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));

	// store the string
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, new_text, -1);

	// path: unselect all and select the current row/path
	gtk_tree_selection_unselect_all(selection);
	gtk_tree_selection_unselect_iter(selection, &iter);
	gtk_tree_selection_select_path(selection, path);

	// cancel editing current col
	gtk_cell_renderer_stop_editing(renderer1, TRUE); //g_signal_emit_by_name(G_OBJECT(renderer1), "editing-canceled");

	// get iter of current row/path
	gboolean gotiterfromstr = gtk_tree_model_get_iter_from_string(model, &iter, path_string);
	gtk_tree_model_iter_next(model, &iter); // now pointing to next cell in same col/path

	// point to next iter/cell in current row/path
	gtk_tree_selection_select_path(selection, path);
	if (gtk_widget_is_focus(color_list))
		gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(color_list), path, column2, renderer2, TRUE);

	gtk_tree_path_free(path);
	isac_validate_color(spw);

	return;
}
void color_list_edited_column2(GtkCellRendererText *cellrt, const char *path_string, const char *new_text, SPWidget *spw)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
	GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
	g_curr_gtreepath = gtk_tree_path_copy((const GtkTreePath *)path);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));

	// store the string
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, new_text, -1);

	// path: unselect all and select the current row/path
	gtk_tree_selection_unselect_all(selection);
	gtk_tree_selection_unselect_iter(selection, &iter);
	gtk_tree_selection_select_path(selection, path);

	// cancel editing current col
	gtk_cell_renderer_stop_editing(renderer2, TRUE); //g_signal_emit_by_name(G_OBJECT(renderer2), "editing-canceled");

	// get iter of current row/path
	gboolean gotiterfromstr = gtk_tree_model_get_iter_from_string(model, &iter, path_string);
	gtk_tree_model_iter_next(model, &iter); // now pointing to next cell in same col/path

	// point to next iter/cell in current row/path
	gtk_tree_selection_select_path(selection, path);
	if (gtk_widget_is_focus(color_list))
		gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(color_list), path, column3, renderer3, TRUE);

	gtk_tree_path_free(path);
	isac_validate_color(spw);

	return;
}
void color_list_edited_column3(GtkCellRendererText *cellrt, const char *path_string, const char *new_text, SPWidget *spw)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
	GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
	g_curr_gtreepath = gtk_tree_path_copy(path);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));

	char *tag, *value, *code;
	gint n_emptyrow = 0;

	// store the string
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, 4, new_text, -1);

	// create a new empty row if there is no empty row
	gtk_tree_model_get_iter_first(model, &iter);
	while (gtk_tree_model_iter_next(model, &iter))
	{
		gtk_tree_model_get(model, &iter, 0, &tag, 2, &value, 4, &code, -1);
		if (strlen(tag) == 0 && strlen(value) == 0 && strlen(code) == 0)
			n_emptyrow++;
	}

	if (n_emptyrow == 0)
	{
		gtk_list_store_append(GTK_LIST_STORE(model), &iter);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, "", 1, 1, 2, "", 3, 1, 4, "", 5, 1, -1);
	}

	// path: unselect all and select the next row/path
	gtk_tree_selection_unselect_all(selection);
	gtk_tree_selection_unselect_iter(selection, &iter);
	gtk_tree_path_next(path);

	// cancel editing current col
	gtk_cell_renderer_stop_editing(renderer3, TRUE); //g_signal_emit_by_name(G_OBJECT(renderer3), "editing-canceled");

	// get iter of next row/path
	gboolean gotiterfrompath = gtk_tree_model_get_iter(model, &iter, path);

	// point to next iter/cell in current row/path
	gtk_tree_selection_select_path(selection, path);
	if (gtk_widget_is_focus(color_list))
		gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(color_list), path, column1, renderer1, TRUE);

	gtk_tree_path_free(path);
	isac_validate_color(spw);

	return;
}

void color_list_editing_started(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path, SPWidget *spw)
{//HSH - Callback function for "editing-started" signal in GtkCellRenderer set in isac_menu_color_init function
	if (GTK_IS_ENTRY (editable))	//check if the GtkCellEditable is of type GtkEntry
    {
      currentEntry = GTK_ENTRY (editable);	//set the global currentEntry      
      currentRenderer = cell;	//set the global currentRenderer
    }
}


static gboolean color_list_on_key_press (GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{//HSH - key press event callback for color list (specifically made for TAB button for now)
	switch (event->keyval)
	{
		case GDK_Tab:
			if (color_list && currentEntry && GTK_IS_ENTRY (currentEntry) && currentRenderer)
			{
				GtkTreeIter iter;
				GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
				////GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
				////g_curr_gtreepath = gtk_tree_path_copy((const GtkTreePath *)path);
				GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
				if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return FALSE;	//set iter; do not carry on if cannot get iter
				GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
				
				//// store the string depending on the current renderer				
				if (currentRenderer == renderer1)
					gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, gtk_entry_get_text(currentEntry), -1);
				else if (currentRenderer == renderer2)
					gtk_list_store_set(GTK_LIST_STORE(model), &iter, 2, gtk_entry_get_text(currentEntry), -1);
				else if (currentRenderer == renderer3)
					gtk_list_store_set(GTK_LIST_STORE(model), &iter, 4, gtk_entry_get_text(currentEntry), -1);
				
				if (currentRenderer == renderer3)
				{
					char *tag, *value, *code;
					gint n_emptyrow = 0;

					// create a new empty row if there is no empty row
					gtk_tree_model_get_iter_first(model, &iter);
					while (gtk_tree_model_iter_next(model, &iter))
					{
						gtk_tree_model_get(model, &iter, 0, &tag, 2, &value, 4, &code, -1);
						if (strlen(tag) == 0 && strlen(value) == 0 && strlen(code) == 0)
							n_emptyrow++;
					}

					if (n_emptyrow == 0)
					{
						gtk_list_store_append(GTK_LIST_STORE(model), &iter);
						gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, "", 1, 1, 2, "", 3, 1, 4, "", 5, 1, -1);
					}
				}

				//// path: unselect all and select the current row/path
				gtk_tree_selection_unselect_all(selection);
				gtk_tree_selection_unselect_iter(selection, &iter);
				if (currentRenderer == renderer3)
					gtk_tree_path_next(path);
				else
					gtk_tree_selection_select_path(selection, path);

				//// cancel editing current col
				gtk_cell_renderer_stop_editing(currentRenderer, TRUE); //g_signal_emit_by_name(G_OBJECT(renderer1), "editing-canceled");

				//// get iter of current row/path
				//gboolean gotiterfromstr = gtk_tree_model_get_iter_from_string(model, &iter, path_string);
				gtk_tree_model_iter_next(model, &iter); // now pointing to next cell in same col/path
				
				//// point to next iter/cell in current row/path
				gtk_tree_selection_select_path(selection, path);
				if (gtk_widget_is_focus(color_list))
				{
					if (currentRenderer == renderer1)
						gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(color_list), path, column2, renderer2, TRUE);
					else if (currentRenderer == renderer2)
						gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(color_list), path, column3, renderer3, TRUE);
					else if (currentRenderer == renderer3)
						gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(color_list), path, column1, renderer1, TRUE);
				}

				gtk_tree_path_free(path);
				isac_validate_color(spw);
				return TRUE;	//skips the default TAB button behaviour (erase unstored text and go back to first column)
			}			
			return FALSE;	//does default TAB button behaviour when not in edit/entry mode
		default:
			return FALSE; 
	}

	return FALSE; 
}

void color_new_list(GtkWidget *button, SPWidget *spw)
{ // called when "Add" button in Color tab is clicked. param GtkWidget *button not used
	//GtkWidget *bttn_off;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
	GtkTreeIter iter;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
	gtk_tree_selection_unselect_all(selection);
	GtkTreePath *path = gtk_tree_path_new_first();
	char *tag, *value, *code;
	gint count = 0;

	while (gtk_tree_model_get_iter(model, &iter, path))
	{
		gtk_tree_model_get(model, &iter, 0, &tag, 2, &value, 4, &code, -1);
		if (strlen(tag) == 0 && strlen(value) == 0 && strlen(code) == 0)
		{
			gtk_tree_selection_select_path(selection, path);
			gtk_tree_view_set_cursor(GTK_TREE_VIEW(color_list), path, gtk_tree_view_get_column(GTK_TREE_VIEW(color_list), 0), TRUE);
			return;
		}
		gtk_tree_path_next(path);
	}

	// if there is no data, create a new empty row
	gtk_list_store_append(GTK_LIST_STORE(model), &iter);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, "", 1, 1, 2, "", 3, 1, 4, "", 5, 1, -1);

	// select the specified iterator
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
	gtk_tree_selection_select_iter(selection, &iter);
	path = gtk_tree_model_get_path(model, &iter); // why disable?
	gtk_tree_selection_select_path(selection, path);

	// set cursor to empty cell
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(color_list), path, NULL, FALSE, 0, 0);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW(color_list), path, gtk_tree_view_get_column(GTK_TREE_VIEW(color_list), 0), TRUE);

	gtk_tree_path_free(path);

	return;
}
void color_deletelastrow()
{
	if (NULL == color_list)
	{
		return;
	}

	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));	
	GtkTreeIter iter;
	GtkTreePath *path = gtk_tree_path_new_first();
	char *tag, *value, *code;
	gtk_tree_model_get_iter_first(model, &iter);
	while (gtk_tree_model_get_iter(model, &iter, path))
	{
		gtk_tree_model_get(model, &iter, 0, &tag, 2, &value, 4, &code, -1);
		if (strlen(tag) == 0 && strlen(value) == 0 && strlen(code) == 0)
		{
			gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
		}
		gtk_tree_path_next(path);
	}

	//gtk_tree_model_get_iter_first(model, &iter);
	//while (gtk_tree_model_iter_next(model, &iter))
	//{
	//	gtk_tree_model_get(model, &iter, 0, &tag, 2, &value, 4, &code, -1);
	//	if (strlen(tag) == 0 && strlen(value) == 0 && strlen(code) == 0)
	//		gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
	//	gtk_tree_path_next(path);
	//}

	//gint rows = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(model), NULL);

	//path = gtk_tree_path_new_from_indices(rows - 1, -1);
	//if (gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path))
	//{
	//	gtk_tree_model_get(model, &iter, 0, &tag, 2, &value, 4, &code, -1);
	//	if (strlen(tag) == 0 && strlen(value) == 0 && strlen(code) == 0)
	//	{
	//		gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
	//	}
	//}

	gtk_tree_path_free(path);

	return;
}

//HSH, clone_list functions
gboolean clone_list_get_iter_first_incomplete(GtkTreeIter* iter) 
{
	if (NULL == iter)
		return FALSE;

	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
	if (gtk_tree_model_get_iter_first(model, iter)) {	//set iter to first item
		gchar *var, *val;
		do {
			gtk_tree_model_get(model, iter, CLONE_VAR_TEXT, &var, CLONE_VAL_TEXT, &val, -1);
			if (strlen(var) == 0 || strlen(val) == 0) {
				return TRUE;	//done: iter should be set
			}

		} while (gtk_tree_model_iter_next(model, iter));
	}
	return FALSE; //empty or incomplete not found
}

void clone_list_add_item(gchar* var, gchar* val, GtkTreeIter* iter) 
{
	if (NULL == var || NULL == val || NULL == clone_list || NULL == iter)
		return;

	//this function will add a new line to the list.
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
	gtk_list_store_append(GTK_LIST_STORE(model), iter);
	gtk_list_store_set(GTK_LIST_STORE(model), iter, 
		CLONE_VAR_TEXT	, var , CLONE_VAR_EDITABLE  , TRUE , 
		CLONE_EQUAL_TEXT, "=", CLONE_EQUAL_EDITABLE, FALSE, 
		CLONE_VAL_TEXT	, val , CLONE_VAL_EDITABLE  , TRUE , -1);
	return;
}

void clone_list_store_current_entry()
{
	if (clone_list && currentEntry && GTK_IS_ENTRY (currentEntry) && currentRenderer)
	{
		GtkTreeIter iter;
		GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clone_list));
		if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return;	//set iter; do not carry on if cannot get iter
		
		//// store the string depending on the current renderer				
		if (currentRenderer == clone_renderer1)
			gtk_list_store_set(GTK_LIST_STORE(model), &iter, CLONE_VAR_TEXT, gtk_entry_get_text(currentEntry), -1);
		else if (currentRenderer == clone_renderer3)
			gtk_list_store_set(GTK_LIST_STORE(model), &iter, CLONE_VAL_TEXT, gtk_entry_get_text(currentEntry), -1);
	}
}

void clone_delete_item(GtkWidget *button, SPWidget *spw)
{
	clone_delete_selected_item();
	isac_validate_clone(spw);
}

void clone_delete_selected_item()
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clone_list));
	if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return;

	gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

	//select next item after deletion
	GtkTreePath* path = gtk_tree_model_get_path(model, &iter);

	gtk_tree_selection_select_path(selection, path);

	gtk_tree_path_free(path);
}

void clone_list_edited_column1(GtkCellRendererText *cellrt, const char *path_string, const char *new_text, SPWidget *spw)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
	GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
	g_curr_gtreepath = gtk_tree_path_copy((const GtkTreePath *)path);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clone_list));

	// store the string
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, CLONE_VAR_TEXT, new_text, -1);
	
	// create a new empty row if there are no empty row
	/*gchar *var, *val;
	gint n_emptyrow = 0;
	gtk_tree_model_get_iter_first(model, &iter);
	do {
		gtk_tree_model_get(model, &iter, CLONE_VAR_TEXT, &var, CLONE_VAL_TEXT, &val, -1);
		if (strlen(var) == 0 || strlen(val) == 0)
			n_emptyrow++;
	} while (gtk_tree_model_iter_next(model, &iter));

	if (n_emptyrow == 0)
	{
		gtk_list_store_append(GTK_LIST_STORE(model), &iter);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter, 
			CLONE_VAR_TEXT	, "" , CLONE_VAR_EDITABLE  , TRUE , 
			CLONE_EQUAL_TEXT, "=", CLONE_EQUAL_EDITABLE, FALSE, 
			CLONE_VAL_TEXT	, "" , CLONE_VAL_EDITABLE  , TRUE , -1);		
	}*/
	if (!clone_list_get_iter_first_incomplete(&iter)) {
		clone_list_add_item("", "", &iter);
	}

	// path: unselect all and select the current row/path
	gtk_tree_selection_unselect_all(selection);
	gtk_tree_selection_unselect_iter(selection, &iter);
	gtk_tree_selection_select_path(selection, path);

	// cancel editing current col
	gtk_cell_renderer_stop_editing(clone_renderer1, TRUE); //g_signal_emit_by_name(G_OBJECT(renderer1), "editing-canceled");

	// get iter of current row/path
	gboolean gotiterfromstr = gtk_tree_model_get_iter_from_string(model, &iter, path_string);
	gtk_tree_model_iter_next(model, &iter); // now pointing to next cell in same col/path

	// point to next iter/cell in current row/path
	gtk_tree_selection_select_path(selection, path);
	if (gtk_widget_is_focus(clone_list))
		gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(clone_list), path, clone_column2, clone_renderer2, TRUE);

	gtk_tree_path_free(path);
	isac_validate_clone(spw);

	return;
}
void clone_list_edited_column2(GtkCellRendererText *cellrt, const char *path_string, const char *new_text, SPWidget *spw)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
	GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
	g_curr_gtreepath = gtk_tree_path_copy((const GtkTreePath *)path);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clone_list));

	// store the string
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, CLONE_EQUAL_TEXT, new_text, -1);
	
	// create a new empty row if there are no empty row
	/*gchar *var, *val;
	gint n_emptyrow = 0;
	gtk_tree_model_get_iter_first(model, &iter);
	do {
		gtk_tree_model_get(model, &iter, CLONE_VAR_TEXT, &var, CLONE_VAL_TEXT, &val, -1);
		if (strlen(var) == 0 || strlen(val) == 0)
			n_emptyrow++;
	} while (gtk_tree_model_iter_next(model, &iter));

	if (n_emptyrow == 0)
	{
		gtk_list_store_append(GTK_LIST_STORE(model), &iter);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter, 
			CLONE_VAR_TEXT	, "" , CLONE_VAR_EDITABLE  , TRUE , 
			CLONE_EQUAL_TEXT, "=", CLONE_EQUAL_EDITABLE, FALSE, 
			CLONE_VAL_TEXT	, "" , CLONE_VAL_EDITABLE  , TRUE , -1);		
	}*/
	if (!clone_list_get_iter_first_incomplete(&iter)) {
		clone_list_add_item("", "", &iter);
	}

	// path: unselect all and select the current row/path
	gtk_tree_selection_unselect_all(selection);
	gtk_tree_selection_unselect_iter(selection, &iter);
	gtk_tree_selection_select_path(selection, path);

	// cancel editing current col
	gtk_cell_renderer_stop_editing(renderer2, TRUE); //g_signal_emit_by_name(G_OBJECT(renderer2), "editing-canceled");

	// get iter of current row/path
	gboolean gotiterfromstr = gtk_tree_model_get_iter_from_string(model, &iter, path_string);
	gtk_tree_model_iter_next(model, &iter); // now pointing to next cell in same col/path

	// point to next iter/cell in current row/path
	gtk_tree_selection_select_path(selection, path);
	if (gtk_widget_is_focus(clone_list))
		gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(clone_list), path, clone_column3, clone_renderer3, TRUE);

	gtk_tree_path_free(path);
	isac_validate_clone(spw);

	return;
}
void clone_list_edited_column3(GtkCellRendererText *cellrt, const char *path_string, const char *new_text, SPWidget *spw)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
	GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
	g_curr_gtreepath = gtk_tree_path_copy(path);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clone_list));

	char *var, *val;
	gint n_emptyrow = 0;
	
	// store the string
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter, CLONE_VAL_TEXT, new_text, -1);

	// create a new empty row if there are no empty row
	/*gtk_tree_model_get_iter_first(model, &iter);
	do {
		gtk_tree_model_get(model, &iter, CLONE_VAR_TEXT, &var, CLONE_VAL_TEXT, &val, -1);
		if (strlen(var) == 0 || strlen(val) == 0)
			n_emptyrow++;
	} while (gtk_tree_model_iter_next(model, &iter));

	if (n_emptyrow == 0)
	{
		gtk_list_store_append(GTK_LIST_STORE(model), &iter);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter, 
			CLONE_VAR_TEXT	, "" , CLONE_VAR_EDITABLE  , TRUE , 
			CLONE_EQUAL_TEXT, "=", CLONE_EQUAL_EDITABLE, FALSE, 
			CLONE_VAL_TEXT	, "" , CLONE_VAL_EDITABLE  , TRUE , -1);		
	}*/
	if (!clone_list_get_iter_first_incomplete(&iter)) {
		clone_list_add_item("", "", &iter);
	}

	// path: unselect all and select the next row/path
	gtk_tree_selection_unselect_all(selection);
	gtk_tree_selection_unselect_iter(selection, &iter);
	gtk_tree_path_next(path);

	// cancel editing current col
	gtk_cell_renderer_stop_editing(renderer3, TRUE); //g_signal_emit_by_name(G_OBJECT(renderer3), "editing-canceled");

	// get iter of next row/path
	gboolean gotiterfrompath = gtk_tree_model_get_iter(model, &iter, path);

	// point to next iter/cell in current row/path
	gtk_tree_selection_select_path(selection, path);
	if (gtk_widget_is_focus(clone_list))
		gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(clone_list), path, clone_column1, clone_renderer1, TRUE);

	gtk_tree_path_free(path);
	isac_validate_clone(spw);

	return;
}

void clone_list_editing_started(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path, SPWidget *spw)
{//HSH - Callback function for "editing-started" signal in GtkCellRenderer set in isac_menu_color_init function
	if (GTK_IS_ENTRY (editable))	//check if the GtkCellEditable is of type GtkEntry
    {
      currentEntry = GTK_ENTRY (editable);	//set the global currentEntry      
      currentRenderer = cell;	//set the global currentRenderer
    }
}

static gboolean clone_list_on_key_press (GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{//HSH - key press event callback for clone list (specifically made for TAB button for now)
	switch (event->keyval)
	{
		case GDK_Tab:
			if (clone_list) {
				GtkTreeIter iter;
				GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
				GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clone_list));
				if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
					//No selection
					//Find first line that is incomplete and start editing that data
					if (!clone_list_get_iter_first_incomplete(&iter))
						clone_list_add_item("", "", &iter);	//this functions adds a new line and iter will be set to this new line if not NULL
					
					//get path for the incomplete/ new  line
					GtkTreePath* path = gtk_tree_model_get_path(model, &iter);

					//start editing this line at first column (value)
					gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(clone_list), path, clone_column1, clone_renderer1, TRUE);

					gtk_tree_path_free(path);

				} else {
					GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
					
					//At this point there's a selection
					if (currentEntry && GTK_IS_ENTRY(currentEntry) && currentRenderer) {
						//editing data
						// store the string depending on the current renderer				
						if (currentRenderer == clone_renderer1)
							gtk_list_store_set(GTK_LIST_STORE(model), &iter, CLONE_VAR_TEXT, gtk_entry_get_text(currentEntry), -1);
						else if (currentRenderer == clone_renderer3)
							gtk_list_store_set(GTK_LIST_STORE(model), &iter, CLONE_VAL_TEXT, gtk_entry_get_text(currentEntry), -1);

						char *var, *val;
						
						gint n_emptyrow = 0;

						// create a new empty row if there is no empty row
						gtk_tree_model_get_iter_first(model, &iter);	//get first line
						do {
							gtk_tree_model_get(model, &iter, CLONE_VAR_TEXT, &var, CLONE_VAL_TEXT, &val, -1);
							if (strlen(var) == 0 || strlen(val) == 0)
								n_emptyrow++;
						}while (gtk_tree_model_iter_next(model, &iter));	//get next line
						
						if (n_emptyrow == 0)
						{
							clone_list_add_item("", "", &iter);
						}
						
						// path: unselect all and select the current row/path
						gtk_tree_selection_unselect_all(selection);
						gtk_tree_selection_unselect_iter(selection, &iter);
						if (currentRenderer == clone_renderer3)
							gtk_tree_path_next(path);
						else
							gtk_tree_selection_select_path(selection, path);

						// cancel editing current col
						gtk_cell_renderer_stop_editing(currentRenderer, TRUE); 

						// get iter of current row/path
						gtk_tree_model_iter_next(model, &iter); // now pointing to next cell in same col/path
						
						// point to next iter/cell in current row/path
						gtk_tree_selection_select_path(selection, path);
						if (gtk_widget_is_focus(clone_list))
						{
							if (currentRenderer == clone_renderer1)
								gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(clone_list), path, clone_column3, clone_renderer3, TRUE);	//go to column3						
							else if (currentRenderer == clone_renderer3)
								gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(clone_list), path, clone_column1, clone_renderer1, TRUE);
						}
					} else {
						//selection
						//pressing tab will start editing first column
						
						//start editing this line at first column (value)
						gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(clone_list), path, clone_column1, clone_renderer1, TRUE);
					}
					gtk_tree_path_free(path);
				}				
				isac_validate_clone(spw);
				return TRUE; //skips the default TAB button behaviour (erase unstored text and go back to first column)
			}					
			break;
		case GDK_Delete:
			if (clone_list) {
				clone_delete_selected_item();
				isac_validate_clone(spw);
				return TRUE;
			}			
			break;
		default:
			return FALSE; 
	}

	return FALSE; 
}

void clone_new_list(GtkWidget *button, SPWidget *spw)
{ // called when "Add" button in Clone tab is clicked. param GtkWidget *button not used
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
	GtkTreeIter iter;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clone_list));
	gtk_tree_selection_unselect_all(selection);
	GtkTreePath *path = gtk_tree_path_new_first();
	char *var, *val;
	
	while (gtk_tree_model_get_iter(model, &iter, path))
	{
		gtk_tree_model_get(model, &iter, CLONE_VAR_TEXT, &var, CLONE_VAL_TEXT, &val, -1);
		if (strlen(var) == 0 || strlen(val) == 0)
		{
			gtk_tree_selection_select_path(selection, path);
			gtk_tree_view_set_cursor(GTK_TREE_VIEW(clone_list), path, gtk_tree_view_get_column(GTK_TREE_VIEW(clone_list), 0), TRUE);
			return;	//Found incomplete entry, cannot add new line.
		}
		gtk_tree_path_next(path);
	}

	clone_list_add_item("", "", &iter);

	// select the specified iterator
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clone_list));
	gtk_tree_selection_select_iter(selection, &iter);
	path = gtk_tree_model_get_path(model, &iter); // why disable?
	//gtk_tree_selection_select_path(selection, path);

	// set cursor to empty cell
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(clone_list), path, NULL, FALSE, 0, 0);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW(clone_list), path, gtk_tree_view_get_column(GTK_TREE_VIEW(clone_list), 0), TRUE);

	gtk_tree_path_free(path);

	return;
}

void clone_list_move_up(GtkWidget *button, GtkWidget *treeview)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	GtkTreePath *patha;
	GtkTreeIter iter, iterb;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))return;

	patha = gtk_tree_model_get_path(model, &iter);
	if (!gtk_tree_path_prev(patha)) return;
	if (!gtk_tree_model_get_iter(model, &iterb, patha))
	{
		gtk_tree_path_free(patha);
		return;
	}
	gtk_tree_path_free(patha);
	gtk_list_store_swap(GTK_LIST_STORE(model), &iterb, &iter);
}

void clone_list_move_down(GtkWidget *button, GtkWidget *treeview)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	GtkTreePath *patha;
	GtkTreeIter iter, iterb;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))return;

	patha = gtk_tree_model_get_path(model, &iter);
	gtk_tree_path_next(patha);
	if (!gtk_tree_model_get_iter(model, &iterb, patha))
	{
		gtk_tree_path_free(patha);
		return;
	}
	gtk_tree_path_free(patha);
	gtk_list_store_swap(GTK_LIST_STORE(model), &iterb, &iter);
}

void clone_deletelastrow()
{
	if (NULL == clone_list)
	{
		return;
	}
	
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));	
	GtkTreeIter iter;
	GtkTreePath *path = gtk_tree_path_new_first();
	char *var, *val;
	gtk_tree_model_get_iter_first(model, &iter);
	while (gtk_tree_model_get_iter(model, &iter, path))
	{
		gtk_tree_model_get(model, &iter, CLONE_VAR_TEXT, &var, CLONE_VAL_TEXT, &val, -1);
		if (strlen(var) == 0 && strlen(val) == 0)
		{
			gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
		}
		gtk_tree_path_next(path);
	}

	gtk_tree_path_free(path);

	return;
}


// naim@ecava.com
void get_tags_by_index(int pageindex)
{
/*
	if (done_loading_tag_names == true)
	{
		done_loading_tag_names = false;

		// below is to set pageindex not go lower than -1
		if (pageindex == 0 && is_pageindex_zero == false)
			is_pageindex_zero = true;
		if (pageindex > 0)
			is_pageindex_zero = false;
		else if (pageindex == -1)
		{
			is_pageindex_zero = false;
			is_pageindex_last = true;
		}
		else if (pageindex < -1)
		{
			pageindex = 0;
			is_pageindex_zero = false;
		}

		memset(passed_to_dll, 0, 2560);
		IgGetTagsByIndex(passed_to_dll, pageindex);

		// replace empty field in column 'group_name' in table 'alarm' to Ungrouped
		char *temp1 = replace(passed_to_dll, "app.alarm[]", "app.alarm[Ungrouped]");
		char *temp2 = replace(temp1, "app.alarm[.", "app.alarm[Ungrouped.");

		// convert back the char *long_tag_names to vector<string> taglist_vector
		std::string s(temp2); //std::string s(passed_to_dll);
		std::string delimiter = "\n";
		size_t pos = 0;
		std::string token;
		taglist_vector.clear(); // clear the vector first
		gtk_list_store_clear(tag_store1); // and also the GtkListStore
		gtk_list_store_clear(tag_store2); // and also the GtkListStore
		while ((pos = s.find(delimiter)) != std::string::npos)
		{
			token = s.substr(0, pos);
			taglist_vector.push_back(token);
			s.erase(0, pos + delimiter.length());
		}
		//taglist_vector.push_back(token);

		// push all strings into store
		GtkTreeIter iter1;
		GtkTreeIter iter2;
		int iterator1 = 0;
		for (std::vector<std::string>::iterator i = taglist_vector.begin(); i != taglist_vector.end(); i++)
		{
			if (iterator1 < 10)
			{
				std::string string1 = *i;
				gtk_list_store_append(tag_store1, &iter1);
				gtk_list_store_set(tag_store1, &iter1, 0, string1.c_str(), -1);
			}
			else
			{
				std::string string1 = *i;
				gtk_list_store_append(tag_store2, &iter2);
				gtk_list_store_set(tag_store2, &iter2, 0, string1.c_str(), -1);
			}
			iterator1++;
		}
		done_loading_tag_names = true;
	}

	*/
	return;
}
void get_tags_by_keyword_index(char *keyword, int pageindex)
{
	/*
	if (done_loading_tag_names2 == true)
	{
		done_loading_tag_names2 = false;

		// below is to set pageindex not go lower than -1
		if (pageindex == 0 && is_pageindex_zero2 == false)
			is_pageindex_zero2 = true;
		if (pageindex > 0)
			is_pageindex_zero2 = false;
		else if (pageindex == -1)
		{
			is_pageindex_zero2 = false;
			is_pageindex_last2 = true;
		}
		else if (pageindex < -1)
		{
			pageindex = 0;
			is_pageindex_zero2 = false;
		}

		//if(strcmp(keyword, "*") == 0) // fix this bug
		//	keyword = "%" ??????????????????????????????????????????????????

		if (strcmp(keyword, "") != 0) // if the keyword from search_entry is not empty,
		{
			memset(passed_to_dll, 0, 2560);
			IgGetTagsByKeywordIndex(passed_to_dll, keyword, pageindex);

			//// replace empty field in column 'group_name' in table 'alarm' to Ungrouped
			//char *temp1 = replace(passed_to_dll, "app.alarm[]", "app.alarm[Ungrouped]");
			//char *temp2 = replace(temp1, "app.alarm[.", "app.alarm[Ungrouped.");

			// convert back the char *long_tag_names to vector<string> taglist_vector
			//std::string s(temp2); //std::string s(passed_to_dll);
			std::string s(passed_to_dll);
			std::string delimiter = "\n";
			size_t pos = 0;
			std::string token;
			taglist_vector.clear(); // clear the vector first
			gtk_list_store_clear(tag_store1); // and also the GtkListStore
			gtk_list_store_clear(tag_store2); // and also the GtkListStore
			while ((pos = s.find(delimiter)) != std::string::npos)
			{
				token = s.substr(0, pos);
				taglist_vector.push_back(token);
				s.erase(0, pos + delimiter.length());
			}
			//taglist_vector.push_back(token);

			// push all strings into store
			GtkTreeIter iter1;
			GtkTreeIter iter2;
			int iterator1 = 0;
			for (std::vector<std::string>::iterator i = taglist_vector.begin(); i != taglist_vector.end(); i++)
			{
				if (iterator1 < 10)
				{
					std::string string1 = *i;
					gtk_list_store_append(tag_store1, &iter1);
					gtk_list_store_set(tag_store1, &iter1, 0, string1.c_str(), -1);
				}
				else
				{
					std::string string1 = *i;
					gtk_list_store_append(tag_store2, &iter2);
					gtk_list_store_set(tag_store2, &iter2, 0, string1.c_str(), -1);
				}
				iterator1++;
			}
		}
		done_loading_tag_names2 = true;
	}
*/
	return;
}
gboolean findtag_search_entry_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	/*
	(void)user_data;

	if (!strcmp(gdk_keyval_name(event->keyval), "Return"))
	{
		const gchar *keyword = gtk_entry_get_text(GTK_ENTRY(search_entry));
		const char *_keyword = (const char*)keyword;

		// strip non-alphanumeric chars
		std::string str1(_keyword);
		//for(std::string::iterator i = str1.end() - 1; i >= str1.begin(); --i)
		//{
		//	if (!isalpha(*i) && !isdigit(*i))
		//		str1.erase(i);
		//}
		char *keyword2 = (char *)str1.c_str();
		gtk_entry_set_text(GTK_ENTRY(search_entry), keyword2);

		gchar *_igx_filename;
		_igx_filename = (gchar *)g_object_get_data(G_OBJECT(search_entry), "search_entry"); //_igx_filename = (gchar *)g_object_get_data(G_OBJECT(search_entry), "search");
		char *igx_filename = (char *)_igx_filename;

		// on-load is igx_filename, user chooses g_igx_filename
		if (strcmp(g_igx_filename, "") != 0) // if there is a filename chosen by user,
			igx_filename = g_igx_filename; // overwrite the on-load filename
		else
			g_igx_filename = igx_filename;

		if (strcmp(_keyword, "") != 0 && g_igx_filename != "" && done_loading_tag_names == true && is_igx_open == true) // if not empty, that is there is a keyword in search_entry
		{
			//char *filename = "C:\\Users\\Ecava\\Documents\\IntegraXor Projects\\Demo\\DEM0.igx";
			findtag_rowCount2 = IgGetTagsKeywordRowCount(g_igx_filename, (char *)_keyword);
			findtag_pageCount2 = (findtag_rowCount2 / 20) + 0;
			if (findtag_rowCount2 % 20 == 0)
				findtag_pageCount2--;

			// set the label
			std::stringstream stringstream1;
			if (findtag_rowCount2 > 0)
			{
				if(findtag_rowCount2 < 20)
					stringstream1 << "Page: 1/1";
				else
					stringstream1 << "Page: 1/" << (findtag_pageCount2 + 1);
				//stringstream1 << "Found: " << findtag_rowCount2 << " tags";
			}
			else
				stringstream1 << "Page: 1/1";
			std::string string1 = stringstream1.str();
			gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());

			get_tags_by_keyword_index((char *)_keyword, 0);

			use_search_result = true;
		}
		else if (strcmp(_keyword, "") == 0 && g_igx_filename != "" && done_loading_tag_names == true && is_igx_open == true) // if keyword is empty, do reset
		{
			resetbutton_clicked_event();
		}
	}
*/
	return FALSE;
}
void findtag_search_entry_changed_event(GtkEditable *editable, gpointer user_data)
{
	/*
	(void)editable;
	(void)user_data;

	const gchar *keyword = gtk_entry_get_text(GTK_ENTRY(search_entry));
	const char *_keyword = (const char*)keyword;

	// strip non-alphanumeric chars
	std::string str1(_keyword);
	//for(std::string::iterator i = str1.end() - 1; i >= str1.begin(); --i)
	//{
	//	if (!isalpha(*i) && !isdigit(*i))
	//		str1.erase(i);
	//}
	char *keyword2 = (char *)str1.c_str();
	gtk_entry_set_text(GTK_ENTRY(search_entry), keyword2);

	gchar *_igx_filename;
	_igx_filename = (gchar *)g_object_get_data(G_OBJECT(search_entry), "search_entry"); //_igx_filename = (gchar *)g_object_get_data(G_OBJECT(search_entry), "search");
	char *igx_filename = (char *)_igx_filename;

	// on-load is igx_filename, user chooses g_igx_filename
	if (strcmp(g_igx_filename, "") != 0) // if there is a filename chosen by user,
		igx_filename = g_igx_filename; // overwrite the on-load filename
	else
		g_igx_filename = igx_filename;

	if (strcmp(_keyword, "") != 0 && g_igx_filename != "" && done_loading_tag_names == true && is_igx_open == true) // if not empty, that is there is a keyword in search_entry
	{
		//char *filename = "C:\\Users\\Ecava\\Documents\\IntegraXor Projects\\Demo\\DEM0.igx";
		findtag_rowCount2 = IgGetTagsKeywordRowCount(g_igx_filename, (char *)_keyword);
		findtag_pageCount2 = (findtag_rowCount2 / 20) + 0;
		if (findtag_rowCount2 % 20 == 0)
			findtag_pageCount2--;

		// set the label
		std::stringstream stringstream1;
		if (findtag_rowCount2 > 0)
		{
			if(findtag_rowCount2 < 20)
				stringstream1 << "Page: 1/1";
			else
				stringstream1 << "Page: 1/" << (findtag_pageCount2 + 1);
			//stringstream1 << "Found: " << findtag_rowCount2 << " tags";
		}
		else
			stringstream1 << "Page: 1/1";
		std::string string1 = stringstream1.str();
		gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());

		get_tags_by_keyword_index((char *)_keyword, 0);

		use_search_result = true;
	}
	else if (strcmp(_keyword, "") == 0 && g_igx_filename != "" && done_loading_tag_names == true && is_igx_open == true)
	{
		resetbutton_clicked_event();
	}
	*/
	return;
}

void findbutton_clicked_event(GtkButton *button, GtkWidget *search_entry)
{
/*
	gtk_window_set_keep_below(GTK_WINDOW(findtag_window), TRUE);

	GtkWidget *dialog = gtk_file_chooser_dialog_new("Open *.igx file",
		GTK_WINDOW(findtag_window),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{

		gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(findtag_window));
		gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
		gtk_window_present(GTK_WINDOW(dialog));

		gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);
		gtk_widget_show(GTK_WIDGET(dialog));

		// char *igx_filename;
		g_igx_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		std::ofstream myfile;
		myfile.open("igconfig.txt", std::fstream::out); // overwrite instead of app(end)
		myfile << g_igx_filename;
		myfile.close();

		std::string string1(g_igx_filename);
		// check for extension. if it's *.igx, then load the tag names
		if (string1.substr(string1.find_last_of(".") + 1) == "igx" && done_loading_tag_names == true)
		{
			// set the database filename label
			std::string string2 = string1;
			const size_t last_slash_idx = string2.find_last_of("\\/");
			if (std::string::npos != last_slash_idx)
			{
				string2.erase(0, last_slash_idx + 1);
			}
			std::stringstream stringstream1;
			stringstream1 << "Project file: " << string2;
			string2 = stringstream1.str();
			gtk_label_set_text(GTK_LABEL(filename_label), (const gchar *)string2.c_str());

			findtag_rowCount = IgGetTagsRowCount(g_igx_filename);
			findtag_pageCount = (findtag_rowCount / 20) + 0;
			if (findtag_rowCount % 20 == 0)
				findtag_pageCount--;

			std::stringstream stringstream2;
			if (findtag_rowCount > 0)
			{
				if(findtag_rowCount < 20)
					stringstream2 << "Page: 1/1";
				else
					stringstream2 << "Page: 1/" << (findtag_pageCount + 1);
				//stringstream2 << "Total: " << findtag_rowCount << " tags";
			}
			else
				stringstream2 << "Page: 1/1";

			std::string string1 = stringstream2.str();
			gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());

			is_igx_open = IgGetTagsOpenFilename(g_igx_filename);
			if (is_igx_open)
				get_tags_by_index(0);
		}
	}
	gtk_widget_destroy(dialog);

	gtk_window_set_keep_above(GTK_WINDOW(findtag_window), TRUE);
*/
	return;
}
void findtag_next_clicked_event(GtkButton *button, gpointer data)
{
	GtkWidget *search_entry = (GtkWidget *)data;
	const gchar *keyword = gtk_entry_get_text(GTK_ENTRY(search_entry));

	if (use_search_result == false)
	{
		if (done_loading_tag_names == true && is_igx_open == true) // if igx_filename (file path) is not empty
		{
			if(findtag_index == -1)
			{
				// do nothing
			}
			else if (findtag_index < -1 && is_pageindex_zero == true)
			{
				// pageindex cannot go lower than -1 (-1 is last index)
				gtk_list_store_clear(tag_store1);
				gtk_list_store_clear(tag_store2);

				findtag_index = 0;
				get_tags_by_index(findtag_index);
				is_pageindex_zero = false;
				is_pageindex_last = false;
			}
			else if (findtag_index == (findtag_pageCount - 1) && is_pageindex_zero == false)
			{
				// we're on last page
				gtk_list_store_clear(tag_store1);
				gtk_list_store_clear(tag_store2);

				findtag_index = -1;
				get_tags_by_index(-1);
				is_pageindex_zero = false;
				is_pageindex_last = true;
			}
			else if (/*findtag_index == (findtag_pageCount -1) &&*/ is_pageindex_zero == true)
			{
				// we're on first page so move to next
				gtk_list_store_clear(tag_store1);
				gtk_list_store_clear(tag_store2);

				findtag_index = 1;
				get_tags_by_index(findtag_index);
				is_pageindex_zero = false;
				is_pageindex_last = false;
			}
			else if (findtag_index < findtag_pageCount && is_pageindex_zero == false)
			{
				// not last, not first, move next
				gtk_list_store_clear(tag_store1);
				gtk_list_store_clear(tag_store2);

				findtag_index++;
				get_tags_by_index(findtag_index);
				is_pageindex_zero = false;
				is_pageindex_last = false;
			}

			if(findtag_rowCount > 19)
			{
				if(is_pageindex_last) // set the label to show remaining tags
				{
					// set the label
					std::stringstream stringstream1;
					stringstream1 << "Page: " << (findtag_pageCount + 1) << "/" << (findtag_pageCount + 1);
					std::string string1 = stringstream1.str();
					gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
				}
				else
				{
					// set the label
					std::stringstream stringstream1;
					stringstream1 << "Page: " << (findtag_index + 1) << "/" << (findtag_pageCount + 1);
					std::string string1 = stringstream1.str();
					gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
				}
			}
			else
			{
				// set the label
				std::stringstream stringstream1;
				stringstream1 << "Page: 1/1";
				std::string string1 = stringstream1.str();
				gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
			}
		}
	}
	else if (use_search_result == true)
	{
		if (done_loading_tag_names2 == true && is_igx_open == true) // if igx_filename (file path) is not empty
		{
			if(findtag_index2 == -1)
			{
				// do nothing
			}
			else if (findtag_index2 < -1 && is_pageindex_zero2 == true)
			{
				// pageindex cannot go lower than -1 (-1 is last index)
				gtk_list_store_clear(tag_store1);
				gtk_list_store_clear(tag_store2);

				findtag_index2 = 0;
				get_tags_by_keyword_index((char *)keyword, findtag_index2);
				is_pageindex_zero2 = false;
				is_pageindex_last2 = false;
			}
			else if (findtag_index2 == (findtag_pageCount2 - 1) && is_pageindex_zero2 == false)
			{
				// we're on last page
				gtk_list_store_clear(tag_store1);
				gtk_list_store_clear(tag_store2);

				findtag_index2 = -1;
				get_tags_by_keyword_index((char *)keyword, -1);
				is_pageindex_zero2 = false;
				is_pageindex_last2 = true;
			}
			else if (/*findtag_index2 == (findtag_pageCount2 -1) &&*/ is_pageindex_zero2 == true)
			{
				// we're on first page so move to next
				gtk_list_store_clear(tag_store1);
				gtk_list_store_clear(tag_store2);

				findtag_index2 = 1;
				get_tags_by_keyword_index((char *)keyword, findtag_index2);
				is_pageindex_zero2 = false;
				is_pageindex_last2 = false;
			}
			else if (findtag_index2 < findtag_pageCount2 && is_pageindex_zero2 == false)
			{
				// not last, not first, move next
				gtk_list_store_clear(tag_store1);
				gtk_list_store_clear(tag_store2);

				findtag_index2++;
				get_tags_by_keyword_index((char *)keyword, findtag_index2);
				is_pageindex_zero2 = false;
				is_pageindex_last2 = false;
			}

			if(findtag_rowCount2 > 19)
			{
				if(is_pageindex_last2) // set the label to show remaining tags
				{
					// set the label
					std::stringstream stringstream1;
					stringstream1 << "Page: " << (findtag_pageCount2 + 1) << "/" << (findtag_pageCount2 + 1);
					std::string string1 = stringstream1.str();
					gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
				}
				else
				{
					// set the label
					std::stringstream stringstream1;
					stringstream1 << "Page: " << (findtag_index2 + 1) << "/" << (findtag_pageCount2 + 1);
					std::string string1 = stringstream1.str();
					gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
				}
			}
			else
			{
				// set the label
				std::stringstream stringstream1;
				stringstream1 << "Page: 1/1";
				std::string string1 = stringstream1.str();
				gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
			}
		}
	}
	return;
}
void findtag_prev_clicked_event(GtkButton *button, gpointer data)
{
	GtkWidget *search_entry = (GtkWidget *)data;
	const gchar *keyword = gtk_entry_get_text(GTK_ENTRY(search_entry));

	if (use_search_result == false)
	{
		if (done_loading_tag_names == true && is_igx_open == true) // if igx_filename (file path) is not empty
		{
			if (is_pageindex_zero == false)
			{
				if (findtag_index == -1) // we're on last page, go to previous page
				{
					gtk_list_store_clear(tag_store1);
					gtk_list_store_clear(tag_store2);
					
					findtag_index = findtag_pageCount - 1;
					get_tags_by_index(findtag_index);
				}
				else
				{
					gtk_list_store_clear(tag_store1);
					gtk_list_store_clear(tag_store2);

					findtag_index--;
					get_tags_by_index(findtag_index);
				}

				if(findtag_rowCount > 19)
				{
					// set the label
					std::stringstream stringstream1;
					stringstream1 << "Page: " << (findtag_index + 1) << "/" << (findtag_pageCount + 1);
					std::string string1 = stringstream1.str();
					gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
				}
				else
				{
					// set the label
					std::stringstream stringstream1;
					stringstream1 << "Page: 1/1";
					std::string string1 = stringstream1.str();
					gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
				}
			}
		}
	}
	else if (use_search_result == true)
	{
		if (done_loading_tag_names2 == true && is_igx_open == true) // if igx_filename (file path) is not empty
		{
			if (is_pageindex_zero2 == false)
			{
				if (findtag_index2 == -1) // we're on last page, go to previous page
				{
					gtk_list_store_clear(tag_store1);
					gtk_list_store_clear(tag_store2);
					
					findtag_index2 = findtag_pageCount2 - 1;
					get_tags_by_keyword_index((char *)keyword, findtag_index2);
				}
				else
				{
					gtk_list_store_clear(tag_store1);
					gtk_list_store_clear(tag_store2);

					findtag_index2--;
					get_tags_by_keyword_index((char *)keyword, findtag_index2);
				}

				if(findtag_rowCount2 > 19)
				{
					// set the label
					std::stringstream stringstream1;
					stringstream1 << "Page: " << (findtag_index2 + 1) << "/" << (findtag_pageCount2 + 1);
					std::string string1 = stringstream1.str();
					gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
				}
				else
				{
					// set the label
					std::stringstream stringstream1;
					stringstream1 << "Page: 1/1";
					std::string string1 = stringstream1.str();
					gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
				}
			}
		}
	}
	return;
}
void findtag_first_clicked_event(GtkButton *button, gpointer data)
{
	GtkWidget *search_entry = (GtkWidget *)data;
	const gchar *keyword = gtk_entry_get_text(GTK_ENTRY(search_entry));

	if (use_search_result == false)
	{
		if (done_loading_tag_names == true && is_igx_open == true)
		{
			gtk_list_store_clear(tag_store1);
			gtk_list_store_clear(tag_store2);

			findtag_index = 0;
			is_pageindex_zero = true;
			is_pageindex_last = false;
			get_tags_by_index(0);
		}

		// set the label
		if(findtag_rowCount > 19)
		{
			std::stringstream stringstream1;
			stringstream1 << "Page: 1/" << (findtag_pageCount + 1);
			std::string string1 = stringstream1.str();
			gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
		}
		else
		{
			std::stringstream stringstream1;
			stringstream1 << "Page: 1/1";
			std::string string1 = stringstream1.str();
			gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
		}
	}
	else if (use_search_result == true)
	{
		if (done_loading_tag_names2 == true && is_igx_open == true)
		{
			gtk_list_store_clear(tag_store1);
			gtk_list_store_clear(tag_store2);

			findtag_index2 = 0;
			is_pageindex_zero2 = true;
			is_pageindex_last2 = false;
			get_tags_by_keyword_index((char *)keyword, findtag_index2);
		}

		if(findtag_rowCount2 > 19)
		{
			// set the label
			std::stringstream stringstream1;
			stringstream1 << "Page: 1/" << (findtag_pageCount2 + 1);
			std::string string1 = stringstream1.str();
			gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
		}
		else
		{
			// set the label
			std::stringstream stringstream1;
			stringstream1 << "Page: 1/1";
			std::string string1 = stringstream1.str();
			gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
		}
	}
	return;
}
void findtag_last_clicked_event(GtkButton *button, gpointer data)
{
	GtkWidget *search_entry = (GtkWidget *)data;
	const gchar *keyword = gtk_entry_get_text(GTK_ENTRY(search_entry));

	if (use_search_result == false)
	{
		if (findtag_pageCount != 0 && done_loading_tag_names == true && is_igx_open == true)
		{
			gtk_list_store_clear(tag_store1);
			gtk_list_store_clear(tag_store2);

			findtag_index = findtag_pageCount;
			is_pageindex_zero = false;
			is_pageindex_last = true;
			get_tags_by_index(-1);

			// set the label
			std::stringstream stringstream1;
			stringstream1 << "Page: " << (findtag_pageCount + 1) << "/" << (findtag_pageCount + 1);
			std::string string1 = stringstream1.str();
			gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
		}
	}
	else if (use_search_result == true)
	{
		if(findtag_pageCount2 < 0) // actually it happens when findtag_pageCount == -1
		{
			// do nothing. it's a bug when we prepend * to a keyword
			// example: keyword=test
			// then keyword=test*
			// then last page not ok
		}
		else if (findtag_pageCount2 != 0 && done_loading_tag_names2 == true && is_igx_open == true)
		{
			gtk_list_store_clear(tag_store1);
			gtk_list_store_clear(tag_store2);

			findtag_index2 = findtag_pageCount2;
			is_pageindex_zero2 = false;
			is_pageindex_last2 = true;
			get_tags_by_keyword_index((char *)keyword, -1);

			// set the label
			std::stringstream stringstream1;
			stringstream1 << "Page: " << (findtag_pageCount2 + 1) << "/" << (findtag_pageCount2 + 1);
			std::string string1 = stringstream1.str();
			gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string1.c_str());
		}
	}
	return;
}
void findtag_ok_clicked_event(GtkButton *button, GtkWidget *tf1)
{
	// Selection
	gchar *value1;
	GtkTreeIter iter1;
	GtkTreeModel *taglist_tree_model1 = gtk_tree_view_get_model(GTK_TREE_VIEW(taglist_tree1));
	GtkTreeSelection *selection1 = gtk_tree_view_get_selection(GTK_TREE_VIEW(taglist_tree1));

	gchar *value2;
	GtkTreeIter iter2;
	GtkTreeModel *taglist_tree_model2 = gtk_tree_view_get_model(GTK_TREE_VIEW(taglist_tree2));
	GtkTreeSelection *selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(taglist_tree2));

	if (gtk_tree_selection_get_selected(selection1, &taglist_tree_model1, &iter1))
	{
		// store the string
		gtk_tree_model_get(taglist_tree_model1, &iter1, 0, &value1, -1);
		
		//HSH - Replace highlighted text if exist
		gint selection_start;
		gint selection_end;
		if(gtk_editable_get_selection_bounds(GTK_EDITABLE(tf1), &selection_start, &selection_end))
		{
			gtk_editable_delete_text(GTK_EDITABLE(tf1), selection_start, selection_end);
		}

		//gtk_entry_set_text(GTK_ENTRY(tf1), value1);
		gint position = gtk_editable_get_position(GTK_EDITABLE(tf1));
		gtk_editable_insert_text(GTK_EDITABLE(tf1), value1, strlen(value1), &position);

		g_signal_emit_by_name(tf1, "focus-out-event");

		// destroy the window
		gtk_widget_destroy(GTK_WIDGET(findtag_window));
	}
	else if (gtk_tree_selection_get_selected(selection2, &taglist_tree_model2, &iter2))
	{
		// store the string
		gtk_tree_model_get(taglist_tree_model2, &iter2, 0, &value2, -1);
		
		//HSH - Replace highlighted text if exist
		gint selection_start;
		gint selection_end;
		if(gtk_editable_get_selection_bounds(GTK_EDITABLE(tf1), &selection_start, &selection_end))
		{
			gtk_editable_delete_text(GTK_EDITABLE(tf1), selection_start, selection_end);
		}

		//gtk_entry_set_text(GTK_ENTRY(tf1), value2);
		gint position = gtk_editable_get_position(GTK_EDITABLE(tf1));
		gtk_editable_insert_text(GTK_EDITABLE(tf1), value2, strlen(value2), &position);

		g_signal_emit_by_name(tf1, "focus-out-event");

		// destroy the window
		gtk_widget_destroy(GTK_WIDGET(findtag_window));
	}

	//gtk_widget_destroy(GTK_WIDGET(findtag_window));
	return;
}
void findtag_color_ok_clicked_event(GtkButton *button, gpointer user_data)
{
	gchar *_value;
	GtkTreeIter iter;
	GtkTreeModel *taglist_tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(taglist_tree1));
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(taglist_tree1));

	gchar *_value9;
	GtkTreeIter iter9;
	GtkTreeModel *taglist_tree_model9 = gtk_tree_view_get_model(GTK_TREE_VIEW(taglist_tree2));
	GtkTreeSelection *selection9 = gtk_tree_view_get_selection(GTK_TREE_VIEW(taglist_tree2));

	if (gtk_tree_selection_get_selected(selection, &taglist_tree_model, &iter))
	{
		gtk_tree_model_get(taglist_tree_model, &iter, 0, &_value, -1);

		GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
		GtkTreeIter iter2;
		GtkTreeSelection *selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
		GtkTreePath *path2;

		if (gtk_tree_selection_get_selected(selection2, &model, &iter2))
		{
			//HSH - append the tag column instead
			gchar * _current_value; 
			gtk_tree_model_get(model, &iter2, 0, &_current_value, -1);
			_value = g_strconcat(_current_value, _value, NULL);

			gtk_list_store_set(GTK_LIST_STORE(model), &iter2, 0, _value, -1);
			g_signal_emit_by_name(renderer1, "edited");
			//HSH - do not move to next field
			//path2 = gtk_tree_model_get_path(model, &iter2);
			//gtk_tree_selection_select_path(selection2, path2);
			//gtk_tree_view_set_cursor(GTK_TREE_VIEW(color_list), path2, gtk_tree_view_get_column(GTK_TREE_VIEW(color_list), 1), TRUE);
		}

		gtk_tree_path_free(path2);
		gtk_widget_destroy(GTK_WIDGET(findtag_window));
	}
	else if (gtk_tree_selection_get_selected(selection9, &taglist_tree_model9, &iter9))
	{
		gtk_tree_model_get(taglist_tree_model9, &iter9, 0, &_value9, -1);

		GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
		GtkTreeIter iter2;
		GtkTreeSelection *selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
		GtkTreePath *path2;

		if (gtk_tree_selection_get_selected(selection2, &model, &iter2))
		{			
			//HSH - append the tag column instead
			gchar * _current_value; 
			gtk_tree_model_get(model, &iter2, 0, &_current_value, -1);
			_value9 = g_strconcat(_current_value, _value9, NULL);

			gtk_list_store_set(GTK_LIST_STORE(model), &iter2, 0, _value9, -1);
			g_signal_emit_by_name(renderer1, "edited");
			//HSH - do not move to next field
			//path2 = gtk_tree_model_get_path(model, &iter2);
			//gtk_tree_selection_select_path(selection2, path2);
			//gtk_tree_view_set_cursor(GTK_TREE_VIEW(color_list), path2, gtk_tree_view_get_column(GTK_TREE_VIEW(color_list), 1), TRUE);
		}

		gtk_tree_path_free(path2);
		gtk_widget_destroy(GTK_WIDGET(findtag_window));
	}

	return;
}
gboolean findtag_taglist_tree1_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	GtkWidget *tf1;
	if (user_data == NULL) // then 'clicked' signal from Color tab
	{
		tf1 = NULL;

		gchar *_value;
		GtkTreeIter iter;
		GtkTreeModel *taglist_tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(taglist_tree1));
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(taglist_tree1));

		if (gtk_tree_selection_get_selected(selection, &taglist_tree_model, &iter))
		{
			gtk_tree_model_get(taglist_tree_model, &iter, 0, &_value, -1);

			GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
			GtkTreeIter iter2;
			GtkTreeSelection *selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
			GtkTreePath *path2;

			if (gtk_tree_selection_get_selected(selection2, &model, &iter2))
			{
				//HSH - append the tag column instead
				gchar * _current_value; 
				gtk_tree_model_get(model, &iter2, 0, &_current_value, -1);
				_value = g_strconcat(_current_value, _value, NULL);

				gtk_list_store_set(GTK_LIST_STORE(model), &iter2, 0, _value, -1);
				g_signal_emit_by_name(renderer1, "edited");
				//HSH - do not move to next field
				//path2 = gtk_tree_model_get_path(model, &iter2);
				//gtk_tree_selection_select_path(selection2, path2);
				//gtk_tree_view_set_cursor(GTK_TREE_VIEW(color_list), path2, gtk_tree_view_get_column(GTK_TREE_VIEW(color_list), 1), TRUE);
			}

			gtk_tree_path_free(path2);
			gtk_widget_destroy(GTK_WIDGET(findtag_window));
		}
	}
	else
	{
		tf1 = (GtkWidget *)user_data;

		if (!strcmp(gdk_keyval_name(event->keyval), "Return"))
		{
			// Selection
			gchar *value;
			GtkTreeIter iter;
			GtkTreeModel *taglist_tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(taglist_tree1));
			GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(taglist_tree1));

			if (gtk_tree_selection_get_selected(selection, &taglist_tree_model, &iter))
			{
				// store the string
				gtk_tree_model_get(taglist_tree_model, &iter, 0, &value, -1);
				
				//HSH - Replace highlighted text if exist
				gint selection_start;
				gint selection_end;
				if(gtk_editable_get_selection_bounds(GTK_EDITABLE(tf1), &selection_start, &selection_end))
				{
					gtk_editable_delete_text(GTK_EDITABLE(tf1), selection_start, selection_end);
				}

				//gtk_entry_set_text(GTK_ENTRY(tf1), value);
				gint position = gtk_editable_get_position(GTK_EDITABLE(tf1));
				gtk_editable_insert_text(GTK_EDITABLE(tf1), value, strlen(value), &position);

				g_signal_emit_by_name(tf1, "focus-out-event");

				// destroy the window
				gtk_widget_destroy(GTK_WIDGET(findtag_window));
			}
		}
	}

	return FALSE;
}
gboolean findtag_taglist_tree2_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	GtkWidget *tf1;
	if (user_data == NULL) // then 'clicked' signal from Color tab
	{
		tf1 = NULL;

		gchar *_value;
		GtkTreeIter iter;
		GtkTreeModel *taglist_tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(taglist_tree2));
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(taglist_tree2));

		if (gtk_tree_selection_get_selected(selection, &taglist_tree_model, &iter))
		{
			gtk_tree_model_get(taglist_tree_model, &iter, 0, &_value, -1);

			GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
			GtkTreeIter iter2;
			GtkTreeSelection *selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
			GtkTreePath *path2;

			if (gtk_tree_selection_get_selected(selection2, &model, &iter2))
			{
				//HSH - append the tag column instead
				gchar * _current_value; 
				gtk_tree_model_get(model, &iter2, 0, &_current_value, -1);
				_value = g_strconcat(_current_value, _value, NULL);

				gtk_list_store_set(GTK_LIST_STORE(model), &iter2, 0, _value, -1);
				g_signal_emit_by_name(renderer1, "edited");
				//HSH - do not move to next field
				//path2 = gtk_tree_model_get_path(model, &iter2);
				//gtk_tree_selection_select_path(selection2, path2);
				//gtk_tree_view_set_cursor(GTK_TREE_VIEW(color_list), path2, gtk_tree_view_get_column(GTK_TREE_VIEW(color_list), 1), TRUE);
			}

			gtk_tree_path_free(path2);
			gtk_widget_destroy(GTK_WIDGET(findtag_window));
		}
	}
	else
	{
		tf1 = (GtkWidget *)user_data;

		if (!strcmp(gdk_keyval_name(event->keyval), "Return"))
		{
			// Selection
			gchar *value;
			GtkTreeIter iter;
			GtkTreeModel *taglist_tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(taglist_tree2));
			GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(taglist_tree2));

			if (gtk_tree_selection_get_selected(selection, &taglist_tree_model, &iter))
			{
				// store the string
				gtk_tree_model_get(taglist_tree_model, &iter, 0, &value, -1);
				
				//HSH - Replace highlighted text if exist
				gint selection_start;
				gint selection_end;
				if(gtk_editable_get_selection_bounds(GTK_EDITABLE(tf1), &selection_start, &selection_end))
				{
					gtk_editable_delete_text(GTK_EDITABLE(tf1), selection_start, selection_end);
				}

				//gtk_entry_set_text(GTK_ENTRY(tf1), value);
				gint position = gtk_editable_get_position(GTK_EDITABLE(tf1));
				gtk_editable_insert_text(GTK_EDITABLE(tf1), value, strlen(value), &position);

				g_signal_emit_by_name(tf1, "focus-out-event");

				// destroy the window
				gtk_widget_destroy(GTK_WIDGET(findtag_window));
			}
		}
	}

	return FALSE;
}
void findtag_taglist_tree_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *treeviewcolumn, gpointer user_data)
{
	GtkWidget *tf1;

	if (user_data == NULL) // then 'clicked' signal from Color tab
	{
		tf1 = NULL;

		gchar *_value;
		GtkTreeIter iter;
		GtkTreeModel *taglist_tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

		if (gtk_tree_selection_get_selected(selection, &taglist_tree_model, &iter))
		{
			gtk_tree_model_get(taglist_tree_model, &iter, 0, &_value, -1);

			GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
			GtkTreeIter iter2;
			GtkTreeSelection *selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
			GtkTreePath *path2;

			if (gtk_tree_selection_get_selected(selection2, &model, &iter2))
			{
				//HSH - append the tag column instead
				gchar * _current_value; 
				gtk_tree_model_get(model, &iter2, 0, &_current_value, -1);
				_value = g_strconcat(_current_value, _value, NULL);

				gtk_list_store_set(GTK_LIST_STORE(model), &iter2, 0, _value, -1);
				g_signal_emit_by_name(renderer1, "edited");
				//HSH - do not move to next field
				//path2 = gtk_tree_model_get_path(model, &iter2);
				//gtk_tree_selection_select_path(selection2, path2);
				//gtk_tree_view_set_cursor(GTK_TREE_VIEW(color_list), path2, gtk_tree_view_get_column(GTK_TREE_VIEW(color_list), 1), TRUE);
			}

			gtk_tree_path_free(path2);
			gtk_widget_destroy(GTK_WIDGET(findtag_window));
		}
	}
	else
	{
		tf1 = (GtkWidget *)user_data;

		GtkTreeModel *taglist_tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
		GtkTreeIter iter;

		if (gtk_tree_model_get_iter(taglist_tree_model, &iter, path))
		{
			// store the string
			gchar *value;
			gtk_tree_model_get(taglist_tree_model, &iter, 0, &value, -1);
			
			//HSH - Replace highlighted text if exist
			gint selection_start;
			gint selection_end;
			if(gtk_editable_get_selection_bounds(GTK_EDITABLE(tf1), &selection_start, &selection_end))
			{
				gtk_editable_delete_text(GTK_EDITABLE(tf1), selection_start, selection_end);
			}

			//gtk_entry_set_text(GTK_ENTRY(tf1), value);
			gint position = gtk_editable_get_position(GTK_EDITABLE(tf1));
			gtk_editable_insert_text(GTK_EDITABLE(tf1), value, strlen(value), &position);

			g_signal_emit_by_name(tf1, "focus-out-event");

			// destroy the window
			gtk_widget_destroy(GTK_WIDGET(findtag_window));
		}
	}

	return;
}
void findtag_cancel_clicked_event(GtkButton *button, gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(findtag_window));
	return;
}
void resetbutton_clicked_event()
{
	// void resetbutton_clicked_event(GtkButton *button, gpointer user_data)
	use_search_result = false; // dont use the search result
	if (is_igx_open) // load the tag names
		get_tags_by_index(0);

	//gtk_entry_set_text(GTK_ENTRY(search_entry), "");

	std::stringstream stringstream2;
	if (findtag_rowCount > 0)
	{
		//stringstream2 << "Total: " << findtag_rowCount << " tags";
		if(findtag_rowCount < 20)
			stringstream2 << "Page: 1/1";
		else
			stringstream2 << "Page: 1/" << (findtag_pageCount + 1);
	}
	else
		stringstream2 << "Page: 1/1";

	std::string string3 = stringstream2.str();
	gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string3.c_str());

	return;
}
void findtag_button_destroy_event(GtkWidget *widget)
{
	/*
	delete[] completepath;
	delete[] passed_to_dll;

	if (is_igx_open)
		IgGetTagsDestruct();
	is_igx_open = false;

	// reset the global variables
	findtag_index = 0;
	findtag_rowCount = 0;
	findtag_pageCount = 0;
	is_pageindex_zero = false;

	findtag_index2 = 0;
	findtag_rowCount2 = 0;
	findtag_pageCount2 = 0;
	is_pageindex_zero2 = false;

	gtk_widget_destroy(widget);
	*/
}
void findtag_button_press_callback(GtkButton *button, gpointer user_data)
{
	/*
	use_search_result = false; // must be reset else navigation problem
	IgGetTagsConstruct();

	completepath = new char[1000]; //completepath = (char *)malloc(1000);
	memset(completepath, 0, 1000);

	passed_to_dll = new char[2560]; // 1 tag name max 128 chars. so 20 x 128 = 2560 chars
	memset(passed_to_dll, 0, 2560);

	SPDesktop *desktop = SP_ACTIVE_DESKTOP;
	SPDocument *doc = SP_ACTIVE_DESKTOP->getDocument();
	gchar const *_uri = doc->getURI();
	char *_uri_char = (char *)_uri;
	char *_completepath = "";
	int howmanyigx = 0;

	if (_uri) // if we got a file path (URI)
	{
		std::string uri(_uri_char);
		size_t found = uri.find_last_of("\\");
		std::string uri2 = uri.substr(0, found); // uri2 now holds the folder e.g. C:\Users\Ecava\Desktop
		_completepath = (char *)uri2.c_str();

		IgGetFilename(passed_to_dll, (char *)uri2.c_str(), howmanyigx);

		if (strcmp(passed_to_dll, "") != 0 && howmanyigx == 1) // if we got an *.igx file, that is the return is not empty and there is only one project file *.igx
		{
			//strcat(_completepath, "\\"); // append \ to C:\Users\Ecava\Desktop
			//strcat(_completepath, filenamefromdll); // lastly append filename

			// the above compiles, but crashes inkscape so we replace them with below statements
			std::string str1(_completepath);
			str1 = str1 + "\\";
			str1 = str1 + passed_to_dll;
			_completepath = (char *)str1.c_str();
		}
		else
			_completepath = "";

	}

	desktop = NULL; doc = NULL;

	// here, completepath either "" or "path to project file *.igx"
	size_t _completepathlength = strlen(_completepath);
	strncpy(completepath, _completepath, _completepathlength);

	GtkWidget *tf1;
	if (user_data == NULL) // then 'clicked' signal from Color tab
	{
		// ?
	}
	else // 'clicked' signal from other than Color tab
	{
		tf1 = (GtkWidget *)user_data;
	}

	findtag_window = gtk_window_new(GTK_WINDOW_TOPLEVEL); //GtkWidget *findtag_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(findtag_window), "Tag Browser");
	gtk_container_set_border_width(GTK_CONTAINER(findtag_window), 6); // ori 15
	gtk_window_set_resizable(GTK_WINDOW(findtag_window), TRUE); // originally TRUE
	gtk_window_set_default_size(GTK_WINDOW(findtag_window), 450, 338); // originally 450x450
	gtk_window_set_position(GTK_WINDOW(findtag_window), GTK_WIN_POS_CENTER);
	//gtk_window_set_transient_for(GTK_WINDOW(findtag_window), GTK_WINDOW(dlg));	//HSH - need to fix this currently NULL!!!
	sp_transientize(findtag_window);	//HSH - Solved the transient problem by calling this to make this dialog transient for previous open doc/dialog (Fixes above problem)
	gtk_window_set_modal(GTK_WINDOW(findtag_window), TRUE);
	gtk_window_present(GTK_WINDOW(findtag_window));

	// vertical align in vbox in window
	GtkWidget *vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(findtag_window), vbox);
	gtk_widget_show(vbox);

	// igx_filename label and button
	filename_label = gtk_label_new("Project file: ");
	gtk_misc_set_alignment(GTK_MISC(filename_label), 0, 0.5); // left alignment, middle
	gtk_widget_set_size_request(filename_label, 385, 25); // 450 wide - 60(button) = 390
	// "Change" button
	GtkWidget *find_button = gtk_button_new_with_label("Change");
	gtk_misc_set_alignment(GTK_MISC(find_button), 1, 0.5); // right alignment
	// create a container
	GtkWidget *hbox0 = gtk_hbox_new(FALSE, 3);
	gtk_widget_set_size_request(find_button, 60, 25);
	// put widgets into the container above
	gtk_box_pack_start(GTK_BOX(hbox0), filename_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox0), find_button, FALSE, FALSE, 0); // TRUE, TRUE
	gtk_container_add(GTK_CONTAINER(vbox), hbox0);
	// show the widgets
	gtk_widget_show(hbox0);
	gtk_widget_show(filename_label);
	gtk_widget_show(find_button);

	// igx_filename label and button
	GtkWidget *tagname_label = gtk_label_new("Tag filter: ");
	gtk_misc_set_alignment(GTK_MISC(tagname_label), 0, 0.5); // left alignment, middle
	// Search entry
	search_entry = gtk_entry_new(); // declared global
	gtk_misc_set_alignment(GTK_MISC(search_entry), 0, 0.5); // left
	gtk_entry_set_max_length(GTK_ENTRY(search_entry), 30);
	// "Filter" button
	//GtkWidget *findtag_search_button = gtk_button_new_with_label("Filter");
	//gtk_misc_set_alignment(GTK_MISC(findtag_search_button), 0, 0.5); // left alignment
	// create a container
	GtkWidget *hbox1 = gtk_hbox_new(FALSE, 3);
	gtk_widget_set_size_request(tagname_label, 62, 25); // 62
	gtk_widget_set_size_request(search_entry, 384, 25); // 320 with "Filter" button. 384 w/o Filter
	//gtk_widget_set_size_request(findtag_search_button, 60, 25);
	// put widgets into the container above
	gtk_box_pack_start(GTK_BOX(hbox1), tagname_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), search_entry, FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX(hbox1), findtag_search_button, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbox1);
	// show the widgets
	gtk_widget_show(hbox1); gtk_widget_show(tagname_label);
	gtk_widget_show(search_entry);//gtk_widget_show(findtag_search_button);

	//// Show reset button and system tags; checkbox and label
	//GtkWidget *reset_button = gtk_button_new_with_mnemonic("Reset");
	//GtkWidget *showsystags_checkbutton = gtk_check_button_new_with_mnemonic("Show system tags");
	//GtkWidget *hbox2 = gtk_hbox_new(FALSE, 3);
	//gtk_misc_set_alignment(GTK_MISC(reset_button), 0, 0.5);
	//gtk_misc_set_alignment(GTK_MISC(showsystags_checkbutton), 1, 0.5);
	//gtk_widget_set_size_request(reset_button, 60, 25);
	//gtk_widget_set_size_request(showsystags_checkbutton, 160, 25);
	//gtk_box_pack_start(GTK_BOX(hbox2), reset_button, FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX(hbox2), showsystags_checkbutton, FALSE, FALSE, 0);
	//GtkWidget *hbox_inside_hbox2 = gtk_hbox_new(FALSE, 0);
	//gtk_widget_set_size_request(hbox_inside_hbox2, 300, 25);
	//gtk_box_pack_start(GTK_BOX(hbox2), hbox_inside_hbox2, TRUE, TRUE, 0);
	//gtk_widget_show(reset_button);
	//gtk_widget_show(showsystags_checkbutton);
	//gtk_widget_show(hbox2);
	//gtk_container_add(GTK_CONTAINER(vbox), hbox2);

	// Scrolled window in vbox
	// GtkListStore *tag_store = gtk_list_store_new(1, G_TYPE_STRING); // moved global
	tag_store1 = gtk_list_store_new(1, G_TYPE_STRING);
	tag_store2 = gtk_list_store_new(1, G_TYPE_STRING);
	taglist_tree1 = gtk_tree_view_new_with_model(GTK_TREE_MODEL(tag_store1));
	taglist_tree2 = gtk_tree_view_new_with_model(GTK_TREE_MODEL(tag_store2));
	gtk_misc_set_padding(GTK_MISC(taglist_tree1), 0, 0);
	gtk_misc_set_padding(GTK_MISC(taglist_tree2), 0, 0);
	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(taglist_tree1)), GTK_SELECTION_SINGLE); // SINGLE or BROWSE
	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(taglist_tree2)), GTK_SELECTION_SINGLE); // SINGLE or BROWSE
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(taglist_tree1), TRUE);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(taglist_tree2), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(taglist_tree1), FALSE); // TRUE to show "Tag Name"
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(taglist_tree2), FALSE);
	GtkCellRenderer *renderer1 = gtk_cell_renderer_text_new();
	GtkCellRenderer *renderer2 = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *column1 = gtk_tree_view_column_new_with_attributes("Tag Name", renderer1, "text", 0, NULL);
	GtkTreeViewColumn *column2 = gtk_tree_view_column_new_with_attributes("Tag Name", renderer2, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(column1, TRUE);
	gtk_tree_view_column_set_resizable(column2, TRUE);
	gtk_tree_view_column_set_alignment(column1, 0.5);
	gtk_tree_view_column_set_alignment(column2, 0.5);
	//gtk_tree_view_column_set_min_width(column, 280);
	gtk_tree_view_append_column(GTK_TREE_VIEW(taglist_tree1), column1);
	gtk_tree_view_append_column(GTK_TREE_VIEW(taglist_tree2), column2);
	GtkWidget *scroll1 = gtk_scrolled_window_new(NULL, NULL);
	GtkWidget *scroll2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_misc_set_padding(GTK_MISC(scroll1), 0, 0);
	gtk_misc_set_padding(GTK_MISC(scroll2), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(scroll1), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(scroll2), 0, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll1), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER); // both automatic
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll2), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll1), GTK_SHADOW_NONE); // GTK_SHADOW_ETCHED_IN
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll2), GTK_SHADOW_NONE);
	gtk_container_add(GTK_CONTAINER(scroll1), taglist_tree1);
	gtk_container_add(GTK_CONTAINER(scroll2), taglist_tree2);
	GtkWidget *hbox3 = gtk_hbox_new(TRUE, 0); // spacing ori 3
	gtk_container_set_border_width(GTK_CONTAINER(hbox3), 3);
	gtk_widget_set_size_request(hbox3, 450, 248); // <--------- size of scrolled window. originally 450x350
	// 238 OK for 10 tags per 2 columns
	// increased height to 248
	gtk_container_add(GTK_CONTAINER(hbox3), scroll1);
	gtk_container_add(GTK_CONTAINER(hbox3), scroll2);
	gtk_container_add(GTK_CONTAINER(vbox), hbox3);
	gtk_widget_show(taglist_tree1); gtk_widget_show(taglist_tree2);
	gtk_widget_show(scroll1); gtk_widget_show(scroll2); gtk_widget_show(hbox3);

	// navigation buttons
	GtkWidget *first = gtk_button_new_with_label("|<");
	GtkWidget *prev = gtk_button_new_with_label("<");
	findtag_total = gtk_label_new("Page: 1/1");
	gtk_label_set_max_width_chars(GTK_LABEL(findtag_total), 20);
	gtk_label_set_width_chars(GTK_LABEL(findtag_total), 20);
	GtkWidget *next = gtk_button_new_with_label(">");
	GtkWidget *last = gtk_button_new_with_label(">|");
	GtkWidget *hbox4 = gtk_hbox_new(FALSE, 3); // TRUE
	gtk_container_add(GTK_CONTAINER(hbox4), first);
	gtk_container_add(GTK_CONTAINER(hbox4), prev);
	gtk_container_add(GTK_CONTAINER(hbox4), findtag_total);
	gtk_container_add(GTK_CONTAINER(hbox4), next);
	gtk_container_add(GTK_CONTAINER(hbox4), last);
	//gtk_box_pack_start(GTK_BOX(hbox4), first, FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX(hbox4), prev, FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX(hbox4), findtag_total, FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX(hbox4), next, FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX(hbox4), last, FALSE, FALSE, 0);
	gtk_widget_set_size_request(hbox4, 450, 25);
	gtk_widget_set_size_request(first, 45, 25); gtk_widget_set_size_request(prev, 45, 25);
	gtk_widget_set_size_request(findtag_total, 110, 25); // 150
	gtk_widget_set_size_request(next, 45, 25); gtk_widget_set_size_request(last, 45, 25);
	gtk_container_add(GTK_CONTAINER(vbox), hbox4);
	gtk_widget_show(first); gtk_widget_show(prev); gtk_widget_show(findtag_total);
	gtk_widget_show(next); gtk_widget_show(last); gtk_widget_show(hbox4);

	// OK and Cancel buttons
	GtkWidget *hbox5 = gtk_hbox_new(TRUE, 3);
	GtkWidget *ok = gtk_button_new_with_label("OK");
	gtk_widget_set_size_request(ok, 220, 25);
	gtk_misc_set_alignment(GTK_MISC(ok), 0.5, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox5), ok, FALSE, FALSE, 0);
	GtkWidget *cancel = gtk_button_new_with_label("Cancel");
	gtk_widget_set_size_request(cancel, 220, 25);
	gtk_misc_set_alignment(GTK_MISC(cancel), 0.5, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox5), cancel, FALSE, FALSE, 0);
	gtk_widget_set_size_request(hbox5, 450, 25);
	gtk_container_add(GTK_CONTAINER(vbox), hbox5);
	gtk_widget_show(ok); gtk_widget_show(cancel);//gtk_widget_show(halign5);
	gtk_widget_show(hbox5);

	gtk_widget_show(findtag_window);
	gtk_window_set_keep_above(GTK_WINDOW(findtag_window), TRUE);


	if (!strcmp(completepath, "")) // if we didnt find one *.igx file, 
	{
		std::ifstream ifstream1("igconfig.txt");
		if (ifstream1.good())
		{
			std::string string1;
			std::getline(ifstream1, string1);

			// check for extension. if it's *.igx, then save it
			if (string1.substr(string1.find_last_of(".") + 1) == "igx")
			{
				//HSH - check if igx file exist
				std::ifstream ifstream2(string1.c_str());
				if (ifstream2.good())
				{
					size_t length = strlen(string1.c_str());
					strncpy(completepath, string1.c_str(), string1.size()); //completepath = (gchar *)string1.c_str();
				}
				ifstream2.close();
			}
		}
		ifstream1.close();
	}

	std::string igx_filename(completepath);

	// load the tag names if there is a project file *.igx
	if (strcmp(igx_filename.c_str(), "") != 0)
	{
		// set the database filename label
		std::string string2(igx_filename.c_str());

		const size_t last_slash_idx = string2.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
			string2.erase(0, last_slash_idx + 1);
		}
		std::stringstream stringstream1;
		stringstream1 << "Project file:   " << string2;
		string2 = stringstream1.str();
		gtk_label_set_text(GTK_LABEL(filename_label), (const gchar *)string2.c_str());

		findtag_rowCount = IgGetTagsRowCount((char *)igx_filename.c_str());
		findtag_pageCount = (findtag_rowCount / 20) + 0;
		if (findtag_rowCount % 20 == 0)
			findtag_pageCount--;

		std::stringstream stringstream2;
		if (findtag_rowCount > 0)
		{
			stringstream2 << "Page: 1/" << (findtag_pageCount + 1);
		}
		else
			stringstream2 << "Page: 1/1";

		std::string string3 = stringstream2.str();
		gtk_label_set_text(GTK_LABEL(findtag_total), (const gchar *)string3.c_str());

		//is_igx_open = IgGetTagsOpenFilename((char *)igx_filename.c_str());
		//if (is_igx_open)
		//	get_tags_by_index(0);
	}
	

	// since we cannot set value to global char*, so we pass around char* by function callback using g_object_set_data
	//g_object_set_data(G_OBJECT(findtag_search_button), "search", strdup(igx_filename.c_str()));
	g_object_set_data(G_OBJECT(search_entry), "search_entry", strdup(igx_filename.c_str()));

	// functions callback
	g_signal_connect_swapped(G_OBJECT(findtag_window), "destroy", G_CALLBACK(findtag_button_destroy_event), findtag_window);
	g_signal_connect(G_OBJECT(search_entry), "key_press_event", G_CALLBACK(findtag_search_entry_key_press_event), NULL);
	g_signal_connect(G_OBJECT(search_entry), "changed", G_CALLBACK(findtag_search_entry_changed_event), NULL);
	//g_signal_connect(G_OBJECT(findtag_search_button), "clicked", G_CALLBACK(findtag_search_button_clicked_event), search_entry);
	g_signal_connect(find_button, "clicked", G_CALLBACK(findbutton_clicked_event), search_entry);
	//g_signal_connect(reset_button, "clicked", G_CALLBACK(resetbutton_clicked_event), NULL);
	if (user_data == NULL) // then 'clicked' signal from Color tab
	{
		g_signal_connect(ok, "clicked", G_CALLBACK(findtag_color_ok_clicked_event), NULL);
		// double Enter/Return on selected tag name will press OK
		g_signal_connect(G_OBJECT(taglist_tree1), "key_press_event", G_CALLBACK(findtag_taglist_tree1_key_press_event), NULL);
		g_signal_connect(G_OBJECT(taglist_tree2), "key_press_event", G_CALLBACK(findtag_taglist_tree2_key_press_event), NULL);
		// double-click
		g_signal_connect(G_OBJECT(taglist_tree1), "row-activated", G_CALLBACK(findtag_taglist_tree_row_activated), NULL);
		g_signal_connect(G_OBJECT(taglist_tree2), "row-activated", G_CALLBACK(findtag_taglist_tree_row_activated), NULL);
	}
	else // 'clicked' signal from other than Color tab
	{
		g_signal_connect(ok, "clicked", G_CALLBACK(findtag_ok_clicked_event), tf1);
		// double Enter/Return on selected tag name will press OK
		g_signal_connect(G_OBJECT(taglist_tree1), "key_press_event", G_CALLBACK(findtag_taglist_tree1_key_press_event), tf1);
		g_signal_connect(G_OBJECT(taglist_tree2), "key_press_event", G_CALLBACK(findtag_taglist_tree2_key_press_event), tf1);
		// double-click
		g_signal_connect(G_OBJECT(taglist_tree1), "row-activated", G_CALLBACK(findtag_taglist_tree_row_activated), tf1);
		g_signal_connect(G_OBJECT(taglist_tree2), "row-activated", G_CALLBACK(findtag_taglist_tree_row_activated), tf1);
	}
	g_signal_connect(cancel, "clicked", G_CALLBACK(findtag_cancel_clicked_event), NULL);
	g_signal_connect(G_OBJECT(next), "clicked", G_CALLBACK(findtag_next_clicked_event), search_entry);
	g_signal_connect(G_OBJECT(prev), "clicked", G_CALLBACK(findtag_prev_clicked_event), search_entry);
	g_signal_connect(G_OBJECT(first), "clicked", G_CALLBACK(findtag_first_clicked_event), search_entry);
	g_signal_connect(G_OBJECT(last), "clicked", G_CALLBACK(findtag_last_clicked_event), search_entry);

	// delete[] allocated char array done in findtag_button_destroy_event()
*/
	return;
}


GtkWidget *isac_menu_bar_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Bar" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl1, *lbl2, *lbl3, *tf1, *tf2, *tf3;

	GtkTooltips *tt = gtk_tooltips_new();

	vb = gtk_vbox_new(FALSE, 4);
	tb = gtk_table_new(4, 4, FALSE);  //(rows, cols, homogeneous) dennis
	tb1 = gtk_table_new(3, 2, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);

	// Create the Tag label
	lbl1 = gtk_label_new_with_mnemonic(_("_Tag*"));
	gtk_misc_set_alignment(GTK_MISC(lbl1), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl1), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl1, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	//Create the Tag entry field
	tf1 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf1, _("Tag name"), NULL);
	//gtk_entry_set_max_length (GTK_ENTRY (tf1), 64);
	// gtk_entry_set_text (GTK_ENTRY (tf1), j["tag"].val.strVal);
	gtk_entry_set_width_chars(GTK_ENTRY(tf1), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf1, 1, 4, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	g_signal_connect(G_OBJECT(tf1), "focus-out-event",
		G_CALLBACK(isac_onbarfocusout), spw);
	gtk_widget_set_size_request(tf1, 300, 23);

	// naim@ecava.com : find button
	GtkWidget *find_button = gtk_button_new_with_label("...");
	gtk_misc_set_alignment(GTK_MISC(find_button), 1, 0.5);
	gtk_table_attach(GTK_TABLE(tb), find_button, 4, 5, 0, 1,
		(GtkAttachOptions)(GTK_FILL), // (GTK_EXPAND | GTK_FILL)
		(GtkAttachOptions)(GTK_SHRINK),
		0, 0);
	g_signal_connect(G_OBJECT(find_button), "clicked", G_CALLBACK(findtag_button_press_callback), tf1);
	gtk_widget_set_size_request(find_button, 25, 23);

	gtk_object_set_data(GTK_OBJECT(spw), "isac_bar_tag", tf1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf1);

	lbl2 = gtk_label_new_with_mnemonic(_("Mi_n*"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl2), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 1, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf2 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf2, _("The minimum value"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf2), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf2), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf2, 1, 5, 1, 2,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_bar_min", tf2);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl2), tf2);

	g_signal_connect(G_OBJECT(tf2), "focus-out-event",
		G_CALLBACK(isac_onbarfocusout), spw);

	lbl3 = gtk_label_new_with_mnemonic(_("Ma_x*"));
	gtk_misc_set_alignment(GTK_MISC(lbl3), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl3), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl3, 0, 1, 2, 3,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf3 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf3, _("The maximum value"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf3), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf3), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf3, 1, 5, 2, 3,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_bar_max", tf3);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl3), tf3);

	g_signal_connect(G_OBJECT(tf3), "focus-out-event",
		G_CALLBACK(isac_onbarfocusout), spw);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	gtk_expander_set_spacing(GTK_EXPANDER(int_expander), 1);
	//gtk_object_set_data (GTK_OBJECT (spw), "interactivity", int_expander);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);


	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	//gtk_text_view_set_editable(GTK_TEXT_VIEW(tf), FALSE);
	gtk_text_buffer_set_text(desc_buffer, "Object's height/level animation according to max & min. You can rotate the graphic to get animation for horizontal or even diagonal direction.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	//gtk_table_attach_defaults (GTK_TABLE (int_expander), tf, 1, 3, 2, 3);
	gtk_widget_show(tf);


	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_bar_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_clone_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Clone" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	
	//GtkTreeSelection *cselection;

	clone_hbox = gtk_hbox_new(FALSE, 12); // hbox is in vbox
	clone_vbox = gtk_vbox_new(FALSE, 4);
	gtk_container_add(GTK_CONTAINER(clone_vbox), clone_hbox);

	clone_store = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_BOOLEAN);
	clone_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(clone_store));
	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(clone_list)), GTK_SELECTION_BROWSE); // SINGLE or BROWSE
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(clone_list), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(clone_list), TRUE);

	clone_renderer1 = gtk_cell_renderer_text_new();
	//g_object_set(G_OBJECT(clone_renderer1), "xalign", 1, NULL);  
	clone_column1 = gtk_tree_view_column_new_with_attributes("Variable", clone_renderer1, 
		"text", CLONE_VAR_TEXT, 
		"editable", CLONE_VAR_EDITABLE,	NULL);
	gtk_tree_view_column_set_resizable(clone_column1, TRUE);
	gtk_tree_view_column_set_alignment(clone_column1, 0.5);
	gtk_tree_view_column_set_min_width(clone_column1, 60);
	gtk_tree_view_append_column(GTK_TREE_VIEW(clone_list), clone_column1);

	clone_renderer2 = gtk_cell_renderer_text_new();
	g_object_set(G_OBJECT(clone_renderer2), "xalign", 0.5, NULL);  
	clone_column2 = gtk_tree_view_column_new_with_attributes("=", clone_renderer2, 
		"text", CLONE_EQUAL_TEXT, 
		"editable", CLONE_EQUAL_EDITABLE, NULL);
	//gtk_tree_view_column_set_resizable(clone_column2, TRUE);
	gtk_tree_view_column_set_alignment(clone_column2, 0.5);
	gtk_tree_view_column_set_min_width(clone_column2, 20);
	gtk_tree_view_append_column(GTK_TREE_VIEW(clone_list), clone_column2);

	clone_renderer3 = gtk_cell_renderer_text_new();
	clone_column3 = gtk_tree_view_column_new_with_attributes("Value", clone_renderer3, 
		"text", CLONE_VAL_TEXT, 
		"editable", CLONE_VAL_EDITABLE, NULL);
	gtk_tree_view_column_set_resizable(clone_column3, TRUE);
	gtk_tree_view_column_set_alignment(clone_column3, 0.5);
	gtk_tree_view_append_column(GTK_TREE_VIEW(clone_list), clone_column3);

	g_signal_connect(G_OBJECT(clone_renderer1), "edited", G_CALLBACK(clone_list_edited_column1), spw);
	g_signal_connect(G_OBJECT(clone_renderer2), "edited", G_CALLBACK(clone_list_edited_column2), spw);
	g_signal_connect(G_OBJECT(clone_renderer3), "edited", G_CALLBACK(clone_list_edited_column3), spw);
	g_signal_connect(G_OBJECT(clone_list), "focus-out-event", G_CALLBACK(isac_onclonefocusout), spw);
	g_signal_connect(G_OBJECT(clone_list), "focus", G_CALLBACK(isac_onclonefocusin), spw);
	
	//HSH - set signals and callback function for when a cell renderer has started editing
	g_signal_connect(G_OBJECT(clone_renderer1), "editing-started", G_CALLBACK(clone_list_editing_started), spw);
	g_signal_connect(G_OBJECT(clone_renderer2), "editing-started", G_CALLBACK(clone_list_editing_started), spw);
	g_signal_connect(G_OBJECT(clone_renderer3), "editing-started", G_CALLBACK(clone_list_editing_started), spw);
	g_signal_connect(G_OBJECT(clone_list), "key-press-event", G_CALLBACK(clone_list_on_key_press), spw);	//HSH - Signal and callback for keypresses within the color list

	clone_scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(clone_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(clone_scroll), GTK_SHADOW_ETCHED_IN);
	gtk_container_add(GTK_CONTAINER(clone_scroll), clone_list);
	gtk_box_pack_start(GTK_BOX(clone_hbox), clone_scroll, TRUE, TRUE, 0);

	clone_bttn = gtk_vbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(clone_bttn), GTK_BUTTONBOX_START);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(clone_bttn), 6);
	gtk_box_pack_start(GTK_BOX(clone_hbox), clone_bttn, FALSE, FALSE, 12);

	clone_newb = gtk_button_new_from_stock(GTK_STOCK_ADD);
	clone_deleteb = gtk_button_new_from_stock(GTK_STOCK_DELETE);
	clone_upb = gtk_button_new_with_mnemonic(_("Move _Up"));
	clone_downb = gtk_button_new_with_mnemonic(_("Move _Down"));

	g_signal_connect(G_OBJECT(clone_newb), "clicked", G_CALLBACK(clone_new_list), spw);
	g_signal_connect(G_OBJECT(clone_deleteb), "clicked", G_CALLBACK(clone_delete_item), spw);
	g_signal_connect(G_OBJECT(clone_upb), "clicked", G_CALLBACK(clone_list_move_up), clone_list);
	g_signal_connect(G_OBJECT(clone_downb), "clicked", G_CALLBACK(clone_list_move_down), clone_list);

	gtk_box_pack_start(GTK_BOX(clone_bttn), gtk_hseparator_new(), FALSE, FALSE, 6); // 1st: hseparator
	gtk_box_pack_start(GTK_BOX(clone_bttn), clone_newb, FALSE, FALSE, 6); // 2nd: Add
	gtk_box_pack_start(GTK_BOX(clone_bttn), clone_deleteb, FALSE, FALSE, 6); // 3rd: Delete
	gtk_box_pack_start(GTK_BOX(clone_bttn), gtk_hseparator_new(), FALSE, FALSE, 6); // 5th: hseparator
	gtk_box_pack_start(GTK_BOX(clone_bttn), clone_upb, FALSE, FALSE, 6); // 6th: Move Up
	gtk_box_pack_start(GTK_BOX(clone_bttn), clone_downb, FALSE, FALSE, 6); // 7th: Move Down
	gtk_box_pack_start(GTK_BOX(clone_bttn), gtk_hseparator_new(), FALSE, FALSE, 6); // 8th: hseparator

	g_signal_connect(G_OBJECT(clone_vbox), "destroy", G_CALLBACK(clone_deletelastrow), NULL);
	g_signal_connect(G_OBJECT(clone_list), "destroy", G_CALLBACK(clone_deletelastrow), NULL);

	/*gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 3);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);


	lbl2 = gtk_label_new(_("  Variable"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 2, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	lbl3 = gtk_label_new(_("  Value"));
	gtk_misc_set_alignment(GTK_MISC(lbl3), 0, 0.5);
	gtk_table_attach(GTK_TABLE(tb), lbl3, 3, 5, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	for (gint i = 1; i <= ISAC_CLONE_DEF_ENTRY; i++)
	{
		tfx = gtk_entry_new();
		gtk_tooltips_set_tip(tt, tfx, _("Map from"), NULL);
		gtk_entry_set_max_length(GTK_ENTRY(tfx), 64);
		gtk_entry_set_width_chars(GTK_ENTRY(tfx), ISAC_TF_SHORT_LEN);
		gtk_table_attach(GTK_TABLE(tb), tfx, 1, 2, 0 + i, 1 + i,
			(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
			(GtkAttachOptions)0, 0, 0);

		g_signal_connect(G_OBJECT(tfx), "focus-out-event",
			G_CALLBACK(isac_onclonefocusout), spw);

		clone_mapfrom_list = g_slist_prepend(clone_mapfrom_list, tfx);

		lblx = gtk_label_new(_("="));
		gtk_misc_set_alignment(GTK_MISC(lblx), 0, 0.5);
		gtk_table_attach(GTK_TABLE(tb), lblx, 2, 3, 0 + i, 1 + i,  //...,0,1,0,1 ... -> left, right, top, bottom
			(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
			(GtkAttachOptions)0, 0, 0);

		tfx = gtk_entry_new();
		gtk_tooltips_set_tip(tt, tfx, _("Map with"), NULL);
		//gtk_entry_set_max_length (GTK_ENTRY (tfx), 64);
		gtk_entry_set_width_chars(GTK_ENTRY(tfx), 50);
		gtk_table_attach(GTK_TABLE(tb), tfx, 3, 4, 0 + i, 1 + i,
			(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
			(GtkAttachOptions)0, 0, 0);

		g_signal_connect(G_OBJECT(tfx), "focus-out-event",
			G_CALLBACK(isac_onclonefocusout), spw);

		clone_mapto_list = g_slist_prepend(clone_mapto_list, tfx);

	}
	clone_mapfrom_list = g_slist_reverse(clone_mapfrom_list);
	clone_mapto_list = g_slist_reverse(clone_mapto_list);*/

	//Info section
	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	//gtk_object_set_data (GTK_OBJECT (spw), "interactivity", int_expander);
	gtk_box_pack_start(GTK_BOX(clone_vbox), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	//gtk_text_view_set_editable(GTK_TEXT_VIEW(tf), FALSE);
	gtk_text_buffer_set_text(desc_buffer, "A way to group several animations together by making tag names into variables, and duplicate them easily. For example, n=TAG1, s=TAG2, f=TAG3. In the grouped objects those variables are accessed as %n, %s and %f receiving the values of TAG1, TAG2 and TAG3.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	//gtk_table_attach_defaults (GTK_TABLE (tb1), tf, 3, 4, 2, 3);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_clone_dsb", dsb);
	
	clone_new_list(NULL, NULL);

	return (GtkWidget *)clone_vbox;
}
GtkWidget *isac_menu_color_init(GtkObject *spw)
{ // called in static void isac_notebook_new (SPWidget *spw)
	/*****************************
	ISAC Features:
	"Color" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkTreeSelection *cselection;

	// <naim@ecava.com> : below declared at global, purpose is to make color_deletelastrow() works
	//GtkWidget *hbox, *vbox, *bttn, *scroll, /**list,*/ *deleteb, *newb, *upb, *downb, *colorb; 
	//GtkListStore *store; // declared static global
	//GtkTreeViewColumn *column1, *column2, *column3;
	//GtkCellRenderer *renderer1, *renderer2, *renderer3;

	// below not used since v3.03
	//GtkTreeIter iter;
	//GtkTooltips *tt = gtk_tooltips_new();

	hbox = gtk_hbox_new(FALSE, 12); // hbox is in vbox
	vbox = gtk_vbox_new(FALSE, 4);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);

	store = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_BOOLEAN);
	color_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	// naim@ecava.com : a statement below
	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list)), GTK_SELECTION_BROWSE); // SINGLE or BROWSE
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(color_list), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(color_list), TRUE);

	renderer1 = gtk_cell_renderer_text_new();
	column1 = gtk_tree_view_column_new_with_attributes("Tag", renderer1, "text", 0, "editable", 1, NULL);
	gtk_tree_view_column_set_resizable(column1, TRUE);
	gtk_tree_view_column_set_alignment(column1, 0.5);
	gtk_tree_view_column_set_min_width(column1, 110);
	gtk_tree_view_append_column(GTK_TREE_VIEW(color_list), column1);

	renderer2 = gtk_cell_renderer_text_new();
	column2 = gtk_tree_view_column_new_with_attributes("Limit", renderer2, "text", 2, "editable", 3, NULL);
	gtk_tree_view_column_set_resizable(column2, TRUE);
	gtk_tree_view_column_set_alignment(column2, 0.5);
	gtk_tree_view_column_set_min_width(column2, 110);
	gtk_tree_view_append_column(GTK_TREE_VIEW(color_list), column2);

	renderer3 = gtk_cell_renderer_text_new();
	column3 = gtk_tree_view_column_new_with_attributes("Color Name/Code", renderer3, "text", 4, "editable", 5, NULL);
	gtk_tree_view_column_set_resizable(column3, TRUE);
	gtk_tree_view_column_set_alignment(column3, 0.5);
	gtk_tree_view_append_column(GTK_TREE_VIEW(color_list), column3);

	g_signal_connect(G_OBJECT(renderer1), "edited", G_CALLBACK(color_list_edited_column1), spw);
	g_signal_connect(G_OBJECT(renderer2), "edited", G_CALLBACK(color_list_edited_column2), spw);
	g_signal_connect(G_OBJECT(renderer3), "edited", G_CALLBACK(color_list_edited_column3), spw);
	g_signal_connect(G_OBJECT(color_list), "focus-out-event", G_CALLBACK(isac_oncolorfocusout), spw);
	g_signal_connect(G_OBJECT(color_list), "focus", G_CALLBACK(isac_oncolorfocusin), spw);
	
	//HSH - set signals and callback function for when a cell renderer has started editing
	g_signal_connect(G_OBJECT(renderer1), "editing-started", G_CALLBACK(color_list_editing_started), spw);
	g_signal_connect(G_OBJECT(renderer2), "editing-started", G_CALLBACK(color_list_editing_started), spw);
	g_signal_connect(G_OBJECT(renderer3), "editing-started", G_CALLBACK(color_list_editing_started), spw);
	g_signal_connect(G_OBJECT(color_list), "key-press-event", G_CALLBACK(color_list_on_key_press), spw);	//HSH - Signal and callback for keypresses within the color list

	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_ETCHED_IN);
	gtk_container_add(GTK_CONTAINER(scroll), color_list);
	gtk_box_pack_start(GTK_BOX(hbox), scroll, TRUE, TRUE, 0);

	bttn = gtk_vbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bttn), GTK_BUTTONBOX_START);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(bttn), 6);
	gtk_box_pack_start(GTK_BOX(hbox), bttn, FALSE, FALSE, 12);

	newb = gtk_button_new_from_stock(GTK_STOCK_ADD);
	//GtkWidget *color_find_tag = gtk_button_new_with_mnemonic(_("_Find")); // naim@ecava.com
	GtkWidget *color_find_tag = gtk_button_new_with_label("...");
	deleteb = gtk_button_new_from_stock(GTK_STOCK_DELETE);
	colorb = gtk_button_new_with_mnemonic(_("Color _Picker"));
	upb = gtk_button_new_with_mnemonic(_("Move _Up"));
	downb = gtk_button_new_with_mnemonic(_("Move _Down"));
	gtk_widget_set_sensitive(colorb, FALSE);
	gtk_widget_set_sensitive(color_find_tag, FALSE); // naim@ecava.com

	g_object_set_data(G_OBJECT(spw), "isac_color_picker_bttn", colorb);
	g_object_set_data(G_OBJECT(spw), "isac_color_find_tag", color_find_tag); // naim@ecava.com

	g_signal_connect(G_OBJECT(newb), "clicked", G_CALLBACK(color_new_list), spw);
	g_signal_connect(G_OBJECT(color_find_tag), "clicked", G_CALLBACK(findtag_button_press_callback), NULL); // naim@ecava.com
	g_signal_connect(G_OBJECT(deleteb), "clicked", G_CALLBACK(color_delete_item), spw);
	g_signal_connect(G_OBJECT(colorb), "clicked", G_CALLBACK(color_selection), spw);
	g_signal_connect(G_OBJECT(upb), "clicked", G_CALLBACK(color_list_move_up), color_list);
	g_signal_connect(G_OBJECT(downb), "clicked", G_CALLBACK(color_list_move_down), color_list);

	cselection = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
	g_signal_connect(cselection, "changed", G_CALLBACK(isac_color_list_onselected), spw); // change selection

	gtk_box_pack_start(GTK_BOX(bttn), gtk_hseparator_new(), FALSE, FALSE, 6); // 1st: hseparator
	gtk_box_pack_start(GTK_BOX(bttn), newb, FALSE, FALSE, 6); // 2nd: Add
	//gtk_box_pack_start(GTK_BOX(bttn), color_find_tag, FALSE, FALSE, 6); // naim@ecava.com
	gtk_box_pack_start(GTK_BOX(bttn), deleteb, FALSE, FALSE, 6); // 3rd: Delete
	gtk_box_pack_start(GTK_BOX(bttn), colorb, FALSE, FALSE, 6); // 4th: Color Picker
	gtk_box_pack_start(GTK_BOX(bttn), color_find_tag, FALSE, FALSE, 6); // naim@ecava.com
	gtk_box_pack_start(GTK_BOX(bttn), gtk_hseparator_new(), FALSE, FALSE, 6); // 5th: hseparator
	gtk_box_pack_start(GTK_BOX(bttn), upb, FALSE, FALSE, 6); // 6th: Move Up
	gtk_box_pack_start(GTK_BOX(bttn), downb, FALSE, FALSE, 6); // 7th: Move Down
	gtk_box_pack_start(GTK_BOX(bttn), gtk_hseparator_new(), FALSE, FALSE, 6); // 8th: hseparator

	g_signal_connect(G_OBJECT(vbox), "destroy", G_CALLBACK(color_deletelastrow), NULL);
	g_signal_connect(G_OBJECT(color_list), "destroy", G_CALLBACK(color_deletelastrow), NULL);

	// Info text at bottom of container
	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	gtk_box_pack_start(GTK_BOX(vbox), int_expander, FALSE, FALSE, 0);
	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	gtk_text_buffer_set_text(desc_buffer, "Numerical values shall associate with standard web color text or color code. Add more colors on one row to get blinking effect. e.g. 'red/blue'. "
		"To get transparent sequence, use 'none' in the series. e.g. 'green/yellow/white/none'.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_color_dsb", dsb);

	color_new_list(NULL, NULL);

	return (GtkWidget *)vbox;
}
GtkWidget *isac_menu_function_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Function" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl1, *lbl2, *lblx, *tf1, *tfx, *cb1, *scroll;

	GtkTooltips *tt = gtk_tooltips_new();

	scroll = gtk_scrolled_window_new(NULL, GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 1, 500, 1, 10, 10)));
	vb = gtk_vbox_new(FALSE, 6);
	tb = gtk_table_new(6, 3, FALSE);  //(rows, cols, homogeneous) dennis
	tb1 = gtk_table_new(3, 2, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	//gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(scroll),tb);
	//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	//gtk_box_pack_start (GTK_BOX (vb), scroll, TRUE, TRUE, 0); //disable as no multiple
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);

	// Create the Tag label
	lbl1 = gtk_label_new_with_mnemonic(_("_Name*"));
	gtk_misc_set_alignment(GTK_MISC(lbl1), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl1), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl1, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	//Create the Tag entry field
	tf1 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf1, _("Function name"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf1), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf1), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf1, 1, 3, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_func_name", tf1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf1);

	g_signal_connect(G_OBJECT(tf1), "focus-out-event",
		G_CALLBACK(isac_onfunctionfocusout), spw);

	/*
	lbl2 = gtk_label_new_with_mnemonic (_("_Effect"));
	gtk_misc_set_alignment (GTK_MISC (lbl2), 0, 0.5);
	gtk_label_set_width_chars (GTK_LABEL (lbl2), ISAC_LBL_LEN);
	gtk_table_attach ( GTK_TABLE (tb), lbl2, 0, 1, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
	(GtkAttachOptions)( GTK_SHRINK | GTK_FILL ),
	(GtkAttachOptions)0, 0, 0 );

	cb1 = gtk_combo_box_new_text ();
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Shadow");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Button");
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), 0);
	gtk_table_attach ( GTK_TABLE (tb), cb1, 1, 3, 1, 2,
	(GtkAttachOptions)( GTK_EXPAND | GTK_FILL ),
	(GtkAttachOptions)0, 0, 0 );
	gtk_object_set_data (GTK_OBJECT (spw), "isac_func_effect", cb1);
	gtk_label_set_mnemonic_widget (GTK_LABEL(lbl2), cb1);
	*/

	for (gint i = 1; i <= ISAC_FUNCTION_DEF_ENTRY; i++)
	{
		lblx = gtk_label_new_with_mnemonic(_("_Param "));
		gtk_misc_set_alignment(GTK_MISC(lblx), 0, 0.5);
		gtk_label_set_width_chars(GTK_LABEL(lblx), ISAC_LBL_LEN);
		gtk_table_attach(GTK_TABLE(tb), lblx, 0, 1, 1 + i, 2 + i,  //...,0,1,0,1 ... -> left, right, top, bottom
			(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
			(GtkAttachOptions)0, 0, 0);

		tfx = gtk_entry_new();
		gtk_tooltips_set_tip(tt, tfx, _("Parameter"), NULL);
		//gtk_entry_set_max_length (GTK_ENTRY (tfx), 64);
		gtk_entry_set_width_chars(GTK_ENTRY(tfx), ISAC_TF_LEN);
		gtk_table_attach(GTK_TABLE(tb), tfx, 1, 3, 1 + i, 2 + i,
			(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions)0, 0, 0);

		g_signal_connect(G_OBJECT(tfx), "focus-out-event",
			G_CALLBACK(isac_onfunctionfocusout), spw);

		gtk_object_set_data(GTK_OBJECT(spw), "isac_func_param", tfx);
		gtk_label_set_mnemonic_widget(GTK_LABEL(lblx), tfx);
	}

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	//gtk_object_set_data (GTK_OBJECT (spw), "interactivity", int_expander);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	//gtk_text_view_set_editable(GTK_TEXT_VIEW(tf), FALSE);
	gtk_text_buffer_set_text(desc_buffer, "This is Function animation", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	//gtk_table_attach_defaults (GTK_TABLE (tb1), tf, 1, 3, 2, 3);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_func_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_get_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Get" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl1, *lbl2, *lbl5, *tf1, *cb1, *cb2;

	GtkTooltips *tt = gtk_tooltips_new();

	vb = gtk_vbox_new(FALSE, 5);
	tb = gtk_table_new(5, 4, FALSE);  //(rows, cols, homogeneous) dennis
	tb1 = gtk_table_new(3, 2, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);

	// Create the Tag label
	lbl1 = gtk_label_new_with_mnemonic(_("_Tag*"));
	gtk_misc_set_alignment(GTK_MISC(lbl1), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl1), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl1, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	//Create the Tag entry field
	tf1 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf1, _("Tag name"), NULL);
	//gtk_entry_set_max_length (GTK_ENTRY (tf1), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf1), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf1, 1, 4, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_get_tag", tf1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf1);
	g_signal_connect(G_OBJECT(tf1), "focus-out-event",
		G_CALLBACK(isac_ongetfocusout), spw);
	gtk_widget_set_size_request(tf1, 300, 23);

	// naim@ecava.com : find button
	GtkWidget *find_button = gtk_button_new_with_label("...");
	gtk_misc_set_alignment(GTK_MISC(find_button), 1, 0.5);
	gtk_table_attach(GTK_TABLE(tb), find_button, 4, 5, 0, 1,
		(GtkAttachOptions)(GTK_FILL), // (GTK_EXPAND | GTK_FILL)
		(GtkAttachOptions)(GTK_SHRINK),
		0, 0);
	g_signal_connect(G_OBJECT(find_button), "clicked", G_CALLBACK(findtag_button_press_callback), tf1);
	gtk_widget_set_size_request(find_button, 20, 23);

	lbl2 = gtk_label_new_with_mnemonic(_("_Alignment"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl2), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 1, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	cb1 = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Right");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Left");
	// gtk_combo_box_append_text(GTK_COMBO_BOX(cb1 ), "Center");
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), 0);
	gtk_table_attach(GTK_TABLE(tb), cb1, 1, 5, 1, 2,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_get_align", cb1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl2), cb1);


	lbl5 = gtk_label_new_with_mnemonic(_("Typ_e"));
	gtk_misc_set_alignment(GTK_MISC(lbl5), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl5), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl5, 0, 1, 4, 5,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	cb2 = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb2), "Good");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb2), "Live");
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), 0);
	gtk_table_attach(GTK_TABLE(tb), cb2, 1, 5, 4, 5,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_get_type", cb2);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl5), cb2);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	//gtk_object_set_data (GTK_OBJECT (spw), "interactivity", int_expander);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	//gtk_text_view_set_editable(GTK_TEXT_VIEW(tf), FALSE);
	gtk_text_buffer_set_text(desc_buffer, "Display values in the text object. Use C/printf float convention like %05.2f to display 01.20 when the tag has the value 1.2", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	//gtk_table_attach_defaults (GTK_TABLE (tb1), tf, 1, 3, 2, 3);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_get_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_opacity_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Opacity" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl1, *lbl2, *lbl3, *tf1, *tf2, *tf3;

	GtkTooltips *tt = gtk_tooltips_new();

	vb = gtk_vbox_new(FALSE, 4);
	tb = gtk_table_new(4, 4, FALSE);  //(rows, cols, homogeneous) dennis
	tb1 = gtk_table_new(3, 2, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);

	// Create the Tag label
	lbl1 = gtk_label_new_with_mnemonic(_("_Tag*"));
	gtk_misc_set_alignment(GTK_MISC(lbl1), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl1), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl1, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	//Create the Tag entry field
	tf1 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf1, _("Tag name"), NULL);
	//gtk_entry_set_max_length (GTK_ENTRY (tf1), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf1), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf1, 1, 4, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_opac_tag", tf1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf1);
	g_signal_connect(G_OBJECT(tf1), "focus-out-event",
		G_CALLBACK(isac_onopacityfocusout), spw);
	gtk_widget_set_size_request(tf1, 300, 23);

	// naim@ecava.com : find button
	GtkWidget *find_button = gtk_button_new_with_label("...");
	gtk_misc_set_alignment(GTK_MISC(find_button), 1, 0.5);
	gtk_table_attach(GTK_TABLE(tb), find_button, 4, 5, 0, 1,
		(GtkAttachOptions)(GTK_FILL), // (GTK_EXPAND | GTK_FILL)
		(GtkAttachOptions)(GTK_SHRINK),
		0, 0);
	g_signal_connect(G_OBJECT(find_button), "clicked", G_CALLBACK(findtag_button_press_callback), tf1);
	gtk_widget_set_size_request(find_button, 20, 23);

	lbl2 = gtk_label_new_with_mnemonic(_("Mi_n*"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl2), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 1, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf2 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf2, _("The minimum value"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf2), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf2), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf2), "0");
	gtk_table_attach(GTK_TABLE(tb), tf2, 1, 5, 1, 2,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_opac_min", tf2);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl2), tf2);

	g_signal_connect(G_OBJECT(tf2), "focus-out-event",
		G_CALLBACK(isac_onopacityfocusout), spw);

	lbl3 = gtk_label_new_with_mnemonic(_("Ma_x*"));
	gtk_misc_set_alignment(GTK_MISC(lbl3), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl3), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl3, 0, 1, 2, 3,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf3 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf3, _("The maximum value"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf3), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf3), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf3), "1");
	gtk_table_attach(GTK_TABLE(tb), tf3, 1, 5, 2, 3,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_opac_max", tf3);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl3), tf3);

	g_signal_connect(G_OBJECT(tf3), "focus-out-event",
		G_CALLBACK(isac_onopacityfocusout), spw);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	//gtk_object_set_data (GTK_OBJECT (spw), "interactivity", int_expander);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	//gtk_text_view_set_editable(GTK_TEXT_VIEW(tf), FALSE);
	gtk_text_buffer_set_text(desc_buffer, "Transparency animation according to max & min. Use digital value for show & hide effect.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	//gtk_table_attach_defaults (GTK_TABLE (tb1), tf, 1, 3, 2, 3);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_opac_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_open_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Open" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl1, *lbl2, *lbl4, *lbl5, *lbl6, *lbl7, *lbl8, *lbl9, *tf2, *tf3, *tf4, *tf5, *tf6, *cb2, *cb3, *cb4;

	GtkTooltips *tt = gtk_tooltips_new();

	vb = gtk_vbox_new(FALSE, 3);
	tb = gtk_table_new(8, 3, FALSE);  //(rows, cols, homogeneous) dennis 
	tb1 = gtk_table_new(3, 2, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);

	// Create the Tag label    
	lbl2 = gtk_label_new_with_mnemonic(_("_Source*"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl2), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf2 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf2, _("Source"), NULL);
	//gtk_entry_set_max_length (GTK_ENTRY (tf2), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf2), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf2, 1, 3, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_open_source", tf2);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf2);

	g_signal_connect(G_OBJECT(tf2), "focus-out-event",
		G_CALLBACK(isac_onopenfocusout), spw);

	lbl2 = gtk_label_new_with_mnemonic(_("_Source Type"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl2), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 1, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	cb3 = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb3), "URL");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb3), "Tag");
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb3), 0);
	gtk_table_attach(GTK_TABLE(tb), cb3, 1, 3, 1, 2,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_open_srctype", cb3);


	lbl9 = gtk_label_new_with_mnemonic(_("_Dest. Type"));
	gtk_misc_set_alignment(GTK_MISC(lbl9), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl9), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl9, 0, 1, 2, 3,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	cb4 = gtk_combo_box_entry_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb4), "Current window");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb4), "New exclusive window");	//HSH - Renamed; previously "New window"
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb4), "New shared window");		//HSH - Added
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb4), 0);
	gtk_tooltips_set_tip(tt, cb4, _("Destination type"), NULL);
	gtk_table_attach(GTK_TABLE(tb), cb4, 1, 3, 2, 3,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_open_type", cb4);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), cb4);

	g_signal_connect(G_OBJECT(cb4), "changed",
		G_CALLBACK(isac_open_combo_changed), spw);

	lbl4 = gtk_label_new_with_mnemonic(_("_X-position"));
	gtk_misc_set_alignment(GTK_MISC(lbl4), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl4), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl4, 0, 1, 4, 5,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf3 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf4, _("X-position"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf3), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf3), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf3), "100");
	gtk_widget_set_sensitive(tf3, FALSE);
	gtk_table_attach(GTK_TABLE(tb), tf3, 1, 3, 4, 5,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_open_xpos", tf3);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf3);

	g_signal_connect(G_OBJECT(tf3), "focus-out-event",
		G_CALLBACK(isac_onopenfocusout), spw);

	lbl5 = gtk_label_new_with_mnemonic(_("_Y-position"));
	gtk_misc_set_alignment(GTK_MISC(lbl5), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl5), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl5, 0, 1, 5, 6,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf4 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf4, _("Y-position"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf4), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf4), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf4), "100");
	gtk_widget_set_sensitive(tf4, FALSE);
	gtk_table_attach(GTK_TABLE(tb), tf4, 1, 3, 5, 6,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_open_ypos", tf4);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf4);

	g_signal_connect(G_OBJECT(tf4), "focus-out-event",
		G_CALLBACK(isac_onopenfocusout), spw);

	lbl6 = gtk_label_new_with_mnemonic(_("_Width"));
	gtk_misc_set_alignment(GTK_MISC(lbl6), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl6), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl6, 0, 1, 6, 7,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf5 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf5, _("Width"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf5), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf5), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf5), "500");
	gtk_widget_set_sensitive(tf5, FALSE);
	gtk_table_attach(GTK_TABLE(tb), tf5, 1, 3, 6, 7,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_open_width", tf5);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf5);

	g_signal_connect(G_OBJECT(tf5), "focus-out-event",
		G_CALLBACK(isac_onopenfocusout), spw);

	lbl7 = gtk_label_new_with_mnemonic(_("_Height"));
	gtk_misc_set_alignment(GTK_MISC(lbl7), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl7), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl7, 0, 1, 7, 8,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf6 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf6, _("Height"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf6), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf6), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf6), "400");
	gtk_widget_set_sensitive(tf6, FALSE);
	gtk_table_attach(GTK_TABLE(tb), tf6, 1, 3, 7, 8,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_open_height", tf6);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf6);

	g_signal_connect(G_OBJECT(tf6), "focus-out-event",
		G_CALLBACK(isac_onopenfocusout), spw);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	//gtk_object_set_data (GTK_OBJECT (spw), "interactivity", int_expander);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	//gtk_text_view_set_editable(GTK_TEXT_VIEW(tf), FALSE);
	gtk_text_buffer_set_text(desc_buffer, "Clicking on this object shall show the screen in 'Current window' or pop-up in another 'New Window'.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	//gtk_table_attach_defaults (GTK_TABLE (tb1), tf, 1, 3, 2, 3);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_open_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_popup_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Pop up" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl1, *lbl2, *lbl4, *lbl5, *lbl6, *lbl7, *lbl8, *lbl9, *tf2, *tf3, *tf4, *tf5, *tf6, *cb2, *cb3, *cb4;

	GtkTooltips *tt = gtk_tooltips_new();

	vb = gtk_vbox_new(FALSE, 3);
	tb = gtk_table_new(8, 3, FALSE);  //(rows, cols, homogeneous) dennis 
	tb1 = gtk_table_new(3, 2, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);


	lbl2 = gtk_label_new_with_mnemonic(_("_Source*"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl2), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf2 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf2, _("Source"), NULL);
	//gtk_entry_set_max_length (GTK_ENTRY (tf2), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf2), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf2, 1, 3, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_popup_source", tf2);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf2);

	g_signal_connect(G_OBJECT(tf2), "focus-out-event",
		G_CALLBACK(isac_onpopupfocusout), spw);


	lbl4 = gtk_label_new_with_mnemonic(_("_X-position"));
	gtk_misc_set_alignment(GTK_MISC(lbl4), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl4), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl4, 0, 1, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf3 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf4, _("X-position"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf3), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf3), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf3), "100");
	gtk_widget_set_sensitive(tf3, TRUE);
	gtk_table_attach(GTK_TABLE(tb), tf3, 1, 3, 1, 2,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_popup_xpos", tf3);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf3);

	g_signal_connect(G_OBJECT(tf3), "focus-out-event",
		G_CALLBACK(isac_onpopupfocusout), spw);

	lbl5 = gtk_label_new_with_mnemonic(_("_Y-position"));
	gtk_misc_set_alignment(GTK_MISC(lbl5), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl5), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl5, 0, 1, 2, 3,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf4 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf4, _("Y-position"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf4), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf4), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf4), "100");
	gtk_widget_set_sensitive(tf4, TRUE);
	gtk_table_attach(GTK_TABLE(tb), tf4, 1, 3, 2, 3,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_popup_ypos", tf4);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf4);

	g_signal_connect(G_OBJECT(tf4), "focus-out-event",
		G_CALLBACK(isac_onpopupfocusout), spw);

	lbl6 = gtk_label_new_with_mnemonic(_("_Width"));
	gtk_misc_set_alignment(GTK_MISC(lbl6), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl6), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl6, 0, 1, 3, 4,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf5 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf5, _("Width"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf5), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf5), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf5), "500");
	gtk_widget_set_sensitive(tf5, TRUE);
	gtk_table_attach(GTK_TABLE(tb), tf5, 1, 3, 3, 4,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_popup_width", tf5);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf5);

	g_signal_connect(G_OBJECT(tf5), "focus-out-event",
		G_CALLBACK(isac_onpopupfocusout), spw);

	lbl7 = gtk_label_new_with_mnemonic(_("_Height"));
	gtk_misc_set_alignment(GTK_MISC(lbl7), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl7), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl7, 0, 1, 4, 5,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf6 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf6, _("Height"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf6), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf6), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf6), "400");
	gtk_widget_set_sensitive(tf6, TRUE);
	gtk_table_attach(GTK_TABLE(tb), tf6, 1, 3, 4, 5,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_popup_height", tf6);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf6);

	g_signal_connect(G_OBJECT(tf6), "focus-out-event",
		G_CALLBACK(isac_onpopupfocusout), spw);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	gtk_text_buffer_set_text(desc_buffer, "Clicking this object shall pop up the htm (or html) page as entered in Source.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_popup_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_rotate_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Rotate" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl1, *lbl2, *lbl3, *tf1, *tf2, *tf3;

	GtkTooltips *tt = gtk_tooltips_new();

	vb = gtk_vbox_new(FALSE, 6);
	tb = gtk_table_new(10, 4, FALSE);  //(rows, cols, homogeneous) dennis 
	tb1 = gtk_table_new(3, 2, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);

	// Create the Tag label
	lbl1 = gtk_label_new_with_mnemonic(_("_Tag*"));
	gtk_misc_set_alignment(GTK_MISC(lbl1), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl1), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl1, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	//Create the Tag entry field
	tf1 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf1, _("Tag name"), NULL);
	gtk_entry_set_width_chars(GTK_ENTRY(tf1), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf1, 1, 4, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_rot_tag", tf1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf1);
	g_signal_connect(G_OBJECT(tf1), "focus-out-event",
		G_CALLBACK(isac_onrotatefocusout), spw);
	gtk_widget_set_size_request(tf1, 300, 23);

	// naim@ecava.com : find button
	GtkWidget *find_button = gtk_button_new_with_label("...");
	gtk_misc_set_alignment(GTK_MISC(find_button), 1, 0.5);
	gtk_table_attach(GTK_TABLE(tb), find_button, 4, 5, 0, 1,
		(GtkAttachOptions)(GTK_FILL), // (GTK_EXPAND | GTK_FILL)
		(GtkAttachOptions)(GTK_SHRINK),
		0, 0);
	g_signal_connect(G_OBJECT(find_button), "clicked", G_CALLBACK(findtag_button_press_callback), tf1);
	gtk_widget_set_size_request(find_button, 20, 23);

	lbl2 = gtk_label_new_with_mnemonic(_("Mi_n"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl2), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 1, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf2 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf2, _("Minimum value"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf2), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf2), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf2), "0");
	gtk_table_attach(GTK_TABLE(tb), tf2, 1, 5, 1, 2,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_rot_min", tf2);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl2), tf2);

	g_signal_connect(G_OBJECT(tf2), "focus-out-event",
		G_CALLBACK(isac_onrotatefocusout), spw);

	lbl3 = gtk_label_new_with_mnemonic(_("Ma_x"));
	gtk_misc_set_alignment(GTK_MISC(lbl3), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl3), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl3, 0, 1, 2, 3,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf3 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf3, _("Maximum value"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf3), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf3), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf3), "100");
	gtk_table_attach(GTK_TABLE(tb), tf3, 1, 5, 2, 3,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_rot_max", tf3);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl3), tf3);

	g_signal_connect(G_OBJECT(tf3), "focus-out-event",
		G_CALLBACK(isac_onrotatefocusout), spw);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	gtk_text_buffer_set_text(desc_buffer, "Rotation animation which associate min to 0 degree & max to 360 degree. Set min bigger than max shall get counter clockwise rotation.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_rot_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_script_init(GtkObject *spw)
{
	/*****************************
	ISAC Features
	"Script" tab menu initialization
	Setting up all the related ebtry fields
	*****************************/
	GtkWidget *hbox, *vbox, *scroll, *tf, *textframe, *tbscroll, *tbspare, *tscroll, *checkbutton, *keydown_table, *lbl;//, *desc_tb, *desc_scroll;
	GtkTooltips *tt = gtk_tooltips_new();
	GtkTreeSelection *selection;
	GtkListStore *store;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkTreeIter iter;
	GtkTreePath *path = gtk_tree_path_new_first();
	GtkTextBuffer *buffer;
	char *event[] = { "mouseup", "mousedown", "mouseover", "mouseout", "mousemove" , "keydown"};	//HSH: added keydown

	hbox = gtk_hbox_new(FALSE, 4);
	vbox = gtk_vbox_new(FALSE, 4);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);

	tscroll = gtk_scrolled_window_new(NULL, GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 1, 500, 1, 10, 10)));
	tbscroll = gtk_table_new(8, 7, FALSE); //rows, cols, homogeneous
	tbspare = gtk_table_new(3, 7, FALSE); //rows, cols, homogeneous

	gtk_container_set_border_width(GTK_CONTAINER(tbscroll), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tbscroll), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tbscroll), 4);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(tscroll), tbscroll);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(tscroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	store = gtk_list_store_new(7, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING);	//HSH add keydown settings to list store
	script_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(script_list), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(script_list), TRUE);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Event", renderer, "text", 0, "underline", 1, NULL);
	gtk_tree_view_column_set_alignment(column, 0.5);
	gtk_tree_view_column_set_min_width(column, 80);
	gtk_tree_view_append_column(GTK_TREE_VIEW(script_list), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Syntax", renderer, "text", 2, NULL);

	//HSH - keydown start	
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Keydown", renderer, "active", 3, "active", 4, "active", 5, "text", 6, NULL);
	//HSH - keydown end
	
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_ETCHED_IN);
	gtk_container_add(GTK_CONTAINER(scroll), script_list);
	gtk_box_pack_start(GTK_BOX(hbox), scroll, FALSE, TRUE, 0);

	for (int i = 0; i< 6; i++) // append the tab events "mouseup" "mousedown" "mouseover" "mouseout" "mousemove" //HSH "keydown"
	{
		gtk_list_store_append(store, &iter);
		// append to column 0 each tab event & append to column 2 with null string
		gtk_list_store_set(store, &iter, 0, event[i], 2, "", 3, FALSE, 4, FALSE, 5, FALSE, 6, "", -1);	//HSH 3 - ctrl, 4 - alt, 5 - shift, 6 - key
	}

	gtk_tree_view_set_cursor(GTK_TREE_VIEW(script_list), path, gtk_tree_view_get_column(GTK_TREE_VIEW(script_list), 0), TRUE);

	//HSH keydown start
	keydown_table = gtk_table_new(1, 5, FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(keydown_table), 4);

	checkbutton = gtk_check_button_new_with_mnemonic(_("_Ctrl"));
	gtk_table_attach(GTK_TABLE(keydown_table), checkbutton, 0, 1, 0, 1,
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_script_keydown_ctrl", checkbutton);
	g_signal_connect(G_OBJECT(checkbutton), "toggled", G_CALLBACK(script_onchange_save_toggle), spw);
	
	checkbutton = gtk_check_button_new_with_mnemonic(_("_Alt"));
	gtk_table_attach(GTK_TABLE(keydown_table), checkbutton, 1, 2, 0, 1,
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_script_keydown_alt", checkbutton);
	g_signal_connect(G_OBJECT(checkbutton), "toggled", G_CALLBACK(script_onchange_save_toggle), spw);

	checkbutton = gtk_check_button_new_with_mnemonic(_("_Shift"));
	gtk_table_attach(GTK_TABLE(keydown_table), checkbutton, 2, 3, 0, 1,
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_script_keydown_shift", checkbutton);
	g_signal_connect(G_OBJECT(checkbutton), "toggled", G_CALLBACK(script_onchange_save_toggle), spw);

	lbl = gtk_label_new_with_mnemonic(_("Key:"));
	gtk_misc_set_alignment(GTK_MISC(lbl), 1, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl), 6);
	gtk_table_attach(GTK_TABLE(keydown_table), lbl, 3, 4, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf, _("Key"), NULL);
	gtk_misc_set_alignment(GTK_MISC(tf), 1, 0);
	gtk_entry_set_width_chars(GTK_ENTRY(tf), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(keydown_table), tf, 4, 5, 0, 1,
		(GtkAttachOptions) 0,//(GTK_EXPAND),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_script_keydown_key", tf);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl), tf);
	g_signal_connect(G_OBJECT(tf), "key-release-event",
		G_CALLBACK(script_onchange_save), spw);
	g_signal_connect(G_OBJECT(tf), "focus-out-event",
		G_CALLBACK(isac_onscriptfocusout), spw);
		
	//gtk_table_attach_defaults(GTK_TABLE(tbscroll), keydown_table, 0, 7, 0, 1);
	gtk_table_attach(GTK_TABLE(tbscroll), keydown_table, 0, 7, 0, 1,
		(GtkAttachOptions)(GTK_SHRINK| GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_script_keydown_table", keydown_table);
	gtk_widget_set_sensitive (keydown_table, FALSE);
	gtk_widget_hide(keydown_table);
	//HSH keydown end

	textframe = gtk_frame_new(NULL);
	gtk_container_set_border_width(GTK_CONTAINER(textframe), 4);
	gtk_widget_set_sensitive(GTK_WIDGET(textframe), TRUE);
	gtk_table_attach_defaults(GTK_TABLE(tbscroll), textframe, 0, 7, 1, 8);	//HSH - move down the table to make way for keydown options
	gtk_widget_show(textframe);
	gtk_frame_set_shadow_type(GTK_FRAME(textframe), GTK_SHADOW_IN);
	gtk_object_set_data(GTK_OBJECT(spw), "desc_frame", textframe);

	tf = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	gtk_text_buffer_set_text(buffer, "", -1);
	gtk_container_add(GTK_CONTAINER(textframe), tf);

	gtk_object_set_data(GTK_OBJECT(spw), "isac_script_code", tf);
	gtk_label_set_mnemonic_widget(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(textframe))), tf);
	g_signal_connect(G_OBJECT(tf), "focus-out-event",
		G_CALLBACK(isac_onscriptfocusout), spw);

	g_signal_connect(G_OBJECT(tf), "key-release-event",
		G_CALLBACK(script_onchange_save), spw);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(script_list));

	g_signal_connect(selection, "changed",
		G_CALLBACK(script_onchange), spw);

	gtk_box_pack_start(GTK_BOX(hbox), tscroll, TRUE, TRUE, 0);
	//HSH: Keydown; inserted tscroll into desc_vbox then into the hbox (2 lines below)
	//gtk_table_attach(GTK_TABLE(desc_tb), tscroll, 0, 1, 1, 8,
	//	(GtkAttachOptions)(GTK_EXPAND| GTK_FILL),
	//	(GtkAttachOptions)0, 0, 0);
	//gtk_box_pack_start(GTK_BOX(hbox), desc_tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), tbspare, FALSE, FALSE, 0);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	gtk_box_pack_start(GTK_BOX(vbox), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf1 = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf1), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf1), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf1));
	gtk_text_buffer_set_text(desc_buffer, "Run user's JavaScript when this object is triggered with the selected event.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf1);
	gtk_widget_show(tf1);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_script_dsb", dsb);

	gtk_tree_path_free(path);
	return (GtkWidget *)vbox;
}
GtkWidget *isac_menu_set_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Set" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl1, *lbl2, *lbl3, *lbl4, *lbl5, *lbl6, *tf1, *tf2, *tf3, *cb1, *cb2, *cb3;

	GtkTooltips *tt = gtk_tooltips_new();

	vb = gtk_vbox_new(FALSE, 6);
	tb = gtk_table_new(6, 4, FALSE);  //(rows, cols, homogeneous) dennis
	tb1 = gtk_table_new(3, 2, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);

	// Create the Tag label
	lbl1 = gtk_label_new_with_mnemonic(_("_Tag*"));
	gtk_misc_set_alignment(GTK_MISC(lbl1), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl1), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl1, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	//Create the Tag entry field
	tf1 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf1, _("Tag name"), NULL);
	gtk_entry_set_width_chars(GTK_ENTRY(tf1), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf1, 1, 4, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_set_tag", tf1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf1);
	g_signal_connect(G_OBJECT(tf1), "focus-out-event",
		G_CALLBACK(isac_onsetfocusout), spw);
	gtk_widget_set_size_request(tf1, 300, 23);

	// naim@ecava.com : find button
	GtkWidget *find_button = gtk_button_new_with_label("...");
	gtk_misc_set_alignment(GTK_MISC(find_button), 1, 0.5);
	gtk_table_attach(GTK_TABLE(tb), find_button, 4, 5, 0, 1,
		(GtkAttachOptions)(GTK_FILL), // (GTK_EXPAND | GTK_FILL)
		(GtkAttachOptions)(GTK_SHRINK),
		0, 0);
	g_signal_connect(G_OBJECT(find_button), "clicked", G_CALLBACK(findtag_button_press_callback), tf1);
	gtk_widget_set_size_request(find_button, 20, 23);

	lbl2 = gtk_label_new_with_mnemonic(_("_Alignment"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl2), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 1, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	cb1 = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Right");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Left");
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), 0);
	gtk_table_attach(GTK_TABLE(tb), cb1, 1, 5, 1, 2,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_set_align", cb1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl2), cb1);


	lbl6 = gtk_label_new_with_mnemonic(_("_Prompt"));
	gtk_misc_set_alignment(GTK_MISC(lbl6), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl6), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl6, 0, 1, 3, 4,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf3 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf3, _("Prompt string"), NULL);
	//gtk_entry_set_max_length (GTK_ENTRY (tf3), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf3), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf3, 1, 5, 3, 4,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_set_prompt", tf3);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl6), tf3);

	g_signal_connect(G_OBJECT(tf3), "focus-out-event",
		G_CALLBACK(isac_onsetfocusout), spw);

	lbl3 = gtk_label_new_with_mnemonic(_("_Source"));
	gtk_misc_set_alignment(GTK_MISC(lbl3), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl3), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl3, 0, 1, 4, 5,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf2 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf2, _("Source value"), NULL);
	gtk_entry_set_width_chars(GTK_ENTRY(tf2), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf2, 1, 5, 4, 5,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_set_source", tf2);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl3), tf2);

	lbl4 = gtk_label_new_with_mnemonic(_("Typ_e"));
	gtk_misc_set_alignment(GTK_MISC(lbl4), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl4), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl4, 0, 1, 5, 6,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	g_signal_connect(G_OBJECT(tf2), "focus-out-event",
		G_CALLBACK(isac_onsetfocusout), spw);

	cb2 = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb2), "Data");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb2), "Variable");
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb2), 0);
	gtk_table_attach(GTK_TABLE(tb), cb2, 1, 5, 5, 6,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_set_type", cb2);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl4), cb2);


	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	gtk_text_buffer_set_text(desc_buffer, "Set parameters into associate tag or special function.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_set_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_slider_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Slider" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl1, *lbl2, *lbl3, *tf1, *tf2, *tf3, *cb;

	GtkTooltips *tt = gtk_tooltips_new();

	vb = gtk_vbox_new(FALSE, 4);
	tb = gtk_table_new(4, 4, FALSE);  //(rows, cols, homogeneous) dennis
	tb1 = gtk_table_new(3, 2, FALSE);


	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);

	// Create the Tag label
	lbl1 = gtk_label_new_with_mnemonic(_("_Tag*"));
	gtk_misc_set_alignment(GTK_MISC(lbl1), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl1), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl1, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	//Create the Tag entry field
	tf1 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf1, _("Tag name"), NULL);
	gtk_entry_set_width_chars(GTK_ENTRY(tf1), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf1, 1, 4, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	g_signal_connect(G_OBJECT(tf1), "focus-out-event",
		G_CALLBACK(isac_onsliderfocusout), spw);
	gtk_widget_set_size_request(tf1, 300, 23);

	// naim@ecava.com : find button
	GtkWidget *find_button = gtk_button_new_with_label("...");
	gtk_misc_set_alignment(GTK_MISC(find_button), 1, 0.5);
	gtk_table_attach(GTK_TABLE(tb), find_button, 4, 5, 0, 1,
		(GtkAttachOptions)(GTK_FILL), // (GTK_EXPAND | GTK_FILL)
		(GtkAttachOptions)(GTK_SHRINK),
		0, 0);
	g_signal_connect(G_OBJECT(find_button), "clicked", G_CALLBACK(findtag_button_press_callback), tf1);
	gtk_widget_set_size_request(find_button, 20, 23);

	gtk_object_set_data(GTK_OBJECT(spw), "isac_slider_tag", tf1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf1);

	lbl2 = gtk_label_new_with_mnemonic(_("Mi_n*"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl2), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 1, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf2 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf2, _("The minimum value"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf2), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf2), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf2), "0");
	gtk_table_attach(GTK_TABLE(tb), tf2, 1, 5, 1, 2,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_slider_min", tf2);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl2), tf2);

	g_signal_connect(G_OBJECT(tf2), "focus-out-event",
		G_CALLBACK(isac_onsliderfocusout), spw);

	lbl3 = gtk_label_new_with_mnemonic(_("Ma_x*"));
	gtk_misc_set_alignment(GTK_MISC(lbl3), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl3), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl3, 0, 1, 2, 3,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf3 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf3, _("The maximum value"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf3), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf3), ISAC_TF_LEN);
	gtk_entry_set_text(GTK_ENTRY(tf3), "100");
	gtk_table_attach(GTK_TABLE(tb), tf3, 1, 5, 2, 3,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_slider_max", tf3);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl3), tf3);

	g_signal_connect(G_OBJECT(tf3), "focus-out-event",
		G_CALLBACK(isac_onsliderfocusout), spw);

	cb = gtk_check_button_new_with_mnemonic(_("Read Only"));
	gtk_table_attach(GTK_TABLE(tb), cb, 1, 3, 3, 4,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_slider_read", cb);

	g_signal_connect(G_OBJECT(cb), "active",
		G_CALLBACK(isac_onsliderfocusout), spw);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	gtk_text_buffer_set_text(desc_buffer, "Position graphic between two locations. After entering tag, max & min, press ALT+D to clone one more similar object, then drag the clone object to desired max destination. This clone object shall disappear during run-time leaving only the original object positioned at origin which will move along origin & destination proportionally according to max & min value.\n\nTurn on 'Read only' check box to disable dragging.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_slider_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_text_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Text" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *tbscroll, *tbspare, *lbl1, *lbl2, *lbl3, *lblx, *tf1, *tfx, *scroll;

	GtkTooltips *tt = gtk_tooltips_new();

	scroll = gtk_scrolled_window_new(NULL, GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 1, 500, 1, 10, 10)));
	vb = gtk_vbox_new(FALSE, 6);
	tb = gtk_table_new(2, 5, FALSE);  //(rows, cols, homogeneous) dennis
	tb1 = gtk_table_new(3, 2, FALSE);
	tbscroll = gtk_table_new(8, 5, FALSE);  //(rows, cols, homogeneous) dennis
	tbspare = gtk_table_new(3, 5, FALSE);  //(rows, cols, homogeneous) dennis


	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tbscroll), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tbscroll), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tbscroll), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll), tbscroll);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vb), tb, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vb), scroll, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tbspare, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);

	// Create the Tag label
	lbl1 = gtk_label_new_with_mnemonic(_("_Tag*"));
	gtk_misc_set_alignment(GTK_MISC(lbl1), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl1), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl1, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	//Create the Tag entry field
	tf1 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf1, _("Tag name"), NULL);
	gtk_entry_set_width_chars(GTK_ENTRY(tf1), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf1, 1, 4, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_text_tag", tf1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf1);
	g_signal_connect(G_OBJECT(tf1), "focus-out-event",
		G_CALLBACK(isac_ontextfocusout), spw);
	gtk_widget_set_size_request(tf1, 300, 23);

	// naim@ecava.com : find button
	GtkWidget *find_button = gtk_button_new_with_label("...");
	gtk_misc_set_alignment(GTK_MISC(find_button), 1, 0.5);
	gtk_table_attach(GTK_TABLE(tb), find_button, 4, 5, 0, 1,
		(GtkAttachOptions)(GTK_FILL), // (GTK_EXPAND | GTK_FILL)
		(GtkAttachOptions)(GTK_SHRINK),
		0, 0);
	g_signal_connect(G_OBJECT(find_button), "clicked", G_CALLBACK(findtag_button_press_callback), tf1);
	gtk_widget_set_size_request(find_button, 20, 23);

	lbl2 = gtk_label_new(_("Tag Value"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_table_attach(GTK_TABLE(tbscroll), lbl2, 0, 2, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	lbl3 = gtk_label_new(_("Tag Text"));
	gtk_misc_set_alignment(GTK_MISC(lbl3), 0, 0.5);
	gtk_table_attach(GTK_TABLE(tbscroll), lbl3, 3, 4, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	for (gint i = 1; i <= ISAC_TEXT_DEF_ENTRY; i++)
	{
		tfx = gtk_entry_new();
		gtk_tooltips_set_tip(tt, tfx, _("Pair value"), NULL);
		gtk_entry_set_width_chars(GTK_ENTRY(tfx), ISAC_TF_SHORT_LEN);
		gtk_table_attach(GTK_TABLE(tbscroll), tfx, 0, 1, 1 + i, 2 + i,
			(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
			(GtkAttachOptions)0, 0, 0);

		g_signal_connect(G_OBJECT(tfx), "focus-out-event",
			G_CALLBACK(isac_ontextfocusout), spw);

		text_value_list = g_slist_prepend(text_value_list, tfx);

		if (i == 1)
		{
			lblx = gtk_label_new(_("*"));
			gtk_misc_set_alignment(GTK_MISC(lblx), 0, 0.5);
			gtk_table_attach(GTK_TABLE(tbscroll), lblx, 1, 2, 1 + i, 2 + i,  //...,0,1,0,1 ... -> left, right, top, bottom
				(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
				(GtkAttachOptions)0, 0, 0);
		}
		lblx = gtk_label_new(_("  ="));
		gtk_misc_set_alignment(GTK_MISC(lblx), 0, 0.5);
		gtk_table_attach(GTK_TABLE(tbscroll), lblx, 2, 3, 1 + i, 2 + i,  //...,0,1,0,1 ... -> left, right, top, bottom
			(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
			(GtkAttachOptions)0, 0, 0);

		tfx = gtk_entry_new();
		gtk_tooltips_set_tip(tt, tfx, _("Display text"), NULL);
		gtk_entry_set_max_length(GTK_ENTRY(tfx), 64);
		gtk_entry_set_width_chars(GTK_ENTRY(tfx), ISAC_TF_LEN);
		gtk_table_attach(GTK_TABLE(tbscroll), tfx, 3, 4, 1 + i, 2 + i,
			(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions)0, 0, 0);

		g_signal_connect(G_OBJECT(tfx), "focus-out-event",
			G_CALLBACK(isac_ontextfocusout), spw);

		text_display_list = g_slist_prepend(text_display_list, tfx);

		if (i == 1)
		{
			lblx = gtk_label_new(_("*"));
			gtk_misc_set_alignment(GTK_MISC(lblx), 0, 0.5);
			gtk_table_attach(GTK_TABLE(tbscroll), lblx, 4, 5, 1 + i, 2 + i,  //...,0,1,0,1 ... -> left, right, top, bottom
				(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
				(GtkAttachOptions)0, 0, 0);
		}
	}

	text_value_list = g_slist_reverse(text_value_list);
	text_display_list = g_slist_reverse(text_display_list);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	gtk_text_buffer_set_text(desc_buffer, "Association of tag enumerated values with text string, e.g. 1 = 'OPEN', 2 = 'MOVING', 3 = 'CLOSED', 4 = 'FAILED'. For digital values use IEC60870 double point convention 1=OFF, 2=ON, 0 and 3=INVALID or TRANSIT, +128 for BAD quality, f=all invalid states, a=alarmed point.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_text_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_tooltips_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Tooltips" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl2, *lbl3, *lbl4, *lblx, *tf2, *tf3, *tf4, *tfx;

	GtkTooltips *tt = gtk_tooltips_new();

	vb = gtk_vbox_new(FALSE, 4);
	tb = gtk_table_new(10, 3, FALSE);  //(rows, cols, homogeneous) dennis
	tb1 = gtk_table_new(3, 2, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);


	gint i = 0;
	for (i = 0; i<ISAC_TOOLTIPS_DEF_ENTRY; i++)
	{

		if (i == 0)
			lblx = gtk_label_new_with_mnemonic(_(g_strdup_printf("%s%i%s", "_Line ", i + 1, "*")));
		else
			lblx = gtk_label_new_with_mnemonic(_(g_strdup_printf("%s%i", "_Line ", i + 1)));

		gtk_misc_set_alignment(GTK_MISC(lblx), 0, 0.5);
		gtk_label_set_width_chars(GTK_LABEL(lblx), ISAC_LBL_LEN);
		gtk_table_attach(GTK_TABLE(tb), lblx, 0, 1, 0 + i, 1 + i,  //...,0,1,0,1 ... -> left, right, top, bottom
			(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
			(GtkAttachOptions)0, 0, 0);

		tfx = gtk_entry_new();
		gtk_tooltips_set_tip(tt, tfx, _("Tooltips"), NULL);
		gtk_entry_set_max_length(GTK_ENTRY(tfx), 128);
		gtk_entry_set_width_chars(GTK_ENTRY(tfx), ISAC_TF_LEN);
		gtk_table_attach(GTK_TABLE(tb), tfx, 1, 3, 0 + i, 1 + i,
			(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions)0, 0, 0);

		gtk_label_set_mnemonic_widget(GTK_LABEL(lblx), tfx);

		tt_value_list = g_slist_prepend(tt_value_list, tfx);

		g_signal_connect(G_OBJECT(tfx), "focus-out-event",
			G_CALLBACK(isac_ontooltipsfocusout), spw);

	}

	tt_value_list = g_slist_reverse(tt_value_list);

	lbl3 = gtk_label_new_with_mnemonic(_("_Size"));
	gtk_misc_set_alignment(GTK_MISC(lbl3), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl3), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl3, 0, 1, i + 1, i + 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf3 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf3, _("The font size"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf3), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf3), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf3, 1, 3, i + 1, i + 2,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_tooltips_size", tf3);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl3), tf3);

	g_signal_connect(G_OBJECT(tf3), "focus-out-event",
		G_CALLBACK(isac_ontooltipsfocusout), spw);

	lbl4 = gtk_label_new_with_mnemonic(_("Styl_e"));
	gtk_misc_set_alignment(GTK_MISC(lbl4), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl4), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl4, 0, 1, i + 2, i + 3,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	tf4 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf4, _("The style"), NULL);
	gtk_entry_set_max_length(GTK_ENTRY(tf4), 64);
	gtk_entry_set_width_chars(GTK_ENTRY(tf4), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf4, 1, 3, i + 2, i + 3,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_tooltips_style", tf4);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl3), tf4);

	g_signal_connect(G_OBJECT(tf4), "focus-out-event",
		G_CALLBACK(isac_ontooltipsfocusout), spw);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	gtk_text_buffer_set_text(desc_buffer, "Additional hint for end user when mouse over this graphic.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_tooltips_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_zoom_init(GtkObject *spw)
{
	/*****************************
	ISAC Features:
	"Zoom" tab menu initialization.
	Setting up all the related entry fields.
	*****************************/
	GtkWidget *vb, *tb, *tb1, *lbl1, *lbl2, *tf1, *cb1;

	GtkTooltips *tt = gtk_tooltips_new();

	vb = gtk_vbox_new(FALSE, 3);
	tb = gtk_table_new(3, 4, FALSE);  //(rows, cols, homogeneous) dennis 
	tb1 = gtk_table_new(3, 2, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tb1), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb1), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb1), 4);
	gtk_box_pack_start(GTK_BOX(vb), tb, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tb1, FALSE, TRUE, 0);

	lbl2 = gtk_label_new_with_mnemonic(_("_Alignment*"));
	gtk_misc_set_alignment(GTK_MISC(lbl2), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl2), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl2, 0, 1, 0, 1,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	cb1 = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "None");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Center");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Left");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Right");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Top");
	gtk_combo_box_append_text(GTK_COMBO_BOX(cb1), "Bottom");
	gtk_combo_box_set_active(GTK_COMBO_BOX(cb1), 0);
	gtk_table_attach(GTK_TABLE(tb), cb1, 1, 5, 0, 1,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_zoom_align", cb1);

	g_signal_connect(G_OBJECT(cb1), "changed",
		G_CALLBACK(isac_zoom_combo_changed), spw);


	// Create the Tag label
	lbl1 = gtk_label_new_with_mnemonic(_("_Tag"));
	gtk_misc_set_alignment(GTK_MISC(lbl1), 0, 0.5);
	gtk_label_set_width_chars(GTK_LABEL(lbl1), ISAC_LBL_LEN);
	gtk_table_attach(GTK_TABLE(tb), lbl1, 0, 1, 1, 2,  //...,0,1,0,1 ... -> left, right, top, bottom
		(GtkAttachOptions)(GTK_SHRINK | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);

	//Create the Tag entry field
	tf1 = gtk_entry_new();
	gtk_tooltips_set_tip(tt, tf1, _("Tag name"), NULL);
	gtk_entry_set_width_chars(GTK_ENTRY(tf1), ISAC_TF_LEN);
	gtk_table_attach(GTK_TABLE(tb), tf1, 1, 4, 1, 2,
		(GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
		(GtkAttachOptions)0, 0, 0);
	gtk_object_set_data(GTK_OBJECT(spw), "isac_zoom_tag", tf1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(lbl1), tf1);
	g_signal_connect(G_OBJECT(tf1), "focus-out-event",
		G_CALLBACK(isac_onzoomfocusout), spw);
	gtk_widget_set_size_request(tf1, 300, 23);

	// naim@ecava.com : find button
	GtkWidget *find_button = gtk_button_new_with_label("...");
	gtk_misc_set_alignment(GTK_MISC(find_button), 1, 0.5);
	gtk_table_attach(GTK_TABLE(tb), find_button, 4, 5, 1, 2,
		(GtkAttachOptions)(GTK_FILL),
		(GtkAttachOptions)(GTK_SHRINK),
		0, 0);
	g_signal_connect(G_OBJECT(find_button), "clicked", G_CALLBACK(findtag_button_press_callback), tf1);
	gtk_widget_set_size_request(find_button, 20, 23);

	GtkWidget *int_label = gtk_label_new_with_mnemonic(_("Info :"));
	GtkWidget *int_expander = gtk_expander_new(NULL);
	gtk_expander_set_expanded(GTK_EXPANDER(int_expander), TRUE);
	gtk_expander_set_label_widget(GTK_EXPANDER(int_expander), int_label);
	gtk_box_pack_start(GTK_BOX(vb), int_expander, FALSE, FALSE, 0);

	GtkWidget *tf = gtk_text_view_new();
	gtk_widget_set_sensitive(GTK_WIDGET(tf), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	GtkTextBuffer *desc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	gtk_text_buffer_set_text(desc_buffer, "Allow end user to zoom into the small area outlined by the rectangular object when it is clicked. This graphic is best set to opacity of 0.1 which is almost transparent.", -1);
	gtk_container_add(GTK_CONTAINER(int_expander), tf);
	gtk_widget_show(tf);

	gchar *dsb = "1";
	gtk_object_set_data(GTK_OBJECT(spw), "isac_zoom_dsb", dsb);

	return (GtkWidget *)vb;
}
GtkWidget *isac_menu_about_init(GtkObject *spw)
{
	GtkWidget *vb, *tb, *tf, *textframe, *tbscroll, *tbspare, *scroll;
	GtkTextBuffer *buffer;

	scroll = gtk_scrolled_window_new(NULL, GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 1, 500, 1, 10, 10)));
	vb = gtk_vbox_new(FALSE, 6);
	tb = gtk_table_new(4, 7, FALSE); //rows,, cols, homogeneous

	tbscroll = gtk_table_new(6, 7, FALSE); //rows, cols, homogeneous
	tbspare = gtk_table_new(3, 7, FALSE); //rows, cols, homogeneous

	gtk_container_set_border_width(GTK_CONTAINER(tb), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tb), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tb), 4);
	gtk_container_set_border_width(GTK_CONTAINER(tbscroll), 4);
	gtk_table_set_row_spacings(GTK_TABLE(tbscroll), 4);
	gtk_table_set_col_spacings(GTK_TABLE(tbscroll), 4);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll), tbscroll);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vb), tb, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vb), scroll, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb), tbspare, FALSE, FALSE, 0);

	tf = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tf), GTK_WRAP_WORD);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tf));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(tf), FALSE);
	gtk_text_buffer_set_text(buffer, 
	    "Inkscape SCADAvis vector graphics display editor (April/2018)\n"
	    "Modified and custom built for:\n"
        "  * SCADAvis.io\n"
        "  * xPlainCloud.com\n"
        "  * OSHMI - Open Substation HMI\n\n"
		"This build is a derivative of the Inkscape Project work and of the Ecava SAGE work,\n"
		" it is not in any way associated with the Inkscape Project nor Ecava.\n"
		"We respect and intend to comply with the branding policy of the Inkscape Project\n"
		"  https://inkscape.org/en/about/branding/\n"
		"In our view the modifications introduced are not substantial and do not intend to create a competitive product.\n"
		"The modifications introduced are meant to allow the use of the sotware as a SCADA Synoptics Editor.\n"
		"We respect the GPL license of the upstream work by making the source code of modifications to the original Inkscape and SAGE code available at:\n"
		"  https://sourceforge.net/p/oshmiopensubstationhmi/mercurial/ci/default/tree/inkscape_sage_src/\n\n"
        "The original SAGE (SCADA Animation Graphic Editor) software and source code is available here:\n"
		"  https://sourceforge.net/projects/sage-scada/\n"
		"Licensed under the GNU GPLv3\n"
		"  https://www.gnu.org/licenses/gpl-3.0.en.html", 
		-1);
	gtk_table_attach_defaults(GTK_TABLE(tbscroll), tf, 0, 8, 0, 6);
	gtk_widget_show(tf);

	return (GtkWidget *)vb;
}


/*****************************
ISAC Features:
Triggered when an input field under respective tab menu
is move into focus; On focus in function.
*****************************/
static void isac_oncolorfocusin(GtkWidget *widget, GdkEventFocus *event, SPWidget *spw)
{
	color_new_list(NULL, NULL);

	return;
}

static void isac_onclonefocusin(GtkWidget *widget, GdkEventFocus *event, SPWidget *spw)
{
	clone_new_list(NULL, NULL);

	return;
}

/*****************************
ISAC Features:
Triggered when an input field under respective tab menu
is move out of focus; On focus out function.
*****************************/
static gboolean isac_onbarfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_bar(spw);
}
static gboolean isac_onclonefocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_clone(spw);
}
static gboolean isac_oncolorfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_color(spw);
}
static gboolean isac_onfunctionfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_function(spw);
}
static gboolean isac_ongetfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_get(spw);
}
static gboolean isac_onopacityfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_opacity(spw);
}
static gboolean isac_onopenfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_open(spw);
}
static gboolean isac_onpopupfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_popup(spw);
}
static gboolean isac_onrotatefocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_rotate(spw);
}
static gboolean isac_onscriptfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_script(spw);
}
static gboolean isac_onsetfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_set(spw);
}
static gboolean isac_onsliderfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_slider(spw);
}
static gboolean isac_ontextfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_text(spw);
}
static gboolean isac_ontooltipsfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_tooltips(spw);
}
static gboolean isac_onzoomfocusout(GtkWidget *widget, GdkEventKey *event, SPWidget *spw)
{
	return isac_validate_zoom(spw);
}


/*****************************
ISAC Features:
Validation/checking on input fields of respective tab menu.
All invalid field value will be shown in red.
Checking includes:
1) Numeric values
2) Max > Min
3) Mandatory fields
*****************************/
static gboolean isac_validate_bar(SPWidget *spw)
{
	GtkWidget *wg_tag, *wg_min, *wg_max;
	gint dsb = 0;
	gchar const *tag, *min, *max;
	gchar *_dsb = "1";

	wg_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg_tag));

	wg_min = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_min"));
	min = gtk_entry_get_text(GTK_ENTRY(wg_min));

	wg_max = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_max"));
	max = gtk_entry_get_text(GTK_ENTRY(wg_max));

	if (strlen(min) == 0)
		dsb = 1;  //Mandatory field check//
	else
	{
		if (!is_num(min))
		{ //Number field check//
			isac_set_text_color(wg_min, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_min, FALSE);
	}

	if (strlen(max) == 0)
		dsb = 1;  //Mandatory field check//
	else
	{
		if (!is_num(max))
		{ //Number field check//
			isac_set_text_color(wg_max, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_max, FALSE);
	}

	if (strlen(tag) == 0)
		dsb = 2;  //Mandatory field check//

	if (dsb == 1)
	{
		_dsb = "1";
		isac_set_tab_active(bar_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_bar_dsb", _dsb);
	}
	else if (dsb == 0)
	{
		_dsb = "0";
		isac_set_tab_active(bar_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_bar_dsb", _dsb);
	}
	else
	{
		_dsb = "2";
		isac_set_tab_active(bar_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_bar_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_clone(SPWidget *spw)
{
	gint dsb = 0, valid = 0;
	gchar const *mapfrom = "", *mapto = "";
	gchar *_dsb = "1";

	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
	path = gtk_tree_path_new_first();

	if (!gtk_tree_model_get_iter(model, &iter, path)) // if there is no data,
	{// no data, Tab_Ready
		dsb = 2;
	}

	while (gtk_tree_model_get_iter(model, &iter, path))
	{
		gtk_tree_model_get(model, &iter, CLONE_VAR_TEXT, &mapfrom, CLONE_VAL_TEXT, &mapto, -1);
		
		if ((strlen(mapfrom) == 0 && strlen(mapto) != 0) || (strlen(mapfrom) != 0 && strlen(mapto) == 0))
		{
			dsb = 1;
			valid++;
		}
		if (strlen(mapfrom) != 0 && strlen(mapto) != 0)
		{
			valid++;
		}
		
		gtk_tree_path_next(path);
	}

	/*for (guint i = 0; i<g_slist_length(clone_mapfrom_list); i++)
	{
		mapfrom = g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(clone_mapfrom_list, i)))));
		mapto = g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(clone_mapto_list, i)))));

		if ((strlen(mapfrom) == 0 && strlen(mapto) != 0) || (strlen(mapfrom) != 0 && strlen(mapto) == 0))
		{
			dsb = 1;
			valid++;
		}
		if (strlen(mapfrom) != 0 && strlen(mapto) != 0)
		{
			valid++;
		}
	}*/

	if (valid == 0)
	{
		dsb = 2;
	}

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(clone_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_clone_dsb", _dsb);
	}
	else if (dsb == 0) {
		_dsb = "0";
		isac_set_tab_active(clone_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_clone_dsb", _dsb);
	}
	else {
		_dsb = "2";
		isac_set_tab_active(clone_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_clone_dsb", _dsb);
	}
	
	//Validate Open tab for Non GROUP objects
	if (nbtype != OBJECT_IS_GROUP)
	{
		isac_validate_open(spw);
	}

	return FALSE;
}
static gboolean isac_validate_color(SPWidget *spw)
{
	GtkWidget *color_new, *color_pick;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;
	char *tag, *value, *code;

	gint dsb = 2; // current: ready by default
	gint dsb2 = 2; // previous: ready by default
	gint dsb3 = 2;
	gchar *_dsb = "1";

	color_pick = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_color_picker_bttn"));
	GtkWidget *color_find_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_color_find_tag"));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
	path = gtk_tree_path_new_first();

	if (!gtk_tree_model_get_iter(model, &iter, path)) // if there is no data,
	{
		gtk_widget_set_sensitive(color_pick, FALSE); // set the "Color Picker" buton to insensitive
		gtk_widget_set_sensitive(color_find_tag, FALSE);
		// no data, Tab_Ready
		dsb = 2;
		dsb2 = 2;
	}

	while (gtk_tree_model_get_iter(model, &iter, path))
	{
		gtk_tree_model_get(model, &iter, 0, &tag, 2, &value, 4, &code, -1);
		if (strlen(tag) == 0 && strlen(value) == 0 && strlen(code) == 0) // all cell empty
		{
			if (dsb2 == 1)
				dsb = 1;
			else
			{
				dsb = 2; // tab is ready
			}
		}
		else if (strlen(tag) == 0 && (strlen(value) != 0 || strlen(code) != 0))
			// Tag empty, Value has value, Code has value
		{
			dsb = 1; // tab error
			dsb2 = 1;
		}
		else if (strlen(tag) != 0 && (strlen(value) == 0 || strlen(code) == 0))
			// Tag empty, Value has value, Code has value
		{
			dsb = 1; // tab error
			dsb2 = 1;
		}
		else if (strlen(tag) != 0 && strlen(value) != 0 && strlen(code) != 0)
			// all entries has value
		{
			if (dsb != 1 && dsb2 != 1)
				dsb = 0; // tab is completed
			else
				dsb = 2; // tab ready
			dsb3 = 0;
		}

		gtk_tree_path_next(path);
	}

	if (dsb == 1) // Tab_Error
	{
		isac_set_tab_active(color_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_color_dsb", _dsb);
	}
	else if (dsb == 0) // by default: Tab_Completed
	{
		_dsb = "0";
		if (dsb2 == 1) // complete by previous data has error, so still error
		{
			_dsb = "1";
			isac_set_tab_active(color_lbl, Tab_Error);
			gtk_object_set_data(GTK_OBJECT(spw), "isac_color_dsb", _dsb);
		}
		else
		{
			isac_set_tab_active(color_lbl, Tab_Completed);
			gtk_object_set_data(GTK_OBJECT(spw), "isac_color_dsb", _dsb);
		}
	}
	else if (dsb == 2) // Tab_Ready
	{
		_dsb = "2";
		if (dsb2 == 1) // Tab_Ready but previous data has error, so still error
		{
			_dsb = "1";
			isac_set_tab_active(color_lbl, Tab_Error);
			gtk_object_set_data(GTK_OBJECT(spw), "isac_color_dsb", _dsb);
		}
		else if (dsb3 == 0) // Tab_Ready but previous data Tab_Completed, so Tab_Completed
		{
			_dsb = "0";
			isac_set_tab_active(color_lbl, Tab_Completed);
			gtk_object_set_data(GTK_OBJECT(spw), "isac_color_dsb", _dsb);
		}
		else // Tab_Ready
		{
			isac_set_tab_active(color_lbl, Tab_Ready);
			gtk_object_set_data(GTK_OBJECT(spw), "isac_color_dsb", _dsb);
		}

		// originally: isac_set_tab_active (color_lbl, Tab_Ready);
		// without the if-else above
		gtk_object_set_data(GTK_OBJECT(spw), "isac_color_dsb", _dsb);
	}
	gtk_tree_path_free(path);
	return FALSE;
}
static gboolean isac_validate_function(SPWidget *spw)
{
	GtkWidget *wg_name;
	gint dsb = 0;
	gchar const *name;
	gchar *_dsb = "1";

	wg_name = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_func_name"));
	name = gtk_entry_get_text(GTK_ENTRY(wg_name));

	if (strlen(name) == 0)
		dsb = 1;  //Mandatory field check//
	else {
		/*
		if(!is_alphanum(tag)){ //Alpha Numeric field check//
		isac_set_text_color(wg_tag,TRUE);
		dsb = 1;
		}
		else
		isac_set_text_color(wg_tag,FALSE);
		*/
	}

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(function_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_func_dsb", _dsb);
	}
	else {
		_dsb = "0";
		isac_set_tab_active(function_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_func_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_get(SPWidget *spw)
{
	GtkWidget *wg_tag;
	gint dsb = 0;
	gchar const *tag;
	gchar *_dsb = "1";

	wg_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_get_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg_tag));


	if (strlen(tag) == 0) dsb = 2;  //Mandatory field check//

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(get_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_get_dsb", _dsb);
	}
	else if (dsb == 0) {
		_dsb = "0";
		isac_set_tab_active(get_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_get_dsb", _dsb);
	}
	else {
		_dsb = "2";
		isac_set_tab_active(get_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_get_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_opacity(SPWidget *spw)
{
	GtkWidget *wg_tag, *wg_min, *wg_max;
	gint dsb = 0;
	gchar const *tag, *min, *max;
	gchar *_dsb = "1";

	wg_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg_tag));

	wg_min = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_min"));
	min = gtk_entry_get_text(GTK_ENTRY(wg_min));

	wg_max = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_max"));
	max = gtk_entry_get_text(GTK_ENTRY(wg_max));

	if (strlen(min) == 0)
		dsb = 1;  //Mandatory field check//
	else {

		if (!is_num(min)){ //Number field check//
			isac_set_text_color(wg_min, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_min, FALSE);

	}

	if (strlen(max) == 0)
		dsb = 1;  //Mandatory field check//
	else {

		if (!is_num(max)){ //Number field check//
			isac_set_text_color(wg_max, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_max, FALSE);

	}

	if (strlen(tag) == 0) dsb = 2;  //Mandatory field check//

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(opacity_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_opac_dsb", _dsb);
	}
	else if (dsb == 0){
		_dsb = "0";
		isac_set_tab_active(opacity_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_opac_dsb", _dsb);
	}

	else {
		_dsb = "2";
		isac_set_tab_active(opacity_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_opac_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_open(SPWidget *spw)
{
	GtkWidget *wg_source, *wg_xpos, *wg_ypos, *wg_width, *wg_height, *wg_type;
	gint dsb = 0;
	gchar const *source, *xpos, *ypos, *width, *height;
	gint type = -1;
	gchar *_dsb = "1";

	wg_source = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_source"));
	source = gtk_entry_get_text(GTK_ENTRY(wg_source));

	wg_xpos = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_xpos"));
	xpos = gtk_entry_get_text(GTK_ENTRY(wg_xpos));

	wg_ypos = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_ypos"));
	ypos = gtk_entry_get_text(GTK_ENTRY(wg_ypos));

	wg_width = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_width"));
	width = gtk_entry_get_text(GTK_ENTRY(wg_width));

	wg_height = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_height"));
	height = gtk_entry_get_text(GTK_ENTRY(wg_height));

	wg_type = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_type"));
	type = gtk_combo_box_get_active(GTK_COMBO_BOX(wg_type));

	if (type != 0)
	{
		if (strlen(xpos) == 0) dsb = 1;
		else {

			if (!is_num(xpos)){ //Number field check//
				isac_set_text_color(wg_xpos, TRUE);
				dsb = 1;
			}
			else {
				isac_set_text_color(wg_xpos, FALSE);
			}
		}
	
		if (strlen(ypos) == 0) dsb = 1;
		else {

			if (!is_num(ypos)){ //Number field check//
				isac_set_text_color(wg_ypos, TRUE);
				dsb = 1;
			}
			else
				isac_set_text_color(wg_ypos, FALSE);

		}
	
		if (strlen(width) == 0) dsb = 1;
		else {

			if (!is_num(width)){ //Number field check//
				isac_set_text_color(wg_width, TRUE);
				dsb = 1;
			}
			else
				isac_set_text_color(wg_width, FALSE);

		}
	
		if (strlen(height) == 0) dsb = 1;
		else {

			if (!is_num(height)){ //Number field check//
				isac_set_text_color(wg_height, TRUE);
				dsb = 1;
			}
			else
				isac_set_text_color(wg_height, FALSE);

		}
	}

	//validation for Non group objects (nbtype is a gint data type that should have been set when opening the Object Properties dialog (Notebook Type)
	if (nbtype != OBJECT_IS_GROUP)
	{
		//Get Clone(Faceplate) tab's current condition
		gchar *clone_dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_clone_dsb"));
		
		//When Clone(Faceplate) tab is set and has no errors
		if (strcmp(clone_dsb, "0") == 0) 
		{		
			if(strlen(source) == 0 || type == 0) dsb = 1;			
		}
		else if (strlen(source) == 0) dsb = 2;  //Mandatory field check
	}
	else if (strlen(source) == 0) dsb = 2;  //Mandatory field check for GROUP objects//

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(open_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_open_dsb", _dsb);
	}
	else if (dsb == 0){
		_dsb = "0";
		isac_set_tab_active(open_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_open_dsb", _dsb);
	}

	else {
		_dsb = "2";
		isac_set_tab_active(open_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_open_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_popup(SPWidget *spw)
{
	GtkWidget *wg_source, *wg_xpos, *wg_ypos, *wg_width, *wg_height, *wg_type;
	gint dsb = 0;
	gchar const *source, *xpos, *ypos, *width, *height;
	gint type = -1;
	gchar *_dsb = "1";

	wg_source = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_source"));
	source = gtk_entry_get_text(GTK_ENTRY(wg_source));

	wg_xpos = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_xpos"));
	xpos = gtk_entry_get_text(GTK_ENTRY(wg_xpos));

	wg_ypos = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_ypos"));
	ypos = gtk_entry_get_text(GTK_ENTRY(wg_ypos));

	wg_width = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_width"));
	width = gtk_entry_get_text(GTK_ENTRY(wg_width));

	wg_height = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_height"));
	height = gtk_entry_get_text(GTK_ENTRY(wg_height));


	if (strlen(xpos) == 0) dsb = 1;
	else {

		if (!is_num(xpos)){ //Number field check//
			isac_set_text_color(wg_xpos, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_xpos, FALSE);

	}

	if (strlen(ypos) == 0) dsb = 1;
	else {

		if (!is_num(ypos)){ //Number field check//
			isac_set_text_color(wg_ypos, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_ypos, FALSE);

	}

	if (strlen(width) == 0) dsb = 1;
	else {

		if (!is_num(width)){ //Number field check//
			isac_set_text_color(wg_width, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_width, FALSE);

	}

	if (strlen(height) == 0) dsb = 1;
	else {

		if (!is_num(height)){ //Number field check//
			isac_set_text_color(wg_height, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_height, FALSE);

	}

	if (strlen(source) == 0) dsb = 2;  //Mandatory field check//

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(popup_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_popup_dsb", _dsb);
	}
	else if (dsb == 0){
		_dsb = "0";
		isac_set_tab_active(popup_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_popup_dsb", _dsb);
	}

	else {
		_dsb = "2";
		isac_set_tab_active(popup_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_popup_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_rotate(SPWidget *spw)
{
	GtkWidget *wg_tag, *wg_min, *wg_max;
	gint dsb = 0;
	gchar const *tag, *min, *max;
	gchar *_dsb = "1";

	wg_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg_tag));

	wg_min = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_min"));
	min = gtk_entry_get_text(GTK_ENTRY(wg_min));

	wg_max = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_max"));
	max = gtk_entry_get_text(GTK_ENTRY(wg_max));

	if (strlen(min) == 0) dsb = 1;
	else {

		if (!is_num(min)){ //Number field check//
			isac_set_text_color(wg_min, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_min, FALSE);

	}

	if (strlen(max) == 0) dsb = 1;
	else {

		if (!is_num(max)){ //Number field check//
			isac_set_text_color(wg_max, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_max, FALSE);
	}

	if (strlen(tag) == 0) dsb = 2;  //Mandatory field check//

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(rotate_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_rot_dsb", _dsb);
	}
	else if (dsb == 0){
		_dsb = "0";
		isac_set_tab_active(rotate_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_rot_dsb", _dsb);
	}
	else {
		_dsb = "2";
		isac_set_tab_active(rotate_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_rot_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_script(SPWidget *spw)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;
	char *scr, *evt, *key;
	gboolean bCtrl, bAlt, bShift;
	guint underline;
	
	gint dsb = 1;
	gchar *_dsb = "1";
	gboolean error = FALSE;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(script_list));
	path = gtk_tree_path_new_first();

	GtkWidget* selection = GTK_WIDGET(gtk_tree_view_get_selection(GTK_TREE_VIEW(script_list)));
	script_onchange(selection, spw);

	while (gtk_tree_model_get_iter(model, &iter, path)){

		gtk_tree_model_get(model, &iter, 0, &evt, 1, &underline, 2, &scr,
			3, bCtrl, 4, bAlt, 5, bShift, 6, &key,
			-1);
				
		if (strlen(scr) != 0)
		{
			dsb = 0;
		}	

		if (!error && underline == PANGO_UNDERLINE_ERROR) {
			error = TRUE;
		}

		gtk_tree_path_next(path);
	}
	
	if (dsb == 1){
		_dsb = "2";
		isac_set_tab_active(script_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_script_dsb", _dsb);
	} else {
		_dsb = "0";
		isac_set_tab_active(script_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_script_dsb", _dsb);
	}

	if (error) {	//HSH - makes the script tab turn red if script is set without a key
		isac_set_tab_active(script_lbl, Tab_Error);
	}

	gtk_tree_path_free(path);
	return FALSE;
}
static gboolean isac_validate_set(SPWidget *spw)
{
	GtkWidget *wg_tag;
	gint dsb = 0;
	gchar const *tag;
	gchar *_dsb = "1";

	wg_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg_tag));


	if (strlen(tag) == 0) dsb = 2;  //Mandatory field check//

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(set_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_set_dsb", _dsb);
	}
	else if (dsb == 0){
		_dsb = "0";
		isac_set_tab_active(set_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_set_dsb", _dsb);
	}
	else {
		_dsb = "2";
		isac_set_tab_active(set_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_set_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_slider(SPWidget *spw)
{
	GtkWidget *wg_tag, *wg_min, *wg_max, *wg_read;
	gint dsb = 0;
	gchar const *tag, *min, *max, *readonly;
	gchar *_dsb = "1";
	gboolean read;

	wg_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg_tag));

	wg_min = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_min"));
	min = gtk_entry_get_text(GTK_ENTRY(wg_min));

	wg_max = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_max"));
	max = gtk_entry_get_text(GTK_ENTRY(wg_max));

	wg_read = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_read"));
	read = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wg_read));


	if (strlen(min) == 0)
		dsb = 1;  //Mandatory field check//
	else {
		if (!is_num(min)){ //Number field check//
			isac_set_text_color(wg_min, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_min, FALSE);

	}

	if (strlen(max) == 0)
		dsb = 1;  //Mandatory field check//
	else {
		if (!is_num(max)){ //Number field check//
			isac_set_text_color(wg_max, TRUE);
			dsb = 1;
		}
		else
			isac_set_text_color(wg_max, FALSE);
	}

	if (strlen(tag) == 0) dsb = 2;  //Mandatory field check//

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(slider_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_slider_dsb", _dsb);
	}
	else if (dsb == 0){
		_dsb = "0";
		isac_set_tab_active(slider_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_slider_dsb", _dsb);
	}
	else {
		_dsb = "2";
		isac_set_tab_active(slider_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_slider_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_text(SPWidget *spw)
{
	GtkWidget *wg_tag;
	gint dsb = 0, valid = 0;
	gchar const *tagval = "", *tagtext = "", *tag;
	gchar *_dsb = "1";

	wg_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_text_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg_tag));


	for (guint i = 0; i<g_slist_length(text_value_list); i++)
	{
		tagval = g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(text_value_list, i)))));
		tagtext = g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(text_display_list, i)))));

		if (strlen(g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(text_value_list, 0)))))) == 0)
		{
			dsb = 1;// No Mandatory field check
			valid++;
		}
		else {
			/*
			if(!is_num(tagval)){ //Number field check//
			isac_set_text_color(GTK_WIDGET (g_slist_nth_data(text_value_list,i)),TRUE);
			dsb = 1;
			}
			else
			isac_set_text_color(GTK_WIDGET (g_slist_nth_data(text_value_list,i)),FALSE);
			*/
		}

		if ((strlen(tagval) == 0 && strlen(tagtext) != 0) || (strlen(tagval) != 0 && strlen(tagtext) == 0))
		{
			dsb = 1;
			valid++;
		}
		if (strlen(tagval) != 0 && strlen(tagtext) != 0)
		{
			valid++;
		}
	}

	if (valid == 0)
	{
		dsb = 2;
	}

	if (strlen(tag) == 0) dsb = 2;  //Mandatory field check//

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(text_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_text_dsb", _dsb);
	}
	else if (dsb == 0){
		_dsb = "0";
		isac_set_tab_active(text_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_text_dsb", _dsb);
	}
	else {
		_dsb = "2";
		isac_set_tab_active(text_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_text_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_tooltips(SPWidget *spw)
{
	GtkWidget *wg_param, *wg_size, *wg_style;
	gint dsb = 0, valid = 0;
	gchar const *param, *size, *style, *tooltipsval = "", *tooltipsstring = "";
	gchar *_dsb = "1";

	wg_size = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_tooltips_size"));
	size = gtk_entry_get_text(GTK_ENTRY(wg_size));

	wg_style = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_tooltips_style"));
	style = gtk_entry_get_text(GTK_ENTRY(wg_style));

	for (guint i = 0; i<g_slist_length(tt_value_list); i++)
	{
		tooltipsval = g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(tt_value_list, i)))));

		if (strlen(tooltipsval) != 0)
		{
			valid++;
		}
	}

	if (!is_num(size)){ //Number field check//
		isac_set_text_color(wg_size, TRUE);
		dsb = 1;
	}
	else isac_set_text_color(wg_size, FALSE);

	if (valid == 0) dsb = 2;

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(tooltips_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_tooltips_dsb", _dsb);
	}
	else if (dsb == 0){
		_dsb = "0";
		isac_set_tab_active(tooltips_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_tooltips_dsb", _dsb);
	}
	else {
		_dsb = "2";
		isac_set_tab_active(tooltips_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_tooltips_dsb", _dsb);
	}

	return FALSE;
}
static gboolean isac_validate_zoom(SPWidget *spw)
{
	GtkWidget *wg_tag, *wg_align;
	gint dsb = 0;
	gchar const *tag, *align;
	gchar *_dsb = "1";

	wg_tag = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_zoom_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg_tag));

	wg_align = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_zoom_align"));
	align = gtk_combo_box_get_active_text(GTK_COMBO_BOX(wg_align));

	if (strcmp(align, "None") == 0) dsb = 2;

	if (dsb == 1){
		_dsb = "1";
		isac_set_tab_active(zoom_lbl, Tab_Error);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_zoom_dsb", _dsb);
	}
	else if (dsb == 0){
		_dsb = "0";
		isac_set_tab_active(zoom_lbl, Tab_Completed);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_zoom_dsb", _dsb);
	}
	else {
		_dsb = "2";
		isac_set_tab_active(zoom_lbl, Tab_Ready);
		gtk_object_set_data(GTK_OBJECT(spw), "isac_zoom_dsb", _dsb);
	}

	return FALSE;
}


//------------------------------------------------
// ISAC Features:
// Parse and format fields values into recognizable ISAC syntax.
//------------------------------------------------
gchar* isac_get_bar_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	std::string svalue;
	GtkWidget *wg;
	gchar const *tag, *min, *max, *jsyntax;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_min"));
	min = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_max"));
	max = gtk_entry_get_text(GTK_ENTRY(wg));

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_dsb"));

	if (strlen(g_strdup_printf("%s", tag)) == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	if (strcmp(dsb, "0") == 0) o["attr"] = "bar";
	else o["@ttr"] = "bar";

	o["tag"] = tag;
	o["min"] = atoi(min);
	o["max"] = atoi(max);
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_clone_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	JSONARRAY ja;
	std::string svalue;

	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;

	gchar const *mapstring = "", *mapfrom = "", *mapto = "", *jsyntax;
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
	path = gtk_tree_path_new_first();

	if (!gtk_tree_model_get_iter(model, &iter, path)) // if there is no data
		return syntax;
	
	while (gtk_tree_model_get_iter(model, &iter, path)) {
		gtk_tree_model_get(model, &iter, CLONE_VAR_TEXT, &mapfrom, CLONE_VAL_TEXT, &mapto, -1);
		
		if (strlen(mapfrom) != 0 || strlen(mapto) != 0)
		{
			mapstring = g_strconcat("%", mapfrom, "=", mapto, NULL);
			ja.push_back(mapstring);
		}

		gtk_tree_path_next(path);
	}
	//HSH, 26/03/2015, Replaced with clone list method of getting data
	/*for (guint i = 0; i<g_slist_length(clone_mapfrom_list); i++)
	{
		mapfrom = g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(clone_mapfrom_list, i)))));
		mapto = g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(clone_mapto_list, i)))));

		if (strlen(mapfrom) != 0 || strlen(mapto) != 0)
		{
			mapstring = g_strconcat("%", mapfrom, "=", mapto, NULL);
			ja.push_back(mapstring);
		}
	}*/

	if (strlen(mapstring) == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_clone_dsb"));

	if (strcmp(dsb, "0") == 0) o["attr"] = "clone";
	else o["@ttr"] = "clone";

	o["map"] = JSONVALUE(ja);
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_color_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o, oarray;
	JSONARRAY ja;
	std::string svalue;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;
	char *tag, *value, *code;
	gchar const *jsyntax;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
	path = gtk_tree_path_new_first();

	if (!gtk_tree_model_get_iter(model, &iter, path)) // if there is no data
		return syntax;

	while (gtk_tree_model_get_iter(model, &iter, path)) {
		gtk_tree_model_get(model, &iter, 0, &tag, 2, &value, 4, &code, -1);
		if (strlen(tag) != 0 && strlen(value) != 0 && strlen(code) != 0) {
			oarray["tag"] = tag;
			oarray["data"] = value;
			oarray["param"] = code;
			ja.push_back(oarray);
		}
		else {
			oarray["tag"] = tag;
			oarray["data"] = value;
			oarray["param"] = code;
			ja.push_back(oarray);
		}
		gtk_tree_path_next(path);
	}

	if (syntax != "")
		syntax = g_strconcat(syntax, ",", NULL);

	isac_validate_color(spw);
	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_color_dsb"));

	if (strcmp(dsb, "0") == 0)
		o["attr"] = "color";
	else
		o["@ttr"] = "color";

	o["list"] = JSONVALUE(ja);
	JSONVALUE(o).ToString(svalue, 0);

	jsyntax = svalue.c_str();

	gtk_tree_path_free(path);
	return g_strconcat(syntax, jsyntax, NULL);
}
gchar* isac_get_function_syntax(gchar *syntax, SPWidget *spw)
{
	GtkWidget *wg;
	gchar const *name, *effect, *param;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_func_name"));
	name = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_func_param"));
	param = gtk_entry_get_text(GTK_ENTRY(wg));

	gchar *formparam;// = reformat_backspc(param);

	if (strlen(g_strdup_printf("%s%s", name, param)) == 0) /* && strcmp(effect,"Shadow")==0)  */ return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_func_dsb"));

	if (strcmp(dsb, "0") == 0)
		//return g_strconcat(syntax, "{attr:\"function\",name:\"",g_ascii_strdown(name,strlen(name)),"\",param:[\"",g_ascii_strdown(formparam,strlen(formparam)),"\"]",",effect:\"", g_ascii_strdown(effect,strlen(effect)),"\"}",NULL);
		return g_strconcat(syntax, "{attr:\"function\",name:\"", g_strdup_printf("%s", name), "\",param:\"", g_ascii_strdown(formparam, strlen(formparam)), "\"}", NULL);
	else
		//return g_strconcat(syntax, "{@ttr:\"function\",name:\"",g_ascii_strdown(name,strlen(name)),"\",param:[\"",g_ascii_strdown(formparam,strlen(formparam)),"\"]",",effect:\"", g_ascii_strdown(effect,strlen(effect)),"\"}",NULL);
		return g_strconcat(syntax, "{@ttr:\"function\",name:\"", g_strdup_printf("%s", name), "\",param:\"", g_ascii_strdown(formparam, strlen(formparam)), "\"}", NULL);
}
gchar* isac_get_get_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	std::string svalue;
	GtkWidget *wg;

	gchar const *tag, *align, *type, *jsyntax; //*format

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_get_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_get_align"));
	align = gtk_combo_box_get_active_text(GTK_COMBO_BOX(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_get_type"));
	type = gtk_combo_box_get_active_text(GTK_COMBO_BOX(wg));

	if (strlen(tag) == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_get_dsb"));

	if (strcmp(dsb, "0") == 0) o["attr"] = "get";
	else o["@ttr"] = "get";

	o["tag"] = tag;
	o["align"] = align;
	o["type"] = type;
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_opacity_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	std::string svalue;
	GtkWidget *wg;
	gchar const *tag, *min, *max, *jsyntax;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_min"));
	min = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_max"));
	max = gtk_entry_get_text(GTK_ENTRY(wg));

	if (strlen(tag) == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_dsb"));

	if (strcmp(dsb, "0") == 0) o["attr"] = "opac";
	else o["@ttr"] = "opac";

	o["tag"] = tag;
	o["min"] = atoi(min);
	o["max"] = atoi(max);
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_open_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	std::string svalue;
	GtkWidget *wg;
	gchar const *src, *type, *x, *y, *width, *height, *istag, *effect, *jsyntax;
	gint srctype = 0;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_source"));
	src = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_srctype"));
	srctype = gtk_combo_box_get_active(GTK_COMBO_BOX(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_type"));
	type = gtk_combo_box_get_active_text(GTK_COMBO_BOX(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_xpos"));
	x = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_ypos"));
	y = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_width"));
	width = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_height"));
	height = gtk_entry_get_text(GTK_ENTRY(wg));

	if (strlen(src) == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_dsb"));

	if (strcmp(dsb, "0") == 0) o["attr"] = "open";
	else o["@ttr"] = "open";

	if (strcmp(type, "Current window") == 0) o["type"] = "_self";
	else if (strcmp(type, "New exclusive window") == 0) o["type"] = "_blank";	//HSH - renamed from "New window"
	else if (strcmp(type, "New shared window") == 0) o["type"] = "_shared";	//HSH - added
	else o["type"] = type;

	o["src"] = src;
	o["istag"] = srctype;
	o["x"] = atoi(x);
	o["y"] = atoi(y);
	o["width"] = atoi(width);
	o["height"] = atoi(height);
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_popup_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	std::string svalue;
	GtkWidget *wg;
	gchar const *src, *x, *y, *width, *height, *jsyntax;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_source"));
	src = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_xpos"));
	x = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_ypos"));
	y = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_width"));
	width = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_height"));
	height = gtk_entry_get_text(GTK_ENTRY(wg));

	if (strlen(src) == 0) return syntax;
	if (strlen(x) == 0) x = "100";
	if (strlen(y) == 0) y = "100";
	if (strlen(width) == 0) width = "500";
	if (strlen(height) == 0) height = "400";

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_dsb"));

	if (strcmp(dsb, "0") == 0) o["attr"] = "popup";
	else o["@ttr"] = "popup";

	o["src"] = src;
	o["x"] = atoi(x);
	o["y"] = atoi(y);
	o["width"] = atoi(width);
	o["height"] = atoi(height);
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_rotate_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	std::string svalue;
	GtkWidget *wg;
	gchar const *tag, *min, *max, *jsyntax;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_min"));
	min = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_max"));
	max = gtk_entry_get_text(GTK_ENTRY(wg));

	if (strlen(tag) == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_dsb"));

	if (strcmp(dsb, "0") == 0) o["attr"] = "rotate";
	else o["@ttr"] = "rotate";

	o["tag"] = tag;
	o["min"] = atoi(min);
	o["max"] = atoi(max);
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_script_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o, oarray;
	JSONARRAY ja;
	std::string svalue;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;
	char *evt, *scr, *key;
	gchar const *jsyntax;
	gboolean bCtrl, bAlt, bShift;

	gint available = 0;
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(script_list));
	path = gtk_tree_path_new_first();
	while (gtk_tree_model_get_iter(model, &iter, path)) {
		gtk_tree_model_get(model, &iter, 0, &evt, 2, &scr, 3, &bCtrl, 4, &bAlt, 5, &bShift, 6, &key, -1);
		if (strlen(scr) != 0) {
			oarray["evt"] = evt;
			
			if (strcmp(evt, "keydown") == 0) {
				string szKeys = "";
				if (strlen(key) != 0) {	//only insert ctrl/alt/shift if key is not empty
					if (bCtrl) {
						szKeys.append("ctrl+");
					}
					if (bAlt) {
						szKeys.append("alt+");
					}
					if (bShift) {
						szKeys.append("shift+");
					}
				}
				szKeys.append(key);
				oarray["keys"] = szKeys.c_str();
			}

			oarray["param"] = scr;
			ja.push_back(oarray);
			available = 1;
		}
		gtk_tree_path_next(path);
	}
	if (!available)
		return syntax;

	if (syntax != "")
		syntax = g_strconcat(syntax, ",", NULL);

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_dsb"));
	if (strcmp(dsb, "0") == 0)
		o["attr"] = "script";
	else
		o["@ttr"] = "script";
	o["list"] = JSONVALUE(ja);
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	gtk_tree_path_free(path);
	return g_strconcat(syntax, jsyntax, NULL);
}
gchar* isac_get_set_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	std::string svalue;
	GtkWidget *wg;
	gchar const *tag, *align, *source, *type, *effect, *prompt, *jsyntax;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_align"));
	align = gtk_combo_box_get_active_text(GTK_COMBO_BOX(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_source"));
	source = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_type"));
	type = gtk_combo_box_get_active_text(GTK_COMBO_BOX(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_prompt"));
	prompt = gtk_entry_get_text(GTK_ENTRY(wg));

	if (strlen(g_strdup_printf("%s", tag)) == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_dsb"));

	if (strcmp(dsb, "0") == 0) o["attr"] = "set";
	else o["@ttr"] = "set";

	o["tag"] = tag;
	o["align"] = align;
	o["src"] = source;
	o["type"] = type;
	o["prompt"] = prompt;
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_slider_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	std::string svalue;
	GtkWidget *wg;

	gchar const *tag, *min, *max, *readonly, *jsyntax;
	gboolean read;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_min"));
	min = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_max"));
	max = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_read"));
	read = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wg));

	if (read == TRUE) {
		readonly = "1";
	}

	else {
		readonly = "0";
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_dsb"));

	if (strlen(g_strdup_printf("%s", tag)) == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	if (strcmp(dsb, "0") == 0) o["attr"] = "slider";
	else o["@ttr"] = "slider";

	o["tag"] = tag;
	o["min"] = atoi(min);
	o["max"] = atoi(max);
	o["readonly"] = atoi(readonly);
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_text_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	JSONARRAY ja;
	std::string svalue;
	GtkWidget *wg;

	gchar const *tag, *textstring = "", *textvalue = "", *textdisplay = "", *jsyntax;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_text_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg));

	for (guint i = 0; i<g_slist_length(text_value_list); i++)
	{
		textvalue = g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(text_value_list, i)))));
		textdisplay = g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(text_display_list, i)))));

		if (strlen(textvalue) != 0 || strlen(textdisplay) != 0)
		{
			textstring = g_strconcat(textvalue, "=", textdisplay, NULL);
			ja.push_back(textstring);
		}
	}

	if (strlen(g_strdup_printf("%s", tag)) == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_text_dsb"));

	if (strcmp(dsb, "0") == 0) o["attr"] = "text";
	else o["@ttr"] = "text";

	o["tag"] = tag;
	o["map"] = JSONVALUE(ja);
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_tooltips_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	JSONARRAY ja;
	std::string svalue;
	GtkWidget *wg;

	gchar const *tooltipsvalue = "", *tooltipsstring = "", *size, *style, *jsyntax;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_tooltips_size"));
	size = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_tooltips_style"));
	style = gtk_entry_get_text(GTK_ENTRY(wg));

	for (guint i = 0; i<g_slist_length(tt_value_list); i++)
	{
		tooltipsvalue = g_strstrip(g_strdup(gtk_entry_get_text(GTK_ENTRY(g_slist_nth_data(tt_value_list, i)))));
		if (strlen(tooltipsvalue) != 0){

			tooltipsstring = g_strconcat(tooltipsvalue, NULL);
			ja.push_back(tooltipsstring);
		}
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_tooltips_dsb"));

	if (strlen(g_strdup_printf("%s", tooltipsstring)) == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	if (strcmp(dsb, "0") == 0) o["attr"] = "tooltips";
	else o["@ttr"] = "tooltips";

	if (strcmp(size, "") == 0) o["size"] = 12;
	else o["size"] = atoi(size);

	o["style"] = style;
	o["param"] = JSONVALUE(ja);
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}
gchar* isac_get_zoom_syntax(gchar *syntax, SPWidget *spw)
{
	JSONOBJECT o;
	std::string svalue;
	GtkWidget *wg;

	gchar const *tag, *align, *jsyntax;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_zoom_tag"));
	tag = gtk_entry_get_text(GTK_ENTRY(wg));

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_zoom_align"));
	align = gtk_combo_box_get_active_text(GTK_COMBO_BOX(wg));

	if (strcmp(align, "None") == 0) return syntax;

	if (syntax != "")
	{
		syntax = g_strconcat(syntax, ",", NULL);
	}

	gchar *dsb = (gchar*)(gtk_object_get_data(GTK_OBJECT(spw), "isac_zoom_dsb"));

	if (strcmp(dsb, "0") == 0) o["attr"] = "zoom";
	else o["@ttr"] = "zoom";

	o["tag"] = tag;
	o["align"] = align;
	JSONVALUE(o).ToString(svalue, 0);
	jsyntax = svalue.c_str();

	return g_strconcat(syntax, (gchar*)jsyntax, NULL);
}


//------------------------------------------------
// ISAC Features:
// Retrieve ISAC syntax value and parse it and assigned it back to each 
// corresponding tab's fields during start up.
//------------------------------------------------
static void isac_bar_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j, jget;
	const char *tag, *cmin, *cmax;
	gint imin, imax;
	string *jvalue;
	std::string svalue;
	GtkWidget *wg;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	jvalue = j["tag"].val.strVal;
	tag = jvalue->c_str();

	jget = j["min"];

	if (jget.jt == JT_STRING){
		jvalue = j["min"].val.strVal;
		cmin = jvalue->c_str();

		jvalue = j["max"].val.strVal;
		cmax = jvalue->c_str();

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_max"));
		gtk_entry_set_text(GTK_ENTRY(wg), cmax);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_min"));
		gtk_entry_set_text(GTK_ENTRY(wg), cmin);
	}

	else {
		imin = j["min"].val.numVal.llVal;
		imax = j["max"].val.numVal.llVal;

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_max"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", imax));

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_min"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", imin));
	}

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_bar_tag"));
	gtk_entry_set_text(GTK_ENTRY(wg), tag);

	isac_validate_bar(spw);
}
static void isac_clone_assign_syntax(char* token, SPWidget *spw){
		
	clone_deletelastrow();

	JSONVALUE j, getj;
	string *jvalue;
	std::string JAvalue, maplist;
	//GtkWidget *wg;

	GtkListStore *store;
	GtkTreeIter iter;

	const char *map;
	char **maptoken, *str = NULL;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	j["map"].ToString(maplist, 0);
	map = maplist.c_str();

	getj.Parse(map);
	size_t nCount = getj.Size();

	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list)));

	for (size_t i = 0; i<nCount; i++){

		j = getj.At(i);

		j.ToString(JAvalue, 0);
		str = (char*)JAvalue.c_str();

		str++;  //remove " in front 
		str++;  //remove % in front
		str[strlen(str) - 1] = '\0';  // remove " trailing behind
		maptoken = (gchar**)g_strsplit(str, "=", 0);

		//wg = GTK_WIDGET(g_slist_nth_data(clone_mapfrom_list, i));
		//gtk_entry_set_text(GTK_ENTRY(wg), (char*)maptoken[0]);
		

		//wg = GTK_WIDGET(g_slist_nth_data(clone_mapto_list, i));
		//gtk_entry_set_text(GTK_ENTRY(wg), (char*)maptoken[1]);
		/*gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, 
			CLONE_VAR_TEXT	, (char*)maptoken[0], CLONE_VAR_EDITABLE  , TRUE , 
			CLONE_EQUAL_TEXT, "="				, CLONE_EQUAL_EDITABLE, FALSE, 
			CLONE_VAL_TEXT	, (char*)maptoken[1], CLONE_VAL_EDITABLE  , TRUE , -1);*/
		clone_list_add_item((gchar*)maptoken[0], (gchar*)maptoken[1], &iter);
	}
	
	//add an empty list item at the bottom of the list
	/*gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 
			CLONE_VAR_TEXT	, "" , CLONE_VAR_EDITABLE  , TRUE , 
			CLONE_EQUAL_TEXT, "=", CLONE_EQUAL_EDITABLE, FALSE, 
			CLONE_VAL_TEXT	, "" , CLONE_VAL_EDITABLE  , TRUE , -1);*/
	clone_list_add_item("", "", &iter);
	
	//select this new empty list item
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(clone_list));
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(clone_list));
	gtk_tree_selection_select_iter(selection, &iter);
	GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_selection_select_path(selection, path);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(clone_list), path, NULL, FALSE, 0, 0);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW(clone_list), path, gtk_tree_view_get_column(GTK_TREE_VIEW(clone_list), CLONE_VAR_TEXT), TRUE);

	gtk_tree_path_free(path);

	isac_validate_clone(spw);
}
static void isac_color_assign_syntax(char* token, SPWidget *spw)
{ // called in void isac_json_parse_syntax (const gchar *syntax, SPWidget *spw)

	color_deletelastrow();

	JSONVALUE j, getj, aj;
	string *jvalue;
	std::string JAvalue, maplist;
	char str_num[100], type;
	const char *tag, *value, *code, *map, *color_code;
	char **maptoken, **color_split, *str = NULL;

	GtkListStore *store;
	GtkTreeIter iter;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	j["list"].ToString(maplist, 0);
	map = maplist.c_str();
	type = 'l'; // list

	if (strlen(map) == 0) {
		j["map"].ToString(maplist, 0);
		map = maplist.c_str();
		type = 'm'; //map
	}

	getj.Parse(map);
	size_t nCount = getj.Size();

	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(color_list)));

	switch (type) {

	case 'l':
		for (size_t i = 0; i<nCount; i++){
			aj = getj.At(i);

			aj.ToString(JAvalue, 0);
			str = (char*)JAvalue.c_str();

			aj.Parse(str);
			jvalue = aj["tag"].val.strVal;
			tag = jvalue->c_str();

			jvalue = aj["data"].val.strVal;
			value = jvalue->c_str();

			jvalue = aj["param"].val.strVal;
			code = jvalue->c_str();

			gtk_list_store_append(store, &iter);
			gtk_list_store_set(store, &iter, 0, tag, 1, 1, 2, value, 3, 1, 4, code, 5, 1, -1);
		}
		break;

	case 'm':
		for (size_t i = 0; i<nCount; i++){
			jvalue = j["tag"].val.strVal;
			tag = jvalue->c_str();

			aj = getj.At(i);

			aj.ToString(JAvalue, 0);
			str = (char*)JAvalue.c_str();

			str++;  //remove " in front 
			str[strlen(str) - 1] = '\0';  // remove " trailing behind
			maptoken = (gchar**)g_strsplit(str, "=", 0);

			gtk_list_store_append(store, &iter);
			gtk_list_store_set(store, &iter, 0, tag, 1, 1, 2, (const char*)maptoken[0], 3, 1, 4, (const char*)maptoken[1], 5, 1, -1);
		}
		break;
	}

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, "", 1, 1, 2, "", 3, 1, 4, "", 5, 1, -1);

	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(color_list));
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(color_list));
	gtk_tree_selection_select_iter(selection, &iter);
	GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_selection_select_path(selection, path);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(color_list), path, NULL, FALSE, 0, 0);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW(color_list), path, gtk_tree_view_get_column(GTK_TREE_VIEW(color_list), 0), TRUE);

	gtk_tree_path_free(path);

	isac_validate_color(spw);
}
static void isac_function_assign_syntax(char* token, SPWidget *spw){
	char *name = "", *param = "", *effect = "";
	int end = 0, effectindex = 0;
	GtkWidget *wg;

	name = strstr(token, "name:");
	param = strstr(token, "param:");
	//effect = strstr(token,"effect:");

	end = strcspn(name, ",}");
	name[end - 1] = '\0';
	name = name + 6;

	end = strcspn(param, "}");
	param[end - 1] = '\0';
	param = param + 7;

	/*
	end = strcspn(effect,",}");
	effect[end-1] = '\0';
	effect = effect+8;
	*/

	gchar* newparam;// = remove_backspc((gchar*)param);

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_func_name"));
	gtk_entry_set_text(GTK_ENTRY(wg), name);

	/*
	if(strcmp(g_ascii_strdown(effect,strlen(effect)),"button")==0)
	effectindex = 1;

	wg = GTK_WIDGET(gtk_object_get_data (GTK_OBJECT (spw), "isac_func_effect"));
	gtk_combo_box_set_active (GTK_COMBO_BOX (wg),effectindex);
	*/

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_func_param"));
	gtk_entry_set_text(GTK_ENTRY(wg), newparam);

	isac_validate_function(spw);
}
static void isac_get_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j;
	string *jvalue;
	GtkWidget *wg;

	const char *tag, *align, *type;
	int alignindex = 0, typeindex = 0;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	jvalue = j["tag"].val.strVal;
	tag = jvalue->c_str();

	jvalue = j["align"].val.strVal;
	align = jvalue->c_str();

	jvalue = j["type"].val.strVal;
	type = jvalue->c_str();


	if (strcmp(g_ascii_strdown(align, strlen(align)), "left") == 0)
		alignindex = 1;
	else if (strcmp(g_ascii_strdown(align, strlen(align)), "center") == 0)
		alignindex = 2;

	if (strcmp(g_ascii_strdown(type, strlen(type)), "live") == 0)
		typeindex = 1;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_get_tag"));
	gtk_entry_set_text(GTK_ENTRY(wg), tag);

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_get_align"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(wg), alignindex);

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_get_type"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(wg), typeindex);

	isac_validate_get(spw);
}
static void isac_opacity_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j, jget;
	const char *tag, *cmin, *cmax;
	gint imin, imax;
	string *jvalue;
	std::string svalue;
	GtkWidget *wg;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	jvalue = j["tag"].val.strVal;
	tag = jvalue->c_str();

	jget = j["min"];

	if (jget.jt == JT_STRING){
		jvalue = j["min"].val.strVal;
		cmin = jvalue->c_str();

		jvalue = j["max"].val.strVal;
		cmax = jvalue->c_str();

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_max"));
		gtk_entry_set_text(GTK_ENTRY(wg), cmax);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_min"));
		gtk_entry_set_text(GTK_ENTRY(wg), cmin);
	}

	else {
		imin = j["min"].val.numVal.llVal;
		imax = j["max"].val.numVal.llVal;

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_max"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", imax));

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_min"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", imin));
	}

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_opac_tag"));
	gtk_entry_set_text(GTK_ENTRY(wg), tag);

	isac_validate_opacity(spw);
}
static void isac_open_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j, jget;
	string *jvalue;
	std::string svalue;
	const char *source, *type, *effect, *cistag, *cxpos, *cypos, *cwidth, *cheight;
	gint iistag, ixpos, iypos, iwidth, iheight;
	int effectindex = 0, typeindex = -1;
	GtkWidget *wg;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	jvalue = j["src"].val.strVal;
	source = jvalue->c_str();

	jget = j["x"];

	if (jget.jt == JT_STRING){
		jvalue = j["istag"].val.strVal;
		cistag = jvalue->c_str();

		jvalue = j["x"].val.strVal;
		cxpos = jvalue->c_str();

		jvalue = j["y"].val.strVal;
		cypos = jvalue->c_str();

		jvalue = j["width"].val.strVal;
		cwidth = jvalue->c_str();

		jvalue = j["height"].val.strVal;
		cheight = jvalue->c_str();

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_srctype"));
		gtk_combo_box_set_active(GTK_COMBO_BOX(wg), atoi(cistag));

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_xpos"));
		gtk_entry_set_text(GTK_ENTRY(wg), cxpos);
		if (typeindex == 0)
			gtk_widget_set_sensitive(wg, FALSE);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_ypos"));
		gtk_entry_set_text(GTK_ENTRY(wg), cypos);
		if (typeindex == 0)
			gtk_widget_set_sensitive(wg, FALSE);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_width"));
		gtk_entry_set_text(GTK_ENTRY(wg), cwidth);
		if (typeindex == 0)
			gtk_widget_set_sensitive(wg, FALSE);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_height"));
		gtk_entry_set_text(GTK_ENTRY(wg), cheight);
		if (typeindex == 0)
			gtk_widget_set_sensitive(wg, FALSE);
	}

	else {
		iistag = j["istag"].val.numVal.llVal;
		ixpos = j["x"].val.numVal.llVal;
		iypos = j["y"].val.numVal.llVal;
		iwidth = j["width"].val.numVal.llVal;
		iheight = j["height"].val.numVal.llVal;

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_srctype"));
		gtk_combo_box_set_active(GTK_COMBO_BOX(wg), iistag);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_xpos"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", ixpos));
		if (typeindex == 0)
			gtk_widget_set_sensitive(wg, FALSE);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_ypos"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", iypos));
		if (typeindex == 0)
			gtk_widget_set_sensitive(wg, FALSE);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_width"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", iwidth));
		if (typeindex == 0)
			gtk_widget_set_sensitive(wg, FALSE);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_height"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", iheight));
		if (typeindex == 0)
			gtk_widget_set_sensitive(wg, FALSE);
	}

	jvalue = j["type"].val.strVal;
	type = jvalue->c_str();

	if (strcmp(g_ascii_strdown(type, strlen(type)), "_self") == 0)
		typeindex = 0;
	else if (strcmp(g_ascii_strdown(type, strlen(type)), "_blank") == 0)
		typeindex = 1;
	else if (strcmp(g_ascii_strdown(type, strlen(type)), "_shared") == 0)
		typeindex = 2;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_type"));
	gtk_entry_set_text((GtkEntry*)(gtk_entry_get_text((GtkEntry*)GTK_BIN(wg))), (const gchar*)type);
	gtk_combo_box_set_active(GTK_COMBO_BOX(wg), typeindex);

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_open_source"));
	gtk_entry_set_text(GTK_ENTRY(wg), source);

	isac_validate_open(spw);
}
static void isac_popup_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j, jget;
	string *jvalue;
	std::string svalue;
	const char *source, *cxpos, *cypos, *cwidth, *cheight;
	gint ixpos, iypos, iwidth, iheight;
	GtkWidget *wg;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	jvalue = j["src"].val.strVal;
	source = jvalue->c_str();

	jget = j["x"];

	if (jget.jt == JT_STRING){
		jvalue = j["x"].val.strVal;
		cxpos = jvalue->c_str();

		jvalue = j["y"].val.strVal;
		cypos = jvalue->c_str();

		jvalue = j["width"].val.strVal;
		cwidth = jvalue->c_str();

		jvalue = j["height"].val.strVal;
		cheight = jvalue->c_str();

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_xpos"));
		gtk_entry_set_text(GTK_ENTRY(wg), cxpos);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_ypos"));
		gtk_entry_set_text(GTK_ENTRY(wg), cypos);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_width"));
		gtk_entry_set_text(GTK_ENTRY(wg), cwidth);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_height"));
		gtk_entry_set_text(GTK_ENTRY(wg), cheight);
	}

	else {
		ixpos = j["x"].val.numVal.llVal;
		iypos = j["y"].val.numVal.llVal;
		iwidth = j["width"].val.numVal.llVal;
		iheight = j["height"].val.numVal.llVal;

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_xpos"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", ixpos));

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_ypos"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", iypos));

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_width"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", iwidth));

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_height"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", iheight));
	}

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_popup_source"));
	gtk_entry_set_text(GTK_ENTRY(wg), source);

	isac_validate_popup(spw);
}
static void isac_rotate_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j, jget;
	string *jvalue;
	std::string svalue;
	const char *tag, *cmin, *cmax;
	gint imin, imax;
	GtkWidget *wg;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	jvalue = j["tag"].val.strVal;
	tag = jvalue->c_str();

	jget = j["min"];

	if (jget.jt == JT_STRING){
		jvalue = j["min"].val.strVal;
		cmin = jvalue->c_str();

		jvalue = j["max"].val.strVal;
		cmax = jvalue->c_str();

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_max"));
		gtk_entry_set_text(GTK_ENTRY(wg), cmax);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_min"));
		gtk_entry_set_text(GTK_ENTRY(wg), cmin);
	}

	else {
		imin = j["min"].val.numVal.llVal;
		imax = j["max"].val.numVal.llVal;

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_max"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", imax));

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_min"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", imin));
	}

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_rot_tag"));
	gtk_entry_set_text(GTK_ENTRY(wg), tag);

	isac_validate_rotate(spw);
}
static void isac_script_assign_syntax(char* token, SPWidget *spw) // called in isac_script_assign_syntax
{

	JSONVALUE
		j,
		getj,
		aj;
	string
		*jvalue;
	std::string
		JAvalue,
		maplist;
	char
		str_num[100],
		type = 'o',
		*str = NULL;
	const char
		*evt,
		*event,
		*scr, *key,
		*map;

	char *scrr;
	
	gboolean			 
		bCtrl = FALSE, 
		bAlt = FALSE, 
		bShift = FALSE;	//HSH - Keydown

	guint underline = PANGO_UNDERLINE_SINGLE;

	GtkTreeModel *model;
	GtkListStore *store;
	GtkTreePath *path;
	GtkTreeIter iter;
	GtkTextView *wg_script;
	GtkTextBuffer *buffer;

	GtkEntry *wg_key;	//HSH Keydown
	GtkToggleButton *wg_ctrl, *wg_alt, *wg_shift;
	string szKey = ""; 

	wg_script = GTK_TEXT_VIEW(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_code"));
	wg_key = GTK_ENTRY(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_key"));	//get key text entry field
	wg_ctrl = GTK_TOGGLE_BUTTON(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_ctrl"));	//get ctrl check button
	wg_alt = GTK_TOGGLE_BUTTON(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_alt"));	//get alt check button
	wg_shift = GTK_TOGGLE_BUTTON(gtk_object_get_data(GTK_OBJECT(spw), "isac_script_keydown_shift"));	//gett shift check button
	
	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	j["list"].ToString(maplist, 0);
	map = maplist.c_str();
	if (strlen(map) != 0)
		type = 'l'; //list

	// here, double backslash in JSON string becomes eight backslash i.e. 4 times
	map = replace(map, "\\\\\\\\", "\\");
	// here, after replaced, it printouts 4 backslashes
	getj.Parse(map);
	map = replace(map, "\\\\\\\\", "\\");

	size_t nCount = getj.Size();
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(script_list));
	store = GTK_LIST_STORE(model);
	path = gtk_tree_path_new_first();
	switch (type) {
	case 'l':
		for (size_t i = 0; i<nCount; i++){
			aj = getj.At(i);
			aj.ToString(JAvalue, 0);
			str = (char*)JAvalue.c_str();
			aj.Parse(str);
			jvalue = aj["evt"].val.strVal;
			event = jvalue->c_str();
			while (gtk_tree_model_get_iter(model, &iter, path)){
				gtk_tree_model_get(model, &iter, 0, &evt, -1);
				if (strcmp(evt, event) == 0) {
					jvalue = aj["param"].val.strVal;
					scr = jvalue->c_str();
					scrr = const_cast<char*>(scr);
					scrr = replace(scrr, "\\\\\\\\", "\\");
					// here, the backslash is four. if we replace the four backslash to one, it will be loaded as " i.e. doublequote
					
					//HSH - keydown start
					if (strcmp(evt, "keydown") == 0) {

						jvalue = aj["keys"].val.strVal;
						const char* keys = jvalue->c_str();
						char* kkeys = const_cast<char*>(keys);
						kkeys = replace(kkeys, "\\\\\\\\", "\\");
						string szKeys = kkeys;
						size_t nStart = 0;
						size_t nFound = 0;
						string szFind;
						
						//find ctrl+
						szFind = "ctrl+";
						nFound = szKeys.find(szFind);
						if (nFound != string::npos) {
							bCtrl = TRUE;
							nStart += szFind.length();
						}

						//find alt+
						szFind = "alt+";
						nFound = szKeys.find(szFind);
						if (nFound != string::npos) {
							bAlt = TRUE;
							nStart += szFind.length();
						}

						//find shift+
						szFind = "shift+";
						nFound = szKeys.find(szFind);
						if (nFound != string::npos) {
							bShift = TRUE;
							nStart += szFind.length();
						}
						
						//the rest of the string should be the key (user input)
						szKey = szKeys.substr(nStart, szKeys.length() - nStart);

						if (szKey.empty() && strlen(scrr) != 0) {
							underline = PANGO_UNDERLINE_ERROR;
						}
					}
					//HSH - keydown end

					gtk_list_store_set(store, &iter, 1, underline, 2, scrr,
						3, bCtrl, 4, bAlt, 5, bShift,
						6, szKey.c_str(),
						-1);
					//gtk_list_store_set(store, &iter, 1, PANGO_UNDERLINE_SINGLE, 2, scrr, -1);
					//gtk_list_store_set(store, &iter, 1, PANGO_UNDERLINE_NONE, 2, scrr, -1);
					break;
				}
				gtk_tree_path_next(path);
			}
		}
		break;
	case 'o':
		jvalue = j["evt"].val.strVal;
		event = jvalue->c_str();
		while (gtk_tree_model_get_iter(model, &iter, path)) {
			gtk_tree_model_get(model, &iter, 0, &evt, -1);
			if (strcmp(evt, event) == 0) {
				jvalue = j["param"].val.strVal;
				scr = jvalue->c_str();
				gtk_list_store_set(store, &iter, 1, underline, 2, scr, -1);
				//gtk_list_store_set(store, &iter, 1, PANGO_UNDERLINE_NONE, 2, scr, -1);
				break;
			}
			gtk_tree_path_next(path);
		}
		break;
	}
	if (gtk_tree_model_get_iter(model, &iter, gtk_tree_path_new_first())) {
		gtk_tree_model_get(model, &iter, 0, &evt, 2, &scr,
			3, &bCtrl, 4, &bAlt, 5, &bShift, 6, &key, -1);
		buffer = gtk_text_view_get_buffer(wg_script);
		gtk_text_buffer_set_text(buffer, scr, -1);

		if (strcmp(evt, "keydown") == 0) {
			if (wg_key != NULL) gtk_entry_set_text(wg_key, key);
			if (wg_ctrl != NULL) gtk_toggle_button_set_active(wg_ctrl, bCtrl);
			if (wg_alt != NULL) gtk_toggle_button_set_active(wg_alt, bCtrl);
			if (wg_shift != NULL) gtk_toggle_button_set_active(wg_shift, bCtrl);
		} 
	}
	isac_validate_script(spw);
	gtk_tree_path_free(path);
}
static void isac_set_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j;
	string *jvalue;
	GtkWidget *wg;
	const char *tag, *align, *source, *type, *prompt;
	int alignindex = 0, typeindex = 0;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	jvalue = j["tag"].val.strVal;
	tag = jvalue->c_str();

	jvalue = j["align"].val.strVal;
	align = jvalue->c_str();

	jvalue = j["src"].val.strVal;
	source = jvalue->c_str();

	jvalue = j["type"].val.strVal;
	type = jvalue->c_str();

	jvalue = j["prompt"].val.strVal;
	prompt = jvalue->c_str();


	if (strcmp(g_ascii_strdown(align, strlen(align)), "left") == 0)
		alignindex = 1;
	else if (strcmp(g_ascii_strdown(align, strlen(align)), "center") == 0)
		alignindex = 2;

	if (strcmp(g_ascii_strdown(type, strlen(type)), "variable") == 0)
		typeindex = 1;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_tag"));
	gtk_entry_set_text(GTK_ENTRY(wg), tag);

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_align"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(wg), alignindex);

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_source"));
	gtk_entry_set_text(GTK_ENTRY(wg), source);

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_type"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(wg), typeindex);

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_set_prompt"));
	gtk_entry_set_text(GTK_ENTRY(wg), prompt);

	isac_validate_set(spw);
}
static void isac_slider_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j, jget;
	string *jvalue;
	std::string svalue;
	GtkWidget *wg;
	const char *tag, *cmin, *cmax, *cread;
	gint imin, imax, iread;
	gboolean readonly = FALSE;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	jvalue = j["tag"].val.strVal;
	tag = jvalue->c_str();

	jget = j["min"];

	if (jget.jt == JT_STRING){
		jvalue = j["min"].val.strVal;
		cmin = jvalue->c_str();

		jvalue = j["max"].val.strVal;
		cmax = jvalue->c_str();

		jvalue = j["readonly"].val.strVal;
		cread = jvalue->c_str();

		if (strcmp(g_ascii_strdown(cread, strlen(cread)), "1") == 0)readonly = TRUE;

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_max"));
		gtk_entry_set_text(GTK_ENTRY(wg), cmax);

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_min"));
		gtk_entry_set_text(GTK_ENTRY(wg), cmin);
	}

	else {
		imin = j["min"].val.numVal.llVal;
		imax = j["max"].val.numVal.llVal;
		iread = j["readonly"].val.numVal.llVal;
		if (iread == 1) readonly = TRUE;

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_max"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", imax));

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_min"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", imin));
	}

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_tag"));
	gtk_entry_set_text(GTK_ENTRY(wg), tag);

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_slider_read"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wg), readonly);

	isac_validate_slider(spw);
}
static void isac_text_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j, getj;
	string *jvalue;
	std::string JAvalue, pairlist;
	GtkWidget *wg;

	const char *tag, *map;
	char **pairtoken, *str = NULL;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	jvalue = j["tag"].val.strVal;
	tag = jvalue->c_str();

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_text_tag"));
	gtk_entry_set_text(GTK_ENTRY(wg), tag);

	j["map"].ToString(pairlist, 0);
	map = pairlist.c_str();

	getj.Parse(map);
	size_t nCount = getj.Size();

	for (size_t i = 0; i<nCount; i++){

		j = getj.At(i);

		j.ToString(JAvalue, 0);
		str = (char*)JAvalue.c_str();

		str++;  //remove " in front 
		str[strlen(str) - 1] = '\0';  // remove " trailing behind

		pairtoken = (gchar**)g_strsplit(str, "=", 0);

		wg = GTK_WIDGET(g_slist_nth_data(text_value_list, i));
		gtk_entry_set_text(GTK_ENTRY(wg), remove_backspc((char*)pairtoken[0]));

		wg = GTK_WIDGET(g_slist_nth_data(text_display_list, i));
		gtk_entry_set_text(GTK_ENTRY(wg), remove_backspc((char*)pairtoken[1]));

	}
	isac_validate_text(spw);
}
static void isac_tooltips_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j, getj, jget;
	string *jvalue;
	std::string JAvalue, ttlist, svalue;
	GtkWidget *wg;

	const char *param, *style, *csize;
	gint isize;
	char *str = NULL;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);

	jget = j["size"];

	if (jget.jt == JT_STRING){
		jvalue = j["size"].val.strVal;
		csize = jvalue->c_str();

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_tooltips_size"));
		gtk_entry_set_text(GTK_ENTRY(wg), csize);
	}

	else {
		isize = j["size"].val.numVal.llVal;

		wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_tooltips_size"));
		gtk_entry_set_text(GTK_ENTRY(wg), g_strdup_printf("%d", isize));
	}

	jvalue = j["style"].val.strVal;
	style = jvalue->c_str();

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_tooltips_style"));
	gtk_entry_set_text(GTK_ENTRY(wg), style);


	j["param"].ToString(ttlist, 0);
	param = ttlist.c_str();

	getj.Parse(param, JSON_FLAG_ALLOWUNQUOTENAME);
	size_t nCount = getj.Size();

	for (size_t i = 0; i<nCount; i++){

		j = getj.At(i);

		j.ToString(JAvalue, 0);
		str = (char*)JAvalue.c_str();

		str++;  //remove " in front 
		str[strlen(str) - 1] = '\0';  // remove " trailing behind

		wg = GTK_WIDGET(g_slist_nth_data(tt_value_list, i));
		gtk_entry_set_text(GTK_ENTRY(wg), remove_backspc(str));
		//gtk_entry_set_text (GTK_ENTRY (wg),str);
	}

	isac_validate_tooltips(spw);
}
static void isac_zoom_assign_syntax(char* token, SPWidget *spw){

	JSONVALUE j;
	string *jvalue;
	GtkWidget *wg;
	const char *tag, *align;
	int alignindex = 0;

	j.Parse(token, JSON_FLAG_ALLOWUNQUOTENAME);
	jvalue = j["tag"].val.strVal;
	tag = jvalue->c_str();

	jvalue = j["align"].val.strVal;
	align = jvalue->c_str();

	if (strcmp(g_ascii_strdown(align, strlen(align)), "center") == 0)
		alignindex = 1;
	else if (strcmp(g_ascii_strdown(align, strlen(align)), "left") == 0)
		alignindex = 2;
	else if (strcmp(g_ascii_strdown(align, strlen(align)), "right") == 0)
		alignindex = 3;
	else if (strcmp(g_ascii_strdown(align, strlen(align)), "top") == 0)
		alignindex = 4;
	else if (strcmp(g_ascii_strdown(align, strlen(align)), "bottom") == 0)
		alignindex = 5;

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_zoom_tag"));
	gtk_entry_set_text(GTK_ENTRY(wg), tag);

	wg = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(spw), "isac_zoom_align"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(wg), alignindex);

	isac_validate_zoom(spw);
}