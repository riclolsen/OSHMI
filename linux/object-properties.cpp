/**
 * @file Object properties dialog.
 */
/* 
 * Inkscape, an Open Source vector graphics editor
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Copyright (C) 2012 Kris De Gussem <Kris.DeGussem@gmail.com>
 * c++ version based on former C-version (GPL v2) with authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *   bulia byak <buliabyak@users.sf.net>
 *   Johan Engelen <goejendaagh@zonnet.nl>
 *   Abhishek Sharma
 */

#include <glibmm.h>
#include <glibmm/i18n.h>
#include "object-properties.h"
#include "widgets/sp-attribute-widget.h"
#include "../../desktop-handles.h"
#include "../../document.h"
#include "../../document-undo.h"
#include "verbs.h"
#include "inkscape.h"
#include "selection.h"
#include "desktop.h"
#include "sp-item.h"
#include "sp-image.h"
#include "xml/repr.h"

#if WITH_GTKMM_3_0
# include <gtkmm/grid.h>
#else
# include <gtkmm/table.h>
#endif

#ifdef HAVE_CONFIG_H
# include "config.h"
//#include <windows.h>
#endif
#include "../../desktop-handles.h"
#include "../../dialogs/dialog-events.h"
#include "../../document.h"
#include "../../helper/window.h"
#include "inkscape.h"
#include "../../document-undo.h"
#include "../../interface.h"
#include "../../macros.h"
#include "../../preferences.h"
#include "selection.h"
#include "sp-item.h"
#include "../../verbs.h"
#include "../../widgets/sp-attribute-widget.h"
#include "../../widgets/sp-widget.h"
#include "isac-menu.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
//#include <windows.h>
#endif
#define MIN_ONSCREEN_DISTANCE 50
GtkWidget *dlg = NULL; // naim@ecava.com: declared extern in isac-menu.h. originally static GtkWidget *dlg = NULL;
static win_data wd;
// impossible original values to make sure they are read from prefs
static gint x = -1000, y = -1000, w = 0, h = 0;
static Glib::ustring const prefs_path = "/dialogs/object/";

