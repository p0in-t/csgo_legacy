//========= Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

// NOTE: old_bf_read is guaranteed to return zeros if it overflows.

#include "Includes.h"


//-----------------------------------------------------------------------------
// Forward declarations.
//-----------------------------------------------------------------------------

class Vector;
class QAngle;

//-----------------------------------------------------------------------------
// You can define a handler function that will be called in case of 
// out-of-range values and overruns here.
//
// NOTE: the handler is only called in debug mode.
//
// Call SetBitBufErrorHandler to install a handler.
//-----------------------------------------------------------------------------

#define WordSwap  WordSwapAsm
#define DWordSwap DWordSwapAsm
#define QWordSwap QWordSwapC
#define ANALYZE_SUPPRESS(wnum)
#define ANALYZE_SUPPRESS2(wnum1, wnum2)
#define ANALYZE_SUPPRESS3(wnum1, wnum2, wnum3)
#define ANALYZE_SUPPRESS4(wnum1, wnum2, wnum3, wnum4)
#define PRINTF_FORMAT_STRING
#define SCANF_FORMAT_STRING
#define IN_Z
#define IN_CAP(x)
#define IN_BYTECAP(x)
#define OUT_Z_CAP(x)
#define OUT_CAP(x)
#define OUT_BYTECAP(x)
#define OUT_Z_BYTECAP(x)
#define INOUT_BYTECAP(x)
#define INOUT_Z_CAP(x)
#define INOUT_Z_BYTECAP(x)
#define OUT_Z_ARRAY
#define INOUT_Z_ARRAY

#pragma warning(push)
#pragma warning (disable:4035) // no return value

#ifndef M_PI
#define M_PI			3.14159265358979323846
#endif


// #define COMPILETIME_MAX and COMPILETIME_MIN for max/min in constant expressions
#define COMPILETIME_MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#define COMPILETIME_MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#ifndef MIN
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

#ifndef MAX
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif

template <typename T>
inline T QWordSwapC(T dw)
{
	std::uint64_t temp;

	temp = *((std::uint64_t*)&dw) >> 56;
	temp |= ((*((std::uint64_t*)&dw) & 0x00FF000000000000ull) >> 40);
	temp |= ((*((std::uint64_t*)&dw) & 0x0000FF0000000000ull) >> 24);
	temp |= ((*((std::uint64_t*)&dw) & 0x000000FF00000000ull) >> 8);
	temp |= ((*((std::uint64_t*)&dw) & 0x00000000FF000000ull) << 8);
	temp |= ((*((std::uint64_t*)&dw) & 0x0000000000FF0000ull) << 24);
	temp |= ((*((std::uint64_t*)&dw) & 0x000000000000FF00ull) << 40);
	temp |= ((*((std::uint64_t*)&dw) & 0x00000000000000FFull) << 56);

	return *((T*)&temp);
}

template <typename T>
inline T WordSwapAsm(T w)
{
	__asm
	{
		mov ax, w
		xchg al, ah
	}
}

template <typename T>
inline T DWordSwapAsm(T dw)
{
	__asm
	{
		mov eax, dw
		bswap eax
	}
}


#define SafeSwapFloat( pOut, pIn )	(*((std::uint_t*)pOut) = DWordSwap( *((std::uint_t*)pIn) ))
#define BigFloat( pOut, pIn )		SafeSwapFloat( pOut, pIn )
#define LittleFloat( pOut, pIn )	( *pOut = *pIn )
#define SwapFloat( pOut, pIn )		BigFloat( pOut, pIn )
#define BigShort( val )				WordSwap( val )
#define BigWord( val )				WordSwap( val )
#define BigLong( val )				DWordSwap( val )
#define BigDWord( val )				DWordSwap( val )
#define BigQWord( val )				QWordSwap( val ) 
#define LittleShort( val )			( val )
#define LittleWord( val )			( val )
#define LittleLong( val )			( val )
#define LittleDWord( val )			( val )
#define LittleQWord( val )			( val )

#define SwapShort( val )			BigShort( val )
#define SwapWord( val )				BigWord( val )
#define SwapLong( val )				BigLong( val )
#define SwapDWord( val )			BigDWord( val )

inline std::uint32_t LoadLittleDWord(std::uint32_t* base, unsigned int dwordIndex)
{
	return LittleDWord(base[dwordIndex]);
}

inline void StoreLittleDWord(std::uint32_t* base, unsigned int dwordIndex, std::uint32_t dword)
{
	base[dwordIndex] = LittleDWord(dword);
}

typedef enum
{
	BITBUFERROR_VALUE_OUT_OF_RANGE=0,		// Tried to write a value with too few bits.
	BITBUFERROR_BUFFER_OVERRUN,				// Was about to overrun a buffer.

	BITBUFERROR_NUM_ERRORS
} BitBufErrorType;


typedef void (*BitBufErrorHandler)( BitBufErrorType errorType, const char *pDebugName );


#if defined( _DEBUG )
	extern void InternalBitBufErrorHandler( BitBufErrorType errorType, const char *pDebugName );
	#define CallErrorHandler( errorType, pDebugName ) InternalBitBufErrorHandler( errorType, pDebugName );
#else
	#define CallErrorHandler( errorType, pDebugName )
#endif


// Use this to install the error handler. Call with NULL to uninstall your error handler.
void SetBitBufErrorHandler( BitBufErrorHandler fn );


//-----------------------------------------------------------------------------
// Helpers.
//-----------------------------------------------------------------------------

