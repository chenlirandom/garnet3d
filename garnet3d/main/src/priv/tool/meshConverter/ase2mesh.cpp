#include "pch.h"
#include "convert.h"

using namespace GN;

// *****************************************************************************
// ASE loader
// *****************************************************************************

struct AseMap
{
    StrA     name;
    StrA     class_;
    int      subno;
    float    amount;
    StrA     bitmap;
    StrA     type;
    Vector2f offset;
    Vector2f tiling;
    float    angle;
    float    blur;
    float    blur_offset;
    float    noise_amt;
    float    noise_size;
    int      noise_level;
    float    noise_phase;
    StrA     filter;
};

struct AseMaterial
{
    StrA          name;
    StrA          class_;
    Vector3f      ambient, diffuse, specular;
    float         shine;
    float         shinestrength;
    float         transparency;
    float         wiresize;
    StrA          shading;
    float         xp_falloff;
    float         selfillum;
    StrA          falloff;
    StrA          xp_type;
    AseMap        mapdiff;
    AseMap        mapbump;
    UInt32        numsub;
    AseMaterial * submaterials;

    AseMaterial() : numsub(0), submaterials(0) {}

    ~AseMaterial() { safeDeleteArray(submaterials); }

    void allocSubMaterials( UInt32 count )
    {
        GN_ASSERT( 0 == numsub && 0 == submaterials );
        if( 0 == count ) return;
        submaterials = new AseMaterial[count];
        numsub = count;
    }
};

struct AseVertex
{
    Vector3f            p;
    DynaArray<Vector3f> t;
    DynaArray<Vector3f> n;

    UInt32 addTexcoord( const Vector3f & v )
    {
        for( UInt32 i = 0; i < t.size(); ++i )
        {
            if( t[i] == v ) return i;
        }
        t.append( v );
        return (UInt32)( t.size() - 1 );
    }

    UInt32 addNormal( const Vector3f & v )
    {
        for( UInt32 i = 0; i < n.size(); ++i )
        {
            if( n[i] == v ) return i;
        }
        n.append( v );
        return (UInt32)( n.size() - 1 );
    }
};

struct AseFace
{
    UInt32   v[3];  ///< vertices (index into AseMesh.vertices)
    UInt32   t[3];  ///< texcoords (index into AseVertex.t)
    UInt32   vn[3]; ///< normal (index into AseVertex.n)
    Vector3f fn;    ///< face normal
    UInt32   s;     ///< smooth group ID
    UInt32   m;     ///< sub material ID
};

struct AseFaceChunk
{
    UInt32            m;
    DynaArray<UInt32> faces; ///< indices into AseMesh.faces
};

struct AseMesh
{
    ///
    /// this group is loaded directly from ASE file.
    //@{
    UInt32                  timevalue;
    DynaArray<AseVertex>    vertices;  ///< vertex array
    DynaArray<AseFace>      faces;     ///< face array
    //@}

    //@{
    DynaArray<AseFaceChunk> chunks; // faces sorted by material
    //@}
};

struct AseNode
{
    StrA parent;
    StrA name;
};

struct AseGeoObject
{
    AseNode node;
    AseMesh mesh;
    UInt32  matid; ///< material ID into global material array
};

struct AseScene
{
    DynaArray<AseMaterial>  materials;
    DynaArray<AseGeoObject> objects;

    const AseMaterial & getChunkMaterial( UInt32 oid, UInt32 cid ) const
    {
        GN_ASSERT( oid < objects.size() );

        const AseGeoObject & o = objects[oid];

        GN_ASSERT( cid < o.mesh.chunks.size() );

        if( "Multi/Sub-Object" == materials[o.matid].class_ )
        {
            const AseFaceChunk & c = o.mesh.chunks[cid];

            return materials[o.matid].submaterials[c.m];
        }
        else
        {
            GN_ASSERT( 0 == cid );
            GN_ASSERT( 1 == o.mesh.chunks.size() );
            return materials[o.matid];
        }
    }
};

struct AseFile
{
    DynaArray<char> buf;
    char *          str;
    int             line;

    bool open( const StrA & filename )
    {
        // read ASE file
        AutoObjPtr<File> fp( core::openFile( filename, "rt" ) );
        if( !fp ) return false;
        buf.resize( fp->size() + 1 );
        size_t readen;
        buf.resize( fp->size() );
        if( !fp->read( buf.cptr(), fp->size(), &readen ) )
        {
            return false;
        }
        GN_ASSERT( readen <= fp->size() );
        buf[readen] = 0;

        str = buf.cptr();
        line = 0;

        // success
        return true;
    }

    void err( const StrA & msg ) { GN_ERROR(sLogger)( "line %d : %s", line, msg.cptr() ); }
    void warn( const StrA & msg ) { GN_WARN(sLogger)( "line %d : %s", line, msg.cptr() ); }
    void info( const StrA & msg ) { GN_INFO(sLogger)( "line %d : %s", line, msg.cptr() ); }

