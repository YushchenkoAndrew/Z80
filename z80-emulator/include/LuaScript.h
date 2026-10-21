#pragma once
// LuaScript - small RAII C++17 wrapper around the Lua 5.4 C API.
//
//   LuaScript lua;
//   lua.DoFile("config.lua");
//
//   int   w    = lua.GetOr("window.width", 800);          // nested path
//   auto  name = lua.Get<std::string>("window.title");    // std::optional<T>
//   auto  ids  = lua.Get<std::vector<int>>("levels");     // arrays
//   auto  cols = lua.Get<std::map<std::string,int>>("colors");
//   float x    = lua.GetOr("items.2.pos.x", 0.0f);        // numeric segment = array index (1-based)
//
//   lua.Set("game.level", 3);
//   lua.Register("log", [](const std::string& s) { puts(s.c_str()); });
//   int r = lua.Call<int>("add", 1, 2).value_or(-1);      // call a Lua function
//   lua.CallMethod("player", "update", 0.016f);           // player:update(0.016)
//
// Rules of thumb:
//  * The Lua stack is always balanced after every public call (no leaks / no state).
//  * Nothing here throws, except Require<T>() and exceptions thrown by your own
//    registered C++ functions (these are caught and turned into Lua errors).
//  * Type checks are strict: Get<int>("x") fails for a string "5" or for 1.5.

#include <charconv>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <limits>
#include <map>
#include <new>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

extern "C" {
#include "../lua54/include/lua.h"
#include "../lua54/include/lauxlib.h"
#include "../lua54/include/lualib.h"
}

// Link to lua library
#ifdef _WIN32
#pragma comment(lib, "../lua54/win32/liblua54.a")
#endif // _WIN32

#ifdef __linux__
#pragma comment(lib, "../lua54/linux/liblua54.a")
#endif // __linux__

#ifdef __EMSCRIPTEN__
#pragma comment(lib, "../lua54/emsc/liblua54.a")
#endif // __EMPSCRIPTEN__