inline int BitByte( int bits )
{
	// return PAD_NUMBER( bits, 8 ) >> 3;
	return (bits + 7) >> 3;
}

//-----------------------------------------------------------------------------
enum EBitCoordType
{
	kCW_None,
	kCW_LowPrecision,
	kCW_Integral
};

//-----------------------------------------------------------------------------
// namespaced helpers
//-----------------------------------------------------------------------------
namespace bitbuf
{
	// ZigZag Transform:  Encodes signed integers so that they can be
	// effectively used with varint encoding.
	//
	// varint operates on unsigned integers, encoding smaller numbers into
	// fewer bytes.  If you try to use it on a signed integer, it will treat
	// this number as a very large unsigned integer, which means that even
	// small signed numbers like -1 will take the maximum number of bytes
	// (10) to encode.  ZigZagEncode() maps signed integers to unsigned
	// in such a way that those with a small absolute value will have smaller
	// encoded values, making them appropriate for encoding using varint.
	//
	//       int32 ->     std::uint32_t
	// -------------------------
	//           0 ->          0
	//          -1 ->          1
	//           1 ->          2
	//          -2 ->          3
	//         ... ->        ...
	//  2147483647 -> 4294967294
	// -2147483648 -> 4294967295
	//
	//        >> encode >>
	//        << decode <<

	inline std::uint32_t ZigZagEncode32(std::int32_t n) 
	{
		// Note:  the right-shift must be arithmetic
		return(n << 1) ^ (n >> 31);
	}

	inline std::int32_t ZigZagDecode32(std::uint32_t n)
	{
		return(n >> 1) ^ -static_cast<std::int32_t>(n & 1);
	}

	inline std::uint64_t ZigZagEncode64(std::int64_t n) 
	{
		// Note:  the right-shift must be arithmetic
		return(n << 1) ^ (n >> 63);
	}

	inline std::int64_t ZigZagDecode64(std::uint64_t n) 
	{
		return(n >> 1) ^ -static_cast<std::int64_t>(n & 1);
	}

	const int kMaxVarintBytes = 10;
	const int kMaxVarint32Bytes = 5;
}

//-----------------------------------------------------------------------------
// Used for serialization
//-----------------------------------------------------------------------------

class bf_write
{
public:
	bf_write();
					
					// nMaxBits can be used as the number of bits in the buffer. 
					// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	bf_write( void *pData, int nBytes, int nMaxBits = -1 );
	bf_write( const char *pDebugName, void *pData, int nBytes, int nMaxBits = -1 );

	// Start writing to the specified buffer.
	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	void			StartWriting( void *pData, int nBytes, int iStartBit = 0, int nMaxBits = -1 );

	// Restart buffer writing.
	void			Reset();

	// Get the base pointer.
	unsigned char*	GetBasePointer() { return m_pData; }

	// Enable or disable assertion on overflow. 99% of the time, it's a bug that we need to catch,
	// but there may be the occasional buffer that is allowed to overflow gracefully.
	void			SetAssertOnOverflow( bool bAssert );

	// This can be set to assign a name that gets output if the buffer overflows.
	const char*		GetDebugName();
	void			SetDebugName( const char *pDebugName );


// Seek to a specific position.
public:
	
	void			SeekToBit( int bitPos );


// Bit functions.
public:

	void			WriteOneBit(int nValue);
	void			WriteOneBitNoCheck(int nValue);
	void			WriteOneBitAt( int iBit, int nValue );
	
	// Write signed or unsigned. Range is only checked in debug.
	void			WriteUBitLong( unsigned int data, int numbits, bool bCheckRange=true );
	void			WriteSBitLong( int data, int numbits );
	
	// Tell it whether or not the data is unsigned. If it's signed,
	// cast to unsigned before passing in (it will cast back inside).
	void			WriteBitLong(unsigned int data, int numbits, bool bSigned);

	// Write a list of bits in.
	bool			WriteBits(const void *pIn, int nBits);

	// writes an unsigned integer with variable bit length
	void			WriteUBitVar( unsigned int data );

	// writes a varint encoded integer
	void			WriteVarInt32( std::uint32_t data );
	void			WriteVarInt64( std::uint64_t data );
	void			WriteSignedVarInt32( std::int32_t data );
	void			WriteSignedVarInt64( std::int64_t data );
	int				ByteSizeVarInt32( std::uint32_t data );
	int				ByteSizeVarInt64(std::uint64_t data );
	int				ByteSizeSignedVarInt32(std::int32_t data );
	int				ByteSizeSignedVarInt64(std::int64_t data );

	// Copy the bits straight out of pIn. This seeks pIn forward by nBits.
	// Returns an error if this buffer or the read buffer overflows.
	bool			WriteBitsFromBuffer( class bf_read *pIn, int nBits );
	
	void			WriteBitAngle( float fAngle, int numbits );
	void			WriteBitCoord (const float f);
	void			WriteBitCoordMP( const float f, EBitCoordType coordType );
	void 			WriteBitCellCoord( const float f, int bits, EBitCoordType coordType );
	void			WriteBitFloat(float val);
	void			WriteBitVec3Coord( const Vector& fa );
	void			WriteBitNormal( float f );
	void			WriteBitVec3Normal( const Vector& fa );
	void			WriteBitAngles( const QAngle& fa );


// Byte functions.
public:

	void			WriteChar(int val);
	void			WriteByte( unsigned int val );
	void			WriteShort(int val);
	void			WriteWord( unsigned int val );
	void			WriteLong(std::int32_t val);
	void			WriteLongLong(std::int64_t val);
	void			WriteFloat(float val);
	bool			WriteBytes( const void *pBuf, int nBytes );