    enum ScanOptionEnum
    {
        IN_CURRENT_BLOCK            = 0,
        IN_CURRENT_AND_CHILD_BLOCKS = 1,
        IN_ALL_BLOCKS               = 2,
        SILENCE                     = 4,
    };

    union ScanOption
    {
        UInt32 u32;

        struct
        {
            unsigned int scope   : 2;
            unsigned int silence : 1;
            unsigned int _       : 29; ///< reserved
        };

        ScanOption( BitFields bits ) : u32(bits) {}
    };

    ///
    /// skip white spaces
    ///
    void skipWhiteSpaces()
    {
        GN_ASSERT( str );
        while( ' ' == *str || '\t' == *str || '\n' == *str )
        {
            if( '\n' == *str ) ++line;
            ++str;
        }
    }

    ///
    /// get next token
    ///
    const char * next( const char * expectedValue = 0, ScanOption option = 0 )
    {
        GN_ASSERT( str );

        if( 0 == str || 0 == *str )
        {
            if( !option.silence ) info( "EOF" );
            return 0;
        }

        skipWhiteSpaces();

        if ( '"' == *str )
        {
            // quoted string

            const char * r = str;

            ++str;
            while( '"' != *str && 0 != *str )
            {
                if( '\n' == *str ) ++line;
                ++str;
            }

            if( 0 != *str )
            {
                GN_ASSERT( '"' == *str );
                ++str;
                if( '\n' == *str ) ++line;
                *str = 0;
                ++str;
            }

            if( expectedValue && 0 != strCmp( expectedValue, r ) )
            {
                if( !option.silence )
                {
                    err( strFormat( "expect '%s', but found '%s'.", expectedValue, r ) );
                }
                return 0;
            }

            // success
            return r;

        }
        else
        {
            const char * r = str;

            while( ' ' != *str && '\t' != *str && '\n' != *str && 0 != *str )
            {
                ++str;
            }

            if( '\n' == *str ) ++line;

            if( 0 != *str ) *str = 0, ++str; // point to start of next token

            if( expectedValue && 0 != strCmp( expectedValue, r ) )
            {
                if( !option.silence )
                {
                    err( strFormat( "expect '%s', but found '%s'.", expectedValue, r ) );
                }
                return 0;
            }

            return r;
        }
    }

    ///
    /// skip until meet specific token
    ///
    bool skipUntil( const char * endtoken, ScanOption option = 0 )
    {
        GN_ASSERT( !strEmpty( endtoken ) );

        const char * token;
        int level = 0;
        for(;;)
        {
            token = next();

            if( 0 == strCmp( "{", token ) ) ++level;
            else if( 0 == strCmp( "}", token ) ) --level;

            if( IN_CURRENT_BLOCK == option.scope && level > 0 ) continue; // skip sub levels

            if( IN_CURRENT_AND_CHILD_BLOCKS == option.scope && level < 0 )
            {
                if( !option.silence ) err( strFormat( "token '%s' not found inside current block!", endtoken ) );
                return false;
            }

            if( 0 == token )
            {
                if( !option.silence ) err( strFormat( "token '%s' not found!", endtoken ) );
                return false;
            }

            if( 0 == strCmp( endtoken, token ) ) return true;
        }
    }

    //
    // True, if next token is a node (begin with '*')
    // -----------------------------------------------------------------------------
    bool nextIsNode()
    {
        skipWhiteSpaces();
        return '*' == *str;
    }

    //
    //
    // -----------------------------------------------------------------------------
    bool skipNode( ScanOption option = 0 )
    {
        do {
            skipWhiteSpaces();
            if( '{' == *str ) return skipUntil( "}", option );
            if( '*' == *str || '}' == *str ) return true; // found next node
        } while( next( 0, option ) );
        return false;
    }

    //
    //
    // -----------------------------------------------------------------------------
    bool readBlockStart( ScanOption option = 0 ) { return 0 != next( "{", option ); }

    //
    //
    // -----------------------------------------------------------------------------
    bool readBlockEnd( ScanOption option = 0 ) { return 0 != next( "}", option ); }

    //
    //
    // -----------------------------------------------------------------------------
    const char * readNode( ScanOption option = 0  )
    {
        const char * token = next( 0, option );
        if( !token ) return false;
        GN_ASSERT( !strEmpty( token ) );

        if( '*' != *token )
        {
            if( !option.silence ) err( "Not a node token" );
            return 0;
        }

        // success
        return token;
    }

    //
    //
    // -----------------------------------------------------------------------------
    const char * readString( ScanOption option = 0  )
    {
        char * token = const_cast<char*>( next( 0, option ) );
        if( !token ) return false;
        GN_ASSERT( !strEmpty( token ) );

        if( '"' != *token )
        {
            if( !option.silence ) err( "Not a string token" );
            return 0;
        }

        // trim quota
        ++token;
        GN_ASSERT( (token+3) <= str );
        GN_ASSERT( '"' == token[str-token-2] );
        token[str-token-2] = 0;
 
        // success
        return token;
    }

