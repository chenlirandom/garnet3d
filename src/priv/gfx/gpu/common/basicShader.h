#ifndef __GN_GFX_BASICSHADER_H__
#define __GN_GFX_BASICSHADER_H__
// *****************************************************************************
/// \file
/// \brief   Basic shader classes and utilities
/// \author  chenli@@REDMOND (2009.4.2)
// *****************************************************************************

namespace GN { namespace gfx
{
    ///
    /// Self-contained GPU program description: all string pointers point to internal data buffer
    ///
    class SelfContainedGpuProgramDesc : public Blob
    {
        DynaArray<UInt8> mBuffer;

        static inline bool
        sCheckShaderCode(
            const char *       type, ///< shader type
            const ShaderCode & sc,
            const char *       begin,
            const char *       end )
        {
            if( 0 != sc.source && ( sc.source < begin || sc.source >= end ) )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common");
                GN_ERROR(sLogger)( "invalid %s shader source pointer.", type );
                return false;
            }

            if( 0 != sc.entry && ( sc.entry < begin || sc.entry >= end ) )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common");
                GN_ERROR(sLogger)( "invalid %s shader entry pointer.", type );
                return false;
            }

            return true;
        }

    public:

        /// initialize from shader description
        bool Init( const GpuProgramDesc & desc )
        {
            // calculate buffer size
            size_t headerLen  = sizeof(desc);
            size_t vsCodeLen  = desc.vs.source ? ( strlen(desc.vs.source) + 1 ) : 0;
            size_t vsEntryLen = desc.vs.entry ? ( strlen(desc.vs.entry) + 1 ) : 0;
            size_t gsCodeLen  = desc.gs.source ? ( strlen(desc.gs.source) + 1 ) : 0;
            size_t gsEntryLen = desc.gs.entry ? ( strlen(desc.gs.entry) + 1 ) : 0;
            size_t psCodeLen  = desc.ps.source ? ( strlen(desc.ps.source) + 1 ) : 0;
            size_t psEntryLen = desc.ps.entry ? ( strlen(desc.ps.entry) + 1 ) : 0;
            size_t length     = headerLen +
                                vsCodeLen + vsEntryLen +
                                gsCodeLen + gsEntryLen +
                                psCodeLen + psEntryLen;

            // allocate buffer
            mBuffer.Resize( length );
            GpuProgramDesc & copy = *(GpuProgramDesc*)mBuffer.ToRawPtr();
            UInt8 * start = mBuffer.ToRawPtr();
            UInt8 * ptr = start;

            // copy header
            memcpy( ptr, &desc, sizeof(desc) );
            ptr += sizeof(desc);

#define COPY_SOURCE( X ) \
        memcpy( ptr, desc.X.source, X##CodeLen ); \
        copy.X.source = ( X##CodeLen > 0 ) ? (const char*)( ptr - start ) : 0; \
        ptr += X##CodeLen;

#define COPY_ENTRY( X ) \
        memcpy( ptr, desc.X.entry, X##EntryLen ); \
        copy.X.entry = ( X##EntryLen > 0 ) ? (const char*)( ptr - start ) : 0; \
        ptr += X##EntryLen;

            // copy codes and entries
            COPY_SOURCE( vs ); COPY_ENTRY( vs );
            COPY_SOURCE( gs ); COPY_ENTRY( gs );
            COPY_SOURCE( ps ); COPY_ENTRY( ps );

            // done
            GN_ASSERT( ((size_t)( ptr - start )) == length );
            return true;
        }

        /// initialize from raw Data buffer
        bool Init( const void * data, size_t length )
        {
            // copy input buffer
            mBuffer.Resize( length );
            memcpy( mBuffer.ToRawPtr(), data, length );

            const char     * start = (const char *)mBuffer.ToRawPtr();
            const char     * end   = start + length;
            GpuProgramDesc & desc  = *(GpuProgramDesc*)start;

            // patch all offsets to pointers
            if( 0 != desc.vs.source ) desc.vs.source = start + (size_t)desc.vs.source;
            if( 0 != desc.vs.entry ) desc.vs.entry = start + (size_t)desc.vs.entry;

            if( 0 != desc.gs.source ) desc.gs.source = start + (size_t)desc.gs.source;
            if( 0 != desc.gs.entry ) desc.gs.entry = start + (size_t)desc.gs.entry;

            if( 0 != desc.ps.source ) desc.ps.source = start + (size_t)desc.ps.source;
            if( 0 != desc.ps.entry ) desc.ps.entry = start + (size_t)desc.ps.entry;

            // check GPU program language
            if( !desc.lang.valid() )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common");
                GN_ERROR(sLogger)( "invalid GPU program language: %d", desc.lang.ToRawEnum() );
                return false;
            }

            // check data integrity
            if( !sCheckShaderCode( "vertex", desc.vs, start, end ) ||
                !sCheckShaderCode( "geometry", desc.gs, start, end ) ||
                !sCheckShaderCode( "pixel", desc.ps, start, end ) )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common");
                GN_ERROR(sLogger)( "Invalid shader binary." );
                return false;
            }

            return true;
        }

        const GpuProgramDesc & desc() const { return *(const GpuProgramDesc*)mBuffer.ToRawPtr(); }

        virtual void         * Data() const { return (void*)mBuffer.ToRawPtr(); }
        virtual size_t         Size() const { return mBuffer.Size(); }
    };

    ///
    /// API agonize uniform (just a memory buffer with time stamp)
    ///
    class SysMemUniform : public Uniform
    {
        const size_t mSize;
        void       * mData;
        SInt32       mTimeStamp;

    public:

        /// ctor
        SysMemUniform( size_t sz )
            : mSize(0==sz?1:sz)
            , mData( HeapMemory::Alloc(mSize) )
            , mTimeStamp(0)
        {
        }

        /// dtor
        ~SysMemUniform() { HeapMemory::Free(mData); }

        /// get parameter size
        virtual size_t Size() const { return mSize; }

        /// get current parameter value
        virtual const void * getval() const { return mData; }

        /// update parameter value
        virtual void update( size_t offset, size_t length, const void * data )
        {
            if( offset >= mSize || (offset+length) > mSize )
            {
                GN_ERROR(GetLogger("GN.gfx.Uniform"))( "Out of range!" );
                return;
            }
            if( NULL == data )
            {
                GN_ERROR(GetLogger("GN.gfx.Uniform"))( "Null pointer!" );
                return;
            }
            memcpy( (UInt8*)mData + offset, data, length );
            ++mTimeStamp;
        }

        /// update parameter value
        template<typename T>
        void update( const T & t ) { set( 0, sizeof(t), &t ); }

        /// get current update time stamp
        SInt32 getTimeStamp() const { return mTimeStamp; }
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_BASICSHADER_H__