	// Returns false if it overflows the buffer.
	bool			WriteString(const char *pStr);
	bool			WriteString(const wchar_t *pStr);


// Status.
public:

	// How many bytes are filled in?
	int				GetNumBytesWritten() const;
	int				GetNumBitsWritten() const;
	int				GetMaxNumBits() const;
	int				GetNumBitsLeft() const;
	int				GetNumBytesLeft() const;
	unsigned char*	GetData();
	const unsigned char*	GetData() const;

	// Has the buffer overflowed?
	bool			CheckForOverflow(int nBits);
	inline bool		IsOverflowed() const {return m_bOverflow;}

	inline void		SetOverflowFlag();

public:
	// The current buffer.
	unsigned char*	m_pData;
	int				m_nDataBytes;
	int				m_nDataBits;
	
	// Where we are in the buffer.
	int				m_iCurBit;
	
private:

	// Errors?
	bool			m_bOverflow;

	bool			m_bAssertOnOverflow;
	const char		*m_pDebugName;
};


//-----------------------------------------------------------------------------
// Inlined methods
//-----------------------------------------------------------------------------

// How many bytes are filled in?
inline int bf_write::GetNumBytesWritten() const
{
	return BitByte(m_iCurBit);
}

inline int bf_write::GetNumBitsWritten() const
{
	return m_iCurBit;
}

inline int bf_write::GetMaxNumBits() const
{
	return m_nDataBits;
}

inline int bf_write::GetNumBitsLeft() const
{
	return m_nDataBits - m_iCurBit;
}

inline int bf_write::GetNumBytesLeft() const
{
	return GetNumBitsLeft() >> 3;
}

inline unsigned char* bf_write::GetData()			
{
	return m_pData;
}

inline const unsigned char* bf_write::GetData() const
{
	return m_pData;
}

inline bool bf_write::CheckForOverflow(int nBits)
{
	if ( m_iCurBit + nBits > m_nDataBits )
	{
		SetOverflowFlag();
		CallErrorHandler( BITBUFERROR_BUFFER_OVERRUN, GetDebugName() );
	}
	
	return m_bOverflow;
}

inline void bf_write::SetOverflowFlag()
{
	m_bOverflow = true;
}

inline void bf_write::WriteOneBitNoCheck(int nValue)
{
	if(nValue)
		m_pData[m_iCurBit >> 3] |= (1 << (m_iCurBit & 7));
	else
		m_pData[m_iCurBit >> 3] &= ~(1 << (m_iCurBit & 7));

	++m_iCurBit;
}

inline void bf_write::WriteOneBit(int nValue)
{
	if( !CheckForOverflow(1) )
		WriteOneBitNoCheck( nValue );
}


inline void	bf_write::WriteOneBitAt( int iBit, int nValue )
{
	if( iBit+1 > m_nDataBits )
	{
		SetOverflowFlag();
		CallErrorHandler( BITBUFERROR_BUFFER_OVERRUN, GetDebugName() );
		return;
	}

	if( nValue )
		m_pData[iBit >> 3] |= (1 << (iBit & 7));
	else
		m_pData[iBit >> 3] &= ~(1 << (iBit & 7));
}


inline void bf_write::WriteUBitLong( unsigned int curData, int numbits, bool bCheckRange )
{
	extern std::uint32_t g_BitWriteMasks[32][33];

	// Bounds checking..
	if ((m_iCurBit+numbits) > m_nDataBits)
	{
		m_iCurBit = m_nDataBits;
		SetOverflowFlag();
		CallErrorHandler( BITBUFERROR_BUFFER_OVERRUN, GetDebugName() );
		return;
	}

	int nBitsLeft = numbits;
	int iCurBit = m_iCurBit;

	// Mask in a dword.
	unsigned int iDWord = iCurBit >> 5;

	std::uint32_t iCurBitMasked = iCurBit & 31;

	std::uint32_t dword = LoadLittleDWord( (std::uint32_t*)m_pData, iDWord );

	dword &= g_BitWriteMasks[iCurBitMasked][nBitsLeft];
	dword |= curData << iCurBitMasked;

	// write to stream (lsb to msb ) properly
	StoreLittleDWord( (std::uint32_t*)m_pData, iDWord, dword );

	// Did it span a dword?
	int nBitsWritten = 32 - iCurBitMasked;
	if ( nBitsWritten < nBitsLeft )
	{
		nBitsLeft -= nBitsWritten;
		curData >>= nBitsWritten;

		// read from stream (lsb to msb) properly 
		dword = LoadLittleDWord( (std::uint32_t*)m_pData, iDWord+1 );

		dword &= g_BitWriteMasks[0][nBitsLeft];
		dword |= curData;

		// write to stream (lsb to msb) properly 
		StoreLittleDWord( (std::uint32_t*)m_pData, iDWord+1, dword );
	}

	m_iCurBit += numbits;
}


//-----------------------------------------------------------------------------
// This is useful if you just want a buffer to write into on the stack.
//-----------------------------------------------------------------------------

template<int SIZE>
class old_bf_write_static : public bf_write
{
public:
	inline old_bf_write_static() : bf_write(m_StaticData, SIZE) {}

	char	m_StaticData[SIZE];
};



//-----------------------------------------------------------------------------
// Used for unserialization
//-----------------------------------------------------------------------------

class old_bf_read
{
public:
	old_bf_read();

	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	old_bf_read( const void *pData, int nBytes, int nBits = -1 );
	old_bf_read( const char *pDebugName, const void *pData, int nBytes, int nBits = -1 );

