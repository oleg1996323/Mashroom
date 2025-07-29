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
    CLIHandler::make_instance(Application::config().system_settings().system_files_dir,cli_history_filename);
    while(Mashroom::instance().read_command()){}
    return 0;std::ranges::range_value_t<std::unordered_map<std::shared_ptr<CommonDataProperties>, std::vector<SublimedDataInfo>>> i{};
}

static_assert(requires{requires serialization::serialize_concept<std::vector<SublimedDataInfo>>;});
static_assert(requires{requires serialization::serialize_concept<std::pair<const std::shared_ptr<CommonDataProperties>, std::vector<SublimedDataInfo> >>;});
static_assert(requires{requires serialization::deserialize_concept<std::vector<SublimedDataInfo>>;});
static_assert(requires{requires serialization::deserialize_concept<std::pair<const std::shared_ptr<CommonDataProperties>, std::vector<SublimedDataInfo> >>;});