#include "include/Typelist.h"
#include <string>

// template<class T, class U, class Arg0 = NullType> 
// struct foreach;

// template<int32_t T, class U, class F, class Arg0>
// struct foreach<TypeList<AnyType<T, std::string>, U>, F, Arg0> {
// 	static inline void Proccess() {
// 		if (!F::template Compare<T>()) return foreach<U, F, Arg0>::Proccess();
// 		AnyListType<T, std::string>::Next(Arg0);
// 	}
// };

// template<int32_t T, class U, class Arg0>
// struct foreach<TypeList<AnyType<T, std::string>, NullType>, U, Arg0> {
// 	static inline void Proccess() {
//     if (!U::template Compare<T>()) return;
// 		AnyListType<T, Arg0>::Next() = AnyType<-1, Arg0>::GetValue();
// 	}
// };

template<class T, class U> 
struct foreach;

template<int32_t T, class V, class U, class F>
struct foreach<TypeList<AnyType<T, V>, U>, F> {
	// static inline void Process() {
	// 	if (F::template Compare<T>()) return Commands::GetValue().Operation(Int2Type<T>());
  //   foreach<U, F>::Process();
	// }

	static inline int32_t Value2Key() {
		if (F::template Compare<T>()) return T;
    return foreach<U, F>::Value2Key();
	}

	static inline auto& Key2Value() {
		if (F::template Compare(Int2Type<T>())) return AnyType<T, V>::GetValue();
    return foreach<U, F>::Key2Value();
	}

	static inline bool Key2Bool() {
		if (F::template Compare(Int2Type<T>())) return true;
    return foreach<U, F>::Key2Value();
	}
};

template<int32_t T, class V, class F>
struct foreach<TypeList<AnyType<T, V>, NullType>, F> {
	// static inline void Process() {
	// 	if (F::template Compare<T>()) return Commands::GetValue().Operation(Int2Type<T>());
  //   Commands::GetValue().Operation(Int2Type<-1>());
	// }

	static inline int32_t Value2Key() {
		if (F::template Compare<T>()) return T;
    return -1;
	}

	static inline auto& Key2Value() {
		if (F::template Compare(Int2Type<T>())) return AnyType<T, V>::GetValue();
    return AnyType<-2, V>::GetValue();
	}

	static inline bool Key2Bool() {
		if (F::template Compare(Int2Type<T>())) return true;
    return false;
	}
};

template<int32_t T, class U, int32_t F, class V>
struct foreach<TypeList<AnyType<T, std::string>, U>, AnyType<F, V*>> {
	static inline auto Key2Process() {
		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return AnyType<F, V*>::GetValue()->Process(Int2Type<T>());
    return foreach<U, AnyType<F, V*>>::Key2Process();
	}
};

template<int32_t T, int32_t F, class V>
struct foreach<TypeList<AnyType<T, std::string>, NullType>, AnyType<F, V*>> {
	static inline auto Key2Process() {
		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return AnyType<F, V*>::GetValue()->Process(Int2Type<T>());
    return AnyType<F, V*>::GetValue()->Process(Int2Type<-1>());
	}
};

template<int32_t T, int32_t U, class V, int32_t F, class Y>
struct foreach<TypeList<TypeList<Int2Type<T>, Int2Type<U>>, V>, AnyType<F, Y*>> {
	static inline void Process() {
    foreach<V, AnyType<F, Y*>>::Process();
		AnyType<F, Y*>::GetValue()->Process(TypeList<Int2Type<T>, Int2Type<U>>());
	}

	static inline void Command() {
		if (!AnyType<-1, int32_t>::Compare(Int2Type<U>())) return foreach<V, AnyType<F, Y*>>::Command();
		AnyType<F, Y*>::GetValue()->Command(Int2Type<T>());
	}

	static inline bool Has() {
		if (AnyType<-1, int32_t>::Compare(Int2Type<U>())) return true;
		return foreach<V, AnyType<F, Y*>>::Has();
	}
};

template<int32_t T, int32_t U, int32_t F, class Y>
struct foreach<TypeList<TypeList<Int2Type<T>, Int2Type<U>>, NullType>, AnyType<F, Y*>> {
	static inline void Process() {
		AnyType<F, Y*>::GetValue()->Process(TypeList<Int2Type<T>, Int2Type<U>>());
	}

	static inline void Command() {
		if (!AnyType<-1, int32_t>::Compare(Int2Type<U>())) return;
		AnyType<F, Y*>::GetValue()->Command(Int2Type<T>());
	}

	static inline bool Has() {
		if (AnyType<-1, int32_t>::Compare(Int2Type<U>())) return true;
		return false;
	}
};