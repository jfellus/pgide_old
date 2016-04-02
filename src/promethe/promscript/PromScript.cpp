/*
 * PromScript.cpp
 *
 *  Created on: 18 oct. 2014
 *      Author: jfellus
 */

#include "PromScript.h"
#include "../../workbench/PromWorkbench.h"

using namespace libboiboites;
namespace pgide {


PromScript::~PromScript() {
	while(!links.empty()) { PromLink* g = links[0]; vector_remove(links, g); delete g; }
	while(!groups.empty()) { PromGroup* g = groups[0]; vector_remove(groups, g); delete g; }
}


std::ostream& operator<<(std::ostream& os, PromScript* a) {
	a->dump(os);
	return os;
}


void PromScript::save() {
	DBG("Save script " << name << " to " << filename);
	save_as(filename);
	POPUP("Saved script " << name << " to " << filename);
}


}
