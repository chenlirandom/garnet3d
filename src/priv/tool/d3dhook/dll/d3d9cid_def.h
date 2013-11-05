// (Script generated header. DO NOT EDIT.)
// Define call ID for all D3D9 and DXGI methods.
#pragma once

enum D3D9_CALL_ID
{

    // CID for IDirect3D9
    CID_IDirect3D9_BASE,
    CID_IDirect3D9_COUNT = 14,
    CID_IDirect3D9_RegisterSoftwareDevice = CID_IDirect3D9_BASE + 0,
    CID_IDirect3D9_GetAdapterCount = CID_IDirect3D9_BASE + 1,
    CID_IDirect3D9_GetAdapterIdentifier = CID_IDirect3D9_BASE + 2,
    CID_IDirect3D9_GetAdapterModeCount = CID_IDirect3D9_BASE + 3,
    CID_IDirect3D9_EnumAdapterModes = CID_IDirect3D9_BASE + 4,
    CID_IDirect3D9_GetAdapterDisplayMode = CID_IDirect3D9_BASE + 5,
    CID_IDirect3D9_CheckDeviceType = CID_IDirect3D9_BASE + 6,
    CID_IDirect3D9_CheckDeviceFormat = CID_IDirect3D9_BASE + 7,
    CID_IDirect3D9_CheckDeviceMultiSampleType = CID_IDirect3D9_BASE + 8,
    CID_IDirect3D9_CheckDepthStencilMatch = CID_IDirect3D9_BASE + 9,
    CID_IDirect3D9_CheckDeviceFormatConversion = CID_IDirect3D9_BASE + 10,
    CID_IDirect3D9_GetDeviceCaps = CID_IDirect3D9_BASE + 11,
    CID_IDirect3D9_GetAdapterMonitor = CID_IDirect3D9_BASE + 12,
    CID_IDirect3D9_CreateDevice = CID_IDirect3D9_BASE + 13,

