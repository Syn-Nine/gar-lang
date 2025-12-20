#include "VM.h"
#include "Environment.h"

#include <raylib.h>
#include <stdint.h>
#include <string>


void VM::LoadRaylib()
{
    
    // register raylib library functions
    RegisterFunc("%ray::BeginDrawing", 0, [this]() { BeginDrawing(); } );

    RegisterFunc("%ray::ClearBackground", 1, [this]() {
        int32_t in0 = PopParamInt();
        ClearBackground(StringToColor(m_env->GetEnumAsString(in0)));
        });

    RegisterFunc("%ray::CloseWindow", 0, [this]() { CloseWindow(); });
    
    RegisterFunc("%ray::DrawText", 1, [this]() {
        int32_t in4 = PopParamInt();
        int32_t in3 = PopParamInt();
        int32_t in2 = PopParamInt();
        int32_t in1 = PopParamInt();
        std::string in0 = PopParamString();
        DrawText(in0.c_str(), in1, in2, in3, StringToColor(m_env->GetEnumAsString(in4)));
        });

    RegisterFunc("%ray::DrawCircle", 1, [this]() {
        int32_t in3 = PopParamInt();
        float in2 = PopParamFloat();
        int32_t in1 = PopParamInt();
        int32_t in0 = PopParamInt();
        DrawCircle(in0, in1, in2, StringToColor(m_env->GetEnumAsString(in3)));
        });

    RegisterFunc("%ray::DrawTriangle", 1, [this]() {
        int32_t in3 = PopParamInt();
        double in2b = PopParamFloat();
        double in2a = PopParamFloat();
        double in1b = PopParamFloat();
        double in1a = PopParamFloat();
        double in0b = PopParamFloat();
        double in0a = PopParamFloat();
        DrawTriangle({ float(in0a), float(in0b) }, { float(in1a), float(in1b) }, { float(in2a), float(in2b) }, StringToColor(m_env->GetEnumAsString(in3)));
        });

    RegisterFunc("%ray::DrawRectangle", 1, [this]() {
        int32_t in4 = PopParamInt();
        int32_t in3 = PopParamInt();
        int32_t in2 = PopParamInt();
        int32_t in1 = PopParamInt();
        int32_t in0 = PopParamInt();
        DrawRectangle(in0, in1, in2, in3, StringToColor(m_env->GetEnumAsString(in4)));
        });

    RegisterFunc("%ray::DrawTexturePro", 13, [this]() {
        int32_t in5 = PopParamInt();
        float in4 = PopParamFloat();
        float in3b = PopParamFloat();
        float in3a = PopParamFloat();
        float in2d = PopParamFloat();
        float in2c = PopParamFloat();
        float in2b = PopParamFloat();
        float in2a = PopParamFloat();
        float in1d = PopParamFloat();
        float in1c = PopParamFloat();
        float in1b = PopParamFloat();
        float in1a = PopParamFloat(); 
        int32_t in0 = PopParamRay(PARAM_RAY_TEXTURE);
        Texture2D* tex = m_ray_textures[in0];
        DrawTexturePro(*tex, { in1a, in1b, in1c, in1d }, { in2a, in2b, in2c, in2d }, { in3a, in3b }, in4, StringToColor(m_env->GetEnumAsString(in5)));
        });

    RegisterFunc("%ray::DrawTextureProRGBA", 16, [this]() {
        uint8_t in5a = PopParamInt();
        uint8_t in5b = PopParamInt();
        uint8_t in5g = PopParamInt();
        uint8_t in5r = PopParamInt();
        float in4 = PopParamFloat();
        float in3b = PopParamFloat();
        float in3a = PopParamFloat();
        float in2d = PopParamFloat();
        float in2c = PopParamFloat();
        float in2b = PopParamFloat();
        float in2a = PopParamFloat();
        float in1d = PopParamFloat();
        float in1c = PopParamFloat();
        float in1b = PopParamFloat();
        float in1a = PopParamFloat();
        int32_t in0 = PopParamRay(PARAM_RAY_TEXTURE);
        Texture2D* tex = m_ray_textures[in0];
        DrawTexturePro(*tex, { in1a, in1b, in1c, in1d }, { in2a, in2b, in2c, in2d }, { in3a, in3b }, in4, { in5r, in5g, in5b, in5a });
        });

    RegisterFunc("%ray::EndDrawing", 0, [this]() { EndDrawing(); });

    
    RegisterFunc("%ray::GetMouseX", 0, [this]() {
        int32_t val = GetMouseX();
        PushParamInt(val);
        });

    RegisterFunc("%ray::GetMouseY", 0, [this]() {
        int val = GetMouseY();
        PushParamInt(val);
        });
    
    RegisterFunc("%ray::GetTime", 0, [this]() {
        double val = GetTime();
        PushParamFloat(val);
        });

    RegisterFunc("%ray::InitWindow", 3, [this]() {
        std::string title = PopParamString();
        int yres = PopParamInt();
        int xres = PopParamInt();
        InitWindow(xres, yres, title.c_str());
        });

    RegisterFunc("%ray::IsMouseButtonReleased", 1, [this]() {
        int32_t in0 = PopParamInt();
        bool val = IsMouseButtonReleased(StringToKey(m_env->GetEnumAsString(in0)));
        PushParamBool(val);
        });

    RegisterFunc("%ray::LoadTexture", 1, [this]() {
        std::string filename = PopParamString();
        Texture2D* tex = new Texture2D(LoadTexture(filename.c_str()));
        int idx = m_ray_textures.size();
        m_ray_textures.push_back(tex);
        PushParamRay(PARAM_RAY_TEXTURE, idx);
        });

    RegisterFunc("%ray::SetTargetFPS", 1, [this]() {
        int fps = PopParamInt();
        SetTargetFPS(fps);
        });
    
    RegisterFunc("%ray::WindowShouldClose", 0, [this]() {
        bool val = WindowShouldClose();
        PushParamBool(val);
        });


    /*RegisterFunc("%input", 0, [this]() {
        std::string str = console_input();
        int addr = NewScratchString(str);
        PushParamString(addr);
        }
    );

    RegisterFunc("%new", 0, [this]() {
        int type = PeekParamType();
        if (type == PARAM_LIST)
        {
            int addr = HeapNewList(PopParamPointer());
            PushParamList(addr);
        }
        else if (type == PARAM_STRING)
        {
            int addr = HeapNewString(PopParamPointer());
            PushParamString(addr);
        }
        }
    );*/

}