    //
    //
    // -----------------------------------------------------------------------------
    bool readString( StrA & result, ScanOption option = 0  )
    {
        const char * s = readString( option );
        if( 0 == s ) return false;
        result = s;
        return true;
    }

    //
    // Note: symbol is a word w/o quotes and spaces.
    // -----------------------------------------------------------------------------
    bool readSymbol( StrA & result, ScanOption option = 0  )
    {
        const char * s = next( 0, option );
        if( 0 == s ) return false;
        if( 'a' <= *s && *s <= 'z' || 'A' <= *s && *s <= 'Z' || '_' == *s )
        {
            result = s;
            return true;
        }
        else
        {
            err( strFormat( "Expect a symbol (start with [_a-zA-Z]), but met: %s", s ) );
            return false;
        }
    }

    //
    //
    // -----------------------------------------------------------------------------
    bool readFloat( float & result, ScanOption option = 0  )
    {
        const char * token = next( 0, option );

        if( str2Float( result, token ) ) return true;
        else if( 0 == strCmp( "1.#QNB", token ) ) { result = 1.0f; return true; }
        else if( 0 == strCmp( "-1.#QNB", token ) ) { result = -1.0f; return true; }
        else
        {
            if( !option.silence ) err( strFormat( "Not valid float : %s", token ) );
            return false;
        }
    }

    //
    //
    // -----------------------------------------------------------------------------
    template<typename INT_TYPE>
    bool readInt( INT_TYPE & result, ScanOption option = 0  )
    {
        const char * token = next( 0, option );
        if( 0 == token ) return false;

        if( !str2Int<INT_TYPE>( result, token ) )
        {
            if( !option.silence ) err( strFormat( "Not valid integer : %s", token ) );
            return false;
        }

        return true;
    }

    //
    //
    // -----------------------------------------------------------------------------
    template<typename INT_TYPE>
    INT_TYPE readOptionalInt( INT_TYPE defval, ScanOption option = 0  )
    {
        INT_TYPE result;
        return readInt( result, option ) ? result : defval;
    }

    //
    //
    // -----------------------------------------------------------------------------
    bool readVector3( Vector3f & result, ScanOption option = 0  )
    {
        return readFloat( result.x, option )
            && readFloat( result.y, option )
            && readFloat( result.z, option );
    }

    //
    //
    // -----------------------------------------------------------------------------
    bool readIndexedVector3Node( const char * nodename, UInt32 index, Vector3f & result, ScanOption option = 0  )
    {
        GN_ASSERT( !strEmpty(nodename) );
        return next( nodename, option )
            && next( strFormat( "%d", index ).cptr(), option )
            && readVector3( result, option );
    }
};