namespace Inkscape {
namespace UI {
namespace Dialog {

ObjectProperties::ObjectProperties (void) : 
    UI::Widget::Panel ("", "/dialogs/object/", SP_VERB_DIALOG_ITEM),
    blocked (false),
    CurrentItem(NULL),
						#if WITH_GTKMM_3_0
							TopTable(Gtk::manage(new Gtk::Grid())),
						#else
						TopTable(Gtk::manage(new Gtk::Table(4, 4))),
						#endif
						LabelID(_("_ID:"), 1),
						//LabelLabel(_("_Label:"), 1),
						//LabelTitle(_("_Title:"),1),
						//LabelImageRendering(_("_Image Rendering:"),1),
						//LabelDescription(_("_Description:"),1),
						//FrameDescription("", FALSE),
						HBoxCheck(FALSE, 0),
						#if WITH_GTKMM_3_0
							CheckTable(Gtk::manage(new Gtk::Grid())),
						#else
							CheckTable(Gtk::manage(new Gtk::Table(1, 2, true))),
						#endif
						CBHide(_("_Hide"), 1),
						CBLock(_("L_ock"), 1),
						BSet (_("_Set"), 1),
						//LabelInteractivity(_("_Interactivity"), 1),
						attrTable(Gtk::manage(new SPAttributeTable())),
						desktop(NULL),
						deskTrack(),
						selectChangedConn(),
						subselChangedConn() 
{
    //initialize labels for the table at the bottom of the dialog
    int_attrs.push_back("onclick");
    int_attrs.push_back("onmouseover");
    int_attrs.push_back("onmouseout");
    int_attrs.push_back("onmousedown");
    int_attrs.push_back("onmouseup");
    int_attrs.push_back("onmousemove");
    int_attrs.push_back("onfocusin");
    int_attrs.push_back("onfocusout");
    int_attrs.push_back("onload");
	
    int_labels.push_back("onclick:");
    int_labels.push_back("onmouseover:");
    int_labels.push_back("onmouseout:");
    int_labels.push_back("onmousedown:");
    int_labels.push_back("onmouseup:");
    int_labels.push_back("onmousemove:");
    int_labels.push_back("onfocusin:");
    int_labels.push_back("onfocusout:");
    int_labels.push_back("onload:");
    
    desktopChangeConn = deskTrack.connectDesktopChanged( sigc::mem_fun(*this, &ObjectProperties::setTargetDesktop) );
    deskTrack.connect(GTK_WIDGET(gobj()));

	#if WITH_GTKMM_3_0
	   CheckTable->set_row_homogeneous();
	  CheckTable->set_column_homogeneous(true);
	#endif
    
	MakeWidget();
}

ObjectProperties::~ObjectProperties (void)
{
    subselChangedConn.disconnect();
    selectChangedConn.disconnect();
    desktopChangeConn.disconnect();
    deskTrack.disconnect();
}

void ObjectProperties::MakeWidget(void)
{
    //Gtk::Box *contents = _getContents();
    //contents->set_spacing(0);
    
    //TopTable->set_border_width(4);

#if WITH_GTKMM_3_0
    TopTable->set_row_spacing(4);
    TopTable->set_column_spacing(0);
#else
    //TopTable->set_row_spacings(4);
    //TopTable->set_col_spacings(0);
#endif

    //contents->pack_start (*TopTable, false, false, 0);

    /* Create the label for the object id */
    //LabelID.set_label (LabelID.get_label() + " ");
    //LabelID.set_alignment (1, 0.5);

#if WITH_GTKMM_3_0
    LabelID.set_valign(Gtk::ALIGN_CENTER);
    TopTable->attach(LabelID, 0, 0, 1, 1);
#else
    //TopTable->attach(LabelID, 0, 1, 0, 1,
    //                  Gtk::SHRINK | Gtk::FILL,
    //                  Gtk::AttachOptions(), 0, 0 );
#endif

    /* Create the entry box for the object id */
    //EntryID.set_tooltip_text (_("The id= attribute (only letters, digits, and the characters .-_: allowed)"));
    //EntryID.set_max_length (64);

#if WITH_GTKMM_3_0
    EntryID.set_valign(Gtk::ALIGN_CENTER);
    TopTable->attach(EntryID, 1, 0, 1, 1);
#else
    //TopTable->attach(EntryID, 1, 2, 0, 1,
    //                 Gtk::EXPAND | Gtk::FILL,
    //                 Gtk::AttachOptions(), 0, 0 );
#endif

    //LabelID.set_mnemonic_widget (EntryID);

    // pressing enter in the id field is the same as clicking Set:
    //EntryID.signal_activate().connect(sigc::mem_fun(this, &ObjectProperties::label_changed));
    // focus is in the id field initially:
    //EntryID.grab_focus();

    /* Create the label for the object label */
    //LabelLabel.set_label (LabelLabel.get_label() + " ");
    //LabelLabel.set_alignment (1, 0.5);

#if WITH_GTKMM_3_0
    //LabelLabel.set_valign(Gtk::ALIGN_CENTER);
    //TopTable->attach(LabelLabel, 0, 1, 1, 1);
#else
    //TopTable->attach(LabelLabel, 0, 1, 1, 2,
    //                 Gtk::SHRINK | Gtk::FILL,
    //                 Gtk::AttachOptions(), 0, 0 );
#endif

    /* Create the entry box for the object label */
    //EntryLabel.set_tooltip_text (_("A freeform label for the object"));
    //EntryLabel.set_max_length (256);

#if WITH_GTKMM_3_0
    //EntryLabel.set_hexpand();
    //EntryLabel.set_valign(Gtk::ALIGN_CENTER);
    //TopTable->attach(EntryLabel, 1, 1, 1, 1);
#else
    //TopTable->attach(EntryLabel, 1, 2, 1, 2,
    //                 Gtk::EXPAND | Gtk::FILL,
    //                 Gtk::AttachOptions(), 0, 0 );
#endif

    //LabelLabel.set_mnemonic_widget (EntryLabel);

    // pressing enter in the label field is the same as clicking Set:
    //EntryLabel.signal_activate().connect(sigc::mem_fun(this, &ObjectProperties::label_changed));

    /* Create the label for the object title */
    //LabelTitle.set_label (LabelTitle.get_label() + " ");
    //LabelTitle.set_alignment (1, 0.5);

#if WITH_GTKMM_3_0
    //LabelTitle.set_valign(Gtk::ALIGN_CENTER);
    //TopTable->attach(LabelTitle, 0, 2, 1, 1);
#else
    //TopTable->attach(LabelTitle, 0, 1, 2, 3,
    //                 Gtk::SHRINK | Gtk::FILL,
    //                 Gtk::AttachOptions(), 0, 0 );
#endif

    /* Create the entry box for the object title */
    //EntryTitle.set_sensitive (FALSE);
    //EntryTitle.set_max_length (256);

#if WITH_GTKMM_3_0
    //EntryTitle.set_hexpand();
    //EntryTitle.set_valign(Gtk::ALIGN_CENTER);
    //TopTable->attach(EntryTitle, 1, 2, 1, 1);
#else
    //TopTable->attach(EntryTitle, 1, 2, 2, 3,
    //                 Gtk::EXPAND | Gtk::FILL,
    //                 Gtk::AttachOptions(), 0, 0 );
#endif

    //LabelTitle.set_mnemonic_widget (EntryTitle);
    // pressing enter in the label field is the same as clicking Set:
    //EntryTitle.signal_activate().connect(sigc::mem_fun(this, &ObjectProperties::label_changed));

    /* Create the frame for the object description */
    //FrameDescription.set_label_widget (LabelDescription);
    //FrameDescription.set_padding (0,0,0,0);
    //contents->pack_start (FrameDescription, true, true, 0);

    /* Create the text view box for the object description */
    //FrameTextDescription.set_border_width(4);
    //FrameTextDescription.set_sensitive (FALSE);
    //FrameDescription.add (FrameTextDescription);
    //FrameTextDescription.set_shadow_type (Gtk::SHADOW_IN);

    //TextViewDescription.set_wrap_mode(Gtk::WRAP_WORD);
    //TextViewDescription.get_buffer()->set_text("");
    //FrameTextDescription.add (TextViewDescription);
    //TextViewDescription.add_mnemonic_label(LabelDescription);

    /* Image rendering */
    /* Create the label for the object ImageRendering */
    //LabelImageRendering.set_label (LabelImageRendering.get_label() + " ");
    //LabelImageRendering.set_alignment (1, 0.5);

#if WITH_GTKMM_3_0
    //LabelImageRendering.set_valign(Gtk::ALIGN_CENTER);
    //TopTable->attach(LabelImageRendering, 0, 3, 1, 1);
#else
    //TopTable->attach(LabelImageRendering, 0, 1, 3, 4,
    //                  Gtk::SHRINK | Gtk::FILL,
    //                  Gtk::AttachOptions(), 0, 0 );
#endif

    /* Create the combo box text for the 'image-rendering' property  */
    //ComboBoxTextImageRendering.append( "auto" );
    //ComboBoxTextImageRendering.append( "optimizeQuality" );
    //ComboBoxTextImageRendering.append( "optimizeSpeed" );
    //ComboBoxTextImageRendering.set_tooltip_text (_("The 'image-rendering' property can influence how a bitmap is up-scaled:\n\t'auto' no preference;\n\t'optimizeQuality' smooth;\n\t'optimizeSpeed' blocky.\nNote that this behaviour is not defined in the SVG 1.1 specification and not all browsers follow this interpretation."));

#if WITH_GTKMM_3_0
    //ComboBoxTextImageRendering.set_valign(Gtk::ALIGN_CENTER);
    //TopTable->attach(ComboBoxTextImageRendering, 1, 3, 1, 1);
#else
    //TopTable->attach(ComboBoxTextImageRendering, 1, 2, 3, 4,
    //                 Gtk::EXPAND | Gtk::FILL,
    //                 Gtk::AttachOptions(), 0, 0 );
#endif

    //LabelImageRendering.set_mnemonic_widget (ComboBoxTextImageRendering);

    //ComboBoxTextImageRendering.signal_changed().connect(sigc::mem_fun(this, &ObjectProperties::image_rendering_changed));

    /* Check boxes */
    //contents->pack_start (HBoxCheck, FALSE, FALSE, 0);
    //CheckTable->set_border_width(4);
    //HBoxCheck.pack_start(*CheckTable, true, true, 0);

    /* Hide */
    //CBHide.set_tooltip_text (_("Check to make the object invisible"));

#if WITH_GTKMM_3_0
    CBHide.set_hexpand();
    CBHide.set_valign(Gtk::ALIGN_CENTER);
    CheckTable->attach(CBHide, 0, 0, 1, 1);
#else
    //CheckTable->attach(CBHide, 0, 1, 0, 1,
    //                   Gtk::EXPAND | Gtk::FILL,
    //                   Gtk::AttachOptions(), 0, 0 );
#endif

    //CBHide.signal_toggled().connect(sigc::mem_fun(this, &ObjectProperties::hidden_toggled));

    /* Lock */
    // TRANSLATORS: "Lock" is a verb here
    //CBLock.set_tooltip_text (_("Check to make the object insensitive (not selectable by mouse)"));

#if WITH_GTKMM_3_0
    CBLock.set_hexpand();
    CBLock.set_valign(Gtk::ALIGN_CENTER);
    CheckTable->attach(CBLock, 1, 0, 1, 1);
#else
    //CheckTable->attach(CBLock, 1, 2, 0, 1,
    //                   Gtk::EXPAND | Gtk::FILL,
    //                   Gtk::AttachOptions(), 0, 0 );
#endif

    //CBLock.signal_toggled().connect(sigc::mem_fun(this, &ObjectProperties::sensitivity_toggled));


    /* Button for setting the object's id, label, title and description. */
#if WITH_GTKMM_3_0
    BSet.set_hexpand();
    BSet.set_valign(Gtk::ALIGN_CENTER);
    CheckTable->attach(BSet, 2, 0, 1, 1);
#else
    //CheckTable->attach(BSet, 2, 3, 0, 1,
    //                   Gtk::EXPAND | Gtk::FILL,
    //                   Gtk::AttachOptions(), 0, 0 );
#endif

    //BSet.signal_clicked().connect(sigc::mem_fun(this, &ObjectProperties::label_changed));

    /* Create the frame for interactivity options */
    //EInteractivity.set_label_widget (LabelInteractivity);
    //contents->pack_start (EInteractivity, FALSE, FALSE, 0);

	sp_item_dialog(); //////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    show_all ();
    widget_setup();
}

void ObjectProperties::widget_setup(void)
{
    if (blocked || !desktop)
    {
        return;
    }
    if (SP_ACTIVE_DESKTOP != desktop)
    {
        return;
    }

    Inkscape::Selection *selection = sp_desktop_selection (SP_ACTIVE_DESKTOP);
    Gtk::Box *contents = _getContents();

    if (!selection->singleItem()) {
        contents->set_sensitive (false);
        CurrentItem = NULL;
        //no selection anymore or multiple objects selected, means that we need
        //to close the connections to the previously selected object
        attrTable->clear();
        return;
    } else {
        contents->set_sensitive (true);
    }
    
    SPItem *item = selection->singleItem();
    if (CurrentItem == item)
    {
        //otherwise we would end up wasting resources through the modify selection
        //callback when moving an object (endlessly setting the labels and recreating attrTable)
        return;
    }
    blocked = true;
    
    CBLock.set_active (item->isLocked());           /* Sensitive */
    CBHide.set_active (item->isExplicitlyHidden()); /* Hidden */
    
    if (item->cloned) {
        /* ID */
        EntryID.set_text ("");
        EntryID.set_sensitive (FALSE);
        LabelID.set_text (_("Ref"));

        /* Label */
        //EntryLabel.set_text ("");
        //EntryLabel.set_sensitive (FALSE);
        //LabelLabel.set_text (_("Ref"));

    } else {
        SPObject *obj = static_cast<SPObject*>(item);

        /* ID */
        EntryID.set_text (obj->getId());
        EntryID.set_sensitive (TRUE);
        LabelID.set_markup_with_mnemonic (_("_ID:"));

        /* Label */
        //EntryLabel.set_text(obj->defaultLabel());
        //EntryLabel.set_sensitive (TRUE);

        /* Title */
        //gchar *title = obj->title();
        //if (title) {
        //    EntryTitle.set_text(title);
        //    g_free(title);
        //}
        //else {
        //    EntryTitle.set_text("");
        //}
        //EntryTitle.set_sensitive(TRUE);

        /* Image Rendering */
        //if( SP_IS_IMAGE( item ) ) {
        //    ComboBoxTextImageRendering.show();
        //    LabelImageRendering.show();
        //    char const *str = obj->getStyleProperty( "image-rendering", "auto" );
        //    if( strcmp( str, "auto" ) == 0 ) {
        //        ComboBoxTextImageRendering.set_active(0);
        //    } else if( strcmp( str, "optimizeQuality" ) == 0 ) {
        //        ComboBoxTextImageRendering.set_active(1);
        //    }  else {
        //        ComboBoxTextImageRendering.set_active(2);
        //    }
        //} else {
        //    ComboBoxTextImageRendering.hide();
        //    ComboBoxTextImageRendering.unset_active();
        //    LabelImageRendering.hide();
        //}

        /* Description */
        //gchar *desc = obj->desc();
        //if (desc) {
        //    TextViewDescription.get_buffer()->set_text(desc);
        //    g_free(desc);
        //} else {
        //    TextViewDescription.get_buffer()->set_text("");
        //}
        //FrameTextDescription.set_sensitive(TRUE);
        
        if (CurrentItem == NULL)
        {
            //attrTable->set_object(obj, int_labels, int_attrs, (GtkWidget*)EInteractivity.gobj());
        }
        else
        {
            attrTable->change_object(obj);
        }
        attrTable->show_all();
    }
    CurrentItem = item;
    blocked = false;
}

void ObjectProperties::label_changed(void)
{
    if (blocked)
    {
        return;
    }
    
    SPItem *item = sp_desktop_selection(SP_ACTIVE_DESKTOP)->singleItem();
    g_return_if_fail (item != NULL);

    blocked = true;

    /* Retrieve the label widget for the object's id */
    gchar *id = g_strdup(EntryID.get_text().c_str());
    g_strcanon (id, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.:", '_');
    if (!strcmp (id, item->getId())) {
        LabelID.set_markup_with_mnemonic(_("_ID:"));
    } else if (!*id || !isalnum (*id)) {
        LabelID.set_text (_("Id invalid! "));
    } else if (SP_ACTIVE_DOCUMENT->getObjectById(id) != NULL) {
        LabelID.set_text (_("Id exists! "));
    } else {
        SPException ex;
        LabelID.set_markup_with_mnemonic(_("_ID:"));
        SP_EXCEPTION_INIT (&ex);
        item->setAttribute("id", id, &ex);
        DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM, _("Set object ID"));
    }
    g_free (id);

    /* Retrieve the label widget for the object's label */
    //Glib::ustring label = EntryLabel.get_text();

    /* Give feedback on success of setting the drawing object's label
     * using the widget's label text
     */
    SPObject *obj = static_cast<SPObject*>(item);
    //if (label.compare (obj->defaultLabel())) {
    //    obj->setLabel(label.c_str());
    //    DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
    //            _("Set object label"));
    //}

    /* Retrieve the title */
    //if (obj->setTitle(EntryTitle.get_text().c_str()))
    //    DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
    //            _("Set object title"));

    /* Retrieve the description */
    //Gtk::TextBuffer::iterator start, end;
    //TextViewDescription.get_buffer()->get_bounds(start, end);
    //Glib::ustring desc = TextViewDescription.get_buffer()->get_text(start, end, TRUE);
    //if (obj->setDesc(desc.c_str()))
    //    DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
    //            _("Set object description"));
    
    blocked = false;
}
void ObjectProperties::image_rendering_changed(void)
{
    if (blocked)
    {
        return;
    }
    
    SPItem *item = sp_desktop_selection(SP_ACTIVE_DESKTOP)->singleItem();
    g_return_if_fail (item != NULL);

    blocked = true;

    Glib::ustring scale = ComboBoxTextImageRendering.get_active_text();

    // We should unset if the parent computed value is auto and the desired value is auto.
    SPCSSAttr *css = sp_repr_css_attr_new();
    sp_repr_css_set_property(css, "image-rendering", scale.c_str());
    Inkscape::XML::Node *image_node = item->getRepr();
    if( image_node ) {
        sp_repr_css_change(image_node, css, "style");
    }
    sp_repr_css_attr_unref( css );
        
    blocked = false;
}

void ObjectProperties::sensitivity_toggled (void)
{
    if (blocked)
    {
        return;
    }

    SPItem *item = sp_desktop_selection(SP_ACTIVE_DESKTOP)->singleItem();
    g_return_if_fail (item != NULL);

    blocked = true;
    item->setLocked(CBLock.get_active());
    DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
               CBLock.get_active()? _("Lock object") : _("Unlock object"));
    blocked = false;
}

void ObjectProperties::hidden_toggled(void)
{
    if (blocked)
    {
        return;
    }

    SPItem *item = sp_desktop_selection(SP_ACTIVE_DESKTOP)->singleItem();
    g_return_if_fail (item != NULL);

    blocked = true;
    item->setExplicitlyHidden(CBHide.get_active());
    DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
               CBHide.get_active()? _("Hide object") : _("Unhide object"));
    blocked = false;
}

void ObjectProperties::setDesktop(SPDesktop *desktop)
{
    Panel::setDesktop(desktop);
    deskTrack.setBase(desktop);
}

void ObjectProperties::setTargetDesktop(SPDesktop *desktop)
{
    if (this->desktop != desktop) {
        if (this->desktop) {
            subselChangedConn.disconnect();
            selectChangedConn.disconnect();
        }
        this->desktop = desktop;
        if (desktop && desktop->selection) {
            selectChangedConn = desktop->selection->connectChanged(sigc::hide(sigc::mem_fun(*this, &ObjectProperties::widget_setup)));
            subselChangedConn = desktop->connectToolSubselectionChanged(sigc::hide(sigc::mem_fun(*this, &ObjectProperties::widget_setup)));
        }
        widget_setup();
    }
}
}
}
}