Color VM::StringToColor(const std::string& s)
{
    static bool first = true;
    static std::map<std::string, Color> enumMap;
    if (first)
    {
        first = false;
        enumMap.insert(std::make_pair(":LIGHTGRAY", LIGHTGRAY));
        enumMap.insert(std::make_pair(":GRAY", GRAY));
        enumMap.insert(std::make_pair(":DARKGRAY", Color{ 60, 60, 60, 255 }));
        enumMap.insert(std::make_pair(":MEDDARKGRAY", Color{ 40, 40, 40, 255 }));
        enumMap.insert(std::make_pair(":DARKDARKGRAY", Color{ 20, 20, 20, 255 }));
        enumMap.insert(std::make_pair(":YELLOW", YELLOW));
        enumMap.insert(std::make_pair(":GOLD", GOLD));
        enumMap.insert(std::make_pair(":ORANGE", ORANGE));
        enumMap.insert(std::make_pair(":PINK", PINK));
        enumMap.insert(std::make_pair(":RED", RED));
        enumMap.insert(std::make_pair(":DARKRED", Color{ 128, 20, 25, 255 }));
        enumMap.insert(std::make_pair(":DARKRED50", Color{ 128, 20, 25, 128 }));
        enumMap.insert(std::make_pair(":MAROON", MAROON));
        enumMap.insert(std::make_pair(":GREEN", GREEN));
        enumMap.insert(std::make_pair(":LIME", LIME));
        enumMap.insert(std::make_pair(":DARKGREEN", DARKGREEN));
        enumMap.insert(std::make_pair(":SKYBLUE", SKYBLUE));
        enumMap.insert(std::make_pair(":BLUE", BLUE));
        enumMap.insert(std::make_pair(":DARKBLUE", DARKBLUE));
        enumMap.insert(std::make_pair(":PURPLE", PURPLE));
        enumMap.insert(std::make_pair(":VIOLET", VIOLET));
        enumMap.insert(std::make_pair(":DARKPURPLE", DARKPURPLE));
        enumMap.insert(std::make_pair(":BEIGE", BEIGE));
        enumMap.insert(std::make_pair(":BROWN", BROWN));
        enumMap.insert(std::make_pair(":DARKBROWN", DARKBROWN));
        enumMap.insert(std::make_pair(":WHITE", WHITE));
        enumMap.insert(std::make_pair(":WHITE50", Color{ 255, 255, 255, 128 }));
        enumMap.insert(std::make_pair(":BLACK", BLACK));
        enumMap.insert(std::make_pair(":BLACK50", Color{ 0, 0, 0, 128 }));
        enumMap.insert(std::make_pair(":BLANK", BLANK));
        enumMap.insert(std::make_pair(":MAGENTA", MAGENTA));
        enumMap.insert(std::make_pair(":RAYWHITE", RAYWHITE));
        enumMap.insert(std::make_pair(":CYAN", Color{ 0, 224, 224, 255 }));
        enumMap.insert(std::make_pair(":TEAL", Color{ 0, 128, 128, 255 }));
        enumMap.insert(std::make_pair(":DARKTEAL", Color{ 0, 32, 32, 255 }));
        enumMap.insert(std::make_pair(":SHARKGRAY", Color{ 34, 32, 39, 255 }));
        enumMap.insert(std::make_pair(":SLATEGRAY", Color{ 140, 173, 181, 255 }));
        enumMap.insert(std::make_pair(":DARKSLATEGRAY", Color{ 67, 99, 107, 255 }));
        enumMap.insert(std::make_pair(":BITTERSWEET", Color{ 254, 111, 94, 255 }));
    }
    if (0 != enumMap.count(s)) return enumMap.at(s);
    return MAROON;
}

