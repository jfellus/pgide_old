/*
 * PromWorkbench.cpp
 *
 *  Created on: 22 oct. 2014
 *      Author: jfellus
 */

#include "PromWorkbench.h"
#include "../promethe/PromProject.h"
#include "../creators/PromGroupCreator.h"
#include "../creators/PromLinkCreator.h"
#include "../promethe/library/ModulesLibrary.h"
#include "../commands/CommandIsolateTag.h"
#include "../commands/CommandTagSelection.h"
#include "../commands/CommandTagSetCSS.h"
#include "../commands/CommandUnisolateTag.h"
#include "../commands/CommandUntagSelection.h"
#include "../commands/CommandCreateTag.h"
#include "../commands/CommandDeleteTag.h"
#include "../commands/CommandHideTag.h"
#include "../commands/CommandShowTag.h"
#include "../commands/CommandLockTag.h"
#include "../commands/CommandUnlockTag.h"
#include "../commands/CommandSetTagName.h"
#include "../commands/CommandScriptSetProperty.h"
#include "../commands/CommandBlurTag.h"
#include "../commands/CommandUnblurTag.h"
#include <commands/CommandPaste.h>
#include "../commands/CommandPasteExt.h"
#include "widgets/BugTracker.h"
#include <workbench/Job.h>
#include "components/LinkLinkComponent.h"
#include "../promethe/promscript/reader.h"

using namespace libboiboites;
namespace pgide {



static void on_scale_selection(double x, double y, double dx, double dy, void* data) {PromWorkbench::cur()->scale_selection(dy);}
static void on_open_recent_document(GtkMenuItem* i, void* param) {std::string s = *((std::string*)param); PromWorkbench::cur()->open(s); }
static void on_compile() {PromWorkbench::cur()->compile();}
static void _on_launch(GtkToggleButton *togglebutton, gpointer user_data) {if(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(togglebutton))) PromWorkbench::cur()->launch_project();}
static void _on_launch2() { PromWorkbench::cur()->launch_project();}
static void _on_launch_gui(GtkToggleButton *togglebutton, gpointer user_data) {if(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(togglebutton)))  PromWorkbench::cur()->launch_project(true);}
static void _on_stop() {PromWorkbench::cur()->stop_project(); }
static void on_edit() {PromWorkbench::cur()->edit();}
static void _on_create_one_to_one_link() {PromWorkbench::cur()->create_link("");}
static void on_display_links_norm()  {PromWorkbench::cur()->toggle_display_links_norm();}
static void on_display_links_name()  {PromWorkbench::cur()->toggle_display_links_name();}



PromWorkbench* PromWorkbench::cur() { return dynamic_cast<PromWorkbench*>(Workbench::cur()); }


PromWorkbench::PromWorkbench() {
	app_dir = TOSTRING(main_dir() << "/");
	application_name = "PGIDE";
	update_title();
	win->set_icon("pgide.png");

	f_read_lines(TOSTRING(home() << "/.pgide/recent.txt"),recent_documents);


	win->add_tab(properties = new PropertiesForm("src/js/properties_form.html"), "Properties");
	win->add_tab(docBrowser = new DocBrowser(), "Doc");
	win->add_tab(createForm = new CreateForm(), "Create");
	win->add_tab(tagsForm = new TagsForm(), "Tags");
	win->add_tab(bugTracker = new BugTracker(), "Bugs");

	update_recent_menu();

	win->enable_menu("_File>_Export script", false);

	win->add_menu("_View>Display links norm", on_display_links_norm);
	win->add_menu("_View>Display links name", on_display_links_name);


	win->add_toolbar("__");
	win->add_toolbar("edit", "style/icons/edit.gif", on_edit);
	win->add_toolbar("compile", "style/icons/compile.gif", on_compile);
	win->add_toolbar_toggle("launch", "style/icons/play.gif", _on_launch);
	win->add_toolbar_toggle("launch_gui", "style/icons/play_gui.gif", _on_launch_gui);
	win->add_toolbar("stop", "style/icons/stop.gif", _on_stop);


	canvas->add_key_listener(new IKeyListener(GDK_KEY_F5, 0, _on_launch2));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_F6, 0, _on_stop));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_c, 0, on_compile));
	canvas->add_key_listener(new IKeyListener(GDK_KEY_o, 0, _on_create_one_to_one_link));

	canvas->add_scroll_listener(new IScrollListener(GDK_CONTROL_MASK|GDK_SUPER_MASK, pgide::on_scale_selection, NULL));

	// Load config

	CSSDefinitions::add(resolve_path("style/basic.defs"));
	CSSDefinitions::add(resolve_path("style/test.css"));
	SVGDefinitions::add(resolve_path("style/svg"));

	ModulesLibrary::add_promethe_default_libraries();
}
void PromWorkbench::update_recent_menu() {
//	while(win->get_menu("_File>_Export script",1)!="__")
//		win->remove_menu("_File>_Export script",1);
//	for(uint i=0; i<MIN(9,recent_documents.size()); i++)
//		win->add_menu(TOSTRING("_File>_" << i+1 << " - " << recent_documents[i]).c_str(),
//				on_open_recent_document, &recent_documents[i], win->get_menu_pos("_File>_Export script")+i+2);
}