    // CID for IDirect3DDevice9
    CID_IDirect3DDevice9_BASE,
    CID_IDirect3DDevice9_COUNT = 116,
    CID_IDirect3DDevice9_TestCooperativeLevel = CID_IDirect3DDevice9_BASE + 0,
    CID_IDirect3DDevice9_GetAvailableTextureMem = CID_IDirect3DDevice9_BASE + 1,
    CID_IDirect3DDevice9_EvictManagedResources = CID_IDirect3DDevice9_BASE + 2,
    CID_IDirect3DDevice9_GetDirect3D = CID_IDirect3DDevice9_BASE + 3,
    CID_IDirect3DDevice9_GetDeviceCaps = CID_IDirect3DDevice9_BASE + 4,
    CID_IDirect3DDevice9_GetDisplayMode = CID_IDirect3DDevice9_BASE + 5,
    CID_IDirect3DDevice9_GetCreationParameters = CID_IDirect3DDevice9_BASE + 6,
    CID_IDirect3DDevice9_SetCursorProperties = CID_IDirect3DDevice9_BASE + 7,
    CID_IDirect3DDevice9_SetCursorPosition = CID_IDirect3DDevice9_BASE + 8,
    CID_IDirect3DDevice9_ShowCursor = CID_IDirect3DDevice9_BASE + 9,
    CID_IDirect3DDevice9_CreateAdditionalSwapChain = CID_IDirect3DDevice9_BASE + 10,
    CID_IDirect3DDevice9_GetSwapChain = CID_IDirect3DDevice9_BASE + 11,
    CID_IDirect3DDevice9_GetNumberOfSwapChains = CID_IDirect3DDevice9_BASE + 12,
    CID_IDirect3DDevice9_Reset = CID_IDirect3DDevice9_BASE + 13,
    CID_IDirect3DDevice9_Present = CID_IDirect3DDevice9_BASE + 14,
    CID_IDirect3DDevice9_GetBackBuffer = CID_IDirect3DDevice9_BASE + 15,
    CID_IDirect3DDevice9_GetRasterStatus = CID_IDirect3DDevice9_BASE + 16,
    CID_IDirect3DDevice9_SetDialogBoxMode = CID_IDirect3DDevice9_BASE + 17,
    CID_IDirect3DDevice9_SetGammaRamp = CID_IDirect3DDevice9_BASE + 18,
    CID_IDirect3DDevice9_GetGammaRamp = CID_IDirect3DDevice9_BASE + 19,
    CID_IDirect3DDevice9_CreateTexture = CID_IDirect3DDevice9_BASE + 20,
    CID_IDirect3DDevice9_CreateVolumeTexture = CID_IDirect3DDevice9_BASE + 21,
    CID_IDirect3DDevice9_CreateCubeTexture = CID_IDirect3DDevice9_BASE + 22,
    CID_IDirect3DDevice9_CreateVertexBuffer = CID_IDirect3DDevice9_BASE + 23,
    CID_IDirect3DDevice9_CreateIndexBuffer = CID_IDirect3DDevice9_BASE + 24,
    CID_IDirect3DDevice9_CreateRenderTarget = CID_IDirect3DDevice9_BASE + 25,
    CID_IDirect3DDevice9_CreateDepthStencilSurface = CID_IDirect3DDevice9_BASE + 26,
    CID_IDirect3DDevice9_UpdateSurface = CID_IDirect3DDevice9_BASE + 27,
    CID_IDirect3DDevice9_UpdateTexture = CID_IDirect3DDevice9_BASE + 28,
    CID_IDirect3DDevice9_GetRenderTargetData = CID_IDirect3DDevice9_BASE + 29,
    CID_IDirect3DDevice9_GetFrontBufferData = CID_IDirect3DDevice9_BASE + 30,
    CID_IDirect3DDevice9_StretchRect = CID_IDirect3DDevice9_BASE + 31,
    CID_IDirect3DDevice9_ColorFill = CID_IDirect3DDevice9_BASE + 32,
    CID_IDirect3DDevice9_CreateOffscreenPlainSurface = CID_IDirect3DDevice9_BASE + 33,
    CID_IDirect3DDevice9_SetRenderTarget = CID_IDirect3DDevice9_BASE + 34,
    CID_IDirect3DDevice9_GetRenderTarget = CID_IDirect3DDevice9_BASE + 35,
    CID_IDirect3DDevice9_SetDepthStencilSurface = CID_IDirect3DDevice9_BASE + 36,
    CID_IDirect3DDevice9_GetDepthStencilSurface = CID_IDirect3DDevice9_BASE + 37,
    CID_IDirect3DDevice9_BeginScene = CID_IDirect3DDevice9_BASE + 38,
    CID_IDirect3DDevice9_EndScene = CID_IDirect3DDevice9_BASE + 39,
    CID_IDirect3DDevice9_Clear = CID_IDirect3DDevice9_BASE + 40,
    CID_IDirect3DDevice9_SetTransform = CID_IDirect3DDevice9_BASE + 41,
    CID_IDirect3DDevice9_GetTransform = CID_IDirect3DDevice9_BASE + 42,
    CID_IDirect3DDevice9_MultiplyTransform = CID_IDirect3DDevice9_BASE + 43,
    CID_IDirect3DDevice9_SetViewport = CID_IDirect3DDevice9_BASE + 44,
    CID_IDirect3DDevice9_GetViewport = CID_IDirect3DDevice9_BASE + 45,
    CID_IDirect3DDevice9_SetMaterial = CID_IDirect3DDevice9_BASE + 46,
    CID_IDirect3DDevice9_GetMaterial = CID_IDirect3DDevice9_BASE + 47,
    CID_IDirect3DDevice9_SetLight = CID_IDirect3DDevice9_BASE + 48,
    CID_IDirect3DDevice9_GetLight = CID_IDirect3DDevice9_BASE + 49,
    CID_IDirect3DDevice9_LightEnable = CID_IDirect3DDevice9_BASE + 50,
    CID_IDirect3DDevice9_GetLightEnable = CID_IDirect3DDevice9_BASE + 51,
    CID_IDirect3DDevice9_SetClipPlane = CID_IDirect3DDevice9_BASE + 52,
    CID_IDirect3DDevice9_GetClipPlane = CID_IDirect3DDevice9_BASE + 53,
    CID_IDirect3DDevice9_SetRenderState = CID_IDirect3DDevice9_BASE + 54,
    CID_IDirect3DDevice9_GetRenderState = CID_IDirect3DDevice9_BASE + 55,
    CID_IDirect3DDevice9_CreateStateBlock = CID_IDirect3DDevice9_BASE + 56,
    CID_IDirect3DDevice9_BeginStateBlock = CID_IDirect3DDevice9_BASE + 57,
    CID_IDirect3DDevice9_EndStateBlock = CID_IDirect3DDevice9_BASE + 58,
    CID_IDirect3DDevice9_SetClipStatus = CID_IDirect3DDevice9_BASE + 59,
    CID_IDirect3DDevice9_GetClipStatus = CID_IDirect3DDevice9_BASE + 60,
    CID_IDirect3DDevice9_GetTexture = CID_IDirect3DDevice9_BASE + 61,
    CID_IDirect3DDevice9_SetTexture = CID_IDirect3DDevice9_BASE + 62,
    CID_IDirect3DDevice9_GetTextureStageState = CID_IDirect3DDevice9_BASE + 63,
    CID_IDirect3DDevice9_SetTextureStageState = CID_IDirect3DDevice9_BASE + 64,
    CID_IDirect3DDevice9_GetSamplerState = CID_IDirect3DDevice9_BASE + 65,
    CID_IDirect3DDevice9_SetSamplerState = CID_IDirect3DDevice9_BASE + 66,
    CID_IDirect3DDevice9_ValidateDevice = CID_IDirect3DDevice9_BASE + 67,
    CID_IDirect3DDevice9_SetPaletteEntries = CID_IDirect3DDevice9_BASE + 68,
    CID_IDirect3DDevice9_GetPaletteEntries = CID_IDirect3DDevice9_BASE + 69,
    CID_IDirect3DDevice9_SetCurrentTexturePalette = CID_IDirect3DDevice9_BASE + 70,
    CID_IDirect3DDevice9_GetCurrentTexturePalette = CID_IDirect3DDevice9_BASE + 71,
    CID_IDirect3DDevice9_SetScissorRect = CID_IDirect3DDevice9_BASE + 72,
    CID_IDirect3DDevice9_GetScissorRect = CID_IDirect3DDevice9_BASE + 73,
    CID_IDirect3DDevice9_SetSoftwareVertexProcessing = CID_IDirect3DDevice9_BASE + 74,
    CID_IDirect3DDevice9_GetSoftwareVertexProcessing = CID_IDirect3DDevice9_BASE + 75,
    CID_IDirect3DDevice9_SetNPatchMode = CID_IDirect3DDevice9_BASE + 76,
    CID_IDirect3DDevice9_GetNPatchMode = CID_IDirect3DDevice9_BASE + 77,
    CID_IDirect3DDevice9_DrawPrimitive = CID_IDirect3DDevice9_BASE + 78,
    CID_IDirect3DDevice9_DrawIndexedPrimitive = CID_IDirect3DDevice9_BASE + 79,
    CID_IDirect3DDevice9_DrawPrimitiveUP = CID_IDirect3DDevice9_BASE + 80,
    CID_IDirect3DDevice9_DrawIndexedPrimitiveUP = CID_IDirect3DDevice9_BASE + 81,
    CID_IDirect3DDevice9_ProcessVertices = CID_IDirect3DDevice9_BASE + 82,
    CID_IDirect3DDevice9_CreateVertexDeclaration = CID_IDirect3DDevice9_BASE + 83,
    CID_IDirect3DDevice9_SetVertexDeclaration = CID_IDirect3DDevice9_BASE + 84,
    CID_IDirect3DDevice9_GetVertexDeclaration = CID_IDirect3DDevice9_BASE + 85,
    CID_IDirect3DDevice9_SetFVF = CID_IDirect3DDevice9_BASE + 86,
    CID_IDirect3DDevice9_GetFVF = CID_IDirect3DDevice9_BASE + 87,
    CID_IDirect3DDevice9_CreateVertexShader = CID_IDirect3DDevice9_BASE + 88,
    CID_IDirect3DDevice9_SetVertexShader = CID_IDirect3DDevice9_BASE + 89,
    CID_IDirect3DDevice9_GetVertexShader = CID_IDirect3DDevice9_BASE + 90,
    CID_IDirect3DDevice9_SetVertexShaderConstantF = CID_IDirect3DDevice9_BASE + 91,
    CID_IDirect3DDevice9_GetVertexShaderConstantF = CID_IDirect3DDevice9_BASE + 92,
    CID_IDirect3DDevice9_SetVertexShaderConstantI = CID_IDirect3DDevice9_BASE + 93,
    CID_IDirect3DDevice9_GetVertexShaderConstantI = CID_IDirect3DDevice9_BASE + 94,
    CID_IDirect3DDevice9_SetVertexShaderConstantB = CID_IDirect3DDevice9_BASE + 95,
    CID_IDirect3DDevice9_GetVertexShaderConstantB = CID_IDirect3DDevice9_BASE + 96,
    CID_IDirect3DDevice9_SetStreamSource = CID_IDirect3DDevice9_BASE + 97,
    CID_IDirect3DDevice9_GetStreamSource = CID_IDirect3DDevice9_BASE + 98,
    CID_IDirect3DDevice9_SetStreamSourceFreq = CID_IDirect3DDevice9_BASE + 99,
    CID_IDirect3DDevice9_GetStreamSourceFreq = CID_IDirect3DDevice9_BASE + 100,
    CID_IDirect3DDevice9_SetIndices = CID_IDirect3DDevice9_BASE + 101,
    CID_IDirect3DDevice9_GetIndices = CID_IDirect3DDevice9_BASE + 102,
    CID_IDirect3DDevice9_CreatePixelShader = CID_IDirect3DDevice9_BASE + 103,
    CID_IDirect3DDevice9_SetPixelShader = CID_IDirect3DDevice9_BASE + 104,
    CID_IDirect3DDevice9_GetPixelShader = CID_IDirect3DDevice9_BASE + 105,
    CID_IDirect3DDevice9_SetPixelShaderConstantF = CID_IDirect3DDevice9_BASE + 106,
    CID_IDirect3DDevice9_GetPixelShaderConstantF = CID_IDirect3DDevice9_BASE + 107,
    CID_IDirect3DDevice9_SetPixelShaderConstantI = CID_IDirect3DDevice9_BASE + 108,
    CID_IDirect3DDevice9_GetPixelShaderConstantI = CID_IDirect3DDevice9_BASE + 109,
    CID_IDirect3DDevice9_SetPixelShaderConstantB = CID_IDirect3DDevice9_BASE + 110,
    CID_IDirect3DDevice9_GetPixelShaderConstantB = CID_IDirect3DDevice9_BASE + 111,
    CID_IDirect3DDevice9_DrawRectPatch = CID_IDirect3DDevice9_BASE + 112,
    CID_IDirect3DDevice9_DrawTriPatch = CID_IDirect3DDevice9_BASE + 113,
    CID_IDirect3DDevice9_DeletePatch = CID_IDirect3DDevice9_BASE + 114,
    CID_IDirect3DDevice9_CreateQuery = CID_IDirect3DDevice9_BASE + 115,

