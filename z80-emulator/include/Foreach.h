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

template<int32_t T, class U, class F>
struct foreach<TypeList<AnyType<T, std::string>, U>, F> {
	// static inline void Process() {
	// 	if (F::template Compare<T>()) return Commands::GetValue().Operation(Int2Type<T>());
  //   foreach<U, F>::Process();
	// }

	static inline int32_t Value2Key() {
		if (F::template Compare<T>()) return T;
    return foreach<U, F>::Value2Key();
	}

	static inline std::string Key2Value() {
		if (F::template Compare(Int2Type<T>())) return AnyType<T, std::string>::GetValue();
    return foreach<U, F>::Key2Value();
	}

	static inline bool Key2Bool() {
		if (F::template Compare(Int2Type<T>())) return true;
    return foreach<U, F>::Key2Value();
	}
};

template<int32_t T, class F>
struct foreach<TypeList<AnyType<T, std::string>, NullType>, F> {
	// static inline void Process() {
	// 	if (F::template Compare<T>()) return Commands::GetValue().Operation(Int2Type<T>());
  //   Commands::GetValue().Operation(Int2Type<-1>());
	// }

	static inline int32_t Value2Key() {
		if (F::template Compare<T>()) return T;
    return -1;
	}

	static inline std::string Key2Value() {
		if (F::template Compare(Int2Type<T>())) return AnyType<T, std::string>::GetValue();
    return "";
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