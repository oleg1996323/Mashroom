#include "application.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <ranges>
#include "program/mashroom.h"
#include "cmd_parse/mashroom_parse.h"
#include "CLI/CLInavig.h"

static int progress_line = 6;

int main(int argc, char* argv[]){
    hProgram = std::make_unique<Mashroom>(Mashroom());
    CLIHandler::make_instance(Application::config().system_settings().system_files_dir,cli_history_filename);
    while(hProgram->read_command()){}
    return 0;
}