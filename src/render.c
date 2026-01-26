// EXPLANATION:
// To render the game viewport and screen shader(s)

void InitViewport(void)
{
    viewport = (RenderData){ .resScale = 2 };
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
    viewport.shader = LoadShader(0, TextFormat("assets/shaders/crt_newpixie%i.fs", GLSL_VERSION));
    viewport.textureLoc      = GetShaderLocation(viewport.shader, "texture0");
    viewport.resolutionLoc   = GetShaderLocation(viewport.shader, "resolution");
    viewport.timeLoc         = GetShaderLocation(viewport.shader, "time");
    viewport.curveLoc        = GetShaderLocation(viewport.shader, "curvature");
    viewport.wiggleToggleLoc = GetShaderLocation(viewport.shader, "wiggleToggle");
    viewport.scanrollLoc     = GetShaderLocation(viewport.shader, "scanroll");
    viewport.vignetteLoc     = GetShaderLocation(viewport.shader, "vignette");
    viewport.ghostingLoc     = GetShaderLocation(viewport.shader, "ghosting");
    viewport.useFrameLoc     = GetShaderLocation(viewport.shader, "useFrame");
    SetShaderValue(viewport.shader, viewport.curveLoc,        (float[]){1.5f},  SHADER_UNIFORM_FLOAT);
    SetShaderValue(viewport.shader, viewport.wiggleToggleLoc, (float[]){0},     SHADER_UNIFORM_FLOAT);
    SetShaderValue(viewport.shader, viewport.scanrollLoc,     (float[]){1.5f},  SHADER_UNIFORM_FLOAT);
    SetShaderValue(viewport.shader, viewport.vignetteLoc,     (float[]){1.01f}, SHADER_UNIFORM_FLOAT);
    SetShaderValue(viewport.shader, viewport.ghostingLoc,     (float[]){0.2f},  SHADER_UNIFORM_FLOAT);
    SetShaderValue(viewport.shader, viewport.useFrameLoc,     (float[]){0},     SHADER_UNIFORM_FLOAT);
    viewport.shaderEnabled = true;
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

void UpdateWindowShader(void)
{
    float res[2] = { GetRenderWidth(), GetRenderHeight() };
    SetShaderValueTexture(viewport.shader, viewport.textureLoc, viewport.renderTarget.texture);
    SetShaderValue(viewport.shader, viewport.resolutionLoc, &res, SHADER_UNIFORM_VEC2);
    SetShaderValue(viewport.shader, viewport.timeLoc, &game.frameTime, SHADER_UNIFORM_FLOAT);
}
