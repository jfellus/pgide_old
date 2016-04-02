/*
 * PromLink.cpp
 *
 *  Created on: 18 oct. 2014
 *      Author: jfellus
 */

#include "PromLink.h"
#include "PromScript.h"


using namespace libboiboites;
namespace pgide {



PromLink::PromLink(PromScript* script, PromGroup* src, PromGroup* dst, const std::string& type) {
	this->project = script->project;
	this->type = type;
	this->script = script;
	this->src = src;
	this->dst = dst;
}

PromLink::PromLink(const PromLink& l) {
	project = l.project;
	src = l.src;
	dst = l.dst;
	type = l.type;
	script = l.script;
}

PromLink::~PromLink() {

}


PromLink* PromLink::copy() {
	return new PromLink(*this);
}


std::ostream& operator<<(std::ostream& os, PromLink* a) {
	a->dump(os);
	return os;
}

}
