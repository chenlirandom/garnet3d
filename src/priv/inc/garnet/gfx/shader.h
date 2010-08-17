#ifndef __GN_GFX_SHADER_H__
#define __GN_GFX_SHADER_H__
// *****************************************************************************
/// \file
/// \brief   graphics shader interface
/// \author  chenli@@REDMOND (2008.12.9)
// *****************************************************************************

namespace GN { namespace gfx
{
    ///
    /// Shading language
    ///
    struct GpuProgramLanguage
    {
        enum Enum
        {
            INVALID       = 0,    ///< Indicate invalid language

            HLSL9         = 1<<0, ///< HLSL for D3D9 and Xenon
            HLSL10        = 1<<1, ///< HLSL for D3D10
            HLSL11        = 1<<2, ///< HLSL for D3D11

            MICROCODE     = 1<<3, ///< Xenon microcode shader

            GLSL          = 1<<4, ///< OpenGL Shading language
            ARB1          = 1<<5, ///< OpenGL ARB1 shading language

            CG            = 1<<6, ///< Nvidia Cg
        };

        /// check for validity
        bool valid() const
        {
            return HLSL9 == *this
                || HLSL10 == *this
                || HLSL11 == *this
                || MICROCODE == *this
                || GLSL == *this
                || ARB1 == *this
                || CG == *this;
        }

        /// convert to string
        const char * toString() const
        {
            switch( *this )
            {
                case HLSL9     : return "HLSL9";
                case HLSL10    : return "HLSL10";
                case HLSL11    : return "HLSL11";
                case MICROCODE : return "MICROCODE";
                case GLSL      : return "GLSL";
                case ARB1      : return "ARB1";
                case CG        : return "CG";
                default        : return "INVALID_GPU_PROGRAM_LANGUAGE";
            };
        }

        /// convert string to from string
        static GpuProgramLanguage sFromString( const char * s )
        {
            if( 0 == stringCompareI( s, "HLSL9" ) )          return HLSL9;
            else if( 0 == stringCompareI( s, "HLSL10" ) )    return HLSL10;
            else if( 0 == stringCompareI( s, "HLSL11" ) )    return HLSL11;
            else if( 0 == stringCompareI( s, "MICROCODE" ) ) return MICROCODE;
            else if( 0 == stringCompareI( s, "GLSL" ) )      return GLSL;
            else if( 0 == stringCompareI( s, "ARB1" ) )      return ARB1;
            else if( 0 == stringCompareI( s, "CG" ) )        return CG;
            else                                      return INVALID;
        }

        GN_DEFINE_ENUM_CLASS_HELPERS( GpuProgramLanguage, Enum )
    };

    ///
    /// shader code
    ///
    struct ShaderCode
    {
        const char * source; ///< NULL terminated shader source. Set to NULL to use fixed functional pipeline.
        const char * entry;  ///< NULL terminated shader entry function (ignored for ASM shader code)

        /// default ctor
        ShaderCode() : source(NULL), entry(NULL) {}
    };

    ///
    /// GPU program description
    ///
    struct GpuProgramDesc
    {
        GpuProgramLanguage lang;  ///< shading language.
        ShaderCode         vs;    ///< vertex shader code
        ShaderCode         gs;    ///< geometry shader code
        ShaderCode         ps;    ///< pixel shader code

        /// compile options
        //@{
        bool optimize; ///< generate optimized shader. Default is on.
        bool debug;    ///< generate debug symbol. Default is on.
        //@}

        ///
        /// default constructor
        ///
        GpuProgramDesc()
            : lang(GpuProgramLanguage::INVALID)
            , optimize(true)
            , debug(true)
        {
        }
    };

    ///
    /// GPU program parameter type
    ///
    struct GpuProgramParameterType
    {
        enum Enum
        {
            UNIFORM, ///< uniform type
            TEXTURE, ///< texture type
            VERTEX,  ///< vertex type
        };

        GN_DEFINE_ENUM_CLASS_HELPERS( GpuProgramParameterType, Enum );
    };

    ///
    /// GPU program uniform description
    ///
    struct GpuProgramUniformParameterDesc
    {
        const char * name; ///< uniform name
        size_t       size; ///< uniform size in bytes
    };

    ///
    /// GPU program texture parameter desc
    struct GpuProgramTextureParameterDesc
    {
        const char * name; ///< texture name
    };

    ///
    /// GPU program attribute (input vertex) description
    ///
    struct GpuProgramAttributeParameterDesc
    {
        const char * name; ///< attribute name.
        //size_t       semanticIndex; ///< attribute semantic index
    };

    enum
    {
        /// indicate a invalid parameter index
        GPU_PROGRAM_PARAMETER_NOT_FOUND = (UInt16)-1
    };

    ///
    /// GPU program parameter accessor template
    ///
    template<class PARAMETER_DESC_CLASS>
    class GpuProgramParameterAccessor
    {
        const UInt8 * & mData;
        const size_t  & mCount;
        const size_t  & mStride;

    public:

        ///
        /// constructor
        ///
        GpuProgramParameterAccessor(
            const PARAMETER_DESC_CLASS * & data,
            const size_t & count,
            const size_t & stride )
            : mData((const UInt8*&)data), mCount(count), mStride(stride)
        {
        }

        ///
        /// return number of parameters
        ///
        size_t count() const { return mCount; }

