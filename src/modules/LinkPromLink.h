/*
 * LinkPromLink.h
 *
 *  Created on: 8 nov. 2014
 *      Author: jfellus
 */

#ifndef LINKPROMLINK_H_
#define LINKPROMLINK_H_


#include <module/Link.h>
#include "../promethe/promscript/PromLink.h"


using namespace libboiboites;
namespace pgide {


class ModulePromGroup;


class LinkPromLink : public Link, IPropertiesListener {
public:
	PromLink* link;
public:
	LinkPromLink(PromLink* link) {
		this->link = link;
		properties.add("type", link->type);
		properties.set("src", &link->src->name);
		properties.set("dst", &link->dst->name);

		add_properties_listener(this);
		realize();
	}

	virtual ~LinkPromLink();

	virtual void connect(ModulePromGroup* src, ModulePromGroup* dst);
	virtual void connect(Module* src, Module* dst);

	virtual void dump(std::ostream& os) {
		os << "LPL(" << link << ")";
	}

	virtual void on_property_change(IPropertiesElement* m, const std::string& name, const std::string& val) {
		if(name=="type") link->type = val;
	}


	virtual void on_change();

	virtual Link* copy();
	virtual void detach(bool bSlave = false);
	virtual void attach();

	void scale(float amount);

private:
	void realize();
	friend std::ostream& operator<<(std::ostream& os, LinkPromLink* a);
};

std::ostream& operator<<(std::ostream& os, LinkPromLink* a);


}



#endif /* LINKPROMLINK_H_ */

