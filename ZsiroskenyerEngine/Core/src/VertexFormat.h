////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Vertex format stored as int64
//	Limitations:
//		TODO
////////////////////////////////////////////////////////////////////////////////

#include <cstdint>
#include <stdexcept>
#include <vector>


class cVertexFormat {
public:
	cVertexFormat() : data(0) {}

	// Enumerations
	enum eType : uint32_t {
		INT = 1,
		NORM = 2,
		FLOAT = 3,
	};
	enum eSemantic : uint32_t {
		POSITION = 0,
		NORMAL = 1,
		COLOR = 2,
		TEXCOORD = 3,
	};
	enum eBits : uint32_t {
		_8_BIT	= 1,
		_16_BIT	= 2,
		_32_BIT	= 3,
	};

	// Vertex attrib struct, uncompressed
	struct VertexAttrib {
		eType type;
		eSemantic semantic;
		uint32_t nComponents;
		eBits bitsPerComponent;
	};

	// Create vertex decl
	inline void Create(std::vector<VertexAttrib> attribs) {
		Create(attribs.data(), attribs.size());
	}
	inline void Create(VertexAttrib* attribs, uint32_t nAttribs) {
		data = 0;
		if (nAttribs > 8) {
			throw std::invalid_argument("8 attributes at maximum");
		}
		try {
			for (decltype(nAttribs) i = 0; i < nAttribs; i++) {
				auto& attrib = attribs[i];
				uint64_t aUint = EncodeAttrib(attrib.type, attrib.semantic, attrib.nComponents, attrib.bitsPerComponent);
				data |= (aUint << (8 * i));
			}
		}
		catch (std::exception&) {
			data = 0;
			throw;
		}
	}
	// Decode vertex decl to array
	inline std::vector<VertexAttrib> Decode() {
		VertexAttrib attrib;
		std::vector<VertexAttrib> v;
		for (int i = 0; i < 8; i++) {
			uint8_t aUint = uint8_t(data >> (i * 8));
			if (aUint == 0)
				break;
			DecodeAttrib(aUint, attrib.type, attrib.semantic, attrib.nComponents, attrib.bitsPerComponent);
			v.push_back(attrib);
		}
		return v;
	}

	// Raw data
	inline uint64_t Raw() {
		return data;
	}

	// Compare
	inline bool operator==(const cVertexFormat& other) const {
		return other.data == data;
	}
	inline bool operator!=(const cVertexFormat& other) const {
		return other.data != data;
	}

private:
	// Encode/Decode to/from 8 bits
	inline void DecodeAttrib(uint8_t attrib, eType& type, eSemantic& semantic, uint32_t& nComponents, eBits& bitsPerComponent) {
		type = eType((attrib >> 6) & 3);
		semantic = eSemantic((attrib >> 4) & 3);
		nComponents = ((attrib >> 2) & 3) + 1;
		bitsPerComponent = eBits(attrib & 3);
	}
	inline uint8_t EncodeAttrib(eType type, eSemantic semantic, uint32_t nComponents, eBits bitsPerComponent) {
		if (nComponents - 1 > 3)
			throw std::invalid_argument("components must be between 1 and 4");
		if (unsigned(semantic) >= 4)
			throw std::invalid_argument("use enum for semantics, don\'t hack");
		if (unsigned(bitsPerComponent) - 1 >= 4)
			throw std::invalid_argument("use enum for component width, don\'t hack");
		if (unsigned(type) - 1 >= 4)
			throw std::invalid_argument("use enum for type, don\'t hack");
		if (type == eType::FLOAT && bitsPerComponent == eBits::_8_BIT)
			throw std::invalid_argument("float cannot be used with 8 bits");

		uint8_t ret = 0;
		ret |= (uint8_t(type) << 6);
		ret |= (uint8_t(semantic) << 4);
		ret |= ((nComponents-1) << 2);
		ret |= uint8_t(bitsPerComponent);
		return ret;
	}

	// 8x8 bit representation of at max 8 attribs
	uint64_t data;
};