// ---------------------------------------------------------------------------
// Conversions C++ <-> Lua. Add your own by specializing luascript::Conv<T>
// ---------------------------------------------------------------------------
namespace luascript {

template <class T, class = void> struct Conv; // Push(L, v) -> int, Get(L, idx) -> optional<T>

template <> struct Conv<bool> {
	static int Push(lua_State* L, bool v) { lua_pushboolean(L, v); return 1; }
	static std::optional<bool> Get(lua_State* L, int i) {
		if (lua_type(L, i) != LUA_TBOOLEAN) return std::nullopt;
		return lua_toboolean(L, i) != 0;
	}
};

template <class T>
struct Conv<T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>> {
	static int Push(lua_State* L, T v) { lua_pushinteger(L, static_cast<lua_Integer>(v)); return 1; }
	static std::optional<T> Get(lua_State* L, int i) {
		if (lua_type(L, i) != LUA_TNUMBER) return std::nullopt;
		int ok = 0;
		lua_Integer v = lua_tointegerx(L, i, &ok); // fails for 1.5, accepts 3.0
		if (!ok) return std::nullopt;
		if constexpr (std::is_unsigned_v<T>) { if (v < 0) return std::nullopt; }
		if constexpr (sizeof(T) < sizeof(lua_Integer)) {
			if (v < static_cast<lua_Integer>(std::numeric_limits<T>::min()) ||
			    v > static_cast<lua_Integer>(std::numeric_limits<T>::max()))
				return std::nullopt;
		}
		return static_cast<T>(v);
	}
};

template <class T>
struct Conv<T, std::enable_if_t<std::is_floating_point_v<T>>> {
	static int Push(lua_State* L, T v) { lua_pushnumber(L, static_cast<lua_Number>(v)); return 1; }
	static std::optional<T> Get(lua_State* L, int i) {
		if (lua_type(L, i) != LUA_TNUMBER) return std::nullopt;
		return static_cast<T>(lua_tonumber(L, i));
	}
};

template <> struct Conv<std::string> {
	static int Push(lua_State* L, const std::string& v) { lua_pushlstring(L, v.data(), v.size()); return 1; }
	static std::optional<std::string> Get(lua_State* L, int i) {
		if (lua_type(L, i) != LUA_TSTRING) return std::nullopt; // no number->string coercion
		size_t len = 0;
		const char* s = lua_tolstring(L, i, &len);
		return std::string(s, len);
	}
};

// Push-only helpers (reading them back would dangle)
template <> struct Conv<std::string_view> {
	static int Push(lua_State* L, std::string_view v) { lua_pushlstring(L, v.data(), v.size()); return 1; }
};
template <> struct Conv<const char*> {
	static int Push(lua_State* L, const char* v) { if (v) lua_pushstring(L, v); else lua_pushnil(L); return 1; }
};
template <> struct Conv<std::nullptr_t> {
	static int Push(lua_State* L, std::nullptr_t) { lua_pushnil(L); return 1; }
};

// optional<U>: nil <-> empty optional
template <class U> struct Conv<std::optional<U>> {
	static int Push(lua_State* L, const std::optional<U>& v) {
		if (!v) { lua_pushnil(L); return 1; }
		return Conv<U>::Push(L, *v);
	}
	static std::optional<std::optional<U>> Get(lua_State* L, int i) {
		if (lua_isnoneornil(L, i)) return std::optional<U>{};
		auto v = Conv<U>::Get(L, i);
		if (!v) return std::nullopt;
		return std::optional<U>(std::move(*v));
	}
};

// std::vector<U> <-> Lua array
template <class U, class A> struct Conv<std::vector<U, A>> {
	static int Push(lua_State* L, const std::vector<U, A>& v) {
		lua_createtable(L, static_cast<int>(v.size()), 0);
		lua_Integer n = 1;
		for (const auto& e : v) { Conv<U>::Push(L, e); lua_rawseti(L, -2, n++); }
		return 1;
	}
	static std::optional<std::vector<U, A>> Get(lua_State* L, int i) {
		if (lua_type(L, i) != LUA_TTABLE || !lua_checkstack(L, 4)) return std::nullopt;
		i = lua_absindex(L, i);
		std::vector<U, A> out;
		const lua_Unsigned n = lua_rawlen(L, i);
		out.reserve(static_cast<size_t>(n));
		for (lua_Unsigned k = 1; k <= n; ++k) {
			lua_rawgeti(L, i, static_cast<lua_Integer>(k));
			auto e = Conv<U>::Get(L, -1);
			lua_pop(L, 1);
			if (!e) return std::nullopt;
			out.push_back(std::move(*e));
		}
		return out;
	}
};

// std::map<std::string,U> <-> Lua table with string keys
template <class U, class C, class A> struct Conv<std::map<std::string, U, C, A>> {
	using M = std::map<std::string, U, C, A>;
	static int Push(lua_State* L, const M& m) {
		lua_createtable(L, 0, static_cast<int>(m.size()));
		for (const auto& [k, v] : m) {
			lua_pushlstring(L, k.data(), k.size());
			Conv<U>::Push(L, v);
			lua_rawset(L, -3);
		}
		return 1;
	}
	static std::optional<M> Get(lua_State* L, int i) {
		if (lua_type(L, i) != LUA_TTABLE || !lua_checkstack(L, 5)) return std::nullopt;
		i = lua_absindex(L, i);
		M out;
		lua_pushnil(L);
		while (lua_next(L, i)) {
			if (lua_type(L, -2) != LUA_TSTRING) { lua_pop(L, 2); return std::nullopt; }
			size_t len = 0;
			const char* k = lua_tolstring(L, -2, &len);
			auto v = Conv<U>::Get(L, -1);
			if (!v) { lua_pop(L, 2); return std::nullopt; }
			out.emplace(std::string(k, len), std::move(*v));
			lua_pop(L, 1);
		}
		return out;
	}
};

} // namespace luascript

// ---------------------------------------------------------------------------
class LuaScript {
public:
	using ErrorHandler = std::function<void(const std::string&)>;

	template <class R>
	using Result = std::conditional_t<std::is_void_v<R>, bool, std::optional<R>>;

	explicit LuaScript(bool openLibs = true) : L_(luaL_newstate()) {
		if (L_ && openLibs) luaL_openlibs(L_);
	}
	~LuaScript() { if (L_) lua_close(L_); }

	LuaScript(const LuaScript&) = delete;
	LuaScript& operator=(const LuaScript&) = delete;
	LuaScript(LuaScript&& o) noexcept
		: L_(std::exchange(o.L_, nullptr)), onError_(std::move(o.onError_)), lastError_(std::move(o.lastError_)) {}
	LuaScript& operator=(LuaScript&& o) noexcept {
		if (this != &o) {
			if (L_) lua_close(L_);
			L_ = std::exchange(o.L_, nullptr);
			onError_ = std::move(o.onError_);
			lastError_ = std::move(o.lastError_);
		}
		return *this;
	}

