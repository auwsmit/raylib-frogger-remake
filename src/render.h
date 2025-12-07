// EXPLANATION:
// To render the game window and screen shader(s)

#ifndef FROGGER_RENDER_HEADER_GUARD
#define FROGGER_RENDER_HEADER_GUARD

#define SHADER_PIXEL_SIZE 8

// Types and Structures
// ----------------------------------------------------------------------------
typedef struct RenderData {
    RenderTexture renderTarget;
    float width, height, scale, x, y,
          renderTexWidth, renderTexHeight, resScale;

    Shader shader;
    int shaderRenderWidthLoc,  shaderRenderHeightLoc,
        shaderPixelWidthLoc, shaderPixelHeightLoc;
    float pixelSize;
    bool shaderEnabled;
} RenderData;

extern RenderData render; // global declaration

// Prototypes
// ----------------------------------------------------------------------------
void InitWindowRender(void);
void InitRenderTexture(void);
void InitScreenShader(void);
void UpdateWindowRenderFrame(void); // update window for aspect ratio, cameras, and shaders

#endif // FROGGER_RENDER_HEADER_GUARD