int _zoom_all(void* p) {
	PromWorkbench* w = (PromWorkbench*)p;
	w->canvas->zoom_all();
	return FALSE;
}
void PromWorkbench::do_open(const std::string& _filename) {
	canvas->OFF();

	std::string filename = _filename;
	if(project) do_close();
	project = new PromProject();

	vector_remove(recent_documents, filename);
	recent_documents.insert(recent_documents.begin(), filename);
	f_write_lines(TOSTRING(home() << "/.pgide/recent.txt"),recent_documents);
	update_recent_menu();

	DBG("Open " << filename);

	if(file_has_ext(filename, ".script")) {
		try {
			PromScriptReader r;
			PromScript* s = r.read_script(filename);
			project->set_script(s);
			document->open(filename);
			update_title();
		} catch(...) {}
	}  else {
		ERROR("Unknown file format " << filename);
	}
	document->update_links_layers();
	canvas->update_layers();
	canvas->ON();
	update();
	g_timeout_add(10, _zoom_all, this);
}

void PromWorkbench::save() {
	if(!project) return;
	if(!project->script) return;
	if(project->script->script->filename.empty()) PromWorkbench::cur()->save_as();
	else JOB_SUBMIT("Save", PromWorkbench::cur()->project->save());
}

void PromWorkbench::do_save(const std::string& filename) {
	if(!project) return;
	canvas->OFF();

	recent_documents.insert(recent_documents.begin(), filename);
	f_write_lines(TOSTRING(home() << "/.pgide/recent.txt"),recent_documents);

	project->save(filename);

	ERROR("Can't save - unknown extension : " << filename);
	canvas->ON();
	DBG("Saved !");
	update();
}




void PromWorkbench::save_script(const std::string& script_name) {
	GroupPromScript* gps = project->get_script_by_name(script_name);
	if(!gps) return;
	gps->script->save();
}


void PromWorkbench::do_new_document() {
	canvas->OFF();
	unselect_all();
	properties->reset();
	if(project) {delete project;	project = 0;}
	canvas->clear();

	STATUS("New network");

	project= new PromProject();
	project->set_script(new PromScript());
	document->new_document();

//	canvas->zoom_reset();
	document->update_links_layers();
	canvas->update_layers();
	canvas->ON();
	canvas->repaint();
	update_title();
	update();
}

void PromWorkbench::do_close() {
	do_new_document();
}


void PromWorkbench::create_module() {
	canvas->start_creator(new PromGroupCreator(project));
}

void PromWorkbench::create_module(const std::string& nametype) {
	canvas->start_creator(new PromGroupCreator(project, nametype));
}

void PromWorkbench::create_link(const std::string& type) {
	canvas->start_creator(new PromLinkCreator(project, type));
}


void PromWorkbench::do_update() {
	Workbench::do_update();
	docBrowser->update(get_selected_modules());
	tagsForm->update();
}


void PromWorkbench::compile() {
	if(!project) return;
	if(project->script->script->filename.empty()) {
		if(question("You must save your project first. Do you wish to do it now ?")) save_as();
		else return;
	} else save();
	if(project->script->script->filename.empty()) return;
	JOB_SUBMIT("Compile", {PromWorkbench::cur()->project->compile();});
}






void PromWorkbench::on_selection_change() {
	Workbench::on_selection_change();
	win->enable_menu("_File>_Export script", dynamic_cast<GroupPromScript*>(get_single_selected_module())!=0 );
}


void PromWorkbench::scale_selection(double amount) {
	amount=1-amount*0.04;
	for(uint i=0; i<get_selected_modules()->size(); i++) {
		ModulePromGroup* m = dynamic_cast<ModulePromGroup*>((*get_selected_modules())[i]);
		if(!m) continue;
		m->scale(amount);
	}
	for(uint i=0; i<get_selected_links()->size(); i++) {
		LinkPromLink* l = dynamic_cast<LinkPromLink*>((*get_selected_links())[i]);
		if(!l) continue;
		l->scale(amount);
	}
}


void PromWorkbench::tag_selection(const std::string& tagname) {
	(new CommandTagSelection(Document::cur(),tagname))->execute();
}

void PromWorkbench::untag_selection(const std::string& tagname) {
	(new CommandUntagSelection(Document::cur(),tagname))->execute();
}



