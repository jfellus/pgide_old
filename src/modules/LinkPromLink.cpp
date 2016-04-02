/*
 * LinkPromLink.cpp
 *
 *  Created on: 8 nov. 2014
 *      Author: jfellus
 */


#include "LinkPromLink.h"
#include "../promethe/PromProject.h"
#include <components/LinkLinkComponent.h>

using namespace libboiboites;
namespace pgide {




LinkPromLink::~LinkPromLink() {
	link->project->remove(this);
	if(link->script) link->script->remove_link(link);
	delete link;
}


void LinkPromLink::realize() {
	ModulePromGroup* s = link->project->get(link->src);
	if(!s) { ERROR("Unknown module : " << link->src); return;}
	ModulePromGroup* d = link->project->get(link->dst);
	if(!d) { ERROR("Unknown module : " << link->dst); return;}
	connect(s,d);
	if(link->type == "") add_class("seq");
	if(link->type == "-") add_class("sync");
	if(link->type == "0") add_class("clock");
	if(link->type == "/") add_class("async");

	if(link->properties.get("bezier_x2")) component->b->x2 = TOFLOAT(link->properties.get_as_string("bezier_x2"));
	if(link->properties.get("bezier_y2")) component->b->y2 = TOFLOAT(link->properties.get_as_string("bezier_y2"));
	if(link->properties.get("bezier_x3")) component->b->x3 = TOFLOAT(link->properties.get_as_string("bezier_x3"));
	if(link->properties.get("bezier_y3")) component->b->y3 = TOFLOAT(link->properties.get_as_string("bezier_y3"));
	if(!link->properties.get("bezier_x2") && src==dst) component->b->set(120,0,0,-120);
}

void LinkPromLink::connect(ModulePromGroup* src, ModulePromGroup* dst) {
	if(!src || !dst) { ERROR("Connection error"); return; }
	Link::connect(src,dst);
	if(link) {
		link->src = src->group;
		link->dst = dst->group;

		if(!link->properties.get("bezier_x2") && src==dst) component->b->set(1200,0,0,-1200);
	}
}

void LinkPromLink::connect(Module* src, Module* dst) {
	if(dynamic_cast<ModulePromGroup*>(src) && dynamic_cast<ModulePromGroup*>(dst))
		connect(dynamic_cast<ModulePromGroup*>(src),dynamic_cast<ModulePromGroup*>(dst));
	else Link::connect(src,dst);
}

std::ostream& operator<<(std::ostream& os, LinkPromLink* a) {
	a->dump(os);
	return os;
}


Link* LinkPromLink::copy() {
	LinkPromLink* l = new LinkPromLink(link->copy());
	return l;
}

void LinkPromLink::detach(bool bSlave) {
	if(!bAttached) return;
	Link::detach(bSlave);
	if(link->script) link->script->remove_link(this->link);
	if(link->project) link->project->remove(this);
}

void LinkPromLink::attach() {
	if(bAttached) return;
	Link::attach();
	if(link->script) link->script->add_link(this->link);
	if(link->project) link->project->add(this);
}

void LinkPromLink::scale(float amount) { ((LinkLinkComponent*)component)->scale(amount);}


void LinkPromLink::on_change() {
	link->properties.set("bezier_x2", TOSTRING(component->b->x2));
	link->properties.set("bezier_y2", TOSTRING(component->b->y2));
	link->properties.set("bezier_x3", TOSTRING(component->b->x3));
	link->properties.set("bezier_y3", TOSTRING(component->b->y3));
}


}
