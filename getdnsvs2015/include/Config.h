/* 
 * Collating here a bunch of fixes needed to compile GetDNS in 
 * Visual Studio. 
 */
#ifndef CH_WINDOWS_CONFIG
#define CH_WINDOWS_CONFIG
/*
 * Set the option STUB_NATIVE_DNSSEC in VS file to avoid the
 * reference to context->unbound.
 * Adding the ifdef key "CH_WINDOWS_PORT" to track changes in
 * the source files.
 */

#ifndef GETDNS_ON_WINDOWS
#define GETDNS_ON_WINDOWS
#endif

/* TODO: windows treads. May need to import code from unboud*/
#if 0
#ifndef HAVE_WINDOWS_THREADS
#define HAVE_WINDOWS_THREADS
#endif
#endif

#ifndef DNSSEC_ROADBLOCK_AVOIDANCE
#define DNSSEC_ROADBLOCK_AVOIDANCE
#endif

#ifndef STUB_NATIVE_DNSSEC
#define STUB_NATIVE_DNSSEC
#endif

#ifndef GLDNS_BUILD_CONFIG_HAVE_SSL
#define GLDNS_BUILD_CONFIG_HAVE_SSL 1
#endif

#ifndef HAVE_OPENSSL_ERR_H
#define HAVE_OPENSSL_ERR_H 1
#endif

#ifndef HAVE_MDNS_SUPPORT
#define HAVE_MDNS_SUPPORT
#endif

#ifndef HAVE_EVP_MD_CTX_NEW
#define HAVE_EVP_MD_CTX_NEW
#endif

#ifndef HAVE_HMAC_CTX_NEW
#define HAVE_HMAC_CTX_NEW
#endif

#ifndef HAVE_HMAC_CTX_FREE
#define HAVE_HMAC_CTX_FREE
#endif

#ifndef HAVE_TLS_v1_2
#define HAVE_TLS_v1_2
#endif

#ifndef HAVE_WINDOWS_THREADS
#define HAVE_WINDOWS_THREADS
#endif

#if 1
#define USE_POLL_DEFAULT_EVENTLOOP
#endif
/*
 * Bypassing some of the security warnings for now. This is of
 * course dangerous. One of the potential benefits of compiling 
 * under Visual Studio is to benefit from the stricter requirements
 * of the MS compiler, and better the source.
 */
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef USE_WINSOCK
#define USE_WINSOCK

#include <WinSock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#endif

/*
 * These include files should be obtained from the actual SSL sources.
 * The version in the "compiled" archive does not actually define the types.
 */
#include <openssl/ossl_typ.h>
#include <openssl/ssl.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

/*
 * These macros are used in the code, but not defined for winsock2.
 */
#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif
#define FD_SETSIZE 1024

#ifndef FD_SET_T 
#define FD_SET_T 
#endif

#ifndef socklen_t
#define socklen_t size_t
#endif

#ifndef ssize_t
#define ssize_t size_t
#endif

#ifndef pid_t
#define pid_t int
#endif

#ifndef ATTR_FORMAT
#define ATTR_FORMAT(x, y, z)  // __attribute__((format(x, y, z)))
#endif

/*
 * adding a few definitions to remove compiler warnings
 */
#ifdef __cplusplus
extern "C" {
#endif
	int gettimeofday(struct timeval* tv, struct timezone* tz);
	size_t strlcpy(char *dst, const char *src, size_t siz);
	uint32_t arc4random_uniform(uint32_t upper_bound);
	uint32_t arc4random(void);
	int getentropy(void *buf, size_t len);
	void _ARC4_LOCK(void);
	void _ARC4_UNLOCK(void);
	int _getpid(void);
	void explicit_bzero(void *buf, size_t len);
#ifdef __cplusplus
}
#endif


#define __attribute__(weak) /* ignored attributes */
#define ATTR_UNUSED(buf) buf /* ignored attribute */
/*
 * Mapping string comparison functions
 */
#define strcasecmp(x,y) _stricmp(x,y)
#define strncasecmp(x,y,z) _strnicmp(x,y,z)

/*
 * EDNS constants defined in the code, not visible in the Windows version
 */

#ifndef EDNS_COOKIE_ROLLOVER_TIME
#define EDNS_COOKIE_ROLLOVER_TIME 1000
#endif // !EDNS_COOKIE_ROLLOVER_TIME

#ifndef EDNS_COOKIE_OPCODE
#define EDNS_COOKIE_OPCODE 10
#endif // EDNS_COOKIE_OPCODE

#ifndef EDNS_PADDING_OPCODE
#define EDNS_PADDING_OPCODE 12
#endif

#ifndef MAX_CNAME_REFERRALS 
#define MAX_CNAME_REFERRALS 16
#endif

#ifndef  MAXIMUM_UPSTREAM_OPTION_SPACE
#define MAXIMUM_UPSTREAM_OPTION_SPACE 256
#endif // ! MAXIMUM_UPSTREAM_OPTION_SPACE

/* 
 * Configuration parameters should be set by external system, but
 * that system is currently not developped.
 */
#ifndef TRUST_ANCHOR_FILE
#define TRUST_ANCHOR_FILE "\\etc\\unbound\\getdns-root.key"
#endif

#ifndef PACKAGE_URL
#define PACKAGE_URL "http://www.example.com/getDNS"
#endif

#if !defined(HAVE_SSL) && !defined(HAVE_NSS) && !defined(HAVE_NETTLE)
#define HAVE_SSL
#endif

#endif /* !CH_WINDOWS_CONFIG */