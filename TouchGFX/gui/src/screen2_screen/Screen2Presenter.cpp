#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

Screen2Presenter::Screen2Presenter(Screen2View& v)
    : view(v)
{
}

void Screen2Presenter::activate() {}
void Screen2Presenter::deactivate() {}

void Screen2Presenter::movePieceLeft() { view.moveLeftPressed(); }
void Screen2Presenter::movePieceRight() { view.moveRightPressed(); }
void Screen2Presenter::rotatePiece() { view.rotatePiecePressed(); }
void Screen2Presenter::dropPieceSpeedUp() { view.moveDownPressed(); }
void Screen2Presenter::dropPieceNormalSpeed() { view.moveDownReleased(); }

bool Screen2Presenter::isLeftPressed()
{
    return (model != 0) ? model->btnLeftPressed : false;
}
void Screen2Presenter::clearLeftFlag()
{
    if (model != 0) model->btnLeftPressed = false;
}

bool Screen2Presenter::isRightPressed()
{
    return (model != 0) ? model->btnRightPressed : false;
}
void Screen2Presenter::clearRightFlag()
{
    if (model != 0) model->btnRightPressed = false;
}

bool Screen2Presenter::isRotatePressed()
{
    return (model != 0) ? model->btnRotatePressed : false;
}
void Screen2Presenter::clearRotateFlag()
{
    if (model != 0) model->btnRotatePressed = false;
}

bool Screen2Presenter::isDownPressed()
{
    return (model != 0) ? model->btnDownPressed : false;
}
void Screen2Presenter::clearDownFlag()
{
    if (model != 0) model->btnDownPressed = false;
}
