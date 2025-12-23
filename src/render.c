// EXPLANATION:
// To render the game viewport and screen shader(s)

void InitViewport(void)
{
    viewport = (RenderData){ .resScale = 1 + 1.0f/3.0f }; // means 1440p default
    InitRenderTexture();
    InitScreenShader();
}

void InitRenderTexture(void)
{
    if (IsRenderTextureValid(viewport.renderTarget))
        UnloadRenderTexture(viewport.renderTarget);

    // Render texture, for setting a desired render resolution
    viewport.renderTexWidth = (float)BASE_RENDER_WIDTH*viewport.resScale;
    viewport.renderTexHeight = (float)BASE_RENDER_HEIGHT*viewport.resScale;
    viewport.renderTarget = LoadRenderTexture((int)viewport.renderTexWidth,
                                            (int)viewport.renderTexHeight);
    SetTextureFilter(viewport.renderTarget.texture, TEXTURE_FILTER_BILINEAR);
}

void InitScreenShader(void)
{
    // Init shader
    // viewport.shader = LoadShader(0, TextFormat("assets/shaders/pixel%i.fs", GLSL_VERSION));
    // viewport.shaderRenderWidthLoc = GetShaderLocation(viewport.shader, "renderWidth");
    // viewport.shaderRenderHeightLoc = GetShaderLocation(viewport.shader, "renderHeight");
    // viewport.shaderPixelWidthLoc = GetShaderLocation(viewport.shader, "pixelWidth");
    // viewport.shaderPixelHeightLoc = GetShaderLocation(viewport.shader, "pixelHeight");
    // viewport.pixelSize = SHADER_PIXEL_SIZE;
    // SetShaderValue(viewport.shader, viewport.shaderRenderWidthLoc, &viewport.renderTexWidth, SHADER_UNIFORM_FLOAT);
    // SetShaderValue(viewport.shader, viewport.shaderRenderHeightLoc, &viewport.renderTexHeight, SHADER_UNIFORM_FLOAT);
    // SetShaderValue(viewport.shader, viewport.shaderPixelWidthLoc, &viewport.pixelSize, SHADER_UNIFORM_FLOAT);
    // SetShaderValue(viewport.shader, viewport.shaderPixelHeightLoc, &viewport.pixelSize, SHADER_UNIFORM_FLOAT);
    viewport.shaderEnabled = false;
}

// Updates window render info for each frame
void UpdateWindowRenderFrame(void)
{
    float winWidth = (float)GetRenderWidth();
    float winHeight = (float)GetRenderHeight();
    viewport.width = winWidth;
    viewport.height = winHeight;

    viewport.scale = fminf(viewport.width/viewport.renderTexWidth, viewport.height/viewport.renderTexHeight);

    float windowAspect = winWidth/winHeight;

    if (windowAspect > ASPECT_RATIO)
    {
        // Window too wide → pillarbox
        viewport.height = winHeight;
        viewport.width = (winHeight*ASPECT_RATIO);
        viewport.x = (winWidth - viewport.width)/2;
        viewport.y = 0;
    }
    else
    {
        // Window too tall → letterbox
        viewport.width = winWidth;
        viewport.height = winWidth/ASPECT_RATIO;
        viewport.x = 0;
        viewport.y = (winHeight - viewport.height)/2;
    }

    // adjust ui camera for possible window resize
    ui.camera.offset = (Vector2) {
        viewport.x + viewport.width/2.0f, viewport.y + viewport.height/2.0f
    };
    ui.camera.zoom = viewport.width/VIRTUAL_WIDTH;
}
