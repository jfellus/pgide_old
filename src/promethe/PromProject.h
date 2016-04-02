/*
 * PromProject.h
 *
 *  Created on: 23 oct. 2014
 *      Author: jfellus
 */

#ifndef PROMPROJECT_H_
#define PROMPROJECT_H_

#include <creators/ModuleCreator.h>
#include <creators/LinkCreator.h>
#include "../workbench/PromWorkbench.h"
#include "../modules/ModulePromGroup.h"
#include "../modules/LinkPromLink.h"
#include "../modules/GroupPromScript.h"


using namespace libboiboites;
namespace pgide {


class PromProject {
public:
	std::vector<ModulePromGroup*> groups;
	std::vector<LinkPromLink*> links;
	GroupPromScript* script;

public:
	PromProject() {script = NULL;}
	~PromProject() {
		while(links.size()>0) delete links[0];
		while(groups.size()>0) delete groups[0];
	}

	void save(const std::string& filename = "");

	void set_script(PromScript* script);

	inline void add(GroupPromScript* s) {script = s;}
	inline void add(ModulePromGroup* g) {groups.push_back(g);}
	inline void add(LinkPromLink* l) {links.push_back(l);}
	inline void remove(ModulePromGroup* g) {vector_remove(groups, g);}
	inline void remove(LinkPromLink* l) {vector_remove(links, l);}
	inline void remove(GroupPromScript* s) {script = NULL;}

	ModulePromGroup* get(PromGroup* g);
	LinkPromLink* get(PromLink* l);
	GroupPromScript* get(PromScript* s);

	ModulePromGroup* get_group_by_name(const std::string& s);
	GroupPromScript* get_script_by_name(const std::string& s);



	uint get_scripts_count() { return script != NULL; }
	uint get_modules_count() { return groups.size(); }
	uint get_links_count() { return links.size(); }



	void compile();



	void create_timescales_groups(GroupPromScript* script);
	void layout_scripts();
	int infer_timescale(Group* g);

	void process_cross_script_link(LinkPromLink* link);

	GroupPromScript* get_owner_script(Module* m) {
		Group* g = dynamic_cast<Group*>(m);
		if(!g) g = m->parent;
		while(g && !dynamic_cast<GroupPromScript*>(g)) g = g->parent;
		return dynamic_cast<GroupPromScript*>(g);
	}
};











}


#endif /* PROMPROJECT_H_ */
