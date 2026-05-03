#include <SerpentLua.hpp>
#include <internal/SerpentLua.hpp>
#include <argon/argon.hpp>

using namespace geode::prelude;
using namespace SerpentLua;
using namespace SerpentLua::internal;

AuthManager* AuthManager::get() {
	static AuthManager* instance;
	if (!instance) instance = new (std::nothrow) AuthManager();
	return instance;
}

void AuthManager::authenticate(std::function<void(geode::Result<>)> callback) {
	async::spawn(
		argon::startAuth(),
		[callback, this](geode::Result<std::string> result) {
			if (result.isErr()) {
				callback(Err(result.unwrapErr()));
				return;
			}

			auto token = result.unwrap();

			this->token = token;

			callback(Ok());
		}
	);
}