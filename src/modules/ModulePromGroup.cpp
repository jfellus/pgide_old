/*
 * ModulePromGroup.cpp
 *
 *  Created on: 8 nov. 2014
 *      Author: jfellus
 */

#include "ModulePromGroup.h"
#include "../promethe/PromProject.h"

using namespace libboiboites;
namespace pgide {


class ComponentPromGroup : public ModuleComponent {
public:
	ComponentPromGroup(ModulePromGroup* mpg, const char* spec) : ModuleComponent(mpg, spec, mpg->text, mpg->text2) {

	}

	virtual ~ComponentPromGroup() {}

	virtual void render_infos(Graphics& g, bool hover) {
		ModuleComponentStyle* style = (ModuleComponentStyle*)this->style;

		double x = hover ? MAX(22,28/canvas->_zoom) : 22;
		double f = hover ? MAX(9,13/canvas->_zoom) : 9;
		g.set_font(f, style->font, style->font_style);

		render_noname(g, 0,0, hover);
		render_nbneurons(g, x, -5, hover);
	}

	void render_noname(Graphics& g, double x, double y, bool hover) {
		ModulePromGroup* mpg = (ModulePromGroup*)module;

		double r = hover ? MAX(10,15/canvas->_zoom) : 10;
		g.set_opacity(0.8);
		g.circle(x,y,r); g.fill(RGB(0,0,0.3));
		g.set_color(RGB_WHITE);
		g.text_center(mpg->group->name, Vector2D(x,y));
	}

	void render_nbneurons(Graphics& g, double x, double y, bool hover) {

	}
};








void ModulePromGroup::realize() {
	update_component();
	component->set_pos(group->posx, group->posy);
}

void ModulePromGroup::create_component(const char* component_spec) {
	component = new ComponentPromGroup(this, component_spec);
	component->set_selectable();
	component->set_user_data("Module", this);
	component->add_selection_listener(this);
	component->add_translate_listener(this);
	visible = true;
}


void ModulePromGroup::set_svg(const std::string& svg) {
	if(!component) create_component(svg.c_str());
	else ((SVGComponent*)component)->set(svg.c_str());
}

void ModulePromGroup::set_svg(const std::list<std::string>& svg_list) {
	for(auto i = svg_list.begin(); i!= svg_list.end(); i++) {
		try {
			set_svg(*i);
			return;
		} catch(...) {}
	}
}

void ModulePromGroup::update_component() {
	std::list<std::string> svgs;
	svgs.push_back(group->type);
	set_svg(svgs);

	component->remove_all_classes();

	component->add_class(group->type);

	PromWorkbench::cur()->update();
	PromWorkbench::cur()->canvas->repaint();
}


ModulePromGroup::~ModulePromGroup() {
	group->project->remove(this);
	if(group->script) group->script->remove_group(group);
	delete group;
}




std::ostream& operator<<(std::ostream& os, ModulePromGroup* a) {
	a->dump(os);
	return os;
}

void ModulePromGroup::detach(bool bSlave) {
	if(!bAttached) return;
	Module::detach(bSlave);
	if(group->script) group->script->remove_group(this->group);
	if(group->project) group->project->remove(this);
}

void ModulePromGroup::attach() {
	if(bAttached) return;
	Module::attach();
	if(group->script) group->script->add_group(this->group);
	if(group->project) group->project->add(this);
}

Module* ModulePromGroup::copy() {
	ModulePromGroup* m = new ModulePromGroup(group->copy());
	group->project->add(m);
	m->component->set_pos(component->x,component->y);
	m->attach();
	return m;
}



void ModulePromGroup::on_translate(double x, double y) {
	set_property("posx", TOSTRING((int)(x)));
	set_property("posy", TOSTRING((int)(y)));
	fire_change_event();
}

void ModulePromGroup::scale(double amount) {
	((ModuleComponent*)component)->scale((float)amount);
}

void ModulePromGroup::on_parent_change(Group* newparent) {
	Group* g = newparent;
	while(g && !dynamic_cast<GroupPromScript*>(g)) g = g->parent;
	GroupPromScript* gps = dynamic_cast<GroupPromScript*>(g);
	if(!gps) return;
	if(group->script) group->script->remove_group(group);
	std::string name = gps->script->new_name();
	gps->script->add_group(group);
	group->name = name;

	process_cross_script_links();
}


void ModulePromGroup::process_cross_script_links() {
	GroupPromScript* script_dst = get_script();
	GroupPromScript* script_src = get_script();
	for(uint i=0; i<in_links.size(); i++) {
		GroupPromScript* script_src = ((ModulePromGroup*)in_links[i]->src)->get_script();
		if(script_src == script_dst) continue;
		group->project->process_cross_script_link((LinkPromLink*)in_links[i]);
	}
	for(uint i=0; i<out_links.size(); i++) {
		GroupPromScript* script_dst = ((ModulePromGroup*)out_links[i]->dst)->get_script();
		if(script_src == script_dst) continue;
		group->project->process_cross_script_link((LinkPromLink*)out_links[i]);
	}
}

GroupPromScript* ModulePromGroup::get_script() {
	Group* g = parent;
	while(g && !dynamic_cast<GroupPromScript*>(g)) g = g->parent;
	return dynamic_cast<GroupPromScript*>(g);
}


void ModulePromGroup::on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val) {
	if(name=="type") {
		group->set_type(val);
		text = type = group->get_type();
		group->set_name(this->name);
		text2 = this->name = group->get_name();
		update_component();
	}
	else if(name=="name") {
		group->set_name(val);
		text2 = this->name = group->get_name();
	} else if(name=="posx" || name=="posy") {
		if(component) component->set_pos(group->posx, group->posy);
	} else if(name=="width" || name=="height") {
		update_component();
	}
}

}
