#include "sys/application.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <ranges>
#include "program/mashroom.h"
#include "cmd_parse/mashroom_parse.h"
#include "OsterLib/CLI/CLInavig.h"

static int progress_line = 6;

int main(int argc, char* argv[]){
    Application::app();
    parse::Mashroom mashroom(Mashroom::command_line());
    CLIHandler::make_instance(
        Application::config().system_config().cache_files_directory(),
        sys::history_filename());
    while(Mashroom::instance().read_command()){}
    return 0;
}