    // CID for IDirect3DStateBlock9
    CID_IDirect3DStateBlock9_BASE,
    CID_IDirect3DStateBlock9_COUNT = 3,
    CID_IDirect3DStateBlock9_GetDevice = CID_IDirect3DStateBlock9_BASE + 0,
    CID_IDirect3DStateBlock9_Capture = CID_IDirect3DStateBlock9_BASE + 1,
    CID_IDirect3DStateBlock9_Apply = CID_IDirect3DStateBlock9_BASE + 2,

    // CID for IDirect3DSwapChain9
    CID_IDirect3DSwapChain9_BASE,
    CID_IDirect3DSwapChain9_COUNT = 7,
    CID_IDirect3DSwapChain9_Present = CID_IDirect3DSwapChain9_BASE + 0,
    CID_IDirect3DSwapChain9_GetFrontBufferData = CID_IDirect3DSwapChain9_BASE + 1,
    CID_IDirect3DSwapChain9_GetBackBuffer = CID_IDirect3DSwapChain9_BASE + 2,
    CID_IDirect3DSwapChain9_GetRasterStatus = CID_IDirect3DSwapChain9_BASE + 3,
    CID_IDirect3DSwapChain9_GetDisplayMode = CID_IDirect3DSwapChain9_BASE + 4,
    CID_IDirect3DSwapChain9_GetDevice = CID_IDirect3DSwapChain9_BASE + 5,
    CID_IDirect3DSwapChain9_GetPresentParameters = CID_IDirect3DSwapChain9_BASE + 6,

