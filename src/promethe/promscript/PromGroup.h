/*
 * PromGroup.h
 *
 *  Created on: 18 oct. 2014
 *      Author: jfellus
 */

#ifndef PROMGROUP_H_
#define PROMGROUP_H_

#include <util/utils.h>
#include <module/Property.h>
#include "../library/ModulesLibrary.h"


using namespace libboiboites;
namespace pgide {




class PromScript;
class PromProject;

class PromGroup {
public:
	PromProject* project = NULL;

	std::string type;
	std::string name;

	Properties* properties;
	float posx = 0, posy = 0;

	PromScript* script;

	int time_scale; // TODO

public:
	PromGroup(PromScript* script, const std::string& type, const std::string& name);
	PromGroup(const PromGroup& g);

	virtual ~PromGroup();


	std::string get_type() {return type;}
	std::string get_name() {return name;}
	void set_type(const std::string& s) {type = s;}
	void set_name(const std::string& s) {name = s;}

	void set(const std::string& property, const std::string& value) {properties->set(property,value);}
	std::string get(const std::string& property) {return properties->get_as_string(property);}

	PromGroup* copy();


	virtual void dump(std::ostream& os) {
		os << "PromGroup(" << get_type() << "," << get_name() << ")";
	}

	friend std::ostream& operator<<(std::ostream& os, PromGroup* a);

};

std::ostream& operator<<(std::ostream& os, PromGroup* a);



}

#endif /* PROMGROUP_H_ */
