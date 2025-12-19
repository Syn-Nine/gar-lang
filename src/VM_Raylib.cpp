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