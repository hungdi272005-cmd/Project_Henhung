#ifndef MODEL_HPP
#define MODEL_HPP

class ModelListener;

class Model
{
public:
    Model();
    // Các biến cờ hiệu báo nút bấm được nhấn
	bool btnLeftPressed = false;
	bool btnRightPressed = false;
	bool btnRotatePressed = false;
	bool btnDownPressed = false;

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

protected:
    ModelListener* modelListener;
};

#endif // MODEL_HPP