using Inkscape::DocumentUndo;

static void sp_item_dialog_destroy( GtkObject */*object*/, gpointer /*data*/ )
{
    sp_signal_disconnect_by_data (INKSCAPE, dlg);
    wd.win = dlg = NULL;
    wd.stop = 0;
}
static gboolean sp_item_dialog_delete( GtkObject */*object*/, GdkEvent */*event*/, gpointer /*data*/ )
{
    gtk_window_get_position ((GtkWindow *) dlg, &x, &y);
    gtk_window_get_size ((GtkWindow *) dlg, &w, &h);

    if (x<0) x=0;
    if (y<0) y=0;

    Inkscape::Preferences *prefs = Inkscape::Preferences::get();
    prefs->setInt(prefs_path + "x", x);
    prefs->setInt(prefs_path + "y", y);
    prefs->setInt(prefs_path + "w", w);
    prefs->setInt(prefs_path + "h", h);
/***************************************
        ISAC features - save last tab page
        nbtype: RECT, TEXT, GROUP, OTHERS
        nbpage: the # page/ # tab
***************************************/
    sage_dialog_delete(prefs, prefs_path);

    //ISAC FEATURES END

    return FALSE; // which means, go ahead and destroy it
}
static void sp_item_widget_modify_selection( SPWidget *spw, Inkscape::Selection *selection, guint /*flags*/, GtkWidget */*itemw*/ )
{
    sp_item_widget_setup (spw, selection);
}
static void sp_item_widget_change_selection ( SPWidget *spw, Inkscape::Selection *selection, GtkWidget */*itemw*/ )
{
    sp_item_widget_setup (spw, selection);
}
static void sp_item_widget_sensitivity_toggled (GtkWidget *widget, SPWidget *spw)
{
    if (g_object_get_data (G_OBJECT (spw), "blocked"))
        return;

    SPItem *item = sp_desktop_selection(SP_ACTIVE_DESKTOP)->singleItem();
    g_return_if_fail (item != NULL);

    gtk_object_set_data (GTK_OBJECT (spw), "blocked", GUINT_TO_POINTER (TRUE));

    item->setLocked(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)));

    DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
                       gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))? _("Lock object") : _("Unlock object"));

    gtk_object_set_data (GTK_OBJECT (spw), "blocked", GUINT_TO_POINTER (FALSE));
}
void sp_item_widget_hidden_toggled(GtkWidget *widget, SPWidget *spw)
{
    if (g_object_get_data (G_OBJECT (spw), "blocked"))
        return;

    SPItem *item = sp_desktop_selection(SP_ACTIVE_DESKTOP)->singleItem();
    g_return_if_fail (item != NULL);

    gtk_object_set_data (GTK_OBJECT (spw), "blocked", GUINT_TO_POINTER (TRUE));

    item->setExplicitlyHidden(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

    DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
                       gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))? _("Hide object") : _("Unhide object"));

    gtk_object_set_data (GTK_OBJECT (spw), "blocked", GUINT_TO_POINTER (FALSE));
}
void sp_item_widget_label_changed( GtkWidget */*widget*/, SPWidget *spw )
{
    if (g_object_get_data (G_OBJECT (spw), "blocked"))
        return;

    SPItem *item = sp_desktop_selection(SP_ACTIVE_DESKTOP)->singleItem();
    g_return_if_fail (item != NULL);

    gtk_object_set_data (GTK_OBJECT (spw), "blocked", GUINT_TO_POINTER (TRUE));

    /* Retrieve the label widget for the object's id */
    GtkWidget *id_entry = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "id"));
    gchar *id = (gchar *) gtk_entry_get_text (GTK_ENTRY (id_entry));
    g_strcanon (id, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.:", '_');
    GtkWidget *id_label = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "id_label"));
    if (!strcmp (id, item->getId())) {
        gtk_label_set_markup_with_mnemonic (GTK_LABEL (id_label), _("_ID:"));
    } else if (!*id || !isalnum (*id)) {
        gtk_label_set_text (GTK_LABEL (id_label), _("Id invalid! "));
    } else if (SP_ACTIVE_DOCUMENT->getObjectById(id) != NULL) {
        gtk_label_set_text (GTK_LABEL (id_label), _("Id exists! "));
    } else {
        SPException ex;
        gtk_label_set_markup_with_mnemonic (GTK_LABEL (id_label), _("_ID:"));
        SP_EXCEPTION_INIT (&ex);
        item->setAttribute("id", id, &ex);
        DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
			               _("Set object ID"));
    }

    /* Retrieve the label widget for the object's label */
    GtkWidget *label_entry = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "label"));
    gchar *label = (gchar *)gtk_entry_get_text (GTK_ENTRY (label_entry));
    g_assert(label != NULL);

    /* Give feedback on success of setting the drawing object's label
     * using the widget's label text
     */
    SPObject *obj = (SPObject*)item;
    if (strcmp (label, obj->defaultLabel())) {
        obj->setLabel(label);
        DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
                           _("Set object label"));
    }

    /* Retrieve the title */
    GtkWidget *w = GTK_WIDGET(g_object_get_data(G_OBJECT(spw), "title"));
    gchar *title = (gchar *)gtk_entry_get_text(GTK_ENTRY (w));
    if (obj->setTitle(title))
        DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
			               _("Set object title"));

    /* Retrieve the description */
    GtkTextView *tv = GTK_TEXT_VIEW(g_object_get_data(G_OBJECT(spw), "desc"));
    GtkTextBuffer *buf = gtk_text_view_get_buffer(tv);
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buf, &start, &end);
    gchar *desc = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
    if (obj->setDesc(desc))
        DocumentUndo::done(SP_ACTIVE_DOCUMENT, SP_VERB_DIALOG_ITEM,
			               _("Set object description"));
    g_free(desc);

    gtk_object_set_data (GTK_OBJECT (spw), "blocked", GUINT_TO_POINTER (FALSE));

}
void sp_item_dialog (void)
{
/**
 * \brief  Dialog
 *
 */
    if (dlg == NULL) {

        gchar title[500];
        sp_ui_dialog_title_string (Inkscape::Verb::get(SP_VERB_DIALOG_ITEM), title);
        Inkscape::Preferences *prefs = Inkscape::Preferences::get();

        dlg = sp_window_new (title, TRUE);
        if (x == -1000 || y == -1000) {
            x = prefs->getInt(prefs_path + "x", -1000);
            y = prefs->getInt(prefs_path + "y", -1000);
        }
        if (w ==0 || h == 0) {
            w = prefs->getInt(prefs_path + "w", 0);
            h = prefs->getInt(prefs_path + "h", 0);
        }

//        if (x<0) x=0;
//        if (y<0) y=0;

/****************************************
    ISAC Features: 
    Shall always reinitialized the windows to the 
    predefined windows size, thus commented the original settings below.
****************************************/
        
        w = ISAC_DEF_WIN_WIDTH;  //ISAC Changed: predefined window's width
        h = ISAC_DEF_WIN_HEIGHT; //ISAC changed: predefined window's height
		
        if (w && h) {
            gtk_window_resize ((GtkWindow *) dlg, w, h);
        }
        if (x >= 0 && y >= 0 && (x < (gdk_screen_width()-MIN_ONSCREEN_DISTANCE)) && (y < (gdk_screen_height()-MIN_ONSCREEN_DISTANCE))) {
            gtk_window_move ((GtkWindow *) dlg, x, y);
        } else {
            gtk_window_set_position(GTK_WINDOW(dlg), GTK_WIN_POS_CENTER);
        }


        sp_transientize (dlg);
        wd.win = dlg;
        wd.stop = 0;

        g_signal_connect   ( G_OBJECT (INKSCAPE), "activate_desktop", G_CALLBACK (sp_transientize_callback), &wd);
        gtk_signal_connect ( GTK_OBJECT (dlg), "event", GTK_SIGNAL_FUNC (sp_dialog_event_handler), dlg);
        gtk_signal_connect ( GTK_OBJECT (dlg), "destroy", G_CALLBACK (sp_item_dialog_destroy), dlg);
        gtk_signal_connect ( GTK_OBJECT (dlg), "delete_event", G_CALLBACK (sp_item_dialog_delete), dlg);
        g_signal_connect   ( G_OBJECT (INKSCAPE), "shut_down", G_CALLBACK (sp_item_dialog_delete), dlg);
        g_signal_connect   ( G_OBJECT (INKSCAPE), "dialogs_hide", G_CALLBACK (sp_dialog_hide), dlg);
        g_signal_connect   ( G_OBJECT (INKSCAPE), "dialogs_unhide", G_CALLBACK (sp_dialog_unhide), dlg);

        // Dialog-specific stuff
        GtkWidget *itemw = sp_item_widget_new ();
        gtk_widget_show (itemw);
        gtk_container_add (GTK_CONTAINER (dlg), itemw);

    }

    gtk_window_present ((GtkWindow *) dlg);
}

