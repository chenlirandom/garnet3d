// script generated file. DO NOT edit.

#include "pch.h"
#include "hooks.h"

// -----------------------------------------------------------------------------
void STDMETHODCALLTYPE D3D11Texture1DHook::GetDesc(
    _Out_  D3D11_TEXTURE1D_DESC * pDesc)
{
    calltrace::AutoTrace trace(L"D3D11Texture1DHook::GetDesc");
    if (_GetDesc_pre_ptr._value) { (this->*_GetDesc_pre_ptr._value)(pDesc); }
    GetRealObj()->GetDesc(pDesc);
    if (_GetDesc_post_ptr._value) { (this->*_GetDesc_post_ptr._value)(pDesc); }
}

