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

	static inline int32_t Index() {
		if (F::template Compare<T>()) return T;
    return foreach<U, F>::Index();
	}
};

template<int32_t T, class F>
struct foreach<TypeList<AnyType<T, std::string>, NullType>, F> {
	// static inline void Process() {
	// 	if (F::template Compare<T>()) return Commands::GetValue().Operation(Int2Type<T>());
  //   Commands::GetValue().Operation(Int2Type<-1>());
	// }

	static inline int32_t Index() {
		if (F::template Compare<T>()) return T;
    return -1;
	}
};