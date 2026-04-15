#include <internal/SerpentLua.hpp>
#include <sol/sol.hpp>

namespace SerpentLua::internal::ScriptBuiltin::ui {
    sol::table entry(sol::state_view state);

    struct Node {
        enum class Type {
            Node, // CCNode
            Sprite, // CCSprite
            Button, // CCMenuItemExt
            Label, // CCLabelBMFont
            Menu // CCMenu
        };
    };
}