	// Start reading from the specified buffer.
	// pData's start address must be dword-aligned.
	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	void			StartReading( const void *pData, int nBytes, int iStartBit = 0, int nBits = -1 );

	// Restart buffer reading.
	void			Reset();

	// Enable or disable assertion on overflow. 99% of the time, it's a bug that we need to catch,
	// but there may be the occasional buffer that is allowed to overflow gracefully.
	void			SetAssertOnOverflow( bool bAssert );

	// This can be set to assign a name that gets output if the buffer overflows.
	const char*		GetDebugName();
	void			SetDebugName( const char *pName );

	void			ExciseBits( int startbit, int bitstoremove );


// Bit functions.
public:
	
	// Returns 0 or 1.
	int				ReadOneBit();


protected:

	unsigned int	CheckReadUBitLong(int numbits);		// For debugging.
	int				ReadOneBitNoCheck();				// Faster version, doesn't check bounds and is inlined.
	bool			CheckForOverflow(int nBits);


public:

	// Get the base pointer.
	const unsigned char*	GetBasePointer() { return m_pData; }

	FORCEINLINE int TotalBytesAvailable( void ) const
	{
		return m_nDataBytes;
	}

	// Read a list of bits in..
	void            ReadBits(void *pOut, int nBits);
	
	float			ReadBitAngle( int numbits );

	unsigned int	ReadUBitLong( int numbits );
	unsigned int	PeekUBitLong( int numbits );
	int				ReadSBitLong( int numbits );

	// reads an unsigned integer with variable bit length
	unsigned int	ReadUBitVar();
	
	// reads a varint encoded integer
	std::uint32_t			ReadVarInt32();
	std::uint64_t			ReadVarInt64();
	std::int32_t			ReadSignedVarInt32();
	std::int64_t			ReadSignedVarInt64();

	// You can read signed or unsigned data with this, just cast to 
	// a signed int if necessary.
	unsigned int	ReadBitLong(int numbits, bool bSigned);
	
	float			ReadBitCoord();
	float			ReadBitCoordMP( EBitCoordType coordType );
	float 			ReadBitCellCoord( int bits, EBitCoordType coordType );
	float			ReadBitFloat();
	float			ReadBitNormal();
	void			ReadBitVec3Coord( Vector& fa );
	void			ReadBitVec3Normal( Vector& fa );
	void			ReadBitAngles( QAngle& fa );


// Byte functions (these still read data in bit-by-bit).
public:
	
	int				ReadChar();
	int				ReadByte();
	int				ReadShort();
	int				ReadWord();
	std::int32_t			ReadLong();
	std::int64_t			ReadLongLong();
	float			ReadFloat();
	bool			ReadBytes(void *pOut, int nBytes);

	// Returns false if bufLen isn't large enough to hold the
	// string in the buffer.
	//
	// Always reads to the end of the string (so you can read the
	// next piece of data waiting).
	//
	// If bLine is true, it stops when it reaches a '\n' or a null-terminator.
	//
	// pStr is always null-terminated (unless bufLen is 0).
	//
	// pOutNumChars is set to the number of characters left in pStr when the routine is 
	// complete (this will never exceed bufLen-1).
	//
	bool			ReadString( char *pStr, int bufLen, bool bLine=false, int *pOutNumChars=NULL );
	bool			ReadWString( wchar_t *pStr, int bufLen, bool bLine=false, int *pOutNumChars=NULL );

	// Reads a string and allocates memory for it. If the string in the buffer
	// is > 2048 bytes, then pOverflow is set to true (if it's not NULL).
	char*			ReadAndAllocateString( bool *pOverflow = 0 );

// Status.
public:
	int				GetNumBytesLeft();
	int				GetNumBytesRead();
	int				GetNumBitsLeft();
	int				GetNumBitsRead() const;

	// Has the buffer overflowed?
	inline bool		IsOverflowed() const {return m_bOverflow;}

	inline bool		Seek(int iBit);					// Seek to a specific bit.
	inline bool		SeekRelative(int iBitDelta);	// Seek to an offset from the current position.

	// Called when the buffer is overflowed.
	inline void		SetOverflowFlag();


public:

	// The current buffer.
	const unsigned char*	m_pData;
	int						m_nDataBytes;
	int						m_nDataBits;
	
	// Where we are in the buffer.
	int				m_iCurBit;


private:	
	// used by varbit reads internally
	inline int CountRunOfZeros();

	// Errors?
	bool			m_bOverflow;

	// For debugging..
	bool			m_bAssertOnOverflow;

	const char		*m_pDebugName;
};

//-----------------------------------------------------------------------------
// Inlines.
//-----------------------------------------------------------------------------

inline int old_bf_read::GetNumBytesRead()	
{
	return BitByte(m_iCurBit);
}

inline int old_bf_read::GetNumBitsLeft()	
{
	return m_nDataBits - m_iCurBit;
}

inline int old_bf_read::GetNumBytesLeft()	
{
	return GetNumBitsLeft() >> 3;
}

inline int old_bf_read::GetNumBitsRead() const
{
	return m_iCurBit;
}

inline void old_bf_read::SetOverflowFlag()
{
	m_bOverflow = true;
}

inline bool old_bf_read::Seek(int iBit)
{
	if(iBit < 0 || iBit > m_nDataBits)
	{
		SetOverflowFlag();
		m_iCurBit = m_nDataBits;
		return false;
	}
	else
	{
		m_iCurBit = iBit;
		return true;
	}
}

