#ifndef __GN_ENGINE_MESH_H__
#define __GN_ENGINE_MESH_H__
// *****************************************************************************
/// \file
/// \brief   Mesh entities
/// \author  chen@@chenli-homepc (2011.2.28)
// *****************************************************************************

// Forward declaration.
namespace GN { namespace gfx { struct FatModel; } }

namespace GN { namespace engine
{
    ///
    /// Static mesh entity
    ///
    class StaticMesh : public Entity
    {
    public:

        /// constructor
        StaticMesh();

        /// destructor
        ~StaticMesh();

        /// get the spacial component
        const SpacialComponent & spacial() const { return mSpacial; }

        /// get the spacial component
        SpacialComponent & spacial() { return mSpacial; }

        /// get the visual component
        const VisualComponent & visual() const { return mVisual; }

        /// get the visual component
        VisualComponent & visual() { return mVisual; }

        /// Load the whole model hierarchy as a single static mesh.
        /// Existing content in the mesh will be discarded.
        bool loadFromModelHierarchy( const gfx::ModelHierarchyDesc & );

        /// Load the whole fat model as single static mesh
        bool loadFromFatModel( const GN::gfx::FatModel & );

        /// Load from file
        bool loadFromFile( const StrA & filename );

    private:
        StackRefCounter<SpacialComponent> mSpacial;
        StackRefCounter<VisualComponent>  mVisual;
    };

    struct SkinnedAnimationInfo
    {
        StrA   name;
        double duration;
    };

    ///
    /// Skinned Mesh Entity
    ///
    class SkinnedMesh : public Entity
    {
    public:

        /// constructor
        SkinnedMesh();

        /// destructor
        ~SkinnedMesh();

        /// Clear to an empty mesh.
        void clear();

        /// get the spacial component of the skeleton root.
        const SpacialComponent & spacial() const { return *mRootSpacial; }

        /// get the spacial component
        SpacialComponent & spacial() { return *mRootSpacial; }

        /// get the visual component
        const VisualComponent & visual() const { return *mVisual; }

        /// get the visual component
        VisualComponent & visual() { return *mVisual; }

        /// get number of animations
        size_t getAnimationCount() const { return mAnimations.size(); }

        /// get information about specific animation.
        bool getAnimationInfo( size_t animationIndex, SkinnedAnimationInfo & info );

        /// Set the animation and the time stamp.
        void setAnimation( size_t animationIndex, float seconds );

        /// Load the whole fat model as single skinned mesh
        bool loadFromFatModel( const GN::gfx::FatModel & );

        /// Load from file
        bool loadFromFile( const StrA & filename );

    private:

        struct SkinnedAnimation;

        struct Skeleton
        {
            DynaArray<AutoRef<SpacialComponent> > spacials;  //< Spacial components for each joint.
            DynaArray<Matrix44f>                  bindPoses; //< Bind pose transformation for each joint in the skeleton.
            AutoRef<gfx::UniformResource>         matrices;  //< Uniform resource that stores final joint matrices
        };

        AutoRef<SpacialComponent>                 mRootSpacial;
        AutoRef<VisualComponent>                  mVisual;
        AutoRef<gfx::EffectResource>              mSkinnedEffect;
        DynaArray<Skeleton>                       mSkeletons;
        DynaArray<SkinnedAnimation*>              mAnimations;
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_ENGINE_MESH_H__