int VM::StringToKey(const std::string& s)
{
    static bool first = true;
    static std::map<std::string, int> enumMap;
    if (first)
    {
        first = false;
        enumMap.insert(std::make_pair(":KEY_NULL", KEY_NULL));
        enumMap.insert(std::make_pair(":KEY_ESCAPE", KEY_ESCAPE));
        enumMap.insert(std::make_pair(":KEY_UP", KEY_UP));
        enumMap.insert(std::make_pair(":KEY_DOWN", KEY_DOWN));
        enumMap.insert(std::make_pair(":KEY_LEFT", KEY_LEFT));
        enumMap.insert(std::make_pair(":KEY_RIGHT", KEY_RIGHT));
        enumMap.insert(std::make_pair(":KEY_SPACE", KEY_SPACE));
        enumMap.insert(std::make_pair(":KEY_ENTER", KEY_ENTER));
        enumMap.insert(std::make_pair(":KEY_BACKSPACE", KEY_BACKSPACE));
        enumMap.insert(std::make_pair(":KEY_TAB", KEY_TAB));
        enumMap.insert(std::make_pair(":KEY_HOME", KEY_HOME));
        enumMap.insert(std::make_pair(":KEY_END", KEY_END));
        enumMap.insert(std::make_pair(":KEY_PAGE_UP", KEY_PAGE_UP));
        enumMap.insert(std::make_pair(":KEY_PAGE_DOWN", KEY_PAGE_DOWN));
        enumMap.insert(std::make_pair(":KEY_DELETE", KEY_DELETE));
        enumMap.insert(std::make_pair(":KEY_PERIOD", KEY_PERIOD));
        enumMap.insert(std::make_pair(":KEY_MINUS", KEY_MINUS));
        //
        enumMap.insert(std::make_pair(":KEY_A", KEY_A));
        enumMap.insert(std::make_pair(":KEY_B", KEY_B));
        enumMap.insert(std::make_pair(":KEY_C", KEY_C));
        enumMap.insert(std::make_pair(":KEY_D", KEY_D));
        enumMap.insert(std::make_pair(":KEY_E", KEY_E));
        enumMap.insert(std::make_pair(":KEY_F", KEY_F));
        enumMap.insert(std::make_pair(":KEY_G", KEY_G));
        enumMap.insert(std::make_pair(":KEY_H", KEY_H));
        enumMap.insert(std::make_pair(":KEY_I", KEY_I));
        enumMap.insert(std::make_pair(":KEY_J", KEY_J));
        enumMap.insert(std::make_pair(":KEY_K", KEY_K));
        enumMap.insert(std::make_pair(":KEY_L", KEY_L));
        enumMap.insert(std::make_pair(":KEY_M", KEY_M));
        enumMap.insert(std::make_pair(":KEY_N", KEY_N));
        enumMap.insert(std::make_pair(":KEY_O", KEY_O));
        enumMap.insert(std::make_pair(":KEY_P", KEY_P));
        enumMap.insert(std::make_pair(":KEY_Q", KEY_Q));
        enumMap.insert(std::make_pair(":KEY_R", KEY_R));
        enumMap.insert(std::make_pair(":KEY_S", KEY_S));
        enumMap.insert(std::make_pair(":KEY_T", KEY_T));
        enumMap.insert(std::make_pair(":KEY_U", KEY_U));
        enumMap.insert(std::make_pair(":KEY_V", KEY_V));
        enumMap.insert(std::make_pair(":KEY_W", KEY_W));
        enumMap.insert(std::make_pair(":KEY_X", KEY_X));
        enumMap.insert(std::make_pair(":KEY_Y", KEY_Y));
        enumMap.insert(std::make_pair(":KEY_Z", KEY_Z));
        enumMap.insert(std::make_pair(":KEY_APOSTROPHE", KEY_APOSTROPHE));
        enumMap.insert(std::make_pair(":KEY_SLASH", KEY_SLASH));
        //
        enumMap.insert(std::make_pair(":KEY_F1", KEY_F1));
        enumMap.insert(std::make_pair(":KEY_F2", KEY_F2));
        enumMap.insert(std::make_pair(":KEY_F3", KEY_F3));
        enumMap.insert(std::make_pair(":KEY_F4", KEY_F4));
        enumMap.insert(std::make_pair(":KEY_F5", KEY_F5));
        enumMap.insert(std::make_pair(":KEY_F6", KEY_F6));
        enumMap.insert(std::make_pair(":KEY_F7", KEY_F7));
        enumMap.insert(std::make_pair(":KEY_F8", KEY_F8));
        enumMap.insert(std::make_pair(":KEY_F9", KEY_F9));
        enumMap.insert(std::make_pair(":KEY_F10", KEY_F10));
        enumMap.insert(std::make_pair(":KEY_F11", KEY_F11));
        enumMap.insert(std::make_pair(":KEY_F12", KEY_F12));
        //
        enumMap.insert(std::make_pair(":KEY_ZERO", KEY_ZERO));
        enumMap.insert(std::make_pair(":KEY_ONE", KEY_ONE));
        enumMap.insert(std::make_pair(":KEY_TWO", KEY_TWO));
        enumMap.insert(std::make_pair(":KEY_THREE", KEY_THREE));
        enumMap.insert(std::make_pair(":KEY_FOUR", KEY_FOUR));
        enumMap.insert(std::make_pair(":KEY_FIVE", KEY_FIVE));
        enumMap.insert(std::make_pair(":KEY_SIX", KEY_SIX));
        enumMap.insert(std::make_pair(":KEY_SEVEN", KEY_SEVEN));
        enumMap.insert(std::make_pair(":KEY_EIGHT", KEY_EIGHT));
        enumMap.insert(std::make_pair(":KEY_NINE", KEY_NINE));

        enumMap.insert(std::make_pair(":KEY_KP_0", KEY_KP_0));
        enumMap.insert(std::make_pair(":KEY_KP_1", KEY_KP_1));
        enumMap.insert(std::make_pair(":KEY_KP_2", KEY_KP_2));
        enumMap.insert(std::make_pair(":KEY_KP_3", KEY_KP_3));
        enumMap.insert(std::make_pair(":KEY_KP_4", KEY_KP_4));
        enumMap.insert(std::make_pair(":KEY_KP_5", KEY_KP_5));
        enumMap.insert(std::make_pair(":KEY_KP_6", KEY_KP_6));
        enumMap.insert(std::make_pair(":KEY_KP_7", KEY_KP_7));
        enumMap.insert(std::make_pair(":KEY_KP_8", KEY_KP_8));
        enumMap.insert(std::make_pair(":KEY_KP_9", KEY_KP_9));
        enumMap.insert(std::make_pair(":KEY_KP_DECIMAL", KEY_KP_DECIMAL));
        enumMap.insert(std::make_pair(":KEY_KP_DIVIDE", KEY_KP_DIVIDE));
        enumMap.insert(std::make_pair(":KEY_KP_MULTIPLY", KEY_KP_MULTIPLY));
        enumMap.insert(std::make_pair(":KEY_KP_SUBTRACT", KEY_KP_SUBTRACT));
        enumMap.insert(std::make_pair(":KEY_KP_ADD", KEY_KP_ADD));
        enumMap.insert(std::make_pair(":KEY_KP_ENTER", KEY_KP_ENTER));
        enumMap.insert(std::make_pair(":KEY_KP_EQUAL", KEY_KP_EQUAL));

        //
        enumMap.insert(std::make_pair(":KEY_LEFT_SHIFT", KEY_LEFT_SHIFT));
        enumMap.insert(std::make_pair(":KEY_LEFT_CONTROL", KEY_LEFT_CONTROL));
        enumMap.insert(std::make_pair(":KEY_LEFT_ALT", KEY_LEFT_ALT));
        enumMap.insert(std::make_pair(":KEY_LEFT_SUPER", KEY_LEFT_SUPER));
        enumMap.insert(std::make_pair(":KEY_RIGHT_SHIFT", KEY_RIGHT_SHIFT));
        enumMap.insert(std::make_pair(":KEY_RIGHT_CONTROL", KEY_RIGHT_CONTROL));
        enumMap.insert(std::make_pair(":KEY_RIGHT_ALT", KEY_RIGHT_ALT));
        enumMap.insert(std::make_pair(":KEY_RIGHT_SUPER", KEY_RIGHT_SUPER));
        //
        enumMap.insert(std::make_pair(":MOUSE_BUTTON_LEFT", MOUSE_BUTTON_LEFT));
        enumMap.insert(std::make_pair(":MOUSE_BUTTON_RIGHT", MOUSE_BUTTON_RIGHT));
        enumMap.insert(std::make_pair(":MOUSE_BUTTON_MIDDLE", MOUSE_BUTTON_MIDDLE));

    }
    if (0 != enumMap.count(s)) return enumMap.at(s);
    return KEY_NULL;
}