// Seek to an offset from the current position.
inline bool	old_bf_read::SeekRelative(int iBitDelta)		
{
	return Seek(m_iCurBit+iBitDelta);
}	

inline bool old_bf_read::CheckForOverflow(int nBits)
{
	if( m_iCurBit + nBits > m_nDataBits )
	{
		SetOverflowFlag();
		CallErrorHandler( BITBUFERROR_BUFFER_OVERRUN, GetDebugName() );
	}

	return m_bOverflow;
}

inline int old_bf_read::ReadOneBitNoCheck()
{
	int value = m_pData[m_iCurBit >> 3] & (1 << (m_iCurBit & 7));
	++m_iCurBit;
	return !!value;
}

inline int old_bf_read::ReadOneBit()
{
	return (!CheckForOverflow(1)) ?	ReadOneBitNoCheck() : 0;
}

inline float old_bf_read::ReadBitFloat()
{
	std::int32_t val;

	if(CheckForOverflow(32))
		return 0.0f;

	int bit = m_iCurBit & 0x7;
	int byte = m_iCurBit >> 3;
	val = m_pData[byte] >> bit;
	val |= ((int)m_pData[byte + 1]) << (8 - bit);
	val |= ((int)m_pData[byte + 2]) << (16 - bit);
	val |= ((int)m_pData[byte + 3]) << (24 - bit);
	if (bit != 0)
		val |= ((int)m_pData[byte + 4]) << (32 - bit);
	m_iCurBit += 32;
	return *((float*)&val);
}


inline unsigned int old_bf_read::ReadUBitLong( int numbits )
{
	extern std::uint32_t g_ExtraMasks[32];

	if ( (m_iCurBit+numbits) > m_nDataBits )
	{
		m_iCurBit = m_nDataBits;
		SetOverflowFlag();
		return 0;
	}

	// Read the current dword.
	int idword1 = m_iCurBit >> 5;
	unsigned int dword1 = LoadLittleDWord( (std::uint32_t*)m_pData, idword1 );

	dword1 >>= (m_iCurBit & 31); // Get the bits we're interested in.

	m_iCurBit += numbits;
	unsigned int ret = dword1;

	// Does it span this dword?
	if ( (m_iCurBit-1) >> 5 == idword1 )
	{
		if (numbits != 32)
			ret &= g_ExtraMasks[numbits];
	}
	else
	{
		int nExtraBits = m_iCurBit & 31;
		unsigned int dword2 = LoadLittleDWord( (std::uint32_t*)m_pData, idword1+1 );

		dword2 &= g_ExtraMasks[nExtraBits];

		// No need to mask since we hit the end of the dword.
		// Shift the second dword's part into the high bits.
		ret |= (dword2 << (numbits - nExtraBits));
	}

	return ret;
}


class CBitBuffer
{
public:
	char const * m_pDebugName;
	bool m_bOverflow;
	int m_nDataBits;
	size_t m_nDataBytes;

	void SetDebugName( char const *pName )
	{
		m_pDebugName = pName;
	}

	CBitBuffer( void )
	{
		m_bOverflow = false;
		m_pDebugName = NULL;
		m_nDataBits = -1;
		m_nDataBytes = 0;
	}

	FORCEINLINE void SetOverflowFlag( void )
	{
		m_bOverflow = true;
	}

	FORCEINLINE bool IsOverflowed( void ) const
	{
		return m_bOverflow;
	}

	static const std::uint32_t s_nMaskTable[33];							// 0 1 3 7 15 ..

};

class CBitWrite : public CBitBuffer
{
	std::uint32_t m_nOutBufWord;
	int m_nOutBitsAvail;
	std::uint32_t *m_pDataOut;
	std::uint32_t *m_pBufferEnd;
	std::uint32_t *m_pData;
	bool m_bFlushed;

public:
	void StartWriting( void *pData, int nBytes, int iStartBit = 0, int nMaxBits = -1 );


	CBitWrite( void *pData, int nBytes, int nBits = -1 )
	{
		m_bFlushed = false;
		StartWriting( pData, nBytes, 0, nBits );
	}
	
	CBitWrite( const char *pDebugName, void *pData, int nBytes, int nBits = -1 )
	{
		m_bFlushed = false;
		SetDebugName( pDebugName );
		StartWriting( pData, nBytes, 0, nBits );
	}

	CBitWrite( void )
	{
		m_bFlushed = false;
	}

	~CBitWrite( void )
	{
		TempFlush();
	}
	FORCEINLINE int GetNumBitsLeft( void ) const
	{
		return m_nOutBitsAvail + ( 32 * ( m_pBufferEnd - m_pDataOut -1 ) );
	}

	FORCEINLINE void Reset( void )
	{
		m_bOverflow = false;
		m_nOutBitsAvail = 32;
		m_pDataOut = m_pData;
		m_nOutBufWord = 0;
		
	}

	FORCEINLINE void TempFlush( void )
	{
		// someone wants to know how much data we have written, or the pointer to it, so we'd better make
		// sure we write our data
		if ( m_nOutBitsAvail != 32 )
		{
			if ( m_pDataOut == m_pBufferEnd )
			{
				SetOverflowFlag();
			}
			else
			{
				StoreLittleDWord( m_pDataOut, 0, LoadLittleDWord(m_pDataOut,0) & ~s_nMaskTable[ 32 - m_nOutBitsAvail ] | m_nOutBufWord );
			}
		}
		m_bFlushed = true;
	}

