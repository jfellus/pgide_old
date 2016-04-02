#include "workbench/PromWorkbench.h"
#include "promethe/PromProject.h"
#include <layout/LayoutFlow.h>
#include <dynamics/ModuleBody.h>
#include <style/CSSDefinitions.h>
#include <util/file.h>

using namespace libboiboites;
using namespace pgide;


int main(int argc, char** argv) {
	gtk_init(&argc, &argv);

	try {
	add_resources_path(TOSTRING(main_dir() << "/resources/"));

	PromWorkbench* w = new PromWorkbench();
	setlocale(LC_NUMERIC, "C");

	DBGV(main_dir());
	if(argc==2) w->open(file_absolute_path(argv[1]));
	else w->new_document();

	w->run();
	}
	catch(std::runtime_error& e) { DBG("ERROR " << e.what());}
//	catch(...) { DBG("FATAL ERROR"); }
	return 0;
}