	explicit operator bool() const { return L_ != nullptr; }
	lua_State* State() const { return L_; } // escape hatch for raw API use

	// ---- errors ----------------------------------------------------------
	// By default errors go to stderr. Install your own sink (logger, exception, ...).
	void SetErrorHandler(ErrorHandler h) { onError_ = std::move(h); }
	const std::string& LastError() const { return lastError_; }
	void ClearError() { lastError_.clear(); }

	// ---- running code ----------------------------------------------------
	bool DoFile(const std::string& path) {
		Guard g(L_);
		if (luaL_loadfile(L_, path.c_str()) != LUA_OK) { ReportTop(); return false; }
		return SafeCall(0, LUA_MULTRET);
	}
	bool DoString(std::string_view code, const char* chunkName = "=string") {
		Guard g(L_);
		if (luaL_loadbufferx(L_, code.data(), code.size(), chunkName, nullptr) != LUA_OK) { ReportTop(); return false; }
		return SafeCall(0, LUA_MULTRET);
	}

	// ---- reading values --------------------------------------------------
	// Paths are dotted: "window.width", "levels.1.name". Numeric segments are array indices.
	// Top-level names are globals.
	template <class T>
	std::optional<T> Get(std::string_view path) {
		Guard g(L_);
		if (!PushPath(path)) return std::nullopt;
		return luascript::Conv<T>::Get(L_, -1);
	}

	template <class T>
	T GetOr(std::string_view path, T fallback) {
		auto v = Get<T>(path);
		return v ? std::move(*v) : std::move(fallback);
	}
	std::string GetOr(std::string_view path, const char* fallback) { return GetOr<std::string>(path, fallback); }

	// Throws std::runtime_error if missing / wrong type. Handy for mandatory config.
	template <class T>
	T Require(std::string_view path) {
		auto v = Get<T>(path);
		if (!v) throw std::runtime_error("lua: missing or wrong type: " + std::string(path));
		return std::move(*v);
	}

	bool Has(std::string_view path) { Guard g(L_); return PushPath(path); }

	// "nil", "boolean", "number", "string", "table", "function", ... ("nil" if missing)
	std::string TypeOf(std::string_view path) {
		Guard g(L_);
		PushPath(path);
		return lua_typename(L_, lua_type(L_, -1));
	}

	// Raw length of the array at path (0 if not a table)
	size_t Length(std::string_view path) {
		Guard g(L_);
		if (!PushPath(path) || !lua_istable(L_, -1)) return 0;
		return static_cast<size_t>(lua_rawlen(L_, -1));
	}

	// String keys of the table at path
	std::vector<std::string> Keys(std::string_view path) {
		Guard g(L_);
		std::vector<std::string> out;
		if (!PushPath(path) || !lua_istable(L_, -1) || !lua_checkstack(L_, 4)) return out;
		lua_pushnil(L_);
		while (lua_next(L_, -2)) {
			if (lua_type(L_, -2) == LUA_TSTRING) {
				size_t len = 0;
				const char* k = lua_tolstring(L_, -2, &len);
				out.emplace_back(k, len);
			}
			lua_pop(L_, 1);
		}
		return out;
	}

	// ---- writing values --------------------------------------------------
	// Parent table must exist for nested paths ("game.level" needs `game` to be a table).
	template <class T>
	bool Set(std::string_view path, T&& value) {
		Guard g(L_);
		if (!lua_checkstack(L_, 8)) return false;
		PushOne(std::forward<T>(value));
		return AssignTop(path);
	}

	// ---- calling Lua from C++ -------------------------------------------
	// Call<R>("fn", args...): R = void -> bool success; otherwise std::optional<R>.
	template <class R = void, class... Args>
	Result<R> Call(std::string_view func, Args&&... args) {
		Guard g(L_);
		if (!PushPath(func) || !lua_isfunction(L_, -1)) {
			ReportError("'" + std::string(func) + "' is not a function");
			return Result<R>{};
		}
		return Invoke<R>(0, std::forward<Args>(args)...);
	}