	FORCEINLINE unsigned char *GetBasePointer()
	{
		TempFlush();
		return reinterpret_cast< unsigned char *>( m_pData );
	}

	FORCEINLINE unsigned char *GetData()
	{
		return GetBasePointer();
	}

	FORCEINLINE void Finish();
	FORCEINLINE void Flush();
	FORCEINLINE void FlushNoCheck();
	FORCEINLINE void WriteOneBit(int nValue);
	FORCEINLINE void WriteOneBitNoCheck(int nValue);
	FORCEINLINE void WriteUBitLong( unsigned int data, int numbits, bool bCheckRange=true );
	FORCEINLINE void WriteSBitLong( int data, int numbits );
	FORCEINLINE void WriteUBitVar( unsigned int data );
	FORCEINLINE void WriteBitFloat( float flValue );
	FORCEINLINE void WriteFloat( float flValue );
	bool WriteBits(const void *pInData, int nBits);
	void WriteBytes( const void *pBuf, int nBytes );
	void SeekToBit( int nSeekPos );

	FORCEINLINE int GetNumBitsWritten( void ) const
	{
		return ( 32 - m_nOutBitsAvail ) + ( 32 * ( m_pDataOut - m_pData ) );
	}

	FORCEINLINE int GetNumBytesWritten( void ) const
	{
		return ( GetNumBitsWritten() + 7 ) >> 3;
	}


	FORCEINLINE void WriteLong(std::int32_t val)
	{
		WriteSBitLong( val, 32 );
	}



	FORCEINLINE void WriteChar( int val )
	{
		WriteSBitLong(val, sizeof(char) << 3 );
	}

	FORCEINLINE void WriteByte( int val )
	{
		WriteUBitLong(val, sizeof(unsigned char) << 3, false );
	}

	FORCEINLINE void WriteShort(int val)
	{
		WriteSBitLong(val, sizeof(short) << 3);
	}

	FORCEINLINE void WriteWord(int val)
	{
		WriteUBitLong(val, sizeof(unsigned short) << 3);
	}
	
	bool WriteString( const char *pStr );
	bool WriteString( const wchar_t *pStr );

	void WriteLongLong(std::int64_t val );

	void WriteBitAngle( float fAngle, int numbits );
	void WriteBitCoord (const float f);
	void WriteBitCoordMP( const float f, EBitCoordType coordType );
	void WriteBitCellCoord( const float f, int bits, EBitCoordType coordType );
	void WriteBitVec3Coord( const Vector& fa );
	void WriteBitNormal( float f );
	void WriteBitVec3Normal( const Vector& fa );
	void WriteBitAngles( const QAngle& fa );

	// Copy the bits straight out of pIn. This seeks pIn forward by nBits.
	// Returns an error if this buffer or the read buffer overflows.
	bool WriteBitsFromBuffer( class bf_read *pIn, int nBits );
	
};

void CBitWrite::Finish( void )
{
	if ( m_nOutBitsAvail != 32 )
	{
		if ( m_pDataOut == m_pBufferEnd )
		{
			SetOverflowFlag();
		}
		StoreLittleDWord( m_pDataOut, 0, m_nOutBufWord );
	}
}

void CBitWrite::FlushNoCheck( void )
{
	StoreLittleDWord( m_pDataOut++, 0, m_nOutBufWord );
	m_nOutBitsAvail = 32;
	m_nOutBufWord = 0;										// ugh - I need this because of 32 bit writes. a<<=32 is a nop
	
}
void CBitWrite::Flush( void )
{
	if ( m_pDataOut == m_pBufferEnd )
	{
		SetOverflowFlag();
	}
	else
	{
		StoreLittleDWord( m_pDataOut++, 0, m_nOutBufWord );
	}
	m_nOutBufWord = 0;										// ugh - I need this because of 32 bit writes. a<<=32 is a nop
	m_nOutBitsAvail = 32;
	
}
void CBitWrite::WriteOneBitNoCheck( int nValue )
{
	m_nOutBufWord |= ( nValue & 1 ) << ( 32 - m_nOutBitsAvail );
	if ( --m_nOutBitsAvail == 0 )
	{
		FlushNoCheck();
	}
}

void CBitWrite::WriteOneBit( int nValue )
{
	m_nOutBufWord |= ( nValue & 1 ) << ( 32 - m_nOutBitsAvail );
	if ( --m_nOutBitsAvail == 0 )
	{
		Flush();
	}
}

FORCEINLINE void CBitWrite::WriteUBitLong( unsigned int nData, int nNumBits, bool bCheckRange )
{
	if ( nNumBits <= m_nOutBitsAvail )
	{
		if ( bCheckRange )
			m_nOutBufWord |= ( nData ) << ( 32 - m_nOutBitsAvail );
		else
			m_nOutBufWord |= ( nData & s_nMaskTable[ nNumBits] ) << ( 32 - m_nOutBitsAvail );
		m_nOutBitsAvail -= nNumBits;
		if ( m_nOutBitsAvail == 0 )
		{
			Flush();
		}
	}
	else
	{
		// split dwords case
		int nOverflowBits = ( nNumBits - m_nOutBitsAvail );
		m_nOutBufWord |= ( nData & s_nMaskTable[m_nOutBitsAvail] ) << ( 32 - m_nOutBitsAvail );
		Flush();
		m_nOutBufWord = ( nData >> ( nNumBits - nOverflowBits ) );
		m_nOutBitsAvail = 32 - nOverflowBits;
	}
}

FORCEINLINE void CBitWrite::WriteSBitLong( int nData, int nNumBits )
{
	WriteUBitLong( ( std::uint32_t ) nData, nNumBits, false );
}

