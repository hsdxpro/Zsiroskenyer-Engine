////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/Node.h
//	2013.oct.11. Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Identifying geometry nodes by their name, allows fast hashes and compare.
////////////////////////////////////////////////////////////////////////////////


#include <cstdint>
#include <functional>

struct tNodeBase {
	union {
		char name[8];
		uint64_t nameId;
	};
	tNodeBase() : nameId(0u) {}
	tNodeBase(const char* s) {
		for (int i=0, j=0; i<8; i++)
			name[i] = s[j], s[i]!='\0'?++j:j;
	}
	tNodeBase(uint64_t n) : nameId(n) {}
	tNodeBase& operator=(const tNodeBase& other) {
		nameId = other.nameId;
	}
	tNodeBase& operator=(const char* s) {
		new (this) tNodeBase(s);
	}
	bool operator==(const tNodeBase& other) const {
		return nameId==other.nameId;
	}
};


template <>
struct std::hash<tNodeBase> {
	std::hash<uint64_t> intHasher;
	size_t operator()(const tNodeBase& node) const {
		return intHasher(node.nameId);
	}
};

template <>
struct std::hash<tNodeBase*> {
	std::hash<tNodeBase> nodeHasher;
	size_t operator()(const tNodeBase* node) const {
		return node==nullptr ? 0:nodeHasher(*node);
	}
};