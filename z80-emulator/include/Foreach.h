#include "include/Typelist.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

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

template<int32_t T, typename U, class V, class R>
struct foreach<TypeList<TypeList<Int2Type<T>, U>, V>, R> {
	template<class Y>
	static inline void Process(Y* ref) {
    foreach<V, R>::Process(ref);
		ref->Process(TypeList<Int2Type<T>, U>());
	}

	template<class Y>
	static inline bool Proccess(Y* ref, const std::vector<int32_t>& sequence) {
		auto index = foreach<typename UnwrapList<U>::type, void>::PartOf(sequence, 0);
		switch (index) {
			case  0: return true;
			case  1: ref->Process2(Int2Type<T>());
			case -1: return false;
		}

		return foreach<V, R>::Process(ref, sequence);
	}

	// static inline void Command(Y* ref) {
	// 	// FIXME: Think how to impl this
	// 	// if (!AnyType<-1, int32_t>::Compare(U())) return foreach<V, Y>::Command(ref);
	// 	// ref->Command(Int2Type<T>());
	// }

	// static inline bool Has() {
	// 	return false;

	// 	// FIXME: Think how to impl this
	// 	// if (Y::template Compare(U())) return true;
	// 	// return foreach<V, Y>::Has();
	// }
};


template<int32_t T, class U, class R>
struct foreach<TypeList<Int2Type<T>, U>, R> {
	static inline int8_t PartOf(const std::vector<int32_t>& sequence, size_t index) {
		if (index >= sequence.size()) return 0;
    if (sequence[index] != +T) return -1;

    return foreach<U, R>::PartOf(sequence, index + 1);
	}
};


template<class R>
struct foreach<NullType, R> {
	template<class Y>
	static inline R Process(Y* ref) {} // ref->Process(TypeList<Int2Type<T>, U>());

	template<class Y>
	static inline bool Process(Y* ref, const std::vector<int32_t>& sequence) { return false; }

	static inline int8_t PartOf(const std::vector<int32_t>& sequence, size_t index) {
    return index == sequence.size() ? 1 : -1;
	}
};

// template<class R>
// struct foreach<NullType, R> {
// 	template<class Y>
// 	static inline R Process(Y* ref) {
// 		// ref->Process(TypeList<Int2Type<T>, U>());
// 	}

// 	static inline void Find(const std::vector<int32_t>& sequence) {}

// 	// static inline void Command(Y* ref) {
// 	// 	// FIXME: Think how to impl this
// 	// 	// if (!AnyType<-1, int32_t>::Compare(U())) return;
// 	// 	// ref->Command(Int2Type<T>());
// 	// }

// 	// static inline bool Has() {
// 	// 	// FIXME: Think how to impl this
// 	// 	// if (Y::template Compare(Int2Type<U>())) return true;
// 	// 	return false;
// 	// }
// };


// template <class T, class U, class R>
// struct foreach<TypeList<T, U>, R> {
    
//   // template<class V, class H>
//   // static R Key2Process(const Any2Type& box, V* ref, H head) {
//   //   if (box.typeId != std::type_index(typeid(T))) {
//   //     return foreach<U, R>::Key2Process(box, ref, head);
//   //   }
    
//   //   return ref->Process2(TypeList<H, TypeList<T, NullType>>());
//   // }

//   template<class V, class H>
//   static R Key2Process(V* ref, const Any2Type& box,  H head) {
// // const Any2Type& box,
// 	}

// };

// template <class R>
// struct foreach<NullType, R> {
//     // template<class V, typename ...Args>
//     // static R Key2Process(const Any2Type&, V*, Args&&...) { return nullptr; }
// };


// Base case specialization to stop the recursive compilation when hitting NullType
// template <>
// struct UnionUnpacker<NullType> {
//     static bool unpackAndProcess(const FlexibleTypeBox&, Panel*, int32_t) {
//         std::cerr << "Error: Passed type ID was not registered in the TypeList!\n";
//         return false; 
//     }
// };


// template<int32_t T, class V, int32_t Y, class U, class F>
// struct foreach<TypeList<T, U>
// TypeList<TypeList<AnyType<T, V>, Int2Type<Y>>, U>, F> {

// 	static inline std::pair<V, int32_t> Key2Value() {
// 		if (F::template Compare(Int2Type<T>())) return std::pair(AnyType<T, V>::GetValue(), Int2Type<Y>().value);
//     return foreach<U, F>::Key2Value();
// 	}

// 	static inline auto Key2Process(F* ref) {
// 		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return ref->Process(Int2Type<T>());
//     return foreach<U, F>::Key2Process(ref);
// 	}

//   template<typename ...Args>
// 	static inline auto Key2Process(F* ref, Args ...args) {
// 		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return ref->Process(args..., Int2Type<T>());
//     return foreach<U, F>::Key2Process(ref, args...);
// 	}
// };

// template<int32_t T, class V, int32_t Y, class F>
// struct foreach<TypeList<TypeList<AnyType<T, V>, Int2Type<Y>>, NullType>, F> {

// 	static inline std::pair<V, int32_t> Key2Value() {
// 		if (F::template Compare(Int2Type<T>())) return std::pair(AnyType<T, V>::GetValue(), Int2Type<Y>().value);
//     return std::pair(AnyType<-2, V>::GetValue(), -1);
// 	}

// 	static inline auto Key2Process(F* ref) {
// 		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return ref->Process(Int2Type<T>());
//     return ref->Process(Int2Type<-1>());
// 	}

//   template<typename ...Args>
// 	static inline auto Key2Process(F* ref, Args ...args) {
// 		if (AnyType<-1, int32_t>::Compare(Int2Type<T>())) return ref->Process(args..., Int2Type<T>());
//     return ref->Process(Int2Type<-1>());
// 	}
// };