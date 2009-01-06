//
// DO NOT EDIT. Generated by "gencmd.cmd"
//

namespace GN { namespace gfx
{

///
/// Renderer command enumerations.
///
enum RndrCommand {
CMD_GET_RENDERER_OPTIONS,///< get renderer options
CMD_GET_DISP_DESC,///< get display descriptor
CMD_GET_D3D_DEVICE,
CMD_GET_OGL_RC,
CMD_GET_CAPS,
CMD_CHECK_TEXTURE_FORMAT_SUPPORT,
CMD_GET_DEFAULT_TEXTURE_FORMAT,
CMD_COMPILE_GPU_PROGRAM,
CMD_CREATE_GPU_PROGRAM,
CMD_CREATE_TEXTURE,
CMD_CREATE_VTXBUF,
CMD_CREATE_IDXBUF,
CMD_BIND_CONTEXT,
CMD_REBIND_CONTEXT,
CMD_GET_CONTEXT,
CMD_PRESENT,
CMD_CLEAR_SCREEN,
CMD_DRAW_INDEXED,
CMD_DRAW,
CMD_DRAW_INDEXED_UP,
CMD_DRAW_UP,
CMD_DRAW_LINES,
CMD_ENABLE_PARAMETER_CHECK,
CMD_DUMP_NEXT_FRAME,
};

///
/// Renderer command handler type.
///
typedef void (*RndrCommandHandler)( Renderer & r, void * param, size_t size );

///
/// Renderer command handler table.
///
extern const RndrCommandHandler g_rndrCommandHandlers[];

}} // end of file