        ///
        /// bracket operator. index must be valid
        ///
        const PARAMETER_DESC_CLASS & operator[]( size_t index ) const
        {
            // must be a valid index
            GN_ASSERT( index < mCount );

            // Note: stride must be larger than size of parameter class
            GN_ASSERT( mStride >= sizeof(PARAMETER_DESC_CLASS) );

            const PARAMETER_DESC_CLASS * p = (const PARAMETER_DESC_CLASS *)(mData + mStride * index);

            return *p;
        }

        ///
        /// Look up parameter with specific name, return GPU_PROGRAM_PARAMETER_NOT_FOUND for invalid name
        ///
        UInt16 operator[]( const char * name ) const
        {
            // Note: stride must be larger than size of parameter class
            GN_ASSERT( mStride >= sizeof(PARAMETER_DESC_CLASS) );

            const UInt8 * p = mData;
            for( size_t i = 0; i < mCount; ++i, p+=mStride )
            {
                /// Assume that the first member of PARAMETER_DESC_CLASS is always parameter name
                const char * paramName = *(const char * const *)p;

                if( 0 == stringCompare( name, paramName ) )
                {
                    // got you!
                    return (UInt16)i;
                }
            }
            GN_ERROR(getLogger("GN.gfx.GpuProgram.GpuProgramParameterDesc"))(
                "Invalid GPU program parameter name: %s", name?name:"<NULLPTR>" );
            return (UInt16)GPU_PROGRAM_PARAMETER_NOT_FOUND;
        }
    };

    /*
    /// GPU program attribute accessor
    ///
    class GpuProgramAtrributeAccessor
    {
        const UInt8 * & mData;
        const size_t  & mCount;
        const size_t  & mStride;

    public:

        ///
        /// constructor
        ///
        GpuProgramParameterAccessor(
            const GpuProgramAttributeParameterDesc * & data,
            const size_t & count,
            const size_t & stride )
            : mData((const UInt8*&)data), mCount(count), mStride(stride)
        {
        }

        ///
        /// return number of parameters
        ///
        size_t count() const { return mCount; }

        ///
        /// bracket operator. index must be valid
        ///
        const GpuProgramAttributeParameterDesc & operator[]( size_t index ) const
        {
            // must be a valid index
            GN_ASSERT( index < mCount );

            // Note: stride must be larger than size of parameter class
            GN_ASSERT( mStride >= sizeof(GpuProgramAttributeParameterDesc) );

            const GpuProgramAttributeParameterDesc * p = (const GpuProgramAttributeParameterDesc *)(mData + mStride * index);

            return *p;
        }

        ///
        /// Look up attribute with specific semantic name and index, return GPU_PROGRAM_PARAMETER_NOT_FOUND for invalid name
        ///
        size_t find( const char * semanticName, size_t semanticIndex ) const
        {
            // Note: stride must be larger than size of parameter class
            GN_ASSERT( mStride >= sizeof(GpuProgramAttributeParameterDesc) );

            const UInt8 * p = mData;
            for( size_t i = 0; i < mCount; ++i, p+=mStride )
            {
                /// Assume that the first member of PARAMETER_DESC_CLASS is always parameter name
                const char * paramName = *(const char * const *)p;
                size_t     * paramIndex = (size_t*)(paramName+1);

                if( semanticIndex == paramIndex && 0 == stringCompare( semanticName, paramName ) )
                {
                    // got you!
                    return i;
                }
            }
            GN_ERROR(getLogger("GN.gfx.GpuProgram"))(
                "Invalid semantic name or index: name=%s index=%d", semanticName?semanticName:"<NULLPTR>", semanticIndex );
            return (size_t)GPU_PROGRAM_PARAMETER_NOT_FOUND;
        }
    };*/

    ///
    /// GPU program parameter descrption
    ///
    class GpuProgramParameterDesc
    {
    public:

        /// parameter accessors
        ///@{
        GpuProgramParameterAccessor<GpuProgramUniformParameterDesc>   uniforms;
        GpuProgramParameterAccessor<GpuProgramTextureParameterDesc>   textures;
        GpuProgramParameterAccessor<GpuProgramAttributeParameterDesc> attributes;
        ///@}

        ///
        /// constructor
        ///
        GpuProgramParameterDesc()
            : uniforms( mUniformArray, mUniformCount, mUniformArrayStride )
            , textures( mTextureArray, mTextureCount, mTextureArrayStride )
            , attributes( mAttributeArray, mAttributeCount, mAttributeArrayStride )
            , mUniformCount(0)
            , mTextureCount(0)
            , mAttributeCount(0)
        {
        }

        ///
        /// destructor
        ///
        virtual ~GpuProgramParameterDesc() {}

    protected:

        // Note: it is subclass's responsibility to initialize these data members.

        /// uniform parameters
        //@{
        const GpuProgramUniformParameterDesc *   mUniformArray;
        size_t                                   mUniformCount;
        size_t                                   mUniformArrayStride;
        //@}

        /// texture parameters
        //@{
        const GpuProgramTextureParameterDesc   * mTextureArray;
        size_t                                   mTextureCount;
        size_t                                   mTextureArrayStride;
        //@}

        /// attribute parameters
        //@{
        const GpuProgramAttributeParameterDesc * mAttributeArray;
        size_t                                   mAttributeCount;
        size_t                                   mAttributeArrayStride;
        //@}
    };

    ///
    /// Graphics shader interface
    ///
    struct GpuProgram : public RefCounter
    {
        ///
        /// get GPU program parameters
        ///
        virtual const GpuProgramParameterDesc & getParameterDesc() const = 0;

    protected:

        /// protected ctor
        GpuProgram() {}
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_SHADER_H__