    // CID for IDirect3DResource9
    CID_IDirect3DResource9_BASE,
    CID_IDirect3DResource9_COUNT = 8,
    CID_IDirect3DResource9_GetDevice = CID_IDirect3DResource9_BASE + 0,
    CID_IDirect3DResource9_SetPrivateData = CID_IDirect3DResource9_BASE + 1,
    CID_IDirect3DResource9_GetPrivateData = CID_IDirect3DResource9_BASE + 2,
    CID_IDirect3DResource9_FreePrivateData = CID_IDirect3DResource9_BASE + 3,
    CID_IDirect3DResource9_SetPriority = CID_IDirect3DResource9_BASE + 4,
    CID_IDirect3DResource9_GetPriority = CID_IDirect3DResource9_BASE + 5,
    CID_IDirect3DResource9_PreLoad = CID_IDirect3DResource9_BASE + 6,
    CID_IDirect3DResource9_GetType = CID_IDirect3DResource9_BASE + 7,

    // CID for IDirect3DVertexDeclaration9
    CID_IDirect3DVertexDeclaration9_BASE,
    CID_IDirect3DVertexDeclaration9_COUNT = 2,
    CID_IDirect3DVertexDeclaration9_GetDevice = CID_IDirect3DVertexDeclaration9_BASE + 0,
    CID_IDirect3DVertexDeclaration9_GetDeclaration = CID_IDirect3DVertexDeclaration9_BASE + 1,

