#ifndef __GN_ENGINE2_FENCEMANAGER_H__
#define __GN_ENGINE2_FENCEMANAGER_H__
// *****************************************************************************
/// \file
/// \brief   fence manager
/// \author  chen@@CHENLI-HP (2007.5.2)
// *****************************************************************************

#include "fence.h"

namespace GN { namespace engine2
{
    ///
    /// managing command fence, shared by resource and draw commands.
    ///
    class RenderEngine::FenceManager
    {
        RenderEngine & mEngine;

        FenceId mFence;

    public:

        ///
        /// ctor
        ///
        FenceManager( RenderEngine & engine ) : mEngine(engine), mFence(0) {}

        //@{

        ///
        /// get current fence value, increase fence value by 1.
        ///
        FenceId getAndIncFence()
        {
            return ++mFence;
        }

        //@}

    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_ENGINE2_FENCEMANAGER_H__