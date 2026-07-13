#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <cstdint>

static Model* pModelInstance = nullptr;

Model::Model() : modelListener(0)
{
    pModelInstance = this;
}

void Model::tick()
{
    // Để trống: Các cờ sẽ được Screen2View chủ động kéo về thông qua Presenter
}

extern "C" void Set_Button_Flag(uint8_t btn_id)
{
    if (pModelInstance != nullptr)
    {
        switch (btn_id)
        {
            case 1: pModelInstance->btnLeftPressed = true;   break;
            case 2: pModelInstance->btnRightPressed = true;  break;
            case 3: pModelInstance->btnRotatePressed = true; break;
            case 4: pModelInstance->btnDownPressed = true;   break;
            default: break;
        }
    }
}
