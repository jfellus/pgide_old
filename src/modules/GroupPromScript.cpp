/*
 * GroupPromScript.cpp
 *
 *  Created on: 8 nov. 2014
 *      Author: jfellus
 */


#include "GroupPromScript.h"
#include <components/GroupComponent.h>
#include "../promethe/PromProject.h"
#include "../workbench/PromWorkbench.h"



using namespace libboiboites;
namespace pgide {



GroupPromScript::GroupPromScript(PromScript* script) : script(script) {
	project = script->project;
	set_property("name", script->name);
}


void GroupPromScript::realize() {
	create_component("script_closed");
	component->hide();
	component->add_class("script");

	component_open = new GroupOpenComponent(this);
	component_open->add_class("script");
	component_open->add_selection_listener(this);
	component_open->layer = -0.5;
}


GroupPromScript::~GroupPromScript() {
	project->remove(this);
	delete script;
}

void GroupPromScript::detach(bool bSlave) {
	if(!bAttached) return;
	Group::detach(bSlave);
	project->remove(this);
}

void GroupPromScript::attach() {
	if(bAttached) return;
	Group::attach();
	project->add(this);
}

void GroupPromScript::on_change() {
	if(!script) return;
	if(component_open) component_open->compute_handle_bounds();
}



}
