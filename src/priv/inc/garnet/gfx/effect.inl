// inline methods of Effect class

//
//
// -----------------------------------------------------------------------------
GN_INLINE size_t GN::gfx::effect::Effect::drawBegin() const
{
    GN_GUARD_SLOW;

    // make sure effect is initialized.
    GN_ASSERT( ok() );

    // prepare technique
    GN_ASSERT( mTechniques.items.validHandle(mActiveTechnique) );
    TechniqueData & t = mTechniques.items[mActiveTechnique];
    if( !t.ready && !initTechnique(mActiveTechnique) ) return 0;
    GN_ASSERT( t.ready );

    // success
    return t.passes.size();

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
GN_INLINE void GN::gfx::effect::Effect::passBegin( size_t mActivePass ) const
{
    GN_GUARD_SLOW;

    GN_ASSERT( !mPassBegun );
    mPassBegun = true;
    mActivePass = mActivePass;

    GN_ASSERT( mTechniques.items.validHandle(mActiveTechnique) );
    TechniqueData & t = mTechniques.items[mActiveTechnique];

    const PassData & p = t.passes[mActivePass];

    // apply render states
    GN_ASSERT( p.rsb );
    mRenderer->bindRenderStateBlock( p.rsb );

    // bind shaders
    Shader * shaders[NUM_SHADER_TYPES];
    for( size_t iShader = 0; iShader < NUM_SHADER_TYPES; ++iShader )
    {
        GN_ASSERT( mShaders.items.validHandle(p.shaders[iShader]) );
        const ShaderData & sd = mShaders.items[p.shaders[iShader]];
        shaders[iShader] = sd.value.get();
    }
    mRenderer->bindShaders( shaders );

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
GN_INLINE void GN::gfx::effect::Effect::commitChanges() const
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegun && mPassBegun );

    GN_ASSERT( mTechniques.items.validHandle(mActiveTechnique) );
    TechniqueData & t = mTechniques.items[mActiveTechnique];

    const PassData & p = t.passes[mActivePass];

    // apply uniforms and textures
    Shader * shaders[NUM_SHADER_TYPES];
    for( size_t iShader = 0; iShader < NUM_SHADER_TYPES; ++iShader )
    {
        GN_ASSERT( mShaders.items.validHandle(p.shaders[iShader]) );
        ShaderData & sd = mShaders.items[p.shaders[iShader]];

        shaders[iShader] = sd.value.get();

        // apply dirty uniforms
        for( size_t iUniform = 0; iUniform < sd.dirtyUniforms.size(); ++iUniform )
        {
            const UniformRefData & ur = sd.uniforms[iUniform];
            const UniformData & ud = mUniforms.items[ur.handle];
            if( ur.ffp )
            {
                sSetFfpParameter( *mRenderer, ur.ffpParameterType, ud );
            }
            else
            {
                GN_ASSERT( !ud.isTextureStates && ur.shaderUniformHandle );
                shaders[iShader]->setUniform( ur.shaderUniformHandle, ud.value );
            }
        }
        sd.dirtyUniforms.clear();

        // apply textures
        for( size_t iTexture = 0; iTexture < sd.textures.size(); ++iTexture )
        {
            const TextureRefData & tr = sd.textures[iTexture];
            GN_ASSERT( mTextures.items.validHandle(tr.handle) );
            const TextureData & td = mTextures.items[tr.handle];
            mRenderer->bindTexture( tr.stage, gTexDict.getResource(td.value) );
        }
    }

    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// uniform management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN_INLINE uint32_t GN::gfx::effect::Effect::getTechniqueHandle( const StrA & name ) const
{
    GN_GUARD_SLOW;
    uint32_t handle = mTechniques.find( name );
    if( 0 == handle ) GN_ERROR( "invalid technique name: %s", name.cstr() );
    return handle;
    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
GN_INLINE void GN::gfx::effect::Effect::setActiveTechnique( uint32_t handle ) const
{
    GN_GUARD_SLOW;

    if( !ok() )
    {
        GN_ERROR( "Uninitialized effect class." );
        return;
    }

    if( 0 == handle )
    {
        // use the first technique.
        mActiveTechnique = mTechniques.items.first();
    }
    else if( !mTechniques.items.validHandle( handle ) )
    {
        GN_ERROR( "Invalid technique handle: %d.", handle );
    }
    else
    {
        mActiveTechnique = handle;
    }
    GN_ASSERT( mTechniques.items.validHandle( mActiveTechnique ) );

    GN_UNGUARD_SLOW;
}


//
//
// -----------------------------------------------------------------------------
GN_INLINE void GN::gfx::effect::Effect::setActiveTechniqueByName( const StrA & name ) const
{
    GN_GUARD_SLOW;
    uint32_t handle = getTechniqueHandle( name );
    if( 0 == handle ) return;
    setActiveTechnique( handle );
    GN_UNGUARD_SLOW;
}


// *****************************************************************************
// uniform management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN_INLINE uint32_t GN::gfx::effect::Effect::getUniformHandle( const StrA & name ) const
{
    GN_GUARD_SLOW;
    uint32_t handle = mUniforms.find( name );
    if( 0 == handle ) GN_ERROR( "invalid uniform name: %s", name.cstr() );
    return handle;
    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
GN_INLINE void GN::gfx::effect::Effect::setUniform( uint32_t handle, const UniformValue & value ) const
{
    GN_GUARD_SLOW;

    if( !mUniforms.items.validHandle(handle) )
    {
        GN_ERROR( "invalid uniform handle: %d", handle );
        return;
    }

    UniformData & u = mUniforms.items[handle];
    u.value = value;

    for( size_t i = 0; i < u.shaders.size(); ++i )
    {
        ShaderRefData & sr = u.shaders[i];
        ShaderData & sd = mShaders.items[sr.shader];
        sd.dirtyUniforms.push_back( sr.index );
    }

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
GN_INLINE void GN::gfx::effect::Effect::setUniformByName( const StrA & name, const UniformValue & value ) const
{
    GN_GUARD_SLOW;
    uint32_t handle = getUniformHandle( name );
    if( 0 == handle ) return;
    setUniform( handle, value );
    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// texture management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN_INLINE uint32_t GN::gfx::effect::Effect::getTextureHandle( const StrA & name ) const
{
    GN_GUARD_SLOW;
    uint32_t handle = mTextures.find( name );
    if( 0 == handle ) GN_ERROR( "invalid texture name: %s", name.cstr() );
    return handle;
    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
GN_INLINE void GN::gfx::effect::Effect::setTexture( uint32_t handle, uint32_t id ) const
{
    GN_GUARD_SLOW;

    if( !mTextures.items.validHandle(handle) )
    {
        GN_ERROR( "invalid texture handle: %d", handle );
        return;
    }

    TextureData & u = mTextures.items[handle];
    u.value = id;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
GN_INLINE void GN::gfx::effect::Effect::setTextureByName( const StrA & name, uint32_t id ) const
{
    GN_GUARD_SLOW;
    uint32_t handle = getTextureHandle( name );
    if( 0 == handle ) return;
    setTexture( handle, id );
    GN_UNGUARD_SLOW;
}