	// obj:method(args...)   (the table is passed as `self`)
	template <class R = void, class... Args>
	Result<R> CallMethod(std::string_view table, std::string_view method, Args&&... args) {
		Guard g(L_);
		if (!PushPath(table) || !lua_istable(L_, -1)) {
			ReportError("'" + std::string(table) + "' is not a table");
			return Result<R>{};
		}
		lua_getfield(L_, -1, std::string(method).c_str());
		if (!lua_isfunction(L_, -1)) {
			ReportError("'" + std::string(table) + "." + std::string(method) + "' is not a function");
			return Result<R>{};
		}
		lua_pushvalue(L_, -2); // self
		return Invoke<R>(1, std::forward<Args>(args)...);
	}

	// ---- exposing C++ to Lua --------------------------------------------
	// Any lambda / function pointer whose params and return type have a Conv<>.
	// Strings must be taken as std::string. Exceptions become Lua errors.
	//   lua.Register("clamp", [](double v, double lo, double hi) { return std::min(std::max(v, lo), hi); });
	//   lua.Register("engine.log", ...)   // needs `engine` table to exist
	template <class F>
	bool Register(std::string_view name, F&& fn) {
		using Fn = std::decay_t<F>;
		using Tr = FnTraits<Fn>;
		Thunk thunk = [f = Fn(std::forward<F>(fn))](lua_State* L) mutable -> int {
			return Dispatch<Tr>(L, f, std::make_index_sequence<std::tuple_size_v<typename Tr::Args>>{});
		};

		Guard g(L_);
		if (!lua_checkstack(L_, 8)) return false;
		void* mem = lua_newuserdatauv(L_, sizeof(Thunk), 0);
		new (mem) Thunk(std::move(thunk));
		if (luaL_newmetatable(L_, kThunkMeta)) {
			lua_pushcfunction(L_, &LuaScript::ThunkGc);
			lua_setfield(L_, -2, "__gc");
		}
		lua_setmetatable(L_, -2);
		lua_pushcclosure(L_, &LuaScript::Trampoline, 1);
		return AssignTop(name);
	}

private:
	using Thunk = std::function<int(lua_State*)>;
	static constexpr const char* kThunkMeta = "LuaScript.Thunk";

	// Restores the stack height on scope exit -> no stack leaks, ever.
	struct Guard {
		lua_State* L; int top;
		explicit Guard(lua_State* l) : L(l), top(lua_gettop(l)) {}
		~Guard() { lua_settop(L, top); }
	};

	// ---- errors
	void ReportError(const std::string& msg) {
		lastError_ = msg;
		if (onError_) onError_(msg);
		else std::fprintf(stderr, "[lua]: %s\n", msg.c_str());
	}
	void ReportTop() { // pops the error object on top of the stack
		size_t n = 0;
		const char* m = lua_tolstring(L_, -1, &n);
		ReportError(m ? std::string(m, n) : std::string("(non-string error object)"));
		lua_pop(L_, 1);
	}
	static int MsgHandler(lua_State* L) { // adds a traceback to runtime errors
		const char* msg = lua_tostring(L, 1);
		luaL_traceback(L, L, msg ? msg : "(non-string error object)", 1);
		return 1;
	}
	// function + nargs args are on top of the stack
	bool SafeCall(int nargs, int nres) {
		const int base = lua_gettop(L_) - nargs;
		lua_pushcfunction(L_, &LuaScript::MsgHandler);
		lua_insert(L_, base);
		const int st = lua_pcall(L_, nargs, nres, base);
		if (st != LUA_OK) { ReportTop(); lua_remove(L_, base); return false; }
		lua_remove(L_, base);
		return true;
	}

