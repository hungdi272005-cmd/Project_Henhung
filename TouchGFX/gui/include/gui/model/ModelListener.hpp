#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

class Model;

class ModelListener
{
public:
    ModelListener() : model(0) {}
    virtual ~ModelListener() {}

    virtual void movePieceLeft() {}
    virtual void movePieceRight() {}
    virtual void rotatePiece() {}
    virtual void dropPieceSpeedUp() {}
    virtual void dropPieceNormalSpeed() {}

    void bind(Model* m) { model = m; }
protected:
    Model* model;
};
#endif // MODELLISTENER_HPP
