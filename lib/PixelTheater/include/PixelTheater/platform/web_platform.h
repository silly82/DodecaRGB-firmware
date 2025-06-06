#pragma once

#include "PixelTheater/core/crgb.h"
#include "PixelTheater/platform/platform.h"
#include "PixelTheater/platform/webgl/camera.h"
#include "PixelTheater/platform/webgl/renderer.h"
#include "PixelTheater/platform/webgl/mesh.h"
#include "PixelTheater/model_def.h"
#include "PixelTheater/platform/webgl/web_model.h"
#include "PixelTheater/platform/webgl/shaders.h"
#include "PixelTheater/model/point.h"

#include <functional>
#include <vector>
#include <array>
#include <cstdint>
#include <memory>

// Only include WebGL and Emscripten headers in web builds
#if defined(PLATFORM_WEB) || defined(EMSCRIPTEN)
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

namespace PixelTheater {

// Add zoom level enum
enum class ZoomLevel {
    CLOSE,
    NORMAL,
    FAR
};

class WebPlatform : public Platform {
public:
    // ============================================
    // Configuration Parameters
    // ============================================
    
    // LED Appearance
    static constexpr float DEFAULT_LED_SIZE = 0.7f;           // Default LED size ratio (1.0 = physically accurate)
    static constexpr float MIN_LED_SIZE_RATIO = 0.3f;         // Minimum LED size ratio
    static constexpr float MAX_LED_SIZE_RATIO = 1.5f;         // Maximum LED size ratio
    static constexpr float PHYSICAL_LED_DIAMETER = 3.8f;      // Physical diameter of each LED in mm
    static constexpr float PHYSICAL_FACE_EDGE = 107.3f;       // Physical edge length of each face in mm
    static constexpr float DEFAULT_ATMOSPHERE_INTENSITY = 1.6f; // Default atmospheric glow intensity
    static constexpr float MIN_ATMOSPHERE_INTENSITY = 0.0f;    // Minimum atmospheric effect
    static constexpr float MAX_ATMOSPHERE_INTENSITY = 2.5f;    // Maximum atmospheric effect
    static constexpr float DEFAULT_LED_SPACING = 5.0f;        // Spacing between LEDs
    static constexpr uint8_t DEFAULT_BRIGHTNESS = 240;        // Initial brightness (0-255)
    
    // Camera Settings
    static constexpr float CAMERA_CLOSE_DISTANCE = 22.0f;      // Close zoom (increased from 12.0f)
    static constexpr float CAMERA_NORMAL_DISTANCE = 28.0f;     // Medium distance (increased from 20.0f)
    static constexpr float CAMERA_FAR_DISTANCE = 55.0f;        // Far zoom (increased from 35.0f)
    static constexpr float CAMERA_FOV_DEGREES = 50.0f;         // Field of view in degrees
    static constexpr float CAMERA_NEAR_PLANE = 0.1f;           // Near clipping plane
    static constexpr float CAMERA_FAR_PLANE = 100.0f;          // Far clipping plane
    
    // Rotation Settings
    static constexpr float ROTATION_SCALE = 0.004f;            // Rotation scale for mouse movement (reduced for smoother turntable)
    static constexpr float MAX_VERTICAL_ROTATION = 1.5f;       // Maximum vertical rotation (about 85 degrees)
    static constexpr float DEFAULT_AUTO_ROTATION_SPEED = 0.4f;  // Default to slow speed
    static constexpr float AUTO_ROTATION_TIME_SCALE = 1.0f;     // No additional scaling needed
    
    // Shader Effects
    static constexpr float COLOR_BRIGHTNESS_BOOST = 1.0f;      // Multiplier for LED color brightness
    static constexpr float MIN_LED_BRIGHTNESS = 0.05f;         // Minimum brightness for visible LEDs
    static constexpr float MAX_DEPTH_FADE = 6.0f;              // Maximum depth for LED visibility fade
    static constexpr float MIN_DEPTH_FADE = 0.4f;              // Minimum depth fade value
    
    explicit WebPlatform();
    ~WebPlatform() override;

    // Prevent copying
    WebPlatform(const WebPlatform&) = delete;
    WebPlatform& operator=(const WebPlatform&) = delete;

    // Model loading (Web-specific)
#if defined(PLATFORM_WEB) || defined(EMSCRIPTEN)
    template<typename ModelDef>
    WebModel createWebModel() {
        WebModel model;
        
        // Metadata
        model.metadata.name = ModelDef::NAME;
        model.metadata.version = ModelDef::VERSION;
        model.metadata.num_leds = ModelDef::LED_COUNT;
        
        // LED positions
        model.leds.positions.reserve(ModelDef::LED_COUNT);
        for (uint16_t i = 0; i < ModelDef::LED_COUNT; i++) {
            // Get coordinates directly from POINTS array
            const auto& point = ModelDef::POINTS[i];
            model.leds.positions.push_back({point.x, point.y, point.z});
        }
        
        // Geometry
        model.geometry.faces.reserve(ModelDef::FACE_COUNT);
        for (uint16_t face = 0; face < ModelDef::FACE_COUNT; face++) {
            WebFace web_face;
            for (uint16_t i = 0; i < 5; i++) {
                web_face.vertices[i] = {
                    ModelDef::FACES[face].vertices[i].x,
                    ModelDef::FACES[face].vertices[i].y,
                    ModelDef::FACES[face].vertices[i].z
                };
            }
            model.geometry.faces.push_back(web_face);
        }
        
        return model;
    }

