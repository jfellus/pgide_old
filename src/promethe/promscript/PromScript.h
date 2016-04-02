/*
 * PromScript.h
 *
 *  Created on: 18 oct. 2014
 *      Author: jfellus
 */

#ifndef PROMSCRIPT_H_
#define PROMSCRIPT_H_

#include <util/utils.h>
#include <exception>
#include <locale.h>
#include <fstream>
#include "PromGroup.h"
#include "PromLink.h"

using namespace libboiboites;
namespace pgide {


class Exception_GroupNotFound : public BasicException {
public:
	Exception_GroupNotFound(const std::string& g) : BasicException("Can't find group : ", g.c_str()) {}
};

class PromProject;

class PromScript {
public:
	std::vector<PromGroup*> groups;
	std::vector<PromLink*> links;
	std::vector<std::string> includes;
	std::vector<std::string> depends;


	Properties properties;

	std::string filename;
	std::string name;

	PromProject* project = 0;

public:
	PromScript() {init();}
	PromScript(const std::string& filename, const std::string& name) : filename(filename), name(name) {init();}
	virtual ~PromScript();

	void init() {	}

	void add_include(const std::string& include) {includes.push_back(include);}
	void add_depend(const std::string& dep) {depends.push_back(dep);}


	PromGroup* get_group_by_name(const std::string& nname) {
		for(uint i=0; i<groups.size(); i++) if(groups[i]->name == name) return groups[i];
		return NULL;
	}

	std::string new_name() {
		std::string no_name = "0";
		for(uint i=0; i<groups.size(); i++) {
			if(TOINT(groups[i]->name) >= TOINT(no_name)) no_name = TOSTRING(TOINT(groups[i]->name)+1);
		}
		return no_name;
	}

	void add_group(PromGroup* group) {
		groups.push_back(group); group->script = this;
	}

	void add_link(PromLink* link) {
		if(link->src==NULL || link->dst==NULL) return;
		links.push_back(link);
		link->script = this;
	}

	void remove_group(PromGroup* group, bool remove_associated_links = true) {
		vector_remove(groups, group);
		if(remove_associated_links)
			for(uint i=0; i<links.size(); i++) {
				if(links[i]->src == group || links[i]->dst == group) remove_link(links[i--]);
			}
	}

	void remove_link(PromLink* link) {
		vector_remove(links, link);
	}

	int get_highest_timescale() {
		int max = 0;
		for(uint i=0; i<groups.size(); i++) {
			int ts = TOINT(groups[i]->get("time_scale"));
			if(ts > max) max = ts;
		}
		return max;
	}

	void save();

	void save_as(const std::string& filename) {
		this->filename = filename;
		create_dir_for(filename);
		std::ofstream f(filename);
		if(!f) DBG("Failed creating file " << filename);
		if(!write(f)) DBG("Failed writing " << filename);
		f.close();
	}

	bool write(std::ostream& f) {
		try {
			f << "\n[Modules]\n\n";
			for(uint i=0; i<groups.size(); i++) {
				f << groups[i]->type << " " << groups[i]->name << "\n";
				for(uint j=0; j<groups[i]->properties->size(); j++) {
					f << (*groups[i]->properties)[j]->name << "=" << (*groups[i]->properties)[j]->get_value_as_string() << "\n";
				}
				f << "\n";
			}
			f<< "\n[Links]\n\n";
			for(uint i=0; i<links.size(); i++) {
				f << links[i]->src->name << "-" << links[i]->type << ">" << links[i]->dst->name << "\n";
			}

			return true;
		} catch(const char* s) { ERROR("Can't write " << s); return false;}
		catch(std::exception& s) { ERROR(s.what()); return false;}
	}

	virtual void dump(std::ostream& os) {
		os << "PromScript(" << name << " : " << groups.size() << " groups, " << links.size() << " links)";
	}
	friend std::ostream& operator<<(std::ostream& os, PromScript* a);


	void dump_groups() {
		for(uint i=0; i<groups.size(); i++) {
			DBG("\t" << groups[i]);
		}
	}

};

std::ostream& operator<<(std::ostream& os, PromScript* a);

}


#endif /* PROMSCRIPT_H_ */
