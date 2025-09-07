#include <internal/std/Playground.hpp>
#include <internal/std/PluginEntry.hpp>
#include <sstream>

using namespace SerpentLua::internal;
using namespace geode::prelude;

ScriptBuiltin::Playground::File ScriptBuiltin::Playground::File::create(const std::string& path, bool readOnly) {
    auto file = ScriptBuiltin::Playground::File();
    file.readOnly = readOnly;
    file.path = path;

    std::filesystem::path thePath = std::filesystem::path(path);
    if (std::filesystem::is_symlink(thePath)) {
        file.errs.push_back("playground does not allow symbolic links.");
        return file;
    }

    for (const auto& part : thePath) {
        if (part == "..") {
            file.errs.push_back("playground does not allow `..`.");
            return file;
        }
    }

    if (!std::filesystem::exists(path)) {
        std::ofstream the(path); // this auto creates it
        the.close(); // we do not need this file just yet
    }

    return file;
}

bool ScriptBuiltin::Playground::File::hasErrs() {
    return errs.empty();
}

std::string ScriptBuiltin::Playground::File::getErr(int index) {
    return errs[index - 1];
}

std::string ScriptBuiltin::Playground::File::read() {
    std::ifstream file(path);
    std::stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}

void ScriptBuiltin::Playground::File::write(const std::string& what) {
    if (readOnly) return; // i am limited by lua's lack of geode::Result and i dont want to wrap around it
    std::ofstream file(path);

    file << what;
}

void ScriptBuiltin::Playground::File::append(const std::string& what) {
    if (readOnly) return;
    // i was just gonna do write(read() + what) but apparently thats bad on memory
    std::ofstream file(path, std::ios::app);

    file << what;
}

sol::table ScriptBuiltin::Playground::entry(sol::state_view state) {
    auto table = state.create_table();

    table["init"] = [](sol::this_state ts) {
        sol::state_view state(ts);

        sol::table table = sol::stack::get<sol::table>(ts, 1);

        auto getprotected = ScriptBuiltin::mainModule["ScriptMetadata"]["get"]();
        sol::object get = getprotected;

        if (get == sol::nil) {
            log::error("Unable to retrieve script metadata.");
            return;
        }

        auto metadata = get.as<SerpentLua::ScriptMetadata*>();


        auto scriptDir = Mod::get()->getConfigDir() / "playground" / metadata->id;
        if (!std::filesystem::exists(scriptDir)) utils::file::createDirectoryAll(scriptDir).unwrap();

        auto saveDir = Mod::get()->getSaveDir() / "playground" / metadata->id;
        if (!std::filesystem::exists(saveDir)) utils::file::createDirectoryAll(saveDir).unwrap(); // shut up compiler

        auto scriptDirStr = scriptDir.string();
        auto saveDirStr = saveDir.string();

        table["script"] = sol::make_object(state, scriptDirStr);
        table["user"] = sol::make_object(state, saveDirStr);
    }; // creates the stuff if they dont exist and initialize variables

    table["get"] = [table](sol::this_state ts, std::string path) {

        std::size_t pos = path.find("://");
        if (pos != std::string::npos) {
            std::string before = path.substr(0, pos);
            std::string after = path.substr(pos + 3);
            return fmt::format("{}/{}", table[before].get<std::string>(), after);
        }

        return std::string("");
    };

    sol::table file_table = state.create_table();

    file_table.set_function("create", &ScriptBuiltin::Playground::File::create);

    table["File"] = file_table;

    state.new_usertype<ScriptBuiltin::Playground::File>("FileInstance", sol::no_constructor,
        "read", &File::read,
        "write", &File::write,
        "append", &File::append,
        "hasErrs", &File::hasErrs, // it looks like a staircase!
        "getErr", &File::getErr
    );

    return table;
}