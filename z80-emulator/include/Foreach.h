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
		if (F::template Compare<T>(Type2Type<V>())) return T;
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

	static inline auto Key2Process(F* ref) {
		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return ref->Process(Int2Type<T>());
    return foreach<U, F>::Key2Process(ref);
	}

};

template<int32_t T, class V, class F>
struct foreach<TypeList<AnyType<T, V>, NullType>, F> {
	// static inline void Process() {
	// 	if (F::template Compare<T>()) return Commands::GetValue().Operation(Int2Type<T>());
  //   Commands::GetValue().Operation(Int2Type<-1>());
	// }

	static inline int32_t Value2Key() {
		if (F::template Compare<T>(Type2Type<V>())) return T;
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

	static inline auto Key2Process(F* ref) {
		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return ref->Process(Int2Type<T>());
    return ref->Process(Int2Type<-1>());
	}

};

template<int32_t T, class V, int32_t Y, class U, class F>
struct foreach<TypeList<TypeList<AnyType<T, V>, Int2Type<Y>>, U>, F> {

	static inline std::pair<V, int32_t> Key2Value() {
		if (F::template Compare(Int2Type<T>())) return std::pair(AnyType<T, V>::GetValue(), Int2Type<Y>().value);
    return foreach<U, F>::Key2Value();
	}

	static inline auto Key2Process(F* ref) {
		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return ref->Process(Int2Type<T>());
    return foreach<U, F>::Key2Process(ref);
	}

  template<typename ...Args>
	static inline auto Key2Process(F* ref, Args ...args) {
		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return ref->Process(args..., Int2Type<T>());
    return foreach<U, F>::Key2Process(ref, args...);
	}
};

template<int32_t T, class V, int32_t Y, class F>
struct foreach<TypeList<TypeList<AnyType<T, V>, Int2Type<Y>>, NullType>, F> {

	static inline std::pair<V, int32_t> Key2Value() {
		if (F::template Compare(Int2Type<T>())) return std::pair(AnyType<T, V>::GetValue(), Int2Type<Y>().value);
    return std::pair(AnyType<-2, V>::GetValue(), -1);
	}

	static inline auto Key2Process(F* ref) {
		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return ref->Process(Int2Type<T>());
    return ref->Process(Int2Type<-1>());
	}

  template<typename ...Args>
	static inline auto Key2Process(F* ref, Args ...args) {
		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return ref->Process(args..., Int2Type<T>());
    return ref->Process(Int2Type<-1>());
	}

};

template<int32_t T, int32_t U, class V, class Y>
struct foreach<TypeList<TypeList<Int2Type<T>, Int2Type<U>>, V>, Y> {
	static inline void Process(Y* ref) {
    foreach<V, Y>::Process(ref);
		ref->Process(TypeList<Int2Type<T>, Int2Type<U>>());
	}

	static inline void Command(Y* ref) {
		if (!AnyType<-1, int32_t>::Compare(Int2Type<U>())) return foreach<V, Y>::Command(ref);
		ref->Command(Int2Type<T>());
	}

	static inline bool Has() {
		if (Y::template Compare(Int2Type<U>())) return true;
		return foreach<V, Y>::Has();
	}
};

template<int32_t T, int32_t U, class Y>
struct foreach<TypeList<TypeList<Int2Type<T>, Int2Type<U>>, NullType>, Y> {
	static inline void Process(Y* ref) {
		ref->Process(TypeList<Int2Type<T>, Int2Type<U>>());
	}

	static inline void Command(Y* ref) {
		if (!AnyType<-1, int32_t>::Compare(Int2Type<U>())) return;
		ref->Command(Int2Type<T>());
	}

	static inline bool Has() {
		if (Y::template Compare(Int2Type<U>())) return true;
		return false;
	}
};