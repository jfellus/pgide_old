/*
 * PromGroup.cpp
 *
 *  Created on: 18 oct. 2014
 *      Author: jfellus
 */

#include "PromGroup.h"
#include "PromScript.h"
#include "../library/ModulesLibrary.h"
#include <util/utils.h>


using namespace libboiboites;
namespace pgide {


PromGroup::PromGroup(PromScript* script, const std::string& type, const std::string& name) {
	this->name = name;
	this->script = script;
	this->project = script->project;
	time_scale = 0;

	this->type = type;

//	ModuleDef* md = ModulesLibrary::get(type);
//	if(!md) throw TOSTRING("No such module definition : " << type);

	this->properties = new Properties();
}

PromGroup::PromGroup(const PromGroup& g) {
	time_scale = g.time_scale;
	project = g.script->project;
	script = g.script;
	type = g.type;
	name = g.name;
	posx = g.posx;
	posy = g.posy;
	properties = new Properties();
	for(uint i=0; i<g.properties->size(); i++) properties->add((*g.properties)[i]);
	script->add_group(this);
}

PromGroup::~PromGroup() {
	delete properties;
}


PromGroup* PromGroup::copy() {
	return new PromGroup(*this);
}


std::ostream& operator<<(std::ostream& os, PromGroup* a) {
	a->dump(os);
	return os;
}


}
