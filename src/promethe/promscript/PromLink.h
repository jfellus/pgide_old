/*
 * PromLink.h
 *
 *  Created on: 18 oct. 2014
 *      Author: jfellus
 */

#ifndef PROMLINK_H_
#define PROMLINK_H_

#include "PromGroup.h"

using namespace libboiboites;
namespace pgide {


class PromScript;
class PromProject;

class PromLink {
public:
	PromProject* project = NULL;
	PromScript* script = NULL;
	PromGroup *src = NULL, *dst = NULL;
	std::string type;

	Properties properties;

public:
	PromLink(PromScript* script, PromGroup* src, PromGroup* dst, const std::string& type);
	PromLink(const PromLink& l);

	virtual ~PromLink();

	PromLink* copy();


	virtual void dump(std::ostream& os) {
		os << "PromLink(" << src << "-" << type << ">" << dst << ")";
	}
	friend std::ostream& operator<<(std::ostream& os, PromLink* a);

};

std::ostream& operator<<(std::ostream& os, PromLink* a);



}

#endif /* PROMLINK_H_ */