    // CID for IDirect3DVertexShader9
    CID_IDirect3DVertexShader9_BASE,
    CID_IDirect3DVertexShader9_COUNT = 2,
    CID_IDirect3DVertexShader9_GetDevice = CID_IDirect3DVertexShader9_BASE + 0,
    CID_IDirect3DVertexShader9_GetFunction = CID_IDirect3DVertexShader9_BASE + 1,

    // CID for IDirect3DPixelShader9
    CID_IDirect3DPixelShader9_BASE,
    CID_IDirect3DPixelShader9_COUNT = 2,
    CID_IDirect3DPixelShader9_GetDevice = CID_IDirect3DPixelShader9_BASE + 0,
    CID_IDirect3DPixelShader9_GetFunction = CID_IDirect3DPixelShader9_BASE + 1,

    // CID for IDirect3DBaseTexture9
    CID_IDirect3DBaseTexture9_BASE,
    CID_IDirect3DBaseTexture9_COUNT = 6,
    CID_IDirect3DBaseTexture9_SetLOD = CID_IDirect3DBaseTexture9_BASE + 0,
    CID_IDirect3DBaseTexture9_GetLOD = CID_IDirect3DBaseTexture9_BASE + 1,
    CID_IDirect3DBaseTexture9_GetLevelCount = CID_IDirect3DBaseTexture9_BASE + 2,
    CID_IDirect3DBaseTexture9_SetAutoGenFilterType = CID_IDirect3DBaseTexture9_BASE + 3,
    CID_IDirect3DBaseTexture9_GetAutoGenFilterType = CID_IDirect3DBaseTexture9_BASE + 4,
    CID_IDirect3DBaseTexture9_GenerateMipSubLevels = CID_IDirect3DBaseTexture9_BASE + 5,

    // CID for IDirect3DTexture9
    CID_IDirect3DTexture9_BASE,
    CID_IDirect3DTexture9_COUNT = 5,
    CID_IDirect3DTexture9_GetLevelDesc = CID_IDirect3DTexture9_BASE + 0,
    CID_IDirect3DTexture9_GetSurfaceLevel = CID_IDirect3DTexture9_BASE + 1,
    CID_IDirect3DTexture9_LockRect = CID_IDirect3DTexture9_BASE + 2,
    CID_IDirect3DTexture9_UnlockRect = CID_IDirect3DTexture9_BASE + 3,
    CID_IDirect3DTexture9_AddDirtyRect = CID_IDirect3DTexture9_BASE + 4,