FORCEINLINE void CBitWrite::WriteUBitVar( unsigned int n )
{
	if ( n < 16 )
		WriteUBitLong( n, 6 );
	else
		if ( n < 256 )
			WriteUBitLong( ( n & 15 ) | 16 | ( ( n & ( 128 | 64 | 32 | 16 ) ) << 2 ), 10 );
		else
			if ( n < 4096 )
				WriteUBitLong( ( n & 15 ) | 32 | ( ( n & ( 2048 | 1024 | 512 | 256 | 128 | 64 | 32 | 16 ) ) << 2 ), 14 );
			else
			{
				WriteUBitLong( ( n & 15 ) | 48, 6 );
				WriteUBitLong( ( n >> 4 ), 32 - 4 );
			}
}

FORCEINLINE void CBitWrite::WriteBitFloat( float flValue )
{
	WriteUBitLong( *((std::uint32_t *) &flValue ), 32 );
}

FORCEINLINE void CBitWrite::WriteFloat( float flValue )
{
	// Pre-swap the float, since WriteBits writes raw data
	LittleFloat( &flValue, &flValue );
	WriteUBitLong( *((std::uint32_t *) &flValue ), 32 );
}

class CBitRead : public CBitBuffer
{
	std::uint32_t m_nInBufWord;
	int m_nBitsAvail;
	std::uint32_t const *m_pDataIn;
	std::uint32_t const *m_pBufferEnd;
	std::uint32_t const *m_pData;

public:
	CBitRead( const void *pData, int nBytes, int nBits = -1 )
	{
		StartReading( pData, nBytes, 0, nBits );
	}
	
	CBitRead( const char *pDebugName, const void *pData, int nBytes, int nBits = -1 )
	{
		SetDebugName( pDebugName );
		StartReading( pData, nBytes, 0, nBits );
	}

	CBitRead( void ) : CBitBuffer()
	{
	}

	FORCEINLINE int Tell( void ) const
	{
		return GetNumBitsRead();
	}
	
	FORCEINLINE size_t TotalBytesAvailable( void ) const
	{
		return m_nDataBytes;
	}

	FORCEINLINE int GetNumBitsLeft() const 
	{
		return m_nDataBits - Tell();
	}

	FORCEINLINE int GetNumBytesLeft() const
	{
		return GetNumBitsLeft() >> 3;
	}

	bool Seek( int nPosition );

	FORCEINLINE bool SeekRelative( int nOffset )
	{
		return Seek( GetNumBitsRead() + nOffset );
	}

	FORCEINLINE unsigned char const * GetBasePointer()
	{
		return reinterpret_cast< unsigned char const *>( m_pData );
	}

	void StartReading( const void *pData, int nBytes, int iStartBit = 0, int nBits = -1 );

	FORCEINLINE int GetNumBitsRead( void ) const;
	FORCEINLINE int GetNumBytesRead( void ) const;

	FORCEINLINE void GrabNextDWord( bool bOverFlowImmediately = false );
	FORCEINLINE void FetchNext( void );
	FORCEINLINE unsigned int ReadUBitLong( int numbits );
	FORCEINLINE int ReadSBitLong( int numbits );
	FORCEINLINE unsigned int ReadUBitVar( void );
	FORCEINLINE unsigned int PeekUBitLong( int numbits );
	FORCEINLINE float ReadBitFloat( void );
	float ReadBitCoord();
	float ReadBitCoordMP( EBitCoordType coordType );
	float ReadBitCellCoord( int bits, EBitCoordType coordType );
	float ReadBitNormal();
	void ReadBitVec3Coord( Vector& fa );
	void ReadBitVec3Normal( Vector& fa );
	void ReadBitAngles( QAngle& fa );
	bool ReadBytes(void *pOut, int nBytes);
	float ReadBitAngle( int numbits );

	// Returns 0 or 1.
	FORCEINLINE int	ReadOneBit( void );
	FORCEINLINE int ReadLong( void );
	FORCEINLINE int ReadChar( void );
	FORCEINLINE int ReadByte( void );
	FORCEINLINE int ReadShort( void );
	FORCEINLINE int ReadWord( void );
	FORCEINLINE float ReadFloat( void );
	void ReadBits(void *pOut, int nBits);

	// Returns false if bufLen isn't large enough to hold the
	// string in the buffer.
	//
	// Always reads to the end of the string (so you can read the
	// next piece of data waiting).
	//
	// If bLine is true, it stops when it reaches a '\n' or a null-terminator.
	//
	// pStr is always null-terminated (unless bufLen is 0).
	//
	// pOutN<umChars is set to the number of characters left in pStr when the routine is 
	// complete (this will never exceed bufLen-1).
	//
	bool ReadString( char *pStr, int bufLen, bool bLine=false, int *pOutNumChars=NULL );
	bool ReadWString( OUT_Z_CAP(maxLenInChars) wchar_t *pStr, int maxLenInChars, bool bLine=false, int *pOutNumChars=NULL );
	char* ReadAndAllocateString( bool *pOverflow = 0 );

	std::int64_t ReadLongLong( void );

	// reads a varint encoded integer
	std::uint32_t			ReadVarInt32();
	std::uint64_t			ReadVarInt64();
	std::int32_t			ReadSignedVarInt32() { return bitbuf::ZigZagDecode32( ReadVarInt32() ); }
	std::int64_t			ReadSignedVarInt64() { return bitbuf::ZigZagDecode64( ReadVarInt64() ); }
};