GtkWidget *sp_item_widget_new (void)
{
/**
 * \brief  Creates new instance of item properties widget
 *
 */

    GtkWidget *spw, *vb, *t, *cb, *l, *f, *tf, *pb, *int_expander, *int_label;
    GtkTextBuffer *desc_buffer;

    GtkTooltips *tt = gtk_tooltips_new();

    /* Create container widget */
    spw = sp_widget_new_global (INKSCAPE);
    gtk_signal_connect ( GTK_OBJECT (spw), "modify_selection",
                         GTK_SIGNAL_FUNC (sp_item_widget_modify_selection),
                         spw );
    gtk_signal_connect ( GTK_OBJECT (spw), "change_selection",
                         GTK_SIGNAL_FUNC (sp_item_widget_change_selection),
                         spw );

    vb = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (spw), vb);

    t = gtk_table_new (3, 4, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(t), 4);
    gtk_table_set_row_spacings (GTK_TABLE (t), 4);
    gtk_table_set_col_spacings (GTK_TABLE (t), 4);
    gtk_box_pack_start (GTK_BOX (vb), t, TRUE, TRUE, 0);


    /* Create the label for the object id */
    l = gtk_label_new_with_mnemonic (_("_ID:"));
    gtk_misc_set_alignment (GTK_MISC (l), 1, 0.5);
    gtk_table_attach ( GTK_TABLE (t), l, 0, 1, 0, 1,
                       (GtkAttachOptions)( GTK_SHRINK | GTK_FILL ),
                       (GtkAttachOptions)0, 0, 0 );
    gtk_object_set_data (GTK_OBJECT (spw), "id_label", l);

    /* Create the entry box for the object id */
    tf = gtk_entry_new ();
    gtk_tooltips_set_tip (tt, tf, _("The id= attribute (only letters, digits, and the characters .-_: allowed)"), NULL);
    gtk_entry_set_max_length (GTK_ENTRY (tf), 64);
    gtk_table_attach ( GTK_TABLE (t), tf, 1, 2, 0, 1,
                       (GtkAttachOptions)( GTK_EXPAND | GTK_FILL ),
                       (GtkAttachOptions)0, 0, 0 );
    gtk_object_set_data (GTK_OBJECT (spw), "id", tf);
    gtk_label_set_mnemonic_widget (GTK_LABEL(l), tf);

    // pressing enter in the id field is the same as clicking Set:
    g_signal_connect ( G_OBJECT (tf), "activate", G_CALLBACK (sp_item_widget_label_changed), spw);
    // focus is in the id field initially:
    gtk_widget_grab_focus (GTK_WIDGET (tf));

    /* Create the label for the object label */
    l = gtk_label_new_with_mnemonic (_("_ISAC:"));
    gtk_misc_set_alignment (GTK_MISC (l), 1, 0.5);
   // gtk_table_attach ( GTK_TABLE (t), l, 0, 1, 1, 2,
     //                  (GtkAttachOptions)( GTK_SHRINK | GTK_FILL ),
       //                (GtkAttachOptions)0, 0, 0 );
    gtk_object_set_data (GTK_OBJECT (spw), "label_label", l);

    /* Create the entry box for the object label */
    tf = gtk_entry_new ();
    gtk_widget_set_sensitive (GTK_WIDGET (tf), FALSE);
	gtk_tooltips_set_tip (tt, tf, _("ISAC syntax"), NULL); //ISAC Changed on the tooltip for "ISAC" label
    //gtk_entry_set_max_length (GTK_ENTRY (tf), 256);
    //gtk_table_attach ( GTK_TABLE (t), tf, 1, 2, 1, 2,
      //                 (GtkAttachOptions)( GTK_EXPAND | GTK_FILL ),
        //               (GtkAttachOptions)0, 0, 0 );
    gtk_object_set_data (GTK_OBJECT (spw), "label", tf);
    gtk_label_set_mnemonic_widget (GTK_LABEL(l), tf);

    // pressing enter in the label field is the same as clicking Set:
    //g_signal_connect ( G_OBJECT (tf), "activate", G_CALLBACK (sp_item_widget_label_changed), spw);


    /* Create the frame for interactivity options */
    int_label = gtk_label_new_with_mnemonic (_("_Interactivity"));
    int_expander = gtk_expander_new (NULL);
    gtk_expander_set_label_widget (GTK_EXPANDER (int_expander),int_label);
    gtk_object_set_data (GTK_OBJECT (spw), "interactivity", int_expander);
    //gtk_box_pack_start (GTK_BOX (vb), int_expander, FALSE, FALSE, 0);