    // CID for IDirect3DVolumeTexture9
    CID_IDirect3DVolumeTexture9_BASE,
    CID_IDirect3DVolumeTexture9_COUNT = 5,
    CID_IDirect3DVolumeTexture9_GetLevelDesc = CID_IDirect3DVolumeTexture9_BASE + 0,
    CID_IDirect3DVolumeTexture9_GetVolumeLevel = CID_IDirect3DVolumeTexture9_BASE + 1,
    CID_IDirect3DVolumeTexture9_LockBox = CID_IDirect3DVolumeTexture9_BASE + 2,
    CID_IDirect3DVolumeTexture9_UnlockBox = CID_IDirect3DVolumeTexture9_BASE + 3,
    CID_IDirect3DVolumeTexture9_AddDirtyBox = CID_IDirect3DVolumeTexture9_BASE + 4,

    // CID for IDirect3DCubeTexture9
    CID_IDirect3DCubeTexture9_BASE,
    CID_IDirect3DCubeTexture9_COUNT = 5,
    CID_IDirect3DCubeTexture9_GetLevelDesc = CID_IDirect3DCubeTexture9_BASE + 0,
    CID_IDirect3DCubeTexture9_GetCubeMapSurface = CID_IDirect3DCubeTexture9_BASE + 1,
    CID_IDirect3DCubeTexture9_LockRect = CID_IDirect3DCubeTexture9_BASE + 2,
    CID_IDirect3DCubeTexture9_UnlockRect = CID_IDirect3DCubeTexture9_BASE + 3,
    CID_IDirect3DCubeTexture9_AddDirtyRect = CID_IDirect3DCubeTexture9_BASE + 4,

    // CID for IDirect3DVertexBuffer9
    CID_IDirect3DVertexBuffer9_BASE,
    CID_IDirect3DVertexBuffer9_COUNT = 3,
    CID_IDirect3DVertexBuffer9_Lock = CID_IDirect3DVertexBuffer9_BASE + 0,
    CID_IDirect3DVertexBuffer9_Unlock = CID_IDirect3DVertexBuffer9_BASE + 1,
    CID_IDirect3DVertexBuffer9_GetDesc = CID_IDirect3DVertexBuffer9_BASE + 2,

    // CID for IDirect3DIndexBuffer9
    CID_IDirect3DIndexBuffer9_BASE,
    CID_IDirect3DIndexBuffer9_COUNT = 3,
    CID_IDirect3DIndexBuffer9_Lock = CID_IDirect3DIndexBuffer9_BASE + 0,
    CID_IDirect3DIndexBuffer9_Unlock = CID_IDirect3DIndexBuffer9_BASE + 1,
    CID_IDirect3DIndexBuffer9_GetDesc = CID_IDirect3DIndexBuffer9_BASE + 2,

    // CID for IDirect3DSurface9
    CID_IDirect3DSurface9_BASE,
    CID_IDirect3DSurface9_COUNT = 6,
    CID_IDirect3DSurface9_GetContainer = CID_IDirect3DSurface9_BASE + 0,
    CID_IDirect3DSurface9_GetDesc = CID_IDirect3DSurface9_BASE + 1,
    CID_IDirect3DSurface9_LockRect = CID_IDirect3DSurface9_BASE + 2,
    CID_IDirect3DSurface9_UnlockRect = CID_IDirect3DSurface9_BASE + 3,
    CID_IDirect3DSurface9_GetDC = CID_IDirect3DSurface9_BASE + 4,
    CID_IDirect3DSurface9_ReleaseDC = CID_IDirect3DSurface9_BASE + 5,