FORCEINLINE int CBitRead::GetNumBitsRead( void ) const
{
	if ( ! m_pData )									   // pesky null ptr bitbufs. these happen.
		return 0;

	int nCurOfs = int(((intptr_t(m_pDataIn) - intptr_t(m_pData))/4)-1);
	nCurOfs  *= 32;
	nCurOfs += ( 32 - m_nBitsAvail );
	int nAdjust = 8 * ( m_nDataBytes & 3 );
	return MIN( nCurOfs + nAdjust, m_nDataBits );
}

FORCEINLINE int CBitRead::GetNumBytesRead( void ) const
{
	return ( (GetNumBitsRead()+7) >> 3 );
}

FORCEINLINE void CBitRead::GrabNextDWord( bool bOverFlowImmediately )
{
	if ( m_pDataIn == m_pBufferEnd )
	{
		m_nBitsAvail = 1;									// so that next read will run out of words
		m_nInBufWord = 0;
		m_pDataIn++;										// so seek count increments like old
		if ( bOverFlowImmediately )
			SetOverflowFlag();
	}
	else
		if ( m_pDataIn > m_pBufferEnd )
		{
			SetOverflowFlag();
			m_nInBufWord = 0;
		}
		else
		{
			m_nInBufWord = LittleDWord( *( m_pDataIn++ ) );
		}
}

FORCEINLINE void CBitRead::FetchNext( void )
{
	m_nBitsAvail = 32;
	GrabNextDWord( false );
}

int CBitRead::ReadOneBit( void )
{
	int nRet = m_nInBufWord & 1;
	if ( --m_nBitsAvail == 0 )
	{
		FetchNext();
	}
	else
		m_nInBufWord >>= 1;
	return nRet;
}


unsigned int CBitRead::ReadUBitLong( int numbits )
{
	if ( m_nBitsAvail >= numbits )
	{
		unsigned int nRet = m_nInBufWord & s_nMaskTable[ numbits ];
		m_nBitsAvail -= numbits;
		if ( m_nBitsAvail )
		{
			m_nInBufWord >>= numbits;
		}
		else
		{
			FetchNext();
		}
		return nRet;
	}
	else
	{
		// need to merge words
		unsigned int nRet = m_nInBufWord;
		numbits -= m_nBitsAvail;
		GrabNextDWord( true );
		if ( m_bOverflow )
			return 0;
		nRet |= ( ( m_nInBufWord & s_nMaskTable[numbits] ) << m_nBitsAvail );
		m_nBitsAvail = 32 - numbits;
		m_nInBufWord >>= numbits;
		return nRet;
	}
}

FORCEINLINE unsigned int CBitRead::PeekUBitLong( int numbits )
{
	int nSaveBA = m_nBitsAvail;
	int nSaveW = m_nInBufWord;
	std::uint32_t const *pSaveP = m_pDataIn;
	unsigned int nRet = ReadUBitLong( numbits );
	m_nBitsAvail = nSaveBA;
	m_nInBufWord = nSaveW;
	m_pDataIn = pSaveP;
	return nRet;
}

FORCEINLINE int CBitRead::ReadSBitLong( int numbits )
{
	int nRet = ReadUBitLong( numbits );
	// sign extend
	return ( nRet << ( 32 - numbits ) ) >> ( 32 - numbits );
}

FORCEINLINE int CBitRead::ReadLong( void )
{
	return ( int ) ReadUBitLong( sizeof(std::int32_t) << 3 );
}

FORCEINLINE float CBitRead::ReadFloat( void )
{
	std::uint32_t nUval = ReadUBitLong( sizeof(std::int32_t) << 3 );
	return * ( ( float * ) &nUval );
}

FORCEINLINE unsigned int CBitRead::ReadUBitVar( void )
{
	unsigned int ret = ReadUBitLong( 6 );
	switch( ret & ( 16 | 32 ) )
	{
		case 16:
			ret = ( ret & 15 ) | ( ReadUBitLong( 4 ) << 4 );
			break;
				
		case 32:
			ret = ( ret & 15 ) | ( ReadUBitLong( 8 ) << 4 );
			break;
		case 48:
			ret = ( ret & 15 ) | ( ReadUBitLong( 32 - 4 ) << 4 );
			break;
	}
	return ret;
}

FORCEINLINE float CBitRead::ReadBitFloat( void )
{
	std::uint32_t nvalue = ReadUBitLong( 32 );
	return *( ( float * ) &nvalue );
}

int CBitRead::ReadChar( void )
{
	return ReadSBitLong(sizeof(char) << 3);
}

int CBitRead::ReadByte( void )
{
	return ReadUBitLong(sizeof(unsigned char) << 3);
}

int CBitRead::ReadShort( void )
{
	return ReadSBitLong(sizeof(short) << 3);
}

int CBitRead::ReadWord( void )
{
	return ReadUBitLong(sizeof(unsigned short) << 3);
}

#define WRAP_READ( bc ) 																									  \
class bf_read : public bc																									  \
{																															  \
public:																														  \
    FORCEINLINE bf_read( void ) : bc(  )																								  \
	{																														  \
	}																														  \
																															  \
	FORCEINLINE bf_read( const void *pData, int nBytes, int nBits = -1 ) : bc( pData, nBytes, nBits )									  \
	{																														  \
	}																														  \
																															  \
	FORCEINLINE bf_read( const char *pDebugName, const void *pData, int nBytes, int nBits = -1 ) : bc( pDebugName, pData, nBytes, nBits ) \
	{																														  \
	}																														  \
};