#ifndef SCREEN2PRESENTER_HPP
#define SCREEN2PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen2View;

class Screen2Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen2Presenter(Screen2View& v);

    virtual void activate();
    virtual void deactivate();
    virtual ~Screen2Presenter() {}

    virtual void movePieceLeft() override;
    virtual void movePieceRight() override;
    virtual void rotatePiece() override;
    virtual void dropPieceSpeedUp() override;
    virtual void dropPieceNormalSpeed() override;

    // Các hàm kiểm tra cờ chủ động
    bool isLeftPressed();
    void clearLeftFlag();
    bool isRightPressed();
    void clearRightFlag();
    bool isRotatePressed();
    void clearRotateFlag();
    bool isDownPressed();
    void clearDownFlag();

private:
    Screen2Presenter();
    Screen2View& view;
};

#endif // SCREEN2PRESENTER_HPP