    // CID for IDirect3DVolume9
    CID_IDirect3DVolume9_BASE,
    CID_IDirect3DVolume9_COUNT = 8,
    CID_IDirect3DVolume9_GetDevice = CID_IDirect3DVolume9_BASE + 0,
    CID_IDirect3DVolume9_SetPrivateData = CID_IDirect3DVolume9_BASE + 1,
    CID_IDirect3DVolume9_GetPrivateData = CID_IDirect3DVolume9_BASE + 2,
    CID_IDirect3DVolume9_FreePrivateData = CID_IDirect3DVolume9_BASE + 3,
    CID_IDirect3DVolume9_GetContainer = CID_IDirect3DVolume9_BASE + 4,
    CID_IDirect3DVolume9_GetDesc = CID_IDirect3DVolume9_BASE + 5,
    CID_IDirect3DVolume9_LockBox = CID_IDirect3DVolume9_BASE + 6,
    CID_IDirect3DVolume9_UnlockBox = CID_IDirect3DVolume9_BASE + 7,

    // CID for IDirect3DQuery9
    CID_IDirect3DQuery9_BASE,
    CID_IDirect3DQuery9_COUNT = 5,
    CID_IDirect3DQuery9_GetDevice = CID_IDirect3DQuery9_BASE + 0,
    CID_IDirect3DQuery9_GetType = CID_IDirect3DQuery9_BASE + 1,
    CID_IDirect3DQuery9_GetDataSize = CID_IDirect3DQuery9_BASE + 2,
    CID_IDirect3DQuery9_Issue = CID_IDirect3DQuery9_BASE + 3,
    CID_IDirect3DQuery9_GetData = CID_IDirect3DQuery9_BASE + 4,

    // CID for IDirect3D9Ex
    CID_IDirect3D9Ex_BASE,
    CID_IDirect3D9Ex_COUNT = 5,
    CID_IDirect3D9Ex_GetAdapterModeCountEx = CID_IDirect3D9Ex_BASE + 0,
    CID_IDirect3D9Ex_EnumAdapterModesEx = CID_IDirect3D9Ex_BASE + 1,
    CID_IDirect3D9Ex_GetAdapterDisplayModeEx = CID_IDirect3D9Ex_BASE + 2,
    CID_IDirect3D9Ex_CreateDeviceEx = CID_IDirect3D9Ex_BASE + 3,
    CID_IDirect3D9Ex_GetAdapterLUID = CID_IDirect3D9Ex_BASE + 4,

    // CID for IDirect3DDevice9Ex
    CID_IDirect3DDevice9Ex_BASE,
    CID_IDirect3DDevice9Ex_COUNT = 15,
    CID_IDirect3DDevice9Ex_SetConvolutionMonoKernel = CID_IDirect3DDevice9Ex_BASE + 0,
    CID_IDirect3DDevice9Ex_ComposeRects = CID_IDirect3DDevice9Ex_BASE + 1,
    CID_IDirect3DDevice9Ex_PresentEx = CID_IDirect3DDevice9Ex_BASE + 2,
    CID_IDirect3DDevice9Ex_GetGPUThreadPriority = CID_IDirect3DDevice9Ex_BASE + 3,
    CID_IDirect3DDevice9Ex_SetGPUThreadPriority = CID_IDirect3DDevice9Ex_BASE + 4,
    CID_IDirect3DDevice9Ex_WaitForVBlank = CID_IDirect3DDevice9Ex_BASE + 5,
    CID_IDirect3DDevice9Ex_CheckResourceResidency = CID_IDirect3DDevice9Ex_BASE + 6,
    CID_IDirect3DDevice9Ex_SetMaximumFrameLatency = CID_IDirect3DDevice9Ex_BASE + 7,
    CID_IDirect3DDevice9Ex_GetMaximumFrameLatency = CID_IDirect3DDevice9Ex_BASE + 8,
    CID_IDirect3DDevice9Ex_CheckDeviceState = CID_IDirect3DDevice9Ex_BASE + 9,
    CID_IDirect3DDevice9Ex_CreateRenderTargetEx = CID_IDirect3DDevice9Ex_BASE + 10,
    CID_IDirect3DDevice9Ex_CreateOffscreenPlainSurfaceEx = CID_IDirect3DDevice9Ex_BASE + 11,
    CID_IDirect3DDevice9Ex_CreateDepthStencilSurfaceEx = CID_IDirect3DDevice9Ex_BASE + 12,
    CID_IDirect3DDevice9Ex_ResetEx = CID_IDirect3DDevice9Ex_BASE + 13,
    CID_IDirect3DDevice9Ex_GetDisplayModeEx = CID_IDirect3DDevice9Ex_BASE + 14,