//
//
// -----------------------------------------------------------------------------
static bool sReadMap( AseMap & m, AseFile & ase )
{
    GN_GUARD;

    if( !ase.readBlockStart() ) return false;

    const char * token;

    while( 0 != ( token = ase.next() ) )
    {
        if( 0 == strCmp( token, "*MAP_NAME" ) )
        {
            if( !ase.readString( m.name ) ) return false;
        }
        else if( 0 == strCmp( token, "*MAP_CLASS" ) )
        {
            if( !ase.readString( m.class_ ) ) return false;
        }
        else if( 0 == strCmp( token, "*MAP_SUBNO" ) )
        {
            if( !ase.readInt( m.subno ) ) return false;
        }
        else if( 0 == strCmp( token, "*MAP_AMOUNT" ) )
        {
            if( !ase.readFloat( m.amount ) ) return false;
        }
        else if( 0 == strCmp( token, "*BITMAP" ) )
        {
            if( !ase.readString( m.bitmap ) ) return false;
        }
        else if( 0 == strCmp( token, "*MAP_TYPE" ) )
        {
            if( !ase.readSymbol( m.type ) ) return false;
        }
        else if( 0 == strCmp( token, "*UVW_U_OFFSET" ) )
        {
            if( !ase.readFloat( m.offset.u ) ) return false;
        }
        else if( 0 == strCmp( token, "*UVW_V_OFFSET" ) )
        {
            if( !ase.readFloat( m.offset.v ) ) return false;
        }
        else if( 0 == strCmp( token, "*UVW_U_TILING" ) )
        {
            if( !ase.readFloat( m.tiling.u ) ) return false;
        }
        else if( 0 == strCmp( token, "*UVW_V_TILING" ) )
        {
            if( !ase.readFloat( m.tiling.v ) ) return false;
        }
        else if( 0 == strCmp( token, "*UVW_ANGLE" ) )
        {
            if( !ase.readFloat( m.angle ) ) return false;
        }
        else if( 0 == strCmp( token, "*UVW_BLUR" ) )
        {
            if( !ase.readFloat( m.blur ) ) return false;
        }
        else if( 0 == strCmp( token, "*UVW_BLUR_OFFSET" ) )
        {
            if( !ase.readFloat( m.blur_offset ) ) return false;
        }
        // Note: this is 3dsmax ASE exporter bug. Should be MAP_NOISE_AMT
        else if( 0 == strCmp( token, "*UVW_NOUSE_AMT" ) )
        {
            if( !ase.readFloat( m.noise_amt ) ) return false;
        }
        else if( 0 == strCmp( token, "*UVW_NOISE_SIZE" ) )
        {
            if( !ase.readFloat( m.noise_size ) ) return false;
        }
        else if( 0 == strCmp( token, "*UVW_NOISE_LEVEL" ) )
        {
            if( !ase.readInt( m.noise_level ) ) return false;
        }
        else if( 0 == strCmp( token, "*UVW_NOISE_PHASE" ) )
        {
            if( !ase.readFloat( m.noise_phase ) ) return false;
        }
        else if( 0 == strCmp( token, "*BITMAP_FILTER" ) )
        {
            if( !ase.readSymbol( m.filter ) ) return false;
        }
        else if( '*' == *token )
        {
            ase.info( strFormat( "skip node %s", token ) );
            if( !ase.skipNode() ) return false;
        }
        else if( 0 == strCmp( token, "}" ) )
        {
            // end of the block
            return true;
        }
        else
        {
            ase.err( strFormat( "expecting node or close-brace, but met '%s'!", token ).cptr() );
            return false;
        }
    }

    // something wrong
    ase.err( "fail to get next token!" );
    return false;

    GN_UNGUARD;
}
//
//
// -----------------------------------------------------------------------------
static bool sReadMaterial( AseMaterial & m, AseFile & ase )
{
    GN_GUARD;

    if( !ase.readBlockStart() ) return false;

    const char * token;

    while( 0 != ( token = ase.next() ) )
    {
        GN_ASSERT( !strEmpty( token ) );

        if( 0 == strCmp( token, "*MATERIAL_NAME" ) )
        {
            if( !ase.readString( m.name ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_CLASS" ) )
        {
            if( !ase.readString( m.class_ ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_AMBIENT" ) )
        {
            if( !ase.readVector3( m.ambient ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_DIFFUSE" ) )
        {
            if( !ase.readVector3( m.diffuse ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_SPECULAR" ) )
        {
            if( !ase.readVector3( m.specular ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_SHINE" ) )
        {
            if( !ase.readFloat( m.shine ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_SHINESTRENGTH" ) )
        {
            if( !ase.readFloat( m.shinestrength ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_TRANSPARENCY" ) )
        {
            if( !ase.readFloat( m.transparency ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_WIRESIZE" ) )
        {
            if( !ase.readFloat( m.wiresize ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_SHADING" ) )
        {
            if( !ase.readSymbol( m.shading ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_XP_FALLOFF" ) )
        {
            if( !ase.readFloat( m.xp_falloff ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_SELFILLUM" ) )
        {
            if( !ase.readFloat( m.selfillum ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_FALLOFF" ) )
        {
            if( !ase.readSymbol( m.falloff ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_XP_TYPE" ) )
        {
            if( !ase.readSymbol( m.xp_type ) ) return false;
        }
        else if( 0 == strCmp( token, "*MAP_", 5 ) )
        {
            const char * map = token + 5;
            if( 0 == strCmp( map, "DIFFUSE" ) )
            {
                if( !sReadMap( m.mapdiff, ase ) ) return false;
            }
            else if( 0 == strCmp( map, "BUMP" ) )
            {
                if( !sReadMap( m.mapbump, ase ) ) return false;
            }
            else
            {
                ase.info( strFormat( "skip unsupport map %s", token ) );
                if( !ase.skipNode() ) return false;
            }
        }
        else if( 0 == strCmp( token, "*NUMSUBMTLS" ) )
        {
            UInt32 count;
            if( !ase.readInt( count ) ) return false;
            m.allocSubMaterials( count );

            // read sub-materials one by one
            for( UInt32 i = 0; i < count; ++i )
            {
                if( !ase.next( "*SUBMATERIAL" ) ) return false;
                if( !ase.next( strFormat("%d",i).cptr() ) ) return false;
                if( !sReadMaterial( m.submaterials[i], ase ) ) return false;
            }
        }
        else if( '*' == *token )
        {
            ase.info( strFormat( "skip node %s", token ) );
            if( !ase.skipNode() ) return false;
        }
        else if( 0 == strCmp( token, "}" ) )
        {
            // end of the block
            return true;
        }
        else
        {
            ase.err( strFormat( "expecting node or close-brace, but met '%s'!", token ).cptr() );
            return false;
        }
    }

    return false;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
static bool sReadMaterials( AseScene & scene, AseFile & ase )
{
    GN_GUARD;

    ase.info( "Read materials ..." );

    if( !ase.readBlockStart() ) return false;

    // read material count
    if( !ase.next( "*MATERIAL_COUNT" ) ) return false;
    UInt32 matcount;
    if( !ase.readInt( matcount ) ) return false;

    scene.materials.resize( matcount );

    // read materials one by one
    for( UInt32 i = 0; i < matcount; ++i )
    {
        if( !ase.next( "*MATERIAL" ) ) return false;
        if( !ase.next( strFormat("%d",i).cptr() ) ) return false;
        if( !sReadMaterial( scene.materials[i], ase ) ) return false;
    }

    // done
    return ase.readBlockEnd();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
static bool sReadMesh( AseMesh & m, AseFile & ase )
{
    GN_GUARD;

    if( !ase.readBlockStart() ) return false;

    if( !ase.next( "*TIMEVALUE" ) || !ase.readInt( m.timevalue ) ) return false;

    UInt32 numvert, numface;
    if( !ase.next( "*MESH_NUMVERTEX" ) || !ase.readInt( numvert ) ) return false;
    if( !ase.next( "*MESH_NUMFACES" ) || !ase.readInt( numface ) ) return false;

    m.vertices.resize( numvert );
    m.faces.resize( numface );

    // read vertices
    if( !ase.next( "*MESH_VERTEX_LIST" ) || !ase.readBlockStart() ) return false;
    for( UInt32 i = 0; i < numvert; ++i )
    {
        if( !ase.readIndexedVector3Node( "*MESH_VERTEX", i, m.vertices[i].p ) ) return false;
    }
    if( !ase.readBlockEnd() ) return false;

    // read faces
    if( !ase.next( "*MESH_FACE_LIST" ) || !ase.readBlockStart() ) return false;
    for( UInt32 i = 0; i < numface; ++i )
    {
        AseFace & f = m.faces[i];
        int dummy;
        if( !ase.next( "*MESH_FACE" ) ) return false;
        if( !ase.next( strFormat( "%d:", i ).cptr() ) ) return false;
        if( !ase.next( "A:" ) || !ase.readInt( f.v[0] ) ) return false;
        if( !ase.next( "B:" ) || !ase.readInt( f.v[1] ) ) return false;
        if( !ase.next( "C:" ) || !ase.readInt( f.v[2] ) ) return false;
        if( !ase.next( "AB:" ) || !ase.readInt( dummy ) ) return false;
        if( !ase.next( "BC:" ) || !ase.readInt( dummy ) ) return false;
        if( !ase.next( "CA:" ) || !ase.readInt( dummy ) ) return false;
        if( !ase.next( "*MESH_SMOOTHING" ) ) return false;
        if( !ase.nextIsNode() )
        {
            // then next must be smooth group ID
            if( !ase.readInt( f.s ) ) return false;
        }
        else
        {
            f.s = 0;
        }
        if( !ase.next( "*MESH_MTLID" ) || !ase.readInt( f.m ) ) return false;
    }
    if( !ase.readBlockEnd() ) return false;

    // read texcoords
    UInt numtexcoord;
    if( !ase.next( "*MESH_NUMTVERTEX" ) || !ase.readInt( numtexcoord ) ) return false;
    if( numtexcoord > 0 )
    {
        DynaArray<Vector3f> texcoords( numtexcoord );

        if( !ase.next( "*MESH_TVERTLIST" ) || !ase.readBlockStart() ) return false;
        for( UInt32 i = 0; i < numtexcoord; ++i )
        {
            if( !ase.readIndexedVector3Node( "*MESH_TVERT", i, texcoords[i] ) ) return false;

            // NOTE: currently, we only supports 2D texcoord
            texcoords[i].z = .0f;
        }
        if( !ase.readBlockEnd() ) return false;

        // read tface list
        if( !ase.next( "*MESH_NUMTVFACES" ) || !ase.next( strFormat( "%d", numface ).cptr() ) ) return false;
        if( !ase.next( "*MESH_TFACELIST" ) || !ase.readBlockStart() ) return false;
        for( UInt32 i = 0; i < numface; ++i )
        {
            AseFace & f = m.faces[i];

            if( !ase.next( "*MESH_TFACE" ) ) return false;
            if( !ase.next( strFormat( "%d", i ).cptr() ) ) return false;

            // for each vertex in the face
            for( UInt32 i = 0; i < 3; ++i )
            {
                AseVertex & v = m.vertices[ f.v[i] ];

                // get the index into texcoord array
                UInt32 t;
                if( !ase.readInt( t ) ) return false;

                // add to vertex's texcoord array, store index in the face.
                f.t[i] = v.addTexcoord( texcoords[t] );
            }
        }
        if( !ase.readBlockEnd() ) return false;
    }

    // skip vertex colors
    UInt numcolor;
    if( !ase.next( "*MESH_NUMCVERTEX" ) || !ase.readInt( numcolor ) ) return false;
    if( numcolor > 0 )
    {
        if( !ase.next( "*MESH_CVERTLIST" ) ) return false;
        if( !ase.skipNode() ) return false;
    }

    // read normals
    if( !ase.next( "*MESH_NORMALS" ) || !ase.readBlockStart() ) return false;
    for( UInt32 i = 0; i < numface; ++i )
    {
        AseFace & f = m.faces[i];
        if( !ase.readIndexedVector3Node( "*MESH_FACENORMAL", i, f.fn ) ) return false;

        for( UInt32 i = 0; i < 3; ++i )
        {
            if( !ase.next( "*MESH_VERTEXNORMAL" ) ) return false;

            UInt32 vi;
            if( !ase.readInt( vi ) ) return false;

            AseVertex & v = m.vertices[vi];

            Vector3f n;
            if( !ase.readVector3( n ) ) return false;

            f.vn[i] = v.addNormal( n );
        }
    }
    if( !ase.readBlockEnd() ) return false;

    // end of mesh block
    return ase.readBlockEnd();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
static bool sReadGeomObject( AseScene & scene, AseFile & ase )
{
    GN_GUARD;

    scene.objects.resize( scene.objects.size() + 1 );
    AseGeoObject & o = scene.objects.back();

    if( !ase.readBlockStart() ) return false;

    const char * token;
    while( 0 != ( token = ase.next() ) )
    {
        if( 0 == strCmp( token, "*NODE_NAME" ) )
        {
            o.node.name = ase.readString();
            if( o.node.name.empty() )
            {
                ase.err( "Node name can't be empty!" );
                return false;
            }
            ase.info( strFormat( "read geometry object '%s' ...", o.node.name ) );
        }
        else if( 0 == strCmp( token, "*NODE_PARENT" ) )
        {
            o.node.parent = ase.readString();
        }
        else if( 0 == strCmp( token, "*NODE_TM" ) )
        {
            // skip *NODE_TM by now
            if( !ase.skipNode() ) return false;
        }
        else if( 0 == strCmp( token, "*MESH" ) )
        {
            if( !sReadMesh( o.mesh, ase ) ) return false;
        }
        else if( 0 == strCmp( token, "*MATERIAL_REF" ) )
        {
            if( !ase.readInt( o.matid ) ) return false;
            if( o.matid >= scene.materials.size() )
            {
                ase.err( "material ID is out of range!" );
                return false;
            }
        }
        else if( '*' == *token )
        {
            ase.info( strFormat( "skip node %s", token ) );
            if( !ase.skipNode() ) return false;
        }
        else if( 0 == strCmp( token, "}" ) )
        {
            // end of the block. do some post processing.

            AseMesh & m = o.mesh;

            AseMaterial & mtl = scene.materials[o.matid];

            // build face chunk array
            if( "Multi/Sub-Object" == mtl.class_ )
            {
                m.chunks.reserve( mtl.numsub );

                for( UInt32 i = 0; i < m.faces.size(); ++i )
                {
                    const AseFace & f = m.faces[i];

                    UInt32 cid;
                    for( cid = 0; cid < m.chunks.size(); ++cid )
                    {
                        AseFaceChunk & c = m.chunks[cid];

                        if( c.m == f.m ) break;
                    }
                    GN_ASSERT( cid <= m.chunks.size() );

                    if( cid == m.chunks.size() )
                    {
                        m.chunks.resize( m.chunks.size() + 1 );
                        m.chunks.back().m = f.m;
                    }

                    m.chunks[cid].faces.append( i );

                    
                }
            }
            else
            {
                m.chunks.resize( 1 );
                m.chunks[0].m = 0;
                m.chunks[0].faces.resize( m.faces.size() );
                for( UInt32 i = 0; i < m.faces.size(); ++i )
                {
                    m.chunks[0].faces[i] = i;
                }
            }

            // success 
            return true;
        }
        else
        {
            ase.err( strFormat( "expecting node or close-brace, but met '%s'!", token ).cptr() );
            return false;
        }
    }

    // something wrong!
    ase.err( "Fail to get next node!" );
    return false;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
static bool sReadAse( AseScene & scene, const StrA & filename )
{
    GN_GUARD;

    // open ase file
    AseFile ase;
    if( !ase.open( filename ) ) return false;

    // read ASE version
    const char * token = ase.next( "*3DSMAX_ASCIIEXPORT" );
    if( !token ) return false;
    if( 200 != ase.readOptionalInt( 0 ) )
    {
        GN_ERROR(sLogger)( "Not support ASE version other than 2.00!" );
        return false;
    }

    while( 0 != ( token = ase.next() ) )
    {
        if( 0 ) {}
        //else if( 0 == strCmp( token, "*SCENE" ) )
        //{
        //    ...
        //}
        else if( 0 == strCmp( token, "*MATERIAL_LIST" ) )
        {
            if( !sReadMaterials( scene, ase ) ) return false;
        }
        else if( 0 == strCmp( token, "*GEOMOBJECT" ) )
        {
            if( !sReadGeomObject( scene, ase ) ) return false;
        }
        else if( '*' == *token )
        {
            ase.info( strFormat( "skip node %s", token ) );
            if( !ase.skipNode() ) return false;
        }
        else
        {
            ase.err( strFormat( "expecting node token, but met '%s'.", token ) );
            return false;
        }
    }

    // success
    return true;

    GN_UNGUARD;
}

// *****************************************************************************
// MESH generator
// *****************************************************************************

struct VertexSelector
{
    UInt32 p;
    UInt32 t;
    UInt32 n;

    bool operator < ( const VertexSelector & rhs ) const
    {
        if( p != rhs.p ) return p < rhs.p;
        if( t != rhs.t ) return t < rhs.t;
        return n < rhs.n;
    }
};

class VertexBuffer
{
    typedef std::map<VertexSelector,UInt32> VtxMap;

    VtxMap                    mMap;
    DynaArray<VertexSelector> mBuffer;

public:

    ///
    /// add vertex into buffer, ignore redundant vertex.
    ///
    UInt32 add( const VertexSelector & vtx )
    {
        std::pair<VtxMap::iterator,bool> i = mMap.insert( VtxMap::value_type(vtx,0xbad) );

        if( i.second )
        {
            // this is a new vertex
            GN_ASSERT( 0xbad == i.first->second );
            GN_ASSERT( mBuffer.size() + 1 == mMap.size() );

            i.first->second = (UInt32)( mBuffer.size() );

            mBuffer.append( vtx );
        }

        return i.first->second;
    }

    ///
    /// get number of vertices in buffer
    ///
    size_t size() const { GN_ASSERT( mMap.size() == mBuffer.size() ); return mBuffer.size(); }

    ///
    /// return specific vertex
    ///
    const VertexSelector & operator[]( size_t idx ) const
    {
        GN_ASSERT( idx < mBuffer.size() );
        return mBuffer[idx];
    }

    ///
    /// return specific vertex
    ///
    VertexSelector & operator[]( size_t idx )
    {
        GN_ASSERT( idx < mBuffer.size() );
        return mBuffer[idx];
    }
};

struct FaceRange
{
    UInt32 vboffset;
    UInt32 vbcount;
    UInt32 iboffset;
    UInt32 ibcount;
};

//
//
// -----------------------------------------------------------------------------
static void reorgMeshFor16bitsIndex(
    DynaArray<FaceRange> & ranges,
    VertexBuffer         & vb,
    DynaArray<UInt32>    & ib )
{
    GN_GUARD;

    if( vb.size() < 0x10000 )
    {
        ranges.resize( 1 );
        ranges[0].vboffset = 0;
        ranges[0].vbcount  = vb.size();
        ranges[0].iboffset = 0;
        ranges[0].ibcount  = ib.size();
    }
    else
    {
        GN_WARN(sLogger)( "This chunk has too much vertices (>0x10000) for 16bit index buffer. Ignored." );
        ranges.clear();
    }

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
static bool sWriteScene( const AseScene & scene, const StrA & name )
{
    GN_GUARD;

    for( size_t oi = 0; oi < scene.objects.size(); ++oi )
    {
        const AseGeoObject & o = scene.objects[oi];

        for( size_t ci = 0; ci < o.mesh.chunks.size(); ++ci )
        {
            GN_INFO(sLogger)( "Write object '%s', chunk %d...", o.node.name.cptr(), ci );

            const AseFaceChunk & c = o.mesh.chunks[ci];

            const AseMaterial & mtl = scene.getChunkMaterial( oi, ci );

            // build vertex and index array
            VertexBuffer      vb;
            DynaArray<UInt32> ib; // indices into vb
            VertexSelector    vs;
            for( size_t fi = 0; fi < c.faces.size(); ++fi )
            {
                const AseFace & f = o.mesh.faces[c.faces[fi]];

                for( size_t fi = 0; fi < 3; ++fi )
                {
                    vs.p = f.v[fi];
                    vs.t = f.t[fi];
                    vs.n = f.vn[fi];

                    UInt32 idx = vb.add( vs );

                    ib.append( idx );
                }
            }

            // split vertex and index array, if they are too large
            DynaArray<FaceRange> ranges;
            reorgMeshFor16bitsIndex( ranges, vb, ib );

            for( size_t ri = 0; ri < ranges.size(); ++ri )
            {
                const FaceRange & r = ranges[ri];
                
                StrA basename = strFormat( "%s_%s_c%02d_r%02d",
                                            name.cptr(),
                                            o.node.name.cptr(),
                                            ci,
                                            ri );
                StrA drawname = basename + ".drawable.xml";
                StrA meshname = basename + ".mesh.xml";
                StrA vbname   = basename + ".vb";
                StrA ibname   = basename + ".ib";

                struct MeshVertex { Vector3f p; Vector3f n; Vector2f t; };

                // write VB
                DynaArray<MeshVertex> memvb( r.vbcount );
                for( size_t vi = 0; vi < r.vbcount; ++vi )
                {
                    const VertexSelector & vs = vb[r.vboffset+vi];
                    const AseVertex & v = o.mesh.vertices[vs.p];
                    memvb[i].p = v.p;
                    memvb[i].n = v.n[vs.n];
                    memvb[i].t = v.t[vs.t];
                }
                AutoObjPtr<File> fvb( core::openFile( vbname, "wb" ) );
                if( !fvb || !fvb->write( memvb.cptr(), sizeof(MeshVertex)*memvb.size(), 0 ) )
                {
                    GN_ERROR(sLogger)( "fail to write vertex %d", vi );
                    return false;
                }
                fvb.clear();

                // calculcate bounding bbox
                Spheref bs;
                calcBoundingBox( bbox, memvb.cptr(), r.vbcount, sizeof(MeshVertex) );

                // write IB
                AutoObjPtr<File> fib( core::openFile( ibname, "wb" ) );
                if( !fib ) return false;
                for( size_t ii = 0; ii < r.ibcount; ++ii )
                {
                    UInt32 idx32 = ib[r.iboffset + ii];

                    GN_ASSERT( r.vboffset <= idx32 && idx32 < (r.vboffset+r.vbcount) );
                    GN_ASSERT( r.vbcount < 0x10000 );

                    UInt16 idx16 = (UInt16)( idx32 - r.vboffset );

                    if( !fib->write( &idx16, sizeof(idx16), 0 ) )
                    {
                        GN_ERROR(sLogger)( "fail to write index %d", ii );
                        return false;
                    }
                }
                fib.clear();

                // write mesh xml
                StrA outdir = dirName( meshname );
                StrA meshxml;
                meshxml.format(
                    "<?xml version=\"1.0\" standalone=\"yes\"?>\n"
                    "<mesh\n"
                    "	primtype  = \"TRIANGLE_LIST\"\n"
                    "	numprim   = \"%d\"\n"
                    "	startvtx  = \"0\"\n"
                    "	minvtxidx = \"0\"\n"
                    "	numvtx    = \"%d\"\n"
                    "	startidx  = \"0\"\n"
                    "	>\n"
                    "\n"
                    "   <bbox x=\"%f\" y=\"%f\" z=\"%f\" w=\"%f\" h=\"%f\" d=\"%f\"/>\n"
                    "\n"
                    "	<vtxfmt>\n"
                    "		<attrib stream = \"0\" offset =  \"0\" semantic = \"POS0\" format = \"FMT_FLOAT3\"/>\n"
                    "		<attrib stream = \"0\" offset = \"12\" semantic = \"NML0\" format = \"FMT_FLOAT3\"/>\n"
                    "		<attrib stream = \"0\" offset = \"24\" semantic = \"TEX0\" format = \"FMT_FLOAT2\"/>\n"
                    "	</vtxfmt>\n"
                    "	\n"
                    "	<vtxbuf stream = \"0\" offset = \"0\" stride = \"32\" ref=\"%s\"/>\n"
                    "\n"
                    "	<idxbuf ref=\"%s\"/>\n"
                    "</mesh>\n",
                    r.ibcount / 3,
                    r.vbcount,
                    bbox.x, bbox.y, bbox.z, bbox.w, bbox.h, bbox.d,
                    relPath( vbname, outdir ).cptr(),
                    relPath( ibname, outdir ).cptr() );
                AutoObjPtr<File> mesh( core::openFile( meshname, "wt" ) );
                if( !mesh || !mesh->write( meshxml.cptr(), meshxml.size(), 0 ) ) return false;

                // write drawable xml
                StrA drawxml;
                drawxml.format(
                    "<?xml version=\"1.0\" standalone=\"yes\"?>\n"
                    "<drawable>\n"
                    "	<effect ref=\"media::/effect/%s.xml\"/>\n"
                    "	<mesh ref=\"%s\"/>\n"
                    "	<texture binding=\"texdiff\" ref=\"%s\"/>\n"
                    "	<texture binding=\"texbump\" ref=\"%s\"/>\n"
                    "	<uniform binding=\"diffuse\" type=\"VECTOR4\" count=\"1\">%f, %f, %f, 1.0f</uniform>\n"
                    "</drawable>\n",
                    mtl.mapdiff.bitmap.empty() ? "diffuse_textured" : "bump_textured",
                    relPath( meshname, outdir ).cptr(),
                    mtl.mapdiff.bitmap.empty() ? "texture://purewhite.bmp" : relPath( mtl.mapdiff.bitmap, outdir ).cptr(),
                    relPath( mtl.mapbump.bitmap, outdir ).cptr(),
                    mtl.diffuse.x, mtl.diffuse.y, mtl.diffuse.z );
                AutoObjPtr<File> draw( core::openFile( drawname, "wt" ) );
                if( !draw || !draw->write( drawxml.cptr(), drawxml.size(), 0 ) ) return false;
            }
        }
    }

    // success
    return true;

    GN_UNGUARD;
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::ase2mesh( const ConvertOptions & co )
{
    GN_GUARD;

    GN_ASSERT( !co.inputFileName.empty() );
    GN_ASSERT( !co.outputFileName.empty() );

    AseScene s;

    if( !sReadAse( s, co.inputFileName ) ) return false;

    if( !sWriteScene( s, co.outputFileName ) ) return false;

    // success
    return true;

    GN_UNGUARD;
}
