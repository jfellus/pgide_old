/*
 * BugTracker.h
 *
 *  Created on: 5 déc. 2014
 *      Author: jfellus
 */

#ifndef BUGTRACKER_H_
#define BUGTRACKER_H_

#include <widget/Browser.h>

using namespace libboiboites;
namespace pgide {


class BugTracker : public Browser {
public:
	BugTracker() : Browser("bugs") {
		open_web("https://github.com/jfellus/pgide/issues/new");
	}
	virtual ~BugTracker() {}
};


}


#endif /* BUGTRACKER_H_ */
