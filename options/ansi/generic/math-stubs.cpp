
#include <math.h>
#include <immintrin.h>

#include <bits/ensure.h>

#include <stdint.h>

#include <frigg/debug.hpp>

// Taken from musl. See musl for the license/copyright!
#define FORCE_EVAL(x) do {                        \
	if (sizeof(x) == sizeof(float)) {         \
		volatile float __x;               \
		__x = (x);                        \
	} else if (sizeof(x) == sizeof(double)) { \
		volatile double __x;              \
		__x = (x);                        \
	} else {                                  \
		volatile long double __x;         \
		__x = (x);                        \
	}                                         \
} while(0)

namespace ieee754 {

struct SoftDouble {
	typedef uint64_t Bits;
	typedef uint64_t Mantissa;
	typedef int16_t Exp;

	static constexpr int kMantissaBits = 52;
	static constexpr int kExpBits = 11;
	static constexpr int kBias = 1023;
	
	// this exponent represents zeros (when mantissa = 0) and subnormals (when mantissa != 0)
	static constexpr Exp kSubExp = -kBias;
	// this exponent represents infinities (when mantissa = 0) and NaNs (when mantissa != 0)
	static constexpr Exp kInfExp = ((Exp(1) << kExpBits) - 1) - kBias;

	static constexpr Bits kMantissaMask = (Bits(1) << kMantissaBits) - 1;
	static constexpr Bits kExpMask = ((Bits(1) << kExpBits) - 1) << kMantissaBits;
	static constexpr Bits kSignMask = Bits(1) << (kMantissaBits + kExpBits);

	SoftDouble(bool negative, Mantissa mantissa, Exp exp)
	: negative(negative), mantissa(mantissa), exp(exp) {
//		frigg::infoLogger.log() << "(" << (int)negative << ", " << (void *)mantissa
//				<< ", " << exp << ")" << frigg::EndLog();
		__ensure(mantissa < (Mantissa(1) << kMantissaBits));
		__ensure((exp + kBias) >= 0);
		__ensure((exp + kBias) < (Exp(1) << kExpBits));
	}

	const bool negative;
	const Mantissa mantissa;
	const Exp exp;
};

template<typename F>
using Bits = typename F::Bits;

template<typename F>
using Mantissa = typename F::Mantissa;

template<typename F>
using Exp = typename F::Exp;

template<typename F>
bool isZero(F x) {
	return x.exp == F::kSubExp && x.mantissa == 0;
}

template<typename F>
bool isFinite(F x) {
	return x.exp != F::kInfExp;
}

// --------------------------------------------------------
// Soft float operations
// --------------------------------------------------------

template<typename F>
F constZero(bool negative) {
	return F(negative, 0, F::kSubExp);
}

template<typename F>
F constOne(bool negative) {
	return F(negative, 0, 0);
}

template<typename F>
F floor(F x) {
	if(!isFinite(x) || isZero(x)) // TODO: need exception for the not-finite case?
		return x;
	
	if(x.exp > F::kMantissaBits)
		return x; // x is already integral
	
	if(x.exp < 0) {
		// TODO: raise inexact
		// return -1 or +0
		return x.negative ? constOne<F>(true) : constZero<F>(false);
	}
	
	Mantissa<F> mask = F::kMantissaMask >> x.exp;
	if(!(x.mantissa & mask))
		return x; // x is already integral
	
	// TODO: raise inexact
	Mantissa<F> integral_position = (Mantissa<F>(1) << F::kMantissaBits) >> x.exp;
	if(x.negative)
		return F(true, (x.mantissa + integral_position) & (~mask), x.exp);
	return F(false, x.mantissa & (~mask), x.exp);
}

template<typename F>
F ceil(F x) {
	if(!isFinite(x) || isZero(x)) // TODO: need exception for the not-finite case?
		return x;
	
	if(x.exp > F::kMantissaBits)
		return x; // x is already integral
	
	if(x.exp < 0) {
		// TODO: raise inexact
		// return -0 or +1
		return x.negative ? constZero<F>(true) : constOne<F>(false);
	}
	
	Mantissa<F> mask = F::kMantissaMask >> x.exp;
	if(!(x.mantissa & mask))
		return x; // x is already integral
	
	// TODO: raise inexact
	Mantissa<F> integral_position = (Mantissa<F>(1) << F::kMantissaBits) >> x.exp;
	if(x.negative)
		return F(true, x.mantissa & (~mask), x.exp);
	return F(false, (x.mantissa + integral_position) & (~mask), x.exp);
}

// --------------------------------------------------------
// Soft float <-> bit string conversion functions
// --------------------------------------------------------

template<typename F>
uint64_t compileBits(F soft) {
	auto bits = Bits<F>(soft.mantissa) | ((Bits<F>(soft.exp) + F::kBias) << soft.kMantissaBits);
	return soft.negative ? (F::kSignMask | bits) : bits;
}

SoftDouble extractBits(uint64_t bits) {
	return SoftDouble(bits & SoftDouble::kSignMask, bits & SoftDouble::kMantissaMask,
			((bits & SoftDouble::kExpMask) >> SoftDouble::kMantissaBits) - SoftDouble::kBias);
}

// --------------------------------------------------------
// Soft float -> native float conversion functions
// --------------------------------------------------------

union DoubleBits {
	double fp;
	uint64_t bits;
};

double compileNative(SoftDouble soft) {
	DoubleBits word;
	word.bits = compileBits(soft);
	return word.fp;
}

SoftDouble extractNative(double native) {
	DoubleBits word;
	word.fp = native;
	return extractBits(word.bits);
}

} // namespace ieee754