    // CID for IDirect3DSwapChain9Ex
    CID_IDirect3DSwapChain9Ex_BASE,
    CID_IDirect3DSwapChain9Ex_COUNT = 3,
    CID_IDirect3DSwapChain9Ex_GetLastPresentCount = CID_IDirect3DSwapChain9Ex_BASE + 0,
    CID_IDirect3DSwapChain9Ex_GetPresentStats = CID_IDirect3DSwapChain9Ex_BASE + 1,
    CID_IDirect3DSwapChain9Ex_GetDisplayModeEx = CID_IDirect3DSwapChain9Ex_BASE + 2,

    // CID for IDirect3D9ExOverlayExtension
    CID_IDirect3D9ExOverlayExtension_BASE,
    CID_IDirect3D9ExOverlayExtension_COUNT = 1,
    CID_IDirect3D9ExOverlayExtension_CheckDeviceOverlayType = CID_IDirect3D9ExOverlayExtension_BASE + 0,

    // CID for IDirect3DDevice9Video
    CID_IDirect3DDevice9Video_BASE,
    CID_IDirect3DDevice9Video_COUNT = 3,
    CID_IDirect3DDevice9Video_GetContentProtectionCaps = CID_IDirect3DDevice9Video_BASE + 0,
    CID_IDirect3DDevice9Video_CreateAuthenticatedChannel = CID_IDirect3DDevice9Video_BASE + 1,
    CID_IDirect3DDevice9Video_CreateCryptoSession = CID_IDirect3DDevice9Video_BASE + 2,

    // CID for IDirect3DAuthenticatedChannel9
    CID_IDirect3DAuthenticatedChannel9_BASE,
    CID_IDirect3DAuthenticatedChannel9_COUNT = 5,
    CID_IDirect3DAuthenticatedChannel9_GetCertificateSize = CID_IDirect3DAuthenticatedChannel9_BASE + 0,
    CID_IDirect3DAuthenticatedChannel9_GetCertificate = CID_IDirect3DAuthenticatedChannel9_BASE + 1,
    CID_IDirect3DAuthenticatedChannel9_NegotiateKeyExchange = CID_IDirect3DAuthenticatedChannel9_BASE + 2,
    CID_IDirect3DAuthenticatedChannel9_Query = CID_IDirect3DAuthenticatedChannel9_BASE + 3,
    CID_IDirect3DAuthenticatedChannel9_Configure = CID_IDirect3DAuthenticatedChannel9_BASE + 4,

    // CID for IDirect3DCryptoSession9
    CID_IDirect3DCryptoSession9_BASE,
    CID_IDirect3DCryptoSession9_COUNT = 9,
    CID_IDirect3DCryptoSession9_GetCertificateSize = CID_IDirect3DCryptoSession9_BASE + 0,
    CID_IDirect3DCryptoSession9_GetCertificate = CID_IDirect3DCryptoSession9_BASE + 1,
    CID_IDirect3DCryptoSession9_NegotiateKeyExchange = CID_IDirect3DCryptoSession9_BASE + 2,
    CID_IDirect3DCryptoSession9_EncryptionBlt = CID_IDirect3DCryptoSession9_BASE + 3,
    CID_IDirect3DCryptoSession9_DecryptionBlt = CID_IDirect3DCryptoSession9_BASE + 4,
    CID_IDirect3DCryptoSession9_GetSurfacePitch = CID_IDirect3DCryptoSession9_BASE + 5,
    CID_IDirect3DCryptoSession9_StartSessionKeyRefresh = CID_IDirect3DCryptoSession9_BASE + 6,
    CID_IDirect3DCryptoSession9_FinishSessionKeyRefresh = CID_IDirect3DCryptoSession9_BASE + 7,
    CID_IDirect3DCryptoSession9_GetEncryptionBltKey = CID_IDirect3DCryptoSession9_BASE + 8,

    CID_TOTAL_COUNT,
    CID_INVALID = 0xFFFFFFFF,
}; // end of enum definition

extern const char * const g_D3D9CallIDText;