/*************************************
      ISAC Features:
      Initializing List
      These list are using for multiple entry field rows in ISAC menu.
      It is necessary to free these list before re-initialize it for 
      repeatitive use or reopen of windows. 
*************************************/
    isac_menu_init(GTK_TABLE(t), GTK_OBJECT(spw));
    sage_gui();
    
    gtk_widget_show_all (spw);

    sp_item_widget_setup (SP_WIDGET (spw), sp_desktop_selection (SP_ACTIVE_DESKTOP));

    return (GtkWidget *) spw;

} 
static void sp_item_widget_setup ( SPWidget *spw, Inkscape::Selection *selection )
{
/**
*  \param selection Selection to use; should not be NULL.
*/
    g_assert (selection != NULL);

    if (g_object_get_data (G_OBJECT (spw), "blocked"))
        return;

    if (!selection->singleItem()) {
        gtk_widget_set_sensitive (GTK_WIDGET (spw), FALSE);
        return;
    } else {
        gtk_widget_set_sensitive (GTK_WIDGET (spw), TRUE);
    }

    gtk_object_set_data (GTK_OBJECT (spw), "blocked", GUINT_TO_POINTER (TRUE));

    SPItem *item = selection->singleItem();

    /* Sensitive */
    GtkWidget *w = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "sensitive"));
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), item->isLocked());

    /* Hidden */
    w = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "hidden"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), item->isExplicitlyHidden());

    if (item->cloned) {

        /* ID */
        w = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "id"));
        gtk_entry_set_text (GTK_ENTRY (w), "");
        gtk_widget_set_sensitive (w, FALSE);
        w = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "id_label"));
        gtk_label_set_text (GTK_LABEL (w), _("Ref"));

        /* Label */
        w = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "label"));
        gtk_entry_set_text (GTK_ENTRY (w), "");
        gtk_widget_set_sensitive (w, FALSE);
        w = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "label_label"));
        gtk_label_set_text (GTK_LABEL (w), _("Ref"));

    } else {
        SPObject *obj = (SPObject*)item;

        /* ID */
        w = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "id"));
        gtk_entry_set_text (GTK_ENTRY (w), obj->getId());
        gtk_widget_set_sensitive (w, TRUE);
        w = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "id_label"));
        gtk_label_set_markup_with_mnemonic (GTK_LABEL (w), _("_ID:"));

        /* Label */
        w = GTK_WIDGET(g_object_get_data (G_OBJECT (spw), "label"));
        //gtk_entry_set_text (GTK_ENTRY (w), obj->defaultLabel());
        //gtk_widget_set_sensitive (w, TRUE);
		//ISAC Changed: if object doesnt have valid json syntax, leave it empty during initialization - by dennis*/
        const gchar *def = obj->defaultLabel();
		
        if(def[0]=='{') //ISAC Changed: If object has a valid json syntax, it shall start with "{"
        {
            gtk_entry_set_text (GTK_ENTRY (w), obj->defaultLabel());
            isac_change_mode(spw);
            isac_json_parse_syntax(def, spw);
        }
		
        else //ISAC Changed: If object has an invalid/empty syntax, set the ISAC label field to empty.
        {
            gtk_entry_set_text (GTK_ENTRY (w), "");
            isac_change_mode(spw);
        }

		gtk_widget_set_sensitive (w, TRUE);

        /* Title */
        w = GTK_WIDGET(g_object_get_data(G_OBJECT(spw), "title"));
        gchar *title = obj->title();
        if (title) {
            gtk_entry_set_text(GTK_ENTRY(w), title);
            g_free(title);
        }
        else gtk_entry_set_text(GTK_ENTRY(w), "");
        gtk_widget_set_sensitive(w, TRUE);

        /* Description */
        w = GTK_WIDGET(g_object_get_data(G_OBJECT(spw), "desc"));
        GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w));
        gchar *desc = obj->desc();
        if (desc) {
            gtk_text_buffer_set_text(buf, desc, -1);
            g_free(desc);
        } else {
            gtk_text_buffer_set_text(buf, "", 0);
        }
        w = GTK_WIDGET(g_object_get_data(G_OBJECT(spw), "desc_frame"));
        gtk_widget_set_sensitive(w, TRUE);

        w = GTK_WIDGET(g_object_get_data(G_OBJECT(spw), "interactivity"));

		/*
        GtkWidget* int_table = GTK_WIDGET(g_object_get_data(G_OBJECT(spw), "interactivity_table"));
        if (int_table){
            gtk_container_remove(GTK_CONTAINER(w), int_table);
        }

        const gchar* int_labels[10] = {"onclick", "onmouseover", "onmouseout", "onmousedown", "onmouseup", "onmousemove","onfocusin", "onfocusout", "onactivate", "onload"};

        int_table = sp_attribute_table_new (obj, 10, int_labels, int_labels);
        gtk_widget_show_all (int_table);
        gtk_object_set_data(GTK_OBJECT(spw), "interactivity_table", int_table);

        gtk_container_add (GTK_CONTAINER (w), int_table);
		*/

    }
    gtk_object_set_data (GTK_OBJECT (spw), "blocked", GUINT_TO_POINTER (FALSE));
}

