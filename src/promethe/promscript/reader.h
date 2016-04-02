/*
 * reader.h
 *
 *  Created on: 5 avr. 2015
 *      Author: jfellus
 */

#ifndef READER_H_
#define READER_H_

#include <util/utils.h>
#include <exception>
#include <locale.h>
#include <fstream>
#include "PromScript.h"
#include "PromGroup.h"
#include "PromLink.h"

namespace pgide {

class PromScriptReader {
public:
	std::string filename;
	PromScript* script;
	PromGroup* module;
	PromLink* link;

public:
	PromScriptReader() {script = 0; module = 0; link = 0;}

	PromScript* read_script(const std::string& filename) {
		this->filename = filename;
		std::ifstream f(filename.c_str());
		if(!f.good()) ERROR("No such script file : " << filename);
		PromScript* s = read_script(f);
		f.close();
		script = 0; module = 0; link = 0; this->filename = "";
		return s;
	}

	PromScript* read_script(std::ifstream& f) {
		std::string line;
		std::string section = "header";
		while (std::getline(f, line)) {
			line = str_trim(str_before(line, "#"));
			if(line.empty()) continue;
			if(line[0]=='[') section = str_to_lower(str_trim(str_between(line, "[", "]")));
			else if(section=="header") read_header_statement(line);
			else if(section=="modules") read_module_statement(line);
			else if(section=="links") read_links_statement(line);
		}

		return script;
	}

	void read_header_statement(const std::string& line) {
		if(str_starts_with(line, "Script")) script = new PromScript(filename, str_trim(str_after(line, "Script")));
		else if(str_starts_with(line, "Include")) script->add_include(str_trim(str_after(line, "Include")));
		else if(str_starts_with(line, "Depends")) script->add_depend(str_trim(str_after(line, "Depends")));
		else if(str_starts_with(line, "Timescale")) {
			std::string params = str_trim(str_after(line, "Timescale"));
//			int id = TOINT(str_trim(str_before(params, " ")));
//			size_t nb = TOINT(str_trim(str_before(str_after(params, " "), " ")).substr(1));
//			int parent = TOINT(str_trim(str_after(str_after(params, " "), " ")));
//			script->add_timescale(id, nb, parent);
		}
	}

	void read_module_statement(const std::string& line) {
		if(str_has(line, "=")) module->set(str_trim(str_before(line, "=")), str_trim(str_after(line, "=")));
		else module = create_module(str_trim(line));
	}

	void read_links_statement(const std::string& line) {
		if(str_has(line, "=")) {
			std::string key = str_trim(str_before(line, "="));
			std::string val = str_trim(str_after(line, "="));
			if(key=="type") link->type = val;
		} else link = create_link(str_trim(line));
	}

	PromGroup* create_module(const std::string& _spec) {
		std::string spec = _spec;
	//	bool bIsScript = spec[0]=='$';
	//	if(bIsScript) spec = spec.substr(1);
		std::string cls = str_trim(str_before(spec, " "));
		std::string id = str_trim(str_after(spec, " "));
		PromGroup* g = new PromGroup(script, cls,id);
		script->add_group(g);
		return g;
	}

	PromLink* create_link(const std::string& spec) {
		std::string ssrc = str_trim(str_before(spec, "-"));
		std::string sdst = str_trim(str_after(spec, ">"));
		std::string stype = str_trim(str_between(spec, "-", ">"));
		std::string srcpin = "";
		std::string dstpin = "";
		if(str_has(ssrc, ".")) { srcpin = str_trim(str_after(ssrc, ".")); ssrc = str_trim(str_before(ssrc, ".")); }
		if(str_has(sdst, ".")) { dstpin = str_trim(str_after(sdst, ".")); sdst = str_trim(str_before(sdst, ".")); }

		PromGroup* src = script->get_group_by_name(ssrc);
		PromGroup* dst = script->get_group_by_name(sdst);
		if(!src) ERROR("No such source module : " << ssrc);
		if(!dst) ERROR("No such source module : " << sdst);

		PromLink* l = new PromLink(script, src, dst, stype);
		script->add_link(l);
		return l;
	}

};



}


#endif /* READER_H_ */
