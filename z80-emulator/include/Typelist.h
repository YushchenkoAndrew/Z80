#pragma once
#include <string>
#include <array>

template<typename T>
struct Type2Type {
	typedef T Type;
	inline Type2Type() {}
};

template<int v>
struct Int2Type {
	enum { value = v };
	inline Int2Type() {}
};

template<class T, class U>
struct TypeList {
	typedef T Head;
	typedef U Tail;
	inline TypeList() {}
};

template<class T>
struct TypeList<T, std::string> {
	std::string val;
	inline TypeList() {}

  std::string& operator = (std::string c) { return val = c; }
  template<class U> std::string& operator = (TypeList<U, std::string>& list) { return val = list.val; }

  std::string& operator* () { return val; }
};

template<class T, class U, int Y>
struct TypeList<TypeList<T, Int2Type<Y>>, std::array<U, +Y>> {
	typedef std::array<U, +Y> ArrT;

	ArrT val;
	inline TypeList() {}

  ArrT& operator = (ArrT c) { return val = c; }
  ArrT& operator = (ArrT& c) { return val = c; }
  ArrT& operator* () { return val; }
};


template<int32_t T, class U>
struct AnyType {

	#pragma always_inline
	static inline U& GetValue() __attribute__((always_inline)) {
		static U value;
		return value;
	}
};

template<int32_t T>
struct AnyType<T, int32_t> {
	static inline int32_t& GetValue() __attribute__((always_inline)) {
		static int32_t value = 0;
		return value;
	}

	template<int32_t U>
	static inline bool Compare(Type2Type<int32_t>) {
		return GetValue() == AnyType<U, int32_t>::GetValue();
	}

	template<int32_t U, class V>
	static inline bool Compare(Type2Type<TypeList<V, int32_t>>) {
		return GetValue() == AnyType<U, TypeList<V, int32_t>>::GetValue().val;
	}

	template<int32_t U>
	static inline bool Compare(Int2Type<U> val) {
		return GetValue() == val.value;
	}
};

template<int32_t T>
struct AnyType<T, float> {
	static inline float& GetValue() __attribute__((always_inline)) {
		static float value = 0.;
		return value;
	}

	template<int32_t U>
	static inline bool Compare(Type2Type<float>) {
		return GetValue() == AnyType<U, float>::GetValue();
	}
};

template<int32_t T>
struct AnyType<T, std::string> {

	#pragma always_inline
	static inline std::string& GetValue() __attribute__((always_inline)) {
		static std::string value;
		return value;
	}

	template<int32_t U>
	static inline bool Compare(Type2Type<std::string>) {
		auto a = GetValue();
		auto b = AnyType<U, std::string>::GetValue();

		return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char a, char b) { return tolower(a) == tolower(b); });
	}

	template<int32_t U, class V>
	static inline bool Compare(Type2Type<TypeList<V, std::string>>) {
		auto a = GetValue();
		auto b = AnyType<U, TypeList<V, std::string>>::GetValue().val;

		return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char a, char b) { return tolower(a) == tolower(b); });
	}
};


struct NullType {};

template<int32_t T, class U>
struct AnyListType {
	static inline U& Get() __attribute__((always_inline)) {
		return AnyType<T, U>::GetValue();
	}

	static inline AnyListType<T, U>& GetValue() __attribute__((always_inline)) {
		static AnyListType<T, U> value;
		return value;
	}

	template<int32_t F, class L>
	static inline AnyListType<T, AnyListType<F, U>>& Next(AnyListType<F, L>) {
		static AnyListType<T, AnyListType<F, U>> value = {};
		return value;
	}

	// template<F>
	// static inline AnyListType& SetNext() __attribute__((always_inline)) {
	// 	static AnyListType value;
	// 	return value;
	// }
};

// template<int32_t T>
// struct AnyListType<T, NullType> {
// 	static inline NullType& GetValue() __attribute__((always_inline)) {
// 		static NullType value;
// 		return value;
// 	}

// 	template<int32_t F>
// 	static inline AnyListType<F, AnyListType<T, NullType>>& Next(Int2Type<F>) {
// 		static AnyListType<F, AnyListType<T, NullType>> value;
// 		return value;
// 	}
// };

// template<int32_t T, class U, class L, class F>
// struct AnyListType<TypeList<AnyType<T, U>, L>, F> {

// 	#pragma always_inline
// 	static inline F& GetValue() __attribute__((always_inline)) {
// 		return L::GetValue();
// 	}
// };

// template<int32_t T, class U>
// struct AnyListType<TypeList<AnyType<T, U>, NullType>, U> {

// 	#pragma always_inline
// 	static inline U& GetValue() __attribute__((always_inline)) {
// 		return AnyType<T, U>::GetValue();
// 	}
// }
