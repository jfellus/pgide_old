/*
 * CommandScriptSetProperty.h
 *
 *  Created on: 14 nov. 2014
 *      Author: jfellus
 */

#ifndef CommandScriptSetProperty_H_
#define CommandScriptSetProperty_H_

#include <commands/Command.h>
#include <module/Document.h>




using namespace libboiboites;
namespace pgide {



class CommandScriptSetProperty : public Command {
public:
	PromScript* script;
	std::string key;
	std::string value;
	std::string oldvalue;
public:
	CommandScriptSetProperty(PromScript* script, const std::string& key, const std::string& value) : script(script), key(key), value(value) {
		oldvalue = script->properties.get_as_string(key);
	}

	virtual ~CommandScriptSetProperty() {

	}

	virtual void execute() {
		script->properties.set(key, value);
		Document::cur()->fire_change_event();
	}

	virtual void undo() {
		script->properties.set(key, oldvalue);
		Document::cur()->fire_change_event();
	}

	virtual std::string get_text() {
		return TOSTRING("Script set " << key);
	}
};



}


#endif /* CommandScriptSetProperty_H_ */
