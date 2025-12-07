// EXPLANATION:
// To render the game window and screen shader(s)

void InitWindowRender(void)
{
    render = (RenderData){ 0 };
    InitRenderTexture();
    InitScreenShader();
}

void InitRenderTexture(void)
{
    // Render texture, for setting a desired render resolution
    render.renderTexWidth = (float)GetRenderWidth();
    render.renderTexHeight = (float)GetRenderHeight();
    render.renderTarget = LoadRenderTexture((int)render.renderTexWidth,
                                            (int)render.renderTexHeight);
    SetTextureFilter(render.renderTarget.texture, TEXTURE_FILTER_BILINEAR);
}

void InitScreenShader(void)
{
    // Init shader
    // render.shader = LoadShader(0, TextFormat("assets/shaders/pixel%i.fs", GLSL_VERSION));
    // render.shaderRenderWidthLoc = GetShaderLocation(render.shader, "renderWidth");
    // render.shaderRenderHeightLoc = GetShaderLocation(render.shader, "renderHeight");
    // render.shaderPixelWidthLoc = GetShaderLocation(render.shader, "pixelWidth");
    // render.shaderPixelHeightLoc = GetShaderLocation(render.shader, "pixelHeight");
    // render.pixelSize = SHADER_PIXEL_SIZE;
    // SetShaderValue(render.shader, render.shaderRenderWidthLoc, &render.renderTexWidth, SHADER_UNIFORM_FLOAT);
    // SetShaderValue(render.shader, render.shaderRenderHeightLoc, &render.renderTexHeight, SHADER_UNIFORM_FLOAT);
    // SetShaderValue(render.shader, render.shaderPixelWidthLoc, &render.pixelSize, SHADER_UNIFORM_FLOAT);
    // SetShaderValue(render.shader, render.shaderPixelHeightLoc, &render.pixelSize, SHADER_UNIFORM_FLOAT);
    render.shaderEnabled = false;
}

// Updates window render info for each frame
void UpdateWindowRenderFrame(void)
{
    render.width = (float)GetRenderWidth(); // TODO configurable resolution
    render.height = (float)GetRenderHeight();

    render.scale = fminf(render.width/render.renderTexWidth, render.height/render.renderTexHeight);

    // int winWidth = GetRenderWidth();
    // int winHeight = GetRenderWidth();
    int winWidth = (int)render.width;
    int winHeight = (int)render.height;
    float windowAspect = (float)winWidth/(float)winHeight;

    if (windowAspect > ASPECT_RATIO)
    {
        // Window too wide → pillarbox
        render.height = (float)winHeight;
        render.width = (float)(winHeight*ASPECT_RATIO);
        render.x = (winWidth - render.width)/2;
        render.y = 0;
    }
    else
    {
        // Window too tall → letterbox
        render.width = (float)winWidth;
        render.height = (float)(winWidth/ASPECT_RATIO);
        render.x = 0;
        render.y = (winHeight - render.height)/2;
    }

    // adjust ui camera for possible window resize
    ui.camera.offset = (Vector2) {
        render.x + render.width/2.0f, render.y + render.height/2.0f
    };
    ui.camera.zoom = (float)render.width/VIRTUAL_WIDTH;
}