int __mlibc_fpclassify(double x) {
	return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x);
}
int __mlibc_fpclassifyf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int __mlibc_fpclassifyl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double acos(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float acosf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double acosl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double asin(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float asinf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double asinl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double atan(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float atanf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double atanl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double atan2(double x, double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float atan2f(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double atan2l(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// Taken from musl. See musl for the license/copyright!
float __sindf(double x) {
	/* |sin(x)/x - s(x)| < 2**-37.5 (~[-4.89e-12, 4.824e-12]). */
	static const double S1 = -0x15555554cbac77.0p-55, /* -0.166666666416265235595 */
			S2 = 0x111110896efbb2.0p-59, /*  0.0083333293858894631756 */
			S3 = -0x1a00f9e2cae774.0p-65, /* -0.000198393348360966317347 */
			S4 = 0x16cd878c3b46a7.0p-71; /*  0.0000027183114939898219064 */

	double r, s, w, z;

	/* Try to optimize for parallel evaluation as in __tandf.c. */
	z = x*x;
	w = z*z;
	r = S3 + z*S4;
	s = z*x;
	return (x + s*(S1 + z*S2)) + s*w*r;
}

// Taken from musl. See musl for the license/copyright!
float __cosdf(double x) {
	/* |cos(x) - c(x)| < 2**-34.1 (~[-5.37e-11, 5.295e-11]). */
	static const double C0 = -0x1ffffffd0c5e81.0p-54, /* -0.499999997251031003120 */
			C1 = 0x155553e1053a42.0p-57, /*  0.0416666233237390631894 */
			C2 = -0x16c087e80f1e27.0p-62, /* -0.00138867637746099294692 */
			C3 = 0x199342e0ee5069.0p-68; /*  0.0000243904487962774090654 */

	double r, w, z;

	/* Try to optimize for parallel evaluation as in __tandf.c. */
	z = x*x;
	w = z*z;
	r = C2+z*C3;
	return ((1.0+z*C0) + w*C1) + (w*z)*r;
}

#define DBL_EPSILON 2.22044604925031308085e-16
#define EPS DBL_EPSILON

/* Get a 32 bit int from a float.  */
#define GET_FLOAT_WORD(w,d)                       \
do {                                              \
  union {float f; uint32_t i;} __u;               \
  __u.f = (d);                                    \
  (w) = __u.i;                                    \
} while (0)

// Taken from musl. See musl for the license/copyright!
int __rem_pio2_large(double *x, double *y, int e0, int nx, int prec)
{
	static const int init_jk[] = {3,4,4,6}; /* initial value for jk */

	/*
	 * Table of constants for 2/pi, 396 Hex digits (476 decimal) of 2/pi
	 *
	 *              integer array, contains the (24*i)-th to (24*i+23)-th
	 *              bit of 2/pi after binary point. The corresponding
	 *              floating value is
	 *
	 *                      ipio2[i] * 2^(-24(i+1)).
	 *
	 * NB: This table must have at least (e0-3)/24 + jk terms.
	 *     For quad precision (e0 <= 16360, jk = 6), this is 686.
	 */
	static const int32_t ipio2[] = {
	0xA2F983, 0x6E4E44, 0x1529FC, 0x2757D1, 0xF534DD, 0xC0DB62,
	0x95993C, 0x439041, 0xFE5163, 0xABDEBB, 0xC561B7, 0x246E3A,
	0x424DD2, 0xE00649, 0x2EEA09, 0xD1921C, 0xFE1DEB, 0x1CB129,
	0xA73EE8, 0x8235F5, 0x2EBB44, 0x84E99C, 0x7026B4, 0x5F7E41,
	0x3991D6, 0x398353, 0x39F49C, 0x845F8B, 0xBDF928, 0x3B1FF8,
	0x97FFDE, 0x05980F, 0xEF2F11, 0x8B5A0A, 0x6D1F6D, 0x367ECF,
	0x27CB09, 0xB74F46, 0x3F669E, 0x5FEA2D, 0x7527BA, 0xC7EBE5,
	0xF17B3D, 0x0739F7, 0x8A5292, 0xEA6BFB, 0x5FB11F, 0x8D5D08,
	0x560330, 0x46FC7B, 0x6BABF0, 0xCFBC20, 0x9AF436, 0x1DA9E3,
	0x91615E, 0xE61B08, 0x659985, 0x5F14A0, 0x68408D, 0xFFD880,
	0x4D7327, 0x310606, 0x1556CA, 0x73A8C9, 0x60E27B, 0xC08C6B,

	#if LDBL_MAX_EXP > 1024
	0x47C419, 0xC367CD, 0xDCE809, 0x2A8359, 0xC4768B, 0x961CA6,
	0xDDAF44, 0xD15719, 0x053EA5, 0xFF0705, 0x3F7E33, 0xE832C2,
	0xDE4F98, 0x327DBB, 0xC33D26, 0xEF6B1E, 0x5EF89F, 0x3A1F35,
	0xCAF27F, 0x1D87F1, 0x21907C, 0x7C246A, 0xFA6ED5, 0x772D30,
	0x433B15, 0xC614B5, 0x9D19C3, 0xC2C4AD, 0x414D2C, 0x5D000C,
	0x467D86, 0x2D71E3, 0x9AC69B, 0x006233, 0x7CD2B4, 0x97A7B4,
	0xD55537, 0xF63ED7, 0x1810A3, 0xFC764D, 0x2A9D64, 0xABD770,
	0xF87C63, 0x57B07A, 0xE71517, 0x5649C0, 0xD9D63B, 0x3884A7,
	0xCB2324, 0x778AD6, 0x23545A, 0xB91F00, 0x1B0AF1, 0xDFCE19,
	0xFF319F, 0x6A1E66, 0x615799, 0x47FBAC, 0xD87F7E, 0xB76522,
	0x89E832, 0x60BFE6, 0xCDC4EF, 0x09366C, 0xD43F5D, 0xD7DE16,
	0xDE3B58, 0x929BDE, 0x2822D2, 0xE88628, 0x4D58E2, 0x32CAC6,
	0x16E308, 0xCB7DE0, 0x50C017, 0xA71DF3, 0x5BE018, 0x34132E,
	0x621283, 0x014883, 0x5B8EF5, 0x7FB0AD, 0xF2E91E, 0x434A48,
	0xD36710, 0xD8DDAA, 0x425FAE, 0xCE616A, 0xA4280A, 0xB499D3,
	0xF2A606, 0x7F775C, 0x83C2A3, 0x883C61, 0x78738A, 0x5A8CAF,
	0xBDD76F, 0x63A62D, 0xCBBFF4, 0xEF818D, 0x67C126, 0x45CA55,
	0x36D9CA, 0xD2A828, 0x8D61C2, 0x77C912, 0x142604, 0x9B4612,
	0xC459C4, 0x44C5C8, 0x91B24D, 0xF31700, 0xAD43D4, 0xE54929,
	0x10D5FD, 0xFCBE00, 0xCC941E, 0xEECE70, 0xF53E13, 0x80F1EC,
	0xC3E7B3, 0x28F8C7, 0x940593, 0x3E71C1, 0xB3092E, 0xF3450B,
	0x9C1288, 0x7B20AB, 0x9FB52E, 0xC29247, 0x2F327B, 0x6D550C,
	0x90A772, 0x1FE76B, 0x96CB31, 0x4A1679, 0xE27941, 0x89DFF4,
	0x9794E8, 0x84E6E2, 0x973199, 0x6BED88, 0x365F5F, 0x0EFDBB,
	0xB49A48, 0x6CA467, 0x427271, 0x325D8D, 0xB8159F, 0x09E5BC,
	0x25318D, 0x3974F7, 0x1C0530, 0x010C0D, 0x68084B, 0x58EE2C,
	0x90AA47, 0x02E774, 0x24D6BD, 0xA67DF7, 0x72486E, 0xEF169F,
	0xA6948E, 0xF691B4, 0x5153D1, 0xF20ACF, 0x339820, 0x7E4BF5,
	0x6863B2, 0x5F3EDD, 0x035D40, 0x7F8985, 0x295255, 0xC06437,
	0x10D86D, 0x324832, 0x754C5B, 0xD4714E, 0x6E5445, 0xC1090B,
	0x69F52A, 0xD56614, 0x9D0727, 0x50045D, 0xDB3BB4, 0xC576EA,
	0x17F987, 0x7D6B49, 0xBA271D, 0x296996, 0xACCCC6, 0x5414AD,
	0x6AE290, 0x89D988, 0x50722C, 0xBEA404, 0x940777, 0x7030F3,
	0x27FC00, 0xA871EA, 0x49C266, 0x3DE064, 0x83DD97, 0x973FA3,
	0xFD9443, 0x8C860D, 0xDE4131, 0x9D3992, 0x8C70DD, 0xE7B717,
	0x3BDF08, 0x2B3715, 0xA0805C, 0x93805A, 0x921110, 0xD8E80F,
	0xAF806C, 0x4BFFDB, 0x0F9038, 0x761859, 0x15A562, 0xBBCB61,
	0xB989C7, 0xBD4010, 0x04F2D2, 0x277549, 0xF6B6EB, 0xBB22DB,
	0xAA140A, 0x2F2689, 0x768364, 0x333B09, 0x1A940E, 0xAA3A51,
	0xC2A31D, 0xAEEDAF, 0x12265C, 0x4DC26D, 0x9C7A2D, 0x9756C0,
	0x833F03, 0xF6F009, 0x8C402B, 0x99316D, 0x07B439, 0x15200C,
	0x5BC3D8, 0xC492F5, 0x4BADC6, 0xA5CA4E, 0xCD37A7, 0x36A9E6,
	0x9492AB, 0x6842DD, 0xDE6319, 0xEF8C76, 0x528B68, 0x37DBFC,
	0xABA1AE, 0x3115DF, 0xA1AE00, 0xDAFB0C, 0x664D64, 0xB705ED,
	0x306529, 0xBF5657, 0x3AFF47, 0xB9F96A, 0xF3BE75, 0xDF9328,
	0x3080AB, 0xF68C66, 0x15CB04, 0x0622FA, 0x1DE4D9, 0xA4B33D,
	0x8F1B57, 0x09CD36, 0xE9424E, 0xA4BE13, 0xB52333, 0x1AAAF0,
	0xA8654F, 0xA5C1D2, 0x0F3F0B, 0xCD785B, 0x76F923, 0x048B7B,
	0x721789, 0x53A6C6, 0xE26E6F, 0x00EBEF, 0x584A9B, 0xB7DAC4,
	0xBA66AA, 0xCFCF76, 0x1D02D1, 0x2DF1B1, 0xC1998C, 0x77ADC3,
	0xDA4886, 0xA05DF7, 0xF480C6, 0x2FF0AC, 0x9AECDD, 0xBC5C3F,
	0x6DDED0, 0x1FC790, 0xB6DB2A, 0x3A25A3, 0x9AAF00, 0x9353AD,
	0x0457B6, 0xB42D29, 0x7E804B, 0xA707DA, 0x0EAA76, 0xA1597B,
	0x2A1216, 0x2DB7DC, 0xFDE5FA, 0xFEDB89, 0xFDBE89, 0x6C76E4,
	0xFCA906, 0x70803E, 0x156E85, 0xFF87FD, 0x073E28, 0x336761,
	0x86182A, 0xEABD4D, 0xAFE7B3, 0x6E6D8F, 0x396795, 0x5BBF31,
	0x48D784, 0x16DF30, 0x432DC7, 0x356125, 0xCE70C9, 0xB8CB30,
	0xFD6CBF, 0xA200A4, 0xE46C05, 0xA0DD5A, 0x476F21, 0xD21262,
	0x845CB9, 0x496170, 0xE0566B, 0x015299, 0x375550, 0xB7D51E,
	0xC4F133, 0x5F6E13, 0xE4305D, 0xA92E85, 0xC3B21D, 0x3632A1,
	0xA4B708, 0xD4B1EA, 0x21F716, 0xE4698F, 0x77FF27, 0x80030C,
	0x2D408D, 0xA0CD4F, 0x99A520, 0xD3A2B3, 0x0A5D2F, 0x42F9B4,
	0xCBDA11, 0xD0BE7D, 0xC1DB9B, 0xBD17AB, 0x81A2CA, 0x5C6A08,
	0x17552E, 0x550027, 0xF0147F, 0x8607E1, 0x640B14, 0x8D4196,
	0xDEBE87, 0x2AFDDA, 0xB6256B, 0x34897B, 0xFEF305, 0x9EBFB9,
	0x4F6A68, 0xA82A4A, 0x5AC44F, 0xBCF82D, 0x985AD7, 0x95C7F4,
	0x8D4D0D, 0xA63A20, 0x5F57A4, 0xB13F14, 0x953880, 0x0120CC,
	0x86DD71, 0xB6DEC9, 0xF560BF, 0x11654D, 0x6B0701, 0xACB08C,
	0xD0C0B2, 0x485551, 0x0EFB1E, 0xC37295, 0x3B06A3, 0x3540C0,
	0x7BDC06, 0xCC45E0, 0xFA294E, 0xC8CAD6, 0x41F3E8, 0xDE647C,
	0xD8649B, 0x31BED9, 0xC397A4, 0xD45877, 0xC5E369, 0x13DAF0,
	0x3C3ABA, 0x461846, 0x5F7555, 0xF5BDD2, 0xC6926E, 0x5D2EAC,
	0xED440E, 0x423E1C, 0x87C461, 0xE9FD29, 0xF3D6E7, 0xCA7C22,
	0x35916F, 0xC5E008, 0x8DD7FF, 0xE26A6E, 0xC6FDB0, 0xC10893,
	0x745D7C, 0xB2AD6B, 0x9D6ECD, 0x7B723E, 0x6A11C6, 0xA9CFF7,
	0xDF7329, 0xBAC9B5, 0x5100B7, 0x0DB2E2, 0x24BA74, 0x607DE5,
	0x8AD874, 0x2C150D, 0x0C1881, 0x94667E, 0x162901, 0x767A9F,
	0xBEFDFD, 0xEF4556, 0x367ED9, 0x13D9EC, 0xB9BA8B, 0xFC97C4,
	0x27A831, 0xC36EF1, 0x36C594, 0x56A8D8, 0xB5A8B4, 0x0ECCCF,
	0x2D8912, 0x34576F, 0x89562C, 0xE3CE99, 0xB920D6, 0xAA5E6B,
	0x9C2A3E, 0xCC5F11, 0x4A0BFD, 0xFBF4E1, 0x6D3B8E, 0x2C86E2,
	0x84D4E9, 0xA9B4FC, 0xD1EEEF, 0xC9352E, 0x61392F, 0x442138,
	0xC8D91B, 0x0AFC81, 0x6A4AFB, 0xD81C2F, 0x84B453, 0x8C994E,
	0xCC2254, 0xDC552A, 0xD6C6C0, 0x96190B, 0xB8701A, 0x649569,
	0x605A26, 0xEE523F, 0x0F117F, 0x11B5F4, 0xF5CBFC, 0x2DBC34,
	0xEEBC34, 0xCC5DE8, 0x605EDD, 0x9B8E67, 0xEF3392, 0xB817C9,
	0x9B5861, 0xBC57E1, 0xC68351, 0x103ED8, 0x4871DD, 0xDD1C2D,
	0xA118AF, 0x462C21, 0xD7F359, 0x987AD9, 0xC0549E, 0xFA864F,
	0xFC0656, 0xAE79E5, 0x362289, 0x22AD38, 0xDC9367, 0xAAE855,
	0x382682, 0x9BE7CA, 0xA40D51, 0xB13399, 0x0ED7A9, 0x480569,
	0xF0B265, 0xA7887F, 0x974C88, 0x36D1F9, 0xB39221, 0x4A827B,
	0x21CF98, 0xDC9F40, 0x5547DC, 0x3A74E1, 0x42EB67, 0xDF9DFE,
	0x5FD45E, 0xA4677B, 0x7AACBA, 0xA2F655, 0x23882B, 0x55BA41,
	0x086E59, 0x862A21, 0x834739, 0xE6E389, 0xD49EE5, 0x40FB49,
	0xE956FF, 0xCA0F1C, 0x8A59C5, 0x2BFA94, 0xC5C1D3, 0xCFC50F,
	0xAE5ADB, 0x86C547, 0x624385, 0x3B8621, 0x94792C, 0x876110,
	0x7B4C2A, 0x1A2C80, 0x12BF43, 0x902688, 0x893C78, 0xE4C4A8,
	0x7BDBE5, 0xC23AC4, 0xEAF426, 0x8A67F7, 0xBF920D, 0x2BA365,
	0xB1933D, 0x0B7CBD, 0xDC51A4, 0x63DD27, 0xDDE169, 0x19949A,
	0x9529A8, 0x28CE68, 0xB4ED09, 0x209F44, 0xCA984E, 0x638270,
	0x237C7E, 0x32B90F, 0x8EF5A7, 0xE75614, 0x08F121, 0x2A9DB5,
	0x4D7E6F, 0x5119A5, 0xABF9B5, 0xD6DF82, 0x61DD96, 0x023616,
	0x9F3AC4, 0xA1A283, 0x6DED72, 0x7A8D39, 0xA9B882, 0x5C326B,
	0x5B2746, 0xED3400, 0x7700D2, 0x55F4FC, 0x4D5901, 0x8071E0,
	#endif
	};

	static const double PIo2[] = {
	  1.57079625129699707031e+00, /* 0x3FF921FB, 0x40000000 */
	  7.54978941586159635335e-08, /* 0x3E74442D, 0x00000000 */
	  5.39030252995776476554e-15, /* 0x3CF84698, 0x80000000 */
	  3.28200341580791294123e-22, /* 0x3B78CC51, 0x60000000 */
	  1.27065575308067607349e-29, /* 0x39F01B83, 0x80000000 */
	  1.22933308981111328932e-36, /* 0x387A2520, 0x40000000 */
	  2.73370053816464559624e-44, /* 0x36E38222, 0x80000000 */
	  2.16741683877804819444e-51, /* 0x3569F31D, 0x00000000 */
	};

	int32_t jz,jx,jv,jp,jk,carry,n,iq[20],i,j,k,m,q0,ih;
	double z,fw,f[20],fq[20],q[20];

	/* initialize jk*/
	jk = init_jk[prec];
	jp = jk;

	/* determine jx,jv,q0, note that 3>q0 */
	jx = nx-1;
	jv = (e0-3)/24;  if(jv<0) jv=0;
	q0 = e0-24*(jv+1);

	/* set up f[0] to f[jx+jk] where f[jx+jk] = ipio2[jv+jk] */
	j = jv-jx; m = jx+jk;
	for (i=0; i<=m; i++,j++)
		f[i] = j<0 ? 0.0 : (double)ipio2[j];

	/* compute q[0],q[1],...q[jk] */
	for (i=0; i<=jk; i++) {
		for (j=0,fw=0.0; j<=jx; j++)
			fw += x[j]*f[jx+i-j];
		q[i] = fw;
	}

	jz = jk;
recompute:
	/* distill q[] into iq[] reversingly */
	for (i=0,j=jz,z=q[jz]; j>0; i++,j--) {
		fw    = (double)(int32_t)(0x1p-24*z);
		iq[i] = (int32_t)(z - 0x1p24*fw);
		z     = q[j-1]+fw;
	}

	/* compute n */
	z  = scalbn(z,q0);       /* actual value of z */
	z -= 8.0*floor(z*0.125); /* trim off integer >= 8 */
	n  = (int32_t)z;
	z -= (double)n;
	ih = 0;
	if (q0 > 0) {  /* need iq[jz-1] to determine n */
		i  = iq[jz-1]>>(24-q0); n += i;
		iq[jz-1] -= i<<(24-q0);
		ih = iq[jz-1]>>(23-q0);
	}
	else if (q0 == 0) ih = iq[jz-1]>>23;
	else if (z >= 0.5) ih = 2;

	if (ih > 0) {  /* q > 0.5 */
		n += 1; carry = 0;
		for (i=0; i<jz; i++) {  /* compute 1-q */
			j = iq[i];
			if (carry == 0) {
				if (j != 0) {
					carry = 1;
					iq[i] = 0x1000000 - j;
				}
			} else
				iq[i] = 0xffffff - j;
		}
		if (q0 > 0) {  /* rare case: chance is 1 in 12 */
			switch(q0) {
			case 1:
				iq[jz-1] &= 0x7fffff; break;
			case 2:
				iq[jz-1] &= 0x3fffff; break;
			}
		}
		if (ih == 2) {
			z = 1.0 - z;
			if (carry != 0)
				z -= scalbn(1.0,q0);
		}
	}

	/* check if recomputation is needed */
	if (z == 0.0) {
		j = 0;
		for (i=jz-1; i>=jk; i--) j |= iq[i];
		if (j == 0) {  /* need recomputation */
			for (k=1; iq[jk-k]==0; k++);  /* k = no. of terms needed */

			for (i=jz+1; i<=jz+k; i++) {  /* add q[jz+1] to q[jz+k] */
				f[jx+i] = (double)ipio2[jv+i];
				for (j=0,fw=0.0; j<=jx; j++)
					fw += x[j]*f[jx+i-j];
				q[i] = fw;
			}
			jz += k;
			goto recompute;
		}
	}

	/* chop off zero terms */
	if (z == 0.0) {
		jz -= 1;
		q0 -= 24;
		while (iq[jz] == 0) {
			jz--;
			q0 -= 24;
		}
	} else { /* break z into 24-bit if necessary */
		z = scalbn(z,-q0);
		if (z >= 0x1p24) {
			fw = (double)(int32_t)(0x1p-24*z);
			iq[jz] = (int32_t)(z - 0x1p24*fw);
			jz += 1;
			q0 += 24;
			iq[jz] = (int32_t)fw;
		} else
			iq[jz] = (int32_t)z;
	}

	/* convert integer "bit" chunk to floating-point value */
	fw = scalbn(1.0,q0);
	for (i=jz; i>=0; i--) {
		q[i] = fw*(double)iq[i];
		fw *= 0x1p-24;
	}

	/* compute PIo2[0,...,jp]*q[jz,...,0] */
	for(i=jz; i>=0; i--) {
		for (fw=0.0,k=0; k<=jp && k<=jz-i; k++)
			fw += PIo2[k]*q[i+k];
		fq[jz-i] = fw;
	}

	/* compress fq[] into y[] */
	switch(prec) {
	case 0:
		fw = 0.0;
		for (i=jz; i>=0; i--)
			fw += fq[i];
		y[0] = ih==0 ? fw : -fw;
		break;
	case 1:
	case 2:
		fw = 0.0;
		for (i=jz; i>=0; i--)
			fw += fq[i];
		// TODO: drop excess precision here once double_t is used
		fw = (double)fw;
		y[0] = ih==0 ? fw : -fw;
		fw = fq[0]-fw;
		for (i=1; i<=jz; i++)
			fw += fq[i];
		y[1] = ih==0 ? fw : -fw;
		break;
	case 3:  /* painful */
		for (i=jz; i>0; i--) {
			fw      = fq[i-1]+fq[i];
			fq[i]  += fq[i-1]-fw;
			fq[i-1] = fw;
		}
		for (i=jz; i>1; i--) {
			fw      = fq[i-1]+fq[i];
			fq[i]  += fq[i-1]-fw;
			fq[i-1] = fw;
		}
		for (fw=0.0,i=jz; i>=2; i--)
			fw += fq[i];
		if (ih==0) {
			y[0] =  fq[0]; y[1] =  fq[1]; y[2] =  fw;
		} else {
			y[0] = -fq[0]; y[1] = -fq[1]; y[2] = -fw;
		}
	}
	return n&7;
}

int __rem_pio2f(float x, double *y) {
	/*
	 * invpio2:  53 bits of 2/pi
	 * pio2_1:   first 25 bits of pi/2
	 * pio2_1t:  pi/2 - pio2_1
	 */
	static const double toint = 1.5/EPS,
			invpio2 = 6.36619772367581382433e-01, /* 0x3FE45F30, 0x6DC9C883 */
			pio2_1  = 1.57079631090164184570e+00, /* 0x3FF921FB, 0x50000000 */
			pio2_1t = 1.58932547735281966916e-08; /* 0x3E5110b4, 0x611A6263 */

	union {float f; uint32_t i;} u = {x};
	double tx[1],ty[1];
	double fn;
	uint32_t ix;
	int n, sign, e0;

	ix = u.i & 0x7fffffff;
	/* 25+53 bit pi is good enough for medium size */
	if (ix < 0x4dc90fdb) {  /* |x| ~< 2^28*(pi/2), medium size */
		/* Use a specialized rint() to get fn.  Assume round-to-nearest. */
		fn = (double)x*invpio2 + toint - toint;
		n  = (int32_t)fn;
		*y = x - fn*pio2_1 - fn*pio2_1t;
		return n;
	}
	if(ix>=0x7f800000) {  /* x is inf or NaN */
		*y = x-x;
		return 0;
	}
	/* scale x into [2^23, 2^24-1] */
	sign = u.i>>31;
	e0 = (ix>>23) - (0x7f+23);  /* e0 = ilogb(|x|)-23, positive */
	u.i = ix - (e0<<23);
	tx[0] = u.f;
	n  =  __rem_pio2_large(tx,ty,e0,1,0);
	if (sign) {
		*y = -ty[0];
		return -n;
	}
	*y = ty[0];
	return n;
}

double cos(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// Taken from musl. See musl for the license/copyright!
float cosf(float x) {
	/* Small multiples of pi/2 rounded to double precision. */
	static const double c1pio2 = 1*M_PI_2, /* 0x3FF921FB, 0x54442D18 */
			c2pio2 = 2*M_PI_2, /* 0x400921FB, 0x54442D18 */
			c3pio2 = 3*M_PI_2, /* 0x4012D97C, 0x7F3321D2 */
			c4pio2 = 4*M_PI_2; /* 0x401921FB, 0x54442D18 */

	double y;
	uint32_t ix;
	unsigned n, sign;

	GET_FLOAT_WORD(ix, x);
	sign = ix >> 31;
	ix &= 0x7fffffff;

	if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
		if (ix < 0x39800000) {  /* |x| < 2**-12 */
			/* raise inexact if x != 0 */
			FORCE_EVAL(x + 0x1p120f);
			return 1.0f;
		}
		return __cosdf(x);
	}
	if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
		if (ix > 0x4016cbe3)  /* |x|  ~> 3*pi/4 */
			return -__cosdf(sign ? x+c2pio2 : x-c2pio2);
		else {
			if (sign)
				return __sindf(x + c1pio2);
			else
				return __sindf(c1pio2 - x);
		}
	}
	if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
		if (ix > 0x40afeddf)  /* |x| ~> 7*pi/4 */
			return __cosdf(sign ? x+c4pio2 : x-c4pio2);
		else {
			if (sign)
				return __sindf(-x - c3pio2);
			else
				return __sindf(x - c3pio2);
		}
	}

	/* cos(Inf or NaN) is NaN */
	if (ix >= 0x7f800000)
		return x-x;

	/* general argument reduction needed */
	n = __rem_pio2f(x,&y);
	switch (n&3) {
	case 0: return  __cosdf(y);
	case 1: return  __sindf(-y);
	case 2: return -__cosdf(y);
	default:
		return  __sindf(y);
	}
}
long double cosl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double sin(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// Taken from musl. See musl for the license/copyright!
float sinf(float x) {
	/* Small multiples of pi/2 rounded to double precision. */
	static const double s1pio2 = 1*M_PI_2, /* 0x3FF921FB, 0x54442D18 */
		s2pio2 = 2*M_PI_2, /* 0x400921FB, 0x54442D18 */
		s3pio2 = 3*M_PI_2, /* 0x4012D97C, 0x7F3321D2 */
		s4pio2 = 4*M_PI_2; /* 0x401921FB, 0x54442D18 */

	double y;
	uint32_t ix;
	int n, sign;

	GET_FLOAT_WORD(ix, x);
	sign = ix >> 31;
	ix &= 0x7fffffff;

	if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
		if (ix < 0x39800000) {  /* |x| < 2**-12 */
			/* raise inexact if x!=0 and underflow if subnormal */
			FORCE_EVAL(ix < 0x00800000 ? x/0x1p120f : x+0x1p120f);
			return x;
		}
		return __sindf(x);
	}
	if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
		if (ix <= 0x4016cbe3) {  /* |x| ~<= 3pi/4 */
			if (sign)
				return -__cosdf(x + s1pio2);
			else
				return __cosdf(x - s1pio2);
		}
		return __sindf(sign ? -(x + s2pio2) : -(x - s2pio2));
	}
	if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
		if (ix <= 0x40afeddf) {  /* |x| ~<= 7*pi/4 */
			if (sign)
				return __cosdf(x + s3pio2);
			else
				return -__cosdf(x - s3pio2);
		}
		return __sindf(sign ? x + s4pio2 : x - s4pio2);
	}

	/* sin(Inf or NaN) is NaN */
	if (ix >= 0x7f800000)
		return x - x;

	/* general argument reduction needed */
	n = __rem_pio2f(x, &y);
	switch (n&3) {
	case 0: return  __sindf(y);
	case 1: return  __cosdf(y);
	case 2: return  __sindf(-y);
	default:
		return -__cosdf(y);
	}
}
long double sinl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double tan(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float tanf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double tanl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double acosh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float acoshf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double acoshl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double asinh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float asinhf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double asinhl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double atanh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float atanhf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double atanhl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double cosh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float coshf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double coshl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double sinh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float sinhf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double sinhl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double tanh(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float tanhf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double tanhl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double exp(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float expf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double expl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double exp2(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// Taken from musl. See musl for the license/copyright!
float exp2f(float x) {
	constexpr int TBLSIZE = 16;

	constexpr float redux = 0x1.8p23f / TBLSIZE;
	constexpr float P1 = 0x1.62e430p-1f;
	constexpr float P2 = 0x1.ebfbe0p-3f;
	constexpr float P3 = 0x1.c6b348p-5f;
	constexpr float P4 = 0x1.3b2c9cp-7f;

	constexpr double exp2ft[TBLSIZE] = {
		0x1.6a09e667f3bcdp-1,
		0x1.7a11473eb0187p-1,
		0x1.8ace5422aa0dbp-1,
		0x1.9c49182a3f090p-1,
		0x1.ae89f995ad3adp-1,
		0x1.c199bdd85529cp-1,
		0x1.d5818dcfba487p-1,
		0x1.ea4afa2a490dap-1,
		0x1.0000000000000p+0,
		0x1.0b5586cf9890fp+0,
		0x1.172b83c7d517bp+0,
		0x1.2387a6e756238p+0,
		0x1.306fe0a31b715p+0,
		0x1.3dea64c123422p+0,
		0x1.4bfdad5362a27p+0,
		0x1.5ab07dd485429p+0,
	};

	double t, r, z;
	union {float f; uint32_t i;} u = {x};
	union {double f; uint64_t i;} uk;
	uint32_t ix, i0, k;

	/* Filter out exceptional cases. */
	ix = u.i & 0x7fffffff;
	if (ix > 0x42fc0000) {  /* |x| > 126 */
		if (ix > 0x7f800000) /* NaN */
			return x;
		if (u.i >= 0x43000000 && u.i < 0x80000000) {  /* x >= 128 */
			x *= 0x1p127f;
			return x;
		}
		if (u.i >= 0x80000000) {  /* x < -126 */
			if (u.i >= 0xc3160000 || (u.i & 0x0000ffff))
				FORCE_EVAL(-0x1p-149f/x);
			if (u.i >= 0xc3160000)  /* x <= -150 */
				return 0;
		}
	} else if (ix <= 0x33000000) {  /* |x| <= 0x1p-25 */
		return 1.0f + x;
	}

	/* Reduce x, computing z, i0, and k. */
	u.f = x + redux;
	i0 = u.i;
	i0 += TBLSIZE / 2;
	k = i0 / TBLSIZE;
	uk.i = (uint64_t)(0x3ff + k)<<52;
	i0 &= TBLSIZE - 1;
	u.f -= redux;
	z = x - u.f;
	/* Compute r = exp2(y) = exp2ft[i0] * p(z). */
	r = exp2ft[i0];
	t = r * z;
	r = r + t * (P1 + z * P2) + t * (z * z) * (P3 + z * P4);

	/* Scale by 2**k */
	return r * uk.f;
}
long double exp2l(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double expm1(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float expm1f(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double expm1l(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double frexp(double x, int *power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float frexpf(float x, int *power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double frexpl(long double x, int *power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double ilogb(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float ilogbf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double ilogbl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double ldexp(double x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float ldexpf(float x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double ldexpl(long double x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double log(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float logf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double logl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double log10(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float log10f(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double log10l(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double log1p(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float log1pf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double log1pl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// Taken from musl. See musl for the license/copyright!
double log2(double x) {
	static const double
		ivln2hi = 1.44269504072144627571e+00, /* 0x3ff71547, 0x65200000 */
		ivln2lo = 1.67517131648865118353e-10, /* 0x3de705fc, 0x2eefa200 */
		Lg1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
		Lg2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
		Lg3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
		Lg4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
		Lg5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
		Lg6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
		Lg7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */

	union {double f; uint64_t i;} u = {x};
	double hfsq,f,s,z,R,w,t1,t2,y,hi,lo,val_hi,val_lo;
	uint32_t hx;
	int k;

	hx = u.i>>32;
	k = 0;
	if (hx < 0x00100000 || hx>>31) {
		if (u.i<<1 == 0)
			return -1/(x*x);  /* log(+-0)=-inf */
		if (hx>>31)
			return (x-x)/0.0; /* log(-#) = NaN */
		/* subnormal number, scale x up */
		k -= 54;
		x *= 0x1p54;
		u.f = x;
		hx = u.i>>32;
	} else if (hx >= 0x7ff00000) {
		return x;
	} else if (hx == 0x3ff00000 && u.i<<32 == 0)
		return 0;

	/* reduce x into [sqrt(2)/2, sqrt(2)] */
	hx += 0x3ff00000 - 0x3fe6a09e;
	k += (int)(hx>>20) - 0x3ff;
	hx = (hx&0x000fffff) + 0x3fe6a09e;
	u.i = (uint64_t)hx<<32 | (u.i&0xffffffff);
	x = u.f;

	f = x - 1.0;
	hfsq = 0.5*f*f;
	s = f/(2.0+f);
	z = s*s;
	w = z*z;
	t1 = w*(Lg2+w*(Lg4+w*Lg6));
	t2 = z*(Lg1+w*(Lg3+w*(Lg5+w*Lg7)));
	R = t2 + t1;

	/*
	 * f-hfsq must (for args near 1) be evaluated in extra precision
	 * to avoid a large cancellation when x is near sqrt(2) or 1/sqrt(2).
	 * This is fairly efficient since f-hfsq only depends on f, so can
	 * be evaluated in parallel with R.  Not combining hfsq with R also
	 * keeps R small (though not as small as a true `lo' term would be),
	 * so that extra precision is not needed for terms involving R.
	 *
	 * Compiler bugs involving extra precision used to break Dekker's
	 * theorem for spitting f-hfsq as hi+lo, unless double_t was used
	 * or the multi-precision calculations were avoided when double_t
	 * has extra precision.  These problems are now automatically
	 * avoided as a side effect of the optimization of combining the
	 * Dekker splitting step with the clear-low-bits step.
	 *
	 * y must (for args near sqrt(2) and 1/sqrt(2)) be added in extra
	 * precision to avoid a very large cancellation when x is very near
	 * these values.  Unlike the above cancellations, this problem is
	 * specific to base 2.  It is strange that adding +-1 is so much
	 * harder than adding +-ln2 or +-log10_2.
	 *
	 * This uses Dekker's theorem to normalize y+val_hi, so the
	 * compiler bugs are back in some configurations, sigh.  And I
	 * don't want to used double_t to avoid them, since that gives a
	 * pessimization and the support for avoiding the pessimization
	 * is not yet available.
	 *
	 * The multi-precision calculations for the multiplications are
	 * routine.
	 */

	/* hi+lo = f - hfsq + s*(hfsq+R) ~ log(1+f) */
	hi = f - hfsq;
	u.f = hi;
	u.i &= (uint64_t)-1<<32;
	hi = u.f;
	lo = f - hi - hfsq + s*(hfsq+R);

	val_hi = hi*ivln2hi;
	val_lo = (lo+hi)*ivln2lo + lo*ivln2hi;

	/* spadd(val_hi, val_lo, y), except for not using double_t: */
	y = k;
	w = y + val_hi;
	val_lo += (y - w) + val_hi;
	val_hi = w;

	return val_lo + val_hi;
}
float log2f(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double log2l(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double logb(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float logbf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double logbl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double modf(double x, double *integral) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float modff(float x, float *integral) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double modfl(long double x, long double *integral) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double scalbn(double x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float scalbnf(float x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double scalbnl(long double x, int power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double scalbln(double x, long power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float scalblnf(float x, long power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double scalblnl(long double x, long power) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double cbrt(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float cbrtf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double cbrtl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double fabs(double x) {
	return signbit(x) ? -x : x;
}
float fabsf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double fabsl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double hypot(double x, double y) {
	__ensure(isfinite(x));
	__ensure(isfinite(y));
	// TODO: fix exception handling
	double u = fabs(x);
	double v = fabs(y);
	if(u > v)
		return u * sqrt(1 + (v / u) * (v / u));
	return v * sqrt(1 + (u / v) * (u / v));
}
float hypotf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double hypotl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double pow(double x, double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float powf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double powl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double sqrt(double x) {
	auto sse_x = _mm_set_sd(x);
	return _mm_cvtsd_f64(_mm_sqrt_sd(sse_x, sse_x));
}
float sqrtf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double sqrtl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double erf(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float erff(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double erfl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double erfc(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float erfcf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double erfcl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double lgamma(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float lgammaf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double lgammal(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double tgamma(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float tgammaf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double tgammal(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double ceil(double x) {
	auto soft_x = ieee754::extractNative(x);
	auto result = ieee754::ceil(soft_x);
	return ieee754::compileNative(result);
}
float ceilf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double ceill(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double floor(double x) {
	auto soft_x = ieee754::extractNative(x);
	auto result = ieee754::floor(soft_x);
	return ieee754::compileNative(result);
}
float floorf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double floorl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double nearbyint(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float nearbyintf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double nearbyintl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double rint(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float rintf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double rintl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long lrint(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long lrintf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long lrintl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long long llrint(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long long llrintf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long long llrintl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double round(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float roundf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double roundl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long lround(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long lroundf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long lroundl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long long llround(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long long llroundf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long long llroundl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double trunc(double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float truncf(float x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double truncl(long double x) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double fmod(double x, double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float fmodf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double fmodl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double remainder(double x, double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float remainderf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double remainderl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double remquo(double x, double y, int *quotient) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float remquof(float x, float y, int *quotient) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double remquol(long double x, long double y, int *quotient) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double copysign(double x, double sign) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float copysignf(float x, float sign) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double copysignl(long double x, long double sign) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double nan(const char *tag) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float nanf(const char *tag) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double nanl(const char *tag) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double nextafter(double x, double dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float nextafterf(float x, float dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double nextafterl(long double x, long double dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double nexttoward(double x, long double dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float nexttowardf(float x, long double dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double nexttowardl(long double x, long double dir) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double fdim(double x, double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float fdimf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double fdiml(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double fmax(double x, double y) {
	__ensure(isfinite(x) && isfinite(y));
	return x < y ? y : x;
}
float fmaxf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double fmaxl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

double fmin(double x, double y) {
	__ensure(isfinite(x) && isfinite(y));
	return x < y ? x : y;
}
float fminf(float x, float y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double fminl(long double x, long double y) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

//gnu extension

void sincos(double, double *, double *) {
	__ensure(!"sincos() not implemented");
	__builtin_unreachable();
}

void sincosf(float x, float *sx, float *cx) {
	// This is a lazy implementation.
	__ensure(sx);
	__ensure(cx);
	*sx = sinf(x);
	*cx = cosf(x);
}
void sincosl(long double, long double *, long double *) {
	__ensure(!"sincosl() not implemented");
	__builtin_unreachable();
}

double exp10(double) {
	__ensure(!"exp10() not implemented");
	__builtin_unreachable();
}
float exp10f(float) {
	__ensure(!"exp10f() not implemented");
	__builtin_unreachable();
}
long double exp10l(long double) {
	__ensure(!"exp10l() not implemented");
	__builtin_unreachable();
}

double pow10(double) {
	__ensure(!"pow10() not implemented");
	__builtin_unreachable();
}
float pow10f(float) {
	__ensure(!"pow10f() not implemented");
	__builtin_unreachable();
}
long double pow10l(long double) {
	__ensure(!"pow10l() not implemented");
	__builtin_unreachable();
}