    // Initialize with a specific model (Web-specific)
    template<typename ModelDef>
    void initializeWithModel() {
        WebModel model = createWebModel<ModelDef>();
        initializeFromWebModel(model);
    }
#endif // defined(PLATFORM_WEB) || defined(EMSCRIPTEN)

    // Core LED array management
    CRGB* getLEDs() override;
    uint16_t getNumLEDs() const override;

    // Hardware control operations
    void show() override;
    void setBrightness(uint8_t brightness) override;
    uint8_t getBrightness() const;
    void clear() override;

    // Performance settings
    void setMaxRefreshRate(uint8_t fps) override;
    void setDither(uint8_t dither) override;

    // Timing Utilities
    float deltaTime() override; // Non-const override
    uint32_t millis() override; // Non-const override (must match base)

    // Random Number Utilities (Overrides from Platform)
    uint8_t random8() override;
    uint16_t random16() override;
    uint32_t random(uint32_t max = 0) override;
    uint32_t random(uint32_t min, uint32_t max) override;
    float randomFloat() override; // 0.0 to 1.0
    float randomFloat(float max) override; // 0.0 to max
    float randomFloat(float min, float max) override; // min to max

    // Logging Utilities (Overrides from Platform, updated signatures)
    void logInfo(const char* format, ...) override;
    void logWarning(const char* format, ...) override;
    void logError(const char* format, ...) override;

#if defined(PLATFORM_WEB) || defined(EMSCRIPTEN)
    // WebGL-specific methods - only available in web builds
    void setLEDSize(float size);
    float getLEDSize() const;
    void setAtmosphereIntensity(float intensity);
    float getAtmosphereIntensity() const;
    void setLEDSpacing(float spacing);
    
    // Mesh visualization methods
    void setShowMesh(bool show);
    bool getShowMesh() const;
    void setMeshOpacity(float opacity);
    float getMeshOpacity() const;
    void setShowWireframe(bool show);
    bool getShowWireframe() const;
    
    // Rotation and view control
    void updateRotation(float deltaX, float deltaY);
    void resetRotation();
    void setAutoRotation(bool enabled, float speed = DEFAULT_AUTO_ROTATION_SPEED);
    void setZoomLevel(int zoom_level);
    
    // JavaScript interface methods (called from JS)
    void onCanvasResize(int width, int height);
    void onMouseDown(int x, int y);
    void onMouseMove(int x, int y, bool shift_key);
    void onMouseUp();
    void onMouseWheel(float delta);

    // WebGL resource management
    EMSCRIPTEN_KEEPALIVE void cleanupWebGL();

    // Update a scene parameter in the animation controller
    void updateSceneParameter(const char* param_id, float value);
#endif // End of web-specific PUBLIC methods

private:
#if defined(PLATFORM_WEB) || defined(EMSCRIPTEN)
    // --- Start of Web-Specific Private Section --- 
    // Web-specific private methods
    void initializeFromWebModel(const WebModel& model);
    void initWebGL();
    void updateVertexBuffer();
    void renderFrame();
    void updateAutoRotation();
    void renderMesh(const float* viewMatrix, const float* projectionMatrix, const float* modelMatrix);
    void renderLEDs(const float* viewMatrix, const float* projectionMatrix, const float* modelMatrix);

    // WebGL resource handles
    std::unique_ptr<WebGLRenderer> _renderer;
    std::unique_ptr<MeshGenerator> _mesh_generator;
    std::unique_ptr<Camera> _camera;
    
    // Shader programs
    GLuint _led_shader_program = 0;
    GLuint _mesh_shader_program = 0;
    GLuint _glow_shader_program = 0;
    GLuint _blur_shader_program = 0;
    GLuint _composite_shader_program = 0;

    // Vertex Buffer Objects (VBOs) and Vertex Array Objects (VAOs)
    GLuint _led_vbo = 0;
    GLuint _led_vao = 0;
    GLuint _mesh_vbo = 0;
    GLuint _mesh_ibo = 0; // Index Buffer Object
    GLuint _mesh_vao = 0;
    size_t _mesh_index_count = 0;

    // Framebuffer for post-processing
    GLuint _framebuffer = 0;
    GLuint _render_texture = 0;
    GLuint _depth_renderbuffer = 0;
    GLuint _quad_vao = 0;
    GLuint _quad_vbo = 0;

    // LED data
    CRGB* _leds = nullptr;
    uint16_t _num_leds = 0;
    std::vector<Point> _led_positions; // Store pre-transformed positions
    
    // Platform settings
    uint8_t _brightness = DEFAULT_BRIGHTNESS;
    float _led_size = DEFAULT_LED_SIZE;
    float _atmosphere_intensity = DEFAULT_ATMOSPHERE_INTENSITY;
    float _led_spacing = DEFAULT_LED_SPACING;
    bool _show_mesh = true;
    float _mesh_opacity = 0.3f;
    bool _show_wireframe = false;

    // Auto-rotation state
    bool _auto_rotation = false;
    float _auto_rotation_speed = DEFAULT_AUTO_ROTATION_SPEED;
    double _last_auto_rotation_time = 0.0;
    
    // Mouse interaction state
    bool _is_dragging = false;
    int _last_mouse_x = 0;
    int _last_mouse_y = 0;
    bool _shift_key_down = false;
    
    // Canvas dimensions
    int _canvas_width = 800;
    int _canvas_height = 600;
    
    // Performance tracking
    uint8_t _max_refresh_rate = 60;
    uint8_t _dither = 1; // Assuming default dither is ON
    double _last_frame_time = 0.0;
    unsigned int _frame_count = 0;
    
    double _last_delta_time = 0.0; // ADDED for deltaTime calculation

#endif // End of Web-Specific Private Section
};

} // namespace PixelTheater