	// ---- paths
	static bool ParseIndex(std::string_view s, lua_Integer& out) {
		if (s.empty()) return false;
		auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), out);
		return ec == std::errc() && p == s.data() + s.size();
	}
	// Pushes the value at `path` (or nil). Returns true if it is non-nil.
	bool PushPath(std::string_view path) {
		lua_pushglobaltable(L_);
		size_t pos = 0;
		while (!path.empty()) {
			const size_t dot = path.find('.', pos);
			const std::string_view seg = path.substr(pos, dot == std::string_view::npos ? dot : dot - pos);
			if (!lua_istable(L_, -1)) { lua_pop(L_, 1); lua_pushnil(L_); return false; }
			lua_Integer idx = 0;
			if (ParseIndex(seg, idx)) lua_geti(L_, -1, idx);
			else lua_getfield(L_, -1, std::string(seg).c_str());
			lua_remove(L_, -2);
			if (dot == std::string_view::npos) break;
			pos = dot + 1;
		}
		return !lua_isnil(L_, -1);
	}
	// Value to assign is on top; always pops it.
	bool AssignTop(std::string_view path) {
		if (path.empty()) { lua_pop(L_, 1); ReportError("empty path"); return false; }
		const size_t dot = path.rfind('.');
		const std::string_view key = dot == std::string_view::npos ? path : path.substr(dot + 1);
		if (dot == std::string_view::npos) {
			lua_pushglobaltable(L_);
		} else if (!PushPath(path.substr(0, dot)) || !lua_istable(L_, -1)) {
			lua_pop(L_, 2);
			ReportError("cannot set '" + std::string(path) + "': parent is not a table");
			return false;
		}
		lua_insert(L_, -2); // table, value
		lua_Integer idx = 0;
		if (ParseIndex(key, idx)) lua_seti(L_, -2, idx);
		else lua_setfield(L_, -2, std::string(key).c_str());
		lua_pop(L_, 1);
		return true;
	}

	// ---- calls
	template <class T>
	int PushOne(T&& v) { return luascript::Conv<std::decay_t<T>>::Push(L_, std::forward<T>(v)); }

	// function (and `pre` already-pushed args, e.g. self) is on the stack
	template <class R, class... Args>
	Result<R> Invoke(int pre, Args&&... args) {
		if (!lua_checkstack(L_, static_cast<int>(sizeof...(Args)) + 8)) {
			ReportError("too many arguments");
			return Result<R>{};
		}
		int n = pre;
		((n += PushOne(std::forward<Args>(args))), ...);
		if (!SafeCall(n, std::is_void_v<R> ? 0 : 1)) return Result<R>{};
		if constexpr (std::is_void_v<R>) {
			return true;
		} else {
			auto v = luascript::Conv<std::decay_t<R>>::Get(L_, -1);
			if (!v) ReportError("unexpected return type from Lua function");
			return v;
		}
	}

	// ---- registered C++ functions
	template <class T> struct FnTraits : FnTraits<decltype(&T::operator())> {};
	template <class R, class... A> struct FnTraits<R (*)(A...)> {
		using Ret = R;
		using Args = std::tuple<A...>;
	};
	template <class C, class R, class... A> struct FnTraits<R (C::*)(A...) const> : FnTraits<R (*)(A...)> {};
	template <class C, class R, class... A> struct FnTraits<R (C::*)(A...)> : FnTraits<R (*)(A...)> {};

	// Returns number of results, or -1 with an error message pushed.
	// Never calls lua_error itself so C++ destructors always run.
	template <class Tr, class F, size_t... I>
	static int Dispatch(lua_State* L, F& f, std::index_sequence<I...>) {
		using Args = typename Tr::Args;
		using R = typename Tr::Ret;
		try {
			std::tuple<std::optional<std::decay_t<std::tuple_element_t<I, Args>>>...> in{
				luascript::Conv<std::decay_t<std::tuple_element_t<I, Args>>>::Get(L, static_cast<int>(I) + 1)...};
			(void)in;
			int bad = 0;
			((!std::get<I>(in) && bad == 0 ? (void)(bad = static_cast<int>(I) + 1) : (void)0), ...);
			if (bad) {
				const std::string m = "bad argument #" + std::to_string(bad);
				lua_pushlstring(L, m.data(), m.size());
				return -1;
			}
			if constexpr (std::is_void_v<R>) {
				f(*std::get<I>(in)...);
				return 0;
			} else {
				return luascript::Conv<std::decay_t<R>>::Push(L, f(*std::get<I>(in)...));
			}
		} catch (const std::exception& e) {
			lua_pushstring(L, e.what());
			return -1;
		} catch (...) {
			lua_pushstring(L, "unknown C++ exception");
			return -1;
		}
	}
	static int Trampoline(lua_State* L) {
		auto* t = static_cast<Thunk*>(lua_touserdata(L, lua_upvalueindex(1)));
		const int n = (*t)(L);
		return n < 0 ? lua_error(L) : n;
	}
	static int ThunkGc(lua_State* L) {
		static_cast<Thunk*>(lua_touserdata(L, 1))->~Thunk();
		return 0;
	}

	lua_State* L_ = nullptr;
	ErrorHandler onError_;
	std::string lastError_;
};