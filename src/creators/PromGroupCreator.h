/*
 * PromGroupCreator.h
 *
 *  Created on: 13 nov. 2014
 *      Author: jfellus
 */

#ifndef PROMGROUPCREATOR_H_
#define PROMGROUPCREATOR_H_

#include <creator/Creator.h>
#include "../promethe/PromProject.h"
#include "../commands/CommandPromGroupCreate.h"


using namespace libboiboites;
namespace pgide {


class PromGroupCreator : public ModuleCreator {
public:
	PromProject* project;
	std::string type;
public:
	PromGroupCreator(PromProject* project);
	PromGroupCreator(PromProject* project, const std::string& type);
	virtual ~PromGroupCreator() {}


	virtual void create(double x, double y) {
		PromGroup* promGroup = new PromGroup(project->script->script, type, "new");
		promGroup->posx = (int)(x);
		promGroup->posy = (int)(y);

		ModulePromGroup* m = new ModulePromGroup(promGroup);
		(new CommandPromGroupCreate(project,m))->execute();
		module = m;
		comp = module->component;
		comp->set_pos(x,y);
		end();
	}

	virtual void end() {
		ModulePromGroup* m = dynamic_cast<ModulePromGroup*>(module);
		if(m) project->script->script->add_group(m->group);
		ModuleCreator::end();
	}
};


}


#endif /* PROMGROUPCREATOR_H_ */