// Buttons: Apply | OK | Cancel
void isac_apply_button (GtkWidget *widget, SPWidget *spw)
{
/***********************************************
    ISAC Features:
    Triggered when "Apply" button is clicked.
    Basically it will parse all the fields and compile 
    a ISAC syntax and display at the "ISAC" label.
    The window will stay open after button clicked.
***********************************************/
	color_deletelastrow(); // naim@ecava.com
    isac_update_syntax (widget, spw);
    sp_item_widget_label_changed (widget, spw);
}
void isac_ok_button (GtkWidget *widget, SPWidget *spw)
{
/***********************************************
    ISAC Features:
    Triggered when "Ok" button is clicked.
    Basically it will parse all the fields and compile 
    a ISAC syntax and display at the "ISAC" label.
    The window will close after button clicked.
***********************************************/
	color_deletelastrow(); // naim@ecava.com
    isac_update_syntax (widget, spw);
    sp_item_widget_label_changed (widget, spw);
    g_signal_emit_by_name ( GTK_OBJECT (dlg), "delete_event");
    g_signal_emit_by_name ( GTK_OBJECT (dlg), "destroy");
}
void isac_cancel_button (GtkWidget *widget, SPWidget *spw)
{
/***********************************************
    ISAC Features:
    Triggered when "Cancel" button is clicked.
    Basically it will just ignore any changes and
    window will close after button clicked.
***********************************************/
    g_signal_emit_by_name ( GTK_OBJECT (dlg), "delete_event");
    g_signal_emit_by_name ( GTK_OBJECT (dlg), "destroy");
}

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:fileencoding=utf-8:textwidth=99 :
