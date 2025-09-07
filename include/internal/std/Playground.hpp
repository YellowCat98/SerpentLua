#include <internal/SerpentLua.hpp>
#include <sol/sol.hpp>

namespace SerpentLua::internal::ScriptBuiltin::Playground {
    sol::table entry(sol::state_view state); 

    struct File final {
        static File create(const std::string& path, bool readOnly);

        std::string read();
        void write(const std::string& what);
        void append(const std::string& what);

        bool hasErrs();
        std::string getErr(int index); // yes this index starts at 1 so that we stay faithful to lua's shitty shit
        int errSize();
    private:
        std::vector<std::string> errs;
        std::string path; // i really dont want to wrap sol2 around std::filesystem::path
        bool readOnly;
    };
}