void PromWorkbench::isolate_tag(const std::string& tagname, bool toggle) {
	Tag* t = Tags::get(tagname);
	if(!t) {ERROR("Tag " << tagname << " doesn't exist"); return;}
	if(t==Tags::isolated && toggle) (new CommandUnisolateTag())->execute();
	else (new CommandIsolateTag(Document::cur(), tagname))->execute();
}

void PromWorkbench::unisolate_tag() {
	if(Tags::isolated == NULL) return;
	(new CommandUnisolateTag())->execute();
}

void PromWorkbench::tag_set_css(const std::string& tagname, const std::string& css) {
	(new CommandTagSetCSS(tagname, css))->execute();
}

void PromWorkbench::add_tag(const std::string& tagname) {
	if(Tags::get(tagname)) {ERROR("Tag " << tagname << " already exists"); return;}
	(new CommandCreateTag(tagname))->execute();
}

void PromWorkbench::remove_tag(const std::string& tagname) {
	if(!Tags::get(tagname)) {ERROR("Tag " << tagname << " doesn't exist"); return;}
	(new CommandDeleteTag(tagname))->execute();
}

void PromWorkbench::show_tag(const std::string& tagname) {
	Tag* t = Tags::get(tagname);
	if(!t) {ERROR("Tag " << tagname << " doesn't exist"); return;}
	if(t->bVisible) return;
	(new CommandShowTag(tagname))->execute();
}

void PromWorkbench::hide_tag(const std::string& tagname) {
	Tag* t = Tags::get(tagname);
	if(!t) {ERROR("Tag " << tagname << " doesn't exist"); return;}
	if(!t->bVisible) return;
	(new CommandHideTag(tagname))->execute();
}

void PromWorkbench::blur_tag(const std::string& tagname) {
	Tag* t = Tags::get(tagname);
	if(!t) {ERROR("Tag " << tagname << " doesn't exist"); return;}
	if(t->bBlur) return;
	DBG("BLUR " << tagname);
	(new CommandBlurTag(Document::cur(),tagname))->execute();
}

void PromWorkbench::unblur_tag(const std::string& tagname) {
	Tag* t = Tags::get(tagname);
	if(!t) {ERROR("Tag " << tagname << " doesn't exist"); return;}
	if(!t->bBlur) return;
	DBG("UNBLUR " << tagname);
	(new CommandUnblurTag(Document::cur(),tagname))->execute();
}

void PromWorkbench::lock_tag(const std::string& tagname) {
	Tag* t = Tags::get(tagname);
	if(!t) {ERROR("Tag " << tagname << " doesn't exist"); return;}
	if(t->bLock) return;
	(new CommandLockTag(tagname))->execute();
}

void PromWorkbench::unlock_tag(const std::string& tagname) {
	Tag* t = Tags::get(tagname);
	if(!t) {ERROR("Tag " << tagname << " doesn't exist"); return;}
	if(!t->bLock) return;
	(new CommandUnlockTag(tagname))->execute();
}

void PromWorkbench::change_tag_name(const std::string& oldname, const std::string& newname) {
	Tag* t = Tags::get(oldname);
	if(!t) {ERROR("Tag " << oldname << " doesn't exist"); return;}
	(new CommandSetTagName(t,newname))->execute();
}



void PromWorkbench::set_script_property(const std::string& scriptname, const std::string& key, const std::string& value) {
	if(!project) return;
	PromScript* s = project->script->script;
	if(!s) {ERROR("Script " << scriptname << "doesn't exist"); return;}
	(new CommandScriptSetProperty(s,key,value))->execute();
}

GroupPromScript* PromWorkbench::get_script_at(double x, double y) {
	if(!project) return NULL;
	Module* g = Document::cur()->get_group_at(x,y);
	while(g!=NULL && !dynamic_cast<GroupPromScript*>(g)) { g = g->parent; }
	return dynamic_cast<GroupPromScript*>(g);
}

void PromWorkbench::launch_project(bool bGui) {
	if(!project) return;
}

void PromWorkbench::stop_project() {
	if(!project) return;
}

void PromWorkbench::compile_script(const std::string& script_name) {
	if(!project) return;
}

void PromWorkbench::launch_script(const std::string& script_name, bool bGui) {
	if(!project) return;
}

void PromWorkbench::stop_script(const std::string& script_name) {
	if(!project) return;
}



void PromWorkbench::cut() {
	Workbench::cut();
}

void PromWorkbench::copy() {
	Workbench::copy();
}

void PromWorkbench::paste() {
	Workbench::paste();
}




/////////////
// DISPLAY //
/////////////

void PromWorkbench::toggle_display_links_norm() {
	libboiboites::LinkComponentStyle::bText2_force = !libboiboites::LinkComponentStyle::bText2_force;
	canvas->repaint();
}

void PromWorkbench::toggle_display_links_name() {
	libboiboites::LinkComponentStyle::bText_force = !libboiboites::LinkComponentStyle::bText_force;
	canvas->repaint();
}


}
